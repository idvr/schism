
#include "text.h"

#include <cassert>
#include <limits>
#include <exception>
#include <stdexcept>
#include <string>
#include <sstream>

#include <boost/assign/list_of.hpp>

#include <scm/log.h>

#include <scm/gl_core/math.h>
#include <scm/gl_core/buffer_objects.h>
#include <scm/gl_core/render_device.h>

#include <scm/gl_util/font/font_face.h>

namespace {
struct vertex {
    scm::math::vec3f pos;
    scm::math::vec3f tex;
};
} // namespace

namespace scm {
namespace gl {

text::text(const render_device_ptr&     device,
           const font_face_cptr&        font,
           const font_face::style_type  stl,
           const std::string&           str)
  : _font(font)
  , _text_style(stl)
  , _text_string(str)
  , _text_kerning(true)
  , _indices_count(0)
  , _topology(PRIMITIVE_TRIANGLE_LIST)
  , _glyph_capacity(20)
  , _render_device(device)
  , _render_context(device->main_context())
{
    using boost::assign::list_of;

    int num_vertices = _glyph_capacity * 4; // one quad per glyph 
    _indices_count   = _glyph_capacity * 6; // two triangles per glyph

    scoped_array<vertex>            vert(new vertex[num_vertices]);
    scoped_array<unsigned short>    ind(new unsigned short[_indices_count]);

    _vertex_buffer = device->create_buffer(BIND_VERTEX_BUFFER, USAGE_STREAM_DRAW, num_vertices * sizeof(vertex), 0);
    _index_buffer  = device->create_buffer(BIND_INDEX_BUFFER, USAGE_STREAM_DRAW, _indices_count * sizeof(unsigned short), 0);
    _vertex_array  = device->create_vertex_array(vertex_format(0, 0, TYPE_VEC3F, sizeof(vertex))
                                                              (0, 2, TYPE_VEC3F, sizeof(vertex)),
                                                 list_of(_vertex_buffer));
    update();
}

text::~text()
{
    _vertex_array.reset();
    _vertex_buffer.reset();
    _index_buffer.reset();
}

const font_face_cptr&
text::font() const
{
    return (_font);
}

const font_face::style_type
text::text_style() const
{
    return (_text_style);
}

const std::string&
text::text_string() const
{
    return (_text_string);
}

void
text::text_string(const std::string& str)
{
    text_string(str, text_style());
}

void
text::text_string(const std::string& str,
                  const font_face::style_type stl)
{
    _text_string = str;
    _text_style  = stl;
    update();
}

bool
text::text_kerning() const
{
    return (_text_kerning);
}

void
text::text_kerning(bool k)
{
    _text_kerning = k;
}

void
text::update()
{
    if (_glyph_capacity < _text_string.size()) {
        // resize the buffers
        if (render_device_ptr device = _render_device.lock()) {
            _indices_count   = 0;
            _glyph_capacity  = static_cast<int>(_text_string.size() + _text_string.size() / 2); // make it 50% bigger as required currently

            int num_vertices = _glyph_capacity * 4; 
            _indices_count   = _glyph_capacity * 6;
            if (!device->resize_buffer(_vertex_buffer, num_vertices * sizeof(vertex))) {
                err() << log::error
                      << "text::update(): unable to resize vertex buffer (size : " << num_vertices * sizeof(vertex) << ")." << log::end;
                return;
            }
            if (!device->resize_buffer(_index_buffer, _indices_count * sizeof(unsigned short))) {
                err() << log::error
                      << "text::update(): unable to resize index buffer (size : " << _indices_count * sizeof(unsigned short) << ")." << log::end;
                return;
            }
        }
        else  {
            err() << log::error
                  << "text::update(): unable to optain render device from weak pointer." << log::end;
            return;
        }
    }

    if (render_context_ptr context = _render_context.lock()) {
        using namespace scm::math;

        vec2i           current_pos = vec2i(0, 0);
        char            prev_char   = 0;
        vertex*         vertex_data = static_cast<vertex*>(context->map_buffer(_vertex_buffer, ACCESS_WRITE_INVALIDATE_BUFFER));
        unsigned short* index_data  = static_cast<unsigned short*>(context->map_buffer(_index_buffer, ACCESS_WRITE_INVALIDATE_BUFFER));

        if ((vertex_data == 0) || (index_data == 0)) {
            err() << log::error
                  << "text::update(): unable to map vertex element or index buffer." << log::end;
            return;
        }

        assert(_text_string.size() < (6 * (std::numeric_limits<unsigned short>::max)()));
        unsigned short str_size = static_cast<unsigned short>( _text_string.size());
        for (unsigned short i = 0; i < str_size; ++i) {
            char  cur_char = _text_string[i];

            if (cur_char == '\n') {
                current_pos.y -= _font->line_advance(_text_style);
                prev_char      = 0;
                continue;
            }
            const font_face::glyph_info& cur_glyph = _font->glyph(cur_char, _text_style);
            // kerning
            if (_text_kerning && prev_char) {
                current_pos.x += _font->kerning(prev_char, cur_char, _text_style);
            }

            vertex_data[i * 4    ].pos = vec2f(current_pos + cur_glyph._bearing);                                   // 00
            vertex_data[i * 4 + 1].pos = vec2f(current_pos + cur_glyph._bearing + vec2i(cur_glyph._box_size.x, 0)); // 10
            vertex_data[i * 4 + 2].pos = vec2f(current_pos + cur_glyph._bearing + cur_glyph._box_size);             // 11
            vertex_data[i * 4 + 3].pos = vec2f(current_pos + cur_glyph._bearing + vec2i(0, cur_glyph._box_size.y)); // 01

            vertex_data[i * 4    ].tex = cur_glyph._texture_origin;                                              // 00
            vertex_data[i * 4 + 1].tex = cur_glyph._texture_origin + vec2f(cur_glyph._texture_box_size.x, 0.0f); // 10
            vertex_data[i * 4 + 2].tex = cur_glyph._texture_origin + cur_glyph._texture_box_size;                // 11
            vertex_data[i * 4 + 3].tex = cur_glyph._texture_origin + vec2f(0.0f, cur_glyph._texture_box_size.y); // 01

            vertex_data[i * 4    ].tex.z = static_cast<float>(_text_style);
            vertex_data[i * 4 + 1].tex.z = static_cast<float>(_text_style);
            vertex_data[i * 4 + 2].tex.z = static_cast<float>(_text_style);
            vertex_data[i * 4 + 3].tex.z = static_cast<float>(_text_style);

            index_data[i * 6    ] = i * 4;
            index_data[i * 6 + 1] = i * 4 + 1;
            index_data[i * 6 + 2] = i * 4 + 2;
            index_data[i * 6 + 3] = i * 4;
            index_data[i * 6 + 4] = i * 4 + 2;
            index_data[i * 6 + 5] = i * 4 + 3;

            // advance the position
            current_pos.x += cur_glyph._advance;

            // remember just drawn glyph for kerning
            prev_char = cur_char;
        }
        
        _indices_count = str_size * 6;

        context->unmap_buffer(_vertex_buffer);
        context->unmap_buffer(_index_buffer);
    }
    else {
        err() << log::error
                << "text::update(): unable to optain render context from weak pointer." << log::end;
        return;
    }
}

} // namespace gl
} // namespace scm
