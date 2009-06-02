
#include "texture_1d.h"

#include <cassert>

#include <scm/gl/utilities/error_checker.h>
#include <scm/gl/utilities/gl_assert.h>

namespace scm {
namespace gl {

texture_1d::texture_1d()
    : texture(GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D),
      _width(0)
{
}

texture_1d::~texture_1d()
{
}

const texture_1d& texture_1d::operator=(const texture_1d& rhs)
{
    gl::texture::operator=(rhs);

    this->_width    = rhs._width;

    return (*this);
}

bool texture_1d::image_data(GLint     mip_level,
                            GLint     internal_format,
                            GLsizei   width,
                            GLenum    format,
                            GLenum    type,
                            const GLvoid *data)
{
    gl_assert_error("entering texture_1d::image_data()");

    gl::error_checker ech;
#ifdef SCM_GL_USE_DIRECT_STATE_ACCESS
    glTextureImage1DEXT(id(),
                        target(),
                        mip_level,
                        internal_format,
                        width,
                        0,
                        format,
                        type,
                        data);
    assert(ech.ok());

#else // SCM_GL_USE_DIRECT_STATE_ACCESS

    binding_guard guard(target(), binding());
    bind();
    glTexImage1D(target(),
                 mip_level,
                 internal_format,
                 width,
                 0,
                 format,
                 type,
                 data);

    assert(ech.ok());
    //if ((_last_error = glGetError()) != GL_NO_ERROR) {
    //    return (false);
    //}
    unbind();
#endif // SCM_GL_USE_DIRECT_STATE_ACCESS

    _width = width;

    parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    parameter(GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);

    gl_assert_error("exiting texture_1d::image_data()");

    return (true);
}

} // namespace gl
} // namespace scm