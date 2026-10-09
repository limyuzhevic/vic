// Debug CLI implementation for NLM
#include "cli/DebugCLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>

namespace nlm {

struct DebugCLI::Impl {
    bool stepByStepEnabled;
    bool paused;
    int verboseLevel;
    std::chrono::steady_clock::time_point stepStartTime;
    std::string currentDebugMode;
    std::vector<std::string> debugHistory;
    size_t maxHistorySize;
    bool autoStep;
    double stepInterval;
    bool showTimestamps;
    bool colorOutput;
    std::string outputFile;
    
    Impl() : stepByStepEnabled(false), paused(false), verboseLevel(2), 
             maxHistorySize(100), autoStep(false), stepInterval(0.1), 
             showTimestamps(true), colorOutput(true) {}
};

DebugCLI::DebugCLI() : initialized(false), config(nullptr), brain(nullptr), cli(nullptr),
    stepByStepMode(false), paused(false), verboseLevel(2) {
    pImpl = std::make_unique<Impl>();
}

DebugCLI::~DebugCLI() = default;

DebugCLI::DebugCLI(DebugCLI&&) noexcept = default;

DebugCLI& DebugCLI::operator=(DebugCLI&&) noexcept = default;

bool DebugCLI::initialize() {
    if (initialized) return true;
    
    try {
        if (!config || !brain) {
            printError("Config or brain not set");
            return false;
        }
        
        setupDebugConfiguration();
        updatePerformanceMetrics();
        
        initialized = true;
        return true;
    } catch (const std::exception& e) {
        printError("Failed to initialize DebugCLI: " + std::string(e.what()));
        return false;
    }
}

bool DebugCLI::verboseCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    int level = 2; // Default info level
    
    if (args.size() >= 2) {
        try {
            level = std::stoi(args[1]);
            if (level < 0 || level > 5) {
                printError("Verbose level must be between 0 and 5");
                return false;
            }
        } catch (...) {
            printError("Invalid verbose level: " + args[1]);
            return false;
        }
    }
    
    verboseLevel = level;
    
    std::string levelStr;
    switch (level) {
        case 0: levelStr = "quiet"; break;
        case 1: levelStr = "error"; break;
        case 2: levelStr = "info"; break;
        case 3: levelStr = "debug"; break;
        case 4: levelStr = "verbose"; break;
        case 5: levelStr = "trace"; break;
    }
    
    printSuccess("Verbose level set to " + levelStr + " (" + std::to_string(level) + ")");
    return true;
}

bool DebugCLI::stepCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    if (!brain) {
        printError("No brain available for stepping");
        return false;
    }
    
    SimulationStep step = 0;
    if (args.size() >= 2) {
        try {
            step = std::stoi(args[1]);
        } catch (...) {
            printError("Invalid step number: " + args[1]);
            return false;
        }
    }
    
    double dt = 0.001;
    if (args.size() >= 3) {
        try {
            dt = std::stod(args[2]);
        } catch (...) {
            printError("Invalid timestep: " + args[2]);
            return false;
        }
    }
    
    printDebugHeader("Step Execution");
    NLM_LOG_INFO("Executing step " + std::to_string(step) + " with timestep " + std::to_string(dt) + "s");
    
    brain->step(step, dt);
    
    printSuccess("Step " + std::to_string(step) + " completed");
    return true;
}

bool DebugCLI::profileCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    std::string profileType = "cpu";
    if (args.size() >= 2) {
        profileType = args[1];
    }
    
    if (profileType == "cpu") {
        double cpuUsage = calculateCPULoad();
        print("CPU Usage: " + std::to_string(cpuUsage) + "%")
            printPerformanceHeader()
            logPerformanceMetrics()
    } else if (profileType == "memory") {
        double memUsage = calculateMemoryUsage();
        print("Memory Usage: " + getMemoryUsageString(memUsage))
            printMemoryHeader()
            logPerformanceMetrics()
    } else if (profileType == "spike") {
        printSpikeHeader()
            logSpikeStatistics()
    } else if (profileType == "learning") {
        printLearningHeader()
            logLearningStatistics()
    } else if (profileType == "timing") {
        printTimingHeader()
            logPerformanceMetrics()
    } else {
        printError("Unknown profile type: " + profileType);
        return false;
    }
    
    return true;
}

bool DebugCLI::memoryCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    double memUsage = calculateMemoryUsage();
    double peakMemUsage = performanceMetrics.peakMemoryUsage;
    
    printMemoryHeader();
    print("Current Memory Usage: " + getMemoryUsageString(memUsage));
    print("Peak Memory Usage: " + getMemoryUsageString(peakMemUsage));
    print("Memory Efficiency: " + std::to_string((memUsage / peakMemUsage) * 100) + "%");
    
    return true;
}

bool DebugCLI::spikeCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    printSpikeHeader()
        logSpikeStatistics()
    
    return true;
}

bool DebugCLI::statsCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    printDebugHeader("Debug Statistics");
    
    updatePerformanceMetrics();
    
    std::string perfStats = formatPerformanceMetrics();
    print("=== Performance ===");
    print(perfStats);
    
    std::string spikeStats = formatSpikeStats();
    print("\n=== Spike Statistics ===");
    print(spikeStats);
    
    std::string learningStats = formatLearningStats();
    print("\n=== Learning Statistics ===");
    print(learningStats);
    
    std::string memStats = formatMemoryStats();
    print("\n=== Memory Statistics ===");
    print(memStats);
    
    std::string timingStats = formatTimingStats();
    print("\n=== Timing Statistics ===");
    print(timingStats);
    
    return true;
}

bool DebugCLI::logCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    std::string logFile = "debug.log";
    if (args.size() >= 2) {
        logFile = args[1];
    }
    
    std::ofstream logFileStream(logFile, std::ios::app);
    if (!logFileStream.is_open()) {
        printError("Cannot open log file: " + logFile);
        return false;
    }
    
    logFileStream << "=== Debug Log ===" << std::endl;
    logFileStream << "Timestamp: " << getFormattedTime() << std::endl;
    logFileStream << "Verbose Level: " << verboseLevel << std::endl;
    logFileStream << "Step by Step: " << (stepByStepMode ? "enabled" : "disabled") << std::endl;
    logFileStream << "Paused: " << (paused ? "yes" : "no") << std::endl;
    
    updatePerformanceMetrics();
    
    logFileStream << "Performance Metrics:" << std::endl;
    logFileStream << "  CPU Usage: " << performanceMetrics.cpuUsage << "%" << std::endl;
    logFileStream << "  Memory Usage: " << performanceMetrics.memoryUsage << " MB" << std::endl;
    logFileStream << "  Total Spikes: " << performanceMetrics.totalSpikes << std::endl;
    logFileStream << "  Active Neurons: " << performanceMetrics.activeNeuronCount << std::endl;
    logFileStream << "  Total Neurons: " << performanceMetrics.totalNeuronCount << std::endl;
    logFileStream << "  Total Synapses: " << performanceMetrics.totalSynapseCount << std::endl;
    logFileStream << "  Average Firing Rate: " << performanceMetrics.averageFiringRate << " Hz" << std::endl;
    logFileStream << "  Average Weight: " << performanceMetrics.averageWeight << std::endl;
    logFileStream << "  Weight Variance: " << performanceMetrics.weightVariance << std::endl;
    
    logFileStream.close();
    
    printSuccess("Debug log written to: " + logFile);
    return true;
}

bool DebugCLI::stackCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    printDebugHeader("Stack Analysis");
    
    if (brain) {
        auto* spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            print("Spike System Analysis:");
            print("  Active Neurons: " + std::to_string(spikeSystem->getActiveNeuronCount()));
            print("  Total Spike Count: " + std::to_string(spikeSystem->getTotalSpikeCount()));
            print("  Firing Rate: " + std::to_string(spikeSystem->getFiringRate()) + " Hz");
        }
    }
    
    print("Stack analysis completed.");
    return true;
}

bool DebugCLI::threadCommand(const std::vector<std::string>& args) {
    printDebugHeader("Thread Analysis");
    
    print("Thread Analysis:");
    print("  DebugCLI is running in the main thread.");
    print("  Performance metrics are updated in the main thread.");
    print("  All debug operations are executed synchronously.");
    
    return true;
}

bool DebugCLI::measureCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    std::string measurementType = "performance";
    if (args.size() >= 2) {
        measurementType = args[1];
    }
    
    updatePerformanceMetrics();
    
    printDebugHeader("Measurement Results");
    print("Measurement Type: " + measurementType);
    print("Timestamp: " + getFormattedTime());
    
    if (measurementType == "performance") {
        printPerformanceHeader();
        printPerformanceMetrics();
    } else if (measurementType == "memory") {
        printMemoryHeader();
        printMemoryMetrics();
    } else if (measurementType == "spike") {
        printSpikeHeader();
        printSpikeMetrics();
    } else if (measurementType == "learning") {
        printLearningHeader();
        printLearningMetrics();
    } else {
        printError("Unknown measurement type: " + measurementType);
        return false;
    }
    
    return true;
}

bool DebugCLI::stepByStepCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("DebugCLI not initialized");
        return false;
    }
    
    bool enable = args.size() >= 2 && (args[1] == "true" || args[1] == "1" || args[1] == "yes");
    
    if (enable) {
        startStepByStep();
        printSuccess("Step-by-step mode enabled");
    } else {
        stopStepByStep();
        printSuccess("Step-by-step mode disabled");
    }
    
    return true;
}

bool DebugCLI::continuousModeCommand(const std::vector<std::string>& args) {
    printDebugHeader("Continuous Mode");
    print("Continuous Mode:");
    print("  Step-by-step mode is " + std::string(stepByStepMode ? "enabled" : "disabled"));
    print("  Paused state is " + std::string(paused ? "true" : "false"));
    print("  Auto-step is " + std::string(pImpl->autoStep ? "enabled" : "disabled"));
    print("  Step interval: " + std::to_string(pImpl->stepInterval) + " seconds");
    
    return true;
}

bool DebugCLI::pauseCommand(const std::vector<std::string>& args) {
    pause();
    printSuccess("Debug execution paused");
    return true;
}

bool DebugCLI::resumeCommand(const std::vector<std::string>& args) {
    resume();
    printSuccess("Debug execution resumed");
    return true;
}

bool DebugCLI::stopCommand(const std::vector<std::string>& args) {
    stopStepByStep();
    pause();
    printSuccess("Debug execution stopped");
    return true;
}

bool DebugCLI::performanceCommand(const std::vector<std::string>& args) {
    updatePerformanceMetrics();
    
    printPerformanceHeader();
    printPerformanceMetrics();
    
    return true;
}

bool DebugCLI::cpuCommand(const std::vector<std::string>& args) {
    double cpuUsage = calculateCPULoad();
    
    printDebugHeader("CPU Usage");
    print("Current CPU Usage: " + getCPULoadString(cpuUsage));
    
    return true;
}

bool DebugCLI::memoryUsageCommand(const std::vector<std::string>& args) {
    double memUsage = calculateMemoryUsage();
    
    printDebugHeader("Memory Usage");
    print("Current Memory Usage: " + getMemoryUsageString(memUsage));
    
    return true;
}

bool DebugCLI::timingCommand(const std::vector<std::string>& args) {
    updatePerformanceMetrics();
    
    printDebugHeader("Timing");
    print("Last Update: " + getFormattedTimestamp(performanceMetrics.lastUpdate));
    print("Total Elapsed Time: " + std::to_string(getElapsedTime()) + " seconds");
    
    return true;
}

bool DebugCLI::spikeStatsCommand(const std::vector<std::string>& args) {
    printSpikeHeader();
    logSpikeStatistics();
    
    return true;
}

bool DebugCLI::learningStatsCommand(const std::vector<std::string>& args) {
    printLearningHeader();
    logLearningStatistics();
    
    return true;
}

bool DebugCLI::analyzeConnectionsCommand(const std::vector<std::string>& args) {
    printDebugHeader("Connection Analysis");
    
    if (!brain) {
        printError("No brain available for analysis");
        return false;
    }
    
    analyzeSynapticConnections();
    
    return true;
}

bool DebugCLI::analyzeWeightsCommand(const std::vector<std::string>& args) {
    printDebugHeader("Weight Analysis");
    
    if (!brain) {
        printError("No brain available for analysis");
        return false;
    }
    
    analyzeSynapticWeights();
    
    return true;
}

bool DebugCLI::analyzePlasticityCommand(const std::vector<std::string>& args) {
    printDebugHeader("Plasticity Analysis");
    
    if (!brain) {
        printError("No brain available for analysis");
        return false;
    }
    
    analyzePlasticityDynamics();
    
    return true;
}

bool DebugCLI::analyzeSpikingPatternsCommand(const std::vector<std::string>& args) {
    printDebugHeader("Spiking Pattern Analysis");
    
    if (!brain) {
        printError("No brain available for analysis");
        return false;
    }
    
    analyzeSpikingActivity();
    
    return true;
}

bool DebugCLI::analyzeSynapticDelaysCommand(const std::vector<std::string>& args) {
    printDebugHeader("Synaptic Delay Analysis");
    
    if (!brain) {
        printError("No brain available for analysis");
        return false;
    }
    
    analyzeSynapticDelays();
    
    return true;
}

void DebugCLI::showHelp() const {
    print("=== Debug CLI Commands ===");
    print("  verbose [level] - Set verbose level (0-5)");
    print("  step [step] [dt] - Execute simulation step");
    print("  profile [type] - Profile performance");
    print("  memory - Show memory usage");
    print("  spike - Show spike statistics");
    print("  stats - Show debug statistics");
    print("  log [filename] - Log debug information");
    print("  stack - Analyze stack");
    print("  thread - Analyze threads");
    print("  measure [type] - Measure metrics");
    print("  step-by-step [true|false] - Enable/disable step-by-step mode");
    print("  continuous - Show continuous mode status");
    print("  pause - Pause execution");
    print("  resume - Resume execution");
    print("  stop - Stop debug execution");
    print("  performance - Show performance metrics");
    print("  cpu - Show CPU usage");
    print("  memory-usage - Show memory usage");
    print("  timing - Show timing information");
    print("  spike-stats - Show spike statistics");
    print("  learning-stats - Show learning statistics");
    print("  analyze connections - Analyze synaptic connections");
    print("  analyze weights - Analyze synaptic weights");
    print("  analyze plasticity - Analyze plasticity dynamics");
    print("  analyze spiking - Analyze spiking activity");
    print("  analyze delays - Analyze synaptic delays");
}

void DebugCLI::showCommandHelp(const std::string& command) const {
    std::map<std::string, std::string> helpTexts = {
        {"verbose", "Set verbose level\nUsage: verbose [level]"},
        {"step", "Execute simulation step\nUsage: step [step] [dt]"},
        {"profile", "Profile performance\nUsage: profile [type]"},
        {"memory", "Show memory usage\nUsage: memory"},
        {"spike", "Show spike statistics\nUsage: spike"},
        {"stats", "Show debug statistics\nUsage: stats"},
        {"log", "Log debug information\nUsage: log [filename]"},
        {"stack", "Analyze stack\nUsage: stack"},
        {"thread", "Analyze threads\nUsage: thread"},
        {"measure", "Measure metrics\nUsage: measure [type]"},
        {"step-by-step", "Enable/disable step-by-step mode\nUsage: step-by-step [true|false]"},
        {"continuous", "Show continuous mode status\nUsage: continuous"},
        {"pause", "Pause execution\nUsage: pause"},
        {"resume", "Resume execution\nUsage: resume"},
        {"stop", "Stop debug execution\nUsage: stop"},
        {"performance", "Show performance metrics\nUsage: performance"},
        {"cpu", "Show CPU usage\nUsage: cpu"},
        {"memory-usage", "Show memory usage\nUsage: memory-usage"},
        {"timing", "Show timing information\nUsage: timing"},
        {"spike-stats", "Show spike statistics\nUsage: spike-stats"},
        {"learning-stats", "Show learning statistics\nUsage: learning-stats"},
        {"analyze connections", "Analyze synaptic connections\nUsage: analyze connections"},
        {"analyze weights", "Analyze synaptic weights\nUsage: analyze weights"},
        {"analyze plasticity", "Analyze plasticity dynamics\nUsage: analyze plasticity"},
        {"analyze spiking", "Analyze spiking activity\nUsage: analyze spiking"},
        {"analyze delays", "Analyze synaptic delays\nUsage: analyze delays"}
    };
    
    auto it = helpTexts.find(command);
    if (it != helpTexts.end()) {
        print(it->second);
    } else {
        printError("No help available for command: " + command);
    }
}

void DebugCLI::updatePerformanceMetrics() {
    performanceMetrics.cpuUsage = calculateCPULoad();
    performanceMetrics.memoryUsage = calculateMemoryUsage();
    performanceMetrics.peakMemoryUsage = std::max(performanceMetrics.peakMemoryUsage, performanceMetrics.memoryUsage);
    
    if (brain) {
        performanceMetrics.totalSpikes = brain->getTotalSpikeCount();
        performanceMetrics.activeNeuronCount = brain->getActiveNeuronCount();
        performanceMetrics.totalNeuronCount = brain->getTotalNeuronCount();
        performanceMetrics.totalSynapseCount = brain->getTotalSynapseCount();
        performanceMetrics.averageFiringRate = brain->getFiringRate();
        
        if (brain->getTotalSynapseCount() > 0) {
            performanceMetrics.averageWeight = brain->getAverageSynapticWeight();
        }
    }
    
    performanceMetrics.lastUpdate = std::chrono::steady_clock::now();
}

void DebugCLI::logPerformanceMetrics() const {
    print("=== Performance Metrics ===");
    print("CPU Usage: " + getCPULoadString(performanceMetrics.cpuUsage));
    print("Memory Usage: " + getMemoryUsageString(performanceMetrics.memoryUsage));
    print("Peak Memory Usage: " + getMemoryUsageString(performanceMetrics.peakMemoryUsage));
    print("Total Spikes: " + std::to_string(performanceMetrics.totalSpikes));
    print("Active Neurons: " + std::to_string(performanceMetrics.activeNeuronCount));
    print("Total Neurons: " + std::to_string(performanceMetrics.totalNeuronCount));
    print("Total Synapses: " + std::to_string(performanceMetrics.totalSynapseCount));
    print("Average Firing Rate: " + std::to_string(performanceMetrics.averageFiringRate) + " Hz");
    print("Average Weight: " + std::to_string(performanceMetrics.averageWeight));
}

void DebugCLI::logSpikeStatistics() const {
    print("=== Spike Statistics ===");
    
    if (brain) {
        print("Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        print("Active Neurons: " + std::to_string(brain->getActiveNeuronCount()));
        print("Firing Rate: " + std::to_string(brain->getFiringRate()) + " Hz");
        
        auto mostActive = brain->getSpikeSystem()->getMostActiveNeurons(5);
        print("Most Active Neurons:");
        for (size_t i = 0; i < mostActive.size(); ++i) {
            print("  " + std::to_string(mostActive[i]));
        }
    }
}

void DebugCLI::logLearningStatistics() const {
    print("=== Learning Statistics ===");
    
    if (brain) {
        print("Total Spike Count: " + std::to_string(brain->getTotalSpikeCount()));
        print("Plasticity Enabled: " + std::to_string(brain->isPlasticityEnabled()));
        print("STDP Enabled: " + std::to_string(brain->isSTDPEnabled()));
        
        auto learningStats = brain->getLearningStatistics();
        print("Learning Statistics:");
        for (const auto& stat : learningStats) {
            print("  " + stat);
        }
    }
}

void DebugCLI::analyzeSynapticConnections() const {
    print("Analyzing synaptic connections...");
    
    if (!brain) {
        printError("No brain available for analysis");
        return;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        printError("No region found for analysis");
        return;
    }
    
    print("Connection Analysis:");
    print("  Total Synapses: " + std::to_string(region->getSynapseCount()));
    print("  Average Weight: " + std::to_string(region->getAverageSynapticWeight()));
    print("  Connection Probability: " + std::to_string(region->getConnectionProbability()));
}

void DebugCLI::analyzeSynapticWeights() const {
    print("Analyzing synaptic weights...");
    
    if (!brain) {
        printError("No brain available for analysis");
        return;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        printError("No region found for analysis");
        return;
    }
    
    auto weights = region->getAllSynapticWeights();
    if (weights.empty()) {
        print("No synaptic weights found");
        return;
    }
    
    double minWeight = *std::min_element(weights.begin(), weights.end());
    double maxWeight = *std::max_element(weights.begin(), weights.end());
    double avgWeight = std::accumulate(weights.begin(), weights.end(), 0.0) / weights.size();
    
    print("Weight Analysis:");
    print("  Min Weight: " + std::to_string(minWeight));
    print("  Max Weight: " + std::to_string(maxWeight));
    print("  Average Weight: " + std::to_string(avgWeight));
    
    size_t strengthened = 0;
    size_t weakened = 0;
    size_t unchanged = 0;
    
    for (const auto& weight : weights) {
        if (weight > 0.5) strengthened++;
        else if (weight < 0.2) weakened++;
        else unchanged++;
    }
    
    print("  Strengthened Synapses (>0.5): " + std::to_string(strengthened));
    print("  Weakened Synapses (<0.2): " + std::to_string(weakened));
    print("  Unchanged Synapses: " + std::to_string(unchanged));
}

void DebugCLI::analyzePlasticityDynamics() const {
    print("Analyzing plasticity dynamics...");
    
    if (!brain) {
        printError("No brain available for analysis");
        return;
    }
    
    auto plasticityStats = brain->getPlasticityStatistics();
    print("Plasticity Statistics:");
    for (const auto& stat : plasticityStats) {
        print("  " + stat);
    }
}

void DebugCLI::analyzeSpikingActivity() const {
    print("Analyzing spiking activity...");
    
    if (!brain) {
        printError("No brain available for analysis");
        return;
    }
    
    auto spikeHistory = brain->getSpikeHistory();
    if (spikeHistory.empty()) {
        print("No spike history available");
        return;
    }
    
    print("Spiking Activity Analysis:");
    print("  Total Spike Count: " + std::to_string(spikeHistory.size()));
    
    size_t maxSpikes = *std::max_element(spikeHistory.begin(), spikeHistory.end());
    print("  Maximum Spikes in Time Window: " + std::to_string(maxSpikes));
}

void DebugCLI::analyzeSynapticDelays() const {
    print("Analyzing synaptic delays...");
    
    if (!brain) {
        printError("No brain available for analysis");
        return;
    }
    
    auto delayStats = brain->getSynapticDelayStatistics();
    print("Synaptic Delay Statistics:");
    for (const auto& stat : delayStats) {
        print("  " + stat);
    }
}

bool DebugCLI::runStepByStep(CLI* cli, const std::vector<std::string>& args) {
    if (!initialized || !brain) {
        return false;
    }
    
    SimulationStep step = 0;
    double dt = 0.001;
    
    if (args.size() >= 2) {
        try {
            step = std::stoi(args[1]);
        } catch (...) {}
    }
    
    if (args.size() >= 3) {
        try {
            dt = std::stod(args[2]);
        } catch (...) {}
    }
    
    printDebugHeader("Step-by-Step Execution");
    NLM_LOG_INFO("Step " + std::to_string(step) + " with timestep " + std::to_string(dt) + "s");
    
    brain->step(step, dt);
    
    pImpl->stepStartTime = std::chrono::steady_clock::now();
    
    return true;
}

std::string DebugCLI::formatPerformanceMetrics() const {
    std::ostringstream oss;
    oss << "Performance Metrics (" << getFormattedTimestamp(performanceMetrics.lastUpdate) << "):" << std::endl;
    oss << "  CPU Usage: " << performanceMetrics.cpuUsage << "%" << std::endl;
    oss << "  Memory Usage: " << performanceMetrics.memoryUsage << " MB" << std::endl;
    oss << "  Total Spikes: " << performanceMetrics.totalSpikes << std::endl;
    oss << "  Active Neurons: " << performanceMetrics.activeNeuronCount << std::endl;
    oss << "  Total Neurons: " << performanceMetrics.totalNeuronCount << std::endl;
    oss << "  Total Synapses: " << performanceMetrics.totalSynapseCount << std::endl;
    oss << "  Average Firing Rate: " << performanceMetrics.averageFiringRate << " Hz" << std::endl;
    oss << "  Average Weight: " << performanceMetrics.averageWeight << std::endl;
    return oss.str();
}

std::string DebugCLI::formatSpikeStats() const {
    std::ostringstream oss;
    oss << "Spike Statistics:" << std::endl;
    
    if (brain) {
        oss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        oss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        oss << "  Firing Rate: " << brain->getFiringRate() << " Hz" << std::endl;
    }
    
    return oss.str();
}

std::string DebugCLI::formatLearningStats() const {
    std::ostringstream oss;
    oss << "Learning Statistics:" << std::endl;
    
    if (brain) {
        oss << "  Plasticity Enabled: " << (brain->isPlasticityEnabled() ? "yes" : "no") << std::endl;
        oss << "  STDP Enabled: " << (brain->isSTDPEnabled() ? "yes" : "no") << std::endl;
        
        auto learningStats = brain->getLearningStatistics();
        for (const auto& stat : learningStats) {
            oss << "  " << stat << std::endl;
        }
    }
    
    return oss.str();
}

std::string DebugCLI::formatMemoryStats() const {
    std::ostringstream oss;
    oss << "Memory Statistics:" << std::endl;
    oss << "  Current Memory Usage: " << getMemoryUsageString(performanceMetrics.memoryUsage) << std::endl;
    oss << "  Peak Memory Usage: " << getMemoryUsageString(performanceMetrics.peakMemoryUsage) << std::endl;
    oss << "  Memory Efficiency: " << (performanceMetrics.memoryUsage / performanceMetrics.peakMemoryUsage * 100) << "%" << std::endl;
    
    return oss.str();
}

std::string DebugCLI::formatTimingStats() const {
    std::ostringstream oss;
    oss << "Timing Statistics:" << std::endl;
    oss << "  Last Update: " << getFormattedTimestamp(performanceMetrics.lastUpdate) << std::endl;
    oss << "  Total Elapsed Time: " << getElapsedTime() << " seconds" << std::endl;
    oss << "  Step-by-Step Mode: " << (stepByStepMode ? "enabled" : "disabled") << std::endl;
    oss << "  Paused: " << (paused ? "yes" : "no") << std::endl;
    
    return oss.str();
}

void DebugCLI::printDebugHeader(const std::string& title) const {
    print("=== " + title + " ===");
}

void DebugCLI::printPerformanceHeader() const {
    print("=== Performance Metrics ===");
}

void DebugCLI::printSpikeHeader() const {
    print("=== Spike Statistics ===");
}

void DebugCLI::printLearningHeader() const {
    print("=== Learning Statistics ===");
}

void DebugCLI::printMemoryHeader() const {
    print("=== Memory Usage ===");
}

void DebugCLI::printTimingHeader() const {
    print("=== Timing Information ===");
}

void DebugCLI::setupDebugConfiguration() const {
    verboseLevel = config->getOr<int>("debug.verbose_level", 2);
    stepByStepMode = config->getOr<bool>("debug.step_by_step", false);
    paused = config->getOr<bool>("debug.paused", false);
    pImpl->autoStep = config->getOr<bool>("debug.auto_step", false);
    pImpl->stepInterval = config->getOr<double>("debug.step_interval", 0.1);
    pImpl->showTimestamps = config->getOr<bool>("debug.show_timestamps", true);
    pImpl->colorOutput = config->getOr<bool>("debug.color_output", true);
}

std::string DebugCLI::getFormattedTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string DebugCLI::getMemoryUsageString() const {
    return getMemoryUsageString(calculateMemoryUsage());
}

std::string DebugCLI::getCPULoadString() const {
    return getCPULoadString(calculateCPULoad());
}

double DebugCLI::calculateCPULoad() const {
    return 0.0;
}

double DebugCLI::calculateMemoryUsage() const {
    return 0.0;
}

void DebugCLI::logPerformanceMetrics() const {
    std::cout << "=== Performance Metrics ===" << std::endl;
    std::cout << "  CPU Usage: " << performanceMetrics.cpuUsage << "%" << std::endl;
    std::cout << "  Memory Usage: " << performanceMetrics.memoryUsage << " MB" << std::endl;
    std::cout << "  Peak Memory Usage: " << performanceMetrics.peakMemoryUsage << " MB" << std::endl;
    std::cout << "  Total Spikes: " << performanceMetrics.totalSpikes << std::endl;
    std::cout << "  Active Neurons: " << performanceMetrics.activeNeuronCount << std::endl;
    std::cout << "  Total Neurons: " << performanceMetrics.totalNeuronCount << std::endl;
    std::cout << "  Total Synapses: " << performanceMetrics.totalSynapseCount << std::endl;
    std::cout << "  Average Firing Rate: " << performanceMetrics.averageFiringRate << " Hz" << std::endl;
    std::cout << "  Average Weight: " << performanceMetrics.averageWeight << std::endl;
}

void DebugCLI::logSpikeStatistics() const {
    std::cout << "=== Spike Statistics ===" << std::endl;
    
    if (brain) {
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing Rate: " << brain->getFiringRate() << " Hz" << std::endl;
        
        auto mostActive = brain->getSpikeSystem()->getMostActiveNeurons(5);
        std::cout << "  Most Active Neurons:" << std::endl;
        for (size_t i = 0; i < mostActive.size(); ++i) {
            std::cout << "    " << mostActive[i] << std::endl;
        }
    }
}

void DebugCLI::logLearningStatistics() const {
    std::cout << "=== Learning Statistics ===" << std::endl;
    
    if (brain) {
        std::cout << "  Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Plasticity Enabled: " << (brain->isPlasticityEnabled() ? "yes" : "no") << std::endl;
        std::cout << "  STDP Enabled: " << (brain->isSTDPEnabled() ? "yes" : "no") << std::endl;
        
        auto learningStats = brain->getLearningStatistics();
        std::cout << "  Learning Statistics:" << std::endl;
        for (const auto& stat : learningStats) {
            std::cout << "    " << stat << std::endl;
        }
    }
}

std::string DebugCLI::getMemoryUsageString(double memoryMB) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << memoryMB << " MB";
    return oss.str();
}

std::string DebugCLI::getCPULoadString(double cpuPercent) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << cpuPercent << "%";
    return oss.str();
}

void DebugCLI::printMemoryMetrics() const {
    double memUsage = calculateMemoryUsage();
    double peakMemUsage = performanceMetrics.peakMemoryUsage;
    
    std::cout << "Current Memory Usage: " << getMemoryUsageString(memUsage) << std::endl;
    std::cout << "Peak Memory Usage: " << getMemoryUsageString(peakMemUsage) << std::endl;
    std::cout << "Memory Efficiency: " << (memUsage / peakMemUsage * 100) << "%" << std::endl;
}

void DebugCLI::printSpikeMetrics() const {
    if (brain) {
        std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing Rate: " << brain->getFiringRate() << " Hz" << std::endl;
        
        auto mostActive = brain->getSpikeSystem()->getMostActiveNeurons(5);
        std::cout << "Most Active Neurons:" << std::endl;
        for (size_t i = 0; i < mostActive.size(); ++i) {
            std::cout << "  " << mostActive[i] << std::endl;
        }
    }
}

void DebugCLI::printLearningMetrics() const {
    if (brain) {
        std::cout << "Plasticity Enabled: " << (brain->isPlasticityEnabled() ? "yes" : "no") << std::endl;
        std::cout << "STDP Enabled: " << (brain->isSTDPEnabled() ? "yes" : "no") << std::endl;
        
        auto learningStats = brain->getLearningStatistics();
        std::cout << "Learning Statistics:" << std::endl;
        for (const auto& stat : learningStats) {
            std::cout << "  " << stat << std::endl;
        }
    }
}
