#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <sstream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, bool enableAdvancedFeatures)
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
    , actionSelectionMode_(0)
    , curiosityThreshold_(0.5f)
    , developmentSpeed_(1.0f)
    , enableSleepCycles_(false)
    , enableMemoryReplay_(false)
    , observationCount_(0)
    , averageReward_(0.0f)
    , averageProcessingTime_(0.0)
    , initialized_(false)
    , brainNullChecked_(false)
{
    if (enableAdvancedFeatures) {
        NLM_LOG_INFO("AgentBrain: Initializing with advanced features enabled");
        NLM_LOG_INFO("AgentBrain: Setting up random seed based on system time");
        randomDevice_.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    } else {
        NLM_LOG_INFO("AgentBrain: Initializing with standard features (backward compatible mode)");
    }
    
    NLM_LOG_INFO("AgentBrain: Starting motor and sensory neuron group initialization");
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        brainNullChecked_ = true;
        NLM_LOG_INFO("AgentBrain: Found brain reference, proceeding with neuron group distribution");
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    NLM_LOG_INFO("AgentBrain: Processing motor neuron population");
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        NLM_LOG_DEBUG("AgentBrain: Assigning motor neuron " + std::to_string(idx) + " to action group");
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); NLM_LOG_INFO("AgentBrain: Added motor neuron to forward group"); break;
                            case 1: motorBackward_.push_back(n); NLM_LOG_INFO("AgentBrain: Added motor neuron to backward group"); break;
                            case 2: motorTurnLeft_.push_back(n); NLM_LOG_INFO("AgentBrain: Added motor neuron to left turn group"); break;
                            case 3: motorTurnRight_.push_back(n); NLM_LOG_INFO("AgentBrain: Added motor neuron to right turn group"); break;
                            case 4: motorInteract_.push_back(n); NLM_LOG_INFO("AgentBrain: Added motor neuron to interact group"); break;
                            case 5: motorWait_.push_back(n); NLM_LOG_INFO("AgentBrain: Added motor neuron to wait group"); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    NLM_LOG_INFO("AgentBrain: Processing sensory neuron population");
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        NLM_LOG_DEBUG("AgentBrain: Assigning sensory neuron " + std::to_string(idx) + " to sensory group");
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); NLM_LOG_INFO("AgentBrain: Added sensory neuron to vision group"); break;
                            case 1: sensoryTouch_.push_back(n); NLM_LOG_INFO("AgentBrain: Added sensory neuron to touch group"); break;
                            case 2: sensoryInternal_.push_back(n); NLM_LOG_INFO("AgentBrain: Added sensory neuron to internal group"); break;
                            case 3: sensoryProprioception_.push_back(n); NLM_LOG_INFO("AgentBrain: Added sensory neuron to proprioception group"); break;
                        }
                    }
                }
            }
        }
        
        NLM_LOG_INFO("AgentBrain: Neuron group initialization complete. Motor groups: " +
                     std::to_string(motorForward_.size() + motorBackward_.size() + 
                                   motorTurnLeft_.size() + motorTurnRight_.size() +
                                   motorInteract_.size() + motorWait_.size()) +
                     " total neurons distributed. " +
                     std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                                   sensoryInternal_.size() + sensoryProprioception_.size()) +
                     " sensory neurons distributed.");
    } else {
        NLM_LOG_ERROR("AgentBrain: ERROR - brain reference is null! Cannot initialize neuron groups.");
        handleError("Brain reference is null during initialization");
    }
    
    // Store initialization start time for performance monitoring
    lastTime_ = std::chrono::high_resolution_clock::now();
    NLM_LOG_INFO("AgentBrain: Initialization completed with advanced features " + 
                 std::string(enableAdvancedFeatures ? "enabled" : "disabled"));
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
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

void AgentBrain::applyMultiNeuromodulation(const std::vector<float>& signals) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: ERROR - brain reference is null! Cannot apply multi neuromodulation.");
        handleError("Brain reference is null in applyMultiNeuromodulation");
        return;
    }
    
    NLM_LOG_INFO("AgentBrain: Applying multi neuromodulation with " + std::to_string(signals.size()) + " signals");
    
    // Check for valid signals vector
    if (signals.empty()) {
        NLM_LOG_WARNING("AgentBrain: Empty signals vector, neuromodulation skipped");
        return;
    }
    
    // Process dopamine (signal[0])
    if (signals.size() > 0) {
        float dopamine = signals[0];
        dopamineLevel_ = dopamine;
        NLM_LOG_DEBUG("AgentBrain: Dopamine signal applied: " + std::to_string(dopamine));
    }
    
    // Process curiosity (signal[1])
    if (signals.size() > 1) {
        curiosityLevel_ = signals[1];
        NLM_LOG_DEBUG("AgentBrain: Curiosity signal applied: " + std::to_string(signals[1]));
    }
    
    // Process novelty (signal[2])
    if (signals.size() > 2) {
        noveltyLevel_ = signals[2];
        NLM_LOG_DEBUG("AgentBrain: Novelty signal applied: " + std::to_string(signals[2]));
    }
    
    // Process prediction error (signal[3])
    if (signals.size() > 3) {
        predictionError_ = signals[3];
        NLM_LOG_DEBUG("AgentBrain: Prediction error signal applied: " + std::to_string(signals[3]));
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        float baseCuriosity = curiosityLevel_;
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = (baseCuriosity * 0.3f) + (curiosityLevel_ * 0.7f); // Blend with base
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        NLM_LOG_DEBUG("AgentBrain: Updated curiosity level: " + std::to_string(curiosityLevel_));
    }
    
    // Clamp neuromodulators to valid ranges
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply neuromodulation to brain regions
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Apply enhanced weight changes based on combined neuromodulators
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Enhanced plasticity with multiple neuromodulators
                float neuromodulationFactor = (dopamineLevel_ + 1.0f) * 0.5f; // Normalize to 0-1
                neuromodulationFactor *= (1.0f - (1.0f - curiosityLevel_) * 0.3f); // Reduce plasticity for high curiosity
                
                // Novelty bonus for recently novel experiences
                float noveltyBonus = 1.0f + (noveltyLevel_ * 0.2f);
                
                float delta = eligibility * neuromodulationFactor * plasticityModifier_ * noveltyBonus;
                
                syn->addToWeight(delta);
                NLM_LOG_DEBUG("AgentBrain: Applied enhanced neuromodulation delta: " + std::to_string(delta));
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Update expected reward with prediction error
    expectedReward_ = expectedReward_ * 0.95f + predictionError_ * 0.05f;
    
    // Trigger callbacks if set
    if (rewardCallback_) {
        rewardCallback_(predictionError_, expectedReward_);
    }
    
    NLM_LOG_INFO("AgentBrain: Multi neuromodulation complete. Dopamine: " + std::to_string(dopamineLevel_));
    updatePerformanceCounter();
}

void AgentBrain::setActionSelectionMode(int mode) {
    actionSelectionMode_ = std::clamp(mode, 0, 2);
    NLM_LOG_INFO("AgentBrain: Set action selection mode to " + std::to_string(actionSelectionMode_));
}

void AgentBrain::setCuriosityThreshold(float threshold) {
    curiosityThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
    NLM_LOG_INFO("AgentBrain: Set curiosity threshold to " + std::to_string(curiosityThreshold_));
}

void AgentBrain::setDevelopmentSpeed(float speed) {
    developmentSpeed_ = std::max(0.0f, speed);
    NLM_LOG_INFO("AgentBrain: Set development speed to " + std::to_string(developmentSpeed_));
}

float AgentBrain::getActionQuality(int actionIndex) const {
    if (!brain_) return 0.0f;
    
    // Simple action quality based on motor neuron activity
    float maxActivity = 0.0f;
    
    switch (actionIndex) {
        case 0: maxActivity = (motorForward_.empty() ? 0.0f : 1.0f); break;  // MoveForward
        case 1: maxActivity = (motorBackward_.empty() ? 0.0f : 1.0f); break; // MoveBackward
        case 2: maxActivity = (motorTurnLeft_.empty() ? 0.0f : 1.0f); break;  // TurnLeft
        case 3: maxActivity = (motorTurnRight_.empty() ? 0.0f : 1.0f); break; // TurnRight
        case 4: maxActivity = (motorInteract_.empty() ? 0.0f : 1.0f); break; // Interact
        case 5: maxActivity = (motorWait_.empty() ? 0.0f : 1.0f); break;  // Wait
        default: maxActivity = 0.0f; break;
    }
    
    // Enhanced quality based on activity levels
    if (!brain_->getRegions().empty()) {
        maxActivity *= 1.0f + (dopamineLevel_ * 0.5f); // Better quality with positive dopamine
        maxActivity *= 1.0f + (curiosityLevel_ * 0.2f); // Higher curiosity may improve quality
    }
    
    maxActivity = std::clamp(maxActivity, 0.0f, 1.0f);
    NLM_LOG_DEBUG("AgentBrain: Action quality for index " + std::to_string(actionIndex) + ": " + std::to_string(maxActivity));
    return maxActivity;
}

bool AgentBrain::wasPlanSuccessful() const {
    // Simple plan success tracking based on rewards and action quality
    bool success = false;
    
    if (averageReward_ > 0.3f) {
        success = true;
    }
    
    // Enhanced success criteria for advanced users
    if (actionSelectionMode_ == 1) { // Planned mode
        if (curiosityLevel_ > curiosityThreshold_) {
            success = success && (dopamineLevel_ > 0.0f);
        }
    }
    
    NLM_LOG_DEBUG("AgentBrain: Plan successful: " + std::string(success ? "true" : "false"));
    return success;
}

void AgentBrain::recordExperience(const std::vector<float>& state, ActionType action, 
                                 const std::vector<float>& nextState, float reward) {
    // Add experience to buffer for memory
    if (!state.empty() && !nextState.empty()) {
        NLM_LOG_INFO("AgentBrain: Recording experience with state size " + std::to_string(state.size()) + 
                     ", action index: " + std::to_string(static_cast<int>(action)) + 
                     ", nextState size: " + std::to_string(nextState.size()) + ", reward: " + std::to_string(reward));
        
        // Store experience tuple
        experienceBuffer_.emplace_back(state, action, nextState, reward);
        
        // Update statistics
        observationCount_++;
        averageReward_ = (averageReward_ * (observationCount_ - 1) + reward) / observationCount_;
        
        // Trim buffer if too large
        if (enableMemoryReplay_ && experienceBuffer_.size() > 1000) {
            // Remove oldest experiences
            size_t removeCount = experienceBuffer_.size() / 4; // Remove 25%
            experienceBuffer_.erase(experienceBuffer_.begin(), experienceBuffer_.begin() + removeCount);
            NLM_LOG_INFO("AgentBrain: Experience buffer trimmed to " + std::to_string(experienceBuffer_.size()) + " entries");
        }
        
        // Trigger action callback if set
        if (actionCallback_) {
            actionCallback_();
        }
    } else {
        NLM_LOG_WARNING("AgentBrain: Invalid experience data - state or nextState is empty");
    }
}

std::vector<std::vector<float>> AgentBrain::retrieveSimilarExperiences(
    const std::vector<float>& state, size_t maxResults) const {
    std::vector<std::vector<float>> similarExperiences;
    
    if (state.empty() || experienceBuffer_.empty()) {
        return similarExperiences;
    }
    
    NLM_LOG_INFO("AgentBrain: Retrieving similar experiences for state with " + std::to_string(state.size()) + " features");
    
    // Simple similarity metric based on Euclidean distance
    std::vector<std::pair<double, std::vector<float>>> similarities;
    
    for (const auto& experience : experienceBuffer_) {
        const auto& expState = std::get<0>(experience);
        
        if (expState.size() == state.size()) {
            double similarity = 0.0;
            double sumDiff = 0.0;
            
            for (size_t i = 0; i < state.size(); ++i) {
                double diff = state[i] - expState[i];
                sumDiff += diff * diff;
            }
            
            if (sumDiff > 0.0) {
                similarity = 1.0 / (1.0 + std::sqrt(sumDiff));
            } else {
                similarity = 1.0; // Identical states
            }
            
            similarities.emplace_back(similarity, expState);
        }
    }
    
    // Sort by similarity (highest first)
    std::sort(similarities.begin(), similarities.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Extract similar states
    size_t resultCount = std::min(maxResults, similarities.size());
    for (size_t i = 0; i < resultCount; ++i) {
        similarExperiences.push_back(similarities[i].second);
    }
    
    NLM_LOG_INFO("AgentBrain: Retrieved " + std::to_string(resultCount) + " similar experiences");
    return similarExperiences;
}

void AgentBrain::logAgentState() const {
    NLM_LOG_INFO("AgentBrain State:");
    NLM_LOG_INFO("  Developmental Age: " + std::to_string(developmentalAge_));
    NLM_LOG_INFO("  Dopamine Level: " + std::to_string(dopamineLevel_));
    NLM_LOG_INFO("  Curiosity Level: " + std::to_string(curiosityLevel_));
    NLM_LOG_INFO("  Novelty Level: " + std::to_string(noveltyLevel_));
    NLM_LOG_INFO("  Prediction Error: " + std::to_string(predictionError_));
    NLM_LOG_INFO("  Expected Reward: " + std::to_string(expectedReward_));
    NLM_LOG_INFO("  Plasticity Modifier: " + std::to_string(plasticityModifier_));
    NLM_LOG_INFO("  Action Selection Mode: " + std::to_string(actionSelectionMode_));
    NLM_LOG_INFO("  Curiosity Threshold: " + std::to_string(curiosityThreshold_));
    NLM_LOG_INFO("  Development Speed: " + std::to_string(developmentSpeed_));
    NLM_LOG_INFO("  Observation Count: " + std::to_string(observationCount_));
    NLM_LOG_INFO("  Average Reward: " + std::to_string(averageReward_));
    NLM_LOG_INFO("  Experience Buffer Size: " + std::to_string(experienceBuffer_.size()));
    NLM_LOG_INFO("  Current Processing Time: " + std::to_string(averageProcessingTime_) + "s");
    NLM_LOG_INFO("  Configuration - Reward Modulation: " + std::string(rewardModulationEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Configuration - Structural Plasticity: " + std::string(structuralPlasticityEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Configuration - Development: " + std::string(developmentEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Configuration - Curiosity: " + std::string(curiosityEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Configuration - Sleep Cycles: " + std::string(enableSleepCycles_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Configuration - Memory Replay: " + std::string(enableMemoryReplay_ ? "enabled" : "disabled"));
}

void AgentBrain::exportNeuralState(std::vector<float>& state) const {
    NLM_LOG_INFO("AgentBrain: Exporting neural state with " + std::to_string(state.size()) + " elements");
    
    // Clear and resize the output state vector
    state.clear();
    
    // Create a comprehensive neural state export
    std::vector<float> exportState;
    
    // Neuromodulation levels
    exportState.push_back(dopamineLevel_);
    exportState.push_back(noveltyLevel_);
    exportState.push_back(curiosityLevel_);
    exportState.push_back(predictionError_);
    exportState.push_back(expectedReward_);
    
    // Development state
    exportState.push_back(static_cast<float>(developmentalAge_));
    exportState.push_back(plasticityModifier_);
    
    // Configuration flags
    exportState.push_back(actionSelectionMode_);
    exportState.push_back(curiosityThreshold_);
    exportState.push_back(developmentSpeed_);
    exportState.push_back(rewardModulationEnabled_ ? 1.0f : 0.0f);
    exportState.push_back(structuralPlasticityEnabled_ ? 1.0f : 0.0f);
    exportState.push_back(developmentEnabled_ ? 1.0f : 0.0f);
    exportState.push_back(curiosityEnabled_ ? 1.0f : 0.0f);
    
    // Statistics
    exportState.push_back(static_cast<float>(observationCount_));
    exportState.push_back(averageReward_);
    
    // Performance
    exportState.push_back(static_cast<float>(averageProcessingTime_));
    
    // Novelty decay factor
    exportState.push_back(sensoryNoveltyDecay_);
    
    // Copy to output parameter
    state = exportState;
    
    NLM_LOG_INFO("AgentBrain: Exported neural state with " + std::to_string(state.size()) + " features");
}

void AgentBrain::importNeuralState(const std::vector<float>& state) {
    if (state.size() < 20) {
        NLM_LOG_WARNING("AgentBrain: Import state vector too small (" + std::to_string(state.size()) + " elements)");
        return;
    }
    
    NLM_LOG_INFO("AgentBrain: Importing neural state with " + std::to_string(state.size()) + " elements");
    
    size_t index = 0;
    
    // Restore neuromodulation levels
    dopamineLevel_ = state[index++];
    noveltyLevel_ = state[index++];
    curiosityLevel_ = state[index++];
    predictionError_ = state[index++];
    expectedReward_ = state[index++];
    
    // Restore development state
    developmentalAge_ = static_cast<double>(state[index++]);
    plasticityModifier_ = state[index++];
    
    // Restore configuration flags
    actionSelectionMode_ = static_cast<int>(state[index++] * 3.0f); // Scale to 0-2
    actionSelectionMode_ = std::clamp(actionSelectionMode_, 0, 2);
    curiosityThreshold_ = state[index++];
    developmentSpeed_ = state[index++];
    rewardModulationEnabled_ = (state[index++] > 0.5f);
    structuralPlasticityEnabled_ = (state[index++] > 0.5f);
    developmentEnabled_ = (state[index++] > 0.5f);
    curiosityEnabled_ = (state[index++] > 0.5f);
    
    // Restore statistics
    observationCount_ = static_cast<size_t>(state[index++]);
    averageReward_ = state[index++];
    
    // Restore performance
    averageProcessingTime_ = static_cast<double>(state[index++]);
    
    // Restore novelty decay
    if (index < state.size()) {
        sensoryNoveltyDecay_ = state[index++];
    }
    
    NLM_LOG_INFO("AgentBrain: Neural state import complete. Dopamine: " + std::to_string(dopamineLevel_));
}

void AgentBrain::handleError(const std::string& errorMessage) const {
    if (!errorMessage.empty()) {
        lastError_ = errorMessage;
        NLM_LOG_ERROR("AgentBrain: Error handled - " + errorMessage);
        
        // Safe error handling - don't crash the system
        // In a real implementation, this might log to a file or send to a monitoring service
    }
}

void AgentBrain::clearNeuronVectors() {
    NLM_LOG_INFO("AgentBrain: Clearing neuron vectors for memory cleanup");
    
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    previousVision_.clear();
    experienceBuffer_.clear();
    
    NLM_LOG_INFO("AgentBrain: Neuron vectors cleared. Total groups: " +
                 std::to_string(motorForward_.size() + motorBackward_.size() + 
                               motorTurnLeft_.size() + motorTurnRight_.size() +
                               motorInteract_.size() + motorWait_.size() +
                               sensoryVision_.size() + sensoryTouch_.size() +
                               sensoryInternal_.size() + sensoryProprioception_.size()));
}

void AgentBrain::cleanupResources() {
    NLM_LOG_INFO("AgentBrain: Starting resource cleanup");
    
    // Clear all internal data structures
    clearNeuronVectors();
    
    // Reset state variables
    reset();
    
    // Clear performance tracking
    resetPerformanceCounters();
    
    // Clear callbacks
    rewardCallback_ = nullptr;
    actionCallback_ = nullptr;
    
    NLM_LOG_INFO("AgentBrain: Resource cleanup complete");
}
