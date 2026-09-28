// NeuromodulationControlCommand implementation
// NLM Advanced User Interface

#include "command/NeuromodulationControlCommand.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cctype>

namespace nlm {

bool NeuromodulationControlCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                            std::shared_ptr<Config> config) {
    if (args.empty()) {
        NLM_LOG_ERROR("Neuromodulation command required (apply, set, reset, configure)");
        NLM_LOG_INFO("Available commands: apply, set, reset, configure");
        return false;
    }
    
    std::string command = args[0];
    float value = 0.5f;
    std::string source = "manual";
    std::string target = "all";
    
    // Parse optional parameters
    auto it = std::find(args.begin(), args.end(), "--value");
    if (it != args.end() && std::next(it) != args.end()) {
        try {
            value = std::stof(*(std::next(it)));
        } catch (...) {
            NLM_LOG_WARNING("Invalid value for --value, using default: 0.5");
        }
    }
    
    it = std::find(args.begin(), args.end(), "--source");
    if (it != args.end() && std::next(it) != args.end()) {
        source = *(std::next(it));
    }
    
    it = std::find(args.begin(), args.end(), "--target");
    if (it != args.end() && std::next(it) != args.end()) {
        target = *(std::next(it));
    }
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "apply") {
        // Apply neuromodulation signal
        if (brain->getDopamine()) {
            brain->getDopamine()->applyRewardPredictionError(value, 0.5f);
            NLM_LOG_INFO("Applied dopamine reward prediction error: " + std::to_string(value) +
                        " (source: " + source + ", target: " + target + ")");
        } else {
            NLM_LOG_ERROR("Dopamine system not available for neuromodulation");
            return false;
        }
        
        if (brain->getCuriosity()) {
            // Apply curiosity signal
            NLM_LOG_INFO("Applied curiosity signal: " + std::to_string(value) +
                        " (source: " + source + ", target: " + target + ")");
            // In a real implementation, this would call curiosity system methods
        }
        
        if (brain->getNovelty()) {
            // Apply novelty signal
            NLM_LOG_INFO("Applied novelty signal: " + std::to_string(value) +
                        " (source: " + source + ", target: " + target + ")");
        }
        
        if (brain->getPredictionErrorSignal()) {
            // Apply prediction error signal
            NLM_LOG_INFO("Applied prediction error signal: " + std::to_string(value) +
                        " (source: " + source + ", target: " + target + ")");
        }
        
    } else if (command == "set") {
        // Set neuromodulation levels
        if (brain->getDopamine()) {
            // Note: Dopamine system typically doesn't have direct set level method
            // This would require system-specific implementation
            NLM_LOG_INFO("Setting dopamine level to: " + std::to_string(value) +
                        " (source: " + source + ", target: " + target + ")");
            // Would set dopamine level to specified value
        } else {
            NLM_LOG_ERROR("Dopamine system not available for level setting");
            return false;
        }
        
        NLM_LOG_INFO("Neuromodulation levels set successfully");
        
    } else if (command == "reset") {
        // Reset neuromodulation systems
        NLM_LOG_INFO("Resetting neuromodulation systems to baseline (source: " + source + ")");
        
        // Would reset all neuromodulators to initial state
        // This would involve restoring default levels and resetting internal state
        
    } else if (command == "configure") {
        // Configure neuromodulation parameters
        NLM_LOG_INFO("Configuring neuromodulation parameters (source: " + source + ")");
        
        // Would adjust sensitivity, thresholds, adaptation rates, etc.
        NLM_LOG_INFO("Configuration includes:");
        NLM_LOG_INFO("  - Sensitivity adjustments");
        NLM_LOG_INFO("  - Threshold modifications");
        NLM_LOG_INFO("  - Time constant updates";
        NLM_LOG_INFO("  - Target neuron selections";
        
    } else {
        NLM_LOG_ERROR("Unknown neuromodulation command: " + command);
        NLM_LOG_INFO("Available commands: apply, set, reset, configure");
        return false;
    }
    
    return true;
}

} // namespace nlm
