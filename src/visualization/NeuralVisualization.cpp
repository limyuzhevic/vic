// NeuralVisualization Implementation

#include "NeuralVisualization.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <thread>

namespace nlm {

struct NeuralVisualization::Impl {
    Brain* brain;
    std::atomic<bool> running;
    std::thread updateThread;
    std::chrono::steady_clock::time_point lastUpdate;
    float updateRate;
    
    // Visualization state
    std::string mode;
    float neuronSize;
    float synapseWidth;
    float activityThreshold;
    bool showSpikes;
    bool showConnections;
    std::string cameraYaw;
    std::string cameraPitch;
    float cameraDistance;
    std::string viewMode;
    std::string colorScheme;
    
    // Recording
    bool recording;
    std::string recordingFilename;
    std::ofstream recordingFile;
    
    // Frame data for recording
    struct FrameData {
        SimulationStep step;
        Timestamp time;
        std::vector<float> neuronActivations;
        std::vector<float> regionActivities;
        std::vector<float> synapseWeights;
        std::vector<std::pair<NeuronId, NeuronId>> spikeEvents;
    };
    
    std::vector<FrameData> recordedFrames;
    
    Impl() : brain(nullptr), running(false), updateRate(0.0f),
             neuronSize(1.0f), synapseWidth(0.5f), activityThreshold(0.1f),
             showSpikes(true), showConnections(true),
             cameraDistance(10.0f), viewMode("perspective"), colorScheme("activity"),
             recording(false) {}
};

NeuralVisualization::NeuralVisualization()
    : pImpl(new Impl)
{
}

NeuralVisualization::~NeuralVisualization() {
    stop();
}

void NeuralVisualization::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Neural visualization initialized");
}

void NeuralVisualization::start() {
    if (pImpl->running) return;
    
    pImpl->running = true;
    pImpl->lastUpdate = std::chrono::steady_clock::now();
    pImpl->updateThread = std::thread(&NeuralVisualization::updateThreadFunc, this);
    NLM_LOG_INFO("Neural visualization started");
}

void NeuralVisualization::stop() {
    if (!pImpl->running) return;
    
    pImpl->running = false;
    if (pImpl->updateThread.joinable()) {
        pImpl->updateThread.join();
    }
    NLM_LOG_INFO("Neural visualization stopped");
}

void NeuralVisualization::update(const Brain* brain, SimulationStep step, Timestamp time) {
    std::lock_guard<std::mutex> lock(updateMutex_);
    processFrame(brain, step, time);
}

void NeuralVisualization::setMode(const std::string& mode) {
    pImpl->mode = mode;
}

void NeuralVisualization::addCallback(std::function<void(const Brain* brain)> callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callbacks_.push_back(callback);
}

void NeuralVisualization::setUpdateInterval(int interval) {
    // Convert milliseconds to update rate
    pImpl->updateRate = 1000.0f / static_cast<float>(interval);
}

bool NeuralVisualization::saveToFile(const std::string& filepath) {
    // Save current visualization state
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for saving: " + filepath);
        return false;
    }
    
    file << "# Neural Visualization State" << std::endl;
    file << "# Mode: " << pImpl->mode << std::endl;
    file << "# Neuron Size: " << pImpl->neuronSize << std::endl;
    file << "# Synapse Width: " << pImpl->synapseWidth << std::endl;
    file << "# Activity Threshold: " << pImpl->activityThreshold << std::endl;
    
    // Save brain state summary
    if (pImpl->brain) {
        file << "# Brain State Summary:" << std::endl;
        file << "# Regions: " << pImpl->brain->getRegionCount() << std::endl;
        file << "# Total Neurons: " << pImpl->brain->getTotalNeuronCount() << std::endl;
        file << "# Total Synapses: " << pImpl->brain->getTotalSynapseCount() << std::endl;
        file << "# Active Neurons: " << pImpl->brain->getActiveNeuronCount() << std::endl;
        file << "# Firing Neurons: " << pImpl->brain->getFiringNeuronCount() << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Visualization saved to: " + filepath);
    return true;
}

bool NeuralVisualization::exportBrainState(const std::string& filepath) {
    // Export brain state for external analysis
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for brain state export: " + filepath);
        return false;
    }
    
    // Export brain state using Brain::save
    if (pImpl->brain) {
        return pImpl->brain->save(filepath);
    }
    
    return false;
}

bool NeuralVisualization::isRunning() const {
    return pImpl->running;
}

std::string NeuralVisualization::getCurrentMode() const {
    return pImpl->mode;
}

size_t NeuralVisualization::getFrameCount() const {
    return pImpl->recordedFrames.size();
}

float NeuralVisualization::getUpdateRate() const {
    return pImpl->updateRate;
}

void NeuralVisualization::setNeuronSize(float size) {
    pImpl->neuronSize = size;
}

void NeuralVisualization::setSynapseWidth(float width) {
    pImpl->synapseWidth = width;
}

void NeuralVisualization::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = threshold;
}

void NeuralVisualization::enableSpikes(bool enable) {
    pImpl->showSpikes = enable;
}

void NeuralVisualization::enableConnections(bool enable) {
    pImpl->showConnections = enable;
}

void NeuralVisualization::setCameraAngle(float yaw, float pitch, float distance) {
    pImpl->cameraDistance = distance;
}

void NeuralVisualization::setViewMode(const std::string& mode) {
    pImpl->viewMode = mode;
}

void NeuralVisualization::setColorScheme(const std::string& scheme) {
    pImpl->colorScheme = scheme;
}

void NeuralVisualization::startRecording(const std::string& filename) {
    pImpl->recording = true;
    pImpl->recordingFilename = filename;
    pImpl->recordingFile.open(filename, std::ios::binary);
    NLM_LOG_INFO("Started recording to: " + filename);
}

void NeuralVisualization::stopRecording() {
    pImpl->recording = false;
    if (pImpl->recordingFile.is_open()) {
        pImpl->recordingFile.close();
    }
    NLM_LOG_INFO("Stopped recording");
}

bool NeuralVisualization::isRecording() const {
    return pImpl->recording;
}

void NeuralVisualization::updateThreadFunc() {
    const float targetInterval = 1000.0f / pImpl->updateRate;  // milliseconds
    
    while (pImpl->running) {
        auto startTime = std::chrono::steady_clock::now();
        
        if (pImpl->brain) {
            update(pImpl->brain, pImpl->brain->getTotalSpikeCount(),
                   static_cast<float>(pImpl->brain->getTotalSpikeCount()) * 0.001f);
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        if (elapsed < targetInterval) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(targetInterval - elapsed)));
        }
    }
}

void NeuralVisualization::processFrame(const Brain* brain, SimulationStep step, Timestamp time) {
    if (!brain) return;
    
    // Record frame data if recording
    if (pImpl->recording) {
        FrameData frame;
        frame.step = step;
        frame.time = time;
        
        // Capture current brain state
        frame.neuronActivations.reserve(brain->getTotalNeuronCount());
        frame.regionActivities.reserve(brain->getRegionCount());
        frame.synapseWeights.reserve(brain->getTotalSynapseCount());
        
        // Get neurons from all regions
        for (const auto& region : brain->getRegions()) {
            for (const auto* neuron : region->getAllNeurons()) {
                float activation = std::abs(neuron->getState().membranePotential - 
                                         neuron->getState().restingPotential);
                frame.neuronActivations.push_back(activation);
            }
        }
        
        // Get region activities
        for (size_t i = 0; i < brain->getRegionCount(); ++i) {
            const auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
            if (region) {
                float regionActivity = region->getAverageFiringRate();
                frame.regionActivities.push_back(regionActivity);
            }
        }
        
        // Get synapse weights
        for (const auto& region : brain->getRegions()) {
            for (const auto* synapse : region->getSynapses()) {
                frame.synapseWeights.push_back(synapse->getWeight());
            }
        }
        
        pImpl->recordedFrames.push_back(frame);
        
        // Write to file if recording
        if (pImpl->recordingFile.is_open()) {
            // Write compressed frame data
            pImpl->recordingFile.write(reinterpret_cast<const char*>(&frame), sizeof(FrameData));
        }
    }
    
    // Update callbacks
    std::lock_guard<std::mutex> callbackLock(callbackMutex_);
    for (const auto& callback : callbacks_) {
        callback(brain);
    }
    
    pImpl->frameCount_++;
}

void NeuralVisualization::drawNeurons(const Brain* brain) {
    // Draw neurons based on current visualization settings
}

void NeuralVisualization::drawSynapses(const Brain* brain) {
    // Draw synapses based on current visualization settings
}

void NeuralVisualization::drawConnections(const Brain* brain) {
    // Draw neural connections
}

void NeuralVisualization::drawSpikes(const Brain* brain, Timestamp time) {
    // Draw spike events
}

} // namespace nlm