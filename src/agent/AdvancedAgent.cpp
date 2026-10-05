// Advanced Agent implementation
#include "AdvancedAgent.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

namespace nlm {

AdvancedAgent::AdvancedAgent(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineBaseline_(0.0f)
    , dopamineSensitivity_(1.0f)
    , curiosityExplorationFactor_(0.3f)
    , curiosityExploitationFactor_(0.7f)
    , noveltyDetectionThreshold_(0.5f)
    , noveltyDecayRate_(0.99f)
    , metaLearningEnabled_(false)
    , transferLearningEnabled_(false)
    , selfSupersvisionEnabled_(false)
    , experimentalModeEnabled_(false)
    , memoryPruningEnabled_(false)
    , memoryCapacity_(10000)
    , currentMemoryUsage_(0)
    , environmentModelEnabled_(false)
    , socialLearningEnabled_(false)
    , safetyConstraintsEnabled_(false)
    , performanceOptimizationEnabled_(false)
    , visualizationEnabled_(false)
    , realTimeLoggingEnabled_(false)
    , currentDevelopmentStage_(0)
    , actionStrategy_(ActionSelectionStrategy::Deterministic)
    , currentMemoryUsage_(0)
    , currentDevelopmentStage_(0)
{
}

AdvancedAgent::~AdvancedAgent() = default;

bool AdvancedAgent::initialize(std::shared_ptr<SimpleWorld> world, std::shared_ptr<Config> config) {
    if (!brain_) {
        NLM_LOG_ERROR("Brain pointer is null");
        return false;
    }
    
    world_ = world;
    config_ = config;
    
    // Initialize advanced systems
    initializeAdvancedNeuromodulation();
    initializeAdvancedPlasticity();
    initializeDevelopmentSystems();
    initializeMemorySystems();
    
    // Load configuration if provided
    if (config_) {
        loadAdvancedConfiguration();
    }
    
    // Initialize episode tracking
    currentEpisode_.reset();
    currentEpisode_.episodeNumber = 0;
    currentEpisode_.startTime = 0.0;
    currentEpisode_.outcome = "ongoing";
    
    NLM_LOG_INFO("AdvancedAgent initialized successfully");
    return true;
}

void AdvancedAgent::initializeAdvancedNeuromodulation() {
    // Initialize dopamine system
    dopamineBaseline_ = config_ ? config_->getOr<float>("neuromod.dopamine.baseline", 0.0f) : 0.0f;
    dopamineSensitivity_ = config_ ? config_->getOr<float>("neuromod.dopamine.sensitivity", 1.0f) : 1.0f;
    
    // Initialize curiosity system
    curiosityExplorationFactor_ = config_ ? config_->getOr<float>("neuromod.curiosity.exploration", 0.3f) : 0.3f;
    curiosityExploitationFactor_ = config_ ? config_->getOr<float>("neuromod.curiosity.exploitation", 0.7f) : 0.7f;
    
    // Initialize novelty detection
    noveltyDetectionThreshold_ = config_ ? config_->getOr<float>("neuromod.novelty.threshold", 0.5f) : 0.5f;
    noveltyDecayRate_ = config_ ? config_->getOr<float>("neuromod.novelty.decay", 0.99f) : 0.99f;
    
    NLM_LOG_INFO("Advanced neuromodulation systems initialized");
}

void AdvancedAgent::initializeAdvancedPlasticity() {
    // Configure STDP with advanced parameters
    if (config_) {
        float ltpWeight = config_->getOr<float>("plasticity.stdp.ltp_weight", 0.01f);
        float ltdWeight = config_->getOr<float>("plasticity.stdp.ltd_weight", 0.012f);
        float tau = config_->getOr<float>("plasticity.stdp.tau", 20.0f);
        configureCustomSTDP(ltpWeight, ltdWeight, tau);
    }
    
    // Initialize other plasticity rules
    plasticityRules_["stdp"] = 1.0f;
    plasticityRules_["hebbian"] = 1.0f;
    plasticityRules_["structural"] = 1.0f;
    
    for (const auto& rule : plasticityRules_) {
        plasticityRuleEnabled_[rule.first] = true;
    }
    
    NLM_LOG_INFO("Advanced plasticity systems initialized");
}

void AdvancedAgent::initializeDevelopmentSystems() {
    if (config_) {
        // Load developmental schedule
        std::string scheduleStr = config_->getOr<std::string>("development.schedule", "0,60,300,900");
        developmentalSchedule_ = parseDevelopmentalSchedule(scheduleStr);
        
        // Load plasticity schedule
        std::string plasticityStr = config_->getOr<std::string>("development.plasticity_schedule", "1.0,0.8,0.5,0.2");
        plasticitySchedule_ = parsePlasticitySchedule(plasticityStr);
    }
    
    currentDevelopmentStage_ = 0;
    NLM_LOG_INFO("Advanced development systems initialized");
}

void AdvancedAgent::initializeMemorySystems() {
    if (config_) {
        memoryCapacity_ = config_->getOr<size_t>("memory.capacity", 10000);
        memoryPruningEnabled_ = config_->getOr<bool>("memory.pruning", false);
    }
    
    currentMemoryUsage_ = 0;
    NLM_LOG_INFO("Advanced memory systems initialized");
}

void AdvancedAgent::loadAdvancedConfiguration() {
    // Load advanced configuration parameters
    if (config_->has("advanced.meta_learning")) {
        metaLearningEnabled_ = config_->get<bool>("advanced.meta_learning").value_or(false);
    }
    
    if (config_->has("advanced.transfer_learning")) {
        transferLearningEnabled_ = config_->get<bool>("advanced.transfer_learning").value_or(false);
    }
    
    if (config_->has("advanced.self_supersvision")) {
        selfSupersvisionEnabled_ = config_->get<bool>"advanced.self_supersvision").value_or(false);
    }
    
    if (config_->has("advanced.experimental_mode")) {
        experimentalModeEnabled_ = config_->get<bool>("advanced.experimental_mode").value_or(false);
    }
    
    if (config_->has("advanced.social_learning")) {
        socialLearningEnabled_ = config_->get<bool>("advanced.social_learning").value_or(false);
    }
    
    if (config_->has("advanced.safety_constraints")) {
        safetyConstraintsEnabled_ = config_->get<bool>("advanced.safety_constraints").value_or(false);
    }
    
    if (config_->has("advanced.performance_optimization")) {
        performanceOptimizationEnabled_ = config_->get<bool>("advanced.performance_optimization").value_or(false);
    }
    
    if (config_->has("advanced.visualization")) {
        visualizationEnabled_ = config_->get<bool>("advanced.visualization").value_or(false);
    }
    
    if (config_->has("advanced.real_time_logging")) {
        realTimeLoggingEnabled_ = config_->get<bool>("advanced.real_time_logging").value_or(false);
    }
}

void AdvancedAgent::setPlasticityRules(const std::string& ruleName, float strength) {
    if (plasticityRules_.find(ruleName) != plasticityRules_.end()) {
        plasticityRules_[ruleName] = strength;
        NLM_LOG_INFO("Plasticity rule '" + ruleName + "' set to strength " + std::to_string(strength));
    } else {
        NLM_LOG_WARNING("Unknown plasticity rule: " + ruleName);
    }
}

void AdvancedAgent::enablePlasticityRule(const std::string& ruleName, bool enable) {
    if (plasticityRuleEnabled_.find(ruleName) != plasticityRuleEnabled_.end()) {
        plasticityRuleEnabled_[ruleName] = enable;
        NLM_LOG_INFO("Plasticity rule '" + ruleName + "' " + (enable ? "enabled" : "disabled"));
    }
}

void AdvancedAgent::configureCustomSTDP(float ltpWeight, float ltdWeight, float tau) {
    customSTDPParameters_["ltp_weight"] = ltpWeight;
    customSTDPParameters_["ltd_weight"] = ltdWeight;
    customSTDPParameters_["tau"] = tau;
    
    if (brain_ && brain_->getSTDP()) {
        brain_->getSTDP()->configure(ltpWeight, ltdWeight, tau);
    }
    
    NLM_LOG_INFO("Custom STDP configured: LTP=" + std::to_string(ltpWeight) + 
                 ", LTD=" + std::to_string(ltdWeight) + ", tau=" + std::to_string(tau));
}

std::string AdvancedAgent::getVisualizationData() const {
    std::ostringstream oss;
    oss << "=== Advanced Agent Visualization ===\n";
    oss << "Neuron Count: " << brain_->getTotalNeuronCount() << "\n";
    oss << "Synapse Count: " << brain_->getTotalSynapseCount() << "\n";
    oss << "Developmental Stage: " << currentDevelopmentStage_ << "\n";
    oss << "Memory Usage: " << currentMemoryUsage_ << " / " << memoryCapacity_ << "\n";
    oss << "Meta Learning: " << (metaLearningEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Transfer Learning: " << (transferLearningEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Self Supervision: " << (selfSupersvisionEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Experimental Mode: " << (experimentalModeEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Social Learning: " << (socialLearningEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Safety Constraints: " << (safetyConstraintsEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Performance Optimization: " << (performanceOptimizationEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Visualization: " << (visualizationEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Real-time Logging: " << (realTimeLoggingEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "=== Current Performance Metrics ===\n";
    oss << "Steps: " << performanceMetrics_.totalSteps << "\n";
    oss << "Successful Actions: " << performanceMetrics_.successfulActions << "\n";
    oss << "Average Curiosity: " << performanceMetrics_.averageCuriosity << "\n";
    oss << "Average Novelty: " << performanceMetrics_.averageNovelty << "\n";
    oss << "Average Prediction Error: " << performanceMetrics_.averagePredictionError << "\n";
    oss << "Learning Rate: " << performanceMetrics_.learningRate << "\n";
    oss << "Simulation Time: " << performanceMetrics_.simulationTime << "s\n";
    oss << "=== Current Episode ===\n";
    oss << "Episode Number: " << currentEpisode_.episodeNumber << "\n";
    oss << "Start Time: " << currentEpisode_.startTime << "\n";
    oss << "End Time: " << currentEpisode_.endTime << "\n";
    oss << "Total Reward: " << currentEpisode_.totalReward << "\n";
    oss << "Steps: " << currentEpisode_.steps << "\n";
    oss << "Final Curiosity: " << currentEpisode_.finalCuriosity << "\n";
    oss << "Outcome: " << currentEpisode_.outcome << "\n";
    
    return oss.str();
}

void AdvancedAgent::configureDopamineSystem(float baseline, float sensitivity) {
    dopamineBaseline_ = baseline;
    dopamineSensitivity_ = sensitivity;
    NLM_LOG_INFO("Dopamine system configured: baseline=" + std::to_string(baseline) + 
                 ", sensitivity=" + std::to_string(sensitivity));
}

void AdvancedAgent::configureCuriositySystem(float explorationFactor, float exploitationFactor) {
    curiosityExplorationFactor_ = explorationFactor;
    curiosityExploitationFactor_ = exploitationFactor;
    NLM_LOG_INFO("Curiosity system configured: exploration=" + std::to_string(explorationFactor) + 
                 ", exploitation=" + std::to_string(exploitationFactor));
}

void AdvancedAgent::configureNoveltySystem(float detectionThreshold, float decayRate) {
    noveltyDetectionThreshold_ = detectionThreshold;
    noveltyDecayRate_ = decayRate;
    NLM_LOG_INFO("Novelty system configured: threshold=" + std::to_string(detectionThreshold) + 
                 ", decay_rate=" + std::to_string(decayRate));
}

void AdvancedAgent::setDevelopmentalSchedule(const std::vector<double>& schedule) {
    developmentalSchedule_ = schedule;
    NLM_LOG_INFO("Developmental schedule updated: " + std::to_string(developmentalSchedule_.size()) + " stages");
}

void AdvancedAgent::setPlasticitySchedule(const std::vector<float>& schedule) {
    plasticitySchedule_ = schedule;
    NLM_LOG_INFO("Plasticity schedule updated: " + std::to_string(plasticitySchedule_.size()) + " stages");
}

void AdvancedAgent::setExperimentalParameter(const std::string& name, float value) {
    experimentalParameters_[name] = value;
    NLM_LOG_INFO("Experimental parameter '" + name + "' set to " + std::to_string(value));
}

float AdvancedAgent::getExperimentalParameter(const std::string& name) const {
    auto it = experimentalParameters_.find(name);
    if (it != experimentalParameters_.end()) {
        return it->second;
    }
    return 0.0f;
}

void AdvancedAgent::addSocialNeighbor(std::shared_ptr<AdvancedAgent> neighbor) {
    if (neighbor) {
        socialNeighbors_.push_back(neighbor);
        NLM_LOG_INFO("Added social neighbor to agent pool");
    }
}

void AdvancedAgent::observeAgent(const AdvancedAgent& otherAgent) {
    if (!socialLearningEnabled_) return;
    
    // Observe other agent's performance metrics
    const auto& otherMetrics = otherAgent.getPerformanceMetrics();
    
    // Learn from successful strategies
    if (otherMetrics.successfulActions > performanceMetrics_.successfulActions) {
        NLM_LOG_INFO("Learning from superior agent performance");
        // Apply learned strategies
        applySocialLearning(otherAgent);
    }
    
    NLM_LOG_INFO("Observed agent performance: curiosity=" + std::to_string(otherMetrics.averageCuriosity) + 
                 ", success_rate=" + std::to_string(static_cast<float>(otherMetrics.successfulActions) / otherMetrics.totalSteps));
}

void AdvancedAgent::applySocialLearning(const AdvancedAgent& otherAgent) {
    // Apply various social learning mechanisms
    if (otherAgent.metaLearningEnabled_) {
        // Apply meta-learning strategies
        NLM_LOG_INFO("Applying meta-learning strategies from peer");
    }
    
    if (otherAgent.transferLearningEnabled_) {
        // Apply transfer learning
        NLM_LOG_INFO("Applying transfer learning from peer");
    }
    
    if (otherAgent.selfSupersvisionEnabled_) {
        // Apply self-supervised learning
        NLM_LOG_INFO("Applying self-supervised learning from peer");
    }
}

bool AdvancedAgent::checkSafetyConstraints(const Action& action) const {
    if (!safetyConstraintsEnabled_) return true;
    
    for (const auto& constraint : safetyConstraints_) {
        if (!constraint.second(action)) {
            NLM_LOG_WARNING("Safety constraint '" + constraint.first + "' violated by action");
            return false;
        }
    }
    
    return true;
}

void AdvancedAgent::addSafetyConstraint(const std::string& constraintName, std::function<bool(const Action&)> constraint) {
    safetyConstraints_[constraintName] = constraint;
    NLM_LOG_INFO("Added safety constraint: " + constraintName);
}

MotorCommand AdvancedAgent::selectActionAdvanced() {
    switch (actionStrategy_) {
        case ActionSelectionStrategy::Deterministic:
            return selectActionDeterministic();
        case ActionSelectionStrategy::Probabilistic:
            return selectActionProbabilistic();
        case ActionSelectionStrategy::CuriosityDriven:
            return selectActionCuriosityDriven();
        case ActionSelectionStrategy::ExplorationExploitation:
            return selectActionExplorationExploitation();
        case ActionSelectionStrategy::Ensemble:
            return selectActionEnsemble();
        default:
            return MotorCommand::Wait;
    }
}

MotorCommand AdvancedAgent::selectActionDeterministic() {
    // Use deterministic selection based on highest neural activity
    return brain_->produceAction()->getType();
}

MotorCommand AdvancedAgent::selectActionProbabilistic() {
    // Use probabilistic selection based on neural activity distribution
    auto* agentBrain = dynamic_cast<AgentBrain*>(brain_.get());
    if (!agentBrain) return MotorCommand::Wait;
    
    return agentBrain->decodeMotorCommand();
}

MotorCommand AdvancedAgent::selectActionCuriosityDriven() {
    // Select action based on curiosity level and exploration bonus
    auto* agentBrain = dynamic_cast<AgentBrain*>(brain_.get());
    if (!agentBrain) return MotorCommand::Wait;
    
    MotorCommand baseAction = agentBrain->decodeMotorCommand();
    
    // Apply curiosity-based exploration
    if (agentBrain->getCuriosityLevel() > 0.3f) {
        float explorationChance = agentBrain->getCuriosityLevel() * curiosityExplorationFactor_;
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        
        if (r < explorationChance) {
            // Select random action
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
    
    return baseAction;
}

MotorCommand AdvancedAgent::selectActionExplorationExploitation() {
    // Balance between exploration and exploitation based on curiosity
    auto* agentBrain = dynamic_cast<AgentBrain*>(brain_.get());
    if (!agentBrain) return MotorCommand::Wait;
    
    float curiosity = agentBrain->getCuriosityLevel();
    
    // Calculate exploration vs exploitation ratio
    float explorationRatio = curiosityExplorationFactor_ * curiosity + (1.0f - curiosityExploitationFactor_) * (1.0f - curiosity);
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    
    if (r < explorationRatio) {
        // Exploration: choose random action
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
    } else {
        // Exploitation: use learned policy
        return agentBrain->decodeMotorCommand();
    }
}

MotorCommand AdvancedAgent::selectActionEnsemble() {
    // Ensemble method: combine multiple action selection strategies
    std::vector<MotorCommand> candidateActions;
    std::vector<float> weights;
    
    // Get actions from different strategies
    candidateActions.push_back(selectActionDeterministic());
    candidateActions.push_back(selectActionProbabilistic());
    
    auto* agentBrain = dynamic_cast<AgentBrain*>(brain_.get());
    if (agentBrain) {
        candidateActions.push_back(agentBrain->decodeMotorCommand());
    }
    
    // Assign weights based on performance
    for (size_t i = 0; i < candidateActions.size(); ++i) {
        weights.push_back(1.0f / candidateActions.size());
    }
    
    // Select action using weighted random choice
    float totalWeight = 0.0f;
    for (float w : weights) totalWeight += w;
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, totalWeight);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < candidateActions.size(); ++i) {
        cumulative += weights[i];
        if (r <= cumulative) {
            return candidateActions[i];
        }
    }
    
    return candidateActions.empty() ? MotorCommand::Wait : candidateActions.back();
}

void AdvancedAgent::setRewardFunction(std::function<float(const SensoryPercept&, const Action&)> rewardFunc) {
    extrinsicRewardFunction_ = rewardFunc;
    NLM_LOG_INFO("Extrinsic reward function set");
}

void AdvancedAgent::setIntrinsicRewardFunction(std::function<float(const SensoryPercept&)> intrinsicFunc) {
    intrinsicRewardFunction_ = intrinsicFunc;
    NLM_LOG_INFO("Intrinsic reward function set");
}

void AdvancedAgent::updateEnvironmentModel(const SensoryPercept& percept, const Action& action, float reward) {
    if (!environmentModelEnabled_) return;
    
    // Update environment model with new experience
    std::string stateKey = perceptToKey(percept);
    std::string actionKey = actionToKey(action);
    
    // Update state-action-reward mapping
    environmentModel_[stateKey + " -> " + actionKey] = reward;
    
    // Occasionally prune old entries if model grows too large
    if (environmentModel_.size() > 10000) {
        pruneEnvironmentModel();
    }
}

float AdvancedAgent::predictFutureReward(const SensoryPercept& percept, const Action& action) const {
    if (!environmentModelEnabled_) return 0.0f;
    
    std::string stateKey = perceptToKey(percept);
    std::string actionKey = actionToKey(action);
    
    std::string key = stateKey + " -> " + actionKey;
    auto it = environmentModel_.find(key);
    
    if (it != environmentModel_.end()) {
        return it->second;
    }
    
    // Return average reward if not found
    if (!environmentModel_.empty()) {
        float totalReward = 0.0f;
        for (const auto& pair : environmentModel_) {
            totalReward += pair.second;
        }
        return totalReward / environmentModel_.size();
    }
    
    return 0.0f;
}

void AdvancedAgent::setOptimizationStrategy(const std::string& strategy) {
    optimizationStrategy_ = strategy;
    NLM_LOG_INFO("Performance optimization strategy set to: " + strategy);
}

double AdvancedAgent::getEstimatedCompletionTime(size_t steps) const {
    if (!performanceOptimizationEnabled_) return -1.0;
    
    // Estimate completion time based on historical performance
    double avgStepTime = performanceMetrics_.simulationTime / performanceMetrics_.totalSteps;
    return avgStepTime * steps;
}

bool AdvancedAgent::saveAdvancedState(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    try {
        // Serialize advanced state
        // This is a simplified version - full implementation would include all state
        file.write(reinterpret_cast<const char*>(&performanceMetrics_), sizeof(performanceMetrics_));
        file.write(reinterpret_cast<const char*>(&currentEpisode_), sizeof(currentEpisode_));
        file.write(reinterpret_cast<const char*>(&experimentalParameters_), sizeof(experimentalParameters_));
        
        file.close();
        NLM_LOG_INFO("Advanced agent state saved to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving advanced state: ") + e.what());
        file.close();
        return false;
    }
}

bool AdvancedAgent::loadAdvancedState(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filepath);
        return false;
    }
    
    try {
        // Deserialize advanced state
        file.read(reinterpret_cast<char*>(&performanceMetrics_), sizeof(performanceMetrics_));
        file.read(reinterpret_cast<char*>(&currentEpisode_), sizeof(currentEpisode_));
        file.read(reinterpret_cast<char*>(&experimentalParameters_), sizeof(experimentalParameters_));
        
        file.close();
        NLM_LOG_INFO("Advanced agent state loaded from: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading advanced state: ") + e.what());
        file.close();
        return false;
    }
}

std::vector<AdvancedAgent> AdvancedAgent::createAgentBatch(size_t count, std::shared_ptr<Brain> brainTemplate) {
    std::vector<AdvancedAgent> agents;
    agents.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        AdvancedAgent agent(brainTemplate);
        agents.push_back(std::move(agent));
    }
    
    NLM_LOG_INFO(std::to_string(count) + " advanced agents created in batch");
    return agents;
}

void AdvancedAgent::runBatchSimulation(std::vector<AdvancedAgent>& agents, std::shared_ptr<SimpleWorld> world, 
                                       size_t stepsPerAgent, bool parallel) {
    if (parallel) {
        // Parallel execution (simplified)
        std::vector<std::thread> threads;
        for (size_t i = 0; i < agents.size(); ++i) {
            threads.emplace_back([&, i]() {
                agents[i].initialize(world, agents[i].config_);
                agents[i].runEpisode(stepsPerAgent);
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    } else {
        // Sequential execution
        for (auto& agent : agents) {
            agent.initialize(world, agent.config_);
            agent.runEpisode(stepsPerAgent);
        }
    }
    
    NLM_LOG_INFO("Batch simulation completed: " + std::to_string(agents.size()) + " agents, " + 
                 std::to_string(stepsPerAgent) + " steps each");
}

void AdvancedAgent::updatePerformanceMetrics() {
    auto* agentBrain = dynamic_cast<AgentBrain*>(brain_.get());
    if (!agentBrain) return;
    
    performanceMetrics_.totalSteps++;
    
    if (performanceMetrics_.totalSteps > 0) {
        // Update moving averages
        performanceMetrics_.averageCuriosity = 
            (performanceMetrics_.averageCuriosity * (performanceMetrics_.totalSteps - 1) + 
             agentBrain->getCuriosityLevel()) / performanceMetrics_.totalSteps;
        
        performanceMetrics_.averageNovelty = 
            (performanceMetrics_.averageNovelty * (performanceMetrics_.totalSteps - 1) + 
             agentBrain->getNoveltyLevel()) / performanceMetrics_.totalSteps;
        
        performanceMetrics_.averagePredictionError = 
            (performanceMetrics_.averagePredictionError * (performanceMetrics_.totalSteps - 1) + 
             agentBrain->getPredictionError()) / performanceMetrics_.totalSteps;
    }
    
    // Update memory usage
    currentMemoryUsage_ = brain_->getTotalNeuronCount() + brain_->getTotalSynapseCount();
    
    if (realTimeLoggingEnabled_) {
        logPerformanceMetrics();
    }
}

void AdvancedAgent::logPerformanceMetrics() {
    std::ostringstream oss;
    oss << "[AdvancedAgent] Step " << performanceMetrics_.totalSteps << ": "
        << "neurons=" << brain_->getTotalNeuronCount()
        << ", synapses=" << brain_->getTotalSynapseCount()
        << ", active=" << brain_->getActiveNeuronCount()
        << ", firing=" << brain_->getFiringNeuronCount()
        << ", curiosity=" << performanceMetrics_.averageCuriosity
        << ", novelty=" << performanceMetrics_.averageNovelty
        << ", prediction_error=" << performanceMetrics_.averagePredictionError
        << ", memory_usage=" << currentMemoryUsage_ << "/" << memoryCapacity_;
    
    logOutput_ += oss.str() + "\n";
    
    if (logLevel_ == "INFO") {
        NLM_LOG_INFO(oss.str());
    } else if (logLevel_ == "WARNING") {
        NLM_LOG_WARNING(oss.str());
    } else if (logLevel_ == "ERROR") {
        NLM_LOG_ERROR(oss.str());
    }
}

void AdvancedAgent::updateDevelopmentalStage() {
    performanceMetrics_.simulationTime += 0.1; // Assume 0.1s per step
    
    // Update developmental stage based on age
    if (performanceMetrics_.simulationTime > developmentalSchedule_[currentDevelopmentStage_]) {
        if (currentDevelopmentStage_ + 1 < developmentalSchedule_.size()) {
            currentDevelopmentStage_++;
            NLM_LOG_INFO("Developmental stage advanced to " + std::to_string(currentDevelopmentStage_));
            
            // Update plasticity based on schedule
            if (currentDevelopmentStage_ < plasticitySchedule_.size()) {
                float newPlasticity = plasticitySchedule_[currentDevelopmentStage_];
                if (brain_ && brain_->getSTDP()) {
                    brain_->getSTDP()->setLTPWeight(0.01f * newPlasticity);
                    brain_->getSTDP()->setLTDWeight(0.012f * newPlasticity);
                }
            }
        }
    }
}

void AdvancedAgent::updatePlasticityParameters() {
    float plasticityMod = 1.0f;
    
    if (brain_ && brain_->getDopamine()) {
        plasticityMod = brain_->getDopamine()->getPlasticityFactor();
    }
    
    // Apply plasticity rules
    for (const auto& rule : plasticityRuleEnabled_) {
        if (rule.second) {
            float currentStrength = plasticityRules_[rule.first];
            plasticityRules_[rule.first] = currentStrength * plasticityMod;
        }
    }
}

void AdvancedAgent::updateMemoryUsage() {
    if (memoryPruningEnabled_) {
        // Simple memory pruning based on capacity
        if (currentMemoryUsage_ > memoryCapacity_) {
            float pruneRatio = 1.0f - (static_cast<float>(memoryCapacity_) / currentMemoryUsage_);
            currentMemoryUsage_ = static_cast<size_t>(currentMemoryUsage_ * pruneRatio);
            NLM_LOG_INFO("Memory pruned: " + std::to_string(currentMemoryUsage_) + " neurons/synapses remaining");
        }
    }
}

float AdvancedAgent::calculateIntrinsicReward(const SensoryPercept& percept) const {
    if (!intrinsicRewardFunction_) return 0.0f;
    
    return intrinsicRewardFunction_(percept);
}

float AdvancedAgent::calculateExtrinsicReward(const SensoryPercept& percept, const Action& action) const {
    if (!extrinsicRewardFunction_) return 0.0f;
    
    return extrinsicRewardFunction_(percept, action);
}

void AdvancedAgent::updateEpisodeData(float reward, float curiosity) {
    currentEpisode_.steps++;
    currentEpisode_.totalReward += reward;
    currentEpisode_.finalCuriosity = curiosity;
    
    // Determine episode outcome
    if (currentEpisode_.totalReward > 10.0f) {
        currentEpisode_.outcome = "success";
    } else if (currentEpisode_.totalReward < -10.0f) {
        currentEpisode_.outcome = "failure";
    } else {
        currentEpisode_.outcome = "neutral";
    }
    
    // Add to history
    episodeHistory_.push_back(currentEpisode_);
}

std::vector<double> AdvancedAgent::parseDevelopmentalSchedule(const std::string& scheduleStr) {
    std::vector<double> schedule;
    std::istringstream iss(scheduleStr);
    std::string token;
    
    while (std::getline(iss, token, ',')) {
        try {
            schedule.push_back(std::stod(token));
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Invalid schedule token: ") + token);
        }
    }
    
    return schedule;
}

std::vector<float> AdvancedAgent::parsePlasticitySchedule(const std::string& scheduleStr) {
    std::vector<float> schedule;
    std::istringstream iss(scheduleStr);
    std::string token;
    
    while (std::getline(iss, token, ',')) {
        try {
            schedule.push_back(std::stof(token));
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Invalid plasticity schedule token: ") + token);
        }
    }
    
    return schedule;
}

std::string AdvancedAgent::perceptToKey(const SensoryPercept& percept) const {
    std::ostringstream oss;
    oss << "vision:" << percept.getVision().size()
        << ":touch:" << percept.getTouch().size()
        << ":internal:" << percept.getInternal().size()
        << ":proprioception:" << percept.getProprioception().size()
        << ":audio:" << percept.getAudio().size();
    return oss.str();
}

std::string AdvancedAgent::actionToKey(const Action& action) const {
    std::ostringstream oss;
    oss << "type:" << static_cast<int>(action.getType())
        << ":params:" << action.getParameters().size();
    return oss.str();
}

void AdvancedAgent::pruneEnvironmentModel() {
    // Simple pruning: remove oldest entries
    int entriesToRemove = environmentModel_.size() - 5000; // Keep 5000 most recent
    if (entriesToRemove > 0) {
        auto it = environmentModel_.begin();
        for (int i = 0; i < entriesToRemove; ++i) {
            it = environmentModel_.erase(it);
        }
        NLM_LOG_INFO("Environment model pruned: " + std::to_string(environmentModel_.size()) + " entries remaining");
    }
}

void AdvancedAgent::runEpisode(size_t steps) {
    // Reset current episode
    currentEpisode_.reset();
    currentEpisode_.episodeNumber++;
    currentEpisode_.startTime = performanceMetrics_.simulationTime;
    currentEpisode_.outcome = "ongoing";
    
    // Run episode
    for (size_t i = 0; i < steps; ++i) {
        if (!world_) {
            break;
        }
        
        // Get sensory input
        auto percept = world_->getSensoryPercept();
        
        // Process sensory input with enhanced processing
        processSensoryInputEnhanced(percept);
        
        // Run brain step
        brain_->step(i);
        
        // Decode motor command with confidence
        auto actionWithConfidence = decodeMotorCommandWithConfidence();
        MotorCommand action = actionWithConfidence.first;
        float confidence = actionWithConfidence.second;
        
        // Check safety constraints
        if (safetyConstraintsEnabled_) {
            Action actionObj(action); // Create action object for safety check
            if (!checkSafetyConstraints(actionObj)) {
                // Fallback to safe action
                action = MotorCommand::Wait;
            }
        }
        
        // Apply action to world
        if (world_) {
            world_->applyMotorCommand(action, world_->getSimulationTime());
        }
        
        // Calculate rewards
        float extrinsicReward = calculateExtrinsicReward(percept, Action(action));
        float intrinsicReward = calculateIntrinsicReward(percept);
        float totalReward = extrinsicReward + intrinsicReward;
        
        // Update neuromodulation
        auto* agentBrain = dynamic_cast<AgentBrain*>(brain_.get());
        if (agentBrain) {
            agentBrain->applyRewardModulation(totalReward, predictFutureReward(percept, Action(action)));
        }
        
        // Update development
        if (agentBrain) {
            agentBrain->updateDevelopment(0.1);
        }
        
        // Update environment model
        updateEnvironmentModel(percept, Action(action), totalReward);
        
        // Update episode data
        updateEpisodeData(totalReward, agentBrain ? agentBrain->getCuriosityLevel() : 0.0f);
        
        // Update performance metrics
        updatePerformanceMetrics();
        
        // Update developmental stage
        updateDevelopmentalStage();
        
        // Update plasticity parameters
        updatePlasticityParameters();
        
        // Update memory usage
        updateMemoryUsage();
        
        // Social learning
        if (socialLearningEnabled_ && i % 100 == 0) {
            for (const auto& neighbor : socialNeighbors_) {
                observeAgent(*neighbor);
            }
        }
        
        // Periodic logging
        if (realTimeLoggingEnabled_ && i % 100 == 0) {
            logPerformanceMetrics();
        }
    }
    
    currentEpisode_.endTime = performanceMetrics_.simulationTime;
    currentEpisode_.outcome = "completed";
    
    NLM_LOG_INFO("Episode completed: " + currentEpisode_.outcome + ", reward=" + std::to_string(currentEpisode_.totalReward) + 
                 ", steps=" + std::to_string(currentEpisode_.steps));
}

} // namespace nlm
```