// BrainInfoCommand implementation
// NLM Advanced User Interface

#include "command/BrainInfoCommand.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

bool BrainInfoCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                             std::shared_ptr<Config> config) {
    brain->logStatus();
    
    if (!args.empty() && args[0] == "verbose") {
        NLM_LOG_INFO("=== Detailed Brain Information ===");
        NLM_LOG_INFO("Brain type: Integrated Artificial Brain (Phase 6)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        NLM_LOG_INFO("Excitatory/Inhibitory ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        
        // Check each system's integration status
        NLM_LOG_INFO("=== System Integration Status ===");
        
        if (brain->getWorkingMemory()) {
            NLM_LOG_INFO("Working Memory: ENABLED - " + std::to_string(brain->getWorkingMemory()->getActiveTraces()) + " active traces");
        } else {
            NLM_LOG_INFO("Working Memory: DISABLED");
        }
        
        if (brain->getEpisodicMemory()) {
            NLM_LOG_INFO("Episodic Memory: ENABLED - " + std::to_string(brain->getEpisodicMemory()->getEpisodeCount()) + " episodes");
        } else {
            NLM_LOG_INFO("Episodic Memory: DISABLED");
        }
        
        if (brain->getDopamine()) {
            NLM_LOG_INFO("Dopamine: ENABLED - level: " + std::to_string(brain->getDopamine()->getLevel()));
        } else {
            NLM_LOG_INFO("Dopamine: DISABLED");
        }
        
        if (brain->getPredictionSystem()) {
            NLM_LOG_INFO("Prediction System: ENABLED");
        } else {
            NLM_LOG_INFO("Prediction System: DISABLED");
        }
        
        if (brain->getDevelopmentSystem()) {
            NLM_LOG_INFO("Development System: ENABLED - stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
        } else {
            NLM_LOG_INFO("Development System: DISABLED");
        }
    }
    
    return true;
}

} // namespace nlm
