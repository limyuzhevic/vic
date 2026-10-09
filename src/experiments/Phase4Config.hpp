#pragma once

/**
 * Phase 4 Configuration
 * 
 * Configuration for Phase 4 cognitive development experiments
 */

#include <string>

namespace nlm {

struct Phase4Config {
    uint64_t maxSteps;
    size_t neuronCount;
    bool enableWorkingMemory;
    bool enableEpisodicMemory;
    bool enableConceptFormation;
    bool enableAttention;
    bool enablePlanning;
    bool enableSocialLearning;
    bool enableGeneralization;
    std::string outputFile;
    
    Phase4Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , enableWorkingMemory(true)
        , enableEpisodicMemory(true)
        , enableConceptFormation(true)
        , enableAttention(true)
        , enablePlanning(true)
        , enableSocialLearning(true)
        , enableGeneralization(true)
        , outputFile("") {}
};

} // namespace nlm
