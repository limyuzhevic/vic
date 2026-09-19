#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/Vision.hpp"
#include "../sensory/Audio.hpp"
#include "../sensory/InternalSignals.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

class AgentBrain::Impl {
public:
    std::shared_ptr<Brain> brain_;
    SimpleWorld* world_;
    
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    Impl(std::shared_ptr<Brain> brain) : brain_(brain), world_(nullptr),
        rewardModulationEnabled_(false), structuralPlasticityEnabled_(false),
        developmentEnabled_(false), curiosityEnabled_(false) {}
};

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain) : pImpl(std::make_unique<Impl>(brain)) {}

AgentBrain::~AgentBrain() = default;

AgentBrain::AgentBrain(AgentBrain&&) noexcept = default;
AgentBrain& AgentBrain::operator=(AgentBrain&&) noexcept = default;

void AgentBrain::initialize(SimpleWorld& world) {
    pImpl->world_ = &world;
}

size_t AgentBrain::getSensoryInputSize() const {
    if (!pImpl->world_) return 0;
    return pImpl->world_->getSensoryPercept().getAllSignals().size();
}

size_t AgentBrain::getMotorOutputSize() const {
    return static_cast<size_t>(MotorCommand::Count);
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!pImpl->brain_) return;
    
    // Convert percept to SensoryInput for brain
    auto visionInput = percept.getVision();
    pImpl->brain_->receiveSensoryInput(visionInput);
    
    // Inject internal signals if enabled
    if (pImpl->curiosityEnabled_) {
        float curiosityLevel = getCuriosityLevel();
        if (curiosityLevel > 0.1f) {
            pImpl->brain_->injectCurrentToNeurons(NeuronType::Internal, curiosityLevel * 2.0f);
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!pImpl->brain_ || !pImpl->world_) {
        return MotorCommand::Wait;
    }
    
    // Get all regions to find motor output
    auto& regions = pImpl->brain_->getRegions();
    if (regions.empty()) {
        return MotorCommand::Wait;
    }
    
    // Check motor neuron activity
    auto* motorRegion = regions[0].get();
    auto& populations = motorRegion->getPopulations();
    
    float maxActivity = 0.0f;
    size_t maxActivityIndex = 0;
    
    for (size_t i = 0; i < populations.size(); ++i) {
        auto& pop = populations[i];
        if (pop->getNeuronType() == NeuronType::Motor) {
            float activity = pop->getAverageFiringRate();
            if (activity > maxActivity) {
                maxActivity = activity;
                maxActivityIndex = i;
            }
        }
    }
    
    // Convert activity to motor command
    if (maxActivity < 0.01f) {
        return MotorCommand::Wait;
    }
    
    // Simple mapping based on activity level
    if (maxActivityIndex == 0) return MotorCommand::MoveForward;
    else if (maxActivityIndex == 1) return MotorCommand::MoveBackward;
    else if (maxActivityIndex == 2) return MotorCommand::TurnLeft;
    else if (maxActivityIndex == 3) return MotorCommand::TurnRight;
    else if (maxActivityIndex == 4) return MotorCommand::LookLeft;
    else if (maxActivityIndex == 5) return MotorCommand::LookRight;
    else if (maxActivityIndex == 6) return MotorCommand::Interact;
    else return MotorCommand::Wait;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!pImpl->brain_) return;
    
    float predictionError = reward - predictedReward;
    
    if (pImpl->rewardModulationEnabled_) {
        // Apply dopamine-based reward modulation
        float modulation = std::min(1.0f, reward * 0.1f);
        
        // Inject to neuromodulatory neurons
        pImpl->brain_->injectCurrentToNeurons(NeuronType::Modulatory, modulation);
        
        // Apply prediction error learning
        if (std::abs(predictionError) > 0.01f) {
            // Update predictions based on error
            if (predictionError > 0.0f) {
                pImpl->brain_->injectCurrentToNeurons(NeuronType::Internal, predictionError * 0.5f);
            }
        }
    }
}

void AgentBrain::updateDevelopment(float timestep) {
    if (!pImpl->brain_) return;
    
    if (pImpl->developmentEnabled_) {
        // Update developmental stage based on activity
        float avgFiringRate = pImpl->brain_->getAverageFiringRate();
        
        if (avgFiringRate > 5.0f) {
            pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (avgFiringRate > 2.0f) {
            pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {
            pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!pImpl->brain_) return DevelopmentalStage::Initial;
    return pImpl->brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    // Simplified neuromodulation level based on curiosity
    if (!pImpl->curiosityEnabled_) return 0.0f;
    return getCuriosityLevel() * 0.5f;
}

float AgentBrain::getCuriosityLevel() const {
    if (!pImpl->curiosityEnabled_) return 0.0f;
    
    // Simplified curiosity based on sensory novelty
    if (!pImpl->world_) return 0.1f;
    
    auto percept = pImpl->world_->getSensoryPercept();
    auto visionData = percept.getVision().getData();
    
    // Compute novelty as variance of vision data
    if (visionData.empty()) return 0.1f;
    
    float mean = std::accumulate(visionData.begin(), visionData.end(), 0.0f) / visionData.size();
    float variance = 0.0f;
    for (float v : visionData) {
        variance += (v - mean) * (v - mean);
    }
    variance /= visionData.size();
    
    return std::min(1.0f, variance * 2.0f);
}

float AgentBrain::getNoveltyLevel() const {
    return getCuriosityLevel() * 0.8f; // Simplified relationship
}

float AgentBrain::getPredictionError() const {
    if (!pImpl->brain_) return 0.0f;
    return 0.0f; // Placeholder - would use actual prediction error system
}

void AgentBrain::reset() {
    if (!pImpl->brain_) return;
    pImpl->brain_->reset();
}

std::shared_ptr<Brain> AgentBrain::getBrain() const {
    return pImpl->brain_;
}

void AgentBrain::enableRewardModulation(bool enable) {
    pImpl->rewardModulationEnabled_ = enable;
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    pImpl->structuralPlasticityEnabled_ = enable;
}

void AgentBrain::enableDevelopment(bool enable) {
    pImpl->developmentEnabled_ = enable;
}

void AgentBrain::enableCuriosity(bool enable) {
    pImpl->curiosityEnabled_ = enable;
}

bool AgentBrain::isRewardModulationEnabled() const {
    return pImpl->rewardModulationEnabled_;
}

bool AgentBrain::isStructuralPlasticityEnabled() const {
    return pImpl->structuralPlasticityEnabled_;
}

bool AgentBrain::isDevelopmentEnabled() const {
    return pImpl->developmentEnabled_;
}

bool AgentBrain::isCuriosityEnabled() const {
    return pImpl->curiosityEnabled_;
}

} // namespace nlm