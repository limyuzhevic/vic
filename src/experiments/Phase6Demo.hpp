#pragma once

#include <string>

namespace nlm {

/**
 * Forward declarations for Phase 6 demo
 */
class Phase6Demo;

/**
 * Enhanced Phase 6 demo options
 */
struct EnhancedDemoOptions {
    bool showHelp = false;
    bool showVersion = false;
    bool verbose = false;
    bool quiet = false;
    bool interactive = false;
    bool exportResults = false;
    bool importResults = false;
    std::string configFile;
    std::string outputFile;
    std::string logFile;
    std::string educationMode;
    std::string powerUserMode;
    std::string exportFormat;
    std::string importFormat;
    int maxSteps = 2000;
    int maxEpisodes = 5;
    float timeout = 0.0f;
    bool enableColors = true;
    bool enableProgress = true;
    bool enableLogging = true;
};

/**
 * Demo results structure
 */
struct DemoResult {
    float totalReward = 0.0f;
    float avgFiringRate = 0.0f;
    float avgSynapticWeight = 0.0f;
    float memoryEpisodesStored = 0.0f;
    float noveltyLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float dopamineLevel = 0.0f;
    double totalWallClockTime = 0.0;
    time_t startTime = 0;
    time_t endTime = 0;
    
    // Integration status
    bool memoryWorkingMemoryIntegrated = false;
    bool memoryEpisodicMemoryIntegrated = false;
    bool neuromodulationIntegrated = false;
    bool predictionIntegrated = false;
    bool developmentIntegrated = false;
    bool checkpointingWorks = false;
    bool replayWorks = false;
};

} // namespace nlm
