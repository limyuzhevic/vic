// ExitCommand implementation
// NLM Advanced User Interface

#include "command/ExitCommand.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

bool ExitCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                         std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Exiting NLM application...");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Thanks for using NLM - Neural Learning Machine");
    NLM_LOG_INFO("Phase 6: Final Integration");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Final status:");
    
    if (brain) {
        brain->logStatus();
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Application terminated by user command.");
    
    // Signal that the application should exit
    // In a real implementation, this would set a flag that the main loop checks
    return false; // Signal to stop processing
}

} // namespace nlm
