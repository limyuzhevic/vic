#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Error handling utilities
namespace ErrorUtils {
    template<typename T>
    bool checkNullptr(const T* ptr, const std::string& context) {
        if (!ptr) {
            NLM_LOG_ERROR("Null pointer error in " + context);
            return false;
        }
        return true;
    }
    
    template<typename T>
    bool checkNullptr(const std::shared_ptr<T>& ptr, const std::string& context) {
        if (!ptr) {
            NLM_LOG_ERROR("Null shared_ptr error in " + context);
            return false;
        }
        return true;
    }
    
    std::string getErrorMessage(const std::string& operation, const std::string& details = "") {
        std::stringstream ss;
        ss << "Operation failed: " << operation;
        if (!details.empty()) {
            ss << " (Details: " << details << ")";
        }
        return ss.str();
    }
}

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
    try {
        // Validate input parameters
        if (!ErrorUtils::checkNullptr(brain_, "AgentBrain constructor")) {
            throw std::invalid_argument("Brain pointer cannot be null");
        }
        
        NLM_LOG_INFO("Initializing AgentBrain with memory validation");
        
        // Initialize motor and sensory neuron groups with error checking
        if (brain_) {
            for (const auto& region : brain_->getRegions()) {
                if (!ErrorUtils::checkNullptr(region, "Brain region")) {
                    NLM_LOG_WARN("Skipping invalid brain region");
                    continue;
                }
                
                for (auto& pop : region->getPopulations()) {
                    if (!ErrorUtils::checkNullptr(pop, "Neural population")) {
                        NLM_LOG_WARN("Skipping invalid neural population");
                        continue;
                    }
                    
                    NeuronType type = pop->getNeuronType();
                    
                    if (type == NeuronType::Motor) {
                        auto neurons = pop->getNeurons();
                        NLM_LOG_DEBUG("Found " + std::to_string(neurons.size()) + " motor neurons");
                        
                        for (Neuron* n : neurons) {
                            if (!ErrorUtils::checkNullptr(n, "Motor neuron")) {
                                NLM_LOG_WARN("Skipping invalid motor neuron");
                                continue;
                            }
                            
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
                        auto neurons = pop->getNeurons();
                        NLM_LOG_DEBUG("Found " + std::to_string(neurons.size()) + " sensory neurons");
                        
                        for (Neuron* n : neurons) {
                            if (!ErrorUtils::checkNullptr(n, "Sensory neuron")) {
                                NLM_LOG_WARN("Skipping invalid sensory neuron");
                                continue;
                            }
                            
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
                    } else {
                        NLM_LOG_DEBUG("Skipping neurons of type: " + std::to_string(static_cast<int>(type)));
                    }
                }
            }
        }
        
        NLM_LOG_INFO("AgentBrain initialized successfully");
        NLM_LOG_INFO("  Motor neuron groups: " + std::to_string(motorForward_.size()) + " forward, " +
                    std::to_string(motorBackward_.size()) + " backward, " +
                    std::to_string(motorTurnLeft_.size()) + " left, " +
                    std::to_string(motorTurnRight_.size()) + " right, " +
                    std::to_string(motorInteract_.size()) + " interact, " +
                    std::to_string(motorWait_.size()) + " wait");
        NLM_LOG_INFO("  Sensory neuron groups: " + std::to_string(sensoryVision_.size()) + " vision, " +
                    std::to_string(sensoryTouch_.size()) + " touch, " +
                    std::to_string(sensoryInternal_.size()) + " internal, " +
                    std::to_string(sensoryProprioception_.size()) + " proprioception");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error initializing AgentBrain: " + std::string(e.what()));
        throw;
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    try {
        if (!ErrorUtils::checkNullptr(&world, "AgentBrain::initialize")) {
            throw std::invalid_argument("World pointer cannot be null");
        }
        
        // Validate world configuration
        if (world.getVisionWidth() == 0 || world.getVisionHeight() == 0) {
            NLM_LOG_WARN("World has zero vision dimensions - initialization may fail");
        }
        
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in AgentBrain::initialize: " + std::string(e.what()));
        throw;
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    try {
        if (!ErrorUtils::checkNullptr(&percept, "AgentBrain::processSensoryInput")) {
            NLM_LOG_ERROR("Cannot process null percept");
            return;
        }
        
        if (!brain_) {
            NLM_LOG_WARN("Cannot process sensory input - brain not initialized");
            return;
        }
        
        // Vision input (256 values -> sensoryVision_ neurons)
        const auto& vision = percept.getVision();
        NLM_LOG_TRACE("Processing vision input: " + std::to_string(vision.size()) + " values");
        
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Validate neuron state
                float current = vision[i] * 5.0f;  // Scale factor
                
                // Clamp current value to reasonable range
                current = std::clamp(current, -100.0f, 100.0f);
                
                sensoryVision_[i]->injectCurrent(current);
            }
        }
        
        // Touch input (8 values -> sensoryTouch_ neurons)
        const auto& touch = percept.getTouch();
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;  // Collision signal
                current = std::clamp(current, -50.0f, 50.0f);
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
        
        // Internal signals (4 values -> sensoryInternal_ neurons)
        const auto& intern = percept.getInternal();
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                current = std::clamp(current, -25.0f, 25.0f);
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
        
        // Proprioception (6 values -> sensoryProprioception_ neurons)
        const auto& proprio = percept.getProprioception();
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                current = std::clamp(current, -20.0f, 20.0f);
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
        
        // Compute novelty with error checking
        if (!vision.empty()) {
            float totalDiff = 0.0f;
            for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
                float diff = std::abs(vision[i] - previousVision_[i]);
                totalDiff += diff;
            }
            
            // Normalize
            noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
            
            // Decay and update
            noveltyLevel_ *= sensoryNoveltyDecay_;
            
            // Store for next time
            previousVision_ = vision;
        }
        
        // Update curiosity based on novelty
        if (curiosityEnabled_) {
            curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
        
        NLM_LOG_TRACE("Sensory processing complete - Novelty: " + std::to_string(noveltyLevel_));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in AgentBrain::processSensoryInput: " + std::string(e.what()));
        throw;
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    try {
        if (!brain_) {
            NLM_LOG_WARN("Cannot decode motor command - brain not initialized");
            return MotorCommand::Wait;
        }
        
        MotorCommand decoded = decodeFromMotorNeurons();
        
        // Apply curiosity-based exploration with bounds checking
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            decoded = selectWithCuriosity(decoded);
        }
        
        NLM_LOG_TRACE("Motor command decoded: " + std::to_string(static_cast<int>(decoded)));
        return decoded;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in AgentBrain::decodeMotorCommand: " + std::string(e.what()));
        return MotorCommand::Wait; // Fail safe
    }
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    try {
        // Calculate average activity in each motor group
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            
            float sum = 0.0f;
            for (Neuron* n : neurons) {
                if (!ErrorUtils::checkNullptr(n, "Motor neuron in activity calculation")) {
                    continue;
                }
                
                // Use membrane potential deviation from rest as activity measure
                float activity = std::abs(n->getState().membranePotential - n->getState().restingPotential);
                sum += activity;
            }
            
            return sum / neurons.size();
        };
        
        float forwardAct = calcActivity(motorForward_);
        float backwardAct = calcActivity(motorBackward_);
        float leftAct = calcActivity(motorTurnLeft_);
        float rightAct = calcActivity(motorTurnRight_);
        float interactAct = calcActivity(motorInteract_);
        float waitAct = calcActivity(motorWait_);
        
        NLM_LOG_TRACE("Motor activities - Forward: " + std::to_string(forwardAct) + 
                     ", Backward: " + std::to_string(backwardAct) +
                     ", Left: " + std::to_string(leftAct) +
                     ", Right: " + std::to_string(rightAct) +
                     ", Interact: " + std::to_string(interactAct) +
                     ", Wait: " + std::to_string(waitAct));
        
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
            NLM_LOG_TRACE("Low motor activity (" + std::to_string(bestActivity) + ") - default to wait");
            return MotorCommand::Wait;
        }
        
        NLM_LOG_DEBUG("Selected motor command: " + std::to_string(static_cast<int>(best)) +
                     " (activity: " + std::to_string(bestActivity) + ")");
        
        return best;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in AgentBrain::decodeFromMotorNeurons: " + std::string(e.what()));
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    try {
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in AgentBrain::selectWithCuriosity: " + std::string(e.what()));
        return defaultCmd;
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    try {
        if (!brain_ || !rewardModulationEnabled_) {
            NLM_LOG_TRACE("Reward modulation disabled or brain not available");
            return;
        }
        
        // Validate input ranges
        reward = std::clamp(reward, -10.0f, 10.0f);
        predictedReward = std::clamp(predictedReward, -10.0f, 10.0f);
        
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
        
        // Dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
        
        // Clamp to reasonable range
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
        
        NLM_LOG_TRACE("Reward modulation - Prediction error: " + std::to_string(predictionError_) +
                     ", Dopamine: " + std::to_string(dopamineLevel_));
        
        // Apply to all synapses with eligibility traces
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (!ErrorUtils::checkNullptr(syn, "Synapse for reward modulation")) {
                    continue;
                }
                
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
        if (ErrorUtils::checkNullptr(stdp, "STDP system for plasticity modulation")) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
        
        NLM_LOG_DEBUG("Applied reward modulation successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in AgentBrain::applyRewardModulation: " + std::string(e.what()));
        throw;
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    try {
        if (!brain_ || !developmentEnabled_) {
            NLM_LOG_TRACE("Development disabled or brain not available");
            return;
        }
        
        if (timestep < 0) {
            NLM_LOG_ERROR("Invalid timestep for development update: " + std::to_string(timestep));
            return;
        }
        
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
            if (ErrorUtils::checkNullptr(sp, "Structural plasticity system")) {
                // Higher synaptogenesis in early development
                float synRate = 0.0001f * plasticityModifier_;
                float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
        
        NLM_LOG_TRACE("Development updated - Age: " + std::to_string(developmentalAge_) +
                     ", Plasticity: " + std::to_string(plasticityModifier_));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in AgentBrain::updateDevelopment: " + std::string(e.what()));
        throw;
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        NLM_LOG_WARN("Cannot get developmental stage - brain not initialized");
        return DevelopmentalStage::Initial;
    }
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
    try {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Clear previous vision
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        
        NLM_LOG_DEBUG("AgentBrain reset completed");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in AgentBrain::reset: " + std::string(e.what()));
        throw;
    }
}

} // namespace nlm
