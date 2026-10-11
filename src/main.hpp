#pragma once

#include <string>
#include <memory>
#include <iostream>

// Forward declarations
namespace nlm {
    class Logger;
    class ConsoleLogger;
    class Phase6IntegratedExperiment;
    
    // Log levels
    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    
    // Phase 6 experiment configuration
    struct Phase6Config {
        uint64_t maxSteps = 10000;
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        bool enableCheckpointing = true;
        bool enableReplay = true;
        bool enableDevelopment = true;
        std::string checkpointPath = "./checkpoint_test.bin";
    };
    
    // Phase 6 integration result
    struct Phase6IntegrationResult {
        bool memoryWorkingMemoryIntegrated = false;
        bool memoryEpisodicMemoryIntegrated = false;
        bool neuromodulationIntegrated = false;
        bool predictionIntegrated = false;
        bool developmentIntegrated = false;
        bool checkpointingWorks = false;
        bool replayWorks = false;
        float totalReward = 0.0f;
        float avgFiringRate = 0.0f;
        float avgSynapticWeight = 0.0f;
        float memoryEpisodesStored = 0.0f;
        float noveltyLevel = 0.0f;
        float curiosityLevel = 0.0f;
        float dopamineLevel = 0.0f;
        time_t startTime = 0;
        time_t endTime = 0;
        double totalWallClockTime = 0.0;
    };
}

// Convenience macros for logging
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)