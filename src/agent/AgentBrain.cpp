#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Constants for sensor transduction scaling factors
namespace {
    constexpr float VISION_SCALE_FACTOR = 5.0f;
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    constexpr float INTERNAL_CENTER_FACTOR = 2.0f;
    constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    constexpr float PROPRIOCEPTIVE_CENTER_FACTOR = 2.0f;
    constexpr float PROPRIOCEPTIVE_SCALE_FACTOR = 3.0f;
    constexpr float NOVELTY_DECAY_FACTOR = 0.99f;
    constexpr float MIN_VISION_SIZE = 1;
    constexpr float MIN_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
    constexpr float EXPLORATION_THRESHOLD = 0.5f;
    constexpr float CURIOUITY_WEIGHT_NOVELTY = 2.0f;
    constexpr float CURIOUITY_WEIGHT_PREDICTION_ERROR = 0.5f;
    constexpr float MAX_NOVELTY_LEVEL = 1.0f;
    constexpr float MIN_NOVELTY_LEVEL = 0.0f;
    constexpr float MAX_DOPAMINE_LEVEL = 1.0f;
    constexpr float MIN_DOPAMINE_LEVEL = -1.0f;
    constexpr float MAX_PLASTICITY_FACTOR = 2.0f;
    constexpr float MIN_PLASTICITY_FACTOR = 0.1f;
    constexpr float DEFAULT_PLASTICITY = 1.0f;
    constexpr float DEFAULT_DEVELOPMENTAL_AGE = 0.0;
    constexpr float INITIAL_PLASTICITY_MODIFIER = 1.0f;
    constexpr float CRITICAL_PERIOD_START = 60.0;
    constexpr float CRITICAL_PERIOD_END = 300.0;
    constexpr float MATURATION_END = 900.0;
    constexpr float ADULT_START = 900.0;
    constexpr float HIGH_PLASTICITY = 1.0f;
    constexpr float MODERATE_PLASTICITY = 0.8f;
    constexpr float LOW_PLASTICITY = 0.5f;
    constexpr float ADULT_PLASTICITY = 0.2f;
    constexpr float EARLY_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float LATE_SYNAPTOGENESIS_RATE = 0.00001f;
    constexpr float HIGH_SYNAPTOGENESIS_MULTIPLIER = 2.0f;
    constexpr float LOW_SYNAPTOGENESIS_MULTIPLIER = 0.5f;
    constexpr float PREDICTION_ERROR_DECAY_RATE = 0.95f;
    constexpr float REWARD_UPDATE_WEIGHT = 0.05f;
    constexpr float ELIGIBILITY_DECAY_RATE = 0.1f;
    constexpr float MIN_ELIGIBILITY_MAGNITUDE = 0.001f;
    constexpr float PLURALITY_FACTOR = 0.5f;
    constexpr float SYNAPTIC_WEIGHT_FACTOR = 0.01f;
    constexpr float LTD_WEIGHT_FACTOR = 0.012f;
    constexpr float STRUCTURED_PLASTICITY_SYNAPTIZATION_RATE = 0.0001f;
    constexpr float STRUCTURED_PLASTICITY_PRUNING_RATE = 0.00001f;
}

// Forward declarations for helper classes
class MotorCommandSelector;
class DevelopmentManager;

// MotorCommandSelector class implementation
class MotorCommandSelector {
public:
    MotorCommand decodeFromMotorNeurons(
        const std::vector<Neuron*>& motorForward,
        const std::vector<Neuron*>& motorBackward,
        const std::vector<Neuron*>& motorTurnLeft,
        const std::vector<Neuron*>& motorTurnRight,
        const std::vector<Neuron*>& motorInteract,
        const std::vector<Neuron*>& motorWait
    ) {
        // Calculate average activity in each motor group
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            float sum = 0.0f;
            for (Neuron* n : neurons) {
                if (!n) continue;
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
            return sum / neurons.size();
        };

        float forwardAct = calcActivity(motorForward);
        float backwardAct = calcActivity(motorBackward);
        float leftAct = calcActivity(motorTurnLeft);
        float rightAct = calcActivity(motorTurnRight);
        float interactAct = calcActivity(motorInteract);
        float waitAct = calcActivity(motorWait);

        // Find maximum activity
        struct CommandWithActivity {
            MotorCommand cmd;
            float activity;
        };
        
        CommandWithActivity commands[] = {
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
        if (bestActivity < MIN_ACTIVITY_THRESHOLD) {
            return MotorCommand::Wait;
        }

        return best;
    }

    MotorCommand selectWithCuriosity(
        MotorCommand defaultCmd,
        float curiosityLevel,
        std::shared_ptr<Brain> brain
    ) {
        // Exploration: occasionally choose random action when curiosity is high
        if (curiosityLevel > EXPLORATION_THRESHOLD) {
            // Higher curiosity = more exploration
            float exploreChance = curiosityLevel * MAX_EXPLORATION_CHANCE;  // Up to 30% random

            float r = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command
                int choice = brain->getRandomGenerator()->uniformInt(0, 7);
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
};

// DevelopmentManager class implementation
class DevelopmentManager {
public:
    struct DevelopmentalParameters {
        double ageThreshold;
        float plasticityModifier;
        DevelopmentalStage stage;
    };

    void updateDevelopmentStage(
        double& developmentalAge,
        float& plasticityModifier,
        std::shared_ptr<Brain> brain,
        bool structuralPlasticityEnabled
    ) {
        developmentalAge += timestep;

        // Define developmental stages with their parameters
        DevelopmentalParameters stages[] = {
            {CRITICAL_PERIOD_START, HIGH_PLASTICITY, DevelopmentalStage::Initial},
            {CRITICAL_PERIOD_END, MODERATE_PLASTICITY, DevelopmentalStage::CriticalPeriod},
            {MATURATION_END, LOW_PLASTICITY, DevelopmentalStage::Maturation},
            {ADULT_START, ADULT_PLASTICITY, DevelopmentalStage::Adult}
        };

        // Find current stage
        DevelopmentalStage currentStage = DevelopmentalStage::Initial;
        float currentPlasticity = INITIAL_PLASTICITY_MODIFIER;

        for (const auto& stage : stages) {
            if (developmentalAge < stage.ageThreshold) {
                currentStage = stage.stage;
                currentPlasticity = stage.plasticityModifier;
                break;
            }
        }

        // Update state
        plasticityModifier = currentPlasticity;
        if (brain) {
            brain->setDevelopmentalStage(currentStage);
        }

        // Handle structural plasticity if enabled
        if (structuralPlasticityEnabled && brain) {
            auto* sp = brain->getStructuralPlasticity();
            if (sp) {
                updateStructuralPlasticityRates(sp, currentPlasticity);
            }
        }
    }

private:
    void updateStructuralPlasticityRates(
        StructuralPlasticity* sp,
        float plasticityModifier
    ) {
        if (!sp) return;

        // Higher synaptogenesis in early development
        float synRate = STRUCTURED_PLASTICITY_SYNAPTIZATION_RATE * plasticityModifier;
        float pruneRate = STRUCTURED_PLASTICITY_PRUNING_RATE * (HIGH_SYNAPTOGENESIS_MULTIPLIER - plasticityModifier);

        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }

    double timestep = 0.0;
};

// Helper functions for neuromodulation
namespace {
    float calculatePredictionError(float reward, float predictedReward) {
        return reward - predictedReward;
    }

    float updateExpectedReward(float currentExpected, float reward) {
        return PREDICTION_ERROR_DECAY_RATE * currentExpected + REWARD_UPDATE_WEIGHT * reward;
    }

    float calculateDopamineLevel(float predictionError) {
        return predictionError;
    }

    float clampDopamineLevel(float dopamine) {
        return std::clamp(dopamine, MIN_DOPAMINE_LEVEL, MAX_DOPAMINE_LEVEL);
    }

    float calculatePlasticityFactor(float dopamineLevel) {
        float plasticityFactor = PLURALITY_FACTOR + PLURALITY_FACTOR * dopamineLevel;
        return std::clamp(plasticityFactor, MIN_PLASTICITY_FACTOR, MAX_PLASTICITY_FACTOR);
    }

    float calculateSynapticWeightChange(
        float eligibilityTrace,
        float dopamineLevel,
        float plasticityModifier
    ) {
        return eligibilityTrace * dopamineLevel * plasticityModifier;
    }

    void updateEligibilityTrace(float& eligibilityTrace, float decayRate) {
        eligibilityTrace *= decayRate;
    }

    bool isEligibilityTraceSignificant(float eligibility) {
        return std::abs(eligibility) > MIN_ELIGIBILITY_MAGNITUDE;
    }

    void applyRewardToSynapses(
        std::shared_ptr<Brain> brain,
        float dopamineLevel,
        float plasticityModifier
    ) {
        if (!brain) return;

        for (const auto& region : brain->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (!syn) continue;

                float eligibility = syn->getEligibilityTrace();

                if (isEligibilityTraceSignificant(eligibility)) {
                    // Apply reward-modulated weight change
                    float delta = calculateSynapticWeightChange(eligibility, dopamineLevel, plasticityModifier);
                    syn->addToWeight(delta);

                    // Decay eligibility trace
                    updateEligibilityTrace(syn->getEligibilityTrace(), ELIGIBILITY_DECAY_RATE);
                }
            }
        }
    }

    void applySTDPToBrain(
        Brain* brain,
        float plasticityFactor
    ) {
        if (!brain) return;

        auto* stdp = brain->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(SYNAPTIC_WEIGHT_FACTOR * plasticityFactor);
            stdp->setLTDWeight(LTD_WEIGHT_FACTOR * plasticityFactor);
        }
    }

    float calculateVisionNovelty(
        const std::vector<float>& vision,
        const std::vector<float>& previousVision
    ) {
        if (vision.empty()) return MIN_NOVELTY_LEVEL;

        float totalDiff = 0.0f;
        size_t compareSize = std::min(vision.size(), previousVision.size());

        for (size_t i = 0; i < compareSize; ++i) {
            float diff = std::abs(vision[i] - previousVision[i]);
            totalDiff += diff;
        }

        // Normalize
        return totalDiff / static_cast<float>(std::max<size_t>(vision.size(), MIN_VISION_SIZE));
    }

    float calculateCuriosityLevel(float noveltyLevel, float predictionError) {
        float curiosity = noveltyLevel * CURIOUITY_WEIGHT_NOVELTY + std::abs(predictionError) * CURIOUITY_WEIGHT_PREDICTION_ERROR;
        return std::clamp(curiosity, MIN_NOVELTY_LEVEL, MAX_NOVELTY_LEVEL);
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
    // Initialize motor and sensory neuron groups
    if (brain_) {
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
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (world.getVisionWidth() > 0 && world.getVisionHeight() > 0) {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    }
    
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

void AgentBrain::injectVisionSensoryCurrent(const std::vector<float>& vision) {
    if (vision.empty()) return;
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * VISION_SCALE_FACTOR;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::injectTouchSensoryCurrent(const std::vector<float>& touch) {
    if (touch.empty()) return;
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * TOUCH_SCALE_FACTOR;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::injectInternalSensoryCurrent(const std::vector<float>& internal) {
    if (internal.empty()) return;
    
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (internal[i] * INTERNAL_CENTER_FACTOR - 1.0f) * INTERNAL_SCALE_FACTOR;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::injectProprioceptionSensoryCurrent(const std::vector<float>& proprioception) {
    if (proprioception.empty()) return;
    
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprioception.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprioception[i] * PROPRIOCEPTIVE_CENTER_FACTOR - 1.0f) * PROPRIOCEPTIVE_SCALE_FACTOR;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::updateNoveltyLevel(const std::vector<float>& vision) {
    if (vision.empty()) return;
    
    // Calculate novelty (difference from previous vision)
    float totalDiff = 0.0f;
    size_t compareSize = std::min(vision.size(), previousVision_.size());
    
    for (size_t i = 0; i < compareSize; ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize - fix division by zero
    noveltyLevel_ = totalDiff / static_cast<float>(std::max<size_t>(vision.size(), MIN_VISION_SIZE));
    
    // Decay and update
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time
    previousVision_ = vision;
}

void AgentBrain::updateCuriosityLevel() {
    if (!curiosityEnabled_) return;
    
    curiosityLevel_ = noveltyLevel_ * CURIOUITY_WEIGHT_NOVELTY + std::abs(predictionError_) * CURIOUITY_WEIGHT_PREDICTION_ERROR;
    curiosityLevel_ = std::clamp(curiosityLevel_, MIN_NOVELTY_LEVEL, MAX_NOVELTY_LEVEL);
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Process vision input
    const auto& vision = percept.getVision();
    injectVisionSensoryCurrent(vision);
    
    // Process touch input
    const auto& touch = percept.getTouch();
    injectTouchSensoryCurrent(touch);
    
    // Process internal signals
    const auto& intern = percept.getInternal();
    injectInternalSensoryCurrent(intern);
    
    // Process proprioception
    const auto& proprio = percept.getProprioception();
    injectProprioceptionSensoryCurrent(proprio);
    
    // Update novelty based on current vision
    updateNoveltyLevel(vision);
    
    // Update curiosity level
    updateCuriosityLevel();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > EXPLORATION_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    static MotorCommandSelector selector;
    return selector.decodeFromMotorNeurons(
        motorForward_, motorBackward_, motorTurnLeft_, motorTurnRight_, motorInteract_, motorWait_
    );
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    static MotorCommandSelector selector;
    return selector.selectWithCuriosity(defaultCmd, curiosityLevel_, brain_);
}

void AgentBrain::updatePredictionError(float reward, float predictedReward) {
    predictionError_ = calculatePredictionError(reward, predictedReward);
}

void AgentBrain::updateExpectedReward(float reward) {
    expectedReward_ = updateExpectedReward(expectedReward_, reward);
}

void AgentBrain::updateDopamineLevel() {
    dopamineLevel_ = calculateDopamineLevel(predictionError_);
    dopamineLevel_ = clampDopamineLevel(dopamineLevel_);
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    updatePredictionError(reward, predictedReward);
    
    // Update expected reward (exponential moving average)
    updateExpectedReward(reward);
    
    // Update dopamine-like signal (based on prediction error)
    updateDopamineLevel();
    
    // Apply to all synapses with eligibility traces
    applyRewardToSynapses(brain_, dopamineLevel_, plasticityModifier_);
    
    // Modulate plasticity based on dopamine
    float plasticityFactor = calculatePlasticityFactor(dopamineLevel_);
    
    // Apply to STDP
    applySTDPToBrain(brain_.get(), plasticityFactor);
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    static DevelopmentManager devManager;
    devManager.updateDevelopmentStage(
        developmentalAge_, plasticityModifier_, brain_, structuralPlasticityEnabled_
    );
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
