// Phase selector implementation for NLM
// Provides unified interface for selecting and executing different phases

#include "PhaseSelector.hpp"
#include <iostream>
#include <string>
#include <algorithm>

namespace nlm {

Phase PhaseSelector::parsePhase(const std::string& phaseStr) {
    std::string phase = phaseStr;
    std::transform(phase.begin(), phase.end(), phase.begin(), ::tolower);
    
    if (phase == "1" || phase == "phase1" || phase == "basic") {
        return Phase1;
    } else if (phase == "2" || phase == "phase2" || phase == "core") {
        return Phase2;
    } else if (phase == "3" || phase == "phase3" || phase == "world") {
        return Phase3;
    } else if (phase == "4" || phase == "phase4" || phase == "cognition") {
        return Phase4;
    } else if (phase == "5" || phase == "phase5" || phase == "personality") {
        return Phase5;
    } else if (phase == "6" || phase == "phase6" || phase == "integration") {
        return Phase6;
    } else if (phase == "benchmark" || phase == "bench") {
        return Benchmark;
    } else if (phase == "all") {
        return All;
    } else {
        return Phase2; // Default
    }
}

Phase PhaseSelector::getCurrentPhase() const {
    return currentPhase;
}

void PhaseSelector::setCurrentPhase(Phase phase) {
    currentPhase = phase;
}

std::string PhaseSelector::phaseToString(Phase phase) {
    switch (phase) {
        case Phase1:
            return "Phase 1 - Basic";
        case Phase2:
            return "Phase 2 - Core Neural Computation";
        case Phase3:
            return "Phase 3 - World Interaction";
        case Phase4:
            return "Phase 4 - Emerging Cognition";
        case Phase5:
            return "Phase 5 - Brain-Personality";
        case Phase6:
            return "Phase 6 - Final Integration";
        case Benchmark:
            return "Benchmark";
        case All:
            return "All Phases";
        default:
            return "Unknown";
    }
}

void PhaseSelector::executePhase(Phase phase, const CLIResult& cliResult) {
    currentPhase = phase;
    std::cout << "Executing " << phaseToString(phase) << std::endl;
    
    // Implementation would depend on the specific phase
    // This is a framework that allows easy extension
    switch (phase) {
        case Phase2:
            std::cout << "Running Phase 2: Real Neural Computation" << std::endl;
            break;
        case Phase3:
            std::cout << "Running Phase 3: World Interaction Demo" << std::endl;
            // Parse arguments for Phase 3
            int episodes = 5;
            int stepsPerEpisode = 200;
            
            if (!cliResult.positionalArgs.empty()) {
                try {
                    episodes = std::stoi(cliResult.positionalArgs[0]);
                } catch (...) {}
            }
            if (cliResult.positionalArgs.size() > 1) {
                try {
                    stepsPerEpisode = std::stoi(cliResult.positionalArgs[1]);
                } catch (...) {}
            }
            
            std::cout << "  Episodes: " << episodes << ", Steps per episode: " << stepsPerEpisode << std::endl;
            break;
        case Phase4:
            std::cout << "Running Phase 4: Emerging Cognition" << std::endl;
            // Parse trials argument
            int trials = 50;
            if (!cliResult.positionalArgs.empty()) {
                try {
                    trials = std::stoi(cliResult.positionalArgs[0]);
                } catch (...) {}
            }
            std::cout << "  Trials: " << trials << std::endl;
            break;
        case Phase6:
            std::cout << "Running Phase 6: Integration Demo" << std::endl;
            break;
        default:
            std::cout << "Phase not yet implemented" << std::endl;
            break;
    }
}

} // namespace nlm
