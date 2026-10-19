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
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm

// ========== ADVANCED AGENT FEATURES ==========

// Advanced agent operations
void AgentBrain::setLearningRate(float rate) {
    NLM_LOG_INFO("Setting learning rate: " + std::to_string(rate));
    
    if (rate < 0.0f || rate > 1.0f) {
        NLM_LOG_ERROR("Learning rate must be between 0.0 and 1.0, got " + std::to_string(rate));
        return;
    }
    
    // Apply to relevant systems
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * rate);
        stdp->setLTDWeight(0.012f * rate);
    }
    
    auto* hebbian = brain_->getHebbian();
    if (hebbian) {
        hebbian->setLearningRate(rate);
    }
    
    plasticityModifier_ = rate;
}

void AgentBrain::enableReplay(bool enable, size_t bufferSize) {
    NLM_LOG_INFO("Replay system " + std::string(enable ? "enabled" : "disabled") + 
                " (buffer size=" + std::to_string(bufferSize) + ")");
    
    if (enable) {
        // In a real implementation, this would set up replay buffers
        // For now, just log the action
        NLM_LOG_INFO("Replay system configured for episode replay with " + 
                    std::to_string(bufferSize) + " episodes");
    }
}

std::map<std::string, float> AgentBrain::getPerformanceStats() const {
    NLM_LOG_INFO("Collecting performance statistics");
    
    std::map<std::string, float> stats;
    
    // Basic performance metrics
    stats["neuromodulation_level"] = getNeuromodulationLevel();
    stats["curiosity_level"] = getCuriosityLevel();
    stats["novelty_level"] = getNoveltyLevel();
    stats["prediction_error"] = getPredictionError();
    stats["developmental_stage"] = static_cast<float>(getDevelopmentalStage());
    stats["plasticity_modifier"] = plasticityModifier_;
    
    // Neural metrics if brain exists
    if (brain_) {
        stats["total_neurons"] = static_cast<float>(brain_->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<float>(brain_->getTotalSynapseCount());
        stats["firing_rate"] = brain_->getAverageFiringRate();
        stats["total_spikes"] = static_cast<float>(brain_->getTotalSpikeCount());
        stats["active_neurons"] = static_cast<float>(brain_->getActiveNeuronCount());
    }
    
    return stats;
}

std::string AgentBrain::analyzeBehavior(size_t window) const {
    NLM_LOG_INFO("Analyzing agent behavior (window=" + std::to_string(window) + ")");
    
    // Simple behavioral analysis
    // In a real implementation, this would analyze decision patterns, action sequences, etc.
    
    std::stringstream ss;
    ss << "Behavior Analysis (last " << window << " actions):\n";
    
    if (brain_) {
        ss << "  Developmental Stage: " << static_cast<int>(getDevelopmentalStage()) << "\n";
        ss << "  Neuromodulation: " << getNeuromodulationLevel() << " (curiosity: " << 
            getCuriosityLevel() << ", novelty: " << getNoveltyLevel() << ")\n";
        ss << "  Plasticity Modifier: " << plasticityModifier_ << "\n";
        ss << "  Total Neural Activity: " << brain_->getTotalSpikeCount() << " spikes\n";
        ss << "  Current Action Pattern: ";
        
        // Determine dominant action pattern based on neuromodulation levels
        if (getNeuromodulationLevel() > 0.5f) {
            ss << "reward-seeking\n";
        } else if (getCuriosityLevel() > 0.5f) {
            ss << "exploration\n";
        } else if (getNoveltyLevel() > 0.5f) {
            ss << "novelty-seeking\n";
        } else {
            ss << "conservative\n";
        }
    }
    
    std::string result = ss.str();
    NLM_LOG_INFO("Behavior analysis:\n" + result);
    return result;
}

std::vector<MotorCommand> AgentBrain::planAction(size_t depth) const {
    NLM_LOG_INFO("Planning action sequence (depth=" + std::to_string(depth) + ")");
    
    std::vector<MotorCommand> plan;
    
    // Simple action planning
    // In a real implementation, this would use the neural planner
    
    // Determine current state
    bool highCuriosity = curiosityLevel_ > 0.5f;
    bool highNovelty = noveltyLevel_ > 0.5f;
    bool highPredictionError = std::abs(predictionError_) > 0.5f;
    
    // Generate plan based on state
    if (highCuriosity && highNovelty) {
        // Exploration
        for (size_t i = 0; i < depth; ++i) {
            if (i % 3 == 0) plan.push_back(MotorCommand::MoveForward);
            else if (i % 3 == 1) plan.push_back(MotorCommand::TurnLeft);
            else plan.push_back(MotorCommand::TurnRight);
        }
    } else if (highPredictionError) {
        // Correction-based
        plan.push_back(MotorCommand::Wait);
        for (size_t i = 1; i < depth; ++i) {
            plan.push_back(MotorCommand::Interact);
        }
    } else {
        // Default behavior
        for (size_t i = 0; i < depth; ++i) {
            plan.push_back(MotorCommand::MoveForward);
        }
    }
    
    NLM_LOG_INFO("Action plan generated: " + std::to_string(plan.size()) + " actions");
    return plan;
}

// Analysis tools
std::vector<MotorCommand> AgentBrain::getDecisionTrail(size_t count) const {
    NLM_LOG_INFO("Retrieving decision trail (count=" + std::to_string(count) + ")");
    
    // In a real implementation, this would retrieve actual decision history
    // For now, return a placeholder trail
    std::vector<MotorCommand> trail;
    
    for (size_t i = 0; i < std::min(count, static_cast<size_t>(10)); ++i) {
        trail.push_back(MotorCommand::Wait);
    }
    
    NLM_LOG_INFO("Decision trail retrieved: " + std::to_string(trail.size()) + " commands");
    return trail;
}

// Performance and debugging
bool AgentBrain::traceExecution(std::vector<MotorCommand>& trail, size_t steps) const {
    NLM_LOG_INFO("Tracing execution (steps=" + std::to_string(steps) + ")");
    
    // Clear existing trail
    trail.clear();
    
    // Generate trace based on current state
    bool highCuriosity = curiosityLevel_ > 0.5f;
    
    for (size_t i = 0; i < steps; ++i) {
        if (highCuriosity && i % 3 == 0) {
            trail.push_back(MotorCommand::MoveForward);
        } else {
            trail.push_back(MotorCommand::Wait);
        }
    }
    
    NLM_LOG_INFO("Execution trace generated: " + std::to_string(trail.size()) + " steps");
    return !trail.empty();
}
