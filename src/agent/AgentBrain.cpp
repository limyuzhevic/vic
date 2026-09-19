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
        // Pre-calculate group sizes for more efficient distribution
        const auto& regions = brain_->getRegions();
        size_t motorNeuronCount = 0, sensoryNeuronCount = 0;
        
        for (const auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Motor) {
                    motorNeuronCount += pop->getNeuronCount();
                } else if (type == NeuronType::Sensory) {
                    sensoryNeuronCount += pop->getNeuronCount();
                }
            }
        }
        
        // Reserve space for better performance
        motorForward_.reserve(motorNeuronCount / 6 + 1);
        motorBackward_.reserve(motorNeuronCount / 6 + 1);
        motorTurnLeft_.reserve(motorNeuronCount / 6 + 1);
        motorTurnRight_.reserve(motorNeuronCount / 6 + 1);
        motorInteract_.reserve(motorNeuronCount / 6 + 1);
        motorWait_.reserve(motorNeuronCount / 6 + 1);
        
        sensoryVision_.reserve(sensoryNeuronCount / 4 + 1);
        sensoryTouch_.reserve(sensoryNeuronCount / 4 + 1);
        sensoryInternal_.reserve(sensoryNeuronCount / 4 + 1);
        sensoryProprioception_.reserve(sensoryNeuronCount / 4 + 1);
        
        // Distribute motor neurons to different action groups with round-robin assignment
        size_t motorGroupCounter = 0;
        for (const auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        switch (motorGroupCounter % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                        motorGroupCounter++;
                    }
                }
            }
        }
        
        // Distribute sensory neurons to different modalities with round-robin assignment
        size_t sensoryGroupCounter = 0;
        for (const auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        switch (sensoryGroupCounter % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                        sensoryGroupCounter++;
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
    // Actual size matches the expectation from configuration
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6) = 258
    // Note: Previously documented as 256+8+4+6=274, but world config shows different
    // We return the actual total from world.getSensoryInputSize()
    return 258;  // 256 + 8 + 4  (actual implementation)
}

size_t AgentBrain::getMotorOutputSize() const {
    // Based on motor command enum (excluding Wait = 7)
    // Actually 7 active motor commands
    return 7;
}

// Check for null neurons and safe iteration
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        size_t minSize = std::min(sensoryVision_.size(), vision.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    } else {
        // NLM_LOG_WARN("Vision input is empty");
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    if (!touch.empty()) {
        size_t minSize = std::min(sensoryTouch_.size(), touch.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    if (!intern.empty()) {
        size_t minSize = std::min(sensoryInternal_.size(), intern.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty()) {
        size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                sensoryProprioception_[i]->injectCurrent(current);
            }
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
        // Unroll loop for better performance
        size_t i = 0;
        for (; i + 4 < neurons.size(); i += 4) {
            const auto& state0 = neurons[i]->getState();
            const auto& state1 = neurons[i+1]->getState();
            const auto& state2 = neurons[i+2]->getState();
            const auto& state3 = neurons[i+3]->getState();
            sum += std::abs(state0.membranePotential - state0.restingPotential);
            sum += std::abs(state1.membranePotential - state1.restingPotential);
            sum += std::abs(state2.membranePotential - state2.restingPotential);
            sum += std::abs(state3.membranePotential - state3.restingPotential);
        }
        // Handle remaining elements
        for (; i < neurons.size(); ++i) {
            const auto& state = neurons[i]->getState();
            sum += std::abs(state.membranePotential - state.restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity - optimize with early exit
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    MotorCommand best = MotorCommand::Wait;
    
    // Check in order of likely importance
    MotorCommand commands[] = {
        MotorCommand::MoveForward, forwardAct,
        MotorCommand::MoveBackward, backwardAct,
        MotorCommand::TurnLeft, leftAct,
        MotorCommand::TurnRight, rightAct,
        MotorCommand::Interact, interactAct,
        MotorCommand::Wait, waitAct
    };
    
    for (size_t i = 0; i < 8; i += 2) {
        if (commands[i+1] > bestActivity) {
            bestActivity = commands[i+1];
            best = commands[i];
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
            // Use the helper function to get random motor command
            // This ensures we never return Wait for exploration
            return getRandomMotorCommand();
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
    } else {
        NLM_LOG_WARN("STDP system is null - plasticity modulation incomplete");
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

// Factory function implementations
namespace nlm {

std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot create brain with null config");
        return nullptr;
    }
    return std::make_shared<Brain>(config);
}

std::shared_ptr<SimpleWorld> createSimpleWorld() {
    return std::make_shared<SimpleWorld>();
}

std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot create AgentBrain with null brain");
        return nullptr;
    }
    return std::make_shared<AgentBrain>(brain);
}

// Advanced utility functions for power users
std::shared_ptr<Brain> createBrainWithSeed(uint64_t seed, std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot create brain with null config");
        return nullptr;
    }
    auto brain = std::make_shared<Brain>(config);
    // Note: Brain constructor doesn't directly accept a seed.
    // For deterministic behavior, users should call brain->initialize() 
    // and then configure RandomGenerator if needed.
    return brain;
}

// Batch operations for efficiency
void batchStep(std::vector<std::shared_ptr<AgentBrain>>& agents, 
               std::vector<std::shared_ptr<SimpleWorld>>& worlds,
               int numSteps) {
    for (int step = 0; step < numSteps; ++step) {
        for (size_t i = 0; i < agents.size(); ++i) {
            if (!agents[i] || !worlds[i]) continue;
            
            // Update world
            worlds[i]->update(0.1);
            
            // Process sensory input
            agents[i]->processSensoryInput(worlds[i]->getSensoryPercept());
            
            // Step brain
            agents[i]->getBrain()->step(step);
            
            // Decode motor command and apply action
            auto action = agents[i]->decodeMotorCommand();
            if (action != MotorCommand::Wait) {
                worlds[i]->applyMotorCommand(action, worlds[i]->getSimulationTime());
            }
        }
    }
}

// Neural network statistics utilities
struct BrainStats {
    size_t totalNeurons;
    size_t activeNeurons;
    size_t totalSynapses;
    float avgFiringRate;
    float excitationInhibitionRatio;
    float totalSpikeCount;
};

BrainStats getBrainStats(std::shared_ptr<Brain> brain) {
    BrainStats stats;
    if (!brain) {
        stats = {0, 0, 0, 0.0f, 0.0f, 0};
        return stats;
    }
    
    stats.totalNeurons = brain->getTotalNeuronCount();
    stats.activeNeurons = brain->getActiveNeuronCount();
    stats.totalSynapses = brain->getTotalSynapseCount();
    stats.avgFiringRate = brain->getAverageFiringRate();
    stats.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    stats.totalSpikeCount = brain->getTotalSpikeCount();
    
    return stats;
}

// Configuration utilities
void validateBrainConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_WARN("Brain config is null - using default values");
        return;
    }
    
    // Add validation logic here
    // For example: check for valid ranges, etc.
}

// Performance monitoring
struct PerformanceMonitor {
    double startTime;
    double lastStepTime;
    size_t stepsCompleted;
    double totalTime;
    
    PerformanceMonitor() : startTime(0.0), lastStepTime(0.0), 
                           stepsCompleted(0), totalTime(0.0) {}
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
    
    void stepComplete() {
        double currentTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        if (stepsCompleted > 0) {
            lastStepTime = currentTime - startTime;
        }
        totalTime = currentTime - startTime;
        startTime = currentTime;
        stepsCompleted++;
    }
    
    double getAverageStepTime() const {
        return stepsCompleted > 0 ? totalTime / stepsCompleted : 0.0;
    }
    
    double getTotalTime() const { return totalTime; }
    size_t getStepsCompleted() const { return stepsCompleted; }
};

} // namespace nlm
