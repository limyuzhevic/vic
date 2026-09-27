#include "CLIEntryPoint.hpp"
#include "RunCommand.hpp"

namespace nlm {
namespace cli {

inline void registerRunCommand(CommandProcessor& processor) {
    processor.registerCommand(new RunCommand());
}

} // namespace cli
} // namespace nlm
