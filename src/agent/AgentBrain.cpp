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
    // Validate input parameter - brain must not be null
    if (!brain) {
        NLM_LOG_ERROR("AgentBrain constructor: brain pointer cannot be null");
        return;
    }

    // Initialize motor and sensory neuron groups
    // Motor neurons are distributed across 6 action groups:
    // 0: MotorForward -> MoveForward command
    // 1: MotorBackward -> MoveBackward command  
    // 2: MotorTurnLeft -> TurnLeft command
    // 3: MotorTurnRight -> TurnRight command
    // 4: MotorInteract -> Interact command
    // 5: MotorWait -> Wait command (no action)
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Calculate total motor neurons already distributed to determine group assignment
                        size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() +
                                                  motorTurnLeft_.size() + motorTurnRight_.size() +
                                                  motorInteract_.size() + motorWait_.size();
                        
                        // Use modulo 6 to distribute evenly across the 6 motor action groups
                        switch (totalMotorNeurons % 6) {
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
                        // Calculate total sensory neurons already distributed to determine group assignment
                        size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        // Use modulo 4 to distribute evenly across the 4 sensory modalities
                        switch (totalSensoryNeurons % 4) {
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
    // Validate world dimensions before allocating memory
    int visionWidth = world.getVisionWidth();
    int visionHeight = world.getVisionHeight();
    
    if (visionWidth <= 0 || visionHeight <= 0) {
        NLM_LOG_ERROR("AgentBrain initialize: Invalid world dimensions " +
                     std::to_string(visionWidth) + "x" + std::to_string(visionHeight));
        return;
    }

    // Allocate vision buffer for novelty detection
    size_t visionBufferSize = static_cast<size_t>(visionWidth) * static_cast<size_t>(visionHeight);
    previousVision_.resize(visionBufferSize, 0.0f);
    
    // Reset developmental state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization statistics
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Document magic numbers for sensory input dimensions:
    // Vision: 16×16 grid = 256 values representing visual intensity
    // Touch: 8 values for collision/detection signals from environment
    // Internal: 4 values for homeostatic signals (energy, hunger, thirst, etc.)
    // Proprioception: 6 values for body position, velocity, and acceleration
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Document magic number for motor output dimensionality:
    // 6 motor commands representing discrete action choices:
    // 0: MoveForward - advance in direction
    // 1: MoveBackward - retreat in direction  
    // 2: TurnLeft - rotate counterclockwise
    // 3: TurnRight - rotate clockwise
    // 4: Interact - perform interaction action
    // 5: Wait - no movement (stay in place)
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Validate brain and percept before processing
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain processSensoryInput: brain pointer is null");
        return;
    }
    
    if (!percept.isValid()) {
        NLM_LOG_ERROR("AgentBrain processSensoryInput: invalid percept provided");
        return;
    }

    // Vision input processing (256 values -> 16x16 grid -> vision neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (!sensoryVision_[i]) {
            NLM_LOG_WARNING("AgentBrain: null vision neuron at index " + std::to_string(i));
            continue;
        }
        
        // Scale vision intensity: multiply by 5.0f to enhance signal for neural processing
        // Each vision neuron represents a 16×16 grid cell
        float current = vision[i] * 5.0f;
        sensoryVision_[i]->injectCurrent(current);
    }
    
    // Touch input processing (8 values for collision/detection -> touch neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (!sensoryTouch_[i]) {
            NLM_LOG_WARNING("AgentBrain: null touch neuron at index " + std::to_string(i));
            continue;
        }
        
        // Scale touch signal: multiply by 8.0f for collision signal detection
        // Each touch neuron responds to collision signals from environment
        float current = touch[i] * 8.0f;
        sensoryTouch_[i]->injectCurrent(current);
    }
    
    // Internal signals processing (4 values for homeostatic -> internal neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (!sensoryInternal_[i]) {
            NLM_LOG_WARNING("AgentBrain: null internal neuron at index " + std::to_string(i));
            continue;
        }
        
        // Center at 0.0f and scale: (value × 2 - 1) × 5
        // Maps [-0.5, 0.5] to [-5, 5] for neural processing
        float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
        sensoryInternal_[i]->injectCurrent(current);
    }
    
    // Proprioception processing (6 values for body state -> proprioception neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (!sensoryProprioception_[i]) {
            NLM_LOG_WARNING("AgentBrain: null proprioception neuron at index " + std::to_string(i));
            continue;
        }
        
        // Center at 0.0f and scale: (value × 2 - 1) × 3
        // Maps [-0.5, 0.5] to [-3, 3] for body position/velocity signals
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
        sensoryProprioception_[i]->injectCurrent(current);
    }
    
    // Novelty detection from visual change (drives curiosity and exploration)
    if (!vision.empty()) {
        // Validate vision buffer size before computation
        if (previousVision_.size() != vision.size()) {
            NLM_LOG_WARNING("AgentBrain: vision buffer size mismatch, reallocating");
            previousVision_.resize(vision.size(), 0.0f);
        }
        
        // Compute total absolute difference between current and previous vision
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize by number of vision pixels to get average change per pixel
        noveltyLevel_ = totalDiff / static_cast<float>(std::max<size_t>(vision.size(), 1));
        
        // Apply exponential decay to novelty signal (older changes matter less)
        // sensoryNoveltyDecay_ typically 0.99 means ~1% decay per timestep
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store current vision for next novelty comparison
        previousVision_ = vision;
    }
    
    // Update curiosity level based on novelty and prediction error
    if (curiosityEnabled_) {
        // Curiosity = (novelty × 2.0) + (|predictionError| × 0.5)
        // Novelty drives exploration of unknown environments
        // Prediction error drives exploration of unexpected outcomes
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        
        // Clamp curiosity to [0.0, 1.0] to maintain stable behavior
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    // Validate brain pointer before decoding
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain decodeMotorCommand: brain pointer is null");
        return MotorCommand::Wait;
    }
    
    // First decode from motor neuron activity using neural computation
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration if enabled and curiosity is high
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        // Curiosity drives exploration when above threshold
        // Higher curiosity increases chance of random action selection
        decoded = selectWithCuriosity(decoded);
    }
    
    // Validate decoded command is within valid range
    if (decoded < MotorCommand::MoveForward || decoded > MotorCommand::Wait) {
        NLM_LOG_WARNING("AgentBrain decodeMotorCommand: invalid decoded command, defaulting to Wait");
        return MotorCommand::Wait;
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using membrane potential deviation
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Validate neuron pointer before accessing state
            if (!n) {
                NLM_LOG_WARNING("AgentBrain: null neuron in calcActivity");
                continue;
            }
            
            // Use membrane potential deviation from resting potential as activity measure
            const auto& state = n->getState();
            sum += std::abs(state.membranePotential - state.restingPotential);
        }
        
        return sum / neurons.size();
    };
    
    // Compute activity for each of the 6 motor action groups
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Define motor command structure with associated activity levels
    struct MotorCommandWithActivity {
        MotorCommand cmd;
        float activity;
    };
    
    // Initialize array of motor commands with their computed activities
    MotorCommandWithActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find command with highest activity level
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if all activities are minimal
    
    for (const auto& cmd : commands) {
        if (cmd.activity > bestActivity) {
            bestActivity = cmd.activity;
            best = cmd.cmd;
        }
    }
    
    // Apply threshold filter to reject weak/noisy neural signals
    // Only execute commands with meaningful neural activity (> 0.5 threshold)
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Curiosity-driven exploration mechanism for stochastic action selection
    // Implements the exploration-exploitation tradeoff based on agent's curiosity level
    
    // Only explore when curiosity is high enough to warrant random actions
    if (curiosityLevel_ > 0.5f) {
        // Calculate exploration probability: curiosity level scales chance (0.0 to 0.3)
        // Higher curiosity = higher chance of random exploration
        float explorationProbability = curiosityLevel_ * 0.3f;
        
        // Generate random number to determine if exploration should occur
        float randomValue = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        
        // Perform random exploration if random value is below threshold
        if (randomValue < explorationProbability) {
            // Select a completely random motor command for exploration
            int randomChoice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (randomChoice) {
                case 0: return MotorCommand::MoveForward;    // Move in current direction
                case 1: return MotorCommand::MoveBackward;   // Move opposite direction
                case 2: return MotorCommand::TurnLeft;       // Rotate left
                case 3: return MotorCommand::TurnRight;      // Rotate right
                case 4: return MotorCommand::LookLeft;       // Pan sensor left
                case 5: return MotorCommand::LookRight;      // Pan sensor right
                case 6: return MotorCommand::Interact;       // Interact with nearby object
                case 7: return MotorCommand::Wait;           // No action (fallback)
                default: return MotorCommand::Wait;         // Safety fallback
            }
        }
    }
    
    // Return original command when no exploration is performed
    return defaultCmd;
}
        }
    }
    
    // Return original command when no exploration is performed
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // Validate inputs and enabled state before processing
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_ERROR("AgentBrain applyRewardModulation: brain pointer is null");
        }
        return;
    }
    
    // Compute prediction error signal (delta = reward - expected)
    // This represents the difference between actual and expected outcomes
    predictionError_ = reward - predictedReward;
    
    // Update expected reward using exponential moving average
    // Gives more weight to historical rewards (0.95) and less to current (0.05)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like neuromodulatory signal based on prediction error
    // Positive error = reward received (excitatory signal)
    // Negative error = punishment received (inhibitory signal)
    dopamineLevel_ = predictionError_;
    
    // Clamp dopamine level to biologically plausible range [-1.0, 1.0]
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward-modulated plasticity to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Check if synapse has meaningful eligibility trace
            if (std::abs(syn->getEligibilityTrace()) > 0.001f) {
                // Compute weight change based on reward prediction error
                // delta = eligibility × dopamine × plasticity_modifier
                float weightChange = syn->getEligibilityTrace() * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(weightChange);
                
                // Decay eligibility trace to prevent unbounded accumulation
                // Decay factor of 0.1 reduces trace by 10% per reward event
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate overall plasticity based on dopamine level
    // Positive dopamine increases synaptic plasticity, negative decreases it
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Update STDP plasticity parameters based on dopamine-modulated factor
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Long-Term Potentiation (LTP) weight change scaled by plasticity factor
        stdp->setLTPWeight(0.01f * plasticityFactor);
        
        // Long-Term Depression (LTD) weight change scaled by plasticity factor
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    // Validate brain and development settings before processing
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_ERROR("AgentBrain updateDevelopment: brain pointer is null");
        }
        return;
    }
    
    // Advance developmental clock with current timestep
    developmentalAge_ += timestep;
    
    // Update developmental stage based on age (biologically inspired stages)
    // This defines how plasticity and behavior change throughout agent's life
    
    if (developmentalAge_ < 60.0) {  // Initial learning phase (~1 minute)
        plasticityModifier_ = 1.0f;  // Maximum plasticity for rapid learning
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // Critical learning period (~5 minutes)
        plasticityModifier_ = 0.8f;   // High but decreasing plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // Maturation phase (~15 minutes)
        plasticityModifier_ = 0.5f;   // Moderate plasticity for refinement
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {  // Adult stable phase (> 15 minutes)
        plasticityModifier_ = 0.2f;   // Low plasticity for stability
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Update structural plasticity based on developmental stage
    // Controls synaptic formation and pruning throughout development
    if (structuralPlasticityEnabled_) {
        auto* structuralPlasticity = brain_->getStructuralPlasticity();
        if (structuralPlasticity) {
            // Synaptogenesis rate: higher in early development with more plasticity
            // Scale factor of 0.0001f per plasticity unit
            float synaptogenesisRate = 0.0001f * plasticityModifier_;
            
            // Pruning rate: inversely related to plasticity (less pruning when more plastic)
            // Scale factor of 0.00001f per (2 - plasticity) unit
            float pruningRate = 0.00001f * (2.0f - plasticityModifier_);
            
            // Apply developmental plasticity rates to structural plasticity system
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
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
