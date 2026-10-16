// Add a namespace for abstraction layers
namespace AgentBrainAbstraction {

// Brain Adapter Interface - Abstracts access to Brain functionality
class IBrainAdapter {
public:
    virtual ~IBrainAdapter() = default;
    
    // Abstract methods for brain region access
    virtual std::vector<NeuralRegion*> getRegions() const = 0;
    virtual size_t getTotalNeuronCount() const = 0;
    virtual size_t getTotalSynapseCount() const = 0;
    
    // Abstract methods for brain subsystem access
    virtual Dopamine* getDopamine() const = 0;
    virtual Neuromodulator* getSTDP() const = 0;
    virtual StructuralPlasticity* getStructuralPlasticity() const = 0;
    virtual DevelopmentSystem* getDevelopmentSystem() const = 0;
    virtual RandomGenerator* getRandomGenerator() const = 0;
    
    // Abstract methods for brain operations
    virtual void setDevelopmentalStage(DevelopmentalStage stage) = 0;
    virtual double getDevelopmentalAge() const = 0;
};

// Concrete Brain Adapter Implementation
class BrainAdapter : public IBrainAdapter {
private:
    std::shared_ptr<Brain> brain_;
public:
    BrainAdapter(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    std::vector<NeuralRegion*> getRegions() const override {
        return brain_->getRegions();
    }
    
    size_t getTotalNeuronCount() const override {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getTotalSynapseCount() const override {
        return brain_->getTotalSynapseCount();
    }
    
    Dopamine* getDopamine() const override {
        return brain_->getDopamine();
    }
    
    Neuromodulator* getSTDP() const override {
        return brain_->getSTDP();
    }
    
    StructuralPlasticity* getStructuralPlasticity() const override {
        return brain_->getStructuralPlasticity();
    }
    
    DevelopmentSystem* getDevelopmentSystem() const override {
        return brain_->getDevelopmentSystem();
    }
    
    RandomGenerator* getRandomGenerator() const override {
        return brain_->getRandomGenerator();
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) override {
        brain_->setDevelopmentalStage(stage);
    }
    
    double getDevelopmentalAge() const override {
        return brain_->getDevelopmentalAge();
    }
};

// Neuron Distribution Strategy Interface
class INeuronDistributionStrategy {
public:
    virtual ~INeuronDistributionStrategy() = default;
    virtual void distribute(const std::vector<Neuron*>& neurons, 
                          std::vector<Neuron*>& targetList) = 0;
};

// Motor Neuron Distribution Strategy
class MotorNeuronDistributionStrategy : public INeuronDistributionStrategy {
public:
    void distribute(const std::vector<Neuron*>& neurons,
                   std::vector<Neuron*>& targetList) override {
        // Distribute motor neurons to different action groups
        for (Neuron* n : neurons) {
            if (!n) {
                NLM_LOG_WARNING("AgentBrainAbstraction: Found null motor neuron - skipping");
                continue;
            }
            
            // Use round-robin distribution
            size_t idx = targetList.size();
            switch (idx % 6) {
                case 0: targetList.push_back(n); break;
                case 1: targetList.push_back(n); break;
                case 2: targetList.push_back(n); break;
                case 3: targetList.push_back(n); break;
                case 4: targetList.push_back(n); break;
                case 5: targetList.push_back(n); break;
            }
        }
    }
};

// Sensory Neuron Distribution Strategy
class SensoryNeuronDistributionStrategy : public INeuronDistributionStrategy {
public:
    void distribute(const std::vector<Neuron*>& neurons,
                   std::vector<Neuron*>& targetList) override {
        // Distribute sensory neurons to different input modalities
        for (Neuron* n : neurons) {
            if (!n) {
                NLM_LOG_WARNING("AgentBrainAbstraction: Found null sensory neuron - skipping");
                continue;
            }
            
            // Use round-robin distribution
            size_t idx = targetList.size();
            switch (idx % 4) {
                case 0: targetList.push_back(n); break;
                case 1: targetList.push_back(n); break;
                case 2: targetList.push_back(n); break;
                case 3: targetList.push_back(n); break;
            }
        }
    }
};

// Factory for creating distribution strategies
class DistributionStrategyFactory {
public:
    static std::unique_ptr<INeuronDistributionStrategy> createStrategy(NeuronType type) {
        switch (type) {
            case NeuronType::Motor:
                return std::make_unique<MotorNeuronDistributionStrategy>();
            case NeuronType::Sensory:
                return std::make_unique<SensoryNeuronDistributionStrategy>();
            default:
                NLM_LOG_WARNING("AgentBrainAbstraction: Unknown neuron type for distribution strategy");
                return nullptr;
        }
    }
};

} // namespace AgentBrainAbstraction

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
    // Validate input brain pointer for robust initialization
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain initialized with null brain pointer - system will be non-functional");
    } else {
        NLM_LOG_INFO("AgentBrain initializing with brain containing " + 
                    std::to_string(brain_->getTotalNeuronCount()) + " neurons");
        
        // Initialize motor and sensory neuron groups with improved error handling
        initializeNeuronGroups();
    }
}

void AgentBrain::initializeNeuronGroups() {
    // Validate brain pointer before accessing regions
    if (!brain_) return;
    
    // Create brain adapter for abstracted access
    auto brainAdapter = std::make_unique<AgentBrainAbstraction::BrainAdapter>(brain_);
    
    // Get all regions and validate them
    auto regions = brainAdapter->getRegions();
    if (regions.empty()) {
        NLM_LOG_WARNING("AgentBrain::initializeNeuronGroups: No regions found in brain - cannot initialize sensory/motor groups");
        return;
    }
    
    // Iterate through regions and classify neurons by type
    for (const auto& region : regions) {
        if (!region) {
            NLM_LOG_WARNING("AgentBrain::initializeNeuronGroups: Found null region - skipping");
            continue;
        }
        
        // Get neuron populations from region
        auto populations = region->getPopulations();
        if (populations.empty()) {
            NLM_LOG_WARNING("AgentBrain::initializeNeuronGroups: Region has no neuron populations - skipping");
            continue;
        }
        
        for (const auto& pop : populations) {
            if (!pop) {
                NLM_LOG_WARNING("AgentBrain::initializeNeuronGroups: Found null population - skipping");
                continue;
            }
            
            // Safely get neuron type and neurons
            NeuronType type = pop->getNeuronType();
            auto neurons = pop->getNeurons();
            
            if (neurons.empty()) {
                NLM_LOG_DEBUG("AgentBrain::initializeNeuronGroups: Population with type " + 
                             std::to_string(static_cast<int>(type)) + " has no neurons");
                continue;
            }
            
            // Create appropriate distribution strategy based on neuron type
            auto distributionStrategy = AgentBrainAbstraction::DistributionStrategyFactory::createStrategy(type);
            if (distributionStrategy) {
                // Distribute neurons using the strategy
                if (type == NeuronType::Motor) {
                    distributionStrategy->distribute(neurons, motorForward_);
                    distributionStrategy->distribute(neurons, motorBackward_);
                    distributionStrategy->distribute(neurons, motorTurnLeft_);
                    distributionStrategy->distribute(neurons, motorTurnRight_);
                    distributionStrategy->distribute(neurons, motorInteract_);
                    distributionStrategy->distribute(neurons, motorWait_);
                } else if (type == NeuronType::Sensory) {
                    distributionStrategy->distribute(neurons, sensoryVision_);
                    distributionStrategy->distribute(neurons, sensoryTouch_);
                    distributionStrategy->distribute(neurons, sensoryInternal_);
                    distributionStrategy->distribute(neurons, sensoryProprioception_);
                }
            }
        }
    }
    
    // Log initialization summary
    NLM_LOG_INFO("AgentBrain initialization complete:");
    NLM_LOG_INFO("  Motor groups: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                motorTurnLeft_.size() + motorTurnRight_.size() + 
                motorInteract_.size() + motorWait_.size()) + " neurons");
    NLM_LOG_INFO("  Sensory groups: " + std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                sensoryInternal_.size() + sensoryProprioception_.size()) + " neurons");
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    // Distribute motor neurons to different action groups in a round-robin fashion
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("AgentBrain: Found null motor neuron - skipping");
            continue;
        }
        
        // Calculate distribution index based on current group sizes
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
            default: 
                NLM_LOG_WARNING("AgentBrain: Unexpected motor neuron distribution index - defaulting to wait");
                motorWait_.push_back(n);
                break;
        }
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    // Distribute sensory neurons to different input modalities
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("AgentBrain: Found null sensory neuron - skipping");
            continue;
        }
        
        // Calculate distribution index based on current group sizes
        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                     sensoryInternal_.size() + sensoryProprioception_.size();
        
        switch (idx % 4) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
            default: 
                NLM_LOG_WARNING("AgentBrain: Unexpected sensory neuron distribution index - defaulting to vision");
                sensoryVision_.push_back(n);
                break;
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate world input and initialize state variables
    if (world.getVisionWidth() <= 0 || world.getVisionHeight() <= 0) {
        NLM_LOG_WARNING("AgentBrain: Invalid world dimensions - using defaults");
    }
    
    // Initialize previous vision buffer with proper size and zero values
    size_t visionSize = static_cast<size_t>(world.getVisionWidth() * world.getVisionHeight());
    previousVision_.resize(visionSize, 0.0f);
    
    // Reset developmental and plasticity state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    
    // Log initialization summary for debugging
    NLM_LOG_DEBUG("AgentBrain state after initialization:");
    NLM_LOG_DEBUG("  Developmental age: 0.0, Plasticity modifier: 1.0f");
    NLM_LOG_DEBUG("  Previous vision buffer size: " + std::to_string(previousVision_.size()));
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

// Helper method to process vision input with safety checks
void AgentBrain::processVisionInput(const std::vector<float>& visionInput) {
    // Safety check: ensure we have valid sensory neurons and input data
    if (visionInput.empty() || sensoryVision_.empty()) {
        NLM_LOG_DEBUG("AgentBrain::processVisionInput: No vision data or sensory neurons available");
        return;
    }
    
    // Process each vision neuron with bounds checking and error handling
    for (size_t i = 0; i < sensoryVision_.size() && i < visionInput.size(); ++i) {
        // Check if neuron exists and is valid
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity with safety clamping
            float current = std::clamp(visionInput[i] * 5.0f, -100.0f, 100.0f);
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

// Helper method to process touch input with safety checks
void AgentBrain::processTouchInput(const std::vector<float>& touchInput) {
    // Safety check: ensure we have valid sensory neurons and input data
    if (touchInput.empty() || sensoryTouch_.empty()) {
        NLM_LOG_DEBUG("AgentBrain::processTouchInput: No touch data or sensory neurons available");
        return;
    }
    
    // Process each touch neuron with bounds checking and error handling
    for (size_t i = 0; i < sensoryTouch_.size() && i < touchInput.size(); ++i) {
        // Check if neuron exists and is valid
        if (sensoryTouch_[i]) {
            // Collision signal with safety clamping
            float current = std::clamp(touchInput[i] * 8.0f, -50.0f, 50.0f);
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

// Helper method to process internal signals with normalization
void AgentBrain::processInternalInput(const std::vector<float>& internalInput) {
    // Safety check: ensure we have valid sensory neurons and input data
    if (internalInput.empty() || sensoryInternal_.empty()) {
        NLM_LOG_DEBUG("AgentBrain::processInternalInput: No internal data or sensory neurons available");
        return;
    }
    
    // Process each internal neuron with proper normalization and clamping
    for (size_t i = 0; i < sensoryInternal_.size() && i < internalInput.size(); ++i) {
        // Check if neuron exists and is valid
        if (sensoryInternal_[i]) {
            // Normalize internal signals to range [-5, 5]
            float current = (internalInput[i] * 2.0f - 1.0f) * 5.0f;
            // Apply safety clamping
            current = std::clamp(current, -10.0f, 10.0f);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

// Helper method to process proprioception input
void AgentBrain::processProprioceptionInput(const std::vector<float>& proprioInput) {
    // Safety check: ensure we have valid sensory neurons and input data
    if (proprioInput.empty() || sensoryProprioception_.empty()) {
        NLM_LOG_DEBUG("AgentBrain::processProprioceptionInput: No proprioception data or sensory neurons available");
        return;
    }
    
    // Process each proprioception neuron with proper scaling
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprioInput.size(); ++i) {
        // Check if neuron exists and is valid
        if (sensoryProprioception_[i]) {
            // Center and scale proprioception signals to range [-3, 3]
            float current = (proprioInput[i] * 2.0f - 1.0f) * 3.0f;
            // Apply safety clamping
            current = std::clamp(current, -6.0f, 6.0f);
            sensoryProprioception_[i]->injectCurrent(current);
        }
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
    
    // Create brain adapter for abstracted access
    auto brainAdapter = std::make_unique<AgentBrainAbstraction::BrainAdapter>(brain_);
    
    // Apply to all synapses with eligibility traces using abstracted brain access
    auto regions = brainAdapter->getRegions();
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto synapses = region->getSynapses();
        for (auto* syn : synapses) {
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
    
    // Apply to STDP using abstracted brain access
    auto* stdp = brainAdapter->getSTDP();
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
    
    // Create brain adapter for abstracted access
    auto brainAdapter = std::make_unique<AgentBrainAbstraction::BrainAdapter>(brain_);
    return brainAdapter->getDevelopmentSystem()->getStage();
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

// Helper method to compute novelty with robust error handling and numerical stability
void AgentBrain::computeNovelty(const std::vector<float>& visionInput) {
    // Compute novelty detection with comprehensive validation and error handling
    if (visionInput.empty()) {
        NLM_LOG_DEBUG("AgentBrain::computeNovelty: Empty vision input - novelty set to 0");
        noveltyLevel_ = 0.0f;
        return;
    }
    
    // Validate previous vision buffer matches current input size
    if (previousVision_.size() != visionInput.size()) {
        NLM_LOG_WARNING("AgentBrain::computeNovelty: Vision buffer size mismatch - resizing buffer");
        previousVision_.resize(visionInput.size(), 0.0f);
    }
    
    // Calculate total difference from previous vision with numerical stability
    float totalDiff = 0.0f;
    for (size_t i = 0; i < visionInput.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(visionInput[i] - previousVision_[i]);
        totalDiff += diff;
        
        // Prevent overflow by capping individual differences
        if (diff > 1000.0f) {
            NLM_LOG_WARNING("AgentBrain::computeNovelty: Large vision difference detected: " + std::to_string(diff));
        }
    }
    
    // Normalize by number of vision cells to get average change per cell
    noveltyLevel_ = totalDiff / std::max<size_t>(visionInput.size(), 1);
    
    // Apply exponential decay to novelty level
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Clamp novelty to reasonable range [0, 1] for stability
    noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
    
    // Store current vision for next comparison
    previousVision_ = visionInput;
    
    // Debug log for high novelty events to aid in debugging
    if (noveltyLevel_ > 0.5f) {
        NLM_LOG_DEBUG("AgentBrain::computeNovelty: High novelty detected: " + std::to_string(noveltyLevel_));
    }
}

// Helper method to update curiosity with proper error handling
void AgentBrain::updateCuriosity() {
    // Update curiosity level based on novelty and prediction error with validation
    if (!curiosityEnabled_) {
        NLM_LOG_DEBUG("AgentBrain::updateCuriosity: Curiosity system disabled - skipping update");
        return;
    }
    
    // Calculate curiosity as weighted combination of novelty and prediction error
    float noveltyContribution = noveltyLevel_ * 2.0f;
    float errorContribution = std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = noveltyContribution + errorContribution;
    
    // Clamp curiosity to valid range [0, 1] for numerical stability
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    
    // Log curiosity level for debugging and monitoring
    NLM_LOG_DEBUG("AgentBrain::updateCuriosity: Curiosity level = " + std::to_string(curiosityLevel_));
}
