#pragma once

#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <optional>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <thread>
#include <algorithm>
#include <map>
#include <cstdint>
#include <limits>

namespace nlm {

// Debug tools for NLM
class DebugTools {
public:
    // Memory debugging
    static void logMemoryUsage(const std::string& category = "MemoryDebug");
    static uint64_t getCurrentMemoryUsage();
    static uint64_t getPeakMemoryUsage();
    
    // Performance monitoring
    static void startPerformanceTimer(const std::string& name);
    static void stopPerformanceTimer(const std::string& name);
    static double getPerformanceTime(const std::string& name);
    static void printPerformanceReport(const std::string& category = "PerformanceReport");
    static void resetPerformanceTimers();
    
    // Event tracking
    static void trackEvent(const std::string& event, const std::string& details = "");
    static void printEventLog(const std::string& category = "EventLog");
    static void clearEventLog();
    
    // System health checks
    static bool checkSystemHealth();
    static std::string getSystemHealthReport();
    
    // Configuration validation
    static bool validateConfiguration();
    static std::string getConfigurationValidationReport();
    
    // Connection tracking
    static void trackConnection(const std::string& source, const std::string& target, float weight);
    static void untrackConnection(const std::string& source, const std::string& target);
    static void printConnectionMap(const std::string& category = "ConnectionMap");
    
    // Neuron and synapse debugging
    static void logNeuronDetails(const NeuronId& neuronId);
    static void logSynapseDetails(const SynapseId& synapseId);
    static void logRegionDetails(const RegionId& regionId);
    static void logPopulationDetails(const PopulationId& populationId);
    
    // Spike debugging
    static void logSpikeHistory(size_t maxEvents = 100);
    static void logDelayedSpikeHistory(size_t maxEvents = 100);
    
    // Memory system debugging
    static void logWorkingMemoryState();
    static void logEpisodicMemoryState();
    static void logAssociativeMemoryState();
    
    // Neuromodulation debugging
    static void logDopamineState();
    static void logCuriosityState();
    static void logNoveltyState();
    static void logPredictionErrorState();
    
    // Event-driven debugging
    static void logEventDrivenState();
    static void logPlasticityState();
    static void logStructuralPlasticityState();
    
    // Development debugging
    static void logDevelopmentState();
    
    // Performance debugging
    static void logPerformanceMetrics();
    static void logStepPerformance();
    
    // Debugging helpers
    static std::string formatMemorySize(uint64_t bytes);
    static std::string formatTime(double seconds);
    static std::string formatPercentage(float value);
    
    // Debug output management
    static void setDebugOutput(std::ostream& output);
    static void enableDebugOutput(bool enable);
    static bool isDebugOutputEnabled();
    
    // Debug file output
    static void setDebugFile(const std::string& filepath);
    static void closeDebugFile();
    
    // Debug log levels
    enum DebugLevel {
        DEBUG_LEVEL_SILENT = -1,
        DEBUG_LEVEL_ERROR = 0,
        DEBUG_LEVEL_WARNING = 1,
        DEBUG_LEVEL_INFO = 2,
        DEBUG_LEVEL_DETAILED = 3,
        DEBUG_LEVEL_VERBOSE = 4,
        DEBUG_LEVEL_TRACE = 5
    };
    
    static void setDebugLevel(DebugLevel level);
    static DebugLevel getDebugLevel();
    static bool isDebugLevelEnabled(DebugLevel level);
    
    // Debug filtering
    static void addDebugFilter(const std::string& category);
    static void removeDebugFilter(const std::string& category);
    static void clearDebugFilters();
    static bool isCategoryFiltered(const std::string& category);
    
    // Debug snapshots
    static void takeSnapshot(const std::string& name);
    static void restoreSnapshot(const std::string& name);
    static void listSnapshots();
    static void clearSnapshots();
    
    // Debug script execution
    static void executeDebugScript(const std::string& script);
    static bool isDebugScriptFile(const std::string& filepath);
    
    // Debug visualization
    static void generateSystemReport(const std::string& outputFile = "");
    static void generatePerformanceReport(const std::string& outputFile = "");
    static void generateEventReport(const std::string& outputFile = "");
    
    // Debug metrics
    static size_t getEventCount();
    static size_t getPerformanceCallCount();
    static size_t getMemoryCheckCount();
    static std::chrono::steady_clock::time_point getSessionStartTime();
    
    // Debug statistics
    static std::string getDebugStatistics();
    static void resetDebugStatistics();
    
    // Debug state
    static bool isDebugActive();
    static void setDebugActive(bool active);
    static std::string getDebugSessionId();
    static void setDebugSessionId(const std::string& sessionId);
    
    // Debug cleanup
    static void cleanup();
    static void shutdown();
    
private:
    // Private implementation details
    struct Impl;
    static std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
