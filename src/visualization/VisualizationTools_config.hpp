// VisualizationTools_config.hpp
#pragma once

#include <string>
#include <map>

// Configuration structure for VisualizationTools
struct VisualizationToolsConfig {
    std::string outputDirectory = "./visualization_output";
    double updateRate = 60.0;           // Hz
    size_t maxHistorySize = 10000;
    int compressionLevel = 6;           // 0-9
    bool enableMultiThreading = true;
    size_t threadCount = 0;             // 0 = auto-detect
    bool enableResourceOptimization = true;
    size_t memoryLimit = 1024 * 1024 * 1024; // 1GB
    bool enableVR = false;
    bool enable3D = false;
    bool enableWebGL = true;
    bool enableAnimations = false;
    std::map<std::string, std::string> customSettings;
};