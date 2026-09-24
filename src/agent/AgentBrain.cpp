#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Validate input
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    // Initialize motor and sensory neuron groups
    try {
        for (const auto& region : brain_->getRegions()) {
            if (!region) continue;
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) continue;
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) continue;
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error initializing neuron groups: ") + e.what());
        // Continue with empty groups rather than failing
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    try {
        // Initialize vision buffer with world dimensions
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        
        // Initialize developmental state
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Validate world parameters
        if (world.getVisionWidth() == 0 || world.getVisionHeight() == 0) {
            NLM_LOG_WARN("Invalid world dimensions, using defaults");
        }
        
        // Log initialization with detailed statistics
        std::stringstream ss;
        ss << "AgentBrain initialized with ";
        ss << sensoryVision_.size() << " vision sensory neurons, ";
        ss << sensoryTouch_.size() << " touch sensory neurons, ";
        ss << sensoryInternal_.size() << " internal sensory neurons, ";
        ss << sensoryProprioception_.size() << " proprioceptive neurons, ";
        ss << motorForward_.size() << " forward motor neurons, ";
        ss << motorBackward_.size() << " backward motor neurons, ";
        ss << motorTurnLeft_.size() << " turn left motor neurons, ";
        ss << motorTurnRight_.size() << " turn right motor neurons, ";
        ss << motorInteract_.size() << " interact motor neurons, ";
        ss << motorWait_.size() << " wait motor neurons";
        
        NLM_LOG_INFO(ss.str());
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error initializing AgentBrain: ") + e.what());
        // Reset to safe state
        previousVision_.clear();
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        // Continue with empty groups rather than failing
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Calculate actual sensory input size based on available neurons
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    size_t visionSize = 256;  // 16x16 vision grid
    size_t touchSize = 8;    // Touch sensors
    size_t internalSize = 4; // Internal states
    size_t proprioceptionSize = 6; // Body position/orientation
    
    size_t totalSize = visionSize + touchSize + internalSize + proprioceptionSize;
    
    // Validate size is reasonable
    if (totalSize == 0) {
        NLM_LOG_WARN("Invalid sensory input size, using default");
        return 256; // Default to vision size
    }
    
    return totalSize;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Calculate actual motor output size based on available motor neurons
    // One motor neuron per action type
    size_t moveForward = motorForward_.size();
    size_t moveBackward = motorBackward_.size();
    size_t turnLeft = motorTurnLeft_.size();
    size_t turnRight = motorTurnRight_.size();
    size_t interact = motorInteract_.size();
    size_t wait = motorWait_.size();
    
    size_t totalMotorOutput = moveForward + moveBackward + turnLeft + turnRight + 
                            interact + wait;
    
    // Validate we have at least some motor capabilities
    if (totalMotorOutput == 0) {
        NLM_LOG_WARN("No motor neurons available, using default action set");
        return 6; // Default to 6 basic actions
    }
    
    NLM_LOG_INFO("Motor output configured with " + std::to_string(totalMotorOutput) + 
                 " motor neurons across " + std::to_string(moveForward + moveBackward + 
                 turnLeft + turnRight + interact + wait) + " action groups");
    
    return totalMotorOutput;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    try {
        if (!brain_) {
            NLM_LOG_WARN("Brain pointer is null, skipping sensory processing");
            return;
        }
        
        // Validate percept data
        if (!percept.getVision().empty()) {
            validateVisionData(percept.getVision());
        }
        
        // Process vision input with error handling
        processVisionInput(percept.getVision());
        
        // Process touch input with error handling
        processTouchInput(percept.getTouch());
        
        // Process internal signals with error handling
        processInternalInput(percept.getInternal());
        
        // Process proprioception with error handling
        processProprioceptionInput(percept.getProprioception());
        
        // Compute novelty with enhanced error handling
        computeNovelty(percept.getVision());
        
        // Update curiosity based on novelty
        if (curiosityEnabled_) {
            updateCuriosityLevel();
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error processing sensory input: ") + e.what());
        // Reset to safe state
        resetSensoryState();
    }
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    // Vision input (256 values -> sensoryVision_ neurons)
    size_t visionNeurons = sensoryVision_.size();
    size_t visionDataSize = vision.size();
    
    if (visionNeurons == 0) {
        NLM_LOG_WARN("No vision neurons available");
        return;
    }
    
    for (size_t i = 0; i < visionNeurons && i < visionDataSize; ++i) {
        if (sensoryVision_[i]) {
            try {
                // Inject current proportional to vision intensity with better scaling
                float current = vision[i] * 5.0f;  // Scale factor
                // Clamp current to reasonable range
                current = std::clamp(current, -50.0f, 50.0f);
                sensoryVision_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error processing vision neuron ") + std::to_string(i) + ": " + e.what());
            }
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    // Touch input (8 values -> sensoryTouch_ neurons)
    size_t touchNeurons = sensoryTouch_.size();
    size_t touchDataSize = touch.size();
    
    if (touchNeurons == 0) {
        NLM_LOG_WARN("No touch neurons available");
        return;
    }
    
    for (size_t i = 0; i < touchNeurons && i < touchDataSize; ++i) {
        if (sensoryTouch_[i]) {
            try {
                float current = touch[i] * 8.0f;  // Collision signal
                current = std::clamp(current, -100.0f, 100.0f);
                sensoryTouch_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error processing touch neuron ") + std::to_string(i) + ": " + e.what());
            }
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& intern) {
    // Internal signals (4 values -> sensoryInternal_ neurons)
    size_t internalNeurons = sensoryInternal_.size();
    size_t internalDataSize = intern.size();
    
    if (internalNeurons == 0) {
        NLM_LOG_WARN("No internal neurons available");
        return;
    }
    
    for (size_t i = 0; i < internalNeurons && i < internalDataSize; ++i) {
        if (sensoryInternal_[i]) {
            try {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                current = std::clamp(current, -50.0f, 50.0f);
                sensoryInternal_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error processing internal neuron ") + std::to_string(i) + ": " + e.what());
            }
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    size_t proprioNeurons = sensoryProprioception_.size();
    size_t proprioDataSize = proprio.size();
    
    if (proprioNeurons == 0) {
        NLM_LOG_WARN("No proprioceptive neurons available");
        return;
    }
    
    for (size_t i = 0; i < proprioNeurons && i < proprioDataSize; ++i) {
        if (sensoryProprioception_[i]) {
            try {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                current = std::clamp(current, -30.0f, 30.0f);
                sensoryProprioception_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error processing proprioceptive neuron ") + std::to_string(i) + ": " + e.what());
            }
        }
    }
}

void AgentBrain::validateVisionData(const std::vector<float>& vision) {
    // Validate vision data quality
    if (vision.empty()) {
        NLM_LOG_WARN("Empty vision data received");
        return;
    }
    
    // Check for NaN or extreme values
    for (size_t i = 0; i < vision.size(); ++i) {
        if (std::isnan(vision[i])) {
            NLM_LOG_WARN(std::string("NaN value detected in vision data at index ") + std::to_string(i));
        } else if (std::isinf(vision[i])) {
            NLM_LOG_WARN(std::string("Infinite value detected in vision data at index ") + std::to_string(i));
        }
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t minSize = std::min(vision.size(), previousVision_.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(minSize, 1);
        
        // Apply biological decay model
        noveltyLevel_ = std::max(0.0f, noveltyLevel_ * sensoryNoveltyDecay_);
        
        // Store for next time
        previousVision_ = vision;
    }
}

void AgentBrain::updateCuriosityLevel() {
    // Update curiosity based on novelty and prediction error
    curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void AgentBrain::resetSensoryState() {
    // Reset sensory state to safe values
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    previousVision_.clear();
    
    // Reset neuromodulation levels
    dopamineLevel_ = 0.0f;
    predictionError_ = 0.0f;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
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
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
