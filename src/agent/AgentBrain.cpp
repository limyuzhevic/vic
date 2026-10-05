#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

namespace nlm {

// MotorNeuronManager implementation
MotorNeuronManager::MotorNeuronManager(std::shared_ptr<Brain> brain)
    : brain_(brain) {
    if (brain_) {
        distributeMotorNeurons();
    }
}

void MotorNeuronManager::distributeMotorNeurons() {
    if (!brain_) return;
    
    // Clear existing groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    motorLookLeft_.clear();
    motorLookRight_.clear();
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute motor neurons based on configuration ratios
                    size_t totalDistributed = motorForward_.size() * AgentBrainConfig::motorForwardRatio +
                                             motorBackward_.size() * AgentBrainConfig::motorBackwardRatio +
                                             motorTurnLeft_.size() * AgentBrainConfig::motorTurnLeftRatio +
                                             motorTurnRight_.size() * AgentBrainConfig::motorTurnRightRatio +
                                             motorInteract_.size() * AgentBrainConfig::motorInteractRatio +
                                             motorWait_.size() * AgentBrainConfig::motorWaitRatio +
                                             motorLookLeft_.size() * AgentBrainConfig::motorLookLeftRatio +
                                             motorLookRight_.size() * AgentBrainConfig::motorLookRightRatio;
                    
                    switch (totalDistributed % 8) {
                        case 0: motorForward_.push_back(n); break;
                        case 1: motorBackward_.push_back(n); break;
                        case 2: motorTurnLeft_.push_back(n); break;
                        case 3: motorTurnRight_.push_back(n); break;
                        case 4: motorInteract_.push_back(n); break;
                        case 5: motorWait_.push_back(n); break;
                        case 6: motorLookLeft_.push_back(n); break;
                        case 7: motorLookRight_.push_back(n); break;
                    }
                }
            }
        }
    }
}

MotorCommand MotorNeuronManager::decodeCommand(const std::vector<Neuron*>& neurons, float threshold) const {
    if (neurons.empty()) return MotorCommand::Wait;
    
    // Calculate average activity in neuron group
    float sum = 0.0f;
    for (const Neuron* n : neurons) {
        if (n) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
    }
    float activity = sum / neurons.size();
    
    // Only return command if activity is above threshold
    if (activity >= threshold) {
        // This is a simplified approach - in reality, we'd need mapping logic
        // For now, return first available motor command
        return MotorCommand::MoveForward;
    }
    
    return MotorCommand::Wait;
}

// SensoryProcessor implementation
SensoryProcessor::SensoryProcessor(AgentBrain* agentBrain)
    : agentBrain_(agentBrain), noveltyLevel_(0.0f), noveltyDecayRate_(AgentBrainConfig::noveltyDecayRate) {
}

void SensoryProcessor::processVision(const std::vector<float>& vision) {
    if (agentBrain_ && !vision.empty()) {
        computeNovelty(vision);
        agentBrain_->previousVision_ = vision;
    }
}

void SensoryProcessor::processTouch(const std::vector<float>& touch) {
    // Process touch input
    if (agentBrain_) {
        // Can be extended for more complex touch processing
    }
}

void SensoryProcessor::processInternal(const std::vector<float>& internal) {
    // Process internal signals
    if (agentBrain_) {
        // Can be extended for homeostatic signal processing
    }
}

void SensoryProcessor::processProprioception(const std::vector<float>& proprioception) {
    // Process proprioceptive signals
    if (agentBrain_) {
        // Can be extended for body position/velocity processing
    }
}

void SensoryProcessor::processAudio(const std::vector<float>& audio) {
    // Process audio signals (placeholder for future implementation)
}

void SensoryProcessor::computeNovelty(const std::vector<float>& currentVision) {
    if (previousVision_.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    size_t minSize = std::min(currentVision.size(), previousVision_.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(currentVision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / std::max<size_t>(minSize, 1);
    
    // Apply decay
    noveltyLevel_ *= noveltyDecayRate_;
}

// NeuromodulationSystem implementation
NeuromodulationSystem::NeuromodulationSystem(AgentBrain* agentBrain)
    : agentBrain_(agentBrain), dopamineLevel_(0.0f), noveltyLevel_(0.0f), curiosityLevel_(0.0f),
      predictionError_(0.0f), expectedReward_(0.0f), plasticityModifier_(1.0f), 
      rewardModulationEnabled_(AgentBrainConfig::enableRewardModulationDefault) {
}

void NeuromodulationSystem::applyRewardModulation(float reward, float predictedReward) {
    if (!agentBrain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Update dopamine signal (based on prediction error)
    updateDopamine(predictionError_);
    
    // Apply to brain synapses with eligibility traces
    if (agentBrain_->getBrain()) {
        for (const auto& region : agentBrain_->getBrain()->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > 0.001f) {
                    // Apply reward-modulated weight change
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace
                    syn->decayEligibilityTrace(AgentBrainConfig::eligibilityTraceDecay);
                }
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    adjustPlasticity(dopamineLevel_);
}

void NeuromodulationSystem::updateDopamine(float predictionError) {
    // Dopamine-like signal (clamped to reasonable range)
    dopamineLevel_ = predictionError;
    dopamineLevel_ = std::clamp(dopamineLevel_, -AgentBrainConfig::predictionErrorClamping, 
                                AgentBrainConfig::predictionErrorClamping);
}

void NeuromodulationSystem::adjustPlasticity(float dopamineLevel) {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    plasticityModifier_ = plasticityFactor;
    
    // Update STDP parameters if available
    if (agentBrain_ && agentBrain_->getBrain()) {
        auto* stdp = agentBrain_->getBrain()->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
}

void NeuromodulationSystem::updateCuriosity(float novelty, float predictionError) {
    noveltyLevel_ = novelty;
    curiosityLevel_ = novelty * 2.0f + std::abs(predictionError) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

// DevelopmentSystem implementation
DevelopmentSystem::DevelopmentSystem(AgentBrain* agentBrain)
    : agentBrain_(agentBrain), developmentalAge_(0.0), plasticityModifier_(1.0f), 
      currentStage_(DevelopmentalStage::Initial) {
}

void DevelopmentSystem::update(double timestep) {
    if (!agentBrain_ || !agentBrain_->getBrain() || !agentBrain_->isDevelopmentEnabled()) {
        return;
    }
    
    developmentalAge_ += timestep;
    updateStage(timestep);
    
    // Update structural plasticity if enabled
    if (agentBrain_->isStructuralPlasticityEnabled()) {
        updateStructuralPlasticity(plasticityModifier_);
    }
}

void DevelopmentSystem::updateStage(double timestep) {
    // Simple developmental stages based on age
    if (developmentalAge_ < AgentBrainConfig::initialStageDuration) {
        plasticityModifier_ = 1.0f;
        currentStage_ = DevelopmentalStage::Initial;
    } else if (developmentalAge_ < AgentBrainConfig::criticalPeriodDuration) {
        plasticityModifier_ = 0.8f;
        currentStage_ = DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge_ < AgentBrainConfig::maturationDuration) {
        plasticityModifier_ = 0.5f;
        currentStage_ = DevelopmentalStage::Maturation;
    } else {
        plasticityModifier_ = 0.2f;
        currentStage_ = DevelopmentalStage::Adult;
    }
}

void DevelopmentSystem::updateStructuralPlasticity(float plasticity) {
    if (!agentBrain_ || !agentBrain_->getBrain()) return;
    
    auto* sp = agentBrain_->getBrain()->getStructuralPlasticity();
    if (sp) {
        // Synaptogenesis rate depends on current plasticity
        float synRate = 0.0001f * plasticity;
        float pruneRate = 0.00001f * (2.0f - plasticity);
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

// CuriositySystem implementation
CuriositySystem::CuriositySystem(AgentBrain* agentBrain)
    : agentBrain_(agentBrain), enabled_(AgentBrainConfig::enableCuriosityDefault), 
      curiosityLevel_(0.0f), brain_(nullptr) {
    if (agentBrain_) {
        brain_ = agentBrain_->getBrain();
    }
}

MotorCommand CuriositySystem::selectWithCuriosity(MotorCommand defaultCommand) {
    if (!enabled_ || !brain_ || curiosityLevel_ <= AgentBrainConfig::curiosityExplorationThreshold) {
        return defaultCommand;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConfig::curiosityExplorationThreshold) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCommand;
}

void CuriositySystem::updateCuriosity(float novelty, float predictionError) {
    curiosityLevel_ = novelty * 2.0f + std::abs(predictionError) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

// AgentBrain implementation
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
    : brain_(brain), config_(config), cachedMotorCount_(0), motorCountValid_(false) {
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot create AgentBrain with null brain pointer");
        return;
    }
    
    // Initialize subsystems
    motorManager_ = std::make_unique<MotorNeuronManager>(brain_);
    sensoryProcessor_ = std::make_unique<SensoryProcessor>(this);
    neuromodulation_ = std::make_unique<NeuromodulationSystem>(this);
    development_ = std::make_unique<DevelopmentSystem>(this);
    curiosity_ = std::make_unique<CuriositySystem>(this);
    
    NLM_LOG_INFO("AgentBrain created with " + std::to_string(brain_->getTotalNeuronCount()) + 
                 " neurons");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeSubsystems(const SimpleWorld& world) {
    if (brain_) {
        motorManager_->updateDistribution();
    }
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    motorCountValid_ = false;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!validateBrainState()) {
        NLM_LOG_ERROR("Invalid brain state, cannot initialize");
        return;
    }
    
    initializeSubsystems(world);
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryProcessor_->getNoveltyLevel()) + " vision sensory neurons, " +
                 std::to_string(motorManager_->getMotorForward().size()) + " forward motor neurons");
}

bool AgentBrain::validateBrainState() const {
    return brain_ != nullptr && brain_->getTotalNeuronCount() > 0;
}

void AgentBrain::safetyCheck() const {
    if (!brain_) {
        NLM_LOG_WARNING("Brain is null during safety check");
    }
    
    // Check for potential issues
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto& neuron : pop->getNeurons()) {
                // Check for NaN or extreme values
                float potential = neuron->getState().membranePotential;
                if (std::isnan(potential) || std::isinf(potential)) {
                    NLM_LOG_ERROR("Invalid neuron potential detected: " + std::to_string(potential));
                }
            }
        }
    }
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    safetyCheck();
    
    if (!brain_) return;
    
    // Vision input processing
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        // Update novelty detection
        sensoryProcessor_->processVision(vision);
        
        // Inject into sensory neurons
        for (size_t i = 0; i < sensoryProcessor_->getNoveltyLevel() && i < vision.size(); ++i) {
            // Simple injection logic - can be enhanced
            // For now, we just track novelty
        }
    }
    
    // Update neuromodulation with novelty and prediction error
    neuromodulation_->updateCuriosity(sensoryProcessor_->getNoveltyLevel(), 
                                      neuromodulation_->getPredictionError());
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!validateBrainState()) {
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = motorManager_->decodeCommand(motorManager_->getMotorForward(), 
                                                        config_.motorActivityThreshold);
    
    // Apply curiosity-based exploration
    if (curiosity_->isCuriosityEnabled() && curiosity_->getCuriosityLevel() > config_.curiosityThreshold) {
        decoded = curiosity_->selectWithCuriosity(decoded);
    }
    
    // Notify event listeners
    notifyEventListeners("motor_command_selected", std::to_string(static_cast<int>(decoded)));
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!validateBrainState()) return;
    
    neuromodulation_->applyRewardModulation(reward, predictedReward);
    
    // Notify event listeners
    notifyEventListeners("neuromodulation_updated", 
                        std::to_string(neuromodulation_->getDopamineLevel()) + "," +
                        std::to_string(neuromodulation_->getPredictionError()));
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!validateBrainState()) return;
    
    development_->update(timestep);
    
    // Notify event listeners
    notifyEventListeners("development_stage_changed", 
                        std::to_string(static_cast<int>(development_->getStage())));
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    return development_ ? development_->getStage() : DevelopmentalStage::Initial;
}

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodulation_ ? neuromodulation_->getDopamineLevel() : 0.0f;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosity_ ? curiosity_->getCuriosityLevel() : 0.0f;
}

float AgentBrain::getNoveltyLevel() const {
    return sensoryProcessor_ ? sensoryProcessor_->getNoveltyLevel() : 0.0f;
}

float AgentBrain::getPredictionError() const {
    return neuromodulation_ ? neuromodulation_->getPredictionError() : 0.0f;
}

void AgentBrain::reset() {
    if (!brain_) return;
    
    neuromodulation_->updateDopamine(0.0f);
    neuromodulation_->updateCuriosity(0.0f, 0.0f);
    
    development_->update(0.0);
    
    previousVision_.clear();
    motorCountValid_ = false;
    
    NLM_LOG_INFO("AgentBrain reset completed");
}

void AgentBrain::removeEventListener(AgentEventListener* listener) {
    auto it = std::find(eventListeners_.begin(), eventListeners_.end(), listener);
    if (it != eventListeners_.end()) {
        eventListeners_.erase(it);
    }
}

void AgentBrain::notifyEventListeners(const std::string& eventType, const std::string& data) const {
    for (auto* listener : eventListeners_) {
        if (listener) {
            // In a real implementation, we'd pass more structured event data
            // For now, we'll handle basic events
            if (eventType == "motor_command_selected") {
                try {
                    int cmd = std::stoi(data);
                    listener->onMotorCommandSelected(static_cast<MotorCommand>(cmd), 1.0f);
                } catch (...) {
                    NLM_LOG_WARNING("Failed to parse motor command from event data");
                }
            } else if (eventType == "neuromodulation_updated") {
                // Parse comma-separated values
                size_t pos = data.find(',');
                if (pos != std::string::npos) {
                    float dopamine = std::stof(data.substr(0, pos));
                    float predictionError = std::stof(data.substr(pos + 1));
                    listener->onNeuromodulationUpdated(dopamine, predictionError);
                }
            } else if (eventType == "development_stage_changed") {
                try {
                    int stage = std::stoi(data);
                    listener->onDevelopmentStageChanged(static_cast<DevelopmentalStage>(stage));
                } catch (...) {
                    NLM_LOG_WARNING("Failed to parse development stage from event data");
                }
            } else if (eventType == "sensory_processing") {
                listener->onSensoryProcessing(std::vector<float>()); // Simplified for now
            }
        }
    }
}

void AgentBrain::printDebugInfo() const {
    std::cout << "=== AgentBrain Debug Information ===" << std::endl;
    std::cout << "Total neurons: " << (brain_ ? brain_->getTotalNeuronCount() : 0) << std::endl;
    std::cout << "Active motor neurons: " << getActiveMotorNeurons() << std::endl;
    std::cout << "Firing neurons: " << getFiringNeuronCount() << std::endl;
    std::cout << "Dopamine level: " << getNeuromodulationLevel() << std::endl;
    std::cout << "Curiosity level: " << getCuriosityLevel() << std::endl;
    std::cout << "Novelty level: " << getNoveltyLevel() << std::endl;
    std::cout << "Prediction error: " << getPredictionError() << std::endl;
    std::cout << "Developmental stage: " << static_cast<int>(getDevelopmentalStage()) << std::endl;
    std::cout << "Reward modulation enabled: " << isRewardModulationEnabled() << std::endl;
    std::cout << "Curiosity enabled: " << isCuriosityEnabled() << std::endl;
    std::cout << "=== End Debug Information ===" << std::endl;
}

std::string AgentBrain::getDebugInfo() const {
    std::stringstream ss;
    ss << "=== AgentBrain Debug Information ===\n";
    ss << "Total neurons: " << (brain_ ? brain_->getTotalNeuronCount() : 0) << "\n";
    ss << "Active motor neurons: " << getActiveMotorNeurons() << "\n";
    ss << "Firing neurons: " << getFiringNeuronCount() << "\n";
    ss << "Dopamine level: " << getNeuromodulationLevel() << "\n";
    ss << "Curiosity level: " << getCuriosityLevel() << "\n";
    ss << "Novelty level: " << getNoveltyLevel() << "\n";
    ss << "Prediction error: " << getPredictionError() << "\n";
    ss << "Developmental stage: " << static_cast<int>(getDevelopmentalStage()) << "\n";
    ss << "Reward modulation enabled: " << isRewardModulationEnabled() << "\n";
    ss << "Curiosity enabled: " << isCuriosityEnabled() << "\n";
    ss << "=== End Debug Information ===\n";
    return ss.str();
}

size_t AgentBrain::getActiveMotorNeurons() const {
    if (motorCountValid_) {
        return cachedMotorCount_;
    }
    
    size_t total = 0;
    if (motorManager_) {
        total += motorManager_->getMotorForward().size();
        total += motorManager_->getMotorBackward().size();
        total += motorManager_->getMotorTurnLeft().size();
        total += motorManager_->getMotorTurnRight().size();
        total += motorManager_->getMotorInteract().size();
        total += motorManager_->getMotorWait().size();
        total += motorManager_->getMotorLookLeft().size();
        total += motorManager_->getMotorLookRight().size();
    }
    
    cachedMotorCount_ = total;
    motorCountValid_ = true;
    return total;
}

size_t AgentBrain::getFiringNeuronCount() const {
    if (!brain_) return 0;
    
    size_t firingCount = 0;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto& neuron : pop->getNeurons()) {
                float potential = neuron->getState().membranePotential;
                if (potential > neuron->getState().restingPotential + 0.1f) {
                    firingCount++;
                }
            }
        }
    }
    
    return firingCount;
}
size_t AgentBrain::getSensoryInputSize() const {
    // Vision (256) + touch (8) + internal (4) + proprioception (6) + audio (0)
    return 274;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action (8 actions)
    return 8;
}

} // namespace nlm