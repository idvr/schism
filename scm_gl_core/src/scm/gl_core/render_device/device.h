
#ifndef SCM_GL_CORE_DEVICE_H_INCLUDED
#define SCM_GL_CORE_DEVICE_H_INCLUDED

#include <iosfwd>
#include <limits>
#include <list>
#include <utility>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/unordered_set.hpp>

#include <scm/core/math.h>
#include <scm/core/pointer_types.h>

#include <scm/gl_core/gl_core_fwd.h>
#include <scm/gl_core/data_formats.h>
#include <scm/gl_core/buffer_objects/buffer.h>
#include <scm/gl_core/state_objects/depth_stencil_state.h>
#include <scm/gl_core/state_objects/blend_state.h>

#include <scm/core/platform/platform.h>
#include <scm/core/utilities/platform_warning_disable.h>

namespace scm {
namespace gl {

namespace opengl {
class gl3_core;
} // namespace detail

class __scm_export(gl_core) render_device : boost::noncopyable
{
////// types //////////////////////////////////////////////////////////////////////////////////////
public:
    struct device_capabilities {
        int             _max_vertex_attributes;
        int             _max_draw_buffers;
        int             _max_dual_source_draw_buffers;
        int             _max_texture_size;
        int             _max_texture_3d_size;
        int             _max_samples;
        int             _max_array_texture_layers;
        int             _max_depth_texture_samples;
        int             _max_color_texture_samples;
        int             _max_integer_samples;
        int             _max_texture_image_units;
        int             _max_frame_buffer_color_attachments;
        int             _max_vertex_uniform_blocks;
        int             _max_geometry_uniform_blocks;
        int             _max_fragment_uniform_blocks;
        int             _max_combined_uniform_blocks;
        int             _max_uniform_buffer_bindings;
        int             _uniform_buffer_offset_alignment;
        int             _max_viewports;
    }; // struct device_capabilities

private:
    typedef boost::unordered_set<render_device_resource*>   resource_ptr_set;

    struct shader_macro {
        shader_macro(const std::string& n, const std::string& v) : _name(n), _value(v) {};
        std::string     _name;
        std::string     _value;
    };
    typedef std::list<shader_macro>             shader_macro_list;
    typedef std::list<std::string>              shader_include_list;
    typedef std::list<shader_ptr>               shader_list;

    typedef std::vector<buffer_ptr>             buffer_array;

////// methods ////////////////////////////////////////////////////////////////////////////////////
public:
    render_device();
    virtual ~render_device();

    // device /////////////////////////////////////////////////////////////////////////////////////
    const opengl::gl3_core&         opengl3_api() const;
    render_context_ptr              main_context() const;
    render_context_ptr              create_context();
    const device_capabilities&      capabilities() const;

    virtual void                    print_device_informations(std::ostream& os) const;

protected:
    void                            init_capabilities();

    void                            register_resource(render_device_resource* res_ptr);
    void                            release_resource(render_device_resource* res_ptr);

    // buffer api /////////////////////////////////////////////////////////////////////////////////
public:
    buffer_ptr                      create_buffer(const buffer::descriptor_type& in_buffer_desc,
                                                  const void*                    in_initial_data = 0);
    buffer_ptr                      create_buffer(buffer_binding in_binding,
                                                  buffer_usage   in_usage,
                                                  scm::size_t    in_size,
                                                  const void*    in_initial_data = 0);
    bool                            resize_buffer(const buffer_ptr& in_buffer, scm::size_t in_size);

    vertex_array_ptr                create_vertex_array(const vertex_format& in_vert_fmt,
                                                        const buffer_array&  in_attrib_buffers,
                                                        const program_ptr&   in_program = program_ptr());
    // shader api /////////////////////////////////////////////////////////////////////////////////
public:
    void                            add_include_path(const std::string& p);
    void                            add_macro_define(const shader_macro& d);
    shader_ptr                      create_shader(shader_stage t, const std::string& s);
    shader_ptr                      create_shader_from_file(shader_stage t, const std::string& f);
    program_ptr                     create_program(const shader_list& in_shaders);
    //shader_ptr                      create_shader(shader::stage_type t, const std::string& s, const shader_macro_list& m, std::ostream& err_os = std::cerr);
    //shader_ptr                      create_shader(shader::stage_type t, const std::string& s, const shader_include_list& i, std::ostream& err_os = std::cerr);
    //shader_ptr                      create_shader(shader::stage_type t, const std::string& s, const shader_macro_list& m, const shader_include_list& i, std::ostream& err_os = std::cerr);
    //shader_ptr                      create_shader_from_file(shader::stage_type t, const std::string& s, std::ostream& err_os = std::cerr);
    //shader_ptr                      create_shader_from_file(shader::stage_type t, const std::string& s, const shader_macro_list& m, std::ostream& err_os = std::cerr);
    //shader_ptr                      create_shader_from_file(shader::stage_type t, const std::string& s, const shader_include_list& i, std::ostream& err_os = std::cerr);
    //shader_ptr                      create_shader_from_file(shader::stage_type t, const std::string& s, const shader_macro_list& m, const shader_include_list& i, std::ostream& err_os = std::cerr);

    //virtual program_ptr                 create_program();
    //shader(shader_type t, const std::string& s, std::ostream& err_os = std::cerr);
    //shader(shader_type t, const std::string& s, const macro_definition_list& m, std::ostream& err_os = std::cerr);
    //shader(shader_type t, const std::string& s, const include_path_list& i, std::ostream& err_os = std::cerr);
    //shader(shader_type t, const std::string& s, const macro_definition_list& m, const include_path_list& i, std::ostream& err_os = std::cerr);

protected:

    // texture api ////////////////////////////////////////////////////////////////////////////////
public:
    texture_1d_ptr                  create_texture_1d(const texture_1d_desc&    in_desc);
    texture_1d_ptr                  create_texture_1d(const texture_1d_desc&    in_desc,
                                                      const data_format         in_initial_data_format,
                                                      const std::vector<void*>& in_initial_mip_level_data);
    texture_1d_ptr                  create_texture_1d(const unsigned      in_size,
                                                      const data_format   in_format,
                                                      const unsigned      in_mip_levels = 1,
                                                      const unsigned      in_array_layers = 1);
    texture_1d_ptr                  create_texture_1d(const unsigned            in_size,
                                                      const data_format         in_format,
                                                      const unsigned            in_mip_levels,
                                                      const unsigned            in_array_layers,
                                                      const data_format         in_initial_data_format,
                                                      const std::vector<void*>& in_initial_mip_level_data);

    texture_2d_ptr                  create_texture_2d(const texture_2d_desc&    in_desc);
    texture_2d_ptr                  create_texture_2d(const texture_2d_desc&    in_desc,
                                                      const data_format         in_initial_data_format,
                                                      const std::vector<void*>& in_initial_mip_level_data);
    texture_2d_ptr                  create_texture_2d(const math::vec2ui& in_size,
                                                      const data_format   in_format,
                                                      const unsigned      in_mip_levels = 1,
                                                      const unsigned      in_array_layers = 1,
                                                      const unsigned      in_samples = 1);
    texture_2d_ptr                  create_texture_2d(const math::vec2ui&       in_size,
                                                      const data_format         in_format,
                                                      const unsigned            in_mip_levels,
                                                      const unsigned            in_array_layers,
                                                      const unsigned            in_samples,
                                                      const data_format         in_initial_data_format,
                                                      const std::vector<void*>& in_initial_mip_level_data);

    texture_3d_ptr                  create_texture_3d(const texture_3d_desc&    in_desc);
    texture_3d_ptr                  create_texture_3d(const texture_3d_desc&    in_desc,
                                                      const data_format         in_initial_data_format,
                                                      const std::vector<void*>& in_initial_mip_level_data);
    texture_3d_ptr                  create_texture_3d(const math::vec3ui& in_size,
                                                      const data_format   in_format,
                                                      const unsigned      in_mip_levels = 1);
    texture_3d_ptr                  create_texture_3d(const math::vec3ui&       in_size,
                                                      const data_format         in_format,
                                                      const unsigned            in_mip_levels,
                                                      const data_format         in_initial_data_format,
                                                      const std::vector<void*>& in_initial_mip_level_data);

    sampler_state_ptr               create_sampler_state(const sampler_state_desc& in_desc);
    sampler_state_ptr               create_sampler_state(texture_filter_mode  in_filter1,
                                                         texture_wrap_mode    in_wrap,
                                                         unsigned             in_max_anisotropy = 1,
                                                         float                in_min_lod = -(std::numeric_limits<float>::max)(),
                                                         float                in_max_lod = (std::numeric_limits<float>::max)(),
                                                         float                in_lod_bias = 0.0f,
                                                         compare_func         in_compare_func = COMPARISON_LESS_EQUAL,
                                                         texture_compare_mode in_compare_mode = TEXCOMPARE_NONE);
    sampler_state_ptr               create_sampler_state(texture_filter_mode  in_filter,
                                                         texture_wrap_mode    in_wrap_s,
                                                         texture_wrap_mode    in_wrap_t,
                                                         texture_wrap_mode    in_wrap_r,
                                                         unsigned             in_max_anisotropy = 1,
                                                         float                in_min_lod = -(std::numeric_limits<float>::max)(),
                                                         float                in_max_lod = (std::numeric_limits<float>::max)(),
                                                         float                in_lod_bias = 0.0f,
                                                         compare_func         in_compare_func = COMPARISON_LESS_EQUAL,
                                                         texture_compare_mode in_compare_mode = TEXCOMPARE_NONE);

    // frame buffer api ///////////////////////////////////////////////////////////////////////////
    render_buffer_ptr               create_render_buffer(const render_buffer_desc& in_desc);
    render_buffer_ptr               create_render_buffer(const math::vec2ui& in_size,
                                                         const data_format   in_format,
                                                         const unsigned      in_samples = 1);
    frame_buffer_ptr                create_frame_buffer();


    // state api //////////////////////////////////////////////////////////////////////////////////
public:
    depth_stencil_state_ptr         create_depth_stencil_state(const depth_stencil_state_desc& in_desc);
    depth_stencil_state_ptr         create_depth_stencil_state(bool in_depth_test, bool in_depth_mask = true, compare_func in_depth_func = COMPARISON_LESS,
                                                               bool in_stencil_test = false, unsigned in_stencil_rmask = ~0u, unsigned in_stencil_wmask = ~0u,
                                                               stencil_ops in_stencil_ops = stencil_ops());
    depth_stencil_state_ptr         create_depth_stencil_state(bool in_depth_test, bool in_depth_mask, compare_func in_depth_func,
                                                               bool in_stencil_test, unsigned in_stencil_rmask, unsigned in_stencil_wmask,
                                                               stencil_ops in_stencil_front_ops, stencil_ops in_stencil_back_ops);
    rasterizer_state_ptr            create_rasterizer_state(const rasterizer_state_desc& in_desc);
    rasterizer_state_ptr            create_rasterizer_state(fill_mode in_fmode, cull_mode in_cmode = CULL_BACK, polygon_orientation in_fface = ORIENT_CCW,
                                                            bool in_msample = false, bool in_sctest = false, bool in_smlines = false);
    blend_state_ptr                 create_blend_state(const blend_state_desc& in_desc);
    blend_state_ptr                 create_blend_state(bool in_enabled,
                                                       blend_func in_src_rgb_func,   blend_func in_dst_rgb_func,
                                                       blend_func in_src_alpha_func, blend_func in_dst_alpha_func,
                                                       blend_equation  in_rgb_equation = EQ_FUNC_ADD, blend_equation in_alpha_equation = EQ_FUNC_ADD,
                                                       unsigned in_write_mask = COLOR_ALL, bool in_alpha_to_coverage = false);
    blend_state_ptr                 create_blend_state(const blend_ops_array& in_blend_ops, bool in_alpha_to_coverage = false);

    // query api //////////////////////////////////////////////////////////////////////////////////
public:
    timer_query_ptr                 create_timer_query();

////// attributes /////////////////////////////////////////////////////////////////////////////////
protected:
    // device /////////////////////////////////////////////////////////////////////////////////////
    shared_ptr<opengl::gl3_core>    _opengl3_api_core;
    render_context_ptr              _main_context;

    // shader api /////////////////////////////////////////////////////////////////////////////////
    shader_macro_list               _default_macro_defines;
    shader_include_list             _default_include_paths;

    device_capabilities             _capabilities;
    resource_ptr_set                _registered_resources;

}; // class render_device

__scm_export(gl_core) std::ostream& operator<<(std::ostream& os, const render_device& ren_dev);

} // namespace gl
} // namespace scm

#include <scm/core/utilities/platform_warning_enable.h>

#endif // SCM_GL_CORE_DEVICE_H_INCLUDED
