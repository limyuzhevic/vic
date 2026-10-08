#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

namespace nlm {

// Configuration constants - moved from magic numbers
namespace AgentBrainConfig {
    constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.1f;
    constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f;
    constexpr float DEFAULT_EXPLORATION_CHANCE = 0.3f;
    constexpr float DEFAULT_EXPLORATION_CURIOUSITY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_REWARD_MODULATION_CLAMP = 1.0f;
    constexpr float DEFAULT_PLASTICITY_FACTOR_MIN = 0.1f;
    constexpr float DEFAULT_PLASTICITY_FACTOR_MAX = 2.0f;
    constexpr float DEFAULT_WAIT_THRESHOLD = 0.5f;
    constexpr float DEFAULT_WAIT_THRESHOLD_FOR_NO_ACTION = 0.5f;
    constexpr float DEFAULT_DEVELOPMENT_STAGE_1_END = 60.0;  // ~1 minute
    constexpr float DEFAULT_DEVELOPMENT_STAGE_2_END = 300.0; // ~5 minutes
    constexpr float DEFAULT_DEVELOPMENT_STAGE_3_END = 900.0; // ~15 minutes
    constexpr float DEFAULT_INITIAL_PLASTICITY = 1.0f;
    constexpr float DEFAULT_CRITICAL_PERIOD_PLASTICITY = 0.8f;
    constexpr float DEFAULT_MATURATION_PLASTICITY = 0.5f;
    constexpr float DEFAULT_ADULT_PLASTICITY = 0.2f;
    constexpr float DEFAULT_SYNAPTAGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(AgentBrainConfig::DEFAULT_INITIAL_PLASTICITY)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(AgentBrainConfig::DEFAULT_NOVELTY_DECAY)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        // Create smart pointer container for motor neurons
        std::vector<std::unique_ptr<Neuron>> motorNeurons;
        std::vector<std::unique_ptr<Neuron>> sensoryNeurons;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        motorNeurons.push_back(std::unique_ptr<Neuron>(n));
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        sensoryNeurons.push_back(std::unique_ptr<Neuron>(n));
                    }
                }
            }
        }
        
        // Distribute motor neurons to different action groups
        distributeMotorNeurons(motorNeurons);
        
        // Distribute sensory neurons
        distributeSensoryNeurons(sensoryNeurons);
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

void AgentBrain::distributeMotorNeurons(std::vector<std::unique_ptr<Neuron>> neurons) {
    for (size_t i = 0; i < neurons.size(); ++i) {
        size_t idx = motorForward_.size() + motorBackward_.size() + 
                    motorTurnLeft_.size() + motorTurnRight_.size() +
                    motorInteract_.size() + motorWait_.size();
        
        switch (idx % 6) {
            case 0: motorForward_.push_back(std::move(neurons[i])); break;
            case 1: motorBackward_.push_back(std::move(neurons[i])); break;
            case 2: motorTurnLeft_.push_back(std::move(neurons[i])); break;
            case 3: motorTurnRight_.push_back(std::move(neurons[i])); break;
            case 4: motorInteract_.push_back(std::move(neurons[i])); break;
            case 5: motorWait_.push_back(std::move(neurons[i])); break;
        }
    }
}

void AgentBrain::distributeSensoryNeurons(std::vector<std::unique_ptr<Neuron>> neurons) {
    for (size_t i = 0; i < neurons.size(); ++i) {
        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                    sensoryInternal_.size() + sensoryProprioception_.size();
        
        switch (idx % 4) {
            case 0: sensoryVision_.push_back(std::move(neurons[i])); break;
            case 1: sensoryTouch_.push_back(std::move(neurons[i])); break;
            case 2: sensoryInternal_.push_back(std::move(neurons[i])); break;
            case 3: sensoryProprioception_.push_back(std::move(neurons[i])); break;
        }
    }
}

float AgentBrain::calculateNeuronActivity(const std::vector<std::unique_ptr<Neuron>>& neurons) {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand AgentBrain::selectBestMotorCommand(
    const std::vector<MotorCommand>& commands,
    const std::vector<float>& activities
) {
    if (commands.empty() || activities.empty()) return MotorCommand::Wait;
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = activities.back();  // Default to wait if nothing stronger
    
    for (size_t i = 0; i < commands.size(); ++i) {
        if (activities[i] > bestActivity) {
            bestActivity = activities[i];
            best = commands[i];
        }
    }
    
    return bestActivity < AgentBrainConfig::DEFAULT_WAIT_THRESHOLD ? MotorCommand::Wait : best;
}

float AgentBrain::applyNoveltyDecay(float novelty) {
    return novelty * AgentBrainConfig::DEFAULT_NOVELTY_DECAY;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action (configurable)
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
        noveltyLevel_ = applyNoveltyDecay(noveltyLevel_);
        
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
    // Calculate average activity in each motor group using improved helper
    float forwardAct = calculateNeuronActivity(motorForward_);
    float backwardAct = calculateNeuronActivity(motorBackward_);
    float leftAct = calculateNeuronActivity(motorTurnLeft_);
    float rightAct = calculateNeuronActivity(motorTurnRight_);
    float interactAct = calculateNeuronActivity(motorInteract_);
    float waitAct = calculateNeuronActivity(motorWait_);
    
    // Create command/activity pairs
    std::vector<MotorCommand> commands = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    
    std::vector<float> activities = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    
    // Use improved selection helper
    MotorCommand best = selectBestMotorCommand(commands, activities);
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConfig::DEFAULT_EXPLORATION_CURIOUSITY_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentBrainConfig::DEFAULT_EXPLORATION_CHANCE;
        
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
    
    // Clamp to reasonable range using config
    dopamineLevel_ = std::clamp(dopamineLevel_, 
                               -AgentBrainConfig::DEFAULT_REWARD_MODULATION_CLAMP,
                               AgentBrainConfig::DEFAULT_REWARD_MODULATION_CLAMP);
    
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
    
    // Modulate plasticity based on dopamine using improved helper
    float plasticityFactor = calculatePlasticityFactor(dopamineLevel_);
    
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
    
    // Simple developmental stages based on age using config constants
    if (developmentalAge_ < AgentBrainConfig::DEFAULT_DEVELOPMENT_STAGE_1_END) {  // ~1 minute
        plasticityModifier_ = AgentBrainConfig::DEFAULT_INITIAL_PLASTICITY;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentBrainConfig::DEFAULT_DEVELOPMENT_STAGE_2_END) {  // ~5 minutes
        plasticityModifier_ = AgentBrainConfig::DEFAULT_CRITICAL_PERIOD_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentBrainConfig::DEFAULT_DEVELOPMENT_STAGE_3_END) {  // ~15 minutes
        plasticityModifier_ = AgentBrainConfig::DEFAULT_MATURATION_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = AgentBrainConfig::DEFAULT_ADULT_PLASTICITY;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = AgentBrainConfig::DEFAULT_SYNAPTAGENESIS_RATE * plasticityModifier_;
            float pruneRate = AgentBrainConfig::DEFAULT_PRUNING_RATE * (2.0f - plasticityModifier_);
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
    plasticityModifier_ = AgentBrainConfig::DEFAULT_INITIAL_PLASTICITY;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
