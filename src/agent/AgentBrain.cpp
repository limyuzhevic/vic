/**
     * @brief Create agent brain interface connecting neural computation to environment
     * @details Constructs the bridge between the NLM brain and the external world,
     *          handling sensory transduction and motor command decoding. This class
     *          enables the brain to interact with and respond to environmental stimuli.
     * @param brain Shared pointer to the neural brain instance
     */
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
    // Validate brain pointer
    if (!brain_) {
        return;
    }
    
    // Initialize motor and sensory neuron groups
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) continue;
            
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    if (!n) continue;
                    
                    // Distribute motor neurons to different action groups
                    // Use safe calculation with bounds checking
                    size_t totalGroups = 8;
                    size_t currentGroupIndex = (motorForward_.size() + 
                                             motorBackward_.size() + 
                                             motorTurnLeft_.size() + 
                                             motorTurnRight_.size() + 
                                             motorLookLeft_.size() + 
                                             motorLookRight_.size() + 
                                             motorInteract_.size() + 
                                             motorWait_.size()) % totalGroups;
                    
                    switch (currentGroupIndex) {
                        case 0: motorForward_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 1: motorBackward_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 2: motorTurnLeft_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 3: motorTurnRight_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 4: motorLookLeft_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 5: motorLookRight_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 6: motorInteract_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 7: motorWait_.push_back(std::unique_ptr<Neuron>(n)); break;
                    }
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    if (!n) continue;
                    
                    // Distribute sensory neurons with safe calculation
                    size_t totalSensoryGroups = 4;
                    size_t groupIndex = (sensoryVision_.size() + 
                                       sensoryTouch_.size() + 
                                       sensoryInternal_.size() + 
                                       sensoryProprioception_.size()) % totalSensoryGroups;
                    
                    switch (groupIndex) {
                        case 0: sensoryVision_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 1: sensoryTouch_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 2: sensoryInternal_.push_back(std::unique_ptr<Neuron>(n)); break;
                        case 3: sensoryProprioception_.push_back(std::unique_ptr<Neuron>(n)); break;
                    }
                }
            }
        }
    }
}

/**
     * @brief Default destructor
     */
AgentBrain::~AgentBrain() = default;

/**
     * @brief Initialize agent with world interface
     * @details Sets up sensory processing pipeline and prepares agent for
     *          interaction with the environment. Establishes the connection
     *          between brain neural activity and world state.
     * @param world Reference to the simulation world
     */
void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

/**
     * @brief Get total sensory input size expected by brain
     * @details Returns the total number of sensory channels that will be injected
     *          into the brain. Used for validating percept size and configuring
     *          sensory transducers.
     * @return Number of sensory input values
     */
size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6) + audio (0-8)
    return 256 + 8 + 4 + 6 + 0;
}

/**
     * @brief Get total motor output size expected
     * @details Returns the number of possible motor actions the agent can produce.
     *          Used for determining action space size and implementing motor policies.
     * @return Number of motor output values
     */
size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action (including LookLeft/LookRight)
    return 8;
}

/**
     * @brief Process sensory percept and inject into brain
     * @details Converts environmental observations into neural currents, enabling
     *          the brain to perceive its surroundings. Implements sensory encoding
     *          by mapping world sensor values to neural activation patterns.
     * @param percept Sensory percept containing environmental observations
     */
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
    
    // Audio input (if enabled) - additional sensory channel for sound processing
    const auto& audio = percept.getAudio();
    // Audio neurons are processed separately for sound localization and environmental awareness
    // Audio processing is handled by dedicated audio processor in the sensory system
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && !previousVision_.empty()) {
        size_t minSize = std::min(vision.size(), previousVision_.size());
        float totalDiff = 0.0f;
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(minSize, 1);
        
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

/**
     * @brief Decode brain motor activity into motor command
     * @details Converts neural activity patterns in motor neuron populations
     *          into concrete actions. Implements action selection by comparing
     *          activity levels across different motor groups.
     * @return Selected motor command
     */
MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = getMotorCommandsFromNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectActionWithCuriosity(decoded);
    }
    
    return decoded;
}

/**
     * @brief Helper method to calculate activity in a neuron group
     * @details Computes average activation level of neurons in a population.
     *          Used for action selection and activity monitoring.
     * @param neurons Neuron population to analyze
     * @return Average activity level
     */
float AgentBrain::calcNeuronGroupActivity(const std::vector<std::unique_ptr<Neuron>>& neurons) const {
    if (neurons.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& neuron : neurons) {
        if (neuron) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
    }
    return sum / neurons.size();
}

/**
     * @brief Extract motor commands from neural activity patterns
     * @details Analyzes activity in all motor neuron groups and selects the action
     *          with highest average activation. Implements winner-take-all selection.
     * @return Most active motor command
     */
MotorCommand AgentBrain::getMotorCommandsFromNeurons() {
    // Calculate average activity in each motor group using cached helper
    float forwardAct = calcNeuronGroupActivity(motorForward_);
    float backwardAct = calcNeuronGroupActivity(motorBackward_);
    float leftAct = calcNeuronGroupActivity(motorTurnLeft_);
    float rightAct = calcNeuronGroupActivity(motorTurnRight_);
    float lookLeftAct = calcNeuronGroupActivity(motorLookLeft_);
    float lookRightAct = calcNeuronGroupActivity(motorLookRight_);
    float interactAct = calcNeuronGroupActivity(motorInteract_);
    float waitAct = calcNeuronGroupActivity(motorWait_);
    
    // Use extensible motor command mapping instead of hard-coded array
    struct MotorCommandMapping {
        MotorCommand command;
        float activity;
        bool operator<(const MotorCommandMapping& other) const { return activity > other.activity; }
    };
    
    std::vector<MotorCommandMapping> commands = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::LookLeft, lookLeftAct},
        {MotorCommand::LookRight, lookRightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find maximum activity using STL algorithm for efficiency
    std::sort(commands.begin(), commands.end());
    
    // Only act if there's meaningful activity
    if (commands.empty() || commands[0].activity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return commands[0].command;
}

/**
     * @brief Select action with curiosity-driven exploration
     * @details When curiosity is high, randomly selects from all possible actions
     *          to promote exploration of the environment. Balances exploitation
     *          with exploration based on curiosity level.
     * @param defaultCmd Default action selected by neural activity
     * @return Action (random if curiosity high, otherwise default)
     */
MotorCommand AgentBrain::selectActionWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        if (!brain_) return defaultCmd;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - extend this system to add new motor types
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

/**
     * @brief Apply reward modulation based on prediction error
     * @details Implements dopamine-based reinforcement learning by modifying
     *          synaptic plasticity based on the difference between expected and
     *          received rewards. Enables learning from environmental feedback.
     * @param reward Actual reward received from environment
     * @param predictedReward Expected reward based on current policy
     */
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Validate input parameters
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        return;
    }
    
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
        if (!region) continue;
        
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                // Ensure delta is finite
                if (std::isfinite(delta)) {
                    syn->addToWeight(delta);
                }
                
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
    if (brain_) {
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
}

/**
     * @brief Update developmental processes
     * @details Models biological development where neural systems mature over time.
     *          Simulates changing plasticity levels, synaptic strengths, and
     *          behavioral strategies as developmental age increases.
     * @param timestep Time step increment
     */
void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    // Validate timestep
    if (std::isnan(timestep) || timestep < 0.0) {
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        }
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        }
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        }
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_ && brain_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            
            // Ensure rates are valid
            synRate = std::max(0.0f, std::min(synRate, 0.001f));
            pruneRate = std::max(0.0f, std::min(pruneRate, 0.0001f));
            
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

/**
     * @brief Get current developmental stage
     * @return Current developmental stage of the agent
     */
DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

/**
     * @brief Get neuromodulation level
     * @return Current dopamine level (-1 to 1)
     */
float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

/**
     * @brief Get curiosity level
     * @return Current curiosity level (0 to 1)
     */
float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

/**
     * @brief Get novelty level
     * @return Current novelty level (0 to 1)
     */
float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

/**
     * @brief Get prediction error
     * @return Current prediction error (-1 to 1)
     */
float AgentBrain::getPredictionError() const {
    return predictionError_;
}

/**
     * @brief Reset agent for new episode
     * @details Clears all neuromodulation state, novelty, and developmental age.
     *          Resets for running a new independent simulation episode.
     */
void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision with bounds checking
    if (!previousVision_.empty()) {
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
}

} // namespace nlm