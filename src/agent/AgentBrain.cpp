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
    , attentionEnabled_(true)
    , socialLearningEnabled_(true)
    , explorationWeight_(1.0f)
    , exploitationWeight_(1.0f)
    , memoryRetrievalStrength_(1.0f)
    , socialLearningStrength_(1.0f)
    , attentionSpan_(30.0f)  // 30 seconds of attention span
    , curiosityDecayRate_(0.95f)
    , stressLevel_(0.0f)
    , learningProgress_(0.0f)
    , performanceScore_(0.0f)
    , confidenceInDecisions_(1.0f)
{
    // Initialize neural groups with better organization
    if (brain_) {
        // Group neurons by type and subtype for better functional organization
        std::map<NeuronType, std::vector<Neuron*>> neuronsByType;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                for (Neuron* n : pop->getNeurons()) {
                    neuronsByType[type].push_back(n);
                }
            }
        }
        
        // Organize motor neurons into functional subgroups
        organizeMotorNeurons(neuronsByType[NeuronType::Motor]);
        
        // Organize sensory neurons into functional modalities
        organizeSensoryNeurons(neuronsByType);
        
        // Initialize connection tracking
        initializeConnectionTracking();
        
        // Initialize memory systems connection
        initializeMemoryConnections();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize previous vision buffer with correct size first
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
        
        // Normalize with better handling
        size_t visionSize = std::max<size_t>(vision.size(), 1);
        noveltyLevel_ = totalDiff / visionSize;
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Clamp novelty level
        noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
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

void AgentBrain::organizeMotorNeurons(const std::vector<Neuron*>& motorNeurons) {
    if (motorNeurons.empty()) return;
    
    // More sophisticated motor neuron organization based on neuron properties
    // Group by firing characteristics and functional roles
    
    // Primary movers: neurons with low threshold and high excitability
    for (Neuron* n : motorNeurons) {
        float threshold = n->getThreshold();
        float potential = n->getMembranePotential();
        
        // Classify neurons into functional groups
        if (threshold < 0.5f && potential > 0.1f) {
            motorForward_.push_back(n);
        } else if (threshold < 0.5f && potential < -0.1f) {
            motorBackward_.push_back(n);
        } else if (threshold > 0.8f && std::abs(potential) < 0.2f) {
            motorWait_.push_back(n);
        } else if (std::abs(potential) > 0.3f) {
            motorTurnLeft_.push_back(n);  // Default to left for odd potentials
        } else {
            motorInteract_.push_back(n);
        }
    }
}

void AgentBrain::organizeSensoryNeurons(const std::map<NeuronType, std::vector<Neuron*>>& neuronsByType) {
    const auto& it = neuronsByType.find(NeuronType::Sensory);
    if (it == neuronsByType.end()) return;
    
    const auto& sensoryNeurons = it->second;
    if (sensoryNeurons.empty()) return;
    
    // More sophisticated sensory neuron organization
    // Group by functional modality based on membrane potential patterns
    
    for (Neuron* n : sensoryNeurons) {
        float potential = n->getMembranePotential();
        
        // Vision neurons: typically positive potentials from external stimuli
        if (potential > 0.3f) {
            sensoryVision_.push_back(n);
        } 
        // Touch neurons: intermediate potentials
        else if (potential > 0.0f && potential <= 0.3f) {
            sensoryTouch_.push_back(n);
        } 
        // Internal neurons: negative potentials (homeostatic)
        else if (potential > -0.3f && potential <= 0.0f) {
            sensoryInternal_.push_back(n);
        } 
        // Proprioception neurons: strongly negative (body state)
        else {
            sensoryProprioception_.push_back(n);
        }
    }
}

void AgentBrain::initializeConnectionTracking() {
    connectionHistory_.clear();
    connectionHistory_.reserve(1000);
    
    // Initialize connection strength tracking
    for (size_t i = 0; i < 1000; ++i) {
        connectionHistory_.push_back(0.0f);
    }
}

void AgentBrain::initializeMemoryConnections() {
    memoryConnections_.clear();
    
    // Connect to memory systems if available
    if (brain_) {
        // Store references to memory systems
        if (auto* workingMem = brain_->getWorkingMemory()) {
            workingMemory_ = workingMem;
        }
        if (auto* episodicMem = brain_->getEpisodicMemory()) {
            episodicMemory_ = episodicMem;
        }
        if (auto* assocMem = brain_->getAssociativeMemory()) {
            associativeMemory_ = assocMem;
        }
    }
}

void AgentBrain::resetAdvancedFeatures() {
    // Reset advanced curiosity and exploration parameters
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    predictionError_ = 0.0f;
    dopamineLevel_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset stress and performance tracking
    stressLevel_ = 0.0f;
    learningProgress_ = 0.0f;
    performanceScore_ = 0.0f;
    confidenceInDecisions_ = 1.0f;
    
    // Reset action planning
    currentActionPlan_.clear();
    actionValues_.clear();
    
    // Reset memory learning
    learnedMemories_.clear();
    
    // Reset social learning
    observedAgents_.clear();
    sharedKnowledge_.clear();
    
    // Reset personality traits to baseline
    personalityTraits_.curiosity = 0.5f;
    personalityTraits_.exploration = 0.5f;
    personalityTraits_.social = 0.5f;
    personalityTraits_.conservative = 0.5f;
    
    // Clear connection history
    connectionHistory_.clear();
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group with weighted importance
    float forwardAct = calcWeightedActivity(motorForward_);
    float backwardAct = calcWeightedActivity(motorBackward_);
    float leftAct = calcWeightedActivity(motorTurnLeft_);
    float rightAct = calcWeightedActivity(motorTurnRight_);
    float interactAct = calcWeightedActivity(motorInteract_);
    float waitAct = calcWeightedActivity(motorWait_);
    
    // Apply exploration bias based on curiosity level
    if (curiosityLevel_ > 0.5f) {
        float explorationFactor = (curiosityLevel_ - 0.5f) * 2.0f;  // 0 to 1
        // Add noise to decision making
        float noise = brain_->getRandomGenerator()->uniformReal(-0.1f, 0.1f);
        forwardAct += noise * explorationFactor;
        backwardAct += noise * explorationFactor;
        leftAct += noise * explorationFactor;
        rightAct += noise * explorationFactor;
        interactAct += noise * explorationFactor;
    }
    
    // Find maximum activity with tie-breaking
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    // Create command-activity pairs
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Sort by activity (descending) to find best
    std::vector<CommandActivity> sorted(commands, commands + 6);
    std::sort(sorted.begin(), sorted.end(),
              [](const CommandActivity& a, const CommandActivity& b) {
                  return a.activity > b.activity;
              });
    
    // Select best command
    best = sorted[0].cmd;
    bestActivity = sorted[0].activity;
    
    // Apply curiosity-based exploration
    if (curiosityLevel_ > 0.3f) {
        best = selectWithCuriosity(best);
    }
    
    return best;
}

float AgentBrain::calcWeightedActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    
    // Apply development stage modifiers to activity calculation
    float activitySum = 0.0f;
    float weightSum = 0.0f;
    
    for (Neuron* n : neurons) {
        // Base activity from membrane potential deviation
        float activity = std::abs(n->getMembranePotential() - n->getRestingPotential());
        
        // Apply developmental stage modifier
        float devModifier = 1.0f;
        if (developmentEnabled_) {
            devModifier = 1.0f + plasticityModifier_ * 0.5f;
        }
        
        // Apply neuromodulation effect
        float modEffect = 1.0f + dopamineLevel_ * 0.3f;
        
        float weightedActivity = activity * devModifier * modEffect;
        activitySum += weightedActivity;
        weightSum += 1.0f;
    }
    
    return (weightSum > 0.0f) ? activitySum / weightSum : 0.0f;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced curiosity-based exploration
    if (!curiosityEnabled_ || curiosityLevel_ <= 0.0f) {
        return defaultCmd;
    }
    
    // Higher curiosity = more exploration
    float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
    
    // Apply exploration strategy bias
    switch (explorationStrategy_) {
        case ExplorationStrategy::Random:
            exploreChance = 0.5f;  // Always 50% exploration
            break;
        case ExplorationStrategy::Directed:
            exploreChance = curiosityLevel_ * 0.1f;  // 10% max
            break;
        case ExplorationStrategy::Balanced:
            exploreChance = curiosityLevel_ * 0.2f;  // 20% max
            break;
    }
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        // Random motor command with weighted probabilities
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
    
    return defaultCmd;
}

void AgentBrain::applyAdvancedNeuromodulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward with adaptive learning rate
    float learningRate = 0.05f * plasticityModifier_;
    expectedReward_ = (1.0f - learningRate) * expectedReward_ + learningRate * reward;
    
    // Dopamine-like signal with adaptive response
    dopamineLevel_ = predictionError_ * (1.0f + stressLevel_ * 0.5f);
    
    // Clamp to reasonable range with wider bounds for exploration
    dopamineLevel_ = std::clamp(dopamineLevel_, -2.0f, 2.0f);
    
    // Apply to brain systems with eligibility traces
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > 0.001f) {
                    // Apply reward-modulated weight change with stress modifier
                    float stressModifier = 1.0f + stressLevel_ * 0.5f;
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_ * stressModifier;
                    syn->addToWeight(delta);
                    
                    // Adaptive eligibility decay
                    float decayRate = 0.1f + stressLevel_ * 0.05f;
                    syn->decayEligibilityTrace(decayRate);
                }
            }
        }
    }
    
    // Update learning progress based on prediction error
    learningProgress_ = std::min(1.0f, learningProgress_ + std::abs(predictionError_) * 0.01f);
    
    // Update performance score
    performanceScore_ = (performanceScore_ * 0.9f) + (reward * 0.1f);
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age with more granularity
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.2f;  // Enhanced plasticity in early stage
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.9f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.6f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else if (developmentalAge_ < 3600.0) {  // ~1 hour
        plasticityModifier_ = 0.3f;  // Adult
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    } else {
        plasticityModifier_ = 0.1f;  // Aging - some plasticity remains
        brain_->setDevelopmentalStage(DevelopmentalStage::Aging);
    }
    
    // Apply development system effects to brain
    applyDevelopmentToBrain(brain_.get());
    
    // Adjust personality traits over time based on development
    adjustPersonalityOverTime(timestep);
}

void AgentBrain::updateStressBasedOnPerformance(float performance) {
    // Stress increases with poor performance, decreases with good performance
    float stressChange = (1.0f - performance) * 0.1f - performance * 0.05f;
    stressLevel_ = std::clamp(stressLevel_ + stressChange, 0.0f, 1.0f);
    
    // High stress reduces exploration
    if (stressLevel_ > 0.7f) {
        explorationWeight_ = 0.3f;
        exploitationWeight_ = 2.0f;
    } else {
        // Restore weights based on personality and curiosity
        explorationWeight_ = personalityTraits_.exploration;
        exploitationWeight_ = personalityTraits_.conservative * 2.0f;
    }
}

void AgentBrain::learnFromMemory(const EpisodicMemoryItem& memory) {
    // Extract relevant patterns from memory and apply to current behavior
    learnedMemories_.push_back(memory);
    
    // Keep only recent memories (last 100)
    if (learnedMemories_.size() > 100) {
        learnedMemories_.erase(learnedMemories_.begin());
    }
    
    // Apply learned actions to action values
    for (const auto& neuron : memory.activeNeurons) {
        // Learn from successful action patterns
        float memoryValue = memory.reward * memory.age * 0.1f;  // Weight by age and reward
        
        // Find corresponding motor command and update its value
        // This is simplified - real implementation would map neurons to commands
        if (!memory.action.empty()) {
            // Map action string to MotorCommand enum
            MotorCommand action = stringToMotorCommand(memory.action);
            updateActionValue(action, memoryValue);
        }
    }
}

float AgentBrain::evaluateActionBenefit(const MotorCommand& action) const {
    // Evaluate action based on learned values and current context
    float baseValue = getActionValue(action);
    
    // Apply developmental modifiers
    float devModifier = 1.0f;
    if (developmentEnabled_) {
        devModifier = 0.5f + plasticityModifier_ * 0.5f;
    }
    
    // Apply stress modifiers
    float stressModifier = 1.0f - stressLevel_ * 0.2f;
    
    // Apply exploration weight
    float explorationModifier = explorationWeight_ * 0.1f;
    
    return baseValue * devModifier * stressModifier + explorationModifier;
}

MotorCommand AgentBrain::stringToMotorCommand(const std::string& action) const {
    // Map string action to MotorCommand enum
    static const std::unordered_map<std::string, MotorCommand> actionMap = {
        {"move_forward", MotorCommand::MoveForward},
        {"move_backward", MotorCommand::MoveBackward},
        {"turn_left", MotorCommand::TurnLeft},
        {"turn_right", MotorCommand::TurnRight},
        {"look_left", MotorCommand::LookLeft},
        {"look_right", MotorCommand::LookRight},
        {"interact", MotorCommand::Interact},
        {"wait", MotorCommand::Wait}
    };
    
    auto it = actionMap.find(action);
    if (it != actionMap.end()) {
        return it->second;
    }
    return MotorCommand::Wait;  // Default
}

void AgentBrain::saveState(const std::string& filename) const {
    // Save agent state to file for persistence
    std::ofstream file(filename, std::ios::binary);
    if (!file) return;
    
    try {
        // Save neural group sizes (not actual neuron pointers)
        uint32_t motorForwardSize = static_cast<uint32_t>(motorForward_.size());
        uint32_t motorBackwardSize = static_cast<uint32_t>(motorBackward_.size());
        uint32_t motorTurnLeftSize = static_cast<uint32_t>(motorTurnLeft_.size());
        uint32_t motorTurnRightSize = static_cast<uint32_t>(motorTurnRight_.size());
        uint32_t motorInteractSize = static_cast<uint32_t>(motorInteract_.size());
        uint32_t motorWaitSize = static_cast<uint32_t>(motorWait_.size());
        
        file.write(reinterpret_cast<const char*>(&motorForwardSize), sizeof(motorForwardSize));
        file.write(reinterpret_cast<const char*>(&motorBackwardSize), sizeof(motorBackwardSize));
        file.write(reinterpret_cast<const char*>(&motorTurnLeftSize), sizeof(motorTurnLeftSize));
        file.write(reinterpret_cast<const char*>(&motorTurnRightSize), sizeof(motorTurnRightSize));
        file.write(reinterpret_cast<const char*>(&motorInteractSize), sizeof(motorInteractSize));
        file.write(reinterpret_cast<const char*>(&motorWaitSize), sizeof(motorWaitSize));
        
        // Save neuromodulation state
        file.write(reinterpret_cast<const char*>(&dopamineLevel_), sizeof(dopamineLevel_));
        file.write(reinterpret_cast<const char*>(&noveltyLevel_), sizeof(noveltyLevel_));
        file.write(reinterpret_cast<const char*>(&curiosityLevel_), sizeof(curiosityLevel_));
        file.write(reinterpret_cast<const char*>(&predictionError_), sizeof(predictionError_));
        file.write(reinterpret_cast<const char*>(&expectedReward_), sizeof(expectedReward_));
        
        // Save development state
        file.write(reinterpret_cast<const char*>(&developmentalAge_), sizeof(developmentalAge_));
        file.write(reinterpret_cast<const char*>(&plasticityModifier_), sizeof(plasticityModifier_));
        
        // Save configuration flags
        file.write(reinterpret_cast<const char*>(&rewardModulationEnabled_), sizeof(rewardModulationEnabled_));
        file.write(reinterpret_cast<const char*>(&structuralPlasticityEnabled_), sizeof(structuralPlasticityEnabled_));
        file.write(reinterpret_cast<const char*>(&developmentEnabled_), sizeof(developmentEnabled_));
        file.write(reinterpret_cast<const char*>(&curiosityEnabled_), sizeof(curiosityEnabled_));
        file.write(reinterpret_cast<const char*>(&attentionEnabled_), sizeof(attentionEnabled_));
        file.write(reinterpret_cast<const char*>(&socialLearningEnabled_), sizeof(socialLearningEnabled_));
        
        // Save advanced parameters
        file.write(reinterpret_cast<const char*>(&explorationWeight_), sizeof(explorationWeight_));
        file.write(reinterpret_cast<const char*>(&exploitationWeight_), sizeof(exploitationWeight_));
        file.write(reinterpret_cast<const char*>(&memoryRetrievalStrength_), sizeof(memoryRetrievalStrength_));
        file.write(reinterpret_cast<const char*>(&socialLearningStrength_), sizeof(socialLearningStrength_));
        file.write(reinterpret_cast<const char*>(&attentionSpan_), sizeof(attentionSpan_));
        file.write(reinterpret_cast<const char*>(&curiosityDecayRate_), sizeof(curiosityDecayRate_));
        
        // Save personality traits
        file.write(reinterpret_cast<const char*>(&personalityTraits_), sizeof(personalityTraits_));
        
        // Save stress and performance
        file.write(reinterpret_cast<const char*>(&stressLevel_), sizeof(stressLevel_));
        file.write(reinterpret_cast<const char*>(&learningProgress_), sizeof(learningProgress_));
        file.write(reinterpret_cast<const char*>(&performanceScore_), sizeof(performanceScore_));
        file.write(reinterpret_cast<const char*>(&confidenceInDecisions_), sizeof(confidenceInDecisions_));
        
        // Save exploration strategy
        file.write(reinterpret_cast<const char*>(&explorationStrategy_), sizeof(explorationStrategy_));
        
        // Save action values (simplified)
        uint32_t actionValuesSize = static_cast<uint32_t>(actionValues_.size());
        file.write(reinterpret_cast<const char*>(&actionValuesSize), sizeof(actionValuesSize));
        if (actionValuesSize > 0) {
            file.write(reinterpret_cast<const char*>(actionValues_.data()), actionValuesSize * sizeof(float));
        }
        
        // Save connection history (simplified)
        uint32_t historySize = static_cast<uint32_t>(connectionHistory_.size());
        file.write(reinterpret_cast<const char*>(&historySize), sizeof(historySize));
        if (historySize > 0) {
            file.write(reinterpret_cast<const char*>(connectionHistory_.data()), historySize * sizeof(float));
        }
        
    } catch (const std::exception& e) {
        // Error handling - log and continue
    }
    
    file.close();
}

void AgentBrain::loadState(const std::string& filename) {
    // Load agent state from file
    std::ifstream file(filename, std::ios::binary);
    if (!file) return;
    
    try {
        // Load neural group sizes
        uint32_t motorForwardSize, motorBackwardSize, motorTurnLeftSize, motorTurnRightSize;
        uint32_t motorInteractSize, motorWaitSize;
        
        file.read(reinterpret_cast<char*>(&motorForwardSize), sizeof(motorForwardSize));
        file.read(reinterpret_cast<char*>(&motorBackwardSize), sizeof(motorBackwardSize));
        file.read(reinterpret_cast<char*>(&motorTurnLeftSize), sizeof(motorTurnLeftSize));
        file.read(reinterpret_cast<char*>(&motorTurnRightSize), sizeof(motorTurnRightSize));
        file.read(reinterpret_cast<char*>(&motorInteractSize), sizeof(motorInteractSize));
        file.read(reinterpret_cast<char*>(&motorWaitSize), sizeof(motorWaitSize));
        
        // Recreate neuron groups (simplified - would need actual neuron pointers)
        // In real implementation, would need to map saved neuron IDs back to actual neurons
        motorForward_.clear();
        motorBackward_.clear();
        motorTurnLeft_.clear();
        motorTurnRight_.clear();
        motorInteract_.clear();
        motorWait_.clear();
        
        // Load neuromodulation state
        file.read(reinterpret_cast<char*>(&dopamineLevel_), sizeof(dopamineLevel_));
        file.read(reinterpret_cast<char*>(&noveltyLevel_), sizeof(noveltyLevel_));
        file.read(reinterpret_cast<char*>(&curiosityLevel_), sizeof(curiosityLevel_));
        file.read(reinterpret_cast<char*>(&predictionError_), sizeof(predictionError_));
        file.read(reinterpret_cast<char*>(&expectedReward_), sizeof(expectedReward_));
        
        // Load development state
        file.read(reinterpret_cast<char*>(&developmentalAge_), sizeof(developmentalAge_));
        file.read(reinterpret_cast<char*>(&plasticityModifier_), sizeof(plasticityModifier_));
        
        // Load configuration flags
        file.read(reinterpret_cast<char*>(&rewardModulationEnabled_), sizeof(rewardModulationEnabled_));
        file.read(reinterpret_cast<char*>(&structuralPlasticityEnabled_), sizeof(structuralPlasticityEnabled_));
        file.read(reinterpret_cast<char*>(&developmentEnabled_), sizeof(developmentEnabled_));
        file.read(reinterpret_cast<char*>(&curiosityEnabled_), sizeof(curiosityEnabled_));
        file.read(reinterpret_cast<char*>(&attentionEnabled_), sizeof(attentionEnabled_));
        file.read(reinterpret_cast<char*>(&socialLearningEnabled_), sizeof(socialLearningEnabled_));
        
        // Load advanced parameters
        file.read(reinterpret_cast<char*>(&explorationWeight_), sizeof(explorationWeight_));
        file.read(reinterpret_cast<char*>(&exploitationWeight_), sizeof(exploitationWeight_));
        file.read(reinterpret_cast<char*>(&memoryRetrievalStrength_), sizeof(memoryRetrievalStrength_));
        file.read(reinterpret_cast<char*>(&socialLearningStrength_), sizeof(socialLearningStrength_));
        file.read(reinterpret_cast<char*>(&attentionSpan_), sizeof(attentionSpan_));
        file.read(reinterpret_cast<char*>(&curiosityDecayRate_), sizeof(curiosityDecayRate_));
        
        // Load personality traits
        file.read(reinterpret_cast<char*>(&personalityTraits_), sizeof(personalityTraits_));
        
        // Load stress and performance
        file.read(reinterpret_cast<char*>(&stressLevel_), sizeof(stressLevel_));
        file.read(reinterpret_cast<char*>(&learningProgress_), sizeof(learningProgress_));
        file.read(reinterpret_cast<char*>(&performanceScore_), sizeof(performanceScore_));
        file.read(reinterpret_cast<char*>(&confidenceInDecisions_), sizeof(confidenceInDecisions_));
        
        // Load exploration strategy
        file.read(reinterpret_cast<char*>(&explorationStrategy_), sizeof(explorationStrategy_));
        
        // Load action values
        uint32_t actionValuesSize;
        file.read(reinterpret_cast<char*>(&actionValuesSize), sizeof(actionValuesSize));
        actionValues_.resize(actionValuesSize);
        if (actionValuesSize > 0) {
            file.read(reinterpret_cast<char*>(actionValues_.data()), actionValuesSize * sizeof(float));
        }
        
        // Load connection history
        uint32_t historySize;
        file.read(reinterpret_cast<char*>(&historySize), sizeof(historySize));
        connectionHistory_.resize(historySize);
        if (historySize > 0) {
            file.read(reinterpret_cast<char*>(connectionHistory_.data()), historySize * sizeof(float));
        }
        
    } catch (const std::exception& e) {
        // Error handling - log and use defaults
    }
    
    file.close();
}

void AgentBrain::reset(bool preserveMemory, bool resetPersonality) {
    if (!preserveMemory) {
        // Clear memory systems
        if (workingMemory_) workingMemory_->clear();
        if (episodicMemory_) episodicMemory_->clear();
        if (associativeMemory_) associativeMemory_->clear();
        
        // Clear learned memories
        learnedMemories_.clear();
    }
    
    if (resetPersonality) {
        // Reset personality to baseline
        personalityTraits_ = PersonalityTraits{0.5f, 0.5f, 0.5f, 0.5f};
        
        // Restore exploration weights from personality
        explorationWeight_ = personalityTraits_.exploration;
        exploitationWeight_ = personalityTraits_.conservative * 2.0f;
    }
    
    // Reset core state
    resetAdvancedFeatures();
    
    // Reset development
    if (brain_) {
        brain_->reset();
        brain_->initialize();
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
    }
}

void AgentBrain::observeOtherAgent(const AgentBrain& otherAgent) {
    if (!socialLearningEnabled_) return;
    
    // Add to observed agents
    observedAgents_.push_back(&otherAgent);
    if (observedAgents_.size() > 10) {  // Keep top 10
        observedAgents_.erase(observedAgents_.begin());
    }
    
    // Learn from observed agent
    // This is simplified - real implementation would extract more sophisticated patterns
    float socialStrength = socialLearningStrength_ * 0.5f;  // Half strength for social learning
    
    // Share knowledge (bidirectional)
    shareKnowledge(otherAgent);
}

void AgentBrain::shareKnowledge(const AgentBrain& otherAgent) {
    if (!socialLearningEnabled_) return;
    
    // Record shared knowledge
    KnowledgeShare share;
    share.sourceAgentId = std::hash<const AgentBrain*>{}( &otherAgent );  // Simplified ID
    share.knowledge = "general_learning";  // Simplified
    share.strength = socialLearningStrength_;
    share.timestamp = static_cast<float>(std::time(nullptr));
    
    sharedKnowledge_.push_back(share);
    
    // Keep recent knowledge
    if (sharedKnowledge_.size() > 50) {
        sharedKnowledge_.erase(sharedKnowledge_.begin());
    }
}

void AgentBrain::teachSkill(const std::string& skillName, float proficiency) {
    // Record teaching event
    SkillTeach teach;
    teach.skillName = skillName;
    teach.proficiency = proficiency;
    teach.timestamp = static_cast<float>(std::time(nullptr));
    
    // Learn from teaching
    learnFromMemory(createMemoryFromSkill(skillName, proficiency));
}

EpisodicMemoryItem AgentBrain::createMemoryFromSkill(const std::string& skillName, float proficiency) {
    EpisodicMemoryItem memory;
    memory.action = skillName;
    memory.reward = proficiency * 10.0f;  // Scale proficiency to reward
    memory.timestamp = static_cast<SimulationStep>(std::time(nullptr));
    memory.age = 10;  // Recent memory
    
    // Create neural pattern for skill
    memory.sensoryState.resize(10, 0.0f);  // Simplified sensory pattern
    memory.resultingSensoryState.resize(10, 0.0f);
    
    return memory;
}

void AgentBrain::setCuriosityParameters(float base, float noveltyWeight, float predictionErrorWeight) {
    curiosityBase_ = base;
    curiosityNoveltyWeight_ = noveltyWeight;
    curiosityPredictionErrorWeight_ = predictionErrorWeight;
    
    // Recalculate curiosity
    curiosityLevel_ = (noveltyLevel_ * curiosityNoveltyWeight_ + 
                      std::abs(predictionError_) * curiosityPredictionErrorWeight_) * curiosityBase_;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void AgentBrain::setExplorationStrategy(ExplorationStrategy strategy) {
    explorationStrategy_ = strategy;
    
    // Adjust exploration parameters based on strategy
    switch (strategy) {
        case ExplorationStrategy::Random:
            curiosityLevel_ = 1.0f;
            explorationWeight_ = 0.8f;
            break;
        case ExplorationStrategy::Directed:
            curiosityLevel_ = 0.5f;
            explorationWeight_ = 0.3f;
            break;
        case ExplorationStrategy::Balanced:
            curiosityLevel_ = 0.7f;
            explorationWeight_ = 0.5f;
            break;
        case ExplorationStrategy::Exploitative:
            curiosityLevel_ = 0.8f;
            explorationWeight_ = 0.9f;
            break;
        case ExplorationStrategy::Conservative:
            curiosityLevel_ = 0.2f;
            explorationWeight_ = 0.1f;
            break;
    }
}

void AgentBrain::adjustExplorationBasedOnContext() {
    if (!brain_) return;
    
    // Adjust exploration based on environmental uncertainty
    // Get current world state (simplified)
    float environmentalUncertainty = 0.5f;  // Default
    
    // Factor in stress level
    environmentalUncertainty *= (1.0f + stressLevel_ * 0.5f);
    
    // Adjust curiosity and exploration based on uncertainty
    if (environmentalUncertainty > 0.7f) {
        // High uncertainty = more exploration
        curiosityLevel_ = std::min(1.0f, curiosityLevel_ * 1.5f);
        explorationWeight_ = std::min(1.0f, explorationWeight_ * 1.2f);
    } else {
        // Low uncertainty = more exploitation
        curiosityLevel_ = std::max(0.0f, curiosityLevel_ * 0.8f);
        explorationWeight_ = std::max(0.1f, explorationWeight_ * 0.9f);
    }
}

float AgentBrain::getLearningProgress() const {
    if (learnedMemories_.empty()) return 0.0f;
    
    float totalProgress = 0.0f;
    for (const auto& memory : learnedMemories_) {
        totalProgress += memory.reward * memory.age * 0.01f;
    }
    
    return std::min(1.0f, totalProgress / learnedMemories_.size());
}

float AgentBrain::getPerformanceScore() const {
    return performanceScore_;
}

float AgentBrain::getConfidenceInDecisions() const {
    return confidenceInDecisions_;
}

void AgentBrain::setStressLevel(float stress) {
    stressLevel_ = std::clamp(stress, 0.0f, 1.0f);
    
    // Update exploration weights based on stress
    if (stressLevel_ > 0.7f) {
        explorationWeight_ = 0.3f;
        exploitationWeight_ = 2.0f;
    } else if (stressLevel_ > 0.4f) {
        explorationWeight_ = 0.5f;
        exploitationWeight_ = 1.5f;
    } else {
        explorationWeight_ = personalityTraits_.exploration;
        exploitationWeight_ = personalityTraits_.conservative * 2.0f;
    }
}

void AgentBrain::adjustPersonalityOverTime(double timestep) {
    // Personality traits slowly adapt over time based on development
    double personalityChangeRate = 0.01f * timestep;
    
    // Curiosity increases slightly with development in early stages
    if (developmentalAge_ < 300.0) {
        personalityTraits_.curiosity = std::min(1.0f, personalityTraits_.curiosity + 0.01f);
    }
    
    // Exploration increases with curiosity
    personalityTraits_.exploration = personalityTraits_.curiosity * 0.8f;
    
    // Social learning increases with development
    personalityTraits_.social = std::min(1.0f, personalityTraits_.social + 0.005f * timestep);
    
    // Conservative trait decreases with age (more risk-taking with age)
    if (developmentalAge_ > 900.0) {
        personalityTraits_.conservative = std::max(0.3f, personalityTraits_.conservative - 0.002f * timestep);
    }
    
    // Update exploration weights from personality
    explorationWeight_ = personalityTraits_.exploration;
    exploitationWeight_ = personalityTraits_.conservative * 2.0f;
}

} // namespace nlm

} // namespace nlm
