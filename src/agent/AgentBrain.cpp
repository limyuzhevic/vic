AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , conceptFormation_()
    , conceptFormationEnabled_(true)
    , neuralPlanner_()
    , neuralPlannerEnabled_(true)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(false)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.95f)
{
    // Initialize concept formation if enabled
    if (conceptFormationEnabled_) {
        conceptFormation_.initialize(brain_.get());
    }
    
    // Initialize neural planner if enabled
    if (neuralPlannerEnabled_) {
        neuralPlanner_.initialize(brain_.get());
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize concept formation
    if (conceptFormationEnabled_) {
        conceptFormation_.initialize(brain_.get());
    }
    
    // Initialize neural planner
    if (neuralPlannerEnabled_) {
        neuralPlanner_.initialize(brain_.get());
    }
    
    // Continue with existing initialization...
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Create a SensoryInput from SensoryPercept for working memory integration
    class SensoryInputForWorkingMemory : public SensoryInput {
    public:
        SensoryInputForWorkingMemory(const SensoryPercept& percept) {
            // Combine all sensory data into one vector for the brain
            std::vector<float> allSignals;
            
            // Add vision data (flattened)
            const auto& vision = percept.getVision();
            allSignals.insert(allSignals.end(), vision.begin(), vision.end());
            
            // Add touch data
            const auto& touch = percept.getTouch();
            allSignals.insert(allSignals.end(), touch.begin(), touch.end());
            
            // Add internal data
            const auto& internal = percept.getInternal();
            allSignals.insert(allSignals.end(), internal.begin(), internal.end());
            
            // Add proprioception data
            const auto& proprio = percept.getProprioception();
            allSignals.insert(allSignals.end(), proprio.begin(), proprio.end());
            
            // Create InternalSignals object for working memory integration
            // This is more appropriate than Vision for working memory
            internalInput_ = std::make_unique<InternalSignals>();
            for (float val : allSignals) {
                internalInput_->addSignal(val);
            }
            
            timestamp_ = percept.getTimestamp();
        }
        
        const char* getType() const override {
            return "Internal";
        }
        
        const std::vector<float>& getData() const override {
            return internalInput_->getData();
        }
        
        size_t getDimensions() const override {
            return internalInput_->getDimensions();
        }
        
        std::unique_ptr<SensoryInput> clone() const override {
            auto clone = std::make_unique<SensoryInputForWorkingMemory>(*this);
            clone->timestamp_ = timestamp_;
            return clone;
        }
        
    private:
        std::unique_ptr<InternalSignals> internalInput_;
    };
    
    // Create SensoryInput and pass to brain for working memory integration
    if (brain_) {
        // Convert SensoryPercept to SensoryInput for working memory integration
        auto sensoryInput = std::make_unique<SensoryInputForWorkingMemory>(percept);
        
        // Pass sensory input to brain (this triggers working memory updates)
        brain_->receiveSensoryInput(*sensoryInput);
        
        // Also ensure working memory is updated after sensory input
        if (brain_->getWorkingMemory()) {
            // Get current timestep from spike system
            if (brain_->getSpikeSystem()) {
                brain_->getWorkingMemory()->update(brain_->getSpikeSystem()->getTimestep());
            }
        }
    }
    
    // Store previous vision for novelty detection
    const auto& vision = percept.getVision();
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
        
        // Clamp novelty to reasonable range
        noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // NEW: Process concepts from sensory input for planning
    if (conceptFormationEnabled_) {
        // Create features for concept formation from sensory input
        std::vector<float> sensoryInputFeatures;
        const auto& vision = percept.getVision();
        const auto& touch = percept.getTouch();
        const auto& internal = percept.getInternal();
        const auto& proprio = percept.getProprioception();
        
        // Combine all sensory features
        sensoryInputFeatures.insert(sensoryInputFeatures.end(), vision.begin(), vision.end());
        sensoryInputFeatures.insert(sensoryInputFeatures.end(), touch.begin(), touch.end());
        sensoryInputFeatures.insert(sensoryInputFeatures.end(), internal.begin(), internal.end());
        sensoryInputFeatures.insert(sensoryInputFeatures.end(), proprio.begin(), proprio.end());
        
        // Process concepts using current reward
        processConcepts(sensoryInputFeatures, predictionError_);
        
        // Update neural planner with new concepts for planning
        if (neuralPlannerEnabled_ && !getConcepts().empty()) {
            // Set current goal based on best matching concept
            std::vector<float> bestConcept = getConcepts()[getBestConcept(sensoryInputFeatures)].pattern;
            setCurrentGoal(bestConcept);
        }
    }

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Plan action based on current state if neural planner is enabled
    MotorCommand plannedAction = ActionType::Wait;
    if (neuralPlannerEnabled_) {
        // Create a state representation from sensory input for planning
        std::vector<float> currentState;
        if (!sensoryVision_.empty()) {
            // Use vision sensory data as state for planning
            currentState = sensoryVision_;
        }
        
        if (!currentState.empty()) {
            // Get target reward from neuromodulation
            float targetReward = predictionError_ > 0.0f ? 0.8f : 0.2f;
            
            // Plan action based on current state
            plannedAction = planAction(currentState, targetReward);
        }
    }
    
    // Use planned action if available, otherwise fall back to neural decoding
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply planned action if it makes sense
    if (plannedAction != ActionType::Wait && plannedAction != decoded) {
        // If planner suggests a different action than neural decoding,
        // use the planned action (goals can override automatic responses)
        return plannedAction;
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    if (!brain_) return MotorCommand::Wait;
    
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (const Neuron* n : neurons) {
            // Null check before accessing neuron state
            if (!n) continue;
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
    if (!brain_ || !curiosityEnabled_) return defaultCmd;
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        // Ensure exploreChance is in valid range
        exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
        
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
    
    // Validate input values
    reward = std::isfinite(reward) ? reward : 0.0f;
    predictedReward = std::isfinite(predictedReward) ? predictedReward : 0.0f;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Clamp prediction error to reasonable range
    predictionError_ = std::clamp(predictionError_, -10.0f, 10.0f);
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Clamp expected reward to reasonable range
    expectedReward_ = std::clamp(expectedReward_, -1.0f, 1.0f);
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp dopamine level to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        for (auto* syn : region->getSynapses()) {
            // Null check for synapse
            if (!syn) continue;
            
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
    
    // Validate timestep
    if (!std::isfinite(timestep) || timestep < 0.0) {
        timestep = 0.0;
    }
    
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
    
    // Clamp plasticity modifier to reasonable range
    plasticityModifier_ = std::clamp(plasticityModifier_, 0.1f, 2.0f);
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            
            // Clamp rates to reasonable ranges
            synRate = std::clamp(synRate, 0.0f, 0.0001f);
            pruneRate = std::clamp(pruneRate, 0.0f, 0.0001f);
            
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

// Process concepts from sensory input and working memory
    void AgentBrain::processConcepts(const std::vector<float>& sensoryInput, float reward) {
        if (!conceptFormationEnabled_ || !brain_) return;
        
        // Convert sensory input to features for concept formation
        std::vector<float> features = sensoryInput;
        
        // Process concepts based on current sensory experience
        conceptFormation_.presentExperience(sensoryInput, features, reward, 
                                            brain_->getCurrentTimeStep());
    }
    
    // Get current concepts for planning and decision making
    const std::vector<nlm::DiscoveredConcept>& AgentBrain::getConcepts() const {
        return conceptFormation_.getConcepts();
    }
    
    // Get best concept matching pattern
    size_t AgentBrain::getBestConcept(const std::vector<float>& pattern) const {
        return conceptFormation_.getMatchingConcept(pattern);
    }
    
    // Clear concepts
    void AgentBrain::clearConcepts() {
        conceptFormation_.clear();
    }
    
    // NEW: Neural planner integration
    // Plan action based on current state and goals
    ActionType AgentBrain::planAction(const std::vector<float>& currentState,
                                    float targetReward) {
        if (!neuralPlannerEnabled_ || !brain_ || !brain_->getPlanner()) {
            return ActionType::Wait;
        }
        
        // Get current concepts for planning context
        const auto& concepts = getConcepts();
        
        // If we have concepts, use them to set a goal for planning
        if (!concepts.empty()) {
            // Use the best matching concept as planning goal
            std::vector<float> goal = concepts[getBestConcept(currentState)].pattern;
            brain_->getPlanner()->setCurrentGoal(goal);
        }
        
        // Plan the next action
        return brain_->getPlanner()->planAction(currentState, targetReward);
    }
    
    // Get current goal for planning
    std::vector<float> AgentBrain::getCurrentGoal() const {
        if (!neuralPlannerEnabled_ || !brain_ || !brain_->getPlanner()) {
            return std::vector<float>();
        }
        return brain_->getPlanner()->getCurrentGoal();
    }
    
    // Set current goal for planning
    void AgentBrain::setCurrentGoal(const std::vector<float>& goal) {
        if (!neuralPlannerEnabled_ || !brain_ || !brain_->getPlanner()) {
            return;
        }
        brain_->getPlanner()->setCurrentGoal(goal);
    }
    
    // Get planning confidence
    float AgentBrain::getPlanningConfidence() const {
        if (!neuralPlannerEnabled_ || !brain_ || !brain_->getPlanner()) {
            return 0.0f;
        }
        return brain_->getPlanner()->getPlanningConfidence();
    }
    
    // Clear planning cache
    void AgentBrain::clearPlanningCache() {
        if (!neuralPlannerEnabled_ || !brain_ || !brain_->getPlanner()) {
            return;
        }
        brain_->getPlanner()->clearCache();
    }
    
    // Process concepts from sensory input and working memory
    void AgentBrain::processConceptsInternal(const std::vector<float>& sensoryInput, float reward) {
        processConcepts(sensoryInput, reward);
    }
    
    // Get current developmental stage
    DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
        if (!brain_) return DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
    }
    
    // Get neuromodulation level
    float AgentBrain::getNeuromodulationLevel() const {
        return dopamineLevel_;
    }
    
    // Get curiosity level
    float AgentBrain::getCuriosityLevel() const {
        return curiosityLevel_;
    }
    
    // Get novelty level
    float AgentBrain::getNoveltyLevel() const {
        return noveltyLevel_;
    }
    
    // Get prediction error
    float AgentBrain::getPredictionError() const {
        return predictionError_;
    }
    
    // Reset agent for new episode
    void AgentBrain::reset() {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Clear previous vision
        if (!previousVision_.empty()) {
            std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        }
        
        // Ensure other neuron vectors are properly initialized
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
        
        // Clear concept formation
        if (conceptFormationEnabled_) {
            conceptFormation_.clear();
        }
        
        // Clear neural planner
        if (neuralPlannerEnabled_) {
            neuralPlanner_.clearCache();
        }
    }

} // namespace nlm
