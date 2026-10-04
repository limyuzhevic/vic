#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const Config& config)
    : brain_(brain)
    , config_(config)
{
    if (brain_) {
        distributeMotorNeurons();
        distributeSensoryNeurons();
    }
    neuromodState_.reset();
    devState_.reset();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::distributeMotorNeurons() {
    motorGroups_.clear();
    
    if (!brain_) return;
    
    // Calculate target distribution based on config
    int forward = config_.motorDistribution[0];
    int backward = config_.motorDistribution[1];
    int left = config_.motorDistribution[2];
    int right = config_.motorDistribution[3];
    int interact = config_.motorDistribution[4];
    int wait = config_.motorDistribution[5];
    int lookLeft = 0;  // Not in original config, keeping for compatibility
    int lookRight = 0; // Not in original config, keeping for compatibility
    
    int totalMotorNeurons = 0;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                totalMotorNeurons += pop->getNeuronCount();
            }
        }
    }
    
    if (totalMotorNeurons == 0) return;
    
    // Distribute neurons according to config ratios
    std::vector<Neuron*> allMotorNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                auto neurons = pop->getNeurons();
                allMotorNeurons.insert(allMotorNeurons.end(), neurons.begin(), neurons.end());
            }
        }
    }
    
    // Calculate distribution ratios
    float totalConfigured = forward + backward + left + right + interact + wait + lookLeft + lookRight;
    if (totalConfigured == 0) {
        // Fallback to equal distribution
        forward = backward = left = right = interact = wait = totalMotorNeurons / 8;
    } else {
        forward = static_cast<int>((static_cast<float>(forward) / totalConfigured) * totalMotorNeurons);
        backward = static_cast<int>((static_cast<float>(backward) / totalConfigured) * totalMotorNeurons);
        left = static_cast<int>((static_cast<float>(left) / totalConfigured) * totalMotorNeurons);
        right = static_cast<int>((static_cast<float>(right) / totalConfigured) * totalMotorNeurons);
        interact = static_cast<int>((static_cast<float>(interact) / totalConfigured) * totalMotorNeurons);
        wait = static_cast<int>((static_cast<float>(wait) / totalConfigured) * totalMotorNeurons);
    }
    
    // Distribute neurons
    size_t idx = 0;
    for (Neuron* neuron : allMotorNeurons) {
        if (idx < static_cast<size_t>(forward)) {
            motorGroups_.forward.push_back(neuron);
        } else if (idx < static_cast<size_t>(forward + backward)) {
            motorGroups_.backward.push_back(neuron);
        } else if (idx < static_cast<size_t>(forward + backward + left)) {
            motorGroups_.left.push_back(neuron);
        } else if (idx < static_cast<size_t>(forward + backward + left + right)) {
            motorGroups_.right.push_back(neuron);
        } else if (idx < static_cast<size_t>(forward + backward + left + right + interact)) {
            motorGroups_.interact.push_back(neuron);
        } else if (idx < static_cast<size_t>(forward + backward + left + right + interact + wait)) {
            motorGroups_.wait.push_back(neuron);
        } else {
            motorGroups_.lookLeft.push_back(neuron);
        }
        idx++;
    }
}

void AgentBrain::distributeSensoryNeurons() {
    sensoryGroups_.clear();
    
    if (!brain_) return;
    
    // Calculate target distribution based on config
    int vision = config_.sensoryDistribution[0];
    int touch = config_.sensoryDistribution[1];
    int internal = config_.sensoryDistribution[2];
    int proprioception = config_.sensoryDistribution[3];
    
    int totalSensoryNeurons = 0;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                totalSensoryNeurons += pop->getNeuronCount();
            }
        }
    }
    
    if (totalSensoryNeurons == 0) return;
    
    // Get all sensory neurons
    std::vector<Neuron*> allSensoryNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                auto neurons = pop->getNeurons();
                allSensoryNeurons.insert(allSensoryNeurons.end(), neurons.begin(), neurons.end());
            }
        }
    }
    
    // Calculate distribution ratios
    float totalConfigured = vision + touch + internal + proprioception;
    if (totalConfigured == 0) {
        // Fallback to equal distribution
        vision = touch = internal = proprioception = totalSensoryNeurons / 4;
    } else {
        vision = static_cast<int>((static_cast<float>(vision) / totalConfigured) * totalSensoryNeurons);
        touch = static_cast<int>((static_cast<float>(touch) / totalConfigured) * totalSensoryNeurons);
        internal = static_cast<int>((static_cast<float>(internal) / totalConfigured) * totalSensoryNeurons);
        proprioception = static_cast<int>((static_cast<float>(proprioception) / totalConfigured) * totalSensoryNeurons);
    }
    
    // Distribute neurons
    size_t idx = 0;
    for (Neuron* neuron : allSensoryNeurons) {
        if (idx < static_cast<size_t>(vision)) {
            sensoryGroups_.vision.push_back(neuron);
        } else if (idx < static_cast<size_t>(vision + touch)) {
            sensoryGroups_.touch.push_back(neuron);
        } else if (idx < static_cast<size_t>(vision + touch + internal)) {
            sensoryGroups_.internal.push_back(neuron);
        } else {
            sensoryGroups_.proprioception.push_back(neuron);
        }
        idx++;
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Use advanced circular buffer for vision history
    visionBuffer_.initialize(world.getVisionWidth() * world.getVisionHeight());
    devState_.reset();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryGroups_.vision.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryGroups_.touch.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryGroups_.internal.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Use configurable sizes instead of hardcoded values
    size_t visionSize = 0;
    if (!sensoryGroups_.vision.empty() && brain_) {
        // Estimate based on brain configuration
        visionSize = brain_->getTotalNeuronCount() / 20; // Approximate
    } else {
        visionSize = 256; // Default fallback
    }
    
    return visionSize + config_.sensoryDistribution[1] + config_.sensoryDistribution[2] + config_.sensoryDistribution[3];
}

size_t AgentBrain::getMotorOutputSize() const {
    // Return number of configured motor groups
    return motorGroups_.forward.size() + motorGroups_.backward.size() +
           motorGroups_.left.size() + motorGroups_.right.size() +
           motorGroups_.interact.size() + motorGroups_.wait.size() +
           motorGroups_.lookLeft.size() + motorGroups_.lookRight.size();
}

void AgentBrain::validateInputArrays(const std::vector<float>& vision, const std::vector<float>& touch,
                                   const std::vector<float>& internal, const std::vector<float>& proprioception) const {
    // Validate input ranges
    for (float val : vision) {
        if (val < 0.0f || val > 1.0f) {
            NLM_LOG_WARNING("Vision value out of range: " + std::to_string(val));
        }
    }
    
    // Validate touch arrays (can be out of range, but warn)
    for (float val : touch) {
        if (val < 0.0f) {
            NLM_LOG_WARNING("Touch value negative: " + std::to_string(val));
        }
    }
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Validate inputs
    validateInputArrays(percept.getVision(), percept.getTouch(), 
                       percept.getInternal(), percept.getProprioception());
    
    // Process vision input with optimized access
    const auto& vision = percept.getVision();
    const size_t visionSize = std::min(vision.size(), sensoryGroups_.vision.size());
    
    if (visionSize > 0) {
        // Use SIMD-like optimization for vision processing
        for (size_t i = 0; i < visionSize; ++i) {
            if (sensoryGroups_.vision[i]) {
                float current = vision[i] * config_.visionScaleFactor;
                sensoryGroups_.vision[i]->injectCurrent(current);
            }
        }
    }
    
    // Process touch input
    const auto& touch = percept.getTouch();
    const size_t touchSize = std::min(touch.size(), sensoryGroups_.touch.size());
    
    if (touchSize > 0) {
        for (size_t i = 0; i < touchSize; ++i) {
            if (sensoryGroups_.touch[i]) {
                float current = touch[i] * config_.touchScaleFactor;
                sensoryGroups_.touch[i]->injectCurrent(current);
            }
        }
    }
    
    // Process internal signals
    const auto& intern = percept.getInternal();
    const size_t internalSize = std::min(intern.size(), sensoryGroups_.internal.size());
    
    if (internalSize > 0) {
        for (size_t i = 0; i < internalSize; ++i) {
            if (sensoryGroups_.internal[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * config_.internalScaleFactor;
                sensoryGroups_.internal[i]->injectCurrent(current);
            }
        }
    }
    
    // Process proprioception
    const auto& proprio = percept.getProprioception();
    const size_t proprioSize = std::min(proprio.size(), sensoryGroups_.proprioception.size());
    
    if (proprioSize > 0) {
        for (size_t i = 0; i < proprioSize; ++i) {
            if (sensoryGroups_.proprioception[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * config_.proprioceptionScaleFactor;
                sensoryGroups_.proprioception[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute advanced novelty detection
    computeAdvancedNovelty(vision);
    
    // Update curiosity and exploration state
    updateExplorationState();
}

void AgentBrain::computeAdvancedNovelty(const std::vector<float>& vision) {
    if (vision.empty()) {
        neuromodState_.noveltyLevel = 0.0f;
        return;
    }
    
    // Use advanced circular buffer for novelty calculation
    float novelty = visionBuffer_.computeNovelty(vision, config_.noveltySensitivity);
    
    // Apply temporal weighting for better novelty detection
    neuromodState_.noveltyHistory = neuromodState_.noveltyHistory * 0.7f + novelty * 0.3f;
    neuromodState_.noveltyLevel = neuromodState_.noveltyHistory * config_.noveltyDecayRate;
    
    // Update vision buffer
    visionBuffer_.update(vision);
}

void AgentBrain::updateExplorationState() {
    float explorationRate = getExplorationRate();
    
    // Update exploration timer
    explorationTimer_ += 0.001; // Assume 1ms timestep
    
    // Determine exploration state based on novelty and curiosity
    float noveltyFactor = neuromodState_.noveltyLevel / config_.maxCuriosityLevel;
    float curiosityFactor = neuromodState_.curiosityLevel / config_.maxCuriosityLevel;
    float combinedExploration = noveltyFactor * config_.noveltyDrivenExploration + 
                               curiosityFactor * config_.curiosityExplorationScaling;
    
    // State transitions
    if (combinedExploration > config_.stateTransitionThreshold_) {
        explorationState_ = ExplorationState::INTENSITY_EXPLORATION;
    } else if (combinedExploration > config_.stateTransitionThreshold_ * 0.6f) {
        explorationState_ = ExplorationState::EXPLORATION;
    } else if (explorationTimer_ > 10.0f) { // 10 seconds threshold
        explorationState_ = ExplorationState::STRATEGIC_EXPLORATION;
    } else {
        explorationState_ = ExplorationState::EXPLOITATION;
    }
}

float AgentBrain::getExplorationRate() const {
    switch (explorationState_) {
        case ExplorationState::EXPLOITATION:
            return config_.baseExplorationRate;
        case ExplorationState::EXPLORATION:
            return config_.baseExplorationRate * 2.0f;
        case ExplorationState::INTENSITY_EXPLORATION:
            return config_.baseExplorationRate * 3.0f;
        case ExplorationState::STRATEGIC_EXPLORATION:
            return config_.baseExplorationRate * 4.0f;
        default:
            return config_.baseExplorationRate;
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeuronsAdvanced();
    
    // Apply curiosity-based exploration using advanced mechanisms
    if (curiosityEnabled_ && neuromodState_.curiosityLevel > config_.curiosityThreshold) {
        decoded = selectWithCuriosityAdvanced(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeuronsAdvanced() {
    // Use optimized motor group activity calculation
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            if (n) { // Safety check
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    // Calculate activity for all motor groups
    float forwardAct = calcActivity(motorGroups_.forward);
    float backwardAct = calcActivity(motorGroups_.backward);
    float leftAct = calcActivity(motorGroups_.left);
    float rightAct = calcActivity(motorGroups_.right);
    float interactAct = calcActivity(motorGroups_.interact);
    float waitAct = calcActivity(motorGroups_.wait);
    float lookLeftAct = calcActivity(motorGroups_.lookLeft);
    float lookRightAct = calcActivity(motorGroups_.lookRight);
    
    // Use machine learning-style softmax for better decision making
    struct CommandWithScore {
        MotorCommand cmd;
        float score;
    };
    
    std::vector<CommandWithScore> commands;
    
    // Add commands with their activity scores
    if (forwardAct > 0.01f) commands.push_back({MotorCommand::MoveForward, forwardAct});
    if (backwardAct > 0.01f) commands.push_back({MotorCommand::MoveBackward, backwardAct});
    if (leftAct > 0.01f) commands.push_back({MotorCommand::TurnLeft, leftAct});
    if (rightAct > 0.01f) commands.push_back({MotorCommand::TurnRight, rightAct});
    if (interactAct > 0.01f) commands.push_back({MotorCommand::Interact, interactAct});
    if (waitAct > 0.01f) commands.push_back({MotorCommand::Wait, waitAct});
    if (lookLeftAct > 0.01f) commands.push_back({MotorCommand::LookLeft, lookLeftAct});
    if (lookRightAct > 0.01f) commands.push_back({MotorCommand::LookRight, lookRightAct});
    
    if (commands.empty()) {
        return MotorCommand::Wait;
    }
    
    // Sort by score (highest first)
    std::sort(commands.begin(), commands.end(), 
              [](const CommandWithScore& a, const CommandWithScore& b) {
                  return a.score > b.score;
              });
    
    // Apply temperature parameter for exploration (higher = more random)
    float temperature = 1.0f + (neuromodState_.curiosityLevel * 2.0f);
    float maxScore = commands[0].score;
    float expSum = 0.0f;
    
    // Calculate softmax probabilities
    std::vector<float> probabilities;
    for (const auto& cmd : commands) {
        float expScore = std::exp(cmd.score / temperature);
        probabilities.push_back(expScore);
        expSum += expScore;
    }
    
    // Normalize probabilities
    for (float& prob : probabilities) {
        prob /= expSum;
    }
    
    // Sample from probability distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());
    
    int chosenIndex = dist(gen);
    return commands[chosenIndex].cmd;
}

MotorCommand AgentBrain::selectWithCuriosityAdvanced(MotorCommand defaultCmd) {
    switch (explorationState_) {
        case ExplorationState::EXPLOITATION:
            return defaultCmd;
            
        case ExplorationState::EXPLORATION:
            return selectWithCuriosityStrategic(defaultCmd);
            
        case ExplorationState::INTENSITY_EXPLORATION:
            // High intensity: combine strategic exploration with random
            if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < 0.3f) {
                return selectWithCuriosityStrategic(defaultCmd);
            }
            return generateStrategicExplorationAction();
            
        case ExplorationState::STRATEGIC_EXPLORATION:
            return generateStrategicExplorationAction();
            
        default:
            return defaultCmd;
    }
}

MotorCommand AgentBrain::selectWithCuriosityStrategic(MotorCommand defaultCmd) {
    // Strategic exploration based on current brain state
    float curiosityFactor = neuromodState_.curiosityLevel / config_.maxCuriosityLevel;
    
    // Generate exploration candidates based on current activity patterns
    std::vector<MotorCommand> explorationCandidates;
    
    // Add default command
    explorationCandidates.push_back(defaultCmd);
    
    // Add random commands weighted by curiosity level
    std::random_device rd;
    std::mt19937 gen(rd());
    
    float exploreChance = curiosityFactor * config_.curiosityExplorationScaling;
    
    // Add some random exploration
    if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < exploreChance) {
        // Select from all possible commands
        std::vector<MotorCommand> allCommands = {
            MotorCommand::MoveForward, MotorCommand::MoveBackward,
            MotorCommand::TurnLeft, MotorCommand::TurnRight,
            MotorCommand::LookLeft, MotorCommand::LookRight,
            MotorCommand::Interact, MotorCommand::Wait
        };
        
        // Remove default command if it's wait to ensure actual exploration
        if (defaultCmd == MotorCommand::Wait) {
            explorationCandidates.insert(explorationCandidates.end(), allCommands.begin(), allCommands.end());
        } else {
            explorationCandidates.push_back(allCommands[brain_->getRandomGenerator()->uniformInt(0, 7)]);
        }
    }
    
    // Sample from candidates with softmax selection
    if (explorationCandidates.size() > 1) {
        float temperature = 1.0f + curiosityFactor;
        float expSum = 0.0f;
        
        // Calculate softmax probabilities
        std::vector<float> scores;
        for (const auto& cmd : explorationCandidates) {
            // Simple heuristic: prefer commands that haven't been used recently
            scores.push_back(1.0f); // Uniform for now
        }
        
        for (float score : scores) {
            expSum += std::exp(score / temperature);
        }
        
        // Sample
        std::vector<float> probs;
        for (float score : scores) {
            probs.push_back(std::exp(score / temperature) / expSum);
        }
        
        std::discrete_distribution<> dist(probs.begin(), probs.end());
        int chosenIndex = dist(gen);
        return explorationCandidates[chosenIndex];
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::generateStrategicExplorationAction() const {
    // Generate goal-directed exploration based on brain state analysis
    
    // Analyze current neural activity patterns
    float totalActivity = 0.0f;
    int activeRegions = 0;
    
    for (const auto& region : brain_->getRegions()) {
        float regionActivity = 0.0f;
        auto neurons = region->getAllNeurons();
        for (Neuron* n : neurons) {
            if (n) {
                regionActivity += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        
        if (regionActivity > 0.1f) {
            totalActivity += regionActivity;
            activeRegions++;
        }
    }
    
    // Generate action based on strategic analysis
    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (activeRegions > 0 && totalActivity > 0.5f) {
        // High activity: try to maintain or redirect
        if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < 0.6f) {
            return decodeFromMotorNeuronsTraditional(); // Current behavior
        } else {
            // Redirect to a different action
            std::vector<MotorCommand> alternatives = {
                MotorCommand::MoveForward, MotorCommand::MoveBackward,
                MotorCommand::TurnLeft, MotorCommand::TurnRight,
                MotorCommand::LookLeft, MotorCommand::LookRight,
                MotorCommand::Interact, MotorCommand::Wait
            };
            
            // Remove current default command
            MotorCommand defaultCmd = decodeFromMotorNeuronsTraditional();
            auto it = std::find(alternatives.begin(), alternatives.end(), defaultCmd);
            if (it != alternatives.end()) {
                alternatives.erase(it);
            }
            
            if (!alternatives.empty()) {
                return alternatives[gen() % alternatives.size()];
            }
        }
    }
    
    // Low activity: try to stimulate
    std::vector<MotorCommand> stimulatingActions = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::LookLeft, MotorCommand::LookRight,
        MotorCommand::Interact
    };
    
    return stimulatingActions[gen() % stimulatingActions.size()];
}

MotorCommand AgentBrain::decodeFromMotorNeuronsTraditional() {
    // Traditional maximum activity selection for backward compatibility
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (n) { // Safety check
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorGroups_.forward);
    float backwardAct = calcActivity(motorGroups_.backward);
    float leftAct = calcActivity(motorGroups_.left);
    float rightAct = calcActivity(motorGroups_.right);
    float interactAct = calcActivity(motorGroups_.interact);
    float waitAct = calcActivity(motorGroups_.wait);
    
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < config_.motorActivityThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    applyRewardModulationAdvanced(reward, predictedReward);
}

void AgentBrain::applyRewardModulationAdvanced(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error with temporal discounting
    neuromodState_.predictionError = reward - predictedReward;
    
    // Update expected reward (exponential moving average with configurable decay)
    float rewardDecay = 0.95f; // Could be configurable
    neuromodState_.expectedReward = rewardDecay * neuromodState_.expectedReward + 
                                   (1.0f - rewardDecay) * reward;
    
    // Dopamine-like signal (based on prediction error with range clamping)
    neuromodState_.dopamineLevel = neuromodState_.predictionError;
    neuromodState_.dopamineLevel = std::clamp(neuromodState_.dopamineLevel, 
                                            config_.dopamineMin, config_.dopamineMax);
    
    // Apply to all synapses with eligibility traces and batch processing
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change with plasticity modifier
                float delta = eligibility * neuromodState_.dopamineLevel * devState_.plasticityModifier;
                syn->addToWeight(delta);
                
                // Decay eligibility trace with configurable rate
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Update plasticity based on dopamine with configurable ranges
    updatePlasticityBasedOnDopamine();
}

void AgentBrain::updatePlasticityBasedOnDopamine() {
    // Modulate plasticity based on dopamine with sigmoidal response
    float dopamine = neuromodState_.dopamineLevel;
    
    // Convert dopamine to plasticity factor using sigmoid function
    float plasticityFactor = 0.5f + 0.5f * std::tanh(dopamine * 2.0f); // Scale for better response
    
    // Apply to STDP with configurable ranges
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Use config values with sigmoid scaling
        float ltpWeight = config_.plasticityRangeMin + 
                         (config_.plasticityRangeMax - config_.plasticityRangeMin) * plasticityFactor;
        float ltdWeight = config_.plasticityRangeMin + 
                         (config_.plasticityRangeMax - config_.plasticityRangeMin) * (1.0f - plasticityFactor);
        
        stdp->setLTPWeight(0.01f * ltpWeight);
        stdp->setLTDWeight(0.012f * ltdWeight);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    updateDevelopmentAdvanced(timestep);
}

void AgentBrain::updateDevelopmentAdvanced(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    devState_.age += timestep;
    
    // Update developmental stage using configurable thresholds
    devState_.updateStage();
    
    // Update structural plasticity with configurable rates
    updateStructuralPlasticity(timestep);
}

void AgentBrain::updateStructuralPlasticity(double timestep) {
    if (!structuralPlasticityEnabled_) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (!sp) return;
    
    // Calculate rates based on developmental stage and config
    float synRate = config_.synaptogenesisRate * devState_.plasticityModifier;
    float pruneRate = config_.pruningRate * (2.0f - devState_.plasticityModifier);
    
    sp->setSynaptogenesisRate(synRate);
    sp->setPruningRate(pruneRate);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodState_.dopamineLevel;
}

float AgentBrain::getCuriosityLevel() const {
    return neuromodState_.curiosityLevel;
}

float AgentBrain::getNoveltyLevel() const {
    return neuromodState_.noveltyLevel;
}

float AgentBrain::getPredictionError() const {
    return neuromodState_.predictionError;
}

float AgentBrain::getExpectedReward() const {
    return neuromodState_.expectedReward;
}

double AgentBrain::getDevelopmentalAge() const {
    return devState_.age;
}

void AgentBrain::reset() {
    neuromodState_.reset();
    devState_.reset();
    visionBuffer_.reset();
    
    explorationState_ = ExplorationState::EXPLOITATION;
    explorationTimer_ = 0.0f;
    
    invalidateCaches();
}

void AgentBrain::updateMotorStatsCache() const {
    motorStatsCache_.forward = motorGroups_.forward.size();
    motorStatsCache_.backward = motorGroups_.backward.size();
    motorStatsCache_.left = motorGroups_.left.size();
    motorStatsCache_.right = motorGroups_.right.size();
    motorStatsCache_.interact = motorGroups_.interact.size();
    motorStatsCache_.wait = motorGroups_.wait.size();
    motorStatsCache_.lookLeft = motorGroups_.lookLeft.size();
    motorStatsCache_.lookRight = motorGroups_.lookRight.size();
    motorStatsCacheValid_ = true;
}

void AgentBrain::updateSensoryStatsCache() const {
    sensoryStatsCache_.vision = sensoryGroups_.vision.size();
    sensoryStatsCache_.touch = sensoryGroups_.touch.size();
    sensoryStatsCache_.internal = sensoryGroups_.internal.size();
    sensoryStatsCache_.proprioception = sensoryGroups_.proprioception.size();
    sensoryStatsCacheValid_ = true;
}

AgentBrain::MotorStats AgentBrain::getMotorStats() const {
    if (!motorStatsCacheValid_) {
        updateMotorStatsCache();
    }
    return motorStatsCache_;
}

AgentBrain::SensoryStats AgentBrain::getSensoryStats() const {
    if (!sensoryStatsCacheValid_) {
        updateSensoryStatsCache();
    }
    return sensoryStatsCache_;
}

void AgentBrain::logSystemState() const {
    MotorStats motorStats = getMotorStats();
    SensoryStats sensoryStats = getSensoryStats();
    
    NLM_LOG_INFO("=== AgentBrain System State ===");
    NLM_LOG_INFO("Motor neurons: Fwd=" + std::to_string(motorStats.forward) +
                 " Bwd=" + std::to_string(motorStats.backward) +
                 " Lft=" + std::to_string(motorStats.left) +
                 " Rgt=" + std::to_string(motorStats.right) +
                 " Int=" + std::to_string(motorStats.interact) +
                 " W=" + std::to_string(motorStats.wait) +
                 " LL=" + std::to_string(motorStats.lookLeft) +
                 " LR=" + std::to_string(motorStats.lookRight));
    
    NLM_LOG_INFO("Sensory neurons: Vis=" + std::to_string(sensoryStats.vision) +
                 " Touch=" + std::to_string(sensoryStats.touch) +
                 " Int=" + std::to_string(sensoryStats.internal) +
                 " Pro=" + std::to_string(sensoryStats.proprioception));
    
    NLM_LOG_INFO("Neuromodulation: Dopamine=" + std::to_string(neuromodState_.dopamineLevel) +
                 " Novelty=" + std::to_string(neuromodState_.noveltyLevel) +
                 " Curiosity=" + std::to_string(neuromodState_.curiosityLevel) +
                 " PredErr=" + std::to_string(neuromodState_.predictionError) +
                 " ExpReward=" + std::to_string(neuromodState_.expectedReward));
    
    NLM_LOG_INFO("Development: Age=" + std::to_string(devState_.age) +
                 " Stage=" + std::to_string(static_cast<int>(devState_.stage)) +
                 " Plasticity=" + std::to_string(devState_.plasticityModifier));
    
    NLM_LOG_INFO("Exploration: State=" + std::to_string(static_cast<int>(explorationState_)) +
                 " Timer=" + std::to_string(explorationTimer_));
}

} // namespace nlm