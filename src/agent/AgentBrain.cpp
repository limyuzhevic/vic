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
        distributeNeurons();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::distributeNeurons() {
    // Track the next index for each group to avoid redundant size calculations
    // This eliminates O(N^2) complexity from repeatedly calculating group sizes
    // The pre-incremented approach ensures constant-time index updates
    size_t motorIndex = 0;
    size_t sensoryIndex = 0;
    
    // Iterate through all brain regions and populations
    // Regions organize neurons by spatial/functional areas
    for (const auto& region : brain_->getRegions()) {
        // Each region contains populations of neurons with similar properties
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                // Distribute motor neurons using pre-incremented index
                // Motor neurons are mapped to 6 action groups for movement control
                distributeMotorNeurons(pop->getNeurons(), motorIndex);
            } else if (type == NeuronType::Sensory) {
                // Distribute sensory neurons using pre-incremented index
                // Sensory neurons are mapped to 4 modalities for perception
                distributeSensoryNeurons(pop->getNeurons(), sensoryIndex);
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons, size_t& index) {
    // Distribute motor neurons using pre-incremented index
    // The modulo 6 provides balanced distribution across 6 action groups:
    // 0: MoveForward (positive X direction)
    // 1: MoveBackward (negative X direction)
    // 2: TurnLeft (negative rotation)
    // 3: TurnRight (positive rotation)
    // 4: Interact (object manipulation)
    // 5: Wait (no movement)
    for (Neuron* n : neurons) {
        switch (index % 6) {
            case 0: motorForward_.push_back(n); break;
            case 1: motorBackward_.push_back(n); break;
            case 2: motorTurnLeft_.push_back(n); break;
            case 3: motorTurnRight_.push_back(n); break;
            case 4: motorInteract_.push_back(n); break;
            case 5: motorWait_.push_back(n); break;
        }
        ++index; // Increment once per neuron
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons, size_t& index) {
    // Distribute sensory neurons using pre-incremented index
    // The modulo 4 provides balanced distribution across 4 sensory modalities:
    // 0: Vision (external visual information)
    // 1: Touch (external tactile information)
    // 2: Internal (internal body state)
    // 3: Proprioception (body position and movement)
    for (Neuron* n : neurons) {
        switch (index % 4) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
        }
        ++index; // Increment once per neuron
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Store vision buffer dimensions for novelty detection
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    // Initialize developmental clock
    developmentalAge_ = 0.0;
    
    // Set baseline plasticity to maximum (100%)
    plasticityModifier_ = 1.0f;
    
    // Log initialization summary
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Calculate total sensory input based on world perception requirements:
    // Vision: 16x16 grid = 256 values (detailed spatial information)
    // Touch: 8 discrete contact points (binary/sensor-based)
    // Internal: 4 values (body state, hunger, thirst, fatigue)
    // Proprioception: 6 values (joint angles, muscle tension, balance)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action command:
    // 0: MoveForward, 1: MoveBackward, 2: TurnLeft, 3: TurnRight,
    // 4: Interact, 5: Wait (default/idle)
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Extract sensory modalities from current perception
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Vision processing (256 values mapped to visual sensory neurons)
    // Each sensory neuron receives proportional input based on intensity
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Current injection scales with light intensity (0-1 range)
            float current = vision[i] * 5.0f;  // Scale factor to amplify weak signals
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch processing (8 discrete contact points)
    // Strongest collisions generate higher neural activation
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal amplification
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal processing (4 body state variables)
    // Variables centered around 0.5: <0.5 indicates deficit, >0.5 indicates surplus
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Center at 0.5 and scale to range [-2.5, 2.5] for neural processing
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception processing (6 body position/movement variables)
    // Similar to internal but scaled down for finer motor control
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Smaller scale factor
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Novelty detection based on visual change from previous perception
    // Measures how much the visual scene has changed since last timestep
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize difference by number of vision pixels
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Apply decay to novelty signal (prevents novelty from growing without bound)
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store current vision for next novelty comparison
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
    // Curiosity = (novelty × 2) + (prediction_error × 0.5), clamped to [0, 1]
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration to promote learning and adaptability
    // When curiosity is high (>0.3), the agent may explore random actions
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    // Activity is measured as the deviation from resting potential (stronger = more active)
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
    
    // Create activity table for all motor commands
    // We need to find the command with highest neural activity
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
    
    // Find command with maximum neural activity
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only execute if there's meaningful neural activity
    // This prevents noise or weak activation from causing random movements
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration strategy: occasionally choose random actions when curiosity is high
    // This implements curiosity-driven exploration in reinforcement learning
    if (curiosityLevel_ > 0.5f) {
        // Calculate probability of exploration based on curiosity level
        // Higher curiosity = more exploration (up to 30% chance)
        float exploreChance = curiosityLevel_ * 0.3f;
        
        // Generate random number to decide whether to explore
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command for exploration
            // Includes additional commands: LookLeft, LookRight
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;  // Additional exploratory action
                case 5: return MotorCommand::LookRight; // Additional exploratory action
                case 6: return MotorCommand::Interact; // Object interaction
                default: return MotorCommand::Wait;    // Fallback
            }
        }
    }
    
    return defaultCmd;  // Stick to the best-decoded command
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
