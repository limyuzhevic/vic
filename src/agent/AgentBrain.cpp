namespace nlm {

// Configuration constants
namespace {
    constexpr float sensoryNoveltyDecayFactor = 0.99f;
    constexpr float visionScaleFactor = 5.0f;
    constexpr float touchScaleFactor = 8.0f;
    constexpr float internalScaleFactor = 5.0f;
    constexpr float proprioScaleFactor = 3.0f;
    constexpr float curiosityNoveltyFactor = 2.0f;
    constexpr float curiosityErrorFactor = 0.5f;
    constexpr float minimumActionThreshold = 0.5f;
    constexpr float curiosityThreshold = 0.3f;
    constexpr float curiosityExploreMaxChance = 0.3f;
    constexpr float motorActionCount = 7;
    constexpr float eligibilityThreshold = 0.001f;
    constexpr size_t motorGroupCount = 6;
    constexpr size_t sensoryGroupCount = 4;
    
    constexpr double developmentStageInitialDuration = 60.0;
    constexpr double developmentStageCriticalPeriodDuration = 240.0;
    constexpr double developmentStageMaturationDuration = 600.0;
}

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
    , sensoryNoveltyDecay_(sensoryNoveltyDecayFactor)
    , motorForward_()
    , motorBackward_()
    , motorTurnLeft_()
    , motorTurnRight_()
    , motorInteract_()
    , motorWait_()
    , sensoryVision_()
    , sensoryTouch_()
    , sensoryInternal_()
    , sensoryProprioception_()
    , previousVision_()
{
    // Validate input
    if (!brain_) {
        return;
    }

    // Pre-allocate motor neuron vectors based on expected motor neuron count
    // Estimate based on typical brain configuration
    const size_t estimatedMotorNeurons = 100;
    motorForward_.reserve(estimatedMotorNeurons / motorGroupCount);
    motorBackward_.reserve(estimatedMotorNeurons / motorGroupCount);
    motorTurnLeft_.reserve(estimatedMotorNeurons / motorGroupCount);
    motorTurnRight_.reserve(estimatedMotorNeurons / motorGroupCount);
    motorInteract_.reserve(estimatedMotorNeurons / motorGroupCount);
    motorWait_.reserve(estimatedMotorNeurons / motorGroupCount);

    // Pre-allocate sensory neuron vectors
    const size_t estimatedSensoryNeurons = 200;
    sensoryVision_.reserve(estimatedSensoryNeurons / sensoryGroupCount);
    sensoryTouch_.reserve(estimatedSensoryNeurons / sensoryGroupCount);
    sensoryInternal_.reserve(estimatedSensoryNeurons / sensoryGroupCount);
    sensoryProprioception_.reserve(estimatedSensoryNeurons / sensoryGroupCount);

    // Initialize motor and sensory neuron groups
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) continue;
            
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                const auto& neurons = pop->getNeurons();
                for (size_t i = 0; i < neurons.size(); ++i) {
                    Neuron* n = neurons[i];
                    if (!n) continue;
                    
                    // Distribute motor neurons to different action groups
                    size_t distributionIdx = motorForward_.size() + motorBackward_.size() + 
                                              motorTurnLeft_.size() + motorTurnRight_.size() +
                                              motorInteract_.size() + motorWait_.size();
                    
                    switch (distributionIdx % motorGroupCount) {
                        case 0: motorForward_.push_back(n); break;
                        case 1: motorBackward_.push_back(n); break;
                        case 2: motorTurnLeft_.push_back(n); break;
                        case 3: motorTurnRight_.push_back(n); break;
                        case 4: motorInteract_.push_back(n); break;
                        case 5: motorWait_.push_back(n); break;
                    }
                }
            } else if (type == NeuronType::Sensory) {
                const auto& neurons = pop->getNeurons();
                for (size_t i = 0; i < neurons.size(); ++i) {
                    Neuron* n = neurons[i];
                    if (!n) continue;
                    
                    // Distribute sensory neurons
                    size_t distributionIdx = sensoryVision_.size() + sensoryTouch_.size() +
                                              sensoryInternal_.size() + sensoryProprioception_.size();
                    
                    switch (distributionIdx % sensoryGroupCount) {
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

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate input parameters
    if (world.getWidth() <= 0.0f || world.getHeight() <= 0.0f) {
        return;
    }
    
    if (world.getVisionWidth() == 0 || world.getVisionHeight() == 0) {
        return;
    }
    
    // Initialize vision buffer with validated size
    size_t visionBufferSize = world.getVisionWidth() * world.getVisionHeight();
    previousVision_.resize(visionBufferSize, 0.0f);
    
    // Validate vision buffer was allocated correctly
    if (previousVision_.size() != visionBufferSize) {
        previousVision_.clear();
        return;
    }
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization with validated neuron counts
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
    if (!brain_) {
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    size_t visionNeuronsProcessed = std::min(sensoryVision_.size(), vision.size());
    for (size_t i = 0; i < visionNeuronsProcessed; ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (neuron) {
            // Inject current proportional to vision intensity
            float current = vision[i] * visionScaleFactor;
            neuron->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    size_t touchNeuronsProcessed = std::min(sensoryTouch_.size(), touch.size());
    for (size_t i = 0; i < touchNeuronsProcessed; ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (neuron) {
            float current = touch[i] * touchScaleFactor;
            neuron->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    size_t internalNeuronsProcessed = std::min(sensoryInternal_.size(), intern.size());
    for (size_t i = 0; i < internalNeuronsProcessed; ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (neuron) {
            float current = (intern[i] * 2.0f - 1.0f) * internalScaleFactor;
            neuron->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    size_t proprioNeuronsProcessed = std::min(sensoryProprioception_.size(), proprio.size());
    for (size_t i = 0; i < proprioNeuronsProcessed; ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * proprioScaleFactor;
            neuron->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t visionSize = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < visionSize; ++i) {
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
        curiosityLevel_ = noveltyLevel_ * curiosityNoveltyFactor + std::abs(predictionError_) * curiosityErrorFactor;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decodedCommand = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold) {
        decodedCommand = selectWithCuriosity(decodedCommand);
    }
    
    return decodedCommand;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calculateNeuronalActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;
            // Use membrane potential deviation from rest as activity measure
            const NeuronState& state = n->getState();
            sum += std::abs(state.membranePotential - state.restingPotential);
        }
        
        return neurons.empty() ? 0.0f : sum / neurons.size();
    };

    float forwardActivity = calculateNeuronalActivity(motorForward_);
    float backwardActivity = calculateNeuronalActivity(motorBackward_);
    float leftActivity = calculateNeuronalActivity(motorTurnLeft_);
    float rightActivity = calculateNeuronalActivity(motorTurnRight_);
    float interactActivity = calculateNeuronalActivity(motorInteract_);
    float waitActivity = calculateNeuronalActivity(motorWait_);
    
    // Find maximum activity
    struct CommandWithActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandWithActivity commands[] = {
        {MotorCommand::MoveForward, forwardActivity},
        {MotorCommand::MoveBackward, backwardActivity},
        {MotorCommand::TurnLeft, leftActivity},
        {MotorCommand::TurnRight, rightActivity},
        {MotorCommand::Interact, interactActivity},
        {MotorCommand::Wait, waitActivity}
    };
    
    MotorCommand bestCommand = MotorCommand::Wait;
    float bestActivity = waitActivity;  // Default to wait if nothing stronger
    
    for (const auto& cmdWithActivity : commands) {
        if (cmdWithActivity.activity > bestActivity) {
            bestActivity = cmdWithActivity.activity;
            bestCommand = cmdWithActivity.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < minimumActionThreshold) {
        return MotorCommand::Wait;
    }
    
    return bestCommand;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > curiosityThreshold && brain_) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * curiosityExploreMaxChance;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, motorActionCount - 1);
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
    if (!brain_ || !rewardModulationEnabled_) {
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
            
            if (std::abs(eligibility) > eligibilityThreshold) {
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
    if (!brain_ || !developmentEnabled_) {
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < developmentStageInitialDuration) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < developmentStageCriticalPeriodDuration) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < developmentStageMaturationDuration) {  // ~15 minutes
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
            float synaptogenesisRate = 0.0001f * plasticityModifier_;
            float pruningRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synaptogenesisRate);
            sp->setPruningRate(pruningRate);
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
