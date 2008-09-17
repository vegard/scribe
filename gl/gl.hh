#ifndef GL_GL_HH
#define GL_GL_HH

extern "C" {
#ifdef __APPLE__
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif
}

#endif
