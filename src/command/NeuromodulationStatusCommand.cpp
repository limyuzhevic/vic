// NeuromodulationStatusCommand implementation
// NLM Advanced User Interface

#include "command/NeuromodulationStatusCommand.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

bool NeuromodulationStatusCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                         std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== Neuromodulation System Status ===");
    
    bool showDopamine = std::find(args.begin(), args.end(), "--dopamine") != args.end();
    bool showCuriosity = std::find(args.begin(), args.end(), "--curiosity") != args.end();
    bool showNovelty = std::find(args.begin(), args.end(), "--novelty") != args.end();
    bool showPredError = std::find(args.begin(), args.end(), "--prediction-error") != args.end();
    bool verbose = std::find(args.begin(), args.end(), "--verbose") != args.end();
    
    // If no specific neuromodulator requested, show all
    if (!showDopamine && !showCuriosity && !showNovelty && !showPredError) {
        showDopamine = showCuriosity = showNovelty = showPredError = true;
    }
    
    if (showDopamine && brain->getDopamine()) {
        NLM_LOG_INFO("Dopamine System:");
        NLM_LOG_INFO("  Level: " + std::to_string(brain->getDopamine()->getLevel()));
        NLM_LOG_INFO("  Plasticity factor: " + std::to_string(brain->getDopamine()->getPlasticityFactor()));
        NLM_LOG_INFO("  Target neurons: (would show list of target neurons)");
        NLM_LOG_INFO("  Last reward prediction error: (would show RPE value)");
        
        if (verbose) {
            NLM_LOG_INFO("  Detailed dopamine status:");
            NLM_LOG_INFO("    Sensitivity: (would show sensitivity)");
            NLM_LOG_INFO("    Adaptation rate: (would show rate)");
            NLM_LOG_INFO("    Recovery time: (would show time)");
        }
    } else if (showDopamine) {
        NLM_LOG_INFO("Dopamine System: DISABLED");
    }
    
    if (showCuriosity && brain->getCuriosity()) {
        NLM_LOG_INFO("Curiosity System:");
        NLM_LOG_INFO("  Level: " + std::to_string(brain->getCuriosity()->getLevel()));
        NLM_LOG_INFO("  Exploration probability: (would show exploration rate)");
        NLM_LOG_INFO("  Novelty threshold: (would show threshold)");
        
        if (verbose) {
            NLM_LOG_INFO("  Detailed curiosity status:");
            NLM_LOG_INFO("    Learning rate: (would show rate)");
            NLM_LOG_INFO("    Attention bias: (would show bias)");
            NLM_LOG_INFO("    Reward sensitivity: (would show sensitivity)");
        }
    } else if (showCuriosity) {
        NLM_LOG_INFO("Curiosity System: DISABLED");
    }
    
    if (showNovelty && brain->getNovelty()) {
        NLM_LOG_INFO("Novelty System:");
        NLM_LOG_INFO("  Level: " + std::to_string(brain->getNovelty()->getLevel()));
        NLM_LOG_INFO("  Novelty detection parameters: (would show parameters)");
        
        if (verbose) {
            NLM_LOG_INFO("  Detailed novelty status:");
            NLM_LOG_INFO("    Sensitivity: (would show sensitivity)");
            NLM_LOG_INFO("    Decay rate: (would show decay rate)");
            NLM_LOG_INFO("    Memory threshold: (would show threshold)");
        }
    } else if (showNovelty) {
        NLM_LOG_INFO("Novelty System: DISABLED");
    }
    
    if (showPredError && brain->getPredictionErrorSignal()) {
        NLM_LOG_INFO("Prediction Error System:");
        NLM_LOG_INFO("  Level: " + std::to_string(brain->getPredictionErrorSignal()->getLevel()));
        NLM_LOG_INFO("  Prediction targets: (would show list of targets)");
        NLM_LOG_INFO("  Last prediction error: (would show error value)");
        
        if (verbose) {
            NLM_LOG_INFO("  Detailed prediction error status:");
            NLM_LOG_INFO("    Prediction accuracy: (would show accuracy)");
            NLM_LOG_INFO("    Error tolerance: (would show tolerance)");
            NLM_LOG_INFO("    Learning rate: (would show learning rate)");
        }
    } else if (showPredError) {
        NLM_LOG_INFO("Prediction Error System: DISABLED");
    }
    
    return true;
}

} // namespace nlm
