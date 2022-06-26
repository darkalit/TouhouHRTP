#include "Debug.h"

const char* GLErrorToString(GLenum err)
{
	switch (err)
	{
	case GL_NO_ERROR:                      return "GL_NO_ERROR";
	case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
	case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
	case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
	case 0x8031: /* not core */            return "GL_TABLE_TOO_LARGE_EXT";
	case 0x8065: /* not core */            return "GL_TEXTURE_TOO_LARGE_EXT";
	case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
	default:
		assert(!"Unhandled GL error code");
		return NULL;
	}
}

const char* GLFrameBufferStatus(GLenum err)
{
	switch (err)
	{
	case GL_FRAMEBUFFER_UNDEFINED:                      return "GL_FRAMEBUFFER_UNDEFINED";
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:          return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:			return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:			return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
	case GL_FRAMEBUFFER_UNSUPPORTED:					return "GL_FRAMEBUFFER_UNSUPPORTED";
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:			return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:		return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
	default:
		assert(!"Unhandled GL error code");
		return NULL;
	}
}
