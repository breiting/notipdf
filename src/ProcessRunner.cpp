#include "core/ProcessRunner.hpp"

#include <cstdlib>
#include <sstream>

namespace no::core {

ProcessResult ProcessRunner::Run(const std::string& executable, const std::vector<std::string>& arguments) const {
    std::ostringstream command;
    command << Escape(executable);

    for (const std::string& argument : arguments) {
        command << " " << Escape(argument);
    }

    ProcessResult result;
    result.CommandLine = command.str();
    result.ExitCode = std::system(result.CommandLine.c_str());
    return result;
}

std::string ProcessRunner::Escape(const std::string& value) {
    std::string escaped = "'";
    for (char c : value) {
        if (c == '\'') {
            escaped += "'\\''";
        } else {
            escaped += c;
        }
    }
    escaped += "'";
    return escaped;
}

}  // namespace no::core
