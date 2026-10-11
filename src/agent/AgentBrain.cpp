// AgentBrain.cpp - Implementation
#include "AgentBrain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}

AgentBrain::~AgentBrain() {}

void AgentBrain::initialize(const SimpleWorld& world) {
    world_ = &world;
    brain_->initialize();
    
    // Setup neuromodulation
    enableRewardModulation(true);
    enableCuriosity(true);
    enableStructuralPlasticity(true);
    enableDevelopment(true);
    
    NLM_LOG_INFO("AgentBrain initialized");
}

void AgentBrain::update(float dt) {
    if (!brain_ || !world_) return;
    
    static SimulationStep step = 0;
    
    // Process sensory input
    processSensoryInput();
    
    // Brain step
    brain_->step(step, step * dt);
    
    // Get motor command
    lastCommand_ = decodeMotorCommand();
    
    // Apply action to world
    applyAction(lastCommand_);
    
    // Compute and apply reward
    float reward = world_->computeReward(brain_->getRegions()[0].get());
    applyRewardModulation(reward, 0.0f);
    
    // Update development
    updateDevelopment(dt);
    
    step++;
}

void AgentBrain::processSensoryInput() {
    if (!brain_ || !world_) return;
    
    // Get observation from world
    SensoryPercept percept = world_->observe(brain_->getRegions()[0].get());
    
    // Send observation to brain
    brain_->receiveSensoryInput(percept);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    MotorCommand cmd;
    
    // Simple action selection based on neural activity
    if (brain_->getRegions().empty()) {
        return cmd;
    }
    
    auto* region = brain_->getRegions()[0].get();
    auto neurons = region->getAllNeurons();
    
    if (neurons.empty()) {
        return cmd;
    }
    
    // Find most active neuron
    Neuron* mostActive = nullptr;
    float maxActivity = 0.0f;
    
    for (auto* neuron : neurons) {
        float activity = neuron->getFiringRate();
        if (activity > maxActivity) {
            maxActivity = activity;
            mostActive = neuron;
        }
    }
    
    // Convert neural activity to action
    if (mostActive) {
        float actionValue = mostActive->getMembranePotential() / 100.0f;
        actionValue = std::max(-1.0f, std::min(1.0f, actionValue));
        
        cmd.type = ActionType::Continuous;
        cmd.value = actionValue;
        cmd.timestamp = SimulationClock::getCurrentTime();
    }
    
    return cmd;
}

void AgentBrain::applyAction(const MotorCommand& command) {
    if (!world_ || !brain_ || brain_->getRegions().empty()) return;
    
    world_->applyAction(brain_->getRegions()[0].get(), command);
}

void AgentBrain::applyRewardModulation(float reward, float predictionError) {
    if (!brain_) return;
    
    // Apply reward to neuromodulation systems
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        dopamine->processReward(reward, predictionError);
    }
    
    // Apply to curiosity system
    auto* curiosity = brain_->getCuriosity();
    if (curiosity) {
        curiosity->processNovelty(reward, predictionError);
    }
}

void AgentBrain::updateDevelopment(float dt) {
    if (!brain_) return;
    
    auto* development = brain_->getDevelopmentSystem();
    if (development) {
        development->update(dt);
    }
}

float AgentBrain::getNoveltyLevel() const {
    if (!brain_) return 0.0f;
    auto* novelty = brain_->getNovelty();
    return novelty ? novelty->getNoveltyLevel() : 0.0f;
}

float AgentBrain::getCuriosityLevel() const {
    if (!brain_) return 0.0f;
    auto* curiosity = brain_->getCuriosity();
    return curiosity ? curiosity->getCuriosityLevel() : 0.0f;
}

float AgentBrain::getNeuromodulationLevel() const {
    if (!brain_) return 0.0f;
    auto* dopamine = brain_->getDopamine();
    return dopamine ? dopamine->getLevel() : 0.0f;
}

void AgentBrain::enableRewardModulation(bool enabled) {
    if (!brain_) return;
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        dopamine->setEnabled(enabled);
    }
}

void AgentBrain::enableCuriosity(bool enabled) {
    if (!brain_) return;
    auto* curiosity = brain_->getCuriosity();
    if (curiosity) {
        curiosity->setEnabled(enabled);
    }
}

void AgentBrain::enableStructuralPlasticity(bool enabled) {
    if (!brain_) return;
    auto* development = brain_->getDevelopmentSystem();
    if (development) {
        development->setStructuralPlasticityEnabled(enabled);
    }
}

void AgentBrain::enableDevelopment(bool enabled) {
    if (!brain_) return;
    auto* development = brain_->getDevelopmentSystem();
    if (development) {
        development->setDevelopmentEnabled(enabled);
    }
}

Brain* AgentBrain::getBrain() const {
    return brain_.get();
}

const MotorCommand& AgentBrain::getLastCommand() const {
    return lastCommand_;
}

} // namespace nlm