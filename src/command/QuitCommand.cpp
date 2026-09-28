// QuitCommand implementation
// NLM Advanced User Interface

#include "command/QuitCommand.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

bool QuitCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                         std::shared_ptr<Config> config) {
    // Quit is the same as exit
    return ExitCommand().execute(args, brain, config);
}

} // namespace nlm
