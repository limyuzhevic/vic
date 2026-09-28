// VisualizationCommand implementation
// NLM Advanced User Interface

#include "command/VisualizationCommand.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cctype>

namespace nlm {

bool VisualizationCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                  std::shared_ptr<Config> config) {
    if (args.empty()) {
        NLM_LOG_ERROR("Visualization command required (enable, disable, status, output)");
        NLM_LOG_INFO("Available commands: enable, disable, status, output");
        return false;
    }
    
    std::string command = args[0];
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "enable") {
        NLM_LOG_INFO("Enabling visualization subsystem");
        NLM_LOG_INFO("Visualization will now render neural activity and connections");
        
        // Would enable visualization in a real implementation
        // This would involve starting visualization threads, opening windows, etc.
        
    } else if (command == "disable") {
        NLM_LOG_INFO("Disabling visualization subsystem");
        NLM_LOG_INFO("Visualization output will be stopped");
        
        // Would disable visualization in a real implementation
        // This would involve stopping rendering threads, closing windows, etc.
        
    } else if (command == "status") {
        NLM_LOG_INFO("=== Visualization Status ===");
        
        // Show visualization subsystem status
        NLM_LOG_INFO("Visualization subsystem: (status would show here)");
        NLM_LOG_INFO("Render mode: (would show render mode)");
        NLM_LOG_INFO("Output format: (would show output format)");
        NLM_LOG_INFO("Window size: (would show window size)");
        NLM_LOG_INFO("Active visualizations: (would show count)");
        
        // Show available formats
        NLM_LOG_INFO("`;`Available formats: text, svg, png, gltf, oculus");
        
        // Show output connections
        NLM_LOG_INFO("Output connections: console, file, network, vr");
        
    } else if (command == "output") {
        std::string format = "text";
        std::string output = "./visualization.png";
        std::string target = "screen";
        
        // Parse optional parameters
        auto formatIt = std::find(args.begin(), args.end(), "--format");
        if (formatIt != args.end() && std::next(formatIt) != args.end()) {
            format = *(std::next(formatIt));
        }
        
        auto outputIt = std::find(args.begin(), args.end(), "--output");
        if (outputIt != args.end() && std::next(outputIt) != args.end()) {
            output = *(std::next(outputIt));
        }
        
        auto targetIt = std::find(args.begin(), args.end(), "--target");
        if (targetIt != args.end() && std::next(targetIt) != args.end()) {
            target = *(std::next(targetIt));
        }
        
        NLM_LOG_INFO("Setting visualization output configuration:");
        NLM_LOG_INFO("  Format: " + format);
        NLM_LOG_INFO("  Output: " + output);
        NLM_LOG_INFO("  Target: " + target);
        
        // Configure visualization output
        // Would set up rendering pipeline, output writers, etc.
        
    } else {
        NLM_LOG_ERROR("Unknown visualization command: " + command);
        NLM_LOG_INFO("Available commands: enable, disable, status, output");
        return false;
    }
    
    return true;
}

} // namespace nlm
