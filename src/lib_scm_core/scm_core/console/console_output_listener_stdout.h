
#ifndef CONSOLE_OUTPUT_LISTENER_STDOUT_H_INCLUDED
#define CONSOLE_OUTPUT_LISTENER_STDOUT_H_INCLUDED

#include <scm_core/console/console_output_listener.h>

#include <scm_core/platform/platform.h>
#include <scm_core/utilities/platform_warning_disable.h>

namespace scm {
namespace con {

class __scm_export console_output_listener_stdout : public console_output_listener
{
public:
    console_output_listener_stdout(console_system& con);
    virtual ~console_output_listener_stdout();

protected:
    void                update(const std::string&           /*update_buffer*/,
                               const console_out_stream&    /*stream_source*/);

}; // class console_output_listener_stdout

} // namespace con
} // namespace scm

#include <scm_core/utilities/platform_warning_enable.h>

#endif // CONSOLE_OUTPUT_LISTENER_STDOUT_H_INCLUDED