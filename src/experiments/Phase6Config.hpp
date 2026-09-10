#pragma once

/**
 * Phase 6 Configuration
 * 
 * Configuration structure for Phase 6 integration experiment
 * Used by Phase6Demo.cpp and Phase6IntegratedExperiment.cpp
 */

namespace nlm {

struct Phase6Config {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin") {}
};

} // namespace nlm
