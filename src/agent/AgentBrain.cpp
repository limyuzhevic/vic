#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

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
    , predictionEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , stepCount_(0)
    , totalSimulationTime_(0.0)
    , accumulatedReward_(0.0f)
    , learningRate_(0.001f)
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
    stepCount_ = 0;
    totalSimulationTime_ = 0.0;
    accumulatedReward_ = 0.0f;
    firingRateHistory_.clear();
    rewardHistory_.clear();
    errorMessages_.clear();
    warningMessages_.clear();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

bool AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    bool validInput = true;
    
    if (!brain_) {
        reportError("Cannot process sensory input: brain is null");
        return false;
    }
    
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
    
    return validInput;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        reportError("Cannot decode motor command: brain is null");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = decodeMotorCommandWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    if (!brain_) {
        reportError("Cannot decode motor command: brain is null");
        return MotorCommand::Wait;
    }
    
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

MotorCommand AgentBrain::decodeMotorCommandWithCuriosity(MotorCommand defaultCmd) {
    if (!brain_) {
        reportError("Cannot apply curiosity: brain is null");
        return defaultCmd;
    }
    
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
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) reportError("Cannot apply reward modulation: brain is null");
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    accumulatedReward_ += reward;
    
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
                syn->addToWeight(delta * learningRate_);
                
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
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) reportError("Cannot update development: brain is null");
        return;
    }
    
    developmentalAge_ += timestep;
    totalSimulationTime_ += timestep;
    stepCount_++;
    
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
    
    // Record firing rate for metrics
    if (brain_) {
        float firingRate = brain_->getAverageFiringRate();
        firingRateHistory_.push_back(firingRate);
        if (firingRateHistory_.size() > 100) {
            firingRateHistory_.erase(firingRateHistory_.begin());
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        reportError("Cannot get developmental stage: brain is null");
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

float AgentBrain::getExpectedReward() const {
    return expectedReward_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    accumulatedReward_ = 0.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Clear error/warning messages
    errorMessages_.clear();
    warningMessages_.clear();
    
    // Clear historical data
    firingRateHistory_.clear();
    rewardHistory_.clear();
    
    NLM_LOG_INFO("AgentBrain reset: all state cleared and initialized");
}

bool AgentBrain::simulateEpisode(SimpleWorld& world, size_t numSteps, bool computeReward) {
    if (!brain_) {
        reportError("Cannot simulate episode: brain is null");
        return false;
    }
    
    NLM_LOG_INFO("Starting simulation episode with " + std::to_string(numSteps) + " steps");
    
    float totalReward = 0.0f;
    
    for (size_t step = 0; step < numSteps; ++step) {
        try {
            // Update world
            world.update(0.1);
            
            // Get sensory input from world
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // Process sensory input in brain
            bool validInput = processSensoryInput(percept);
            if (!validInput) {
                reportWarning("Sensory input processing failed at step " + std::to_string(step));
            }
            
            // Run brain step
            brain_->step(static_cast<SimulationStep>(step));
            
            // Get action from brain
            MotorCommand motorCmd = decodeMotorCommand();
            previousMotorCommand_ = motorCmd;
            
            // Apply motor command to world
            world.applyMotorCommand(motorCmd, world.getSimulationTime());
            
            // Compute reward if requested
            if (computeReward) {
                float reward = world.computeReward(brain_->getRegions()[0].get());
                totalReward += reward;
                
                // Apply reward modulation
                applyRewardModulation(reward, 0.0f);
                
                // Record reward for metrics
                rewardHistory_.push_back(reward);
                if (rewardHistory_.size() > 100) {
                    rewardHistory_.erase(rewardHistory_.begin());
                }
            }
            
            // Update development
            updateDevelopment(0.1);
            
            // Log progress periodically
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Reward: " + std::to_string(totalReward / (step + 1)) +
                           " | Firing: " + std::to_string(brain_->getAverageFiringRate()) +
                           " | Curiosity: " + std::to_string(curiosityLevel_));
            }
            
        } catch (const std::exception& e) {
            reportError("Error at step " + std::to_string(step) + ": " + std::string(e.what()));
            return false;
        }
    }
    
    NLM_LOG_INFO("Episode completed: " + std::to_string(numSteps) + " steps, " +
                std::to_string(totalReward) + " total reward, " +
                std::to_string(brain_->getAverageFiringRate()) + " average firing rate");
    
    return true;
}

std::map<std::string, float> AgentBrain::getAgentState() const {
    std::map<std::string, float> state;
    
    if (brain_) {
        state["neuron_count"] = static_cast<float>(brain_->getTotalNeuronCount());
        state["synapse_count"] = static_cast<float>(brain_->getTotalSynapseCount());
        state["firing_rate"] = brain_->getAverageFiringRate();
        state["total_spikes"] = static_cast<float>(brain_->getTotalSpikeCount());
    }
    
    state["curiosity_level"] = curiosityLevel_;
    state["novelty_level"] = noveltyLevel_;
    state["prediction_error"] = predictionError_;
    state["neuromodulation_level"] = dopamineLevel_;
    state["developmental_age"] = static_cast<float>(developmentalAge_);
    state["plasticity_modifier"] = plasticityModifier_;
    state["learning_rate"] = learningRate_;
    state["accumulated_reward"] = accumulatedReward_;
    
    return state;
}

std::string AgentBrain::getAgentStatus() const {
    std::ostringstream oss;
    oss << "Agent Status Report:\n";
    oss << "===================\n\n";
    
    oss << "Configuration:\n";
    oss << "  Reward modulation: " << (rewardModulationEnabled_ ? "enabled" : "disabled") << "\n";
    oss << "  Structural plasticity: " << (structuralPlasticityEnabled_ ? "enabled" : "disabled") << "\n";
    oss << "  Development: " << (developmentEnabled_ ? "enabled" : "disabled") << "\n";
    oss << "  Curiosity: " << (curiosityEnabled_ ? "enabled" : "disabled") << "\n";
    oss << "  Prediction: " << (predictionEnabled_ ? "enabled" : "disabled") << "\n\n";
    
    if (brain_) {
        oss << "Brain State:\n";
        oss << "  Neurons: " << brain_->getTotalNeuronCount() << "\n";
        oss << "  Synapses: " << brain_->getTotalSynapseCount() << "\n";
        oss << "  Firing rate: " << brain_->getAverageFiringRate() << " Hz\n";
        oss << "  Total spikes: " << brain_->getTotalSpikeCount() << "\n";
        oss << "  Developmental stage: " << static_cast<int>(getDevelopmentalStage()) << "\n\n";
    }
    
    oss << "Agent State:\n";
    oss << "  Curiosity level: " << curiosityLevel_ << "\n";
    oss << "  Novelty level: " << noveltyLevel_ << "\n";
    oss << "  Prediction error: " << predictionError_ << "\n";
    oss << "  Neuromodulation level: " << dopamineLevel_ << "\n";
    oss << "  Developmental age: " << developmentalAge_ << "s\n";
    oss << "  Plasticity modifier: " << plasticityModifier_ << "\n";
    oss << "  Learning rate: " << learningRate_ << "\n";
    oss << "  Accumulated reward: " << accumulatedReward_ << "\n\n";
    
    oss << "Performance:\n";
    oss << "  Steps simulated: " << stepCount_ << "\n";
    oss << "  Total simulation time: " << totalSimulationTime_ << "s\n";
    oss << "  Average firing rate: " << (firingRateHistory_.empty() ? 0.0f : 
                                   std::accumulate(firingRateHistory_.begin(), firingRateHistory_.end(), 0.0f) / 
                                   firingRateHistory_.size()) << " Hz\n";
    
    if (!errorMessages_.empty()) {
        oss << "\nErrors:\n";
        oss << errorMessages_ << "\n";
    }
    
    if (!warningMessages_.empty()) {
        oss << "\nWarnings:\n";
        for (const auto& warning : warningMessages_) {
            oss << "  - " << warning << "\n";
        }
    }
    
    return oss.str();
}

AgentBrain::AgentMetrics AgentBrain::getMetrics() const {
    AgentMetrics metrics;
    
    if (brain_) {
        metrics.averageFiringRate = brain_->getAverageFiringRate();
        metrics.totalSteps = stepCount_;
        metrics.activeNeurons = brain_->getActiveNeuronCount();
    }
    
    metrics.curiosityLevel = curiosityLevel_;
    metrics.noveltyLevel = noveltyLevel_;
    metrics.predictionError = predictionError_;
    metrics.neuromodulationLevel = dopamineLevel_;
    
    return metrics;
}

std::string AgentBrain::metricsToString() const {
    std::ostringstream oss;
    AgentMetrics metrics = getMetrics();
    
    oss << "Agent Metrics:\n";
    oss << "================\n\n";
    oss << "Neural Activity:\n";
    oss << "  Average firing rate: " << metrics.averageFiringRate << " Hz\n";
    oss << "  Active neurons: " << metrics.activeNeurons << "\n";
    oss << "  Total steps: " << metrics.totalSteps << "\n\n";
    
    oss << "Cognitive State:\n";
    oss << "  Curiosity level: " << metrics.curiosityLevel << "\n";
    oss << "  Novelty level: " << metrics.noveltyLevel << "\n";
    oss << "  Prediction error: " << metrics.predictionError << "\n";
    oss << "  Neuromodulation level: " << metrics.neuromodulationLevel << "\n\n";
    
    oss << "Learning State:\n";
    oss << "  Developmental age: " << developmentalAge_ << "s\n";
    oss << "  Plasticity modifier: " << plasticityModifier_ << "\n";
    oss << "  Learning rate: " << learningRate_ << "\n";
    oss << "  Accumulated reward: " << accumulatedReward_ << "\n";
    
    return oss.str();
}

void AgentBrain::reportError(const std::string& error) {
    std::ostringstream oss;
    oss << "[ERROR] " << error;
    errorMessages_ += oss.str() + "\n";
    NLM_LOG_ERROR(oss.str());
}

void AgentBrain::reportWarning(const std::string& warning) {
    std::ostringstream oss;
    oss << "[WARNING] " << warning;
    warningMessages_.push_back(oss.str());
    NLM_LOG_WARNING(oss.str());
}

} // namespace nlm

// Additional helper functions for Python bindings
template<typename T>
T getOptionalValue(const std::map<std::string, float>& state, const std::string& key, T defaultValue) {
    auto it = state.find(key);
    if (it != state.end()) {
        return static_cast<T>(it->second);
    }
    return defaultValue;
}

// Python-specific helper functions
void addAgentBrainHelpers(py::module& m) {
    m.def("getOptionalValue", &getOptionalValue<double>,
         py::arg("state"), py::arg("key"), py::arg("default_value"),
         "Get optional value from agent state");
    
    m.def("printAgentMetrics", [](const std::string& metrics) {
        std::cout << metrics << std::endl;
    }, py::arg("metrics"), "Print agent metrics to console");
}
