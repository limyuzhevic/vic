// SensoryMotorManager implementation
#include "BrainComponents.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

SensoryMotorManager::SensoryMotorManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config)
    : brain_(brain), config_(config) {}

bool SensoryMotorManager::initialize() {
    NLM_LOG_INFO("Initializing SensoryMotorManager...");
    
    // Collect sensory and motor neurons from brain regions
    for (auto& region : brain_->getRegions()) {
        auto* sensoryPop = region->getPopulation(RegionId(1)); // Assuming sensory population is in region 1
        auto* motorPop = region->getPopulation(RegionId(2));   // Assuming motor population is in region 2
        
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                sensoryNeurons_.push_back(neuron);
            }
        }
        
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                motorNeurons_.push_back(neuron);
            }
        }
    }
    
    NLM_LOG_INFO("SensoryMotorManager initialized - " +
                 std::to_string(sensoryNeurons_.size()) + " sensory neurons, " +
                 std::to_string(motorNeurons_.size()) + " motor neurons");
    return true;
}

void SensoryMotorManager::reset() {
    NLM_LOG_INFO("Resetting SensoryMotorManager...");
    sensoryNeurons_.clear();
    motorNeurons_.clear();
    NLM_LOG_INFO("SensoryMotorManager reset complete");
}

void SensoryMotorManager::update() {
    // Sensory-motor updates are handled during simulation steps
}

void SensoryMotorManager::logStatus() const {
    NLM_LOG_INFO("=== SensoryMotorManager Status ===");
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(sensoryNeurons_.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(motorNeurons_.size()));
    
    // Count active neurons
    size_t activeSensory = 0;
    for (auto* neuron : sensoryNeurons_) {
        if (neuron->isFiring()) activeSensory++;
    }
    
    size_t activeMotor = 0;
    for (auto* neuron : motorNeurons_) {
        if (neuron->isFiring()) activeMotor++;
    }
    
    NLM_LOG_INFO("Active sensory neurons: " + std::to_string(activeSensory));
    NLM_LOG_INFO("Active motor neurons: " + std::to_string(activeMotor));
}

void SensoryMotorManager::processSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons based on input
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons_.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        sensoryNeurons_[i]->injectCurrent(normalizedValue);
        
        // Store in working memory if significant
        if (std::abs(normalizedValue) > 0.5f && brain_->getWorkingMemory()) {
            brain_->getWorkingMemory()->storeToNeuron(sensoryNeurons_[i]->getId(), std::abs(normalizedValue) / 10.0f);
        }
    }
}

void SensoryMotorManager::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void SensoryMotorManager::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<Action> SensoryMotorManager::generateAction() const {
    // Simple action selection based on motor neuron activity
    if (motorNeurons_.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons_) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

} // namespace nlm