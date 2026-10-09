#include "VisualizationInterface.hpp"

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    VisualizationMode currentMode;
    RecordingMode recordingMode;
    bool recording;
    std::string recordingFilename;
    size_t frameCount;
    
    std::vector<SpikeEvent> spikeHistory;
    std::map<std::string, std::vector<double>> timeSeriesData;
    VisualizationStats stats;
    
    std::vector<std::vector<unsigned char>> recordedFrames;
    std::vector<double> recordingTimestamps;
    
    bool showNeuronInfo;
    bool showSynapseInfo;
    bool showConnections;
    bool showHeatmap;
    bool showTimeSeries;
    bool showStatistics;
    bool showHelp;
    
    double fps;
    double frameTime;
    size_t drawCalls;
    
    float cameraX, cameraY, cameraZ;
    float cameraPitch, cameraYaw;
    float zoomLevel;
    
    const Brain* brain;
    
    Impl() : active(false), updateRate(60.0), currentMode(VisualizationMode::OVERVIEW),
             recordingMode(RecordingMode::NONE), recording(false), frameCount(0),
             showNeuronInfo(true), showSynapseInfo(true), showConnections(true),
             showHeatmap(true), showTimeSeries(true), showStatistics(true), showHelp(false),
             fps(0.0), frameTime(0.0), drawCalls(0), cameraX(0.0f), cameraY(10.0f), 
             cameraZ(30.0f), cameraPitch(0.0f), cameraYaw(0.0f), zoomLevel(1.0f),
             brain(nullptr) {
    }
    
    ~Impl() {
        close();
    }
    
    void close() {
        active = false;
        recording = false;
        brain = nullptr;
    }
    
    bool initialize(int width, int height, const std::string& title) {
        active = true;
        return true;
    }
    
    void update(const Brain& brain) {
        updateStats(brain);
        
        if (auto* spikeSystem = brain.getSpikeSystem()) {
            const auto& history = spikeSystem->getSpikeHistory();
            spikeHistory.assign(history.begin(), history.end());
        }
        
        addTimeSeriesData("Neurons", static_cast<double>(brain.getTotalNeuronCount()));
        addTimeSeriesData("Synapses", static_cast<double>(brain.getTotalSynapseCount()));
        addTimeSeriesData("Active Neurons", static_cast<double>(brain.getActiveNeuronCount()));
        addTimeSeriesData("Firing Neurons", static_cast<double>(brain.getFiringNeuronCount()));
        addTimeSeriesData("Avg Firing Rate", brain.getAverageFiringRate());
        
        if (recording) {
            recordFrame();
        }
    }
    
    void updateStats(const Brain& brain) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
        
        stats.neuronCount = brain.getTotalNeuronCount();
        stats.synapseCount = brain.getTotalSynapseCount();
        stats.activeNeuronCount = brain.getActiveNeuronCount();
        stats.firingNeuronCount = brain.getFiringNeuronCount();
        stats.averageFiringRate = brain.getAverageFiringRate();
        stats.totalSpikeCount = brain.getTotalSpikeCount();
        stats.lastUpdateTime = duration.count();
        stats.startTime = 0.0;
    }
    
    void render() {
        renderUI();
    }
    
    void renderUI() {
        if (showStatistics) {
            std::cout << "=== NLM Visualization ===" << std::endl;
            std::cout << "FPS: " << fps << std::endl;
            std::cout << "Neurons: " << stats.neuronCount << std::endl;
            std::cout << "Synapses: " << stats.synapseCount << std::endl;
            std::cout << "Active: " << stats.activeNeuronCount << std::endl;
            std::cout << "Firing: " << stats.firingNeuronCount << std::endl;
            std::cout << "Avg Firing Rate: " << stats.averageFiringRate << std::endl;
            std::cout << "Total Spikes: " << stats.totalSpikeCount << std::endl;
            std::cout << "Recording: " << (recording ? "Yes" : "No") << std::endl;
            std::cout << "========================" << std::endl;
        }
        
        if (showHelp && currentMode == VisualizationMode::OVERVIEW) {
            std::cout << "--- Controls ---" << std::endl;
            std::cout << "[1-5]: Change visualization mode" << std::endl;
            std::cout << "[r]: Reset camera" << std::endl;
            std::cout << "[t]: Toggle time series" << std::endl;
            std::cout << "[h]: Toggle help" << std::endl;
            std::cout << "[s]: Start/stop recording" << std::endl;
            std::cout << "[e]: Export data" << std::endl;
            std::cout << "[ESC]: Exit" << std::endl;
        }
    }
    
    void addTimeSeriesData(const std::string& name, double value) {
        timeSeriesData[name].push_back(value);
        
        if (timeSeriesData[name].size() > 1000) {
            timeSeriesData[name].erase(timeSeriesData[name].begin());
        }
    }
    
    void recordFrame() {
        frameCount++;
    }
    
    void toggleRecording(const std::string& filename) {
        if (recording) {
            recording = false;
            recordingMode = RecordingMode::NONE;
            
            if (!recordingFilename.empty()) {
                exportData(recordingFilename);
            }
        } else {
            recording = true;
            recordingFilename = filename.empty() ? "nlm_recording_" + std::to_string(frameCount) : filename;
            frameCount = 0;
        }
    }
    
    bool exportData(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "NLM Visualization Data Export" << std::endl;
        file << "============================" << std::endl;
        file << "Timestamp: " << stats.lastUpdateTime << std::endl;
        file << "Neuron Count: " << stats.neuronCount << std::endl;
        file << "Synapse Count: " << stats.synapseCount << std::endl;
        file << "Active Neurons: " << stats.activeNeuronCount << std::endl;
        file << "Firing Neurons: " << stats.firingNeuronCount << std::endl;
        file << "Average Firing Rate: " << stats.averageFiringRate << std::endl;
        file << "Total Spikes: " << stats.totalSpikeCount << std::endl;
        file << std::endl;
        
        file << "Time Series Data:" << std::endl;
        for (const auto& pair : timeSeriesData) {
            file << pair.first << ": ";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                file << pair.second[i];
                if (i < pair.second.size() - 1) file << ", ";
            }
            file << std::endl;
        }
        
        file.close();
        return true;
    }
    
    void setVisualizationMode(VisualizationMode mode) {
        currentMode = mode;
    }
    
    VisualizationMode getVisualizationMode() const {
        return currentMode;
    }
    
    void setRecordingMode(RecordingMode mode) {
        recordingMode = mode;
    }
    
    void setCameraPosition(float x, float y, float z) {
        cameraX = x;
        cameraY = y;
        cameraZ = z;
    }
    
    void setCameraRotation(float pitch, float yaw) {
        cameraPitch = pitch;
        cameraYaw = yaw;
    }
    
    void zoom(float factor) {
        zoomLevel *= factor;
    }
    
    VisualizationStats getStats() const {
        return stats;
    }
    
    bool isActive() const {
        return active;
    }
    
    bool isRecording() const {
        return recording;
    }
    
    void setBrainForRendering(const Brain* b) {
        brain = b;
    }
    
    std::chrono::high_resolution_clock::time_point startTime;
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() {
    pImpl->close();
}

bool VisualizationInterface::initialize(int width, int height, const std::string& title) {
    return pImpl->initialize(width, height, title);
}

void VisualizationInterface::update(const Brain& brain) {
    pImpl->update(brain);
}

void VisualizationInterface::render() {
    pImpl->render();
}

void VisualizationInterface::close() {
    pImpl->close();
}

bool VisualizationInterface::isActive() const {
    return pImpl->isActive();
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->setUpdateRate(hz);
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->getUpdateRate();
}

void VisualizationInterface::setVisualizationMode(VisualizationMode mode) {
    pImpl->setVisualizationMode(mode);
}

VisualizationMode VisualizationInterface::getVisualizationMode() const {
    return pImpl->getVisualizationMode();
}

void VisualizationInterface::setRecordingMode(RecordingMode mode) {
    pImpl->setRecordingMode(mode);
}

void VisualizationInterface::toggleRecording(const std::string& filename) {
    pImpl->toggleRecording(filename);
}

bool VisualizationInterface::isRecording() const {
    return pImpl->isRecording();
}

VisualizationStats VisualizationInterface::getStats() const {
    return pImpl->getStats();
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeNeurons(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeSynapses(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeFiringRates(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeWeightMatrix(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeMonitoring(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeMemorySystems(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeNeuromodulation(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeRegionHeatmap(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeRegionalActivity(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::visualizeTimeSeries(const Brain& brain) {
    // Placeholder implementation
}

void VisualizationInterface::addTimeSeriesData(const std::string& name, double value) {
    pImpl->addTimeSeriesData(name, value);
}

void VisualizationInterface::visualizeDebugInfo(const Brain& brain) {
    // Simple debug output
    std::cout << "Debug Info: Neurons=" << brain.getTotalNeuronCount() 
              << " Synapses=" << brain.getTotalSynapseCount()
              << " Active=" << brain.getActiveNeuronCount() 
              << " Firing=" << brain.getFiringNeuronCount() 
              << " AvgRate=" << brain.getAverageFiringRate() << std::endl;
}

void VisualizationInterface::visualizePerformanceStats() {
    // Simple performance stats
    static size_t lastFrameCount = 0;
    if (frameCount - lastFrameCount >= 100) {
        if (frameCount > 0) {
            fps = 100.0 / (frameTime * 100);
        }
        lastFrameCount = frameCount;
    }
    
    std::cout << "Performance: FPS=" << fps 
              << " FrameTime=" << (frameTime * 1000) << "ms"
              << " DrawCalls=" << drawCalls << std::endl;
}

void VisualizationInterface::setCameraPosition(float x, float y, float z) {
    pImpl->setCameraPosition(x, y, z);
}

void VisualizationInterface::setCameraRotation(float pitch, float yaw) {
    pImpl->setCameraRotation(pitch, yaw);
}

void VisualizationInterface::zoom(float factor) {
    pImpl->zoom(factor);
}

void VisualizationInterface::setBrainForRendering(const Brain& brain) {
    pImpl->setBrainForRendering(&brain);
}

} // namespace nlm