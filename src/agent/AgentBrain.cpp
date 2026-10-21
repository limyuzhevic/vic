#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>
#include <ctime>

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
    , curiosityThreshold_(0.5f)
    , explorationRate_(0.1f)
    , developmentSpeed_(1.0)
    , processingStepCount_(0)
    , processingTimePerStep_(0.0)
    , performanceStepCount_(0)
    , maxEnergy_(100.0f)
    , minEnergy_(0.0f)
    , energy_(maxEnergy_)
    , worldInterfaceActive_(false)
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
    previousVision_.clear();
}

// Advanced methods for power users
void AgentBrain::processBatchSensoryInputs(const std::vector<SensoryPercept>& percepts) {
    for (const auto& percept : percepts) {
        processSensoryInput(percept);
    }
}

std::vector<MotorCommand> AgentBrain::decodeBatchMotorCommands(size_t count) {
    std::vector<MotorCommand> commands;
    commands.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        commands.push_back(decodeMotorCommand());
    }
    
    return commands;
}

void AgentBrain::applyCustomModulation(const std::map<std::string, float>& customSignals) {
    for (const auto& signal : customSignals) {
        std::string key = signal.first;
        float value = signal.second;
        
        if (key == "dopamine" || key == "reward") {
            dopamineLevel_ = value;
        } else if (key == "curiosity") {
            curiosityLevel_ = value;
        } else if (key == "novelty") {
            noveltyLevel_ = value;
        } else if (key == "prediction_error") {
            predictionError_ = value;
        } else if (key == "expected_reward") {
            expectedReward_ = value;
        }
    }
}

std::map<std::string, float> AgentBrain::getModulationState() const {
    std::map<std::string, float> state;
    state["dopamine"] = dopamineLevel_;
    state["curiosity"] = curiosityLevel_;
    state["novelty"] = noveltyLevel_;
    state["prediction_error"] = predictionError_;
    state["expected_reward"] = expectedReward_;
    return state;
}

void AgentBrain::setDevelopmentSpeed(float speed) {
    if (speed < 0.0f) speed = 0.0f;
    
    plasticityModifier_ = speed;
    
    // Update brain developmental stage based on speed
    if (speed > 2.0f) {
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (speed > 1.0f) {
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else if (speed > 0.5f) {
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    } else {
        brain_->setDevelopmentalStage(DevelopmentalStage::Aging);
    }
}

void AgentBrain::pauseDevelopment(bool pause) {
    if (pause) {
        // Store current speed
        developmentSpeed_ = plasticityModifier_;
        plasticityModifier_ = 0.0f;
        
        // Update brain
        if (brain_) {
            // Don't change developmental stage, just stop development
        }
    } else {
        // Resume development
        if (developmentSpeed_ > 0.0f) {
            plasticityModifier_ = developmentSpeed_;
        } else {
            plasticityModifier_ = 1.0f;
        }
    }
}

double AgentBrain::getDevelopmentalAge() const {
    return developmentalAge_;
}

void AgentBrain::setCuriosityThreshold(float threshold) {
    if (threshold < 0.0f) threshold = 0.0f;
    curiosityThreshold_ = threshold;
}

void AgentBrain::setExplorationRate(float rate) {
    if (rate < 0.0f) rate = 0.0f;
    if (rate > 1.0f) rate = 1.0f;
    explorationRate_ = rate;
}

float AgentBrain::getExplorationRate() const {
    return explorationRate_;
}

void AgentBrain::enableAllLearning(bool enable) {
    rewardModulationEnabled_ = enable;
    structuralPlasticityEnabled_ = enable;
    developmentEnabled_ = enable;
    curiosityEnabled_ = enable;
    
    // Update brain systems if available
    if (brain_) {
        if (enable) {
            brain_->enableAllPlasticity(true);
        }
        // Development system is controlled by agent
    }
}

void AgentBrain::resetAllMemories() {
    if (brain_) {
        brain_->reset();
    }
    
    // Clear agent's memory state
    previousVision_.clear();
    // Additional memory reset logic would go here
}

size_t AgentBrain::getMemoryStats() const {
    size_t totalStats = 0;
    if (brain_) {
        // This would aggregate stats from all memory systems
        // For now, return a placeholder
        totalStats = brain_->getTotalNeuronCount() + brain_->getTotalSynapseCount();
    }
    return totalStats;
}

double AgentBrain::getProcessingTimePerStep() const {
    return processingTimePerStep_;
}

size_t AgentBrain::getPerformanceStats() const {
    size_t totalStats = 0;
    // Aggregate performance statistics from all systems
    if (brain_) {
        totalStats += brain_->getTotalSpikeCount();
        totalStats += brain_->getFiringNeuronCount();
    }
    totalStats += performanceStepCount_;
    return totalStats;
}

void AgentBrain::resetPerformanceStats() {
    processingTimePerStep_ = 0.0;
    performanceStepCount_ = 0;
}

void AgentBrain::setActionLimits(const std::map<ActionType, float>& limits) {
    // Store action limits for validation
    actionLimits_ = limits;
}

void AgentBrain::setEnergyLimits(float maxEnergy, float minEnergy) {
    maxEnergy_ = maxEnergy;
    minEnergy_ = minEnergy;
}

bool AgentBrain::checkSafetyConstraints() const {
    // Check energy constraints
    if (energy_ < minEnergy_) return false;
    if (energy_ > maxEnergy_) return false;
    
    // Check developmental constraints
    if (developmentalAge_ > 1000.0) return false;  // Reasonable age limit
    
    // Check plasticity constraints
    if (plasticityModifier_ < 0.0f || plasticityModifier_ > 10.0f) return false;
    
    // Additional safety checks would go here
    
    return true;
}

std::string AgentBrain::exportState() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"dopamine\": " << dopamineLevel_ << ",\n";
    ss << "  \"curiosity\": " << curiosityLevel_ << ",\n";
    ss << "  \"novelty\": " << noveltyLevel_ << ",\n";
    ss << "  \"prediction_error\": " << predictionError_ << ",\n";
    ss << "  \"expected_reward\": " << expectedReward_ << ",\n";
    ss << "  \"developmental_age\": " << developmentalAge_ << ",\n";
    ss << "  \"plasticity_modifier\": " << plasticityModifier_ << ",\n";
    ss << "  \"reward_modulation_enabled\": " << (rewardModulationEnabled_ ? "true" : "false") << ",\n";
    ss << "  \"structural_plasticity_enabled\": " << (structuralPlasticityEnabled_ ? "true" : "false") << ",\n";
    ss << "  \"development_enabled\": " << (developmentEnabled_ ? "true" : "false") << ",\n";
    ss << "  \"curiosity_enabled\": " << (curiosityEnabled_ ? "true" : "false") << "\n";
    ss << "}\n";
    
    return ss.str();
}

bool AgentBrain::importState(const std::string& state) {
    // Parse JSON-like state string (simplified implementation)
    // In a real implementation, this would use a proper JSON parser
    try {
        size_t pos = 0;
        
        // Extract dopamine level
        pos = state.find("\"dopamine\": ");
        if (pos != std::string::npos) {
            pos += 13;
            size_t end = state.find(',', pos);
            if (end != std::string::npos) {
                dopamineLevel_ = std::stof(state.substr(pos, end - pos));
            }
        }
        
        // Extract curiosity level
        pos = state.find("\"curiosity\": ");
        if (pos != std::string::npos) {
            pos += 14;
            size_t end = state.find(',', pos);
            if (end != std::string::npos) {
                curiosityLevel_ = std::stof(state.substr(pos, end - pos));
            }
        }
        
        // Extract other fields (simplified)
        // In production, use proper JSON parsing
        
        return true;
    } catch (...) {
        return false;
    }
}

void AgentBrain::calibrateMotorMapping(const std::map<ActionType, std::vector<float>>& mapping) {
    // Store motor calibration data
    motorMapping_ = mapping;
    
    // Apply calibration to brain
    if (brain_) {
        // Update motor neuron connections based on calibration
        // This would involve modifying synaptic weights
    }
}

void AgentBrain::calibrateSensoryScaling(const std::map<SensoryType, float>& scaling) {
    // Store sensory calibration data
    sensoryScaling_ = scaling;
    
    // Apply calibration to brain
    if (brain_) {
        // Update sensory neuron sensitivity based on calibration
    }
}

void AgentBrain::autoTuneParameters() {
    // Automated parameter optimization using simple heuristics
    // This is a placeholder for advanced optimization algorithms
    
    // Adjust development speed based on performance
    double avgPerformance = static_cast<double>(getPerformanceStats()) / 
                          std::max(1.0, performanceStepCount_);
    
    if (avgPerformance > 1000.0) {
        // Performance is good, slightly increase development speed
        setDevelopmentSpeed(plasticityModifier_ * 1.05f);
    } else if (avgPerformance < 100.0) {
        // Performance is poor, slow down development
        setDevelopmentSpeed(plasticityModifier_ * 0.95f);
    }
    
    // Reset performance stats after tuning
    resetPerformanceStats();
}

} // namespace nlm
