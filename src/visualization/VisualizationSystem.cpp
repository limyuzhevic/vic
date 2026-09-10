// Console visualization backend implementation
#pragma once

#include "VisualizationBackend.hpp"
#include <iostream>
#include <unordered_map>
#include "../core/Types/Types.hpp"

namespace nlm {

class ConsoleVisualizationBackend : public VisualizationBackend {
public:
    ConsoleVisualizationBackend() = default;
    ~ConsoleVisualizationBackend() override = default;
    
    bool initialize() override {
        active_ = true;
        return true;
    }
    
    void shutdown() override {
        active_ = false;
    }
    
    void update() override {
        // In console mode, updates are handled directly in render()
    }
    
    void render() override {
        if (!active_) return;
        
        // Clear screen (ANSI escape sequence)
        std::cout << "\033[2J\033[H";
        
        std::cout << "=== NLM BRAIN VISUALIZATION ===" << std::endl;
        std::cout << "Active neurons: " << neuronCount_ << std::endl;
        std::cout << "Firing rate: " << averageFiringRate_ << " Hz" << std::endl;
        
        if (!regionActivities_.empty()) {
            std::cout << "\nRegion Activities:" << std::endl;
            for (const auto& [regionId, activity] : regionActivities_) {
                std::cout << "  Region " << regionId << ": " << activity << "%" << std::endl;
            }
        }
    }
    
    void setSettings(const VisualizationSettings& settings) override {
        settings_ = settings;
    }
    
    bool isActive() const override {
        return active_;
    }
    
    // Console-specific methods
    void setWindowSize(int width, int height) {
        width_ = width;
        height_ = height;
    }
    
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    
    // Set brain data for visualization
    void setBrainData(size_t neuronCount, float averageFiringRate,
                     const std::unordered_map<RegionId, float>& regionActivities) {
        neuronCount_ = neuronCount;
        averageFiringRate_ = averageFiringRate;
        regionActivities_ = regionActivities;
    }
    
protected:
    VisualizationSettings settings_;
    bool active_ = false;
    int width_ = 80;
    int height_ = 24;
    
    // Data for visualization
    size_t neuronCount_ = 0;
    float averageFiringRate_ = 0.0f;
    std::unordered_map<RegionId, float> regionActivities_;
};

// Web visualization backend
class WebVisualizationBackend : public VisualizationBackend {
public:
    WebVisualizationBackend() = default;
    ~WebVisualizationBackend() override = default;
    
    bool initialize() override {
        active_ = false;
        return true;
    }
    
    void shutdown() override {
        active_ = false;
    }
    
    void update() override {
        // Web updates would happen in background threads
    }
    
    void render() override {
        // Web rendering is done in browser
    }
    
    void setSettings(const VisualizationSettings& settings) override {
        settings_ = settings;
    }
    
    bool isActive() const override {
        return active_;
    }
    
    // Web-specific methods
    bool startServer(const std::string& port) {
        // Would start web server here
        return true;
    }
    
    void stopServer() {
        active_ = false;
    }
    
protected:
    VisualizationSettings settings_;
    bool active_ = false;
};

// CSVSensorRecorder implementation
class CSVSensorRecorder : public SimulationRecorder {
public:
    CSVSensorRecorder() = default;
    ~CSVSensorRecorder() override {
        if (file_.is_open()) {
            file_.close();
        }
    }
    
    bool startRecording(const std::string& filepath, const VisualizationSettings& settings) override {
        filepath_ = filepath;
        settings_ = settings;
        
        // Create output file
        file_.open(filepath, std::ios::out);
        if (!file_.is_open()) {
            return false;
        }
        
        // Write header
        file_ << "step,time,neuron_count,active_neurons,firing_neurons,total_spikes," 
              << "avg_firing_rate,working_memory_traces,episodic_episodes," 
              << "dopamine_level,curiosity_level,novelty_level,prediction_error_level," 
              << "development_stage" << std::endl;
        
        return true;
    }
    
    void stopRecording() override {
        if (file_.is_open()) {
            file_.close();
        }
    }
    
    bool isRecording() const override {
        return file_.is_open();
    }
    
    void recordFrame(const Brain& brain, size_t step, double time) override {
        if (!isRecording()) return;
        
        file_ << step << "," << time << ","
              << brain.getTotalNeuronCount() << ","
              << brain.getActiveNeuronCount() << ","
              << brain.getFiringNeuronCount() << ","
              << brain.getTotalSpikeCount() << ","
              << brain.getAverageFiringRate() << ","
              << (brain.getWorkingMemory() ? brain.getWorkingMemory()->getActiveTraces() : 0) << ","
              << (brain.getEpisodicMemory() ? brain.getEpisodicMemory()->getEpisodeCount() : 0) << ","
              << (brain.getDopamine() ? brain.getDopamine()->getLevel() : 0.0f) << ","
              << (brain.getCuriosity() ? brain.getCuriosity()->getLevel() : 0.0f) << ","
              << (brain.getNovelty() ? brain.getNovelty()->getLevel() : 0.0f) << ","
              << (brain.getPredictionErrorSignal() ? brain.getPredictionErrorSignal()->getLevel() : 0.0f) << ","
              << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    }
    
    bool loadRecording(const std::string& filepath) override {
        // For simplicity, just clear current data
        frames_.clear();
        steps_.clear();
        times_.clear();
        return true;
    }
    
    void startReplay() override {
        replayIndex_ = 0;
    }
    
    void stopReplay() override {
        replayIndex_ = 0;
    }
    
    bool isReplaying() const override {
        return replayIndex_ < frames_.size();
    }
    
    size_t getCurrentReplayFrame() const override {
        return replayIndex_;
    }
    
    size_t getTotalFrames() const override {
        return frames_.size();
    }
    
    std::vector<double> getRecordedTimes() const override {
        return times_;
    }
    
    std::vector<size_t> getRecordedSteps() const override {
        return steps_;
    }
    
    const Brain* getFrameState(size_t frameIndex) override {
        return nullptr; // Would restore full brain state here
    }
    
    bool exportData(const std::string& outputPath, const std::string& format) override {
        if (format == "csv") {
            return exportCSV(outputPath);
        }
        return false;
    }
    
    // Export to CSV
    bool exportCSV(const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "step,time,neuron_count,active_neurons,firing_neurons,total_spikes," 
             << "avg_firing_rate,working_memory_traces,episodic_episodes," 
             << "dopamine_level,curiosity_level,novelty_level,prediction_error_level," 
             << "development_stage" << std::endl;
        
        for (size_t i = 0; i < frames_.size(); ++i) {
            file << i << "," << times_[i] << ","
                 << std::get<0>(frames_[i]) << ","
                 << std::get<1>(frames_[i]) << ","
                 << std::get<2>(frames_[i]) << ","
                 << std::get<3>(frames_[i]) << ","
                 << std::get<4>(frames_[i]) << ","
                 << std::get<5>(frames_[i]) << ","
                 << std::get<6>(frames_[i]) << ","
                 << std::get<7>(frames_[i]) << ","
                 << std::get<8>(frames_[i]) << ","
                 << std::get<9>(frames_[i]) << ","
                 << std::get<10>(frames_[i]) << ","
                 << std::get<11>(frames_[i]) << ","
                 << std::get<12>(frames_[i]) << std::endl;
        }
        
        file.close();
        return true;
    }
    
    void setCompressionLevel(size_t level) override {
        compressionLevel_ = level;
    }
    
protected:
    std::string filepath_;
    VisualizationSettings settings_;
    std::ofstream file_;
    
    // Recorded data
    std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, // step, neuron_count, active, firing, total_spikes
                          float, size_t, size_t, float, float, float, float, float, float>> frames_;
    std::vector<double> times_;
    std::vector<size_t> steps_;
    size_t replayIndex_ = 0;
    size_t compressionLevel_ = 0;
};

// RealTimePerformanceMonitor implementation
class RealTimePerformanceMonitor : public PerformanceMonitor {
public:
    RealTimePerformanceMonitor() : lastUpdateTime_(0.0), isEnabled_(true) {}
    ~RealTimePerformanceMonitor() override = default;
    
    void update() override {
        auto now = std::chrono::high_resolution_clock::now();
        double currentTime = std::chrono::duration<double>(now.time_since_epoch()).count();
        
        // Calculate delta time
        double delta = currentTime - lastUpdateTime_;
        if (delta > 0.0) {
            updateRate_ = 1.0 / delta;
        }
        lastUpdateTime_ = currentTime;
        
        // Update metrics (placeholder - would gather actual metrics)
        metrics_.simulationTime = 0.001; // Placeholder
        metrics_.renderingTime = 0.0005; // Placeholder
        metrics_.updateTime = 0.0003;    // Placeholder
        metrics_.cpuUsage = 50.0;        // Placeholder
        metrics_.memoryUsage = 512.0;   // Placeholder
        metrics_.spikeCount = 42;        // Placeholder
        metrics_.neuronCount = 1000;    // Placeholder
        metrics_.synapseCount = 15000;   // Placeholder
        metrics_.eiratio = 1.5f;        // Placeholder
        metrics_.averageFiringRate = 8.5f; // Placeholder
        metrics_.memoryTraces = 42;     // Placeholder
        metrics_.episodicEpisodes = 12; // Placeholder
        metrics_.noveltyLevel = 0.3f;   // Placeholder
        metrics_.curiosityLevel = 0.5f; // Placeholder
        metrics_.dopamineLevel = 0.7f;  // Placeholder
    }
    
    const Metrics& getMetrics() const override {
        return metrics_;
    }
    
    void setUpdateRate(double hz) override {
        updateRate_ = hz;
    }
    
    void enableMetrics(bool enable) override {
        isEnabled_ = enable;
    }
    
protected:
    Metrics metrics_;
    double lastUpdateTime_;
    double updateRate_ = 1.0;
    bool isEnabled_ = true;
};

// RealTimeVisualizationRecorder implementation
class RealTimeVisualizationRecorder : public VisualizationRecorder {
public:
    RealTimeVisualizationRecorder() = default;
    ~RealTimeVisualizationRecorder() override = default;
    
    void recordFrame(size_t step, double time, const Brain& brain,
                    const PerformanceMonitor::Metrics& metrics) override {
        frames_.push_back(FrameData{step, time, &brain, metrics});
        
        // Limit frame count if configured
        if (maxFrames_ > 0 && frames_.size() > maxFrames_) {
            frames_.erase(frames_.begin(), frames_.begin() + (frames_.size() - maxFrames_));
        }
    }
    
    bool exportCSV(const std::string& filepath) const override {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "step,time,neuron_count,active_neurons,firing_neurons,total_spikes," 
             << "avg_firing_rate,working_memory_traces,episodic_episodes," 
             << "dopamine_level,curiosity_level,novelty_level,prediction_error_level," 
             << "development_stage,simulation_time,rendering_time,update_time," 
             << "cpu_usage,memory_usage" << std::endl;
        
        for (const auto& frame : frames_) {
            const auto& brain = *frame.brain;
            const auto& metrics = frame.metrics;
            
            file << frame.step << "," << frame.time << ","
                 << brain.getTotalNeuronCount() << ","
                 << brain.getActiveNeuronCount() << ","
                 << brain.getFiringNeuronCount() << ","
                 << brain.getTotalSpikeCount() << ","
                 << brain.getAverageFiringRate() << ","
                 << (brain.getWorkingMemory() ? brain.getWorkingMemory()->getActiveTraces() : 0) << ","
                 << (brain.getEpisodicMemory() ? brain.getEpisodicMemory()->getEpisodeCount() : 0) << ","
                 << (brain.getDopamine() ? brain.getDopamine()->getLevel() : 0.0f) << ","
                 << (brain.getCuriosity() ? brain.getCuriosity()->getLevel() : 0.0f) << ","
                 << (brain.getNovelty() ? brain.getNovelty()->getLevel() : 0.0f) << ","
                 << (brain.getPredictionErrorSignal() ? brain.getPredictionErrorSignal()->getLevel() : 0.0f) << ","
                 << static_cast<int>(brain.getDevelopmentalStage()) << ","
                 << metrics.simulationTime << ","
                 << metrics.renderingTime << ","
                 << metrics.updateTime << ","
                 << metrics.cpuUsage << ","
                 << metrics.memoryUsage << std::endl;
        }
        
        file.close();
        return true;
    }
    
    bool exportJSON(const std::string& filepath) const override {
        // Would implement JSON export here
        return false;
    }
    
    bool exportBinary(const std::string& filepath) const override {
        // Would implement binary export here
        return false;
    }
    
    void compress() override {
        // Would implement compression here
    }
    
    bool isCompressed() const override {
        return compressed_;
    }
    
    size_t getTotalFrames() const override {
        return frames_.size();
    }
    
    double getDuration() const override {
        if (frames_.empty()) return 0.0;
        return frames_.back().time - frames_.front().time;
    }
    
    std::string getFileSize() const override {
        // Would calculate actual file size
        return "N/A";
    }
    
    void setMaxFrames(size_t max) {
        maxFrames_ = max;
    }
    
    void setCompression(bool enable) {
        compressed_ = enable;
    }
    
protected:
    struct FrameData {
        size_t step;
        double time;
        const Brain* brain;
        PerformanceMonitor::Metrics metrics;
    };
    
    std::vector<FrameData> frames_;
    size_t maxFrames_ = 10000;
    bool compressed_ = false;
};

// Implementation of VisualizationSystem
VisualizationSystem::VisualizationSystem() = default;
VisualizationSystem::~VisualizationSystem() {
    shutdown();
}

bool VisualizationSystem::initialize(const VisualizationSettings& settings) {
    settings_ = settings;
    
    // Create backend
    switch (settings.backend.compare("web")) {
        case 0:
            backend_ = std::make_unique<WebVisualizationBackend>();
            if (settings.webPort.empty()) {
                return false;
            }
            break;
        case 1:
        default:
            backend_ = std::make_unique<ConsoleVisualizationBackend>();
            break;
    }
    
    if (!backend_->initialize()) {
        return false;
    }
    
    // Create recorder
    recorder_ = std::make_unique<CSVSensorRecorder>();
    
    // Create performance monitor
    monitor_ = std::make_unique<RealTimePerformanceMonitor>();
    
    // Create visualization recorder
    visualRecorder_ = std::make_unique<RealTimeVisualizationRecorder>();
    
    return true;
}

void VisualizationSystem::shutdown() {
    if (backend_) {
        backend_->shutdown();
        backend_.reset();
    }
    
    recorder_.reset();
    monitor_.reset();
    visualRecorder_.reset();
}

void VisualizationSystem::update() {
    if (!isInitialized()) return;
    
    // Update performance monitor
    if (monitor_) {
        monitor_->update();
    }
    
    // Update backend
    if (backend_) {
        backend_->update();
    }
}

void VisualizationSystem::render() {
    if (!isInitialized()) return;
    
    if (backend_) {
        backend_->render();
    }
}

void VisualizationSystem::setBrain(std::shared_ptr<Brain> brain) {
    brain_ = brain;
    if (brain_ && backend_ && std::dynamic_pointer_cast<ConsoleVisualizationBackend>(backend_)) {
        // Update console backend with brain data
        size_t neuronCount = brain_->getTotalNeuronCount();
        float avgFiringRate = brain_->getAverageFiringRate();
        
        std::unordered_map<RegionId, float> regionActivities;
        for (const auto& region : brain_->getRegions()) {
            float activity = static_cast<float>(region->getActiveNeuronCount()) / 
                           std::max(1.0f, static_cast<float>(region->getTotalNeuronCount()));
            regionActivities[region->getId()] = activity * 100.0f;
        }
        
        std::dynamic_pointer_cast<ConsoleVisualizationBackend>(backend_)
            ->setBrainData(neuronCount, avgFiringRate, regionActivities);
    }
}

bool VisualizationSystem::startRecording(const std::string& filepath) {
    if (!recorder_) return false;
    
    std::string actualFilepath = filepath.empty() ? "nlm_recording.csv" : filepath;
    return recorder_->startRecording(actualFilepath, settings_);
}

void VisualizationSystem::stopRecording() {
    if (recorder_) {
        recorder_->stopRecording();
    }
}

bool VisualizationSystem::isRecording() const {
    return recorder_ && recorder_->isRecording();
}

bool VisualizationSystem::loadReplay(const std::string& filepath) {
    if (!recorder_) return false;
    return recorder_->loadRecording(filepath);
}

void VisualizationSystem::startReplay() {
    if (recorder_) {
        recorder_->startReplay();
    }
}

void VisualizationSystem::stopReplay() {
    if (recorder_) {
        recorder_->stopReplay();
    }
}

bool VisualizationSystem::isReplaying() const {
    return recorder_ && recorder_->isReplaying();
}

size_t VisualizationSystem::getCurrentReplayFrame() const {
    return recorder_ ? recorder_->getCurrentReplayFrame() : 0;
}

size_t VisualizationSystem::getTotalFrames() const {
    return recorder_ ? recorder_->getTotalFrames() : 0;
}

bool VisualizationSystem::exportData(const std::string& outputPath, const std::string& format) {
    if (!recorder_) return false;
    return recorder_->exportData(outputPath, format);
}

bool VisualizationSystem::exportVisualizationData(const std::string& outputPath, const std::string& format) {
    if (!visualRecorder_) return false;
    
    if (format == "csv") {
        return visualRecorder_->exportCSV(outputPath);
    } else if (format == "json") {
        return visualRecorder_->exportJSON(outputPath);
    } else if (format == "binary") {
        return visualRecorder_->exportBinary(outputPath);
    }
    
    return false;
}

const PerformanceMonitor::Metrics& VisualizationSystem::getMetrics() const {
    static const PerformanceMonitor::Metrics emptyMetrics {
        0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0.0f
    };
    
    return monitor_ ? monitor_->getMetrics() : emptyMetrics;
}

void VisualizationSystem::setSettings(const VisualizationSettings& settings) {
    settings_ = settings;
    if (backend_) {
        backend_->setSettings(settings_);
    }
}

const VisualizationSettings& VisualizationSystem::getSettings() const {
    return settings_;
}

bool VisualizationSystem::isInitialized() const {
    return backend_ && backend_->isActive();
}

bool VisualizationSystem::isActive() const {
    return isInitialized();
}

void VisualizationSystem::setBackend(std::unique_ptr<VisualizationBackend> backend) {
    shutdown();
    backend_ = std::move(backend);
    if (backend_) {
        backend_->initialize();
        backend_->setSettings(settings_);
    }
}

void VisualizationSystem::exportVisualizationData(const std::string& outputPath, const std::string& format, bool compress) {
    if (!visualRecorder_) return;
    
    bool success = false;
    if (format == "csv") {
        success = visualRecorder_->exportCSV(outputPath);
    } else if (format == "json") {
        success = visualRecorder_->exportJSON(outputPath);
    } else if (format == "binary") {
        success = visualRecorder_->exportBinary(outputPath);
    }
    
    if (success && compress) {
        visualRecorder_->compress();
    }
}

void VisualizationSystem::compressRecording() {
    if (visualRecorder_) {
        visualRecorder_->compress();
    }
}

bool VisualizationSystem::isCompressed() const {
    return visualRecorder_ && visualRecorder_->isCompressed();
}

size_t VisualizationSystem::getTotalFrames() const {
    return visualRecorder_ ? visualRecorder_->getTotalFrames() : 0;
}

double VisualizationSystem::getDuration() const {
    return visualRecorder_ ? visualRecorder_->getDuration() : 0.0;
}

std::string VisualizationSystem::getFileSize() const {
    return visualRecorder_ ? visualRecorder_->getFileSize() : "0 bytes";
}

void VisualizationSystem::updateWithBrain(const Brain& brain) {
    if (!brain_) {
        brain_ = std::const_pointer_cast<Brain>(std::shared_ptr<const Brain>(&brain, [](const Brain*) {}));
    }
    
    setBrain(brain_);
}

// VisualizationInterface implementation
VisualizationInterface::VisualizationInterface() = default;
VisualizationInterface::~VisualizationInterface() {
    shutdown();
}

bool VisualizationInterface::initialize(const VisualizationSettings& settings) {
    visualizationSystem_ = std::make_shared<VisualizationSystem>();
    
    if (!visualizationSystem_->initialize(settings)) {
        visualizationSystem_.reset();
        return false;
    }
    
    active_ = true;
    return true;
}

bool VisualizationInterface::initialize(std::shared_ptr<class Config> config) {
    config_ = config;
    
    VisualizationSettings settings;
    
    if (auto updateRateOpt = config->get<double>("visualization_update_rate")) {
        settings.networkUpdateRate = *updateRateOpt;
        settings.activityUpdateRate = *updateRateOpt;
    }
    
    if (auto enableRecordingOpt = config->get<bool>("visualization_recording_enabled")) {
        settings.enableRecording = *enableRecordingOpt;
    }
    
    if (auto recordingFormatOpt = config->get<std::string>("visualization_recording_format")) {
        settings.recordingFormat = *recordingFormatOpt;
    }
    
    if (auto recordingIntervalOpt = config->get<size_t>("visualization_recording_interval")) {
        settings.recordingInterval = *recordingIntervalOpt;
    }
    
    if (auto compressOpt = config->get<bool>("visualization_recording_compress")) {
        settings.compressRecording = *compressOpt;
    }
    
    if (auto maxFramesOpt = config->get<size_t>("visualization_recording_max_frames")) {
        settings.maxRecordedFrames = *maxFramesOpt;
    }
    
    if (auto backendOpt = config->get<std::string>("visualization_backend")) {
        settings.backend = *backendOpt;
    }
    
    if (auto headlessOpt = config->get<bool>("visualization_headless")) {
        settings.headless = *headlessOpt;
    }
    
    return initialize(settings);
}

void VisualizationInterface::update() {
    if (visualizationSystem_) {
        visualizationSystem_->update();
    }
}

void VisualizationInterface::render() {
    if (visualizationSystem_) {
        visualizationSystem_->render();
    }
}

void VisualizationInterface::close() {
    if (visualizationSystem_) {
        visualizationSystem_.reset();
    }
    active_ = false;
}

bool VisualizationInterface::isActive() const {
    return active_ && visualizationSystem_ && visualizationSystem_->isActive();
}

void VisualizationInterface::setUpdateRate(double hz) {
    updateRate_ = hz;
    if (visualizationSystem_) {
        auto& settings = visualizationSystem_->getSettings();
        settings.networkUpdateRate = hz;
        settings.activityUpdateRate = hz;
    }
}

double VisualizationInterface::getUpdateRate() const {
    return updateRate_;
}

void VisualizationInterface::visualizeNetwork(const class Brain& brain) {
    if (visualizationSystem_) {
        visualizationSystem_->setStepAndTime(0, 0.0);
    }
}

void VisualizationInterface::visualizeActivity(const class Brain& brain) {
    if (visualizationSystem_) {
        visualizationSystem_->setStepAndTime(0, 0.0);
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<class SpikeEvent>& spikes) {
    if (visualizationSystem_) {
    }
}

void VisualizationInterface::visualizeWeights(const class Brain& brain) {
    if (visualizationSystem_) {
        visualizationSystem_->setStepAndTime(0, 0.0);
    }
}

void VisualizationInterface::setBrain(std::shared_ptr<class Brain> brain) {
    if (visualizationSystem_) {
        visualizationSystem_->setBrain(brain);
    }
}

void VisualizationInterface::updateWithBrain(const class Brain& brain, size_t step, double time) {
    if (visualizationSystem_) {
        visualizationSystem_->setStepAndTime(step, time);
        visualizationSystem_->updateWithBrain(brain);
    }
}

bool VisualizationInterface::startRecording(const std::string& filepath) {
    if (visualizationSystem_) {
        return visualizationSystem_->startRecording(filepath);
    }
    return false;
}

void VisualizationInterface::stopRecording() {
    if (visualizationSystem_) {
        visualizationSystem_->stopRecording();
    }
}

bool VisualizationInterface::isRecording() const {
    if (visualizationSystem_) {
        return visualizationSystem_->isRecording();
    }
    return false;
}

bool VisualizationInterface::loadReplay(const std::string& filepath) {
    if (visualizationSystem_) {
        return visualizationSystem_->loadReplay(filepath);
    }
    return false;
}

void VisualizationInterface::startReplay() {
    if (visualizationSystem_) {
        visualizationSystem_->startReplay();
    }
}

void VisualizationInterface::stopReplay() {
    if (visualizationSystem_) {
        visualizationSystem_->stopReplay();
    }
}

bool VisualizationInterface::isReplaying() const {
    if (visualizationSystem_) {
        return visualizationSystem_->isReplaying();
    }
    return false;
}

size_t VisualizationInterface::getCurrentReplayFrame() const {
    if (visualizationSystem_) {
        return visualizationSystem_->getCurrentReplayFrame();
    }
    return 0;
}

size_t VisualizationInterface::getTotalFrames() const {
    if (visualizationSystem_) {
        return visualizationSystem_->getTotalFrames();
    }
    return 0;
}

bool VisualizationInterface::exportData(const std::string& outputPath, const std::string& format) {
    if (visualizationSystem_) {
        return visualizationSystem_->exportData(outputPath, format);
    }
    return false;
}

bool VisualizationInterface::exportVisualizationData(const std::string& outputPath, const std::string& format) {
    if (visualizationSystem_) {
        return visualizationSystem_->exportVisualizationData(outputPath, format);
    }
    return false;
}

const PerformanceMonitor::Metrics& VisualizationInterface::getMetrics() const {
    static const PerformanceMonitor::Metrics emptyMetrics {
        0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0.0f
    };
    
    if (visualizationSystem_) {
        return visualizationSystem_->getMetrics();
    }
    return emptyMetrics;
}

void VisualizationInterface::setSettings(const VisualizationSettings& settings) {
    if (visualizationSystem_) {
        visualizationSystem_->setSettings(settings);
    }
}

const VisualizationSettings& VisualizationInterface::getSettings() const {
    static VisualizationSettings emptySettings;
    if (visualizationSystem_) {
        return visualizationSystem_->getSettings();
    }
    return emptySettings;
}

bool VisualizationInterface::isInitialized() const {
    return active_ && visualizationSystem_ && visualizationSystem_->isInitialized();
}

bool VisualizationInterface::isActive() const {
    return isActive();
}

// VisualizationSystem implementation
VisualizationSystem::VisualizationSystem() = default;
VisualizationSystem::~VisualizationSystem() {
    shutdown();
}

bool VisualizationSystem::initialize(const VisualizationSettings& settings) {
    settings_ = settings;
    
    // Create backend
    switch (settings.backend.compare("web")) {
        case 0:
            backend_ = std::make_unique<WebVisualizationBackend>();
            if (settings.webPort.empty()) {
                return false;
            }
            break;
        case 1:
        default:
            backend_ = std::make_unique<ConsoleVisualizationBackend>();
            break;
    }
    
    if (!backend_->initialize()) {
        return false;
    }
    
    // Create recorder
    recorder_ = std::make_unique<CSVSensorRecorder>();
    
    // Create performance monitor
    monitor_ = std::make_unique<RealTimePerformanceMonitor>();
    
    // Create visualization recorder
    visualRecorder_ = std::make_unique<RealTimeVisualizationRecorder>();
    
    return true;
}

void VisualizationSystem::shutdown() {
    if (backend_) {
        backend_->shutdown();
        backend_.reset();
    }
    
    recorder_.reset();
    monitor_.reset();
    visualRecorder_.reset();
}

void VisualizationSystem::update() {
    if (!isInitialized()) return;
    
    // Update performance monitor
    if (monitor_) {
        monitor_->update();
    }
    
    // Update backend
    if (backend_) {
        backend_->update();
    }
}

void VisualizationSystem::render() {
    if (!isInitialized()) return;
    
    if (backend_) {
        backend_->render();
    }
}

void VisualizationSystem::setBrain(std::shared_ptr<Brain> brain) {
    brain_ = brain;
    if (brain_ && backend_ && std::dynamic_pointer_cast<ConsoleVisualizationBackend>(backend_)) {
        size_t neuronCount = brain_->getTotalNeuronCount();
        float avgFiringRate = brain_->getAverageFiringRate();
        
        std::unordered_map<RegionId, float> regionActivities;
        for (const auto& region : brain_->getRegions()) {
            float activity = static_cast<float>(region->getActiveNeuronCount()) / 
                           std::max(1.0f, static_cast<float>(region->getTotalNeuronCount()));
            regionActivities[region->getId()] = activity * 100.0f;
        }
        
        std::dynamic_pointer_cast<ConsoleVisualizationBackend>(backend_)
            ->setBrainData(neuronCount, avgFiringRate, regionActivities);
    }
}

void VisualizationSystem::setStepAndTime(size_t step, double time) {
    currentStep_ = step;
    currentTime_ = time;
}

void VisualizationSystem::setBrainAndStep(std::shared_ptr<Brain> brain, size_t step, double time) {
    setBrain(brain);
    setStepAndTime(step, time);
}

bool VisualizationSystem::startRecording(const std::string& filepath) {
    if (!recorder_) return false;
    
    std::string actualFilepath = filepath.empty() ? "nlm_recording.csv" : filepath;
    return recorder_->startRecording(actualFilepath, settings_);
}

void VisualizationSystem::stopRecording() {
    if (recorder_) {
        recorder_->stopRecording();
    }
}

bool VisualizationSystem::isRecording() const {
    return recorder_ && recorder_->isRecording();
}

bool VisualizationSystem::loadReplay(const std::string& filepath) {
    if (!recorder_) return false;
    return recorder_->loadRecording(filepath);
}

void VisualizationSystem::startReplay() {
    if (recorder_) {
        recorder_->startReplay();
    }
}

void VisualizationSystem::stopReplay() {
    if (recorder_) {
        recorder_->stopReplay();
    }
}

bool VisualizationSystem::isReplaying() const {
    return recorder_ && recorder_->isReplaying();
}

size_t VisualizationSystem::getCurrentReplayFrame() const {
    return recorder_ ? recorder_->getCurrentReplayFrame() : 0;
}

size_t VisualizationSystem::getTotalFrames() const {
    return recorder_ ? recorder_->getTotalFrames() : 0;
}

bool VisualizationSystem::exportData(const std::string& outputPath, const std::string& format) {
    if (!recorder_) return false;
    return recorder_->exportData(outputPath, format);
}

bool VisualizationSystem::exportVisualizationData(const std::string& outputPath, const std::string& format) {
    if (!visualRecorder_) return false;
    
    if (format == "csv") {
        return visualRecorder_->exportCSV(outputPath);
    } else if (format == "json") {
        return visualRecorder_->exportJSON(outputPath);
    } else if (format == "binary") {
        return visualRecorder_->exportBinary(outputPath);
    }
    
    return false;
}

const PerformanceMonitor::Metrics& VisualizationSystem::getMetrics() const {
    static const PerformanceMonitor::Metrics emptyMetrics {
        0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0.0f
    };
    
    return monitor_ ? monitor_->getMetrics() : emptyMetrics;
}

void VisualizationSystem::setSettings(const VisualizationSettings& settings) {
    settings_ = settings;
    if (backend_) {
        backend_->setSettings(settings_);
    }
}

const VisualizationSettings& VisualizationSystem::getSettings() const {
    return settings_;
}

bool VisualizationSystem::isInitialized() const {
    return backend_ && backend_->isActive();
}

bool VisualizationSystem::isActive() const {
    return isInitialized();
}

void VisualizationSystem::setBackend(std::unique_ptr<VisualizationBackend> backend) {
    shutdown();
    backend_ = std::move(backend);
    if (backend_) {
        backend_->initialize();
        backend_->setSettings(settings_);
    }
}

void VisualizationSystem::exportVisualizationData(const std::string& outputPath, const std::string& format, bool compress) {
    if (!visualRecorder_) return;
    
    bool success = false;
    if (format == "csv") {
        success = visualRecorder_->exportCSV(outputPath);
    } else if (format == "json") {
        success = visualRecorder_->exportJSON(outputPath);
    } else if (format == "binary") {
        success = visualRecorder_->exportBinary(outputPath);
    }
    
    if (success && compress) {
        visualRecorder_->compress();
    }
}

void VisualizationSystem::compressRecording() {
    if (visualRecorder_) {
        visualRecorder_->compress();
    }
}

bool VisualizationSystem::isCompressed() const {
    return visualRecorder_ && visualRecorder_->isCompressed();
}

size_t VisualizationSystem::getTotalFrames() const {
    return visualRecorder_ ? visualRecorder_->getTotalFrames() : 0;
}

double VisualizationSystem::getDuration() const {
    return visualRecorder_ ? visualRecorder_->getDuration() : 0.0;
}

std::string VisualizationSystem::getFileSize() const {
    return visualRecorder_ ? visualRecorder_->getFileSize() : "0 bytes";
}

void VisualizationSystem::updateWithBrain(const Brain& brain) {
    if (!brain_) {
        brain_ = std::const_pointer_cast<Brain>(std::shared_ptr<const Brain>(&brain, [](const Brain*) {}));
    }
    
    setBrain(brain_);
}

// Global factory functions
VisualizationInterface* createVisualizationInterface() {
    return new VisualizationInterface();
}

VisualizationBackend* createConsoleVisualizationBackend() {
    return new ConsoleVisualizationBackend();
}

VisualizationBackend* createWebVisualizationBackend() {
    return new WebVisualizationBackend();
}

SimulationRecorder* createCSVSensorRecorder() {
    return new CSVSensorRecorder();
}

PerformanceMonitor* createRealTimePerformanceMonitor() {
    return new RealTimePerformanceMonitor();
}

VisualizationRecorder* createRealTimeVisualizationRecorder() {
    return new RealTimeVisualizationRecorder();
}

} // namespace nlm