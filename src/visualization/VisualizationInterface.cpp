#include "VisualizationInterface.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include <chrono>
#include <string>
#include <iostream>

namespace nlm {

class VisualizationInterface::Impl {
public:
    bool initialized_;
    bool paused_;
    float fps_;
    std::chrono::steady_clock::time_point lastFrameTime_;
    
    bool visionEnabled_;
    bool worldEnabled_;
    bool brainEnabled_;
    bool agentEnabled_;
    
    std::string recordingFile_;
    bool recording_;
    
    // Reference to systems (not ownership)
    SimpleWorld* world_;
    AgentBrain* agent_;
    Brain* brain_;
    
    Impl() : initialized_(false), paused_(false), fps_(0.0f), visionEnabled_(true),
             worldEnabled_(true), brainEnabled_(true), agentEnabled_(true),
             recording_(false), world_(nullptr), agent_(nullptr), brain_(nullptr) {}
};

VisualizationInterface::Impl::Impl() : initialized_(false), paused_(false), fps_(0.0f),
    visionEnabled_(true), worldEnabled_(true), brainEnabled_(true), agentEnabled_(true),
    recording_(false), world_(nullptr), agent_(nullptr), brain_(nullptr) {}

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    std::cout << "Initializing visualization system..." << std::endl;
    pImpl->initialized_ = true;
    pImpl->lastFrameTime_ = std::chrono::steady_clock::now();
    std::cout << "Visualization system initialized successfully." << std::endl;
    return true;
}

void VisualizationInterface::update(float timestep) {
    if (pImpl->paused_ || !pImpl->initialized_) return;
    
    // Calculate FPS
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - pImpl->lastFrameTime_);
    pImpl->lastFrameTime_ = currentTime;
    if (duration.count() > 0.0f) {
        pImpl->fps_ = 1.0f / duration.count();
    }
    
    // Update visualization components if enabled
    if (pImpl->world_ && pImpl->worldEnabled_) {
        std::cout << "[VIS] World: pos(" << pImpl->world_->getAgentBody().x << ", " 
                  << pImpl->world_->getAgentBody().y << ") energy(" 
                  << pImpl->world_->getAgentBody().energy << ")" << std::endl;
    }
    
    if (pImpl->agent_ && pImpl->agentEnabled_) {
        std::cout << "[VIS] Agent: stage(" << static_cast<int>(pImpl->agent_->getDevelopmentalStage()) << ") " 
                  << "curiosity(" << pImpl->agent_->getCuriosityLevel() << ") novelty(" 
                  << pImpl->agent_->getNoveltyLevel() << ")" << std::endl;
    }
    
    if (pImpl->brain_ && pImpl->brainEnabled_) {
        std::cout << "[VIS] Brain: neurons(" << pImpl->brain_->getTotalNeuronCount() << ") " 
                  << "spikes(" << pImpl->brain_->getTotalSpikeCount() << ") " 
                  << "firing(" << pImpl->brain_->getAverageFiringRate() << ")" << std::endl;
    }
}

void VisualizationInterface::render() {
    if (pImpl->paused_ || !pImpl->initialized_) return;
    
    std::cout << "\n=== NLM VISUALIZATION ===" << std::endl;
    std::cout << "FPS: " << pImpl->fps_ << std::endl;
    std::cout << "Paused: " << (pImpl->paused_ ? "Yes" : "No") << std::endl;
    std::cout << "Recording: " << (pImpl->recording_ ? "Yes" : "No") << std::endl;
    std::cout << "=====================\n" << std::endl;
}

void VisualizationInterface::startRecording(const std::string& filepath) {
    if (!pImpl->initialized_) {
        std::cout << "Error: Visualization not initialized" << std::endl;
        return;
    }
    
    pImpl->recording_ = true;
    pImpl->recordingFile_ = filepath;
    std::cout << "Started recording visualization to: " << filepath << std::endl;
}

void VisualizationInterface::stopRecording() {
    if (pImpl->recording_) {
        pImpl->recording_ = false;
        std::cout << "Stopped recording visualization." << std::endl;
    }
}

void VisualizationInterface::togglePause() {
    pImpl->paused_ = !pImpl->paused_;
    std::cout << (pImpl->paused_ ? "Paused visualization." : "Unpaused visualization.") << std::endl;
}

bool VisualizationInterface::isPaused() const {
    return pImpl->paused_;
}

float VisualizationInterface::getFPS() const {
    return pImpl->fps_;
}

void VisualizationInterface::enableVision(bool enable) {
    pImpl->visionEnabled_ = enable;
    std::cout << (enable ? "Enabled vision visualization." : "Disabled vision visualization.") << std::endl;
}

void VisualizationInterface::enableWorld(bool enable) {
    pImpl->worldEnabled_ = enable;
    std::cout << (enable ? "Enabled world visualization." : "Disabled world visualization.") << std::endl;
}

void VisualizationInterface::enableBrain(bool enable) {
    pImpl->brainEnabled_ = enable;
    std::cout << (enable ? "Enabled brain visualization." : "Disabled brain visualization.") << std::endl;
}

void VisualizationInterface::enableAgent(bool enable) {
    pImpl->agentEnabled_ = enable;
    std::cout << (enable ? "Enabled agent visualization." : "Disabled agent visualization.") << std::endl;
}

bool VisualizationInterface::isVisionEnabled() const {
    return pImpl->visionEnabled_;
}

bool VisualizationInterface::isWorldEnabled() const {
    return pImpl->worldEnabled_;
}

bool VisualizationInterface::isBrainEnabled() const {
    return pImpl->brainEnabled_;
}

bool VisualizationInterface::isAgentEnabled() const {
    return pImpl->agentEnabled_;
}

} // namespace nlm