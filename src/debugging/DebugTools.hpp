#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Comprehensive debugging tools for neural simulation
class DebugTools {
public:
    DebugTools();
    ~DebugTools();
    
    // Initialize debugging tools
    bool initialize();
    
    // Start/stop debugging
    void startDebugging();
    void stopDebugging();
    bool isDebugging() const;
    
    // Neural state inspection
    void inspectBrain(const class Brain& brain);
    void displayNeuronInfo(int neuronId, const class Brain& brain);
    void displaySynapseInfo(int synapseId, const class Brain& brain);
    void displayNetworkStats(const class Brain& brain);
    
    // Memory leak detection
    void startMemoryMonitoring();
    void stopMemoryMonitoring();
    void checkMemoryLeaks();
    void displayMemoryReport();
    
    // Performance profiling
    void startProfiling();
    void stopProfiling();
    void addProfilePoint(const std::string& name);
    void displayProfileReport();
    
    // Spike processing debugging
    void addSpikeDebugInfo(const class SpikeEvent& spike);
    void displaySpikeAnalysis();
    void visualizeSpikeTimeline();
    
    // Performance bottleneck analysis
    void updatePerformanceCounters();
    void detectSlowOperations();
    void displayBottleneckReport();
    
    // Event timeline debugging
    void logEvent(const std::string& eventType, const std::string& description, 
                  double timestamp, const std::string& details = "");
    void displayEventLog();
    void exportEventLog(const std::string& filename);
    
    // Real-time debugging interfaces
    void updateDebugDisplay();
    bool handleDebugInput();
    void toggleDebugOverlay();
    void togglePerformanceProfiler();
    void toggleMemoryProfiler();
    
    // Cross-platform debugging utilities
    void captureCrashDump();
    void generateDebugReport();
    void sendDebugInfo(const std::string& target);
    
    // Debugging hooks
    void setDebugHook(const std::string& hookName, std::function<void()> hook);
    void executeDebugHooks(const std::string& hookName);
    void registerDebugCallback(const std::string& event, std::function<void()> callback);
    
    // Export capabilities
    void exportDebugData(const std::string& filename);
    void exportNeuralState(const std::string& filename, const class Brain& brain);
    void exportPerformanceData(const std::string& filename);
    
    // Configuration
    void setMaxMemoryUsage(double mb);
    void setProfileInterval(double seconds);
    void setDebugVerbosity(int level);
    void setOutputFile(const std::string& filename);
    
    // State management
    void reset();
    void pause();
    void resume();
    
    // Python integration
    void updateFromPython(const py::dict& debugData);
    py::dict getDebugState() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory leak detection utilities
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Start/stop memory profiling
    void start();
    void stop();
    bool isActive() const;
    
    // Allocate tracking
    void* trackAllocation(void* ptr, size_t size, const std::string& context);
    void trackDeallocation(void* ptr);
    
    // Leak detection
    void analyzeLeaks();
    void reportLeaks();
    double getMemoryLeakSize() const;
    std::vector<std::string> getLeakContexts() const;
    
    // Memory usage monitoring
    double getCurrentMemoryUsage() const;
    double getPeakMemoryUsage() const;
    std::vector<double> getMemoryHistory() const;
    
    // Export
    void exportMemoryReport(const std::string& filename);
    
private:
    struct MemoryBlock {
        void* address;
        size_t size;
        std::string context;
        double timestamp;
        bool allocated;
    };
    
    std::vector<MemoryBlock> memoryBlocks;
    double peakMemoryUsage;
    bool active;
    double startTime;
};

// Performance bottleneck analyzer
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    // Operation tracking
    void startOperation(const std::string& operationName);
    void endOperation(const std::string& operationName);
    void recordOperationTime(const std::string& name, double timeMs);
    
    // Bottleneck detection
    void analyzeBottlenecks();
    std::vector<std::string> getBottleneckOperations() const;
    double getOperationTime(const std::string& name) const;
    double getAverageTime(const std::string& name) const;
    
    // Performance thresholds
    void setThreshold(const std::string& operation, double thresholdMs);
    void setPerformanceWarningLevel(double level);
    void setPerformanceCriticalLevel(double level);
    
    // Reporting
    void displayPerformanceReport();
    void exportPerformanceReport(const std::string& filename);
    
private:
    struct OperationStats {
        std::string name;
        std::vector<double> times;
        double totalTime;
        double averageTime;
        double minTime;
        double maxTime;
        int count;
        double threshold;
    };
    
    std::map<std::string, OperationStats> operationStats;
    std::vector<std::string> bottleneckOperations;
    double warningLevel;
    double criticalLevel;
};

// Neural state inspector
class NeuralStateInspector {
public:
    NeuralStateInspector();
    ~NeuralStateInspector();
    
    // Brain state inspection
    void inspectBrainState(const class Brain& brain);
    void displayNeuronDetails(int neuronId, const class Brain& brain);
    void displaySynapseDetails(int synapseId, const class Brain& brain);
    void displayRegionInfo(int regionId, const class Brain& brain);
    void displayPopulationInfo(int populationId, const class Brain& brain);
    
    // Activity analysis
    void analyzeNeuralActivity(const class Brain& brain);
    void displayFiringRateStats(const class Brain& brain);
    void displayConnectionStats(const class Brain& brain);
    void displaySpikingActivity(const class Brain& brain);
    
    // Visualization helpers
    void prepareInspectionData(const class Brain& brain);
    std::vector<std::string> getAvailableInspectionItems() const;
    bool hasInspectionItem(const std::string& item) const;
    
private:
    struct NeuralMetrics {
        int totalNeurons;
        int totalSynapses;
        int activeNeurons;
        int firingNeurons;
        double averageFiringRate;
        double averageMembranePotential;
        double excitationInhibitionRatio;
        int developmentStage;
    } metrics;
    
    std::map<std::string, std::string> neuronInfo;
    std::map<std::string, std::string> synapseInfo;
};

// Event timeline visualization
class EventTimeline {
public:
    EventTimeline();
    ~EventTimeline();
    
    // Event management
    void addEvent(const std::string& type, const std::string& description,
                  double timestamp, const std::string& details = "");
    void clearEvents();
    void removeOldEvents(double thresholdTime);
    
    // Timeline properties
    void setTimelineRange(double startTime, double endTime);
    void setEventColors(const std::map<std::string, sf::Color>& colors);
    void setMaxEventsPerFrame(int maxEvents);
    
    // Visualization
    void render(sf::RenderWindow& window);
    void renderTimeline(sf::RenderWindow& window);
    void renderEvents(sf::RenderWindow& window);
    void renderEventDetails(sf::RenderWindow& window);
    
    // Interaction
    bool handleInput(sf::RenderWindow& window);
    void zoom(double factor);
    void pan(float dx, float dy);
    void selectEvent(size_t index);
    
    // Export
    void exportTimeline(const std::string& filename);
    std::vector<EventData> getEventsInRange(double startTime, double endTime) const;
    
private:
    struct EventData {
        std::string type;
        std::string description;
        double timestamp;
        std::string details;
        sf::Color color;
        size_t id;
    };
    
    std::vector<EventData> events;
    double timelineStart;
    double timelineEnd;
    std::map<std::string, sf::Color> eventColors;
    int maxEventsPerFrame;
    size_t selectedEventIndex;
    sf::Font font;
};

} // namespace nlm
