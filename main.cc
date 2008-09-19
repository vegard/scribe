extern "C" {
#include <stdint.h>
}

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

extern "C" {
#include <png.h>
}

extern "C" {
#include <SDL.h>
#include <SDL_opengl.h>
}

#include "camera.hh"
#include "character.hh"
#include "object.hh"
#include "scene.hh"
#include "spring_camera.hh"
#include "vector.hh"

static int camera_zoom = 8;

static const vector& get_camera_distance()
{
	static const double camera_angle = 20.0 * M_PI / 180;
	static vector v;

	v = vector(0,
		camera_zoom * sin(camera_angle),
		camera_zoom * cos(camera_angle));
	return v;
}

static character* my_character;
static background* my_background;

static spring_camera* my_camera;
static scene* my_scene;

static void
init()
{
	my_character = new character();
	my_background = new background();

	my_camera = new spring_camera(*my_character,
		get_camera_distance(), vector(0, 1, 0), 1e-5, 2e-3);
	my_scene = new scene(*my_camera, *my_background);

	my_scene->_objects.push_back(my_character);
}

static void
deinit()
{
	delete my_scene;
	delete my_camera;
	delete my_background;
	delete my_character;
}

static void
capture()
{
	static const unsigned int width = 640;
	static const unsigned int height = 480;

	static unsigned int frame = 0;
	frame++;

	printf("\e[Lframe %d\r", frame);
	fflush(stdout);

	static unsigned short capture[width * height * 3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_SHORT, capture);

	static char fn[32];
	snprintf(fn, sizeof(fn), "png/%04d.png", frame - 1);

	FILE *fp = fopen(fn, "wb");
	if(!fp) {
		fprintf(stderr, "error: fopen\n");
		exit(1);
	}

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	if(!png_ptr) {
		fprintf(stderr, "error: png_create_write_struct\n");
		exit(1);
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		fprintf(stderr, "error: png_create_info_struct\n");
		exit(1);
	}

	png_init_io(png_ptr, fp);
	png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
	png_set_IHDR(png_ptr, info_ptr, width, height, 16, PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

	png_bytep rows[height];

	for(unsigned int i = 0; i < height; i++)
		rows[i] = (png_bytep) &capture[(height - i - 1) * width * 3];

	png_set_rows(png_ptr, info_ptr, rows);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	fclose(fp);
}

static void
display()
{
	static unsigned int frame = 0;
	static Uint32 time_prev = 0;
	static Uint32 time_prev_fps = 0;

	static char fps[16] = "FPS: 0";

	Uint32 time = SDL_GetTicks();
	if(time - time_prev_fps >= 250) {
		snprintf(fps, sizeof(fps),
			"FPS: %d", 1000 * frame / (time - time_prev_fps));

		frame = 0;
		time_prev_fps = time;
	}

	frame++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	my_scene->draw();
	SDL_GL_SwapBuffers();

#ifdef CONFIG_CAPTURE
	/* If we're capturing, we want a constant frame rate for the
	 * captured data, not necessarily for what's displayed on the
	 * screen. */
	my_scene->update(25);
	capture();
#else
	/* Limit the frame rate */
	int delta = time - time_prev;
	if (delta < 10)
		delta = 10;
	if (delta > 100)
		delta = 100;
	my_scene->update(delta);
#endif

	time_prev = time;
}

static Uint32
displayTimer(Uint32 interval, void* param)
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	SDL_PushEvent(&event);
	return interval;
}

static void
resize(int width, int height)
{
	if(width == 0 || height == 0)
		return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, width, height);
	gluPerspective(45, (float) width / height, 1, 48);
}

static void
keyboard(SDL_KeyboardEvent* key)
{
	switch(key->keysym.sym) {
	case SDLK_DOWN:
		my_character->walk_forwards();
		break;
	case SDLK_UP:
		my_character->walk_backwards();
		break;
	case SDLK_LEFT:
		my_character->walk_left();
		break;
	case SDLK_RIGHT:
		my_character->walk_right();
		break;
	case SDLK_F1: {
			static bool tracking = false;
			tracking = !tracking;
			my_character->set_tracking(tracking);
		}
		break;
	case SDLK_ESCAPE: {
			SDL_Event event;
			event.type = SDL_QUIT;
			SDL_PushEvent(&event);
		}
		break;
	case ' ':
		my_character->jump();
		break;
	case '-':
		if(camera_zoom >= 16)
			break;

		camera_zoom++;
		my_camera->set_distance(get_camera_distance());
		break;
	case '+':
		if(camera_zoom <= 4)
			break;

		camera_zoom--;
		my_camera->set_distance(get_camera_distance());
		break;
	case SDLK_F12:
		capture();
		break;
	default:
		printf("key %d\n", key->keysym.sym);
	}
}

static void
keyboardUp(SDL_KeyboardEvent* key)
{
	switch(key->keysym.sym) {
	case SDLK_DOWN:
		my_character->stop_forwards();
		break;
	case SDLK_UP:
		my_character->stop_backwards();
		break;
	case SDLK_LEFT:
		my_character->stop_left();
		break;
	case SDLK_RIGHT:
		my_character->stop_right();
		break;
	default:
		break;
	}
}

int
main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) {
		fprintf(stderr, "error: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_Surface* surface = SDL_SetVideoMode(640, 480, 0, SDL_OPENGL);
	if (!surface) {
		fprintf(stderr, "error: %s\n", SDL_GetError());
		exit(1);
	}

	init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_POLYGON_OFFSET_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glClearColor(0.40, 0.55, 0.0, 0.0);
	static const GLfloat fog_color[4] = {0.54, 0.44, 0.28, 0};
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_DENSITY, 1);
	glFogf(GL_FOG_START, 12);
	glFogf(GL_FOG_END, 30);
	glFogfv(GL_FOG_COLOR, fog_color);
	glEnable(GL_FOG);

	glShadeModel(GL_SMOOTH);

	resize(640, 480);

	/* 25 FPS */
	SDL_TimerID display_timer = SDL_AddTimer(1000 / 100,
		&displayTimer, NULL);
	if (!display_timer) {
		fprintf(stderr, "error: %s\n", SDL_GetError());
		exit(1);
	}

	bool running = true;
	SDL_Event event;
	while (running) {
		SDL_WaitEvent(&event);

		switch (event.type) {
		case SDL_USEREVENT:
			display();
			break;
		case SDL_KEYDOWN:
			keyboard(&event.key);
			break;
		case SDL_KEYUP:
			keyboardUp(&event.key);
			break;
		case SDL_MOUSEBUTTONDOWN:
			break;
		case SDL_QUIT:
			running = false;
			break;
		case SDL_VIDEORESIZE:
			resize(event.resize.w, event.resize.h);
			break;
		}
	}

	deinit();

	SDL_Quit();
	return 0;
}
