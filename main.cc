extern "C" {
#include <stdint.h>
}

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

extern "C" {
#include <png.h>
}

extern "C" {
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
}

static GLuint
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

GLuint link_up[2];
GLuint link_down[2];
GLuint link_left[2];
GLuint link_right[2];
GLuint desert[2];
GLuint desert_wall_bottom;
GLuint desert_wall_middle;
GLuint desert_wall_top;

static void
init()
{
	link_up[0] = texture_load_png("sprites/link-up-1.png");
	link_up[1] = texture_load_png("sprites/link-up-2.png");
	link_down[0] = texture_load_png("sprites/link-down-1.png");
	link_down[1] = texture_load_png("sprites/link-down-2.png");
	link_left[0] = texture_load_png("sprites/link-left-1.png");
	link_left[1] = texture_load_png("sprites/link-left-2.png");
	link_right[0] = texture_load_png("sprites/link-right-1.png");
	link_right[1] = texture_load_png("sprites/link-right-2.png");
	desert[0] = texture_load_png("tiles/desert-1.png");
	desert[1] = texture_load_png("tiles/desert-2.png");
	desert_wall_bottom = texture_load_png("tiles/desert-wall-bottom.png");
	desert_wall_middle = texture_load_png("tiles/desert-wall-middle.png");
	desert_wall_top = texture_load_png("tiles/desert-wall-top.png");
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

static int link_frame = 0;
static enum {
	FACE_A,
	FACE_B,
	AWAY_A,
	AWAY_B,
	LEFT_A,
	LEFT_B,
	RIGHT_A,
	RIGHT_B,
} link_dir = FACE_A;

static int link_x = 0;
static int link_y = 0;
static int link_z = 0;

static int link_vx = 0;
static int link_vy = 0;
static int link_vz = 0;

static float camera_x = 0;
static float camera_y = 2.0 * 256;
static float camera_z = 0.0 * 256;

static float camera_vx = 0;
static float camera_vy = 0;
static float camera_vz = 0;

static void
display()
{
	//glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	gluLookAt(camera_x / 256.0, camera_y / 256.0, camera_z / 256.0,
		link_x / 256.0, link_y + 2.0, link_z / 256.0,
		0.0, 1.0, 0.0);

	float camera_ax = 1e-3 * (link_x - camera_x)
		- 0.5 * camera_vx;
	float camera_ay = 1e-3 * (1 * 256 + link_y - camera_y)
		- 0.5 * camera_vy;
	float camera_az = 1e-3 * (4 * 256 + link_z - camera_z)
		- 0.5 * camera_vz;
	camera_vx += camera_ax;
	camera_vy += camera_ay;
	camera_vz += camera_az;

	camera_x += camera_vx;
	camera_y += camera_vy;
	camera_z += camera_vz;

	glTranslatef(-0.5, 0, 0);

	glBindTexture(GL_TEXTURE_2D, desert[0]);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	for(int y = -4; y < 4; y++) {
		for(int x = -4; x < 4; x++) {
			glTexCoord2i(0, 0); glVertex3f(x + 0, 0, y + 0);
			glTexCoord2i(1, 0); glVertex3f(x + 1, 0, y + 0);
			glTexCoord2i(1, 1); glVertex3f(x + 1, 0, y + 1);
			glTexCoord2i(0, 1); glVertex3f(x + 0, 0, y + 1);
		}
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, desert_wall_bottom);
	glBegin(GL_QUADS);
	for(int x = -4; x < 4; x++) {
		glTexCoord2i(0, 0); glVertex3f(x + 0, 1, -4);
		glTexCoord2i(1, 0); glVertex3f(x + 1, 1, -4);
		glTexCoord2i(1, 1); glVertex3f(x + 1, 0, -4);
		glTexCoord2i(0, 1); glVertex3f(x + 0, 0, -4);

		glTexCoord2i(0, 0); glVertex3f(-4, 1, x + 1);
		glTexCoord2i(1, 0); glVertex3f(-4, 1, x + 0);
		glTexCoord2i(1, 1); glVertex3f(-4, 0, x + 0);
		glTexCoord2i(0, 1); glVertex3f(-4, 0, x + 1);

		glTexCoord2i(0, 0); glVertex3f(4, 1, x + 0);
		glTexCoord2i(1, 0); glVertex3f(4, 1, x + 1);
		glTexCoord2i(1, 1); glVertex3f(4, 0, x + 1);
		glTexCoord2i(0, 1); glVertex3f(4, 0, x + 0);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, desert_wall_middle);
	glBegin(GL_QUADS);
	for(int y = 1; y < 3; y++) {
		for(int x = -4; x < 4; x++) {
			glTexCoord2i(0, 0); glVertex3f(x + 0, y + 1, -4);
			glTexCoord2i(1, 0); glVertex3f(x + 1, y + 1, -4);
			glTexCoord2i(1, 1); glVertex3f(x + 1, y + 0, -4);
			glTexCoord2i(0, 1); glVertex3f(x + 0, y + 0, -4);
		}
	}
	for(int y = 1; y < 3; y++) {
		for(int x = -4; x < 4; x++) {
			glTexCoord2i(0, 0); glVertex3f(-4, y + 1, x + 1);
			glTexCoord2i(1, 0); glVertex3f(-4, y + 1, x + 0);
			glTexCoord2i(1, 1); glVertex3f(-4, y + 0, x + 0);
			glTexCoord2i(0, 1); glVertex3f(-4, y + 0, x + 1);
		}
	}
	for(int y = 1; y < 3; y++) {
		for(int x = -4; x < 4; x++) {
			glTexCoord2i(0, 0); glVertex3f(4, y + 1, x + 0);
			glTexCoord2i(1, 0); glVertex3f(4, y + 1, x + 1);
			glTexCoord2i(1, 1); glVertex3f(4, y + 0, x + 1);
			glTexCoord2i(0, 1); glVertex3f(4, y + 0, x + 0);
		}
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, desert_wall_top);
	glBegin(GL_QUADS);
	for(int x = -4; x < 4; x++) {
		glTexCoord2i(0, 0); glVertex3f(x + 0, 4, -4);
		glTexCoord2i(1, 0); glVertex3f(x + 1, 4, -4);
		glTexCoord2i(1, 1); glVertex3f(x + 1, 3, -4);
		glTexCoord2i(0, 1); glVertex3f(x + 0, 3, -4);

		glTexCoord2i(0, 0); glVertex3f(-4, 4, x + 1);
		glTexCoord2i(1, 0); glVertex3f(-4, 4, x + 0);
		glTexCoord2i(1, 1); glVertex3f(-4, 3, x + 0);
		glTexCoord2i(0, 1); glVertex3f(-4, 3, x + 1);

		glTexCoord2i(0, 0); glVertex3f(4, 4, x + 0);
		glTexCoord2i(1, 0); glVertex3f(4, 4, x + 1);
		glTexCoord2i(1, 1); glVertex3f(4, 3, x + 1);
		glTexCoord2i(0, 1); glVertex3f(4, 3, x + 0);
	}
	glEnd();

	glPushMatrix();
	glTranslatef(link_x / 256.0, 0, link_z / 256.0);

	switch(link_dir) {
	case FACE_A:
		glBindTexture(GL_TEXTURE_2D, link_down[0]);
		break;
	case FACE_B:
		glBindTexture(GL_TEXTURE_2D, link_down[1]);
		break;
	case AWAY_A:
		glBindTexture(GL_TEXTURE_2D, link_up[0]);
		break;
	case AWAY_B:
		glBindTexture(GL_TEXTURE_2D, link_up[1]);
		break;
	case LEFT_A:
		glBindTexture(GL_TEXTURE_2D, link_left[0]);
		break;
	case LEFT_B:
		glBindTexture(GL_TEXTURE_2D, link_left[1]);
		break;
	case RIGHT_A:
		glBindTexture(GL_TEXTURE_2D, link_right[0]);
		break;
	case RIGHT_B:
		glBindTexture(GL_TEXTURE_2D, link_right[1]);
		break;
	}

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2i(0, 0); glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2i(1, 0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2i(1, 1); glVertex3f(1.0, 0.0, 0.0);
	glTexCoord2i(0, 1); glVertex3f(0.0, 0.0, 0.0);
	glEnd();

	switch(link_dir) {
	case FACE_A:
		glBindTexture(GL_TEXTURE_2D, link_up[0]);
		break;
	case FACE_B:
		glBindTexture(GL_TEXTURE_2D, link_up[1]);
		break;
	case AWAY_A:
		glBindTexture(GL_TEXTURE_2D, link_down[0]);
		break;
	case AWAY_B:
		glBindTexture(GL_TEXTURE_2D, link_down[1]);
		break;
	case LEFT_A:
		glBindTexture(GL_TEXTURE_2D, link_right[0]);
		break;
	case LEFT_B:
		glBindTexture(GL_TEXTURE_2D, link_right[1]);
		break;
	case RIGHT_A:
		glBindTexture(GL_TEXTURE_2D, link_left[0]);
		break;
	case RIGHT_B:
		glBindTexture(GL_TEXTURE_2D, link_left[1]);
		break;
	}

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2i(0, 0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2i(1, 0); glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2i(1, 1); glVertex3f(0.0, 0.0, 0.0);
	glTexCoord2i(0, 1); glVertex3f(1.0, 0.0, 0.0);
	glEnd();
	glPopMatrix();

	glPopMatrix();

	glutSwapBuffers();

	if(0)
		capture();

	if(link_frame == 0) {
		if(link_vx || link_vz) {
			switch(link_dir) {
			case FACE_A:
				link_dir = FACE_B;
				break;
			case FACE_B:
				link_dir = FACE_A;
				break;
			case AWAY_A:
				link_dir = AWAY_B;
				break;
			case AWAY_B:
				link_dir = AWAY_A;
				break;
			case LEFT_A:
				link_dir = LEFT_B;
				break;
			case LEFT_B:
				link_dir = LEFT_A;
				break;
			case RIGHT_A:
				link_dir = RIGHT_B;
				break;
			case RIGHT_B:
				link_dir = RIGHT_A;
				break;
			}
		}
	}

	link_frame = (link_frame + 1) % 128;
	link_x += link_vx;
	link_y += link_vy;
	link_z += link_vz;
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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 6.0, 8.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);
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
	case GLUT_KEY_LEFT:
		if(key_left)
			break;
		key_left = 1;

		if(!(link_vx || link_vz))
			link_frame = 0;
		link_vx--;
		link_dir = LEFT_A;
		break;
	case GLUT_KEY_UP:
		if(key_up)
			break;
		key_up = 1;

		if(!(link_vx || link_vz))
			link_frame = 0;
		link_vz--;
		link_dir = AWAY_A;
		break;
	case GLUT_KEY_RIGHT:
		if(key_right)
			break;
		key_right = 1;

		if(!(link_vx || link_vz))
			link_frame = 0;
		link_vx++;
		link_dir = RIGHT_A;
		break;
	case GLUT_KEY_DOWN:
		if(key_down)
			break;
		key_down = 1;

		if(!(link_vx || link_vz))
			link_frame = 0;
		link_vz++;
		link_dir = FACE_A;
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
	case GLUT_KEY_LEFT:
		if(!key_left)
			break;
		key_left = 0;

		link_vx++;
		if(!(link_vx || link_vz))
			link_dir = LEFT_B;
		break;
	case GLUT_KEY_UP:
		if(!key_up)
			break;
		key_up = 0;

		link_vz++;
		if(!(link_vx || link_vz))
			link_dir = AWAY_B;
		break;
	case GLUT_KEY_RIGHT:
		if(!key_right)
			break;
		key_right = 0;

		link_vx--;
		if(!(link_vx || link_vz))
			link_dir = RIGHT_B;
		break;
	case GLUT_KEY_DOWN:
		if(!key_down)
			break;
		key_down = 0;

		link_vz--;
		if(!(link_vx || link_vz))
			link_dir = FACE_B;
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
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	std::srand(std::time(NULL));

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

	//glutFullScreen();
	glutMainLoop();

	glutDestroyWindow(window);

	deinit();
	return 0;
}
