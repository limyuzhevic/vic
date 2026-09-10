namespace nlm {

// Performance monitoring constants
namespace Performance {
    constexpr double DEFAULT_NOVELTY_DECAY = 0.99f;
    constexpr float DEFAULT_CURIOUS_EXPLORATION_THRESHOLD = 0.3f;
    constexpr float DEFAULT_MAX_CURIOUS_LEVEL = 1.0f;
    constexpr float DEFAULT_HIGH_CURIOUS_THRESHOLD = 0.5f;
    constexpr float DEFAULT_EXPLORATION_CHANCE = 0.3f;
    constexpr float DEFAULT_MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_ACTION_STRENGTH = 0.75f;
    constexpr size_t DEFAULT_ACTION_NEIGHBORS = 5;
    constexpr double DEFAULT_DEVELOPMENT_INITIAL_TIME = 60.0;
    constexpr double DEFAULT_DEVELOPMENT_CRITICAL_TIME = 300.0;
    constexpr double DEFAULT_DEVELOPMENT_MATURATION_TIME = 900.0;
    constexpr float DEFAULT_PLASTICITY_INITIAL = 1.0f;
    constexpr float DEFAULT_PLASTICITY_CRITICAL = 0.8f;
    constexpr float DEFAULT_PLASTICITY_MATURATION = 0.5f;
    constexpr float DEFAULT_PLASTICITY_ADULT = 0.2f;
    constexpr float DEFAULT_DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DEFAULT_DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float DEFAULT_ELIGIBILITY_DECAY = 0.1f;
    constexpr float DEFAULT_PREDICTION_ERROR_WINDOW = 0.95f;
    constexpr float DEFAULT_PREDICTION_UPDATE = 0.05f;
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    constexpr float DEFAULT_REWARD_BASELINE = 0.0f;
    constexpr float DEFAULT_REWARD_STANDARD = 1.0f;
    constexpr float DEFAULT_REWARD_NEGATIVE = -1.0f;
    constexpr double DEFAULT_REWARD_PREDICTION_DECAY = 0.99;
    constexpr double DEFAULT_REWARD_PREDICTION_ACCURACY = 0.01;
    constexpr double DEFAULT_SIMULATION_TOLERANCE = 1e-6;
    constexpr size_t DEFAULT_SIMULATION_MAX_STEPS = 1000000;
    constexpr double DEFAULT_SIMULATION_TIMESTEP_MIN = 1e-6;
    constexpr double DEFAULT_SIMULATION_TIMESTEP_MAX = 0.01;
    constexpr size_t DEFAULT_OBSERVATION_LIMBO_DEFAULT = 256;
    constexpr size_t DEFAULT_OBSERVATION_LIMBO_MAX = 1000;
    constexpr size_t DEFAULT_OBSERVATION_LIMBO_MIN = 1;
    constexpr float DEFAULT_PREDICTION_CONFIDENCE_MIN = 0.0f;
    constexpr float DEFAULT_PREDICTION_CONFIDENCE_MAX = 1.0f;
    constexpr float DEFAULT_PREDICTION_ERROR_THRESHOLD = 0.1f;
    constexpr float DEFAULT_PREDICTION_ERROR_WEIGHT = 0.5f;
    constexpr float DEFAULT_CONCEPT_FORMATION_THRESHOLD = 0.7f;
    constexpr float DEFAULT_CONCEPT_FORMATION_SIMILARITY = 0.8f;
    constexpr size_t DEFAULT_CONCEPT_FORMATION_MAX_CONCEPTS = 100;
    constexpr size_t DEFAULT_CONCEPT_FORMATION_MIN_CONCEPTS = 1;
    constexpr size_t DEFAULT_ATTENTION_FOCUS_MAX = 5;
    constexpr size_t DEFAULT_ATTENTION_FOCUS_MIN = 1;
    constexpr float DEFAULT_ATTENTION_FOCUS_THRESHOLD = 0.5f;
    constexpr float DEFAULT_ATTENTION_FOCUS_DEFAULT = 0.75f;
    constexpr size_t DEFAULT_PLANNING_ACTION_SEQUENCE_MAX = 10;
    constexpr size_t DEFAULT_PLANNING_ACTION_SEQUENCE_MIN = 1;
    constexpr float DEFAULT_PLANNING_SEQUENCE_QUALITY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_PLANNING_SEQUENCE_QUALITY_DEFAULT = 0.7f;
    constexpr float DEFAULT_CURIOSITY_BASELINE = 0.0f;
    constexpr float DEFAULT_CURIOSITY_MAX = 1.0f;
    constexpr float DEFAULT_CURIOSITY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_CURIOSITY_DECAY = 0.99f;
    constexpr float DEFAULT_CURIOSITY_NOVELTY_FACTOR = 2.0f;
    constexpr float DEFAULT_CURIOSITY_PREDICTION_ERROR_FACTOR = 0.5f;
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    constexpr float DEFAULT_NOVELTY_WEIGHT = 1.0f;
    constexpr float DEFAULT_NEUROMODULATION_DOPAMINE_POSITIVE = 1.0f;
    constexpr float DEFAULT_NEUROMODULATION_DOPAMINE_NEGATIVE = -1.0f;
    constexpr float DEFAULT_NEUROMODULATION_NOVELTY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_NEUROMODULATION_NOVELTY_POSITIVE = 1.0f;
    constexpr float DEFAULT_NEUROMODULATION_NOVELTY_NEGATIVE = -0.5f;
    constexpr float DEFAULT_NEUROMODULATION_CURIOUS_THRESHOLD = 0.5f;
    constexpr float DEFAULT_NEUROMODULATION_CURIOUS_POSITIVE = 1.0f;
    constexpr float DEFAULT_NEUROMODULATION_CURIOUS_NEGATIVE = -0.5f;
    constexpr float DEFAULT_NEUROMODULATION_PREDICTION_ERROR_THRESHOLD = 0.5f;
    constexpr float DEFAULT_NEUROMODULATION_PREDICTION_ERROR_POSITIVE = 1.0f;
    constexpr float DEFAULT_NEUROMODULATION_PREDICTION_ERROR_NEGATIVE = -0.5f;
    constexpr size_t DEFAULT_MEMORY_CAPACITY = 10000;
    constexpr size_t DEFAULT_MEMORY_ACTIVE = 100;
    constexpr size_t DEFAULT_MEMORY_RECENT = 50;
    constexpr size_t DEFAULT_MEMORY_ASSOCIATIVE = 500;
    constexpr float DEFAULT_MEMORY_DECAY = 0.99f;
    constexpr float DEFAULT_MEMORY_STRENGTH = 1.0f;
    constexpr float DEFAULT_MEMORY_THRESHOLD = 0.5f;
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 50;
    constexpr size_t DEFAULT_WORKING_MEMORY_WINDOW = 20;
    constexpr float DEFAULT_WORKING_MEMORY_DECAY = 0.95f;
    constexpr float DEFAULT_WORKING_MEMORY_ACTIVATION_THRESHOLD = 0.1f;
    constexpr float DEFAULT_WORKING_MEMORY_ACTIVATION_DEFAULT = 0.5f;
    constexpr size_t DEFAULT_Episodic_MEMORY_CAPACITY = 1000;
    constexpr size_t DEFAULT_Episodic_MEMORY_REPLAY = 10;
    constexpr float DEFAULT_Episodic_MEMORY_STRENGTH = 1.0f;
    constexpr float DEFAULT_Episodic_MEMORY_DECAY = 0.98f;
    constexpr float DEFAULT_Episodic_MEMORY_THRESHOLD = 0.5f;
    constexpr size_t DEFAULT_SEMANTIC_MEMORY_CAPACITY = 5000;
    constexpr size_t DEFAULT_SEMANTIC_MEMORY_ASSOCIATIONS = 100;
    constexpr float DEFAULT_SEMANTIC_MEMORY_STRENGTH = 1.0f;
    constexpr float DEFAULT_SEMANTIC_MEMORY_DECAY = 0.97f;
    constexpr float DEFAULT_SEMANTIC_MEMORY_THRESHOLD = 0.3f;
    constexpr size_t DEFAULT_PROCEDURAL_MEMORY_CAPACITY = 1000;
    constexpr size_t DEFAULT_PROCEDURAL_MEMORY_SEQUENCE_LENGTH = 10;
    constexpr float DEFAULT_PROCEDURAL_MEMORY_STRENGTH = 1.0f;
    constexpr float DEFAULT_PROCEDURAL_MEMORY_DECAY = 0.96f;
    constexpr float DEFAULT_PROCEDURAL_MEMORY_THRESHOLD = 0.4f;
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_CAPACITY = 5000;
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_PATTERNS = 100;
    constexpr float DEFAULT_ASSOCIATIVE_MEMORY_STRENGTH = 1.0f;
    constexpr float DEFAULT_ASSOCIATIVE_MEMORY_DECAY = 0.98f;
    constexpr float DEFAULT_ASSOCIATIVE_MEMORY_THRESHOLD = 0.6f;
    constexpr size_t DEFAULT_PERFORMANCE_BUFFER_SIZE = 1024;
    constexpr size_t DEFAULT_PERFORMANCE_FRAMEWORK = 64;
    constexpr size_t DEFAULT_PERFORMANCE_ITERATIONS = 1000;
    constexpr float DEFAULT_PERFORMANCE_MIN_TIME = 1e-6;
    constexpr float DEFAULT_PERFORMANCE_MAX_TIME = 1.0f;
    constexpr size_t DEFAULT_PERFORMANCE_MEMORY_POOL = 1000000;
    constexpr size_t DEFAULT_PERFORMANCE_EVENT_QUEUE = 10000;
    constexpr size_t DEFAULT_CHECKPOINT_VERSION = 1;
    constexpr size_t DEFAULT_CHECKPOINT_MAGIC = 0x4E4C4D00;  // 'NLM\0'
    constexpr size_t DEFAULT_CHECKPOINT_SIZE = 4096;
    constexpr float DEFAULT_CHECKPOINT_COMPRESSION_LEVEL = 6.0f;
    constexpr float DEFAULT_CHECKPOINT_ENCRYPTION_KEY = 0xCAFEBABE;
    constexpr float DEFAULT_VISUALIZATION_MIN_COORD = -10.0f;
    constexpr float DEFAULT_VISUALIZATION_MAX_COORD = 10.0f;
    constexpr size_t DEFAULT_VISUALIZATION_MIN_SIZE = 1;
    constexpr size_t DEFAULT_VISUALIZATION_MAX_SIZE = 100;
    constexpr float DEFAULT_VISUALIZATION_MIN_OPACITY = 0.1f;
    constexpr float DEFAULT_VISUALIZATION_MAX_OPACITY = 1.0f;
    constexpr size_t DEFAULT_VISUALIZATION_BUFFER_SIZE = 1024;
}

// Create singleton instance for easy access
AgentBrain* AgentBrain::singletonInstance = nullptr;

// Static helper functions
namespace AgentBrainUtils {
    // Calculate motor neuron activity
    float calculateMotorActivity(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    }
    
    // Determine action from activity levels
    MotorCommand determineActionFromActivity(float forward, float backward, float left, float right, float interact, float wait) {
        struct { MotorCommand cmd; float activity; } commands[] = {
            {MotorCommand::MoveForward, forward},
            {MotorCommand::MoveBackward, backward},
            {MotorCommand::TurnLeft, left},
            {MotorCommand::TurnRight, right},
            {MotorCommand::Interact, interact},
            {MotorCommand::Wait, wait}
        };
        
        MotorCommand best = MotorCommand::Wait;
        float bestActivity = wait;
        
        for (const auto& c : commands) {
            if (c.activity > bestActivity) {
                bestActivity = c.activity;
                best = c.cmd;
            }
        }
        
        if (bestActivity < 0.5f) {
            return MotorCommand::Wait;
        }
        
        return best;
    }
    
    // Calculate curiosity level based on novelty and prediction error
    float calculateCuriosity(float novelty, float predictionError) {
        float curiosity = novelty * 2.0f + std::abs(predictionError) * 0.5f;
        return std::clamp(curiosity, 0.0f, 1.0f);
    }
    
    // Calculate development plasticity modifier
    float calculatePlasticityModifier(double developmentalAge) {
        if (developmentalAge < 60.0) {
            return 1.0f;
        } else if (developmentalAge < 300.0) {
            return 0.8f;
        } else if (developmentalAge < 900.0) {
            return 0.5f;
        } else {
            return 0.2f;
        }
    }
    
    // Calculate developmental stage
    DevelopmentalStage getDevelopmentalStage(double developmentalAge) {
        if (developmentalAge < 60.0) {
            return DevelopmentalStage::Initial;
        } else if (developmentalAge < 300.0) {
            return DevelopmentalStage::CriticalPeriod;
        } else if (developmentalAge < 900.0) {
            return DevelopmentalStage::Maturation;
        } else {
            return DevelopmentalStage::Adult;
        }
    }
}

// Implementation of improved AgentBrain methods
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input processing with optimized bounds checking
    const auto& vision = percept.getVision();
    size_t visionLimit = std::min(sensoryVision_.size(), vision.size());
    for (size_t i = 0; i < visionLimit; ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * Performance::DEFAULT_SENSE_CURRENT_SCALE_FACTOR;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input processing
    const auto& touch = percept.getTouch();
    size_t touchLimit = std::min(sensoryTouch_.size(), touch.size());
    for (size_t i = 0; i < touchLimit; ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * Performance::DEFAULT_SENSE_TOUCH_SCALE_FACTOR;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals processing
    const auto& intern = percept.getInternal();
    size_t internalLimit = std::min(sensoryInternal_.size(), intern.size());
    for (size_t i = 0; i < internalLimit; ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * Performance::DEFAULT_SENSE_INTERNAL_SCALE_FACTOR;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception processing
    const auto& proprio = percept.getProprioception();
    size_t proprioLimit = std::min(sensoryProprioception_.size(), proprio.size());
    for (size_t i = 0; i < proprioLimit; ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * Performance::DEFAULT_SENSE_PROPRIOCEPTION_SCALE_FACTOR;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Novelty detection with improved algorithm
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        size_t diffLimit = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < diffLimit; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty with enhanced algorithm
    if (curiosityEnabled_) {
        curiosityLevel_ = AgentBrainUtils::calculateCuriosity(noveltyLevel_, std::abs(predictionError_));
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > Performance::DEFAULT_CURIOUS_EXPLORATION_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using optimized helper
    float forwardAct = AgentBrainUtils::calculateMotorActivity(motorForward_);
    float backwardAct = AgentBrainUtils::calculateMotorActivity(motorBackward_);
    float leftAct = AgentBrainUtils::calculateMotorActivity(motorTurnLeft_);
    float rightAct = AgentBrainUtils::calculateMotorActivity(motorTurnRight_);
    float interactAct = AgentBrainUtils::calculateMotorActivity(motorInteract_);
    float waitAct = AgentBrainUtils::calculateMotorActivity(motorWait_);
    
    return AgentBrainUtils::determineActionFromActivity(
        forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct);
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > Performance::DEFAULT_HIGH_CURIOUS_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * Performance::DEFAULT_EXPLORATION_CHANCE;
        
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
    expectedReward_ = Performance::DEFAULT_REWARD_PREDICTION_ERROR_WINDOW * expectedReward_ + 
                     Performance::DEFAULT_PREDICTION_UPDATE * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, Performance::DEFAULT_DOPAMINE_CLAMP_MIN, 
                               Performance::DEFAULT_DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(Performance::DEFAULT_ELIGIBILITY_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    float plasticityFactor = Performance::DEFAULT_PLASTICITY_INITIAL + 
                            Performance::DEFAULT_PLASTICITY_ADULT * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, Performance::DEFAULT_PLASTICITY_MIN, 
                                  Performance::DEFAULT_PLASTICITY_MAX);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(Performance::DEFAULT_STDP_LTP_WEIGHT * plasticityFactor);
        stdp->setLTDWeight(Performance::DEFAULT_STDP_LTD_WEIGHT * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Use optimized helper for development stage calculation
    DevelopmentalStage stage = AgentBrainUtils::getDevelopmentalStage(developmentalAge_);
    brain_->setDevelopmentalStage(stage);
    
    // Calculate plasticity modifier
    plasticityModifier_ = AgentBrainUtils::calculatePlasticityModifier(developmentalAge_);
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = Performance::DEVELOPMENT_INITIAL_SYNAPTOGENESIS_RATE * plasticityModifier_;
            float pruneRate = Performance::DEVELOPMENT_INITIAL_PRUNING_RATE * (Performance::DEFAULT_PLASTICITY_MAX - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void AgentBrain::reset() {
    dopamineLevel_ = Performance::DEFAULT_REWARD_BASELINE;
    noveltyLevel_ = Performance::DEFAULT_REWARD_BASELINE;
    curiosityLevel_ = Performance::DEFAULT_CURIOSITY_BASELINE;
    predictionError_ = Performance::DEFAULT_REWARD_BASELINE;
    expectedReward_ = Performance::DEFAULT_REWARD_BASELINE;
    developmentalAge_ = Performance::DEFAULT_DEVELOPMENT_INITIAL_TIME;
    plasticityModifier_ = Performance::DEFAULT_PLASTICITY_INITIAL;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// Factory method for creating AgentBrain instances
std::unique_ptr<AgentBrain> AgentBrain::createAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_unique<AgentBrain>(std::move(brain));
}

// Static method to get singleton instance
AgentBrain* AgentBrain::getInstance() {
    return singletonInstance;
}

// Set singleton instance
void AgentBrain::setInstance(AgentBrain* instance) {
    singletonInstance = instance;
}

// Enhanced logging methods
void AgentBrain::logDevelopmentStatus() const {
    NLM_LOG_INFO("Development Status:");
    NLM_LOG_INFO("  Age: " + std::to_string(developmentalAge_) + "s");
    NLM_LOG_INFO("  Stage: " + std::to_string(static_cast<int>(getDevelopmentalStage())));
    NLM_LOG_INFO("  Plasticity Modifier: " + std::to_string(plasticityModifier_));
    NLM_LOG_INFO("  Neuromodulation Level: " + std::to_string(dopamineLevel_));
}

void AgentBrain::logCuriosityStatus() const {
    NLM_LOG_INFO("Curiosity Status:");
    NLM_LOG_INFO("  Novelty Level: " + std::to_string(noveltyLevel_));
    NLM_LOG_INFO("  Curiosity Level: " + std::to_string(curiosityLevel_));
    NLM_LOG_INFO("  Prediction Error: " + std::to_string(predictionError_));
    NLM_LOG_INFO("  Exploration Enabled: " + std::to_string(curiosityEnabled_));
}

// Enhanced configuration methods
void AgentBrain::configureForPerformance(float targetPerformance) {
    // Adjust parameters based on target performance
    curiosityEnabled_ = targetPerformance > 0.7f;
    rewardModulationEnabled_ = targetPerformance > 0.5f;
    developmentEnabled_ = targetPerformance > 0.3f;
    structuralPlasticityEnabled_ = targetPerformance > 0.4f;
    
    // Set appropriate novelty decay based on performance target
    sensoryNoveltyDecay_ = 0.95f + 0.05f * (1.0f - targetPerformance);
}

// Performance metrics collection
void AgentBrain::collectPerformanceMetrics(PerformanceMetrics& metrics) const {
    metrics.totalSpikes = brain_->getTotalSpikeCount();
    metrics.averageFiringRate = metrics.totalSpikes / std::max<size_t>(metrics.totalNeurons, 1);
    metrics.energyConsumption = metrics.totalSpikes * 0.01f;
    metrics.memoryUsage = metrics.totalNeurons * 1000; // Rough estimate
    metrics.sensoryEvents = 0; // Would need to track this
    metrics.motorCommands = 0; // Would need to track this
    
    // Calculate component times (simplified)
    metrics.componentTimes["sensory"] = 0.2 * metrics.totalSimulationTime;
    metrics.componentTimes["neural"] = 0.5 * metrics.totalSimulationTime;
    metrics.componentTimes["motor"] = 0.2 * metrics.totalSimulationTime;
    metrics.componentTimes["plasticity"] = 0.1 * metrics.totalSimulationTime;
}

} // namespace nlm

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
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
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
