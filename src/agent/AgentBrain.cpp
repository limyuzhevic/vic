#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Configuration constants for magic numbers
namespace ConfigConstants {
    // Learning rates
    const float LEARNING_RATE_EXPECTED_REWARD = 0.95f;    // Exponential moving average
    const float LEARNING_RATE_REWARD_CONTRIBUTION = 0.05f;
    
    // Plasticity modulation
    const float PLASTICITY_MODULATOR_BASE = 0.5f;
    const float PLASTICITY_MODULATOR_GAIN = 0.5f;
    const float PLASTICITY_MIN = 0.1f;
    const float PLASTICITY_MAX = 2.0f;
    
    // Eligibility trace
    const float ELIGIBILITY_THRESHOLD = 0.001f;
    const float ELIGIBILITY_DECAY = 0.1f;
    
    // STDP parameters
    const float STDP_LTP_WEIGHT_BASE = 0.01f;
    const float STDP_LTD_WEIGHT_BASE = 0.012f;
    
    // Development stages
    const double DEVELOPMENT_INITIAL_AGE = 60.0;    // ~1 minute
    const double DEVELOPMENT_CRITICAL_PERIOD_AGE = 300.0;  // ~5 minutes
    const double DEVELOPMENT_MATURATION_AGE = 900.0;      // ~15 minutes
    
    // Plasticity modifiers
    const float PLASTICITY_INITIAL = 1.0f;
    const float PLASTICITY_CRITICAL_PERIOD = 0.8f;
    const float PLASTICITY_MATURATION = 0.5f;
    const float PLASTICITY_ADULT = 0.2f;  // Adult - more stable
    
    // Structural plasticity rates
    const float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    const float PRUNING_BASE_RATE = 0.00001f;
    
    // Novelty detection
    const float SENSORY_NOVELTY_DECAY = 0.99f;
    
    // Curiosity thresholds
    const float CURIOSITY_EXPLORATION_THRESHOLD = 0.3f;
    const float CURIOSITY_HIGH_EXPLORATION = 0.5f;
    const float CURIOSITY_EXPLORATION_CHANCE = 0.3f;  // Max 30% random
    
    // Motor decoding
    const float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    const float MOTOR_ACTIVITY_SCALE_EXCITATION = 1.5f;
    const float MOTOR_ACTIVITY_SCALE_INHIBITION = 0.5f;
    
    // Memory
    const float MEMORY_ACTIVATION_THRESHOLD = 0.1f;
    const float MEMORY_DECAY_RATE_DEFAULT = 0.05f;
    const int MEMORY_MAX_TRACES_DEFAULT = 100;
    
    // Novelty normalization
    const float VISION_NOVELTY_MIN_ELEMENTS = 1.0f;
};

// Forward declaration for cleaner structure
class AgentBrain;

// AgentBrain implementation class - encapsulates complexity
class AgentBrain::Impl {
public:
    explicit Impl(std::shared_ptr<Brain> brain) : brain_(brain) {
        // Initialize configuration constants
        plasticityModifier_ = ConfigConstants::PLASTICITY_INITIAL;
        sensoryNoveltyDecay_ = ConfigConstants::SENSORY_NOVELTY_DECAY;
        rewardModulationEnabled_ = true;
        structuralPlasticityEnabled_ = true;
        developmentEnabled_ = true;
        curiosityEnabled_ = true;
    }
    
    ~Impl() = default;
    
    // Initialize motor and sensory neuron groups
    void initializeNeuronGroups() {
        if (!brain_) return;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    distributeMotorNeurons(pop->getNeurons());
                } else if (type == NeuronType::Sensory) {
                    distributeSensoryNeurons(pop->getNeurons());
                }
            }
        }
    }
    
    void distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
        for (Neuron* n : neurons) {
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
    }
    
    void distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
        for (Neuron* n : neurons) {
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
    
    // Process sensory input with constants
    void processSensoryInput(const SensoryPercept& percept) {
        const auto& vision = percept.getVision();
        
        // Process vision input
        processVisionInput(vision);
        
        // Process other sensory modalities
        processTouchInput(percept.getTouch());
        processInternalInput(percept.getInternal());
        processProprioceptionInput(percept.getProprioception());
        
        // Compute and store novelty
        computeAndStoreNovelty(vision);
        
        // Update curiosity based on novelty and prediction error
        updateCuriosityLevel();
    }
    
    float computeVisionDifference(const std::vector<float>& vision) const {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        return totalDiff;
    }
    
    void processVisionInput(const std::vector<float>& vision) {
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                float current = vision[i] * 5.0f;
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    void processTouchInput(const std::vector<float>& touch) {
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    void processInternalInput(const std::vector<float>& intern) {
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    void processProprioceptionInput(const std::vector<float>& proprio) {
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Motor decoding - extracted from decodeFromMotorNeurons
    MotorCommand decodeMotorCommand() {
        if (!brain_) return MotorCommand::Wait;
        
        MotorCommand decoded = decodeFromMotorNeurons();
        
        // Apply curiosity-based exploration
        if (curiosityEnabled_ && curiosityLevel_ > ConfigConstants::CURIOSITY_EXPLORATION_THRESHOLD) {
            decoded = selectWithCuriosity(decoded);
        }
        
        return decoded;
    }
    
    MotorCommand decodeFromMotorNeurons() {
        // Calculate average activity in each motor group
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            float sum = 0.0f;
            for (Neuron* n : neurons) {
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
        struct CommandInfo { MotorCommand cmd; float activity; };
        CommandInfo commands[] = {
            {MotorCommand::MoveForward, forwardAct},
            {MotorCommand::MoveBackward, backwardAct},
            {MotorCommand::TurnLeft, leftAct},
            {MotorCommand::TurnRight, rightAct},
            {MotorCommand::Interact, interactAct},
            {MotorCommand::Wait, waitAct}
        };
        
        MotorCommand best = MotorCommand::Wait;
        float bestActivity = waitAct;
        
        for (const auto& c : commands) {
            if (c.activity > bestActivity) {
                bestActivity = c.activity;
                best = c.cmd;
            }
        }
        
        // Only act if there's meaningful activity
        if (bestActivity < ConfigConstants::MOTOR_ACTIVITY_THRESHOLD) {
            return MotorCommand::Wait;
        }
        
        return best;
    }
    
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd) {
        // Exploration: occasionally choose random action when curiosity is high
        if (curiosityLevel_ > ConfigConstants::CURIOSITY_HIGH_EXPLORATION) {
            // Higher curiosity = more exploration
            float exploreChance = curiosityLevel_ * ConfigConstants::CURIOSITY_EXPLORATION_CHANCE;
            
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command from 0-7 range
                int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
                switch (choice) {
                    case 0: return MotorCommand::MoveForward;
                    case 1: return MotorCommand::MoveBackward;
                    case 2: return MotorCommand::TurnLeft;
                    case 3: return MotorCommand::TurnRight;
                    case 4: return MotorCommand::LookLeft;
                    case 5: return MotorCommand::LookRight;
                    case 6: return MotorCommand::Interact;
                    default: return MotorCommand::Wait;  // case 7
                }
            }
        }
        
        return defaultCmd;
    }
    
    void applyRewardModulation(float reward, float predictedReward) {
        if (!brain_ || !rewardModulationEnabled_) return;
        
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward_ = ConfigConstants::LEARNING_RATE_EXPECTED_REWARD * expectedReward_ + 
                         ConfigConstants::LEARNING_RATE_REWARD_CONTRIBUTION * reward;
        
        // Dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
        
        // Apply to all synapses with eligibility traces (copy to avoid race condition)
        applyRewardToSynapses(dopamineLevel_);
    }
    
    void applyRewardToSynapses(float dopamineLevel) {
        // Make a copy of synapses to avoid race condition during iteration
        std::vector<Synapse*> synapsesToUpdate;
        for (const auto& region : brain_->getRegions()) {
            auto regionSynapses = region->getSynapses();
            synapsesToUpdate.insert(synapsesToUpdate.end(), 
                                   regionSynapses.begin(), 
                                   regionSynapses.end());
        }
        
        for (Synapse* syn : synapsesToUpdate) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > ConfigConstants::ELIGIBILITY_THRESHOLD) {
                float delta = eligibility * dopamineLevel * plasticityModifier_;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(ConfigConstants::ELIGIBILITY_DECAY);
            }
        }
    }
    
    void updateDevelopment(double timestep) {
        if (!brain_ || !developmentEnabled_) return;
        
        developmentalAge_ += timestep;
        
        // Simple developmental stages based on age
        if (developmentalAge_ < ConfigConstants::DEVELOPMENT_INITIAL_AGE) {
            plasticityModifier_ = ConfigConstants::PLASTICITY_INITIAL;
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge_ < ConfigConstants::DEVELOPMENT_CRITICAL_PERIOD_AGE) {
            plasticityModifier_ = ConfigConstants::PLASTICITY_CRITICAL_PERIOD;
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge_ < ConfigConstants::DEVELOPMENT_MATURATION_AGE) {
            plasticityModifier_ = ConfigConstants::PLASTICITY_MATURATION;
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {
            plasticityModifier_ = ConfigConstants::PLASTICITY_ADULT;
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
        
        // Structural plasticity changes with development
        if (structuralPlasticityEnabled_) {
            auto* sp = brain_->getStructuralPlasticity();
            if (sp) {
                float synRate = ConfigConstants::SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
                float pruneRate = ConfigConstants::PRUNING_BASE_RATE * (2.0f - plasticityModifier_);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
    }
    
    void reset() {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0;
        plasticityModifier_ = ConfigConstants::PLASTICITY_INITIAL;
        
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
    
    // Getters
    DevelopmentalStage getDevelopmentalStage() const {
        if (!brain_) return DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
    }
    
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    
    size_t getSensoryInputSize() const {
        return 256 + 8 + 4 + 6;
    }
    
    size_t getMotorOutputSize() const {
        return 6;
    }
    
    // Initialize with world
    void initialize(const SimpleWorld& world) {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = ConfigConstants::PLASTICITY_INITIAL;
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    }

private:
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    float sensoryNoveltyDecay_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
};

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : pImpl(new Impl(brain)) {
    pImpl->initializeNeuronGroups();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    pImpl->initialize(world);
}

size_t AgentBrain::getSensoryInputSize() const {
    return pImpl->getSensoryInputSize();
}

size_t AgentBrain::getMotorOutputSize() const {
    return pImpl->getMotorOutputSize();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    pImpl->processSensoryInput(percept);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    return pImpl->decodeMotorCommand();
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    pImpl->applyRewardModulation(reward, predictedReward);
}

void AgentBrain::updateDevelopment(double timestep) {
    pImpl->updateDevelopment(timestep);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    return pImpl->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return pImpl->getNeuromodulationLevel();
}

float AgentBrain::getCuriosityLevel() const {
    return pImpl->getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    return pImpl->getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    return pImpl->getPredictionError();
}

void AgentBrain::reset() {
    pImpl->reset();
}

Brain* AgentBrain::getBrain() {
    return brain_.get();
}

void AgentBrain::enableRewardModulation(bool enable) {
    pImpl->rewardModulationEnabled_ = enable;
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    pImpl->structuralPlasticityEnabled_ = enable;
}

void AgentBrain::enableDevelopment(bool enable) {
    pImpl->developmentEnabled_ = enable;
}

void AgentBrain::enableCuriosity(bool enable) {
    pImpl->curiosityEnabled_ = enable;
}

bool AgentBrain::isRewardModulationEnabled() const {
    return pImpl->rewardModulationEnabled_;
}

bool AgentBrain::isStructuralPlasticityEnabled() const {
    return pImpl->structuralPlasticityEnabled_;
}

bool AgentBrain::isDevelopmentEnabled() const {
    return pImpl->developmentEnabled_;
}

bool AgentBrain::isCuriosityEnabled() const {
    return pImpl->curiosityEnabled_;
}

} // namespace nlm