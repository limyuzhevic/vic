#pragma once

#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

namespace nlm {

// Enhanced configuration management commands
namespace ConfigCommands {
    bool configList(const Config& config, const std::vector<std::string>& args);
    bool configExport(const Config& config, const std::vector<std::string>& args);
    bool configImport(const Config& config, const std::vector<std::string>& args);
    bool configValidate(const Config& config, const std::vector<std::string>& args);
    bool configCompare(const Config& config, const std::vector<std::string>& args);
    bool configInfo(const Config& config, const std::vector<std::string>& args);
}

// Performance profiling and benchmarking
namespace BenchmarkCommands {
    bool benchmarkRun(const Config& config, const std::vector<std::string>& args);
    bool benchmarkCompare(const Config& config, const std::vector<std::string>& args);
    bool benchmarkProfile(const Config& config, const std::vector<std::string>& args);
    bool benchmarkStats(const Config& config, const std::vector<std::string>& args);
}

// Advanced debugging and monitoring
namespace DebugCommands {
    bool debugMonitor(const Config& config, const std::vector<std::string>& args);
    bool debugTrace(const Config& config, const std::vector<std::string>& args);
    bool debugProfile(const Config& config, const std::vector<std::string>& args);
    bool debugAnalyze(const Config& config, const std::vector<std::string>& args);
    bool debugInspect(const Config& config, const std::vector<std::string>& args);
}

// Memory system inspection
namespace MemoryCommands {
    bool memoryInspect(const Config& config, const std::vector<std::string>& args);
    bool memoryStats(const Config& config, const std::vector<std::string>& args);
    bool memoryCompare(const Config& config, const std::vector<std::string>& args);
    bool memoryBenchmark(const Config& config, const std::vector<std::string>& args);
}

// Neuromodulation analysis
namespace NeuromodulationCommands {
    bool neuromodulationAnalyze(const Config& config, const std::vector<std::string>& args);
    bool neuromodulationProfile(const Config& config, const std::vector<std::string>& args);
    bool neuromodulationStats(const Config& config, const std::vector<std::string>& args);
    bool neuromodulationTrace(const Config& config, const std::vector<std::string>& args);
}

// Development stage management
namespace DevelopmentCommands {
    bool developmentStatus(const Config& config, const std::vector<std::string>& args);
    bool developmentStage(const Config& config, const std::vector<std::string>& args);
    bool developmentRun(const Config& config, const std::vector<std::string>& args);
    bool developmentProgress(const Config& config, const std::vector<std::string>& args);
}

// Experiment management
namespace ExperimentCommands {
    bool experimentList(const Config& config, const std::vector<std::string>& args);
    bool experimentRun(const Config& config, const std::vector<std::string>& args);
    bool experimentRunAll(const Config& config, const std::vector<std::string>& args);
    bool experimentStatus(const Config& config, const std::vector<std::string>& args);
    bool experimentSave(const Config& config, const std::vector<std::string>& args);
    bool experimentCompare(const Config& config, const std::vector<std::string>& args);
    bool experimentAnalyze(const Config& config, const std::vector<std::string>& args);
}

// Visualization and output generation
namespace VisualizationCommands {
    bool visualizationGenerate(const Config& config, const std::vector<std::string>& args);
    bool visualizationExport(const Config& config, const std::vector<std::string>& args);
    bool visualizationStats(const Config& config, const std::vector<std::string>& args);
    bool visualizationAnimate(const Config& config, const std::vector<std::string>& args);
}

} // namespace nlm
