#include "BrainProfiler.hpp"
#include "BrainProfiler.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <filesystem>
#include <chrono>

namespace nlm {

BrainProfiler::BrainProfiler(std::shared_ptr<Brain> brain, const Config& config)
    : brain_(brain)
    , config_(config)
    , samplingInterval_(config.samplingInterval)
    , previousTotalSpikeCount_(0)
    , previousPendingSpikeCount_(0)
    , previousMemoryUsage_(0.0)
    , previousTime_(0.0) {

    // Create profiling components
    performanceProfiler_ = std::make_unique<PerformanceProfiler>();
    memoryMonitor_ = std::make_unique<MemoryMonitor>();
    eventProfiler_ = std::make_unique<EventProfiler>();

    // Configure components
    performanceProfiler_->setMode(config.enableProfiler ? PerformanceProfiler::Mode::COMPREHENSIVE : PerformanceProfiler::Mode::OFF);
    memoryMonitor_->setEnabled(config.enableProfiler && config.profileMemorySystems);
    eventProfiler_->setEnabled(config.enableProfiler && config.profileEventProcessing);

    // Link components
    performanceProfiler_->setMemoryMonitor(memoryMonitor_.get());
    performanceProfiler_->setEventProfiler(eventProfiler_.get());
    memoryMonitor_->setProfiler(performanceProfiler_.get());
    eventProfiler_->setProfiler(performanceProfiler_.get());

    if (config.autoStart) {
        start();
    }
}

BrainProfiler::~BrainProfiler() {
    stop();
}

void BrainProfiler::updateConfig(const Config& config) {
    config_ = config;
    samplingInterval_ = config.samplingInterval;

    // Update component configurations
    performanceProfiler_->setMode(config.enableProfiler ? PerformanceProfiler::Mode::COMPREHENSIVE : PerformanceProfiler::Mode::OFF);
    memoryMonitor_->setEnabled(config.enableProfiler && config.profileMemorySystems);
    eventProfiler_->setEnabled(config.enableProfiler && config.profileEventProcessing);
}

const BrainProfiler::Config& BrainProfiler::getConfig() const {
    return config_;
}

void BrainProfiler::start() {
    if (running_) return;

    running_ = true;
    paused_ = false;
    lastSampleTime_ = std::chrono::high_resolution_clock::now();

    performanceProfiler_->start();
    memoryMonitor_->start();
    eventProfiler_->start();
}

void BrainProfiler::stop() {
    if (!running_) return;

    running_ = false;
    paused_ = false;

    performanceProfiler_->stop();
    memoryMonitor_->stop();
    eventProfiler_->stop();

    // Export data if configured
    if (config_.exportOnStop) {
        createExportDirectory();
        exportToJSON(config_.exportDirectory + "/brain_profile_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json");
        exportToCSV(config_.exportDirectory + "/brain_profile.csv");
        generateReport(config_.exportDirectory + "/brain_profile_report.txt");
    }
}

void BrainProfiler::pause() {
    if (!running_ || paused_) return;
    paused_ = true;
}

void BrainProfiler::resume() {
    if (!running_ || !paused_) return;
    paused_ = false;
    lastSampleTime_ = std::chrono::high_resolution_clock::now();
}

bool BrainProfiler::isRunning() const {
    return running_ && !paused_;
}

bool BrainProfiler::isPaused() const {
    return paused_;
}

const BrainProfiler::PerformanceSnapshot* BrainProfiler::getCurrentSnapshot() const {
    std::lock_guard<std::mutex> lock(snapshotsMutex_);
    if (snapshots_.empty()) return nullptr;
    return &snapshots_.back();
}

std::vector<BrainProfiler::PerformanceSnapshot> BrainProfiler::getSnapshotsInRange(SimulationStep startStep, SimulationStep endStep) const {
    std::lock_guard<std::mutex> lock(snapshotsMutex_);
    std::vector<PerformanceSnapshot> result;
    for (const auto& snapshot : snapshots_) {
        if (snapshot.step >= startStep && snapshot.step <= endStep) {
            result.push_back(snapshot);
        }
    }
    return result;
}

const std::vector<BrainProfiler::PerformanceSnapshot>& BrainProfiler::getAllSnapshots() const {
    return snapshots_;
}

void BrainProfiler::integrateWithBrain() {
    if (!running_ || paused_) return;

    // Sample current state
    sample();

    // Update profiler with current brain state
    updateProfilerWithBrainState();
}

void BrainProfiler::exportToJSON(const std::string& filename) const {
    nlohmann::json jsonData;
    exportToJSONInternal(jsonData);

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    file << jsonData.dump(2);
}

void BrainProfiler::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    exportToCSVInternal(file);
}

void BrainProfiler::generateReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    file << getReportText();
}

std::string BrainProfiler::getReportText() const {
    std::stringstream ss;
    generateReportInternal(ss);
    return ss.str();
}

PerformanceProfiler* BrainProfiler::getPerformanceProfiler() {
    return performanceProfiler_.get();
}

MemoryMonitor* BrainProfiler::getMemoryMonitor() {
    return memoryMonitor_.get();
}

EventProfiler* BrainProfiler::getEventProfiler() {
    return eventProfiler_.get();
}

void BrainProfiler::enableProfiler(bool enable) {
    config_.enableProfiler = enable;
    performanceProfiler_->setMode(enable ? PerformanceProfiler::Mode::COMPREHENSIVE : PerformanceProfiler::Mode::OFF);
    memoryMonitor_->setEnabled(enable && config_.profileMemorySystems);
    eventProfiler_->setEnabled(enable && config_.profileEventProcessing);
}

void BrainProfiler::setProfilerMode(PerformanceProfiler::Mode mode) {
    performanceProfiler_->setMode(mode);
}

void BrainProfiler::enableMemoryMonitoring(bool enable) {
    config_.profileMemorySystems = enable;
    memoryMonitor_->setEnabled(config_.enableProfiler && enable);
}

void BrainProfiler::enableEventProfiling(bool enable) {
    config_.profileEventProcessing = enable;
    eventProfiler_->setEnabled(config_.enableProfiler && enable);
}

void BrainProfiler::sample() {
    PerformanceSnapshot snapshot;

    // Get current time and step from brain if available
    if (brain_) {
        snapshot.step = brain_->getTotalStepCount();  // Note: Brain doesn't have getTotalStepCount, this is a placeholder
        snapshot.timestamp = static_cast<Timestamp>(snapshot.step) * 0.001;  // Assuming 1ms timestep
    } else {
        // Use internal tracking
        static SimulationStep currentStep = 0;
        snapshot.step = currentStep++;
        snapshot.timestamp = static_cast<Timestamp>(snapshot.step) * 0.001;
    }

    // Collect metrics from brain
    if (brain_) {
        // These would need to be added to Brain API
        snapshot.neuronCount = brain_->getTotalNeuronCount();
        snapshot.synapseCount = brain_->getTotalSynapseCount();
        snapshot.activeNeuronCount = brain_->getActiveNeuronCount();
        snapshot.firingNeuronCount = brain_->getFiringNeuronCount();
        snapshot.totalSpikeCount = brain_->getTotalSpikeCount();
        snapshot.pendingSpikeCount = brain_->getPendingSpikeEventCount();
        snapshot.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
        snapshot.averageFiringRate = brain_->getAverageFiringRate();
    } else {
        // Initialize with zeros if brain not available
        snapshot.neuronCount = 0;
        snapshot.synapseCount = 0;
        snapshot.activeNeuronCount = 0;
        snapshot.firingNeuronCount = 0;
        snapshot.totalSpikeCount = 0;
        snapshot.pendingSpikeCount = 0;
        snapshot.excitationInhibitionRatio = 1.0f;
        snapshot.averageFiringRate = 0.0;
    }

    // Get memory usage from memory monitor
    if (memoryMonitor_) {
        snapshot.memoryUsageMB = memoryMonitor_->getCurrentMemoryUsage() / (1024.0 * 1024.0);
    }

    // Calculate step duration
    auto currentTime = std::chrono::high_resolution_clock::now();
    snapshot.stepDuration = (currentTime - lastSampleTime_).count();
    lastSampleTime_ = currentTime;

    // Store snapshot
    {
        std::lock_guard<std::mutex> lock(snapshotsMutex_);
        snapshots_.push_back(snapshot);

        // Clean up old snapshots (keep last hour)
        auto cutoff = std::chrono::high_resolution_clock::now() -
                      std::chrono::hours(1);
        // This is simplified - in practice you'd need to track snapshot timestamps
    }
}

void BrainProfiler::updateProfilerWithBrainState() {
    // Update performance profiler with current brain state
    if (performanceProfiler_ && brain_) {
        // Profile step timing
        performanceProfiler_->profileStepStart(brain_->getTotalStepCount(), 0.0);  // Placeholder
        performanceProfiler_->profileStepEnd(brain_->getTotalStepCount(), 0.0);    // Placeholder

        // Profile neuron and synapse processing
        performanceProfiler_->profileNeuronProcessing(brain_->getTotalNeuronCount());
        performanceProfiler_->profileSynapseProcessing(brain_->getTotalSynapseCount());

        // Profile memory systems
        if (memoryMonitor_) {
            size_t memoryBytes = memoryMonitor_->getCurrentMemoryUsage();
            performanceProfiler_->profileMemorySystem("working_memory", memoryBytes / 4);
            performanceProfiler_->profileMemorySystem("episodic_memory", memoryBytes / 4);
        }

        // Profile event processing
        if (eventProfiler_) {
            // Get queue stats
            const auto* spikeQueueStats = eventProfiler_->getQueueStats("spike_queue");
            if (spikeQueueStats) {
                performanceProfiler_->profileEventQueue("spike_queue", spikeQueueStats->currentSize, 0.0);
            }
        }
    }
}

void BrainProfiler::createExportDirectory() const {
    std::error_code ec;
    std::filesystem::create_directories(config_.exportDirectory, ec);
}

void BrainProfiler::generateReportInternal(std::ostream& output) const {
    output << "=== Brain Performance Profiling Report ===\n";
    output << "Generated: " << std::chrono::system_clock::now() << "\n\n";

    output << "Configuration:\n";
    output << "  Profiler Enabled: " << (config_.enableProfiler ? "Yes" : "No") << "\n";
    output << "  Auto Start: " << (config_.autoStart ? "Yes" : "No") << "\n";
    output << "  Sampling Interval: " << config_.samplingInterval << " seconds\n";
    output << "  Profile Time Steps: " << (config_.profileTimeSteps ? "Yes" : "No") << "\n";
    output << "  Profile Neuron Processing: " << (config_.profileNeuronProcessing ? "Yes" : "No") << "\n";
    output << "  Profile Synapse Processing: " << (config_.profileSynapseProcessing ? "Yes" : "No") << "\n";
    output << "  Profile Plasticity: " << (config_.profilePlasticity ? "Yes" : "No") << "\n";
    output << "  Profile Memory Systems: " << (config_.profileMemorySystems ? "Yes" : "No") << "\n";
    output << "  Profile Event Processing: " << (config_.profileEventProcessing ? "Yes" : "No") << "\n";
    output << "  Profile Concurrent Operations: " << (config_.profileConcurrentOperations ? "Yes" : "No") << "\n";
    output << "  Profile Spikes: " << (config_.profileSpikes ? "Yes" : "No") << "\n\n";

    output << "Status:\n";
    output << "  Running: " << (running_ ? "Yes" : "No") << "\n";
    output << "  Paused: " << (paused_ ? "Yes" : "No") << "\n";
    output << "  Total Snapshots: " << snapshots_.size() << "\n\n";

    if (!snapshots_.empty()) {
        const auto& latest = snapshots_.back();
        output << "Latest Snapshot (Step " << latest.step << "):\n";
        output << "  Timestamp: " << latest.timestamp << " seconds\n";
        output << "  Step Duration: " << latest.stepDuration << " seconds\n";
        output << "  Neuron Count: " << latest.neuronCount << "\n";
        output << "  Synapse Count: " << latest.synapseCount << "\n";
        output << "  Active Neurons: " << latest.activeNeuronCount << "\n";
        output << "  Firing Neurons: " << latest.firingNeuronCount << "\n";
        output << "  Total Spikes: " << latest.totalSpikeCount << "\n";
        output << "  Pending Spikes: " << latest.pendingSpikeCount << "\n";
        output << "  E/I Ratio: " << latest.excitationInhibitionRatio << "\n";
        output << "  Average Firing Rate: " << latest.averageFiringRate << " Hz\n";
        output << "  Memory Usage: " << latest.memoryUsageMB << " MB\n\n";

        // Calculate deltas
        output << "Delta (since last snapshot):\n";
        size_t deltaSpikes = latest.totalSpikeCount - previousTotalSpikeCount_;
        size_t deltaPending = latest.pendingSpikeCount - previousPendingSpikeCount_;
        double deltaMemory = latest.memoryUsageMB - previousMemoryUsage_;
        double deltaTime = latest.stepDuration - 0.001;  // Approximate

        output << "  Spike Delta: " << deltaSpikes << "\n";
        output << "  Pending Spike Delta: " << deltaPending << "\n";
        output << "  Memory Delta: " << deltaMemory << " MB\n";
        output << "  Step Duration Delta: " << deltaTime << " seconds\n\n";

        // Update previous values
        previousTotalSpikeCount_ = latest.totalSpikeCount;
        previousPendingSpikeCount_ = latest.pendingSpikeCount;
        previousMemoryUsage_ = latest.memoryUsageMB;
    }
}

void BrainProfiler::exportToJSONInternal(nlohmann::json& json) const {
    json["config"] = {
        {"enableProfiler", config_.enableProfiler},
        {"autoStart", config_.autoStart},
        {"samplingInterval", config_.samplingInterval},
        {"profileTimeSteps", config_.profileTimeSteps},
        {"profileNeuronProcessing", config_.profileNeuronProcessing},
        {"profileSynapseProcessing", config_.profileSynapseProcessing},
        {"profilePlasticity", config_.profilePlasticity},
        {"profileMemorySystems", config_.profileMemorySystems},
        {"profileEventProcessing", config_.profileEventProcessing},
        {"profileConcurrentOperations", config_.profileConcurrentOperations},
        {"profileSpikes", config_.profileSpikes},
        {"exportDirectory", config_.exportDirectory},
        {"exportOnStop", config_.exportOnStop}
    };

    json["status"] = {
        {"running", running_},
        {"paused", paused_},
        {"totalSnapshots", snapshots_.size()}
    };

    // Export snapshots
    json["snapshots"] = nlohmann::json::array();
    for (const auto& snapshot : snapshots_) {
        nlohmann::json snapshotJson;
        snapshotJson["step"] = snapshot.step;
        snapshotJson["timestamp"] = snapshot.timestamp;
        snapshotJson["stepDuration"] = snapshot.stepDuration;
        snapshotJson["neuronCount"] = snapshot.neuronCount;
        snapshotJson["synapseCount"] = snapshot.synapseCount;
        snapshotJson["activeNeuronCount"] = snapshot.activeNeuronCount;
        snapshotJson["firingNeuronCount"] = snapshot.firingNeuronCount;
        snapshotJson["totalSpikeCount"] = snapshot.totalSpikeCount;
        snapshotJson["pendingSpikeCount"] = snapshot.pendingSpikeCount;
        snapshotJson["excitationInhibitionRatio"] = snapshot.excitationInhibitionRatio;
        snapshotJson["averageFiringRate"] = snapshot.averageFiringRate;
        snapshotJson["memoryUsageMB"] = snapshot.memoryUsageMB;
        snapshotJson["memoryPoolUsage"] = snapshot.memoryPoolUsage;
        snapshotJson["eventQueueSize"] = snapshot.eventQueueSize;

        // Component metrics
        snapshotJson["componentMetrics"] = snapshot.componentMetrics;

        json["snapshots"].push_back(snapshotJson);
    }
}

void BrainProfiler::exportToCSVInternal(std::ostream& output) const {
    output << "Step,Timestamp,Step Duration,Neuron Count,Synapse Count,Active Neurons,Firing Neurons,Total Spikes,Pending Spikes,E/I Ratio,Average Firing Rate,Memory Usage (MB)\n";

    for (const auto& snapshot : snapshots_) {
        output << snapshot.step << "," << snapshot.timestamp << "," << snapshot.stepDuration << ",";
        output << snapshot.neuronCount << "," << snapshot.synapseCount << ",";
        output << snapshot.activeNeuronCount << "," << snapshot.firingNeuronCount << ",";
        output << snapshot.totalSpikeCount << "," << snapshot.pendingSpikeCount << ",";
        output << snapshot.excitationInhibitionRatio << "," << snapshot.averageFiringRate << ",";
        output << snapshot.memoryUsageMB << "\n";
    }
}
