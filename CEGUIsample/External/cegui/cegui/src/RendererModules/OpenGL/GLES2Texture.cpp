/***********************************************************************
    created:    Sun Jan 11 2009
    author:     Paul D Turner
*************************************************************************/
/***************************************************************************
     Copyright (C) 2004 - 2009 Paul D Turner & The CEGUI Development Team

     Permission is hereby granted, free of charge, to any person obtaining
     a copy of this software and associated documentation files (the
     "Software"), to deal in the Software without restriction, including
     without limitation the rights to use, copy, modify, merge, publish,
     distribute, sublicense, and/or sell copies of the Software, and to
     permit persons to whom the Software is furnished to do so, subject to
     the following conditions:

     The above copyright notice and this permission notice shall be
     included in all copies or substantial portions of the Software.

     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
     EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
     IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
     OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
     ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
     OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/RendererModules/OpenGL/GLES2Texture.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/System.h"
#include "CEGUI/ImageCodec.h"
#include "CEGUI/Logger.h"
#include "CEGUI/RendererModules/OpenGL/GL.h"

#include <cmath>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
GLES2Texture::GLES2Texture(OpenGLRendererBase& owner, const String& name)
    : OpenGLTexture(owner, name)
{
}

//----------------------------------------------------------------------------//                                      
GLES2Texture::~GLES2Texture()
{                                                                                                                     
}

//----------------------------------------------------------------------------//
void GLES2Texture::initInternalPixelFormatFields(const PixelFormat fmt)
{
    d_isCompressed = false;

    switch(fmt)
    {
    case PF_RGBA:
        d_format = GL_RGBA;
        d_subpixelFormat = GL_UNSIGNED_BYTE;
        break;

    case PF_RGB:
        d_format = GL_RGB;
        d_subpixelFormat = GL_UNSIGNED_BYTE;
        break;

    case PF_RGB_565:
        d_format = GL_RGB;
        d_subpixelFormat = GL_UNSIGNED_SHORT_5_6_5;
        break;

    case PF_RGBA_4444:
        d_format = GL_RGBA;
        d_subpixelFormat = GL_UNSIGNED_SHORT_4_4_4_4;
        break;

    default:
        CEGUI_THROW(RendererException(
                        "invalid or unsupported CEGUI::PixelFormat."));
    }
}



//----------------------------------------------------------------------------//
GLsizei GLES2Texture::getCompressedTextureSize(const Sizef& pixel_size) const
{
    GLsizei blocksize = 16;
    return (
               static_cast<GLsizei>(
                   std::ceil(pixel_size.d_width / 4) *
                   std::ceil(pixel_size.d_height / 4) *
                   blocksize));
}

//----------------------------------------------------------------------------//
void GLES2Texture::setTextureSize_impl(const Sizef& sz)
{
    const Sizef size(d_owner.getAdjustedTextureSize(sz));
    d_size = size;

    // make sure size is within boundaries
    GLfloat maxSize;
    glGetFloatv(GL_MAX_TEXTURE_SIZE, &maxSize);

    char tBuff[250];
    Logger::getSingleton().logEvent( tBuff );

    if((size.d_width > maxSize) || (size.d_height > maxSize))
        CEGUI_THROW(RendererException("size too big"));

    // save old texture binding
    GLuint old_tex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&old_tex));

    Logger::getSingleton().logEvent( tBuff );

    // set texture to required size
    glBindTexture(GL_TEXTURE_2D, d_ogltexture);

    Logger::getSingleton().logEvent( tBuff );

    if(d_isCompressed)
    {
        const GLsizei image_size = getCompressedTextureSize(size);
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, d_format,
                               static_cast<GLsizei>(size.d_width),
                               static_cast<GLsizei>(size.d_height),
                               0, image_size, 0);
    }
    else
    {
#ifndef CEGUI_GLES3_SUPPORT
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
#endif
                     static_cast<GLsizei>(size.d_width),
                     static_cast<GLsizei>(size.d_height),
                     0, GL_RGBA , GL_UNSIGNED_BYTE, 0);
    }

    // restore previous texture binding.
    glBindTexture(GL_TEXTURE_2D, old_tex);
}

//----------------------------------------------------------------------------//
void GLES2Texture::blitToMemory(void* targetData)
{
    /* OpenGL ES 3.1 or below doesn't support
       "glGetTexImage"/"glGetCompressedTexImage", so we need to emulate it
       using "glReadPixels". */

    GLint old_pack;
    glGetIntegerv(GL_PACK_ALIGNMENT, &old_pack);
    GLuint texture_framebuffer(0);
    GLint framebuffer_old(0), pack_alignment_old(0);
    glGenFramebuffers(1, &texture_framebuffer);
    GLenum framebuffer_target(0), framebuffer_param(0);
    framebuffer_param = GL_READ_FRAMEBUFFER_BINDING;
    framebuffer_target = GL_READ_FRAMEBUFFER;
    glGetIntegerv(framebuffer_param, &framebuffer_old);
    glBindFramebuffer(framebuffer_target, texture_framebuffer);
    glFramebufferTexture2D(framebuffer_target, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           d_ogltexture, 0);
    GLint read_buffer_old(0), pixel_pack_buffer_old(0);
    glGetIntegerv(GL_READ_BUFFER, &read_buffer_old);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &pixel_pack_buffer_old);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack_alignment_old);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, static_cast<GLsizei>(d_dataSize.d_width),
                 static_cast<GLsizei>(d_dataSize.d_height), GL_RGBA, GL_UNSIGNED_BYTE,
                 targetData);
    glPixelStorei(GL_PACK_ALIGNMENT, pack_alignment_old);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pixel_pack_buffer_old);
    glReadBuffer(read_buffer_old);
    glBindFramebuffer(framebuffer_target, framebuffer_old);
    glDeleteFramebuffers(1, &texture_framebuffer);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section


