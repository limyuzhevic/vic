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
    // Initialize motor neuron groups from brain regions
    // Use population ID to determine action based on neural organization
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                // Motor neurons are organized by their population/cell type
                // Use population ID to determine action, not arbitrary modulo
                PopulationId popId = pop->getId();
                
                // Create logical groups based on population ID for better organization
                // This preserves the neural organization from the brain
                for (Neuron* n : pop->getNeurons()) {
                    // Determine action based on population ID, not hardcoded groups
                    uint64_t popIndex = popId.index();
                    
                    // Map population ID to action based on pattern:
                    // population 0: MoveForward, 1: MoveBackward, 2: TurnLeft, 3: TurnRight, 4: Interact, 5+: Wait
                    if (popIndex < 6) {
                        // Action mapping based on population ID
                        switch (popIndex) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    } else {
                        // For populations > 5, use Wait action
                        motorWait_.push_back(n);
                    }
                }
            } else if (type == NeuronType::Sensory) {
                // Sensory neurons organized by type and modality
                // Vision neurons are primary sensory input
                for (Neuron* n : pop->getNeurons()) {
                    // Check neuron type for better classification
                    NeuronType neuronType = n->getType();
                    if (neuronType == NeuronType::Vision) {
                        sensoryVision_.push_back(n);
                    } else if (neuronType == NeuronType::Touch) {
                        sensoryTouch_.push_back(n);
                    } else if (neuronType == NeuronType::Internal) {
                        sensoryInternal_.push_back(n);
                    } else if (neuronType == NeuronType::Proprioception) {
                        sensoryProprioception_.push_back(n);
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
    
    // Ensure all motor and sensory neurons are properly initialized for decoding
    // (They are already set up in the constructor and populated by the brain)
    
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
    if (!brain_ || !brain_->getSpikeSystem()) return;
    
    auto* spikeSystem = brain_->getSpikeSystem();
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            spikeSystem->injectCurrent(sensoryVision_[i]->getId(), current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            spikeSystem->injectCurrent(sensoryTouch_[i]->getId(), current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            spikeSystem->injectCurrent(sensoryInternal_[i]->getId(), current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            spikeSystem->injectCurrent(sensoryProprioception_[i]->getId(), current);
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

// Fix decodeFromMotorNeurons() to properly read neural membrane potentials
MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using neural membrane potentials
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
    
    // Find maximum activity using proper neural activity comparison
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
    
    // Use proper neural activity comparison, not arbitrary modulo
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful neural activity (threshold-based)
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

// Fix selectWithCuriosity() to remove arbitrary modulo and integrate neural activity properly
MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Add curiosity-based exploration to motor commands
    MotorCommand finalCommand = defaultCmd;
    
    // Apply curiosity-based exploration with intelligent sampling
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration, but intelligently biased
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Intelligent exploration based on neural activity patterns
            // Use random selection but select from actual motor neuron populations
            if (!brain_->getRegions().empty()) {
                // Get all motor neuron groups from brain regions
                std::vector<std::vector<Neuron*>> motorGroups;
                motorGroups.push_back(motorForward_);
                motorGroups.push_back(motorBackward_);
                motorGroups.push_back(motorTurnLeft_);
                motorGroups.push_back(motorTurnRight_);
                motorGroups.push_back(motorInteract_);
                motorGroups.push_back(motorWait_);
                
                // Filter to only groups that have neurons
                std::vector<size_t> validGroupIndices;
                for (size_t i = 0; i < motorGroups.size(); ++i) {
                    if (!motorGroups[i].empty()) {
                        validGroupIndices.push_back(i);
                    }
                }
                
                if (!validGroupIndices.empty()) {
                    // Use uniform random selection from valid groups
                    size_t randomGroup = validGroupIndices[brain_->getRandomGenerator()->uniformInt(0, validGroupIndices.size() - 1)];
                    
                    // Set command based on group index
                    switch (randomGroup) {
                        case 0: finalCommand = MotorCommand::MoveForward; break;
                        case 1: finalCommand = MotorCommand::MoveBackward; break;
                        case 2: finalCommand = MotorCommand::TurnLeft; break;
                        case 3: finalCommand = MotorCommand::TurnRight; break;
                        case 4: finalCommand = MotorCommand::Interact; break;
                        case 5: finalCommand = MotorCommand::Wait; break;
                    }
                }
            } else {
                // Fallback to basic random exploration if no regions
                finalCommand = static_cast<MotorCommand>(brain_->getRandomGenerator()->uniformInt(0, 5));
            }
        }
    }
    
    return finalCommand;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Apply to brain neuromodulation system
    if (brain_->getDopamine()) {
        brain_->getDopamine()->signalRewardPredictionError(predictionError_);
    }
    
    // Update neuromodulator levels through brain systems
    if (brain_->getAcetylcholine()) {
        // ACh responds to salience/novelty
        brain_->getAcetylcholine()->signalAttention(std::abs(predictionError_) * 0.5f);
    }
    
    if (brain_->getNorepinephrine()) {
        // NE responds to arousal/novelty
        brain_->getNorepinephrine()->signalArousal(noveltyLevel_ * 0.7f);
    }
    
    if (brain_->getSerotonin()) {
        // 5-HT responds to reward and mood
        brain_->getSerotonin()->signalMood(predictionError_ * 0.5f);
    }
    
    // Apply synaptic changes through brain's neuromodulation
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * predictionError_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on neuromodulators
    // Positive prediction error increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * predictionError_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to brain's neuromodulation systems
    if (brain_->getSTDP()) {
        brain_->getSTDP()->setLTPWeight(0.01f * plasticityFactor);
        brain_->getSTDP()->setLTDWeight(0.012f * plasticityFactor);
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