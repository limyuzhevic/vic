// Define constants for cleaner code
constexpr size_t NUM_MOTOR_GROUPS = 6;
constexpr size_t NUM_SENSORY_GROUPS = 4;

// Motor command activity threshold for action selection
constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;

// Curiosity-based exploration thresholds
constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.3f;
constexpr float CURIOSITY_HIGH_THRESHOLD = 0.5f;

// Novelty computation constants
constexpr float NOVELTY_VISION_MULTIPLIER = 5.0f;
constexpr float NOVELTY_TOUCH_MULTIPLIER = 8.0f;
constexpr float NOVELTY_INTERNAL_MULTIPLIER = 5.0f;
constexpr float NOVELTY_PROPrioCEPTION_MULTIPLIER = 3.0f;
constexpr float NOVELTY_INTERNAL_OFFSET = 1.0f;
constexpr float NOVELTY_PROPrioCEPTION_OFFSET = 1.0f;
constexpr float NOVELTY_NORMALIZATION_DIVISOR = 1.0f;

// Development stage age thresholds
constexpr double DEVELOPMENT_INITIAL_AGE = 60.0;
constexpr double DEVELOPMENT_CRITICAL_AGE = 300.0;
constexpr double DEVELOPMENT_MATURATION_AGE = 900.0;

// Plasticity rate constants
constexpr float PLASTICITY_SYNAPTOGENESIS_RATE = 0.0001f;
constexpr float PLASTICITY_PRUNING_RATE = 0.00001f;
constexpr float PLASTICITY_FACTOR_2 = 2.0f;

// Neuromodulation constants
constexpr float NEUROMODULATION_DOPAMINE_CLAMP_MIN = -1.0f;
constexpr float NEUROMODULATION_DOPAMINE_CLAMP_MAX = 1.0f;
constexpr float NEUROMODULATION_ELIGIBILITY_THRESHOLD = 0.001f;
constexpr float NEUROMODULATION_ELIGIBILITY_DECAY = 0.1f;
constexpr float NEUROMODULATION_PLASTICITY_BASE = 0.5f;
constexpr float NEUROMODULATION_PLASTICITY_MOD = 0.5f;
constexpr float NEUROMODULATION_STDP_LTP_BASE = 0.01f;
constexpr float NEUROMODULATION_STDP_LTD_BASE = 0.012f;
constexpr float NEUROMODULATION_STDP_CLAMP_MIN = 0.1f;
constexpr float NEUROMODULATION_STDP_CLAMP_MAX = 2.0f;

// Random choice constants
constexpr int RANDOM_MOTOR_COMMAND_MAX = 7;
constexpr int RANDOM_INTERACTION_CHOICE = 4;

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
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
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot initialize with null brain pointer");
        return;
    }
    
    // Validate brain structure has required regions
    const auto& regions = brain_->getRegions();
    if (regions.empty()) {
        NLM_LOG_WARNING("AgentBrain: Brain has no regions, cannot assign neurons");
        return;
    }
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        const auto& populations = region->getPopulations();
        if (populations.empty()) {
            NLM_LOG_WARNING("AgentBrain: Region has no populations");
            continue;
        }
        
        for (const auto& pop : populations) {
            if (!pop) continue;
            
            NeuronType type = pop->getNeuronType();
            const auto& neurons = pop->getNeurons();
            if (neurons.empty()) continue;
            
            if (type == NeuronType::Motor) {
                // Validate motor group assignment logic
                size_t idx = motorForward_.size() + motorBackward_.size() + 
                            motorTurnLeft_.size() + motorTurnRight_.size() +
                            motorInteract_.size() + motorWait_.size();
                
                MotorGroup group = static_cast<MotorGroup>(idx % NUM_MOTOR_GROUPS);
                switch (group) {
                    case MotorGroup::Forward: motorForward_.reserve(motorForward_.size() + neurons.size()); break;
                    case MotorGroup::Backward: motorBackward_.reserve(motorBackward_.size() + neurons.size()); break;
                    case MotorGroup::TurnLeft: motorTurnLeft_.reserve(motorTurnLeft_.size() + neurons.size()); break;
                    case MotorGroup::TurnRight: motorTurnRight_.reserve(motorTurnRight_.size() + neurons.size()); break;
                    case MotorGroup::Interact: motorInteract_.reserve(motorInteract_.size() + neurons.size()); break;
                    case MotorGroup::Wait: motorWait_.reserve(motorWait_.size() + neurons.size()); break;
                }
                
                for (Neuron* n : neurons) {
                    if (!n) {
                        NLM_LOG_ERROR("AgentBrain: Null neuron pointer found in motor population");
                        continue;
                    }
                    
                    switch (group) {
                        case MotorGroup::Forward: motorForward_.push_back(n); break;
                        case MotorGroup::Backward: motorBackward_.push_back(n); break;
                        case MotorGroup::TurnLeft: motorTurnLeft_.push_back(n); break;
                        case MotorGroup::TurnRight: motorTurnRight_.push_back(n); break;
                        case MotorGroup::Interact: motorInteract_.push_back(n); break;
                        case MotorGroup::Wait: motorWait_.push_back(n); break;
                    }
                }
            } else if (type == NeuronType::Sensory) {
                // Validate sensory group assignment logic
                size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                            sensoryInternal_.size() + sensoryProprioception_.size();
                
                SensoryGroup group = static_cast<SensoryGroup>(idx % NUM_SENSORY_GROUPS);
                switch (group) {
                    case SensoryGroup::Vision: sensoryVision_.reserve(sensoryVision_.size() + neurons.size()); break;
                    case SensoryGroup::Touch: sensoryTouch_.reserve(sensoryTouch_.size() + neurons.size()); break;
                    case SensoryGroup::Internal: sensoryInternal_.reserve(sensoryInternal_.size() + neurons.size()); break;
                    case SensoryGroup::Proprioception: sensoryProprioception_.reserve(sensoryProprioception_.size() + neurons.size()); break;
                }
                
                for (Neuron* n : neurons) {
                    if (!n) {
                        NLM_LOG_ERROR("AgentBrain: Null neuron pointer found in sensory population");
                        continue;
                    }
                    
                    switch (group) {
                        case SensoryGroup::Vision: sensoryVision_.push_back(n); break;
                        case SensoryGroup::Touch: sensoryTouch_.push_back(n); break;
                        case SensoryGroup::Internal: sensoryInternal_.push_back(n); break;
                        case SensoryGroup::Proprioception: sensoryProprioception_.push_back(n); break;
                    }
                }
            }
        }
    }
    
    // Validate that we have meaningful neuron assignments
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() + 
                              motorTurnLeft_.size() + motorTurnRight_.size() +
                              motorInteract_.size() + motorWait_.size();
    size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                               sensoryInternal_.size() + sensoryProprioception_.size();
    
    if (totalMotorNeurons == 0) {
        NLM_LOG_WARNING("AgentBrain: No motor neurons assigned");
    }
    if (totalSensoryNeurons == 0) {
        NLM_LOG_WARNING("AgentBrain: No sensory neurons assigned");
    }

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::initialize: Cannot initialize with null brain pointer");
        return;
    }
    
    // Validate world configuration
    if (world.getVisionWidth() <= 0 || world.getVisionHeight() <= 0) {
        NLM_LOG_ERROR("AgentBrain::initialize: Invalid world vision dimensions");
        return;
    }
    
    // Resize previous vision buffer with bounds checking
    size_t visionBufferSize = static_cast<size_t>(world.getVisionWidth()) * 
                             static_cast<size_t>(world.getVisionHeight());
    previousVision_.resize(visionBufferSize, 0.0f);
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Validate neuron group assignments
    size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                               sensoryInternal_.size() + sensoryProprioception_.size();
    
    if (totalSensoryNeurons == 0) {
        NLM_LOG_WARNING("AgentBrain::initialize: No sensory neurons available for input processing");
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Use named constants for input dimensions
    constexpr size_t VISION_INPUT_SIZE = 256;
    constexpr size_t TOUCH_INPUT_SIZE = 8;
    constexpr size_t INTERNAL_INPUT_SIZE = 4;
    constexpr size_t PROPRIOCEPTION_INPUT_SIZE = 6;
    
    return VISION_INPUT_SIZE + TOUCH_INPUT_SIZE + INTERNAL_INPUT_SIZE + PROPRIOCEPTION_INPUT_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    return NUM_MOTOR_GROUPS;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput: Cannot process sensory input with null brain pointer");
        return;
    }
    
    // Validate percept data
    if (!percept.getVision().empty() && sensoryVision_.empty()) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Vision neurons available but percept has no vision data");
    }
    
    // Validate neuron group null pointers and input dimensions
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Vision input processing
    if (!sensoryVision_.empty()) {
        size_t visionLimit = std::min(sensoryVision_.size(), vision.size());
        for (size_t i = 0; i < visionLimit; ++i) {
            if (sensoryVision_[i]) {
                float scaledInput = vision[i] * NOVELTY_VISION_MULTIPLIER;
                sensoryVision_[i]->injectCurrent(scaledInput);
            }
        }
    }
    
    // Touch input processing
    if (!sensoryTouch_.empty()) {
        size_t touchLimit = std::min(sensoryTouch_.size(), touch.size());
        for (size_t i = 0; i < touchLimit; ++i) {
            if (sensoryTouch_[i]) {
                float scaledInput = touch[i] * NOVELTY_TOUCH_MULTIPLIER;
                sensoryTouch_[i]->injectCurrent(scaledInput);
            }
        }
    }
    
    // Internal signals processing
    if (!sensoryInternal_.empty()) {
        size_t internalLimit = std::min(sensoryInternal_.size(), intern.size());
        for (size_t i = 0; i < internalLimit; ++i) {
            if (sensoryInternal_[i]) {
                float normalizedInternal = (intern[i] * 2.0f - NOVELTY_INTERNAL_OFFSET) * NOVELTY_INTERNAL_MULTIPLIER;
                sensoryInternal_[i]->injectCurrent(normalizedInternal);
            }
        }
    }
    
    // Proprioception processing
    if (!sensoryProprioception_.empty()) {
        size_t proprioLimit = std::min(sensoryProprioception_.size(), proprio.size());
        for (size_t i = 0; i < proprioLimit; ++i) {
            if (sensoryProprioception_[i]) {
                float normalizedProprio = (proprio[i] * 2.0f - NOVELTY_PROPrioCEPTION_OFFSET) * NOVELTY_PROPrioCEPTION_MULTIPLIER;
                sensoryProprioception_[i]->injectCurrent(normalizedProprio);
            }
        }
    }
    
    // Novelty detection (computes difference from previous vision)
    if (!vision.empty() && !sensoryVision_.empty() && !previousVision_.empty() && 
        vision.size() == previousVision_.size()) {
        float totalDifference = 0.0f;
        for (size_t i = 0; i < vision.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDifference += diff;
        }
        
        // Normalized novelty measure
        noveltyLevel_ = totalDifference / static_cast<float>(vision.size());
        
        // Apply decay factor
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next novelty computation
        previousVision_ = vision;
    }
    
    // Update curiosity level based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::decodeMotorCommand: Cannot decode motor command with null brain pointer");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration if enabled and level is sufficient
    if (curiosityEnabled_ && curiosityLevel_ > CURIOSITY_EXPLORATION_THRESHOLD) {
        MotorCommand curiousAction = selectWithCuriosity(decoded);
        if (curiousAction != MotorCommand::Wait) {
            decoded = curiousAction;
        }
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Helper lambda to compute neural activity for a group of neurons
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) {
                NLM_LOG_ERROR("AgentBrain::decodeFromMotorNeurons: Null neuron pointer in activity calculation");
                continue;
            }
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    // Calculate activity for each motor neuron group
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Structure to hold command activity pairs
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
    
    // Find command with highest activity
    MotorCommand bestCommand = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& cmd : commands) {
        if (cmd.activity > bestActivity) {
            bestActivity = cmd.activity;
            bestCommand = cmd.cmd;
        }
    }
    
    // Only execute command if activity exceeds threshold
    if (bestActivity < MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return bestCommand;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration logic: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > CURIOSITY_HIGH_THRESHOLD) {
        // Higher curiosity increases exploration probability
        float exploreChance = curiosityLevel_ * 0.3f;
        
        if (!brain_) {
            NLM_LOG_ERROR("AgentBrain::selectWithCuriosity: Cannot select curiosity action with null brain pointer");
            return defaultCmd;
        }
        
        // Generate random number to decide whether to explore
        float randomValue = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (randomValue < exploreChance) {
            // Random motor command selection
            int choice = brain_->getRandomGenerator()->uniformInt(0, RANDOM_MOTOR_COMMAND_MAX);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case RANDOM_INTERACTION_CHOICE: return MotorCommand::Interact;
                case 5: return MotorCommand::LookLeft;
                case 6: return MotorCommand::LookRight;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::applyRewardModulation: Cannot apply reward modulation with null brain pointer");
        return;
    }
    
    if (!rewardModulationEnabled_) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Reward modulation is disabled");
        return;
    }
    
    // Compute prediction error (difference between actual and predicted reward)
    predictionError_ = reward - predictedReward;
    
    // Update expected reward using exponential moving average
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Compute dopamine-like signal from prediction error
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, NEUROMODULATION_DOPAMINE_CLAMP_MIN, NEUROMODULATION_DOPAMINE_CLAMP_MAX);
    
    // Apply reward-modulated weight changes to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        if (!region) {
            NLM_LOG_ERROR("AgentBrain::applyRewardModulation: Null region pointer found");
            continue;
        }
        
        const auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            continue;
        }
        
        for (auto* syn : synapses) {
            if (!syn) {
                NLM_LOG_ERROR("AgentBrain::applyRewardModulation: Null synapse pointer found");
                continue;
            }
            
            float eligibility = syn->getEligibilityTrace();
            if (std::abs(eligibility) > NEUROMODULATION_ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(NEUROMODULATION_ELIGIBILITY_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine level
    // Positive dopamine increases plasticity, negative decreases it
    float plasticityFactor = NEUROMODULATION_PLASTICITY_BASE + 
                            NEUROMODULATION_PLASTICITY_MOD * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, NEUROMODULATION_STDP_CLAMP_MIN, NEUROMODULATION_STDP_CLAMP_MAX);
    
    // Apply plasticity factor to STDP parameters
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(NEUROMODULATION_STDP_LTP_BASE * plasticityFactor);
        stdp->setLTDWeight(NEUROMODULATION_STDP_LTD_BASE * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::updateDevelopment: Cannot update development with null brain pointer");
        return;
    }
    
    if (!developmentEnabled_) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment: Development is disabled");
        return;
    }
    
    // Advance developmental age
    developmentalAge_ += timestep;
    
    // Determine developmental stage based on age thresholds
    if (developmentalAge_ < DEVELOPMENT_INITIAL_AGE) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < DEVELOPMENT_CRITICAL_AGE) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < DEVELOPMENT_MATURATION_AGE) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Update structural plasticity rates based on development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (!sp) {
            NLM_LOG_ERROR("AgentBrain::updateDevelopment: Cannot get structural plasticity system");
            return;
        }
        
        // Synaptogenesis rate decreases with development (higher plasticity early)
        float synaptogenesisRate = PLASTICITY_SYNAPTOGENESIS_RATE * plasticityModifier_;
        
        // Pruning rate increases with development (more stability needed later)
        float pruningRate = PLASTICITY_PRUNING_RATE * (PLASTICITY_FACTOR_2 - plasticityModifier_);
        
        sp->setSynaptogenesisRate(synaptogenesisRate);
        sp->setPruningRate(pruningRate);
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::getDevelopmentalStage: Cannot get developmental stage with null brain pointer");
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

void AgentBrain::reset() {
    // Reset neuromodulation state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear sensory history
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
