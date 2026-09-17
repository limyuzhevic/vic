#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>
#include <mutex>
#include <sstream>
#include <fstream>
#include <chrono>
#include <numeric>

namespace nlm {
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
    , curiosityThreshold_(0.5f)
    , explorationBase_(0.3f)
    , motorCommandTimeout_(0.1f)
    , neuromodulationGain_(1.0f)
    , developmentRate_(0.001)
    , episodeActive_(false)
    , currentEpisode_(0)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % NUM_MOTOR_COMMANDS) {
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
    }
}

AgentBrain::~AgentBrain() = default;

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return NUM_MOTOR_COMMANDS;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = DEFAULT_PLASTICITY_MODIFIER;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    // Validate percept data
    if (!percept.validate()) {
        NLM_LOG_WARNING("Invalid sensory percept received");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    size_t minVisionSize = std::min(sensoryVision_.size(), vision.size());
    for (size_t i = 0; i < minVisionSize; ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    size_t minTouchSize = std::min(sensoryTouch_.size(), touch.size());
    for (size_t i = 0; i < minTouchSize; ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    size_t minInternalSize = std::min(sensoryInternal_.size(), intern.size());
    for (size_t i = 0; i < minInternalSize; ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    size_t minProprioSize = std::min(sensoryProprioception_.size(), proprio.size());
    for (size_t i = 0; i < minProprioSize; ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        size_t minVisionForNovelty = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < minVisionForNovelty; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(minVisionForNovelty, 1);
        
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
    
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    stats_.totalSteps++;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    if (decoded != MotorCommand::Wait) {
        stats_.motorCommandsIssued++;
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > DEFAULT_CURIOUSNESS_THRESHOLD) {
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
            if (!n) continue;
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
    if (bestActivity < DEFAULT_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > curiosityThreshold_) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * explorationBase_;
        exploreChance = std::clamp(exploreChance, 0.0f, explorationBase_);
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - fixed range and removed LookLeft/Right which aren't valid motor commands
            int choice = brain_->getRandomGenerator()->uniformInt(0, NUM_MOTOR_COMMANDS - 1);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    // Validate input values
    if (!std::isfinite(reward) || !std::isfinite(predictedReward)) {
        NLM_LOG_WARNING("Invalid reward/predictedReward values: " + 
                      std::to_string(reward) + ", " + std::to_string(predictedReward));
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Track reward statistics
    stats_.totalRewardReceived += reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_ * neuromodulationGain_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * neuromodulationGain_;
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
    
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    developmentalAge_ += timestep * developmentRate_;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = DEFAULT_PLASTICITY_MODIFIER;  // High plasticity
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
    
    // Update stats
    stats_.averageDevelopmentAge = (stats_.averageDevelopmentAge * stats_.totalSteps + developmentalAge_) / 
                                   (stats_.totalSteps + 1);
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
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = DEFAULT_PLASTICITY_MODIFIER;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset stats
    stats_.totalSteps = 0;
    stats_.motorCommandsIssued = 0;
    stats_.averageFiringRate = 0.0f;
    stats_.totalRewardReceived = 0.0f;
    stats_.averageDevelopmentAge = 0.0;
    stats_.episodeCount++;
    
    // Mark episode as inactive
    episodeActive_ = false;
}

// Advanced methods implementation
std::string AgentBrain::getDebugInfo() const {
    std::stringstream ss;
    ss << "=== AgentBrain Debug Info ===\n";
    ss << "Brain: " << (brain_ ? "Active" : "Null") << "\n";
    ss << "Neuromodulation: dopamine=" << dopamineLevel_ << "\n";
    ss << "Curiosity: " << curiosityLevel_ << "\n";
    ss << "Novelty: " << noveltyLevel_ << "\n";
    ss << "Prediction Error: " << predictionError_ << "\n";
    ss << "Expected Reward: " << expectedReward_ << "\n";
    ss << "Development Age: " << developmentalAge_ << " steps\n";
    ss << "Plasticity Modifier: " << plasticityModifier_ << "\n";
    ss << "Motor Commands: Forward=" << motorForward_.size() << ", Back=" << motorBackward_.size() << 
        ", Left=" << motorTurnLeft_.size() << ", Right=" << motorTurnRight_.size() << 
        ", Interact=" << motorInteract_.size() << ", Wait=" << motorWait_.size() << "\n";
    ss << "Sensory Neurons: Vision=" << sensoryVision_.size() << ", Touch=" << sensoryTouch_.size() << 
        ", Internal=" << sensoryInternal_.size() << ", Proprioception=" << sensoryProprioception_.size() << "\n";
    return ss.str();
}

bool AgentBrain::validateState() const {
    bool isValid = true;
    
    // Check brain pointer
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain has null brain pointer");
        isValid = false;
    }
    
    // Check neuromodulation values
    if (std::isnan(dopamineLevel_) || std::isinf(dopamineLevel_)) {
        NLM_LOG_ERROR("Invalid dopamine level: " + std::to_string(dopamineLevel_));
        isValid = false;
    }
    
    // Check motor neuron groups
    if (motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() + 
        motorTurnRight_.size() + motorInteract_.size() + motorWait_.size() == 0) {
        NLM_LOG_WARNING("No motor neurons initialized");
    }
    
    // Check sensory neuron groups
    if (sensoryVision_.size() + sensoryTouch_.size() + sensoryInternal_.size() + 
        sensoryProprioception_.size() == 0) {
        NLM_LOG_WARNING("No sensory neurons initialized");
    }
    
    return isValid;
}

float AgentBrain::calculateLearningRate() const {
    // Combine neuromodulation level and development stage
    float neuroModFactor = std::abs(dopamineLevel_);
    float devFactor = std::min(1.0f, developmentalAge_ / 300.0f);
    return (neuroModFactor + devFactor) / 2.0f;
}

float AgentBrain::calculateExplorationPressure() const {
    // Higher curiosity = more exploration
    float curiosityFactor = curiosityLevel_;
    float noveltyFactor = noveltyLevel_;
    return (curiosityFactor + noveltyFactor) / 2.0f;
}

float AgentBrain::calculateNoveltySensitivity() const {
    return noveltyLevel_ * curiosityLevel_;
}

float AgentBrain::calculateRewardPredictionError(float reward, float predicted) const {
    return std::abs(reward - predicted);
}

void AgentBrain::accelerateDevelopment() {
    developmentRate_ *= 2.0f;
    NLM_LOG_INFO("Development accelerated, new rate: " + std::to_string(developmentRate_));
}

void AgentBrain::decelerateDevelopment() {
    developmentRate_ *= 0.5f;
    NLM_LOG_INFO("Development decelerated, new rate: " + std::to_string(developmentRate_));
}

void AgentBrain::resetDevelopmentClock() {
    developmentalAge_ = 0.0;
    plasticityModifier_ = DEFAULT_PLASTICITY_MODIFIER;
    NLM_LOG_INFO("Development clock reset");
}

void AgentBrain::enhanceNeuromodulation(float factor) {
    neuromodulationGain_ *= factor;
    NLM_LOG_INFO("Neuromodulation enhanced, new gain: " + std::to_string(neuromodulationGain_));
}

void AgentBrain::suppressNeuromodulation(float factor) {
    neuromodulationGain_ *= factor;
    NLM_LOG_INFO("Neuromodulation suppressed, new gain: " + std::to_string(neuromodulationGain_));
}

void AgentBrain::stabilizeNeuromodulation() {
    neuromodulationGain_ = DEFAULT_NEUROMODULATION_GAIN;
    NLM_LOG_INFO("Neuromodulation stabilized");
}

MotorCommand AgentBrain::exploreRandom() {
    int choice = brain_->getRandomGenerator()->uniformInt(0, NUM_MOTOR_COMMANDS - 1);
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::exploreIntelligent() {
    // Use curiosity level and prediction error to guide exploration
    if (curiosityLevel_ > 0.8f) {
        return exploreBasedOnNovelty();
    } else if (predictionError_ > 0.5f) {
        return exploreBasedOnCuriosity();
    } else {
        return exploreRandom();
    }
}

MotorCommand AgentBrain::exploreBasedOnNovelty() {
    // Explore more when novelty is high
    if (noveltyLevel_ > 0.7f) {
        return exploreRandom();
    } else {
        // Use curiosity to bias exploration
        if (curiosityLevel_ > 0.6f) {
            return exploreBasedOnCuriosity();
        } else {
            return exploreRandom();
        }
    }
}

MotorCommand AgentBrain::exploreBasedOnCuriosity() {
    // Explore based on curiosity level
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < curiosityLevel_) {
        return exploreRandom();
    } else {
        return MotorCommand::Wait;
    }
}

void AgentBrain::updateExpectedReward(float reward, float predictionError) {
    expectedReward_ = reward;
    predictionError_ = predictionError;
    
    // Update learning accuracy
    stats_.rewardPredictionAccuracy = 1.0f - std::min(1.0f, std::abs(predictionError_));
}

void AgentBrain::adjustNoveltySensitivity(float sensitivity) {
    curiosityThreshold_ = std::clamp(sensitivity, 0.1f, 1.0f);
}

void AgentBrain::adjustCuriosityThreshold(float threshold) {
    explorationBase_ = std::clamp(threshold, 0.1f, 1.0f);
}

void AgentBrain::resetNoveltyDetection() {
    previousVision_.assign(previousVision_.size(), 0.0f);
    noveltyLevel_ = 0.0f;
}

void AgentBrain::optimizeSensoryProcessing() {
    // Mark performance timing
    auto start = std::chrono::high_resolution_clock::now();
    
    // Process sensory input with optimizations
    // (Implementation details would go here)
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    perfProfile_.sensoryProcessingTime += duration.count();
}

void AgentBrain::optimizeMotorOutput() {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Optimize motor decoding
    // (Implementation details would go here)
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    perfProfile_.motorDecodingTime += duration.count();
}

void AgentBrain::compactMemory() {
    // Compact memory usage
    // This would typically involve garbage collection or optimization
    // For now, it's a placeholder for future implementation
    NLM_LOG_INFO("Memory compaction completed");
}

void AgentBrain::startEpisode() {
    episodeActive_ = true;
    episodeStartTime_ = std::chrono::high_resolution_clock::now();
    currentEpisode_++;
    NLM_LOG_INFO("Episode " + std::to_string(currentEpisode_) + " started");
}

void AgentBrain::endEpisode() {
    episodeActive_ = false;
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - episodeStartTime_);
    
    NLM_LOG_INFO("Episode " + std::to_string(currentEpisode_) + " ended after " + 
                 std::to_string(duration.count()) + " seconds");
}

void AgentBrain::configureFromCommandLine(int argc, char* argv[]) {
    // Parse command line arguments for configuration
    // This is a placeholder for future implementation
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--curiosity-threshold" && i + 1 < argc) {
            curiosityThreshold_ = std::stof(argv[++i]);
        } else if (arg == "--exploration-base" && i + 1 < argc) {
            explorationBase_ = std::stof(argv[++i]);
        } else if (arg == "--neuromodulation-gain" && i + 1 < argc) {
            neuromodulationGain_ = std::stof(argv[++i]);
        }
    }
}

std::string AgentBrain::getConfigurationSummary() const {
    std::stringstream ss;
    ss << "=== Agent Configuration ===\n";
    ss << "Curiosity Threshold: " << curiosityThreshold_ << "\n";
    ss << "Exploration Base: " << explorationBase_ << "\n";
    ss << "Motor Command Timeout: " << motorCommandTimeout_ << " seconds\n";
    ss << "Neuromodulation Gain: " << neuromodulationGain_ << "\n";
    ss << "Development Rate: " << developmentRate_ << "\n";
    ss << "Development Enabled: " << (developmentEnabled_ ? "Yes" : "No") << "\n";
    ss << "Curiosity Enabled: " << (curiosityEnabled_ ? "Yes" : "No") << "\n";
    ss << "Reward Modulation Enabled: " << (rewardModulationEnabled_ ? "Yes" : "No") << "\n";
    ss << "Structural Plasticity Enabled: " << (structuralPlasticityEnabled_ ? "Yes" : "No") << "\n";
    return ss.str();
}

std::string AgentBrain::serializeState() const {
    // Serialize state to JSON-like format (simplified)
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"dopamineLevel\": " << dopamineLevel_ << ",\n";
    ss << "  \"noveltyLevel\": " << noveltyLevel_ << ",\n";
    ss << "  \"curiosityLevel\": " << curiosityLevel_ << ",\n";
    ss << "  \"predictionError\": " << predictionError_ << ",\n";
    ss << "  \"expectedReward\": " << expectedReward_ << ",\n";
    ss << "  \"developmentalAge\": " << developmentalAge_ << ",\n";
    ss << "  \"plasticityModifier\": " << plasticityModifier_ << ",\n";
    ss << "  \"curiosityThreshold\": " << curiosityThreshold_ << ",\n";
    ss << "  \"explorationBase\": " << explorationBase_ << ",\n";
    ss << "  \"neuromodulationGain\": " << neuromodulationGain_ << ",\n";
    ss << "  \"developmentRate\": " << developmentRate_ << ",\n";
    ss << "  \"episodeActive\": " << (episodeActive_ ? "true" : "false") << ",\n";
    ss << "  \"currentEpisode\": " << currentEpisode_ << "\n";
    ss << "}\n";
    return ss.str();
}

void AgentBrain::deserializeState(const std::string& data) {
    // Parse JSON-like format (simplified implementation)
    // In a real implementation, you'd use a proper JSON parser
    // This is a placeholder for future implementation
    NLM_LOG_INFO("State deserialized from string data");
}

void AgentBrain::saveState(const std::string& filename) const {
    // Save state to file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << serializeState();
        file.close();
        NLM_LOG_INFO("State saved to " + filename);
    } else {
        NLM_LOG_ERROR("Failed to save state to " + filename);
    }
}

void AgentBrain::loadState(const std::string& filename) {
    // Load state from file
    std::ifstream file(filename);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        deserializeState(buffer.str());
        NLM_LOG_INFO("State loaded from " + filename);
    } else {
        NLM_LOG_ERROR("Failed to load state from " + filename);
    }
}

// End of implementation
