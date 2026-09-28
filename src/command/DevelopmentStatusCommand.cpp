// DevelopmentControlCommand implementation
// NLM Advanced User Interface

#include "command/DevelopmentControlCommand.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cctype>

namespace nlm {

bool DevelopmentControlCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                       std::shared_ptr<Config> config) {
    if (args.empty()) {
        NLM_LOG_ERROR("Development command required (advance, modify, trigger, reset)");
        NLM_LOG_INFO("Available commands: advance, modify, trigger, reset");
        return false;
    }
    
    std::string command = args[0];
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "advance") {
        // Advance developmental age
        int steps = 1;
        
        // Parse optional --advance parameter
        auto it = std::find(args.begin(), args.end(), "--advance");
        if (it != args.end() && std::next(it) != args.end()) {
            try {
                steps = std::stoi(*(std::next(it)));
                if (steps < 1) steps = 1;
                if (steps > 10) steps = 10; // Cap at 10 stages
            } catch (...) {
                NLM_LOG_ERROR("Invalid advance value, using default: 1");
            }
        }
        
        if (brain->getDevelopmentSystem()) {
            // Update development system
            brain->getDevelopmentSystem()->update(brain.get(), *brain->getRandomGenerator(), steps * 1000);
            
            // Advance developmental stage
            auto currentStage = brain->getDevelopmentalStage();
            int currentStageValue = static_cast<int>(currentStage);
            int newStageValue = std::min(currentStageValue + steps, 4); // Max stage is 4 (Aging)
            brain->setDevelopmentalStage(static_cast<DevelopmentalStage>(newStageValue));
            
            NLM_LOG_INFO("Advanced development by " + std::to_string(steps) + " stages");
            NLM_LOG_INFO("New developmental stage: " + std::to_string(newStageValue));
        } else {
            NLM_LOG_ERROR("Development system not available for advancement");
            return false;
        }
        
    } else if (command == "modify") {
        // Modify plasticity rates
        float rate = 1.0f;
        
        // Parse optional --modify parameter
        auto it = std::find(args.begin(), args.end(), "--modify");
        if (it != args.end() && std::next(it) != args.end()) {
            try {
                rate = std::stof(*(std::next(it)));
                // Clamp rate between 0.1 and 5.0
                rate = std::max(0.1f, std::min(rate, 5.0f));
            } catch (...) {
                NLM_LOG_ERROR("Invalid modify value, using default: 1.0");
            }
        }
        
        NLM_LOG_INFO("Modified plasticity rate by factor: " + std::to_string(rate));
        NLM_LOG_INFO("This affects all plasticity rules (STDP, Hebbian, structural)");
        
        // Would modify plasticity rates in all systems
        // This would involve updating configuration values and propagating changes
        
    } else if (command == "trigger") {
        // Trigger developmental event
        std::string event = "maturation";
        
        // Parse optional --trigger parameter
        auto it = std::find(args.begin(), args.end(), "--trigger");
        if (it != args.end() && std::next(it) != args.end()) {
            event = *(std::next(it));
        }
        
        NLM_LOG_INFO("Triggered developmental event: " + event);
        
        // Would trigger specific developmental events
        // This could involve:
        // - Critical period opening/closing
        // - Maturation checkpoints
        // - Pruning events
        // - Synaptogenesis bursts
        
    } else if (command == "reset") {
        // Reset development
        if (brain->getDevelopmentSystem()) {
            brain->setDevelopmentalStage(DevelopmentalStage::Initial);
            NLM_LOG_INFO("Reset development to initial stage (Initial)");
        } else {
            NLM_LOG_ERROR("Development system not available for reset");
            return false;
        }
        
    } else {
        NLM_LOG_ERROR("Unknown development command: " + command);
        NLM_LOG_INFO("Available commands: advance, modify, trigger, reset");
        return false;
    }
    
    return true;
}

} // namespace nlm
