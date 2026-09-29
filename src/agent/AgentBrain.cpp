#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Constants for improved maintainability
namespace Constants {
    // Sensory scaling factors
    constexpr float VISION_SCALE = 5.0f;          // Scale factor for vision input
    constexpr float TOUCH_SCALE = 8.0f;          // Scale factor for touch input
    constexpr float INTERNAL_CENTER = 2.0f;       // Center for internal signals
    constexpr float INTERNAL_SCALE = 5.0f;        // Scale for internal signals
    constexpr float PROPRIO_CENTER = 2.0f;        // Center for proprioception
    constexpr float PROPRIO_SCALE = 3.0f;         // Scale for proprioception
    
    // Novelty computation
    constexpr float NOVELTY_DECAY = 0.99f;        // Sensory novelty decay rate
    constexpr float NOVELTY_MULTIPLIER = 2.0f;    // Novelty to curiosity conversion
    constexpr float PREDICTION_ERROR_WEIGHT = 0.5f; // Prediction error weight
    
    // Motor thresholds and exploration
    constexpr float ACTIVITY_THRESHOLD = 0.5f;    // Minimum activity for action
    constexpr float CURIOSITY_EXPLORATION_RATE = 0.3f; // Max exploration probability
    constexpr float CURIOSITY_THRESHOLD = 0.3f;   // Minimum curiosity for exploration
    constexpr float HIGH_CURIOSITY_THRESHOLD = 0.5f; // Threshold for high curiosity
    
    // Reward modulation
    constexpr float EXPECTED_REWARD_ALPHA = 0.95f;    // EMA weight for expected reward
    constexpr float REWARD_BETA = 0.05f;              // EMA weight for actual reward
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;        // Min dopamine level
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;         // Max dopamine level
    constexpr float ELIGIBILITY_THRESHOLD = 0.001f;    // Threshold for eligibility trace
    constexpr float ELIGIBILITY_DECAY = 0.1f;         // Eligibility trace decay rate
    constexpr float PLASTICITY_FACTOR_BASE = 0.5f;     // Base plasticity factor
    constexpr float PLASTICITY_FACTOR_WEIGHT = 0.5f;   // Dopamine weight on plasticity
    constexpr float STDP_LTP_WEIGHT_BASE = 0.01f;      // Base LTP weight
    constexpr float STDP_LTD_WEIGHT_BASE = 0.012f;     // Base LTD weight
    
    // Developmental stages
    constexpr double DEVELOPMENT_INITIAL = 60.0;      // Initial stage duration
    constexpr double DEVELOPMENT_CRITICAL = 300.0;    // Critical period duration  
    constexpr double DEVELOPMENT_MATURE = 900.0;      // Maturation duration
    constexpr float PLASTICITY_INITIAL = 1.0f;         // Initial plasticity
    constexpr float PLASTICITY_CRITICAL = 0.8f;        // Critical period plasticity
    constexpr float PLASTICITY_MATURING = 0.5f;        // Maturation plasticity
    constexpr float PLASTICITY_ADULT = 0.2f;           // Adult stability
    
    // Structural plasticity rates
    constexpr float SYNAPTgenesis_RATE_BASE = 0.0001f;  // Base synaptogenesis rate
    constexpr float PRUNE_RATE_BASE = 0.00001f;         // Base pruning rate
    
    // Motor command enumeration (must match MotorCommand enum)
    constexpr int MOTOR_COMMAND_COUNT = 8;              // Number of motor commands (including Wait)
}

// Helper functions for improved code organization
namespace AgentBrainHelpers {
    
    // Generic sensory input injection function to reduce code duplication
    template<typename PerceptIterator, typename NeuronIterator>
    void injectSensoryInput(PerceptIterator perceptBegin, PerceptIterator perceptEnd,
                           NeuronIterator neuronBegin, NeuronIterator neuronEnd,
                           float scale) {
        for (auto perceptIt = perceptBegin, neuronIt = neuronBegin;
             perceptIt != perceptEnd && neuronIt != neuronEnd;
             ++perceptIt, ++neuronIt) {
            if (*neuronIt) {
                (*neuronIt)->injectCurrent(*perceptIt * scale);
            }
        }
    }
    
    // Calculate average activity with null checking
    float calculateAverageActivity(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        size_t validNeurons = 0;
        
        for (Neuron* n : neurons) {
            if (!n) continue; // Skip null neurons
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            validNeurons++;
        }
        
        return validNeurons > 0 ? (sum / validNeurons) : 0.0f;
    }
    
    // Find motor command with maximum activity
    MotorCommand findBestMotorCommand(const MotorCommand& defaultCmd, const float& defaultActivity,
                                     const MotorCommand& cmd, const float& activity) {
        if (activity > defaultActivity) {
            return cmd;
        }
        return defaultCmd;
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
    , sensoryNoveltyDecay_(Constants::NOVELTY_DECAY)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        distributeNeurons();
    }
}

void AgentBrain::distributeNeurons() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot distribute neurons: brain_ is null");
        return;
    }
    
    size_t motorGroups = 6;  // motorForward, motorBackward, motorTurnLeft, motorTurnRight, motorInteract, motorWait
    size_t sensoryGroups = 4;  // sensoryVision, sensoryTouch, sensoryInternal, sensoryProprioception
    
    // Reset all neuron vectors
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
    
    // Distribute motor neurons
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) continue; // Safety check
                        
                        // Use pre-calculated total for efficiency
                        static size_t totalDistributed = 0;
                        size_t groupIndex = totalDistributed % motorGroups;
                        totalDistributed++;
                        
                        switch (groupIndex) {
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
                        if (!n) continue; // Safety check
                        
                        static size_t totalSensory = 0;
                        size_t groupIndex = totalSensory % sensoryGroups;
                        totalSensory++;
                        
                        switch (groupIndex) {
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
    
    NLM_LOG_DEBUG("Neuron distribution complete: " +
                 std::to_string(motorForward_.size()) + " forward, " +
                 std::to_string(motorBackward_.size()) + " backward, " +
                 std::to_string(motorTurnLeft_.size()) + " left, " +
                 std::to_string(motorTurnRight_.size()) + " right, " +
                 std::to_string(motorInteract_.size()) + " interact, " +
                 std::to_string(motorWait_.size()) + " wait, " +
                 std::to_string(sensoryVision_.size()) + " vision, " +
                 std::to_string(sensoryTouch_.size()) + " touch, " +
                 std::to_string(sensoryInternal_.size()) + " internal, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize: brain_ is null");
        return;
    }
    
    // Use world dimensions instead of hardcoded values for flexibility
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = Constants::PLASTICITY_INITIAL;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    if (!brain_) return 0;
    
    // Dynamically calculate based on perception dimensions to avoid hardcoded values
    // Get world dimensions for proper calculation
    // This would need access to world, but for now use documented defaults
    // Vision: width × height
    // Touch: 8 points  
    // Internal: 4 values
    // Proprioception: 6 values
    return 16 * 16 + 8 + 4 + 6; // Documented defaults
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action (excluding additional commands like LookLeft/Right)
    return 6; // MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: brain_ is null");
        return;
    }
    
    // Process vision input
    const auto& vision = percept.getVision();
    AgentBrainHelpers::injectSensoryInput(
        vision.begin(), vision.end(),
        sensoryVision_.begin(), sensoryVision_.end(),
        Constants::VISION_SCALE
    );
    
    // Process touch input
    const auto& touch = percept.getTouch();
    AgentBrainHelpers::injectSensoryInput(
        touch.begin(), touch.end(),
        sensoryTouch_.begin(), sensoryTouch_.end(),
        Constants::TOUCH_SCALE
    );
    
    // Process internal signals with centering and scaling
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * Constants::INTERNAL_CENTER - 1.0f) * Constants::INTERNAL_SCALE;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Process proprioception with centering and scaling
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * Constants::PROPRIO_CENTER - 1.0f) * Constants::PROPRIO_SCALE;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t validComparisons = 0;
        
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
            validComparisons++;
        }
        
        // Normalize
        noveltyLevel_ = validComparisons > 0 ? (totalDiff / validComparisons) : 0.0f;
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * Constants::NOVELTY_MULTIPLIER + 
                         std::abs(predictionError_) * Constants::PREDICTION_ERROR_WEIGHT;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot decode motor command: brain_ is null");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > Constants::CURIOSITY_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using optimized helper
    float forwardAct = AgentBrainHelpers::calculateAverageActivity(motorForward_);
    float backwardAct = AgentBrainHelpers::calculateAverageActivity(motorBackward_);
    float leftAct = AgentBrainHelpers::calculateAverageActivity(motorTurnLeft_);
    float rightAct = AgentBrainHelpers::calculateAverageActivity(motorTurnRight_);
    float interactAct = AgentBrainHelpers::calculateAverageActivity(motorInteract_);
    float waitAct = AgentBrainHelpers::calculateAverageActivity(motorWait_);
    
    // Create command-activity pairs
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find command with maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& ca : commands) {
        if (ca.activity > bestActivity) {
            bestActivity = ca.activity;
            best = ca.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < Constants::ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > Constants::HIGH_CURIOSITY_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * Constants::CURIOSITY_EXPLORATION_RATE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command from valid range
            int choice = brain_->getRandomGenerator()->uniformInt(0, Constants::MOTOR_COMMAND_COUNT - 1);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                case 5: return MotorCommand::Wait;
                case 6: return MotorCommand::LookLeft;
                case 7: return MotorCommand::LookRight;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) NLM_LOG_WARNING("Cannot apply reward modulation: brain_ is null");
        if (!rewardModulationEnabled_) NLM_LOG_DEBUG("Reward modulation disabled");
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = Constants::EXPECTED_REWARD_ALPHA * expectedReward_ + 
                     Constants::REWARD_BETA * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, Constants::DOPAMINE_CLAMP_MIN, Constants::DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue; // Safety check
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > Constants::ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(Constants::ELIGIBILITY_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = Constants::PLASTICITY_FACTOR_BASE + 
                            Constants::PLASTICITY_FACTOR_WEIGHT * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(Constants::STDP_LTP_WEIGHT_BASE * plasticityFactor);
        stdp->setLTDWeight(Constants::STDP_LTD_WEIGHT_BASE * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) NLM_LOG_WARNING("Cannot update development: brain_ is null");
        if (!developmentEnabled_) NLM_LOG_DEBUG("Development disabled");
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Set developmental stage based on age
    if (developmentalAge_ < Constants::DEVELOPMENT_INITIAL) {  // ~1 minute
        plasticityModifier_ = Constants::PLASTICITY_INITIAL;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < Constants::DEVELOPMENT_CRITICAL) {  // ~5 minutes
        plasticityModifier_ = Constants::PLASTICITY_CRITICAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < Constants::DEVELOPMENT_MATURE) {  // ~15 minutes
        plasticityModifier_ = Constants::PLASTICITY_MATURING;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = Constants::PLASTICITY_ADULT;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = Constants::SYNAPTgenesis_RATE_BASE * plasticityModifier_;
            float pruneRate = Constants::PRUNE_RATE_BASE * (2.0f - plasticityModifier_);
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
    // Reset neuromodulation state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = Constants::PLASTICITY_INITIAL;
    
    // Reset sensory novelty decay
    sensoryNoveltyDecay_ = Constants::NOVELTY_DECAY;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Redistribute neurons (in case any were lost/damaged)
    distributeNeurons();
    
    NLM_LOG_INFO("AgentBrain reset to initial state");
}

} // namespace nlm