#pragma once

#include "Brain.hpp"
#include "Profiler.hpp"
#include "MemoryMonitor.hpp"
#include "EventProfiler.hpp"
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace nlm {

class BrainProfiler {
public:
    // Profiling configuration
    struct Config {
        bool enableProfiler{false};
        bool autoStart{false};
        double samplingInterval{0.1};  // seconds
        bool profileTimeSteps{true};
        bool profileNeuronProcessing{true};
        bool profileSynapseProcessing{true};
        bool profilePlasticity{true};
        bool profileMemorySystems{true};
        bool profileEventProcessing{true};
        bool profileConcurrentOperations{true};
        bool profileSpikes{true};
        std::string exportDirectory{"./profiles"};
        bool exportOnStop{false};
    };

    // Performance metrics snapshot
    struct PerformanceSnapshot {
        SimulationStep step{0};
        Timestamp timestamp{0.0};
        double stepDuration{0.0};
        size_t neuronCount{0};
        size_t synapseCount{0};
        size_t activeNeuronCount{0};
        size_t firingNeuronCount{0};
        size_t totalSpikeCount{0};
        size_t pendingSpikeCount{0};
        float excitationInhibitionRatio{1.0f};
        double averageFiringRate{0.0};
        double memoryUsageMB{0.0};
        size_t memoryPoolUsage{0};
        size_t eventQueueSize{0};
        std::map<std::string, double> componentMetrics;
        std::vector<double> threadUtilizations;
    };

    BrainProfiler(std::shared_ptr<Brain> brain, const Config& config = Config{});
    ~BrainProfiler();

    // Configuration
    void updateConfig(const Config& config);
    const Config& getConfig() const;

    // Profiling control
    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning() const;
    bool isPaused() const;

    // Data access
    const PerformanceSnapshot* getCurrentSnapshot() const;
    std::vector<PerformanceSnapshot> getSnapshotsInRange(SimulationStep startStep, SimulationStep endStep) const;
    const std::vector<PerformanceSnapshot>& getAllSnapshots() const;

    // Integration with Brain
    void integrateWithBrain();

    // Reporting and exporting
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    void generateReport(const std::string& filename) const;
    std::string getReportText() const;

    // Access to sub-profilers
    PerformanceProfiler* getPerformanceProfiler();
    MemoryMonitor* getMemoryMonitor();
    EventProfiler* getEventProfiler();

    // Configuration setters
    void enableProfiler(bool enable);
    void setProfilerMode(PerformanceProfiler::Mode mode);
    void enableMemoryMonitoring(bool enable);
    void enableEventProfiling(bool enable);

private:
    std::shared_ptr<Brain> brain_;
    Config config_;

    // Profiling components
    std::unique_ptr<PerformanceProfiler> performanceProfiler_;
    std::unique_ptr<MemoryMonitor> memoryMonitor_;
    std::unique_ptr<EventProfiler> eventProfiler_;

    // State management
    bool running_{false};
    bool paused_{false};
    std::chrono::high_resolution_clock::time_point lastSampleTime_;
    std::chrono::duration<double> samplingInterval_;

    // Data storage
    std::vector<PerformanceSnapshot> snapshots_;
    mutable std::mutex snapshotsMutex_;

    // Previous state for delta calculations
    size_t previousTotalSpikeCount_{0};
    size_t previousPendingSpikeCount_{0};
    double previousMemoryUsage_{0.0};
    Timestamp previousTime_{0.0};

    // Helper functions
    void sample();
    void updateProfilerWithBrainState();
    void createExportDirectory() const;
    void generateReportInternal(std::ostream& output) const;
    void exportToJSONInternal(nlohmann::json& json) const;
    void exportToCSVInternal(std::ostream& output) const;
};
