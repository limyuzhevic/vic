// HelpCommand implementation
// NLM Advanced User Interface

#include "command/HelpCommand.hpp"
#include "command/CommandRegistry.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <iomanip>

namespace nlm {

bool HelpCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                         std::shared_ptr<Config> config) {
    if (args.empty()) {
        // Display help for all commands
        NLM_LOG_INFO("=== NLM Advanced User Commands ===");
        NLM_LOG_INFO("");
        
        auto commandNames = CommandRegistry::getInstance().getCommandNames();
        
        // Sort command names for consistent display
        std::vector<std::string> sortedNames = commandNames;
        std::sort(sortedNames.begin(), sortedNames.end());
        
        // Find the longest command name for formatting
        size_t maxNameLength = 0;
        for (const auto& name : sortedNames) {
            maxNameLength = std::max(maxNameLength, name.length());
        }
        
        // Display command list with formatting
        NLM_LOG_INFO("Available commands:");
        for (const auto& name : sortedNames) {
            auto cmd = CommandRegistry::getInstance().getCommand(name);
            if (cmd) {
                // Format: command_name    description
                std::string formatted = "  " + name;
                formatted += std::string(maxNameLength - name.length() + 3, ' ');
                formatted += "- " + cmd->getDescription();
                NLM_LOG_INFO(formatted);
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("For detailed help on a specific command, use: help <command_name>");
        NLM_LOG_INFO("");
        
        NLM_LOG_INFO("Command categories:");
        NLM_LOG_INFO("  Brain Inspection: brain-info, neuron-analysis");
        NLM_LOG_INFO("  Memory Systems: memory-status, memory-control");
        NLM_LOG_INFO("  Neuromodulation: neuromod-status, neuromod-control");
        NLM_LOG_INFO("  Development: dev-status, dev-control");
        NLM_LOG_INFO("  Checkpoint Management: checkpoint-control");
        NLM_LOG_INFO("  Visualization: visualization");
        NLM_LOG_INFO("  Debugging: debug");
        NLM_LOG_INFO("  System: help, exit, quit");
        
    } else {
        // Display help for specific command
        std::string commandName = args[0];
        auto cmd = CommandRegistry::getInstance().getCommand(commandName);
        
        if (cmd) {
            NLM_LOG_INFO("=== Command: " + commandName + " ===");
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Description: " + cmd->getDescription());
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Usage: " + cmd->getUsage());
            NLM_LOG_INFO("");
            
            // Additional help information
            NLM_LOG_INFO("Command information:");
            NLM_LOG_INFO("  Command name: " + cmd->getName());
            NLM_LOG_INFO("  Can be completed: " + std::string(cmd->canComplete() ? "yes" : "no"));
            
            // Show example usage
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Example usage:");
            
            if (commandName == "brain-info") {
                NLM_LOG_INFO("  brain-info              - Show basic brain status");
                NLM_LOG_INFO("  brain-info verbose      - Show detailed brain information");
            } else if (commandName == "neuron-analysis") {
                NLM_LOG_INFO("  neuron-analysis 42            - Analyze neuron 42");
                NLM_LOG_INFO("  neuron-analysis 42 --population  - Analyze neuron population");
                NLM_LOG_INFO("  neuron-analysis 42 --connectivity - Analyze neuron connectivity");
            } else if (commandName == "memory-status") {
                NLM_LOG_INFO("  memory-status              - Show all memory systems");
                NLM_LOG_INFO("  memory-status --working     - Show working memory only");
                NLM_LOG_INFO("  memory-status --verbose     - Show detailed memory information");
            } else if (commandName == "memory-control") {
                NLM_LOG_INFO("  memory-control store --value 0.8     - Store memory value 0.8");
                NLM_LOG_INFO("  memory-control retrieve         - Retrieve stored memories");
                NLM_LOG_INFO("  memory-control clear --clear-all  - Clear all memory");
            } else if (commandName == "neuromod-status") {
                NLM_LOG_INFO("  neuromod-status              - Show all neuromodulators");
                NLM_LOG_INFO("  neuromod-status --dopamine    - Show dopamine only");
                NLM_LOG_INFO("  neuromod-status --verbose     - Show detailed neuromodulator info");
            } else if (commandName == "neuromod-control") {
                NLM_LOG_INFO("  neuromod-control apply --value 1.0     - Apply neuromodulation signal");
                NLM_LOG_INFO("  neuromod-control set --value 0.5       - Set neuromodulation levels");
                NLM_LOG_INFO("  neuromod-control reset                - Reset to baseline");
            } else if (commandName == "dev-status") {
                NLM_LOG_INFO("  dev-status                  - Show development status");
                NLM_LOG_INFO("  dev-status --stage 2        - Set stage to 2");
                NLM_LOG_INFO("  dev-status --plasticity     - Show plasticity rates");
            } else if (commandName == "dev-control") {
                NLM_LOG_INFO("  dev-control advance --advance 2    - Advance 2 development stages");
                NLM_LOG_INFO("  dev-control modify --modify 2.0     - Modify plasticity by 2x");
                NLM_LOG_INFO("  dev-control trigger --trigger maturation - Trigger maturation");
            } else if (commandName == "checkpoint-control") {
                NLM_LOG_INFO("  checkpoint-control save --path ./brain.bin  - Save brain state");
                NLM_LOG_INFO("  checkpoint-control load --path ./brain.bin  - Load brain state");
                NLM_LOG_INFO("  checkpoint-control list                  - List checkpoints");
            } else if (commandName == "visualization") {
                NLM_LOG_INFO("  visualization enable               - Enable visualization");
                NLM_LOG_INFO("  visualization output --format png   - Set output format");
            } else if (commandName == "debug") {
                NLM_LOG_INFO("  debug enable                      - Enable debug mode");
                NLM_LOG_INFO("  debug inject --value 1.0 --target neuron 42 - Inject debug input");
                NLM_LOG_INFO("  debug inspect                     - Inspect brain state");
            } else if (commandName == "exit" || commandName == "quit") {
                NLM_LOG_INFO("  exit                          - Exit application");
                NLM_LOG_INFO("  quit                          - Exit application (same as exit)");
            }
        } else {
            NLM_LOG_ERROR("Unknown command: " + commandName);
            NLM_LOG_INFO("Type 'help' for a list of available commands.");
            return false;
        }
    }
    
    return true;
}

} // namespace nlm
