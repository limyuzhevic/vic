#include <limits>

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
    // Advanced features for experienced users
    , explorationWeight_(1.0f)
    , learningRate_(0.1f)
    , noveltyThreshold_(0.5f)
    , memoryPersistenceEnabled_(false)
    , memoryRetention_(0.8f)
    , averageActivityLevel_(0.0f)
    , totalMemoryUsage_(0)
    , dopamineGain_(1.0f)
    , curiosityGain_(1.0f)
    , noveltyDecay_(0.99f)
    , developmentSpeed_(1.0f)
    , sensoryGain_(1.0f)
    , motorThreshold_(0.5f)
    , debugVisualizationEnabled_(false)
    , visualizationScale_(1.0f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) continue;  // Safety check
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        // Check for potential overflow
                        if (idx < static_cast<size_t>(std::numeric_limits<size_t>::max())) {
                            switch (idx % 6) {
                                case 0: motorForward_.push_back(n); break;
                                case 1: motorBackward_.push_back(n); break;
                                case 2: motorTurnLeft_.push_back(n); break;
                                case 3: motorTurnRight_.push_back(n); break;
                                case 4: motorInteract_.push_back(n); break;
                                case 5: motorWait_.push_back(n); break;
                            }
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) continue;  // Safety check
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        // Check for potential overflow
                        if (idx < static_cast<size_t>(std::numeric_limits<size_t>::max())) {
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
        size_t validNeurons = 0;
        
        for (Neuron* n : neurons) {
            if (!n) continue;  // Null check
            const NeuronState& state = n->getState();
            
            // Improved activity calculation: consider both threshold crossing and potential magnitude
            float activity = 0.0f;
            if (state.firingState == FiringState::Active || 
                state.firingState == FiringState::Refractory) {
                // Neuron is firing or has recently fired
                activity = std::max(0.0f, state.membranePotential - state.threshold);
            } else {
                // Inactive neuron: measure how close to threshold
                activity = std::max(0.0f, state.membranePotential - state.threshold);
            }
            
            // Add contribution from resting potential deviation (background activity)
            float potentialDeviation = std::abs(state.membranePotential - state.restingPotential);
            activity += potentialDeviation * 0.1f;  // Weight background activity
            
            sum += activity;
            validNeurons++;
        }
        
        return validNeurons > 0 ? (sum / validNeurons) : 0.0f;
    };
    
    // Calculate activity for each motor group with bounds checking
    std::vector<std::pair<MotorCommand, float>> commandActivities;
    
    commandActivities.emplace_back(MotorCommand::MoveForward, calcActivity(motorForward_));
    commandActivities.emplace_back(MotorCommand::MoveBackward, calcActivity(motorBackward_));
    commandActivities.emplace_back(MotorCommand::TurnLeft, calcActivity(motorTurnLeft_));
    commandActivities.emplace_back(MotorCommand::TurnRight, calcActivity(motorTurnRight_));
    commandActivities.emplace_back(MotorCommand::Interact, calcActivity(motorInteract_));
    commandActivities.emplace_back(MotorCommand::Wait, calcActivity(motorWait_));
    
    // Find maximum activity with improved thresholding
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;  // Changed: start at 0 instead of waitAct
    
    for (const auto& cmdActivity : commandActivities) {
        if (cmdActivity.second > bestActivity) {
            bestActivity = cmdActivity.second;
            best = cmdActivity.first;
        }
    }
    
    // Improved thresholding: more nuanced decision making
    const float ACTIVITY_THRESHOLD = 0.1f;  // Lower threshold for more responsive control
    const float MAX_ACTIVITY = 10.0f;      // Maximum expected activity
    
    // Apply sigmoidal response curve for smooth control
    float normalizedActivity = std::clamp(bestActivity / MAX_ACTIVITY, 0.0f, 1.0f);
    float activityConfidence = 1.0f / (1.0f + std::exp(-10.0f * (normalizedActivity - 0.5f)));
    
    // Only act if there's meaningful activity and high confidence
    if (activityConfidence < 0.7f || bestActivity < ACTIVITY_THRESHOLD) {
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

std::string AgentBrain::getDebugInfo() const {
    std::ostringstream oss;
    oss << "=== AgentBrain Debug Info ===\n";
    oss << "Sensory Neurons: " << sensoryVision_.size() + sensoryTouch_.size() + 
        sensoryInternal_.size() + sensoryProprioception_.size() << "\n";
    oss << "Motor Neurons: " << motorForward_.size() + motorBackward_.size() + 
        motorTurnLeft_.size() + motorTurnRight_.size() + 
        motorInteract_.size() + motorWait_.size() << "\n";
    oss << "Current Activity Level: " << averageActivityLevel_ << "\n";
    oss << "Exploration Ratio: " << (curiosityEnabled_ ? curiosityLevel_ : 0.0f) << "\n";
    oss << "Memory Usage: " << totalMemoryUsage_ << " bytes\n";
    oss << "Neuromodulation Level: " << dopamineLevel_ << "\n";
    oss << "Development Stage: " << static_cast<int>(getDevelopmentalStage()) << "\n";
    oss << "=== End Debug Info ===\n";
    return oss.str();
}

float AgentBrain::getAverageActivityLevel() const {
    float totalActivity = 0.0f;
    size_t totalNeurons = 0;
    
    // Calculate average activity across all neurons
    for (const auto* neurons : {
        &motorForward_, &motorBackward_, &motorTurnLeft_, &motorTurnRight_,
        &motorInteract_, &motorWait_, &sensoryVision_, &sensoryTouch_,
        &sensoryInternal_, &sensoryProprioception_
    }) {
        for (Neuron* n : *neurons) {
            if (!n) continue;
            const NeuronState& state = n->getState();
            float activity = std::abs(state.membranePotential - state.restingPotential);
            totalActivity += activity;
            totalNeurons++;
        }
    }
    
    averageActivityLevel_ = totalNeurons > 0 ? (totalActivity / totalNeurons) : 0.0f;
    return averageActivityLevel_;
}

float AgentBrain::getExplorationRatio() const {
    if (!curiosityEnabled_) return 0.0f;
    
    float noveltyComponent = noveltyLevel_ * explorationWeight_;
    float predictionErrorComponent = std::abs(predictionError_) * (1.0f - explorationWeight_);
    
    return std::clamp(noveltyComponent + predictionErrorComponent, 0.0f, 1.0f);
}

size_t AgentBrain::getTotalMemoryUsage() const {
    return totalMemoryUsage_;
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep * developmentSpeed_;
    
    // Simple developmental stages based on age
    if (developmentalAge_ < 60.0 * developmentSpeed_) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0 * developmentSpeed_) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0 * developmentSpeed_) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Update memory usage based on development
    totalMemoryUsage_ = static_cast<size_t>(developmentalAge_ * 100.0f);  // Rough estimate
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_ * developmentSpeed_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_) * developmentSpeed_;
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        // Log error: No brain available
        return;
    }
    
    // Apply sensory gain
    float visionGain = sensoryGain_;
    float touchGain = sensoryGain_ * 2.0f;  // Touch is typically stronger
    float internalGain = sensoryGain_ * 1.5f;
    float proprioceptionGain = sensoryGain_ * 1.2f;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (!sensoryVision_[i]) {
            // Log error: null neuron pointer in sensoryVision_
            continue;
        }
        
        // Inject current proportional to vision intensity with gain
        float current = vision[i] * visionGain * 5.0f;  // Scale factor
        
        // Validate current range
        if (std::isnan(current) || std::isinf(current)) {
            current = 0.0f;  // Reset to safe value
        }
        
        sensoryVision_[i]->injectCurrent(current);
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (!sensoryTouch_[i]) {
            // Log error: null neuron pointer in sensoryTouch_
            continue;
        }
        
        float current = touch[i] * touchGain * 8.0f;  // Collision signal
        
        // Validate current range
        if (std::isnan(current) || std::isinf(current)) {
            current = 0.0f;  // Reset to safe value
        }
        
        sensoryTouch_[i]->injectCurrent(current);
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (!sensoryInternal_[i]) {
            // Log error: null neuron pointer in sensoryInternal_
            continue;
        }
        
        float current = (intern[i] * internalGain * 2.0f - 1.0f) * 5.0f;  // Center and scale
        
        // Validate current range
        if (std::isnan(current) || std::isinf(current)) {
            current = 0.0f;  // Reset to safe value
        }
        
        sensoryInternal_[i]->injectCurrent(current);
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (!sensoryProprioception_[i]) {
            // Log error: null neuron pointer in sensoryProprioception_
            continue;
        }
        
        float current = (proprio[i] * proprioceptionGain * 2.0f - 1.0f) * 3.0f;  // Center and scale
        
        // Validate current range
        if (std::isnan(current) || std::isinf(current)) {
            current = 0.0f;  // Reset to safe value
        }
        
        sensoryProprioception_[i]->injectCurrent(current);
    }
    
    // Compute novelty with threshold and decay
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            if (diff > noveltyThreshold_) {  // Apply novelty threshold
                // Validate difference range
                if (!std::isnan(diff) && !std::isinf(diff)) {
                    totalDiff += diff;
                }
            }
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Apply novelty decay (controllable by advanced users)
        noveltyLevel_ *= noveltyDecay_;
        
        // Validate novelty level range
        if (std::isnan(noveltyLevel_) || std::isinf(noveltyLevel_)) {
            noveltyLevel_ = 0.0f;
        }
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity with enhanced calculation
    if (curiosityEnabled_) {
        // Apply curiosity gain and exploration weight
        float curiosityBase = noveltyLevel_ * 2.0f * curiosityGain_;
        float predictionErrorComponent = std::abs(predictionError_) * 0.5f * curiosityGain_;
        curiosityLevel_ = curiosityBase + predictionErrorComponent;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f * explorationWeight_);
        
        // Validate curiosity level range
        if (std::isnan(curiosityLevel_) || std::isinf(curiosityLevel_)) {
            curiosityLevel_ = 0.0f;
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration with enhanced control
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply motor threshold
    if (curiosityLevel_ < motorThreshold_) {
        return MotorCommand::Wait;
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        size_t validNeurons = 0;
        
        for (Neuron* n : neurons) {
            if (!n) continue;  // Null check
            const NeuronState& state = n->getState();
            
            // Improved activity calculation: consider both threshold crossing and potential magnitude
            float activity = 0.0f;
            if (state.firingState == FiringState::Active || 
                state.firingState == FiringState::Refractory) {
                // Neuron is firing or has recently fired
                activity = std::max(0.0f, state.membranePotential - state.threshold);
            } else {
                // Inactive neuron: measure how close to threshold
                activity = std::max(0.0f, state.membranePotential - state.threshold);
            }
            
            // Add contribution from resting potential deviation (background activity)
            float potentialDeviation = std::abs(state.membranePotential - state.restingPotential);
            activity += potentialDeviation * 0.1f;  // Weight background activity
            
            sum += activity;
            validNeurons++;
        }
        
        return validNeurons > 0 ? (sum / validNeurons) : 0.0f;
    };
    
    // Calculate activity for each motor group with bounds checking
    std::vector<std::pair<MotorCommand, float>> commandActivities;
    
    commandActivities.emplace_back(MotorCommand::MoveForward, calcActivity(motorForward_));
    commandActivities.emplace_back(MotorCommand::MoveBackward, calcActivity(motorBackward_));
    commandActivities.emplace_back(MotorCommand::TurnLeft, calcActivity(motorTurnLeft_));
    commandActivities.emplace_back(MotorCommand::TurnRight, calcActivity(motorTurnRight_));
    commandActivities.emplace_back(MotorCommand::Interact, calcActivity(motorInteract_));
    commandActivities.emplace_back(MotorCommand::Wait, calcActivity(motorWait_));
    
    // Find maximum activity with improved thresholding
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;  // Changed: start at 0 instead of waitAct
    
    for (const auto& cmdActivity : commandActivities) {
        if (cmdActivity.second > bestActivity) {
            bestActivity = cmdActivity.second;
            best = cmdActivity.first;
        }
    }
    
    // Improved thresholding: more nuanced decision making
    const float ACTIVITY_THRESHOLD = 0.1f;  // Lower threshold for more responsive control
    const float MAX_ACTIVITY = 10.0f;      // Maximum expected activity
    
    // Apply sigmoidal response curve for smooth control
    float normalizedActivity = std::clamp(bestActivity / MAX_ACTIVITY, 0.0f, 1.0f);
    float activityConfidence = 1.0f / (1.0f + std::exp(-10.0f * (normalizedActivity - 0.5f)));
    
    // Only act if there's meaningful activity and high confidence
    if (activityConfidence < 0.7f || bestActivity < ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f * explorationWeight_;  // Up to 30% random, scaled by exploration weight
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command with enhanced variety
            int choice = brain_->getRandomGenerator()->uniformInt(0, 8);  // Added LookLeft/LookRight
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;  // Added for more advanced movement
                case 5: return MotorCommand::LookRight;  // Added for more advanced movement
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

} // namespace nlm
