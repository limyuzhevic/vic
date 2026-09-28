#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

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
    // Initialize advanced configuration defaults
    , learningStrategy_(LearningStrategy::Balanced)
    , actionSelectionStrategy_(ActionSelectionStrategy::Boltzmann)
    , explorationRate_(0.1f)
    , exploitationRate_(0.9f)
    , dopamineEffect_(1.0f)
    , curiosityEffect_(1.0f)
    , noveltyEffect_(0.5f)
    , predictionErrorEffect_(1.0f)
    , developmentSpeed_(1.0)
    , criticalPeriodAge_(60.0)
    , maturationPeriodAge_(300.0)
    , workingMemoryEnabled_(false)
    , episodicMemoryEnabled_(false)
    , semanticMemoryEnabled_(false)
    , debugLevel_(DebugLevel::Basic)
    , profilingEnabled_(false)
    , sensoryFilteringEnabled_(false)
    , visionFilteringStrength_(0.5f)
    , touchFilteringStrength_(0.5f)
    , rewardFunctionEnabled_(false)
    , episodeLimit_(1000)
    , stepLimit_(10000)
    , timeLimit_(0.0)
    , currentEpisode_(0)
    , stepsInCurrentEpisode_(0)
    , currentEpisodeTime_(0.0)
{
    // Initialize random number generator
    randomGen_ = std::make_unique<std::mt19937>(std::random_device{}());
    
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
    
    // Reset advanced state
    currentEpisode_ = 0;
    stepsInCurrentEpisode_ = 0;
    currentEpisodeTime_ = 0.0;
    
    // Initialize performance tracking
    currentStats_ = PerformanceStats();
    currentStats_.totalTime = 0.0;
    currentStats_.stepsExecuted = 0;
    currentStats_.avgFiringRate = 0.0f;
    currentStats_.avgSpikesPerStep = 0.0f;
    currentStats_.totalReward = 0.0f;
    currentStats_.maxFiringRate = 0.0f;
    currentStats_.avgNeuromodulationLevel = 0.0f;
    currentStats_.memoryUsage = 0;
    currentStats_.cpuUsage = 0.0;
    
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
            // Apply sensory filtering if enabled
            float current = vision[i] * 5.0f; // Scale factor
            if (sensoryFilteringEnabled_) {
                current *= visionFilteringStrength_;
            }
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f; // Collision signal
            if (sensoryFilteringEnabled_) {
                current *= touchFilteringStrength_;
            }
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f; // Center and scale
            if (sensoryFilteringEnabled_) {
                current *= visionFilteringStrength_; // Use same strength for internal
            }
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f; // Center and scale
            if (sensoryFilteringEnabled_) {
                current *= visionFilteringStrength_; // Use same strength for proprioception
            }
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
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * curiosityEffect_ + std::abs(predictionError_) * predictionErrorEffect_ * 0.5f;
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
    
    // Apply action selection strategy
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct; // Default to wait if nothing stronger
    
    switch (actionSelectionStrategy_) {
        case ActionSelectionStrategy::Deterministic:
            // Pure exploitation: choose highest activity
            bestActivity = std::max({forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct});
            if (bestActivity >= forwardAct) best = MotorCommand::MoveForward;
            if (bestActivity >= backwardAct) best = MotorCommand::MoveBackward;
            if (bestActivity >= leftAct) best = MotorCommand::TurnLeft;
            if (bestActivity >= rightAct) best = MotorCommand::TurnRight;
            if (bestActivity >= interactAct) best = MotorCommand::Interact;
            break;
            
        case ActionSelectionStrategy::Random:
            // Pure exploration: random choice
            std::uniform_int_distribution<> dist(0, 7);
            int choice = dist(*randomGen_);
            switch (choice) {
                case 0: best = MotorCommand::MoveForward; break;
                case 1: best = MotorCommand::MoveBackward; break;
                case 2: best = MotorCommand::TurnLeft; break;
                case 3: best = MotorCommand::TurnRight; break;
                case 4: best = MotorCommand::LookLeft; break;
                case 5: best = MotorCommand::LookRight; break;
                case 6: best = MotorCommand::Interact; break;
                default: best = MotorCommand::Wait; break;
            }
            break;
            
        case ActionSelectionStrategy::Boltzmann:
            // Soft-max over activity values
            std::vector<std::pair<MotorCommand, float>> options = {
                {MotorCommand::MoveForward, forwardAct},
                {MotorCommand::MoveBackward, backwardAct},
                {MotorCommand::TurnLeft, leftAct},
                {MotorCommand::TurnRight, rightAct},
                {MotorCommand::Interact, interactAct},
                {MotorCommand::Wait, waitAct}
            };
            
            float maxAct = std::max_element(options.begin(), options.end(),
                                          [](const auto& a, const auto& b) { return a.second < b.second; })->second;
            
            std::vector<float> weights;
            for (const auto& opt : options) {
                weights.push_back(std::exp((opt.second - maxAct) / 2.0f)); // Temperature parameter
            }
            
            float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
            if (sum > 0.0f) {
                std::uniform_real_distribution<> dist(0.0, sum);
                float r = dist(*randomGen_);
                float cumsum = 0.0f;
                for (size_t i = 0; i < options.size(); ++i) {
                    cumsum += weights[i];
                    if (r <= cumsum) {
                        best = options[i].first;
                        break;
                    }
                }
            }
            break;
            
        case ActionSelectionStrategy::Ucb:
            // Upper confidence bound for exploration
            // This is a simplified version using activity with exploration bonus
            float explorationBonus = 2.0f * std::sqrt(std::log(currentEpisode_ + 1) / std::max(1.0, stepsInCurrentEpisode_));
            
            std::vector<std::pair<MotorCommand, float>> ucbOptions = {
                {MotorCommand::MoveForward, forwardAct + explorationBonus},
                {MotorCommand::MoveBackward, backwardAct + explorationBonus},
                {MotorCommand::TurnLeft, leftAct + explorationBonus},
                {MotorCommand::TurnRight, rightAct + explorationBonus},
                {MotorCommand::Interact, interactAct + explorationBonus},
                {MotorCommand::Wait, waitAct + explorationBonus}
            };
            
            best = std::max_element(ucbOptions.begin(), ucbOptions.end(),
                                   [](const auto& a, const auto& b) { return a.second < b.second; })->first;
            break;
            
        case ActionSelectionStrategy::EpsGreedy:
            // Epsilon-greedy with adaptive epsilon based on curiosity
            float epsilon = explorationRate_ * (1.0f - curiosityLevel_); // More curious = less greedy
            std::uniform_real_distribution<> dist(0.0, 1.0);
            
            if (dist(*randomGen_) < epsilon) {
                // Explore: random action
                std::uniform_int_distribution<> actionDist(0, 5);
                int choice = actionDist(*randomGen_);
                switch (choice) {
                    case 0: best = MotorCommand::MoveForward; break;
                    case 1: best = MotorCommand::MoveBackward; break;
                    case 2: best = MotorCommand::TurnLeft; break;
                    case 3: best = MotorCommand::TurnRight; break;
                    case 4: best = MotorCommand::Interact; break;
                    default: best = MotorCommand::Wait; break;
                }
            } else {
                // Exploit: best activity
                bestActivity = std::max({forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct});
                if (bestActivity >= forwardAct) best = MotorCommand::MoveForward;
                if (bestActivity >= backwardAct) best = MotorCommand::MoveBackward;
                if (bestActivity >= leftAct) best = MotorCommand::TurnLeft;
                if (bestActivity >= rightAct) best = MotorCommand::TurnRight;
                if (bestActivity >= interactAct) best = MotorCommand::Interact;
            }
            break;
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
        
        std::uniform_real_distribution<> dist(0.0f, 1.0f);
        float r = dist(*randomGen_);
        if (r < exploreChance) {
            // Random motor command
            std::uniform_int_distribution<> actionDist(0, 7);
            int choice = actionDist(*randomGen_);
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
    dopamineLevel_ = predictionError_ * dopamineEffect_;
    
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
    
    developmentalAge_ += timestep * developmentSpeed_;
    currentEpisodeTime_ += timestep;
    stepsInCurrentEpisode_++;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < criticalPeriodAge_) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < maturationPeriodAge_) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Custom developmental stages
    for (const auto& stage : customStages_) {
        if (developmentalAge_ >= stage.first) {
            brain_->setDevelopmentalStage(stage.second);
            break;
        }
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

void AgentBrain::addCustomDevelopmentalStage(double age, DevelopmentalStage stage) {
    customStages_[age] = stage;
}

DevelopmentalStage AgentBrain::getCustomDevelopmentalStage(double age) const {
    for (const auto& stage : customStages_) {
        if (age >= stage.first) {
            return stage.second;
        }
    }
    return DevelopmentalStage::Initial;
}

void AgentBrain::updatePerformanceStats(size_t stepsExecuted, float avgFiringRate, 
                                       float totalReward, double timeElapsed) {
    currentStats_.totalTime = timeElapsed;
    currentStats_.stepsExecuted = stepsExecuted;
    currentStats_.avgFiringRate = avgFiringRate;
    currentStats_.avgSpikesPerStep = avgFiringRate * 2.0f; // Approximate
    currentStats_.totalReward = totalReward;
    currentStats_.maxFiringRate = std::max(currentStats_.maxFiringRate, avgFiringRate);
    currentStats_.avgNeuromodulationLevel = (currentStats_.avgNeuromodulationLevel + dopamineLevel_) / 2.0f;
    
    // Simple memory usage estimation
    currentStats_.memoryUsage = stepsExecuted * 100; // Rough estimate
    
    // CPU usage estimation
    currentStats_.cpuUsage = timeElapsed > 0.0 ? (1.0 / timeElapsed) * 100.0 : 0.0;
}

void AgentBrain::recordProfileData(const std::string& name, double value) {
    if (profilingEnabled_) {
        profileData_[name] = value;
    }
}

void AgentBrain::addCustomActionMapping(ActionType action, const std::string& name) {
    actionMappings_[action] = name;
}

void AgentBrain::removeCustomActionMapping(ActionType action) {
    actionMappings_.erase(action);
}

std::string AgentBrain::getActionMappingName(ActionType action) const {
    auto it = actionMappings_.find(action);
    if (it != actionMappings_.end()) {
        return it->second;
    }
    return "Unknown";
}

void AgentBrain::triggerExternalEvent(const std::string& event, const std::map<std::string, std::string>& data) {
    if (externalEventCallback_) {
        std::string eventData = event;
        for (const auto& kv : data) {
            eventData += " | " + kv.first + "=" + kv.second;
        }
        externalEventCallback_(eventData);
    }
}

} // namespace nlm