#pragma once

/**
 * Phase 3 Configuration
 * 
 * Configuration for Phase 3 neural development experiments
 */

#include <string>

namespace nlm {

struct Phase3Config {
    uint64_t maxSteps;
    size_t neuronCount;
    bool enableSensoryProcessing;
    bool enableMemoryFormation;
    bool enableEmotionGeneration;
    std::string outputFile;
    
    Phase3Config()
        : maxSteps(5000)
        , neuronCount(500)
        , enableSensoryProcessing(true)
        , enableMemoryFormation(true)
        , enableEmotionGeneration(false)
        , outputFile("") {}
};

} // namespace nlm
