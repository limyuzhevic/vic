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
    // Advanced learning systems
    , knowledgeBase_()
    , taskPriorities_()
    , skillLevels_()
    
    // State management
    , goalPosition_()
    , hasGoal_(false)
    , threatDetectionEnabled_(true)
    , socialLearningEnabled_(false)
    , multiTaskLearning_(false)
    , transferLearningEnabled_(false)
    , hierarchicalRL_(false)
    , metaCognitionEnabled_(false)
    
    // Cognitive state
    , decisionConfidence_(0.5f)
    , uncertainty_(0.5f)
    
    // Additional learning parameters
    , learningRate_(0.01f)
    , explorationRate_(0.2f)
{
    // Initialize performance metrics
    performanceMetrics_.averageReward = 0.0f;
    performanceMetrics_.successRate = 0.0f;
    performanceMetrics_.explorationEfficiency = 0.0f;
    performanceMetrics_.learningSpeed = 0.0f;
    
    // Initialize personality traits
    personality_.curiosity = 0.5f;
    personality_.boldness = 0.5f;
    personality_.exploration = 0.5f;
    personality_.learning = 0.5f;
    
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

    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// === Advanced Agent Behaviors and Control Systems Implementations ===

std::string AgentBrain::getBehaviorProfile() const {
    std::stringstream profile;
    profile << "{\n";
    profile << "  \"curiosity\": " << curiosityLevel_ << ",\n";
    profile << "  \"novelty\": " << noveltyLevel_ << ",\n";
    profile << "  \"dopamine\": " << dopamineLevel_ << ",\n";
    profile << "  \"exploration_rate\": " << explorationRate_ << ",\n";
    profile << "  \"learning_rate\": " << learningRate_ << ",\n";
    profile << "  \"boldness\": " << personality_.boldness << ",\n";
    profile << "  \"development_stage\": \"";
    
    switch (getDevelopmentalStage()) {
        case DevelopmentalStage::Initial: profile << "Initial"; break;
        case DevelopmentalStage::CriticalPeriod: profile << "CriticalPeriod"; break;
        case DevelopmentalStage::Maturation: profile << "Maturation"; break;
        case DevelopmentalStage::Adult: profile << "Adult"; break;
        case DevelopmentalStage::Aging: profile << "Aging"; break;
    }
    profile << "\",\n";
    profile << "  \"reward_modulation\": " << (rewardModulationEnabled_ ? "true" : "false") << ",\n";
    profile << "  \"curiosity_enabled\": " << (curiosityEnabled_ ? "true" : "false") << "\n";
    profile << "}\n";
    return profile.str();
}

void AgentBrain::setPersonality(float curiosity, float boldness, float exploration, float learning) {
    personality_.curiosity = std::clamp(curiosity, 0.0f, 1.0f);
    personality_.boldness = std::clamp(boldness, 0.0f, 1.0f);
    personality_.exploration = std::clamp(exploration, 0.0f, 1.0f);
    personality_.learning = std::clamp(learning, 0.0f, 1.0f);
    
    NLM_LOG_INFO("Agent personality set: curiosity=" + std::to_string(personality_.curiosity) +
                 ", boldness=" + std::to_string(personality_.boldness) +
                 ", exploration=" + std::to_string(personality_.exploration) +
                 ", learning=" + std::to_string(personality_.learning));
}

void AgentBrain::adaptiveLearning(const std::vector<float>& rewards, float successRate) {
    // Simple adaptive learning: adjust parameters based on performance
    if (rewards.empty()) return;
    
    float avgReward = std::accumulate(rewards.begin(), rewards.end(), 0.0f) / rewards.size();
    
    // Adjust learning rate based on stability (lower if rewards are volatile)
    float rewardVariance = 0.0f;
    for (float reward : rewards) {
        rewardVariance += (reward - avgReward) * (reward - avgReward);
    }
    rewardVariance /= rewards.size();
    
    if (rewardVariance > 0.1f) {
        // High variance - reduce learning rate to stabilize
        learningRate_ *= 0.9f;
        NLM_LOG_INFO("Adaptive learning: High reward variance, reducing learning rate to " + std::to_string(learningRate_));
    } else {
        // Low variance - can increase learning rate
        learningRate_ = std::min(learningRate_ * 1.1f, 0.1f);
        NLM_LOG_INFO("Adaptive learning: Low reward variance, increasing learning rate to " + std::to_string(learningRate_));
    }
    
    // Adjust exploration rate based on success rate
    if (successRate > 0.8f) {
        // High success - reduce exploration for exploitation
        explorationRate_ *= 0.9f;
    } else if (successRate < 0.3f) {
        // Low success - increase exploration for discovery
        explorationRate_ = std::min(explorationRate_ * 1.2f, 0.5f);
    }
    
    // Update performance metrics
    performanceMetrics_.averageReward = avgReward;
    performanceMetrics_.successRate = successRate;
    performanceMetrics_.explorationEfficiency = (successRate > 0.0f) ? (avgReward / successRate) : 0.0f;
}

void AgentBrain::resetAll() {
    // Reset all states
    reset(); // Call the existing reset function
    
    // Reset advanced learning systems
    knowledgeBase_.clear();
    taskPriorities_.clear();
    skillLevels_.clear();
    
    // Reset state management
    hasGoal_ = false;
    goalPosition_ = AgentBody();
    
    // Reset cognitive state
    decisionConfidence_ = 0.5f;
    uncertainty_ = 0.5f;
    
    // Reset additional learning parameters
    learningRate_ = 0.01f;
    explorationRate_ = 0.2f;
    
    // Reset personality
    personality_.curiosity = 0.5f;
    personality_.boldness = 0.5f;
    personality_.exploration = 0.5f;
    personality_.learning = 0.5f;
    
    // Reset performance metrics
    performanceMetrics_.averageReward = 0.0f;
    performanceMetrics_.successRate = 0.0f;
    performanceMetrics_.explorationEfficiency = 0.0f;
    performanceMetrics_.learningSpeed = 0.0f;
    
    NLM_LOG_INFO("Agent reset complete - all advanced behaviors cleared");
}

bool AgentBrain::saveState(const std::string& filepath) {
    // Simple checkpoint implementation
    // In a real implementation, this would use serialization
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    try {
        // Write key state variables
        file.write(reinterpret_cast<const char*>(&curiosityLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&noveltyLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&dopamineLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&predictionError_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&expectedReward_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&developmentalAge_), sizeof(double));
        file.write(reinterpret_cast<const char*>(&plasticityModifier_), sizeof(float));
        
        // Write personality
        file.write(reinterpret_cast<const char*>(&personality_.curiosity), sizeof(float));
        file.write(reinterpret_cast<const char*>(&personality_.boldness), sizeof(float));
        file.write(reinterpret_cast<const char*>(&personality_.exploration), sizeof(float));
        file.write(reinterpret_cast<const char*>(&personality_.learning), sizeof(float));
        
        // Write flags
        file.write(reinterpret_cast<const char*>(&rewardModulationEnabled_), sizeof(bool));
        file.write(reinterpret_cast<const char*>(&curiosityEnabled_), sizeof(bool));
        
        NLM_LOG_INFO("Agent state saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save agent state: " + std::string(e.what()));
        return false;
    }
}

bool AgentBrain::loadState(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filepath);
        return false;
    }
    
    try {
        // Read key state variables
        file.read(reinterpret_cast<char*>(&curiosityLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&noveltyLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&dopamineLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&predictionError_), sizeof(float));
        file.read(reinterpret_cast<char*>(&expectedReward_), sizeof(float));
        file.read(reinterpret_cast<char*>(&developmentalAge_), sizeof(double));
        file.read(reinterpret_cast<char*>(&plasticityModifier_), sizeof(float));
        
        // Read personality
        file.read(reinterpret_cast<char*>(&personality_.curiosity), sizeof(float));
        file.read(reinterpret_cast<char*>(&personality_.boldness), sizeof(float));
        file.read(reinterpret_cast<char*>(&personality_.exploration), sizeof(float));
        file.read(reinterpret_cast<char*>(&personality_.learning), sizeof(float));
        
        // Read flags
        file.read(reinterpret_cast<char*>(&rewardModulationEnabled_), sizeof(bool));
        file.read(reinterpret_cast<char*>(&curiosityEnabled_), sizeof(bool));
        
        NLM_LOG_INFO("Agent state loaded from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load agent state: " + std::string(e.what()));
        return false;
    }
}

std::string AgentBrain::getStateSummary() const {
    std::stringstream summary;
    summary << "=== Agent State Summary ===\n";
    summary << "Curiosity Level: " << curiosityLevel_ << "\n";
    summary << "Novelty Level: " << noveltyLevel_ << "\n";
    summary << "Dopamine Level: " << dopamineLevel_ << "\n";
    summary << "Learning Rate: " << learningRate_ << "\n";
    summary << "Exploration Rate: " << explorationRate_ << "\n";
    summary << "Development Stage: ";
    
    switch (getDevelopmentalStage()) {
        case DevelopmentalStage::Initial: summary << "Initial"; break;
        case DevelopmentalStage::CriticalPeriod: summary << "CriticalPeriod"; break;
        case DevelopmentalStage::Maturation: summary << "Maturation"; break;
        case DevelopmentalStage::Adult: summary << "Adult"; break;
        case DevelopmentalStage::Aging: summary << "Aging"; break;
    }
    summary << "\n";
    summary << "Personality - Curiosity: " << personality_.curiosity << ", Boldness: " << personality_.boldness << ", ";
    summary << "Exploration: " << personality_.exploration << ", Learning: " << personality_.learning << "\n";
    summary << "Performance - Success Rate: " << performanceMetrics_.successRate * 100 << "%, ";
    summary << "Average Reward: " << performanceMetrics_.averageReward << "\n";
    summary << "=== End State Summary ===\n";
    return summary.str();
}

std::shared_ptr<AgentBrain> AgentBrain::createFromConfig(const std::shared_ptr<Config>& config) {
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for agent creation");
        return nullptr;
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Apply configuration settings
    if (config->has("agent.curiosity_level")) {
        agent->setCuriosityLevel(config->getOr<float>("agent.curiosity_level", 0.5f));
    }
    
    if (config->has("agent.learning_rate")) {
        agent->setLearningRate(config->getOr<float>("agent.learning_rate", 0.01f));
    }
    
    if (config->has("agent.exploration_rate")) {
        agent->setExplorationRate(config->getOr<float>("agent.exploration_rate", 0.2f));
    }
    
    // Enable features based on config
    if (config->has("agent.reward_modulation")) {
        agent->enableRewardModulation(config->getOr<bool>("agent.reward_modulation", true));
    }
    
    if (config->has("agent.curiosity_enabled")) {
        agent->enableCuriosity(config->getOr<bool>("agent.curiosity_enabled", true));
    }
    
    NLM_LOG_INFO("Agent created from configuration");
    return agent;
}

void AgentBrain::setTaskPriority(const std::string& task, float priority) {
    taskPriorities_[task] = std::clamp(priority, 0.0f, 1.0f);
}

float AgentBrain::getTaskPriority(const std::string& task) const {
    auto it = taskPriorities_.find(task);
    return (it != taskPriorities_.end()) ? it->second : 0.0f;
}

void AgentBrain::setSkillLevel(const std::string& skill, int level) {
    skillLevels_[skill] = level;
}

int AgentBrain::getSkillLevel(const std::string& skill) const {
    auto it = skillLevels_.find(skill);
    return (it != skillLevels_.end()) ? it->second : 0;
}

void AgentBrain::setKnowledgeBase(const std::map<std::string, std::vector<float>>& knowledge) {
    knowledgeBase_ = knowledge;
}

std::vector<float> AgentBrain::applyTransferLearning(const std::string& task) {
    auto it = knowledgeBase_.find(task);
    if (it != knowledgeBase_.end()) {
        return it->second;
    }
    return std::vector<float>();
}

} // namespace nlm
