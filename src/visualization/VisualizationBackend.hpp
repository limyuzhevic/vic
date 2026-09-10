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

// CSV implementation of SimulationRecorder
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

// Implementation of RealTimePerformanceMonitor
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

// Implementation of RealTimeVisualizationRecorder
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