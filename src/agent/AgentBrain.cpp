#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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

// Advanced AgentBrain methods implementation

void AgentBrain::explore(int maxSteps) {
    NLM_LOG_INFO("Starting exploration with curiosity-driven behavior for " + std::to_string(maxSteps) + " steps");
    
    float totalExploration = 0.0f;
    int explorationSteps = 0;
    
    for (int step = 0; step < maxSteps; ++step) {
        // Simple exploration loop with curiosity
        if (curiosityLevel_ > 0.3f) {
            // Random action exploration
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            MotorCommand action;
            switch (choice) {
                case 0: action = MotorCommand::MoveForward; break;
                case 1: action = MotorCommand::MoveBackward; break;
                case 2: action = MotorCommand::TurnLeft; break;
                case 3: action = MotorCommand::TurnRight; break;
                case 4: action = MotorCommand::LookLeft; break;
                case 5: action = MotorCommand::LookRight; break;
                case 6: action = MotorCommand::Interact; break;
                default: action = MotorCommand::Wait; break;
            }
            
            // Apply exploration action
            brain_->injectCurrentToNeurons(NeuronType::Motor, 20.0f);
            
            // Record exploration experience
            std::vector<float> dummyObservation(256, 0.0f);
            recordExperience(actionToString(action), dummyObservation, 0.0f, false);
            
            totalExploration += curiosityLevel_;
            explorationSteps++;
            
            // Update curiosity
            curiosityLevel_ = std::max(0.0f, curiosityLevel_ - 0.01f);
        }
        
        // Step the brain
        brain_->step(step, step * 0.001f);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("Exploration step " + std::to_string(step) + 
                        " | Curiosity: " + std::to_string(curiosityLevel_) +
                        " | Exploration rate: " + std::to_string(explorationSteps > 0 ? totalExploration / explorationSteps : 0.0f));
        }
    }
    
    NLM_LOG_INFO("Exploration completed: " + std::to_string(explorationSteps) + " exploratory steps, " +
                std::to_string(totalExploration / std::max(1, explorationSteps)) + " avg curiosity level");
}

std::vector<float> AgentBrain::getMotorCommandStrengths() const {
    std::vector<float> strengths;
    
    // Calculate activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    strengths.push_back(calcActivity(motorForward_));
    strengths.push_back(calcActivity(motorBackward_));
    strengths.push_back(calcActivity(motorTurnLeft_));
    strengths.push_back(calcActivity(motorTurnRight_));
    strengths.push_back(calcActivity(motorInteract_));
    strengths.push_back(calcActivity(motorWait_));
    
    return strengths;
}

void AgentBrain::recordExperience(const std::string& action, 
                                  const std::vector<float>& observation,
                                  float reward, bool terminal) {
    // Store experience in replay buffer for learning
    ExperienceRecord record;
    record.action = action;
    record.observation = observation;
    record.reward = reward;
    record.terminal = terminal;
    record.timestamp = developmentalAge_;
    
    // Simple replay buffer (FIFO)
    if (experienceReplay_.size() >= maxReplaySize_) {
        experienceReplay_.pop_front();
    }
    experienceReplay_.push_back(record);
    
    // Learn from experience if in learning mode
    if (learningModeEnabled_) {
        // Update action values using simple temporal difference learning
        float tdError = reward + (terminal ? 0.0f : 0.9f * expectedReward_) - expectedReward_;
        
        // Update action value (simplified)
        float actionValue = actionValues_[action] + 0.01f * tdError;
        actionValues_[action] = actionValue;
    }
}

float AgentBrain::getLearningStats() const {
    float totalValue = 0.0f;
    int count = 0;
    
    for (const auto& pair : actionValues_) {
        totalValue += pair.second;
        count++;
    }
    
    return count > 0 ? totalValue / count : 0.0f;
}

void AgentBrain::enableLearningMode(bool enabled) {
    learningModeEnabled_ = enabled;
    if (enabled) {
        NLM_LOG_INFO("Learning mode enabled");
        // Initialize action values if not already done
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        std::string action = neuronToActionString(n);
                        if (actionValues_.find(action) == actionValues_.end()) {
                            actionValues_[action] = 0.0f;
                        }
                    }
                }
            }
        }
    } else {
        NLM_LOG_INFO("Learning mode disabled");
    }
}

void AgentBrain::setExplorationRate(float rate) {
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
    NLM_LOG_INFO("Exploration rate set to " + std::to_string(explorationRate_));
}

void AgentBrain::simulateEpisode(int maxSteps) {
    NLM_LOG_INFO("Simulating episode with max steps: " + std::to_string(maxSteps));
    
    float totalReward = 0.0f;
    int steps = 0;
    
    for (int step = 0; step < maxSteps; ++step) {
        // Simple world simulation loop
        brain_->step(step, step * 0.001f);
        
        // Simple action selection based on neural activity
        MotorCommand action = decodeMotorCommand();
        
        // Apply action and get reward
        float reward = brain_->getRandomGenerator()->uniformReal(0.0f, 0.5f);
        totalReward += reward;
        
        // Record experience
        std::vector<float> observation(256, brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f));
        recordExperience(actionToString(action), observation, reward, false);
        
        steps++;
        
        // Update development
        updateDevelopment(0.001f);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Episode step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(reward) +
                        " | Total: " + std::to_string(totalReward));
        }
    }
    
    NLM_LOG_INFO("Episode simulation completed: " + std::to_string(steps) + " steps, " +
                std::to_string(totalReward) + " total reward");
}

void AgentBrain::saveExperienceReplay(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for writing: " + filename);
        return;
    }
    
    file << "Action,Observation,Reward,Terminal,Timestamp\n";
    for (const auto& record : experienceReplay_) {
        file << record.action << ",";
        file << "[";
        for (size_t i = 0; i < record.observation.size(); ++i) {
            file << record.observation[i];
            if (i < record.observation.size() - 1) file << ",";
        }
        file << "]," << record.reward << "," << (record.terminal ? "true" : "false") 
             << "," << record.timestamp << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Saved " + std::to_string(experienceReplay_.size()) + 
                " experiences to " + filename);
}

void AgentBrain::loadExperienceReplay(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for reading: " + filename);
        return;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 5) {
            ExperienceRecord record;
            record.action = tokens[0];
            
            // Parse observation (simplified - assumes format [val1,val2,...])
            std::string obsStr = tokens[1];
            if (obsStr.front() == '[' && obsStr.back() == ']') {
                obsStr = obsStr.substr(1, obsStr.size() - 2);
                std::stringstream obsStream(obsStr);
                float val;
                char comma;
                while (obsStream >> val) {
                    record.observation.push_back(val);
                    if (obsStream >> comma && comma == ',') continue;
                    else break;
                }
            }
            
            record.reward = std::stof(tokens[2]);
            record.terminal = (tokens[3] == "true");
            record.timestamp = std::stof(tokens[4]);
            
            experienceReplay_.push_back(record);
        }
    }
    
    file.close();
    NLM_LOG_INFO("Loaded " + std::to_string(experienceReplay_.size()) + 
                " experiences from " + filename);
}

int AgentBrain::getExperienceCount() const {
    return static_cast<int>(experienceReplay_.size());
}

void AgentBrain::clearExperienceReplay() {
    experienceReplay_.clear();
    NLM_LOG_INFO("Cleared experience replay buffer");
}

std::vector<float> AgentBrain::getRecentObservations(int count) const {
    std::vector<float> result;
    
    count = std::min(count, static_cast<int>(experienceReplay_.size()));
    
    // Extract recent observations (simplified approach)
    int startIndex = std::max(0, static_cast<int>(experienceReplay_.size()) - count);
    for (int i = startIndex; i < static_cast<int>(experienceReplay_.size()); ++i) {
        for (float val : experienceReplay_[i].observation) {
            result.push_back(val);
        }
    }
    
    return result;
}

std::vector<std::string> AgentBrain::getActionHistory(int count) const {
    std::vector<std::string> result;
    
    count = std::min(count, static_cast<int>(experienceReplay_.size()));
    
    int startIndex = std::max(0, static_cast<int>(experienceReplay_.size()) - count);
    for (int i = startIndex; i < static_cast<int>(experienceReplay_.size()); ++i) {
        result.push_back(experienceReplay_[i].action);
    }
    
    return result;
}

// Helper methods
std::string AgentBrain::actionToString(MotorCommand cmd) const {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}

std::string AgentBrain::neuronToActionString(Neuron* neuron) const {
    // Simple mapping from neuron to action (simplified)
    return "Action_" + std::to_string(neuron->getId().value);
}

} // namespace nlm
