// Visualization backend implementation
#pragma once

#include "VisualizationBackend.hpp"
#include <iostream>
#include <unordered_map>
#include "../core/Types/Types.hpp"

namespace nlm {

ConsoleVisualizationBackend::ConsoleVisualizationBackend() = default;
ConsoleVisualizationBackend::~ConsoleVisualizationBackend() = default;

bool ConsoleVisualizationBackend::initialize() {
    active_ = true;
    return true;
}

void ConsoleVisualizationBackend::shutdown() {
    active_ = false;
}

void ConsoleVisualizationBackend::update() {
    // In console mode, updates are handled directly in render()
}

void ConsoleVisualizationBackend::render() {
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

void ConsoleVisualizationBackend::setSettings(const VisualizationSettings& settings) {
    settings_ = settings;
}

bool ConsoleVisualizationBackend::isActive() const {
    return active_;
}

void ConsoleVisualizationBackend::setWindowSize(int width, int height) {
    width_ = width;
    height_ = height;
}

int ConsoleVisualizationBackend::getWidth() const { return width_; }
int ConsoleVisualizationBackend::getHeight() const { return height_; }

void ConsoleVisualizationBackend::setBrainData(size_t neuronCount, float averageFiringRate,
                                            const std::unordered_map<RegionId, float>& regionActivities) {
    neuronCount_ = neuronCount;
    averageFiringRate_ = averageFiringRate;
    regionActivities_ = regionActivities;
}

WebVisualizationBackend::WebVisualizationBackend() = default;
WebVisualizationBackend::~WebVisualizationBackend() = default;

bool WebVisualizationBackend::initialize() {
    active_ = false;
    return true;
}

void WebVisualizationBackend::shutdown() {
    active_ = false;
}

void WebVisualizationBackend::update() {
    // Web updates would happen in background threads
}

void WebVisualizationBackend::render() {
    // Web rendering is done in browser
}

void WebVisualizationBackend::setSettings(const VisualizationSettings& settings) {
    settings_ = settings;
}

bool WebVisualizationBackend::isActive() const {
    return active_;
}

bool WebVisualizationBackend::startServer(const std::string& port) {
    // Would start web server here
    return true;
}

void WebVisualizationBackend::stopServer() {
    active_ = false;
}

// CSVSensorRecorder implementation
CSVSensorRecorder::CSVSensorRecorder() = default;
CSVSensorRecorder::~CSVSensorRecorder() {
    if (file_.is_open()) {
        file_.close();
    }
}

bool CSVSensorRecorder::startRecording(const std::string& filepath, const VisualizationSettings& settings) {
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

void CSVSensorRecorder::stopRecording() {
    if (file_.is_open()) {
        file_.close();
    }
}

bool CSVSensorRecorder::isRecording() const {
    return file_.is_open();
}

void CSVSensorRecorder::recordFrame(const Brain& brain, size_t step, double time) {
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

bool CSVSensorRecorder::loadRecording(const std::string& filepath) {
    // For simplicity, just clear current data
    frames_.clear();
    steps_.clear();
    times_.clear();
    return true;
}

void CSVSensorRecorder::startReplay() {
    replayIndex_ = 0;
}

void CSVSensorRecorder::stopReplay() {
    replayIndex_ = 0;
}

bool CSVSensorRecorder::isReplaying() const {
    return replayIndex_ < frames_.size();
}

size_t CSVSensorRecorder::getCurrentReplayFrame() const {
    return replayIndex_;
}

size_t CSVSensorRecorder::getTotalFrames() const {
    return frames_.size();
}

std::vector<double> CSVSensorRecorder::getRecordedTimes() const {
    return times_;
}

std::vector<size_t> CSVSensorRecorder::getRecordedSteps() const {
    return steps_;
}

const Brain* CSVSensorRecorder::getFrameState(size_t frameIndex) {
    return nullptr; // Would restore full brain state here
}

bool CSVSensorRecorder::exportData(const std::string& outputPath, const std::string& format) {
    if (format == "csv") {
        return exportCSV(outputPath);
    }
    return false;
}

bool CSVSensorRecorder::exportCSV(const std::string& filepath) {
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

void CSVSensorRecorder::setCompressionLevel(size_t level) {
    compressionLevel_ = level;
}

// RealTimePerformanceMonitor implementation
RealTimePerformanceMonitor::RealTimePerformanceMonitor() : lastUpdateTime_(0.0), isEnabled_(true) {}
RealTimePerformanceMonitor::~RealTimePerformanceMonitor() = default;

void RealTimePerformanceMonitor::update() {
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

const PerformanceMonitor::Metrics& RealTimePerformanceMonitor::getMetrics() const {
    return metrics_;
}

void RealTimePerformanceMonitor::setUpdateRate(double hz) {
    updateRate_ = hz;
}

void RealTimePerformanceMonitor::enableMetrics(bool enable) {
    isEnabled_ = enable;
}

// RealTimeVisualizationRecorder implementation
RealTimeVisualizationRecorder::RealTimeVisualizationRecorder() = default;
RealTimeVisualizationRecorder::~RealTimeVisualizationRecorder() = default;

void RealTimeVisualizationRecorder::recordFrame(size_t step, double time, const Brain& brain,
                                            const PerformanceMonitor::Metrics& metrics) {
    frames_.push_back(FrameData{step, time, &brain, metrics});
    
    // Limit frame count if configured
    if (maxFrames_ > 0 && frames_.size() > maxFrames_) {
        frames_.erase(frames_.begin(), frames_.begin() + (frames_.size() - maxFrames_));
    }
}

bool RealTimeVisualizationRecorder::exportCSV(const std::string& filepath) const {
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

bool RealTimeVisualizationRecorder::exportJSON(const std::string& filepath) const {
    // Would implement JSON export here
    return false;
}

bool RealTimeVisualizationRecorder::exportBinary(const std::string& filepath) const {
    // Would implement binary export here
    return false;
}

void RealTimeVisualizationRecorder::compress() {
    // Would implement compression here
}

bool RealTimeVisualizationRecorder::isCompressed() const {
    return compressed_;
}

size_t RealTimeVisualizationRecorder::getTotalFrames() const {
    return frames_.size();
}

double RealTimeVisualizationRecorder::getDuration() const {
    if (frames_.empty()) return 0.0;
    return frames_.back().time - frames_.front().time;
}

std::string RealTimeVisualizationRecorder::getFileSize() const {
    // Would calculate actual file size
    return "N/A";
}

void RealTimeVisualizationRecorder::setMaxFrames(size_t max) {
    maxFrames_ = max;
}

void RealTimeVisualizationRecorder::setCompression(bool enable) {
    compressed_ = enable;
}

// Implementation of VisualizationSystem
class VisualizationSystemImpl {
public:
    VisualizationSettings settings_;
    
    std::unique_ptr<VisualizationBackend> backend_;
    std::unique_ptr<SimulationRecorder> recorder_;
    std::unique_ptr<PerformanceMonitor> monitor_;
    std::unique_ptr<VisualizationRecorder> visualRecorder_;
    
    std::shared_ptr<Brain> brain_;
    
    size_t currentStep_ = 0;
    double currentTime_ = 0.0;
    
    bool isInitialized() const {
        return backend_ && backend_->isActive();
    }
    
    bool isActive() const {
        return isInitialized();
    }
};

// Factory functions for creating visualization components
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