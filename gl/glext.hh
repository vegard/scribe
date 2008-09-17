#ifndef GL_GLEXT_HH
#define GL_GLEXT_HH

extern "C" {
#ifdef __APPLE__
# include <OpenGL/glext.h>
#else
# include <GL/glext.h>
#endif
}

#endif
