#pragma once

#include <string>
#include <vector>

namespace no::core {

/**
 * \brief Represents the result of an executed child process.
 */
struct ProcessResult {
    int ExitCode = -1;
    std::string CommandLine;
};

/**
 * \brief Executes external tools required by the application.
 */
class ProcessRunner {
   public:
    /**
     * \brief Executes a command with arguments.
     *
     * \param executable The executable name or absolute path.
     * \param arguments The command line arguments.
     * \return The process result.
     */
    ProcessResult Run(const std::string& executable, const std::vector<std::string>& arguments) const;

   private:
    static std::string Escape(const std::string& value);
};

}  // namespace no::core
