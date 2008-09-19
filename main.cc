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

#include "gl/gl.hh"
#include "gl/glu.hh"
#include "gl/glut.hh"

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
string_stroke(void *font, const char *string)
{
	for(const char *c = string; *c; ++c)
		glutStrokeCharacter(font, *c);
}

static unsigned int
string_stroke_width(void *font, const char *string)
{
	int sum = 0;

	for(const char *c = string; *c; ++c)
		sum += glutStrokeWidth(font, *c);

	return sum;
}

static void
display()
{
	static unsigned int frame = 0;
	static int time_prev = 0;
	static int time_prev_fps = 0;

	static char fps[16] = "FPS: 0";

	int time = glutGet(GLUT_ELAPSED_TIME);
	if(time - time_prev_fps >= 250) {
		snprintf(fps, sizeof(fps),
			"FPS: %d", 1000 * frame / (time - time_prev_fps));

		frame = 0;
		time_prev_fps = time;
	}

	frame++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	my_scene->draw();

	/* Display FPS counter */
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(1, 1, 1);

	glPushMatrix();
	glTranslatef(0, 2, -1);
	glScalef(1e-2, 1e-2, 1e-2);
	glTranslatef(
		-0.5 * string_stroke_width(GLUT_STROKE_ROMAN, fps),
		0, 0);
	string_stroke(GLUT_STROKE_ROMAN, fps);
	glPopMatrix();

	glutSwapBuffers();

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

static void
idle()
{
	static int time_prev = 0;

	int time = glutGet(GLUT_ELAPSED_TIME);
	if(100 * (time - time_prev) < 1000)
		return;

	time_prev = time;

	glutPostRedisplay();
}

static void
reshape(int width, int height)
{
	if(width == 0 || height == 0)
		return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, width, height);
	gluPerspective(45, (float) width / height, 1, 48);
}

static void
keyboard(unsigned char key, int x, int y)
{
	switch(key) {
	case 27:
		exit(0);
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
	default:
		printf("key %d\n", key);
	}
}

static void
keyboardUp(unsigned char key, int x, int y)
{
}

static bool special_pressed[256] = {};

static void
special(int key, int x, int y)
{
	if(key < 0 || key > 255) {
		std::cerr << "warning: Unhandled special key "
			<< key << std::endl;
		return;
	}

	if(special_pressed[key]) {
		std::cerr << "warning: Received double key-press for special "
			"key " << key << std::endl;
		return;
	}

	special_pressed[key] = true;

	switch(key) {
	case GLUT_KEY_DOWN:
		my_character->walk_forwards();
		break;
	case GLUT_KEY_UP:
		my_character->walk_backwards();
		break;
	case GLUT_KEY_LEFT:
		my_character->walk_left();
		break;
	case GLUT_KEY_RIGHT:
		my_character->walk_right();
		break;
	case GLUT_KEY_F1: {
		static bool tracking = false;
		tracking = !tracking;
		my_character->set_tracking(tracking);
		break;
	}
	case GLUT_KEY_F12:
		capture();
		break;
	default:
		printf("special %d\n", key);
	}
}

static void
specialUp(int key, int x, int y)
{
	if(key < 0 || key > 255) {
		std::cerr << "warning: Unhandled special key "
			<< key << std::endl;
		return;
	}

	if(!special_pressed[key]) {
		std::cerr << "warning: Received phantom key-release for "
			"special key " << key << std::endl;
		return;
	}

	special_pressed[key] = false;

	switch(key) {
	case GLUT_KEY_DOWN:
		my_character->stop_forwards();
		break;
	case GLUT_KEY_UP:
		my_character->stop_backwards();
		break;
	case GLUT_KEY_LEFT:
		my_character->stop_left();
		break;
	case GLUT_KEY_RIGHT:
		my_character->stop_right();
		break;
	default:
		printf("special up %d\n", key);
	}
}

static void
mouse(int button, int state, int x, int y)
{
}

static void
motion(int x, int y)
{
}

static void
passiveMotion(int x, int y)
{
}

static void
entry(int state)
{
}

int
main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);

	int window = glutCreateWindow("Scribe");

	glutIgnoreKeyRepeat(1);

	glutDisplayFunc(&display);
	glutIdleFunc(&idle);
	glutReshapeFunc(&reshape);
	glutKeyboardFunc(&keyboard);
	glutKeyboardUpFunc(&keyboardUp);
	glutSpecialFunc(&special);
	glutSpecialUpFunc(&specialUp);
	glutMouseFunc(&mouse);
	glutMotionFunc(&motion);
	glutPassiveMotionFunc(&passiveMotion);
	glutEntryFunc(&entry);

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

	glutMainLoop();

	glutDestroyWindow(window);

	deinit();
	return 0;
}
