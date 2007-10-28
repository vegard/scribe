extern "C" {
#include <stdint.h>
}

#include <cmath>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include <png.h>
}

extern "C" {
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
}

#include "camera.hh"
#include "character.hh"
#include "object.hh"
#include "scene.hh"
#include "spring_camera.hh"
#include "vector.hh"

GLuint
texture_load_png(const char *fn)
{
	FILE *fp = fopen(fn, "rb");
	if(!fp)
		exit(1);

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	if(!png_ptr)
		exit(1);

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
		exit(1);

	png_init_io(png_ptr, fp);

	uint8_t *image = new uint8_t[4 * 16 * 16];
	png_bytep row_pointers[16];

	for(int i = 0; i < 16; i++)
		row_pointers[i] = &image[4 * 16 * i];

	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	if(png_ptr->color_type == PNG_COLOR_TYPE_RGB) {
		fprintf(stderr, "error: %s: rgb not implemented\n", fn);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 16, 16,
			GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, image);
	} else if(png_ptr->color_type == PNG_COLOR_TYPE_RGBA) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 16, 16,
			GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, image);
	} else {
		fprintf(stderr, "error: %s: unhandled png type\n", fn);
		exit(1);
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(fp);

	return texture;
}

static character protagonist;
static background background;

static spring_camera c(protagonist, vector(0, 2, 8), 1e-5, 1e-2);
static scene s(c, background);

static void
init()
{
	protagonist.load_textures();
	background.load_textures();

	s._objects.push_back(&protagonist);
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

	char *fn;
	asprintf(&fn, "png/%04d.png", frame - 1);

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

	free(fn);
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

	static char fps[10] = "0";

	int time = glutGet(GLUT_ELAPSED_TIME);
	int delta_time = time > time_prev
		? time - time_prev : time_prev - time;
	if(delta_time > 1000) {
		snprintf(fps, sizeof(fps),
			"FPS: %d", 1000 * frame / delta_time);

		frame = 0;
		time_prev = time;
	}

	frame++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	s.draw();

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

	if(0)
		capture();

	s.update();
}

static void
reshape(int width, int height)
{
	if(width == 0 || height == 0)
		return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, width, height);
	gluPerspective(45, (float) width / height, 1, 64);
}

static void
keyboard(unsigned char key, int x, int y)
{
	switch(key) {
	case 27:
		exit(0);
	default:
		printf("key %d\n", key);
	}
}

static void
keyboardUp(unsigned char key, int x, int y)
{
}

static int key_left = 0;
static int key_up = 0;
static int key_right = 0;
static int key_down = 0;

static void
special(int key, int x, int y)
{
	switch(key) {
	case GLUT_KEY_DOWN:
		if(key_down)
			break;
		key_down = 1;

		protagonist.walk_forwards();
		break;
	case GLUT_KEY_UP:
		if(key_up)
			break;
		key_up = 1;

		protagonist.walk_backwards();
		break;
	case GLUT_KEY_LEFT:
		if(key_left)
			break;
		key_left = 1;

		protagonist.walk_left();
		break;
	case GLUT_KEY_RIGHT:
		if(key_right)
			break;
		key_right = 1;

		protagonist.walk_right();
		break;
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
	switch(key) {
	case GLUT_KEY_DOWN:
		if(!key_down)
			break;
		key_down = 0;

		protagonist.stop_forwards();
		break;
	case GLUT_KEY_UP:
		if(!key_up)
			break;
		key_up = 0;

		protagonist.stop_backwards();
		break;
	case GLUT_KEY_LEFT:
		if(!key_left)
			break;
		key_left = 0;

		protagonist.stop_left();
		break;
	case GLUT_KEY_RIGHT:
		if(!key_right)
			break;
		key_right = 0;

		protagonist.stop_right();
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

	int window = glutCreateWindow("Link");

	glutIgnoreKeyRepeat(1);

	glutDisplayFunc(&display);
	glutIdleFunc(&display);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	static const GLfloat fog_color[4] = {0.2, 0, 0, 0};
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
