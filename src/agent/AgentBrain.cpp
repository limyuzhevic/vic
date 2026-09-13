// Initialize behaviors
    behaviorList_.reserve(8);
    
    // Create and enable behaviors based on configuration
    if (memoryGuidedEnabled_) {
        auto memoryBehavior = std::make_unique<MemoryGuidedBehavior>();
        memoryBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        behaviorList_.push_back(std::move(memoryBehavior));
    }
    
    if (socialLearningEnabled_) {
        auto socialBehavior = std::make_unique<SocialLearningBehavior>();
        socialBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        behaviorList_.push_back(std::move(socialBehavior));
    }
    
    if (metaCognitionEnabled_) {
        auto metaBehavior = std::make_unique<MetaCognitionBehavior>();
        metaBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        behaviorList_.push_back(std::move(metaBehavior));
    }
    
    if (curiosityDrivenEnabled_) {
        auto curiosityBehavior = std::make_unique<CuriosityDrivenBehavior>();
        curiosityBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        behaviorList_.push_back(std::move(curiosityBehavior));
    }
    
    if (rewardShapingEnabled_) {
        auto rewardBehavior = std::make_unique<RewardShapingBehavior>();
        rewardBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        behaviorList_.push_back(std::move(rewardBehavior));
    }
    
    if (transferLearningEnabled_) {
        auto transferBehavior = std::make_unique<TransferLearningBehavior>();
        transferBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        behaviorList_.push_back(std::move(transferBehavior));
    }
    
    if (hierarchicalPlanningEnabled_) {
        auto hierarchicalBehavior = std::make_unique<HierarchicalPlanningBehavior>();
        hierarchicalBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        behaviorList_.push_back(std::move(hierarchicalBehavior));
    }
    
    NLM_LOG_INFO("AgentBrain constructed with " + std::to_string(behaviorList_.size()) + " behaviors");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::setBrain(std::shared_ptr<Brain> brain) {
    brain_ = brain;
    
    // Reinitialize behaviors with new brain
    for (auto& behavior : behaviorList_) {
        if (behavior) {
            behavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        }
    }
}

void AgentBrain::enableMemoryGuided(bool enable) {
    memoryGuidedEnabled_ = enable;
    if (enable) {
        auto memoryBehavior = std::make_unique<MemoryGuidedBehavior>();
        memoryBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        // Add or replace in behavior list
        addOrReplaceBehavior(std::move(memoryBehavior), "MemoryGuided");
    }
}

void AgentBrain::enableSocialLearning(bool enable) {
    socialLearningEnabled_ = enable;
    if (enable) {
        auto socialBehavior = std::make_unique<SocialLearningBehavior>();
        socialBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        addOrReplaceBehavior(std::move(socialBehavior), "SocialLearning");
    }
}

void AgentBrain::enableMetaCognition(bool enable) {
    metaCognitionEnabled_ = enable;
    if (enable) {
        auto metaBehavior = std::make_unique<MetaCognitionBehavior>();
        metaBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        addOrReplaceBehavior(std::move(metaBehavior), "MetaCognition");
    }
}

void AgentBrain::enableCuriosityDriven(bool enable) {
    curiosityDrivenEnabled_ = enable;
    if (enable) {
        auto curiosityBehavior = std::make_unique<CuriosityDrivenBehavior>();
        curiosityBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        addOrReplaceBehavior(std::move(curiosityBehavior), "CuriosityDriven");
    }
}

void AgentBrain::enableRewardShaping(bool enable) {
    rewardShapingEnabled_ = enable;
    if (enable) {
        auto rewardBehavior = std::make_unique<RewardShapingBehavior>();
        rewardBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        addOrReplaceBehavior(std::move(rewardBehavior), "RewardShaping");
    }
}

void AgentBrain::enableTransferLearning(bool enable) {
    transferLearningEnabled_ = enable;
    if (enable) {
        auto transferBehavior = std::make_unique<TransferLearningBehavior>();
        transferBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        addOrReplaceBehavior(std::move(transferBehavior), "TransferLearning");
    }
}

void AgentBrain::enableHierarchicalPlanning(bool enable) {
    hierarchicalPlanningEnabled_ = enable;
    if (enable) {
        auto hierarchicalBehavior = std::make_unique<HierarchicalPlanningBehavior>();
        hierarchicalBehavior->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
        addOrReplaceBehavior(std::move(hierarchicalBehavior), "HierarchicalPlanning");
    }
}

void AgentBrain::addOrReplaceBehavior(std::unique_ptr<Behavior> newBehavior, const std::string& name) {
    // Check if behavior with same name already exists
    for (auto it = behaviorList_.begin(); it != behaviorList_.end(); ++it) {
        if ((*it) && (*it)->getName() == name) {
            *it = std::move(newBehavior);
            if (*it) {
                (*it)->initialize(this, episodicMemory_.get(), nullptr, brain_.get());
            }
            return;
        }
    }
    // If not found, add to list
    behaviorList_.push_back(std::move(newBehavior));
}

void AgentBrain::disableAllBehaviors() {
    for (auto& behavior : behaviorList_) {
        if (behavior) {
            behavior->setEnabled(false);
        }
    }
    // Clear the list
    behaviorList_.clear();
}

const std::vector<std::unique_ptr<Behavior>>& AgentBrain::getBehaviors() const {
    return behaviorList_;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand baseCommand = decodeFromMotorNeurons();
    
    // Apply all enabled behaviors
    MotorCommand behaviorCommand = MotorCommand::Wait;
    for (auto& behavior : behaviorList_) {
        if (behavior && behavior->shouldActivate()) {
            MotorCommand cmd = behavior->update(1.0); // Use timestep 1.0 for now
            if (cmd != MotorCommand::Wait) {
                behaviorCommand = cmd;
            }
        }
    }
    
    // If no behavior produced a command, use base command
    if (behaviorCommand == MotorCommand::Wait) {
        return baseCommand;
    }
    
    return behaviorCommand;
}

float AgentBrain::getMetaCognitiveConfidence() const {
    float totalConfidence = 0.0f;
    int count = 0;
    
    for (const auto& behavior : behaviorList_) {
        if (behavior) {
            totalConfidence += behavior->getConfidence();
            count++;
        }
    }
    
    return count > 0 ? totalConfidence / count : 0.5f;
}

std::vector<float> AgentBrain::getWorkingMemoryState() const {
    if (workingMemory_) {
        return workingMemory_->retrieve();
    }
    return std::vector<float>();
}

void AgentBrain::setWorkingMemory(std::shared_ptr<NeuralWorkingMemory> memory) {
    workingMemory_ = memory;
}

void AgentBrain::setEpisodicMemory(std::shared_ptr<NeuralEpisodicMemory> memory) {
    episodicMemory_ = memory;
}

void AgentBrain::setNeuralPlanner(std::shared_ptr<NeuralPlanner> planner) {
    neuralPlanner_ = planner;
}

bool AgentBrain::hasMemoryGuided() const {
    return memoryGuidedEnabled_;
}

bool AgentBrain::hasSocialLearning() const {
    return socialLearningEnabled_;
}

bool AgentBrain::hasMetaCognition() const {
    return metaCognitionEnabled_;
}

bool AgentBrain::hasCuriosityDriven() const {
    return curiosityDrivenEnabled_;
}

bool AgentBrain::hasRewardShaping() const {
    return rewardShapingEnabled_;
}

bool AgentBrain::hasTransferLearning() const {
    return transferLearningEnabled_;
}

bool AgentBrain::hasHierarchicalPlanning() const {
    return hierarchicalPlanningEnabled_;
}

float AgentBrain::getAverageBehaviorConfidence() const {
    float totalConfidence = 0.0f;
    int count = 0;
    
    for (const auto& behavior : behaviorList_) {
        if (behavior) {
            totalConfidence += behavior->getConfidence();
            count++;
        }
    }
    
    return count > 0 ? totalConfidence / count : 0.0f;
}

void AgentBrain::updateBehaviorConfidence(const std::string& behaviorName, float delta) {
    for (auto& behavior : behaviorList_) {
        if (behavior && behavior->getName() == behaviorName) {
            behavior->updateConfidence(delta);
            break;
        }
    }
}

void AgentBrain::recordSocialObservation(ActionType observedAction,
                                        const std::vector<float>& observerState,
                                        const std::vector<float>& resultingState,
                                        float reward) {
    // Record observation in social learning memory
    if (socialLearningEnabled_) {
        // In a full implementation, this would update social learning behavior
        // For now, just log
        NLM_LOG_INFO("Recorded social observation: " + std::to_string(static_cast<int>(observedAction)));
    }
}

void AgentBrain::updateMetaCognition(double timestep) {
    // Update meta-cognitive awareness based on performance
    if (metaCognitionEnabled_) {
        // In a full implementation, this would track prediction accuracy,
        // confidence calibration, and metacognitive monitoring
    }
}

void AgentBrain::updateCuriosityDriven(double timestep) {
    // Update curiosity-based exploration
    if (curiosityDrivenEnabled_) {
        // In a full implementation, this would adjust exploration based on uncertainty
    }
}

void AgentBrain::updateRewardShaping(double timestep) {
    // Apply reward shaping modifications
    if (rewardShapingEnabled_) {
        // In a full implementation, this would apply shaped rewards
    }
}

void AgentBrain::updateTransferLearning(double timestep) {
    // Update transfer learning capabilities
    if (transferLearningEnabled_) {
        // In a full implementation, this would update skill representations
    }
}

void AgentBrain::updateHierarchicalPlanning(double timestep) {
    // Update hierarchical planning
    if (hierarchicalPlanningEnabled_) {
        // In a full implementation, this would update high-level plans
    }
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