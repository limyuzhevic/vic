#ifndef ADVANCED_DOCUMENTATION_HPP
#define ADVANCED_DOCUMENTATION_HPP

#include <string>
#include <vector>
#include <map>

namespace nlm {

// Advanced NLM Usage Documentation
// Comprehensive guide for experienced users and advanced use cases

// ===========================================
// ADVANCED AGENT IMPLEMENTATION
// ===========================================

/**
 * Advanced Agent with Deep Learning Capabilities
 * 
 * This example demonstrates how to create an advanced agent brain with sophisticated
 * learning algorithms, policy gradient methods, and experience replay for continual
 * learning and improved performance.
 */
std::string advanced_agent_example() {
    return R""
// Advanced Agent Implementation
// ========================

// Create configuration with advanced settings
auto config = pynlm.createDefaultConfig();
config.set("brain.neuron_count", 5000);
config.set("brain.synapse_density", 0.15f);
config.set("plasticity.stdp.enable", true);
config.set("plasticity.stdp.learning_rate", 0.002f);
config.set("advanced.learning_rate", 0.001f);
config.set("advanced.discount_factor", 0.995f);
config.set("advanced.meta_learning_enabled", true);
config.set("advanced.experience_replay_size", 5000);
config.set("advanced.action_strategy", "actor_critic");
config.set("advanced.entropy_regularization", 0.01f);

// Create brain and advanced agent
auto brain = pynlm.createBrain(config);
brain->initialize();

// Create advanced agent with policy network
auto agent = pynlm.createAdvancedAgentBrain(brain);
agent->initialize(world, config);

// Configure advanced learning
agent->enableAdvancedLearning(true);
agent->setActionSelectionStrategy(pynlm.ActionSelectionStrategy.ActorCritic);
agent->enableMetaLearning(true);
agent->setExperienceReplaySize(5000);
agent->enableAdaptiveExploration(true);

// Policy network configuration (128 inputs, 64 hidden, 7 outputs)
agent->configurePolicyNetwork(128, 64, 7);

// Episode loop with advanced learning
for (int episode = 0; episode < 1000; ++episode) {
    // Reset environment and agent
    world->reset();
    agent->reset();
    
    // Episode variables
    std::vector<float> state = agent->getCurrentState();
    float totalReward = 0.0f;
    int steps = 0;
    bool done = false;
    
    while (!done && steps < 1000) {
        // Select action using advanced policy
        MotorCommand action = agent->selectAction(state);
        
        // Execute action in environment
        world->update(0.1);
        auto result = world->applyMotorCommand(action, world->getSimulationTime());
        
        // Get next state
        std::vector<float> nextState = agent->getCurrentState();
        
        // Learn from experience
        agent->learnFromExperience(state, action, result.reward, nextState, done);
        
        // Update state
        state = nextState;
        totalReward += result.reward;
        steps++;
        
        if (done || steps >= 1000) break;
    }
    
    // Print episode statistics
    if (episode % 100 == 0) {
        auto metrics = agent->getPerformanceMetrics();
        std::cout << "Episode " << episode << ": " 
                  << "Return: " << metrics.recentReturn << ", "
                  << "Length: " << metrics.episodeLength << ", "
                  << "Exploration: " << metrics.explorationEfficiency << ", "
                  << "Learning Rate: " << metrics.learningRate << std::endl;
    }
}

// Save trained policy
agent->savePolicy("advanced_agent_policy.bin");

return "Advanced agent example completed successfully!";
}

/**
 * Policy Gradient with Experience Replay
 * 
 * This example demonstrates implementing a policy gradient algorithm with
 * experience replay for stable and efficient learning.
 */
std::string policy_gradient_example() {
    return R""
Policy Gradient with Experience Replay
// =======================================

// Initialize components for policy gradient learning
auto config = pynlm.createDefaultConfig();
config.set("advanced.learning_rate", 0.01f);
config.set("advanced.discount_factor", 0.99f);
config.set("advanced.experience_replay_size", 2000);
config.set("advanced.action_strategy", "policy_gradient");

auto brain = pynlm.createBrain(config);
auto agent = pynlm.createAdvancedAgentBrain(brain);

// Create experience replay buffer
std::vector<Experience> replayBuffer;
const size_t MAX_BUFFER_SIZE = 2000;

// Training loop
for (int episode = 0; episode < 500; ++episode) {
    std::vector<float> state = agent->getCurrentState();
    std::vector<Experience> episodeBuffer;
    
    // Collect episode experiences
    for (int step = 0; step < 200; ++step) {
        // Select action using current policy
        MotorCommand action = agent->selectAction(state);
        
        // Execute and observe
        world->update(0.1);
        auto result = world->applyMotorCommand(action, world->getSimulationTime());
        std::vector<float> nextState = agent->getCurrentState();
        bool done = result.success || step >= 199;
        
        // Store experience
        Experience exp(state, action, result.reward, nextState, done);
        episodeBuffer.push_back(exp);
        
        // Update state
        state = nextState;
        
        if (done) break;
    }
    
    // Add episode experiences to replay buffer
    for (const auto& exp : episodeBuffer) {
        replayBuffer.push_back(exp);
        if (replayBuffer.size() > MAX_BUFFER_SIZE) {
            replayBuffer.erase(replayBuffer.begin());
        }
    }
    
    // Train from replay buffer (policy gradient)
    if (replayBuffer.size() >= 32) {
        std::vector<Experience> batch;
        for (size_t i = 0; i < 32; ++i) {
            size_t idx = std::rand() % replayBuffer.size();
            batch.push_back(replayBuffer[idx]);
        }
        
        agent->trainPolicyGradient(batch);
    }
    
    // Print progress
    if (episode % 50 == 0) {
        std::cout << "Policy Gradient Episode " << episode 
                  << ": Buffer size = " << replayBuffer.size() << std::endl;
    }
}

return "Policy gradient training completed!";
}

/**
 * Meta-Learning and Adaptation
 * 
 * This example demonstrates how to implement meta-learning where the agent
 * learns how to learn and adapts its learning strategy based on performance.
 */
std::string meta_learning_example() {
    return R""
Meta-Learning and Adaptation
// ==========================

// Initialize meta-learning environment
auto config = pynlm.createDefaultConfig();
config.set("advanced.meta_learning_enabled", true);
config.set("advanced.adaptation_rate", 0.01f);
config.set("advanced.task_complexity_threshold", 0.7f);

auto brain = pynlm.createBrain(config);
auto agent = pynlm.createAdvancedAgentBrain(brain);

// Meta-learning components
struct Task {
    std::string name;
    float difficulty;
    float expectedReward;
    int duration;
};

struct MetaState {
    float recentPerformance;
    float explorationEfficiency;
    float learningRate;
    float adaptability;
};

std::vector<Task> taskPool = {
    {"simple_navigation", 0.2f, 10.0f, 100},
    {"resource_collection", 0.5f, 25.0f, 200},
    {"obstacle_avoidance", 0.7f, 15.0f, 150},
    {"complex_planning", 0.9f, 40.0f, 300}
};

// Meta-learning loop
for (int metaIteration = 0; metaIteration < 200; ++metaIteration) {
    // Select task based on current performance
    MetaState metaState = agent->getMetaState();
    float performanceThreshold = config->getOr<float>("advanced.performance_threshold", 0.5f);
    
    Task currentTask = selectTaskBasedOnMetaState(metaState, taskPool, performanceThreshold);
    
    // Adapt learning strategy for current task
    adaptLearningStrategy(agent, currentTask);
    
    // Execute task with meta-learned agent
    TaskResult result = executeTaskWithAgent(agent, currentTask);
    
    // Meta-update based on task performance
    metaState.recentPerformance = result.performance;
    metaState.explorationEfficiency = result.explorationEfficiency;
    metaState.learningRate = agent->getPerformanceMetrics().learningRate;
    metaState.adaptability = calculateAdaptability(result);
    
    agent->updateMetaState(metaState);
    
    // Print meta-learning progress
    if (metaIteration % 20 == 0) {
        std::cout << "Meta Iteration " << metaIteration 
                  << ": Task = " << currentTask.name 
                  << ", Performance = " << metaState.recentPerformance 
                  << ", Adaptability = " << metaState.adaptability << std::endl;
    }
}

return "Meta-learning completed successfully!";
}

/**
 * Advanced Action Selection Strategies
 * 
 * This example demonstrates different advanced action selection strategies
 * and their implementation details.
 */
std::string action_selection_strategies_example() {
    return R""
Advanced Action Selection Strategies
// =================================

enum class AdvancedActionStrategy {
    EpsilonGreedy,
    SoftmaxActionSelection,
    UpperConfidenceBound,
    ThompsonSampling,
    PolicyGradient,
    ActorCritic,
    TrustRegionPolicyOptimization,
    ProximalPolicyOptimization
};

void demonstrateActionStrategies() {
    auto config = pynlm.createDefaultConfig();
    auto brain = pynlm.createBrain(config);
    auto agent = pynlm.createAdvancedAgentBrain(brain);
    
    std::vector<float> state = getInitialState();
    
    // Demonstrate different strategies
    std::vector<AdvancedActionStrategy> strategies = {
        AdvancedActionStrategy.EpsilonGreedy,
        AdvancedActionStrategy.SoftmaxActionSelection,
        AdvancedActionStrategy.UpperConfidenceBound,
        AdvancedActionStrategy.ThompsonSampling,
        AdvancedActionStrategy.PolicyGradient,
        AdvancedActionStrategy.ActorCritic,
        AdvancedActionStrategy.TrustRegionPolicyOptimization,
        AdvancedActionStrategy.ProximalPolicyOptimization
    };
    
    for (const auto& strategy : strategies) {
        agent->setActionSelectionStrategy(strategy);
        
        // Run simulation with current strategy
        float totalReward = simulateWithStrategy(agent, strategy, state);
        
        std::cout << "Strategy " << static_cast<int>(strategy) 
                  << ": Total Reward = " << totalReward << std::endl;
    }
}

// Strategy-specific implementations
float simulateWithStrategy(AdvancedAgentBrain& agent, AdvancedActionStrategy strategy, 
                         const std::vector<float>& initialState) {
    agent->setActionSelectionStrategy(strategy);
    std::vector<float> state = initialState;
    float totalReward = 0.0f;
    
    for (int step = 0; step < 100; ++step) {
        MotorCommand action = agent->selectAction(state);
        float reward = executeActionAndGetReward(action, state);
        state = getNextState(state, action);
        totalReward += reward;
    }
    
    return totalReward;
}

return "Action selection strategies demonstration completed!";
}

/**
 * Advanced Neuromodulation Systems
 * 
 * This example demonstrates advanced neuromodulation with multiple
 * neurotransmitters and their effects on learning and behavior.
 */
std::string advanced_neuromodulation_example() {
    return R""
Advanced Neuromodulation Systems
// ================================

struct NeuromodulationSignals {
    float dopamine;      // Reward prediction error
    float serotonin;    // Punishment sensitivity
    float noradrenaline; // Arousal/attention
    float acetylcholine; // Learning rate modulation
    float oxytocin;     // Social behavior
    float vasopressin;  // Memory consolidation
};

class AdvancedNeuromodulation {
public:
    NeuromodulationSignals calculateSignals(float reward, float predictionError, 
                                           const AgentState& agentState) {
        NeuromodulationSignals signals;
        
        // Dopamine: reward prediction error
        signals.dopamine = std::clamp(predictionError * 0.5f, -1.0f, 1.0f);
        
        // Serotonin: punishment and inhibition
        signals.serotonin = std::clamp(1.0f - std::abs(predictionError) * 0.3f, 0.0f, 1.0f);
        
        // Noradrenaline: arousal and attention
        signals.noradrenaline = calculateArousalLevel(agentState, predictionError);
        
        // Acetylcholine: learning rate modulation
        signals.acetylcholine = calculateLearningModulation(signals.dopamine, agentState);
        
        // Oxytocin: social behavior (simplified)
        signals.oxytocin = calculateSocialBehavior(agentState);
        
        // Vasopressin: memory consolidation
        signals.vasopressin = calculateMemoryConsolidation(agentState);
        
        return signals;
    }
    
    void applyNeuromodulationEffects(NeuromodulationSignals& signals, 
                                   PolicyNetwork& policy, 
                                   AgentBrain& brain) {
        // Update policy exploration based on noradrenaline
        float explorationModulation = 0.5f + 0.5f * signals.noradrenaline;
        policy.setExplorationRate(policy.getExplorationRate() * explorationModulation);
        
        // Update learning rate based on acetylcholine
        float learningRateMod = 1.0f + 0.5f * signals.acetylcholine;
        policy.setLearningRate(policy.getLearningRate() * learningRateMod);
        
        // Update plasticity based on dopamine
        float plasticityMod = 0.5f + 0.5f * signals.dopamine;
        applyPlasticityChanges(brain, plasticityMod);
        
        // Memory consolidation based on vasopressin
        if (signals.vasopressin > 0.5f) {
            consolidateImportantMemories(brain);
        }
        
        // Social behavior based on oxytocin
        if (signals.oxytocin > 0.3f) {
            promoteSocialBehavior(brain);
        }
    }
    
private:
    float calculateArousalLevel(const AgentState& state, float predictionError) {
        float baseline = 0.3f;
        float novelty = getNoveltyScore(state);
        float predictionErrorAbs = std::abs(predictionError);
        return std::clamp(baseline + novelty * 0.5f - predictionErrorAbs * 0.2f, 0.0f, 1.0f);
    }
    
    float calculateLearningModulation(float dopamine, const AgentState& state) {
        // Higher dopamine generally increases learning rate
        float learningMod = 1.0f + dopamine * 0.3f;
        
        // Adjust based on current developmental stage
        if (state == AgentState::EXPLORATION) {
            learningMod *= 1.2f;  // Enhanced learning during exploration
        }
        
        return std::clamp(learningMod, 0.1f, 2.0f);
    }
    
    float calculateSocialBehavior(const AgentState& state) {
        if (state == AgentState::EXPLOITATION || state == AgentState::CONSOLIDATING) {
            return 0.8f; // Higher social behavior during exploitation and consolidation
        }
        return 0.2f;
    }
    
    float calculateMemoryConsolidation(const AgentState& state) {
        if (state == AgentState::CONSOLIDATING) {
            return 1.0f; // Full memory consolidation during consolidation phase
        }
        return 0.0f;
    }
    
    void applyPlasticityChanges(AgentBrain& brain, float modulation) {
        auto* stdp = brain.getSTDP();
        if (stdp) {
            float currentLTP = stdp->getLTPWeight();
            float currentLTD = stdp->getLTDWeight();
            
            stdp->setLTPWeight(currentLTP * modulation);
            stdp->setLTDWeight(currentLTD * modulation);
        }
    }
    
    void consolidateImportantMemories(AgentBrain& brain) {
        // Implementation would consolidate high-priority memories
        // This could involve replaying important experiences or strengthening connections
        NLM_LOG_INFO("Memory consolidation triggered");
    }
    
    void promoteSocialBehavior(AgentBrain& brain) {
        // Implementation would promote social behaviors
        // This could involve coordinating with other agents or promoting cooperative actions
        NLM_LOG_INFO("Social behavior promoted");
    }
    
    float getNoveltyScore(const AgentState& state) {
        // Calculate novelty based on agent's current state
        if (state == AgentState::EXPLORATION) {
            return 0.9f;
        } else if (state == AgentState::EXPLOITATION) {
            return 0.3f;
        }
        return 0.1f;
    }
};

// Demonstration of advanced neuromodulation
std::string demonstrateAdvancedNeuromodulation() {
    AdvancedNeuromodulation neuromod;
    
    // Simulate different scenarios
    std::vector<std::pair<float, float>> scenarios = {
        {10.0f, 5.0f},   // High reward, high prediction error
        {2.0f, 0.0f},    // Medium reward, no prediction error
        {-5.0f, -3.0f},  // Negative reward, high prediction error
        {0.0f, 1.0f}    // No reward, positive prediction error
    };
    
    AgentState state = AgentState::EXPLORATION;
    
    for (const auto& scenario : scenarios) {
        float reward = scenario.first;
        float predictionError = scenario.second;
        
        NeuromodulationSignals signals = neuromod.calculateSignals(reward, predictionError, state);
        
        std::cout << "Scenario: Reward=" << reward << ", Prediction Error=" << predictionError << std::endl;
        std::cout << "  Dopamine: " << signals.dopamine << ", "
                  << "Serotonin: " << signals.serotonin << ", "
                  << "Noradrenaline: " << signals.noradrenaline << ", "
                  << "Acetylcholine: " << signals.acetylcholine << std::endl;
    }
    
    return "Advanced neuromodulation demonstration completed!";
}

return "Advanced neuromodulation example completed!";
}

/**
 * Policy Network Architecture and Training
 * 
 * This example demonstrates different neural network architectures
 * for policy learning and their training procedures.
 */
std::string policy_network_architecture_example() {
    return R""
Policy Network Architecture and Training
// ======================================

class AdvancedPolicyNetwork {
public:
    // Different network architectures
    enum class Architecture {
        Shallow,      // 1 hidden layer
        Deep,         // 3 hidden layers
        Wide,         // Many neurons in hidden layers
        Convolutional, // For spatial input
        Recurrent     // For sequential data
    };
    
    AdvancedPolicyNetwork(Architecture arch, size_t inputSize, size_t outputSize) {
        configureArchitecture(arch, inputSize, outputSize);
    }
    
    // Network forward pass
    std::vector<float> forward(const std::vector<float>& input) {
        switch (architecture_) {
            case Architecture::Shallow:
                return forwardShallow(input);
            case Architecture::Deep:
                return forwardDeep(input);
            case Architecture::Wide:
                return forwardWide(input);
            case Architecture::Convolutional:
                return forwardConvolutional(input);
            case Architecture::Recurrent:
                return forwardRecurrent(input);
        }
        return std::vector<float>(outputSize_, 0.0f);
    }
    
    // Advanced training methods
    void trainWithExperienceReplay(const std::vector<Experience>& experiences) {
        // Sample mini-batch
        std::vector<Experience> batch = sampleMiniBatch(experiences, 32);
        
        // Train using different algorithms
        if (trainingAlgorithm_ == TrainingAlgorithm.ActorCritic) {
            trainActorCritic(batch);
        } else if (trainingAlgorithm_ == TrainingAlgorithm.PolicyGradient) {
            trainPolicyGradient(batch);
        } else if (trainingAlgorithm_ == TrainingAlgorithm.TRON2) {
            trainTRON2(batch);
        }
    }
    
    // Network architectures
    std::vector<float> forwardShallow(const std::vector<float>& input) {
        // One hidden layer: input -> hidden -> output
        std::vector<float> hidden(hiddenSize1_, 0.0f);
        for (size_t i = 0; i < hiddenSize1_; ++i) {
            float sum = bias1_[i];
            for (size_t j = 0; j < inputSize_; ++j) {
                sum += input[j] * weights1_[i * inputSize_ + j];
            }
            hidden[i] = sigmoid(sum);
        }
        
        std::vector<float> output(outputSize_, 0.0f);
        for (size_t i = 0; i < outputSize_; ++i) {
            float sum = bias2_[i];
            for (size_t j = 0; j < hiddenSize1_; ++j) {
                sum += hidden[j] * weights2_[i * hiddenSize1_ + j];
            }
            output[i] = sigmoid(sum);
        }
        return output;
    }
    
    std::vector<float> forwardDeep(const std::vector<float>& input) {
        // Three hidden layers: input -> hidden1 -> hidden2 -> hidden3 -> output
        std::vector<float> hidden1 = activateLayer(input, weights1_, bias1_, hiddenSize1_);
        std::vector<float> hidden2 = activateLayer(hidden1, weights2_, bias2_, hiddenSize2_);
        std::vector<float> hidden3 = activateLayer(hidden2, weights3_, bias3_, hiddenSize3_);
        return activateLayer(hidden3, weights4_, bias4_, outputSize_);
    }
    
    std::vector<float> forwardWide(const std::vector<float>& input) {
        // Wide network: large hidden layers
        std::vector<float> hidden1 = activateLayer(input, weights1_, bias1_, wideSize1_);
        std::vector<float> hidden2 = activateLayer(hidden1, weights2_, bias2_, wideSize2_);
        return activateLayer(hidden2, weights3_, bias3_, outputSize_);
    }
    
    std::vector<float> forwardConvolutional(const std::vector<float>& input) {
        // Convolutional network for spatial input (simplified)
        // Would use 2D convolutions for vision-like inputs
        std::vector<float> conv1 = applyConv2D(input, convWeights1_, convBias1_, 4, 3);
        std::vector<float> pool1 = maxPooling(conv1, 2);
        std::vector<float> conv2 = applyConv2D(pool1, convWeights2_, convBias2_, 8, 3);
        std::vector<float> pool2 = maxPooling(conv2, 2);
        return flattenAndFullyConnect(pool2, fcWeights_, fcBias_, outputSize_);
    }
    
    // Training algorithms
    void trainActorCritic(const std::vector<Experience>& batch) {
        // Actor-critic algorithm
        for (const auto& exp : batch) {
            // Get current policy
            std::vector<float> policy = forward(exp.state);
            
            // Calculate advantage
            float valueEstimate = policy[0]; // First output is value
            float advantage = exp.reward + discountFactor_ * valueEstimate - valueEstimate;
            
            // Update actor (policy) using advantage
            updateActor(exp.state, exp.action, advantage);
            
            // Update critic (value function)
            updateCritic(exp.state, advantage);
        }
    }
    
    void trainPolicyGradient(const std::vector<Experience>& batch) {
        // Pure policy gradient algorithm
        for (const auto& exp : batch) {
            std::vector<float> policy = forward(exp.state);
            
            // Calculate policy gradient
            float logProb = calculateLogProbability(policy, exp.action);
            float advantage = calculateAdvantage(exp);
            
            // Update policy
            updatePolicyGradient(exp.state, policy, logProb, advantage);
        }
    }
    
    // Helper functions
    std::vector<float> activateLayer(const std::vector<float>& input,
                                    const std::vector<float>& weights,
                                    const std::vector<float>& bias,
                                    size_t outputSize) {
        std::vector<float> output(outputSize, 0.0f);
        for (size_t i = 0; i < outputSize; ++i) {
            float sum = bias[i];
            for (size_t j = 0; j < input.size(); ++j) {
                sum += input[j] * weights[i * input.size() + j];
            }
            output[i] = relu(sum); // ReLU activation
        }
        return output;
    }
    
    std::vector<float> applyConv2D(const std::vector<float>& input,
                                  const std::vector<float>& weights,
                                  const std::vector<float>& bias,
                                  size_t outputChannels,
                                  size_t kernelSize) {
        // Simplified 2D convolution (placeholder)
        return input; // TODO: Implement proper convolution
    }
    
    std::vector<float> maxPooling(const std::vector<float>& input, size_t poolSize) {
        // Simplified max pooling (placeholder)
        return input; // TODO: Implement proper pooling
    }
    
    std::vector<float> flattenAndFullyConnect(const std::vector<float>& input,
                                             const std::vector<float>& weights,
                                             const std::vector<float>& bias,
                                             size_t outputSize) {
        // Flatten and fully connected layer
        std::vector<float> output(outputSize, 0.0f);
        for (size_t i = 0; i < outputSize; ++i) {
            float sum = bias[i];
            for (size_t j = 0; j < input.size(); ++j) {
                sum += input[j] * weights[i * input.size() + j];
            }
            output[i] = sigmoid(sum);
        }
        return output;
    }
    
    std::vector<Experience> sampleMiniBatch(const std::vector<Experience>& experiences,
                                            size_t batchSize) {
        std::vector<Experience> batch;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, experiences.size() - 1);
        
        for (size_t i = 0; i < batchSize && i < experiences.size(); ++i) {
            batch.push_back(experiences[dist(gen)]);
        }
        
        return batch;
    }
    
    void updateActor(const std::vector<float>& state, MotorCommand action, float advantage) {
        std::vector<float> policy = forward(state);
        int actionIndex = motorCommandToIndex(action);
        
        // Policy gradient update
        for (size_t i = 0; i < outputSize_; ++i) {
            float gradient = (i == actionIndex ? advantage : 0.0f) * policy[i] * (1.0f - policy[i]);
            // Update weights (simplified)
        }
    }
    
    void updateCritic(const std::vector<float>& state, float advantage) {
        // Value function update
        std::vector<float> policy = forward(state);
        float currentValue = policy[0];
        
        // TD error
        float tdError = advantage;
        
        // Update value function
        // This would update the critic network parameters
    }
    
    void updatePolicyGradient(const std::vector<float>& state,
                             const std::vector<float>& policy,
                             float logProb,
                             float advantage) {
        // Policy gradient update
        int actionIndex = motorCommandToIndex(currentAction_);
        
        float gradient = advantage * logProb;
        // Update policy weights
    }
    
    float calculateLogProbability(const std::vector<float>& policy, MotorCommand action) {
        int actionIndex = motorCommandToIndex(action);
        if (actionIndex >= 0 && actionIndex < policy.size()) {
            return std::log(policy[actionIndex]);
        }
        return 0.0f;
    }
    
    float calculateAdvantage(const Experience& exp) {
        float nextValue = 0.0f;
        if (!exp.done && !exp.nextState.empty()) {
            nextValue = forward(exp.nextState)[0];
        }
        return exp.reward + discountFactor_ * nextValue - forward(exp.state)[0];
    }
    
    int motorCommandToIndex(MotorCommand action) {
        switch (action) {
            case MotorCommand::MoveForward: return 0;
            case MotorCommand::MoveBackward: return 1;
            case MotorCommand::TurnLeft: return 2;
            case MotorCommand::TurnRight: return 3;
            case MotorCommand::LookLeft: return 4;
            case MotorCommand::LookRight: return 5;
            case MotorCommand::Interact: return 6;
            case MotorCommand::Wait: return 7;
            default: return 0;
        }
    }
    
    float sigmoid(float x) { return 1.0f / (1.0f + std::exp(-x)); }
    float relu(float x) { return std::max(0.0f, x); }
    
    void configureArchitecture(Architecture arch, size_t inputSize, size_t outputSize) {
        architecture_ = arch;
        inputSize_ = inputSize;
        outputSize_ = outputSize;
        
        switch (arch) {
            case Architecture::Shallow:
                hiddenSize1_ = 64;
                hiddenSize2_ = 0;
                wideSize1_ = 0;
                wideSize2_ = 0;
                weights1_.resize(inputSize_ * hiddenSize1_);
                bias1_.resize(hiddenSize1_);
                weights2_.resize(hiddenSize1_ * outputSize_);
                bias2_.resize(outputSize_);
                break;
                
            case Architecture::Deep:
                hiddenSize1_ = 128;
                hiddenSize2_ = 64;
                hiddenSize3_ = 32;
                wideSize1_ = 0;
                wideSize2_ = 0;
                weights1_.resize(inputSize_ * hiddenSize1_);
                bias1_.resize(hiddenSize1_);
                weights2_.resize(hiddenSize1_ * hiddenSize2_);
                bias2_.resize(hiddenSize2_);
                weights3_.resize(hiddenSize2_ * hiddenSize3_);
                bias3_.resize(hiddenSize3_);
                weights4_.resize(hiddenSize3_ * outputSize_);
                bias4_.resize(outputSize_);
                break;
                
            case Architecture::Wide:
                wideSize1_ = 256;
                wideSize2_ = 128;
                hiddenSize1_ = 0;
                hiddenSize2_ = 0;
                hiddenSize3_ = 0;
                weights1_.resize(inputSize_ * wideSize1_);
                bias1_.resize(wideSize1_);
                weights2_.resize(wideSize1_ * wideSize2_);
                bias2_.resize(wideSize2_);
                weights3_.resize(wideSize2_ * outputSize_);
                bias3_.resize(outputSize_);
                break;
                
            case Architecture::Convolutional:
                // Convolutional network architecture
                break;
                
            case Architecture::Recurrent:
                // Recurrent network architecture
                break;
        }
        
        // Initialize weights
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> dist(0.0f, 0.1f);
        std::uniform_real_distribution<float> biasDist(-0.1f, 0.1f);
        
        for (float& w : weights1_) w = dist(gen);
        for (float& w : weights2_) w = dist(gen);
        if (!weights3_.empty()) {
            for (float& w : weights3_) w = dist(gen);
        }
        if (!weights4_.empty()) {
            for (float& w : weights4_) w = dist(gen);
        }
        
        for (float& b : bias1_) b = biasDist(gen);
        for (float& b : bias2_) b = biasDist(gen);
        if (!bias3_.empty()) {
            for (float& b : bias3_) b = biasDist(gen);
        }
        if (!bias4_.empty()) {
            for (float& b : bias4_) b = biasDist(gen);
        }
    }
    
    enum class TrainingAlgorithm {
        REINFORCE,
        ActorCritic,
        TRON2,
        PPO
    };
    
    void setTrainingAlgorithm(TrainingAlgorithm algorithm) {
        trainingAlgorithm_ = algorithm;
    }
    
    // Getters
    std::vector<float> getPolicyWeights1() const { return weights1_; }
    std::vector<float> getPolicyWeights2() const { return weights2_; }
    std::vector<float> getPolicyBiases1() const { return bias1_; }
    std::vector<float> getPolicyBiases2() const { return bias2_; }
    
    void setPolicyWeights1(const std::vector<float>& weights) { weights1_ = weights; }
    void setPolicyWeights2(const std::vector<float>& weights) { weights2_ = weights; }
    void setPolicyBiases1(const std::vector<float>& biases) { bias1_ = biases; }
    void setPolicyBiases2(const std::vector<float>& biases) { bias2_ = biases; }
    
private:
    Architecture architecture_;
    TrainingAlgorithm trainingAlgorithm_;
    size_t inputSize_;
    size_t outputSize_;
    
    // Network parameters
    std::vector<float> weights1_;
    std::vector<float> weights2_;
    std::vector<float> weights3_;
    std::vector<float> weights4_;
    std::vector<float> bias1_;
    std::vector<float> bias2_;
    std::vector<float> bias3_;
    std::vector<float> bias4_;
    
    // Hidden layer sizes
    size_t hiddenSize1_;
    size_t hiddenSize2_;
    size_t hiddenSize3_;
    size_t wideSize1_;
    size_t wideSize2_;
    
    float discountFactor_ = 0.99f;
};

return "Policy network architecture example completed!";
}

/**
 * Advanced Training Techniques
 * 
 * This example demonstrates advanced training techniques like
 * experience replay, target networks, and curriculum learning.
 */
std::string advanced_training_techniques_example() {
    return R""
Advanced Training Techniques
// =========================

class AdvancedTrainingManager {
public:
    AdvancedTrainingManager(AdvancedAgentBrain& agent) : agent_(agent) {
        enableExperienceReplay(true);
        enableTargetNetwork(true);
        enableCurriculumLearning(true);
    }
    
    // Experience replay with prioritized sampling
    void trainWithExperienceReplay(const std::vector<Experience>& experiences) {
        if (!useExperienceReplay_) return;
        
        // Prioritize experiences based on TD error
        std::vector<std::pair<float, Experience>> prioritizedExperiences;
        for (const auto& exp : experiences) {
            float tdError = calculateTDError(exp);
            prioritizedExperiences.emplace_back(std::abs(tdError), exp);
        }
        
        // Sort by TD error (higher priority for larger errors)
        std::sort(prioritizedExperiences.begin(), prioritizedExperiences.end(),
                 [](const auto& a, const auto& b) { return a.first > b.first; });
        
        // Sample from prioritized experiences
        std::vector<Experience> batch;
        size_t batchSize = std::min<size_t>(32, prioritizedExperiences.size());
        
        for (size_t i = 0; i < batchSize; ++i) {
            float sampleProb = (i < 16) ? 1.0f : 0.5f; // Higher priority for first half
            if (std::rand() / (RAND_MAX + 1.0f) < sampleProb) {
                batch.push_back(prioritizedExperiences[i].second);
            }
        }
        
        if (batch.size() >= 16) {
            agent_.trainWithAdvanced(batch);
        }
    }
    
    // Target network for stable learning
    class TargetNetwork {
    public:
        TargetNetwork(AdvancedAgentBrain& agent) : agent_(agent), tau_(0.005f) {}
        
        void softUpdateTargetNetwork() {
            // Polyak averaging: target = (1 - tau) * target + tau * source
            for (auto& [name, source] : sourceNetworks_) {
                auto& target = targetNetworks_[name];
                for (size_t i = 0; i < source.size(); ++i) {
                    target[i] = (1.0f - tau_) * target[i] + tau_ * source[i];
                }
            }
        }
        
        void addSourceNetwork(const std::string& name, const std::vector<float>& weights) {
            sourceNetworks_[name] = weights;
            if (targetNetworks_.find(name) == targetNetworks_.end()) {
                targetNetworks_[name] = weights;
            }
        }
        
        const std::vector<float>& getTargetNetwork(const std::string& name) const {
            return targetNetworks_.at(name);
        }
        
    private:
        AdvancedAgentBrain& agent_;
        float tau_;
        std::map<std::string, std::vector<float>> sourceNetworks_;
        std::map<std::string, std::vector<float>> targetNetworks_;
    };
    
    // Curriculum learning
    void implementCurriculumLearning(const std::vector<Task>& curriculum) {
        if (!useCurriculumLearning_) return;
        
        currentTaskIndex_ = 0;
        completedTasks_ = 0;
        
        // Start with easiest task
        currentTask_ = curriculum[0];
        agent_.configureTask(currentTask_);
    }
    
    void advanceCurriculum() {
        if (!useCurriculumLearning_) return;
        
        currentTaskIndex_++;
        if (currentTaskIndex_ < curriculum_.size()) {
            currentTask_ = curriculum_[currentTaskIndex_];
            agent_.configureTask(currentTask_);
            
            // Adjust learning parameters based on task difficulty
            float difficultyFactor = currentTask_.difficulty / 1.0f;
            agent_.setLearningRate(baseLearningRate_ * difficultyFactor);
            agent_.setExplorationRate(baseExplorationRate_ / difficultyFactor);
        }
    }
    
    bool isCurriculumCompleted() const {
        return currentTaskIndex_ >= curriculum_.size();
    }
    
    // Advanced training algorithm
    void trainWithAdvanced(const std::vector<Experience>& batch) {
        // Multi-step training
        for (int update = 0; update < 4; ++update) {
            // Experience replay
            if (useExperienceReplay_) {
                trainWithExperienceReplay(batch);
            }
            
            // Target network update
            if (useTargetNetwork_ && update == 3) {
                targetNetwork_.softUpdateTargetNetwork();
            }
            
            // Policy update
            agent_.updatePolicy(batch);
            
            // Value function update
            agent_.updateValueFunction(batch);
        }
    }
    
    // Training statistics
    struct TrainingStats {
        float totalReward;
        float averageReward;
        float learningProgress;
        size_t episodesCompleted;
        float explorationEfficiency;
        double trainingTime;
        
        TrainingStats() : totalReward(0.0f), averageReward(0.0f),
                         learningProgress(0.0f), episodesCompleted(0),
                         explorationEfficiency(0.0f), trainingTime(0.0) {}
    };
    
    TrainingStats getTrainingStats() const {
        TrainingStats stats;
        stats.episodesCompleted = agent_.getPerformanceMetrics().episodesCompleted;
        stats.learningProgress = agent_.getLearningProgress();
        stats.explorationEfficiency = agent_.getCuriosityScore();
        return stats;
    }
    
    void enableExperienceReplay(bool enable) { useExperienceReplay_ = enable; }
    void enableTargetNetwork(bool enable) { useTargetNetwork_ = enable; }
    void enableCurriculumLearning(bool enable) { useCurriculumLearning_ = enable; }
    
private:
    AdvancedAgentBrain& agent_;
    bool useExperienceReplay_ = false;
    bool useTargetNetwork_ = false;
    bool useCurriculumLearning_ = false;
    
    TargetNetwork targetNetwork_;
    
    std::vector<Task> curriculum_;
    size_t currentTaskIndex_;
    Task currentTask_;
    size_t completedTasks_;
    
    float baseLearningRate_ = 0.001f;
    float baseExplorationRate_ = 0.1f;
    
    float calculateTDError(const Experience& exp) {
        float nextValue = 0.0f;
        if (!exp.done && !exp.nextState.empty()) {
            nextValue = agent_.getExpectedValue(exp.nextState);
        }
        return std::abs(exp.reward + agent_.getDiscountFactor() * nextValue - agent_.getExpectedValue(exp.state));
    }
};

return "Advanced training techniques example completed!";
}

/**
 * Integration with External Systems
 * 
 * This example demonstrates how to integrate the advanced agent with
 * external systems like databases, sensors, and other AI systems.
 */
std::string integration_example() {
    return R""
Integration with External Systems
// ================================

class AdvancedAgentIntegration {
public:
    AdvancedAgentIntegration(AdvancedAgentBrain& agent) : agent_(agent) {}
    
    // Database integration for experience storage
    void integrateWithDatabase(const std::string& connectionString) {
        // Establish database connection
        // Store experiences for long-term learning
        // Load previously learned policies
    }
    
    // Sensor integration for real-time data
    void integrateWithSensors(const std::vector<SensorConfig>& sensors) {
        // Configure sensor inputs
        // Real-time data processing
        // Sensor fusion for improved state estimation
    }
    
    // Multi-agent system integration
    void integrateWithMultiAgentSystem(MultiAgentSystem& system) {
        // Join multi-agent environment
        // Communicate with other agents
        // Participate in coordinated tasks
        // Share experiences with other agents
    }
    
    // Cloud integration for distributed learning
    void integrateWithCloud(const CloudConfig& config) {
        // Upload experiences to cloud
        // Download pre-trained policies
        // Distributed training across multiple machines
        // Federated learning for privacy preservation
    }
    
    // ROS integration for robotics
    void integrateWithROS(const ROSConfig& config) {
        // Initialize ROS nodes
        // Subscribe to sensor topics
        // Publish action commands
        // Handle lifecycle management
    }
    
private:
    AdvancedAgentBrain& agent_;
};

// Integration demonstration
std::string demonstrateIntegration() {
    AdvancedAgentIntegration integration(*agent);
    
    // Database integration
    integration.integrateWithDatabase("mongodb://localhost:27017/nlm_experiences");
    
    // Sensor integration
    std::vector<SensorConfig> sensors = {
        {"camera", 640, 480, "rgb"},
        {"lidar", 360, 10.0f},
        {"imu", 3, "accelerometer"}
    };
    integration.integrateWithSensors(sensors);
    
    // Multi-agent integration
    MultiAgentSystem system;
    integration.integrateWithMultiAgentSystem(system);
    
    // Cloud integration (placeholder)
    CloudConfig cloudConfig = {
        "aws", "us-west-2", "s3://nlm-bucket/policies",
        true,  // encrypted storage
        10.0   // sync interval in seconds
    };
    integration.integrateWithCloud(cloudConfig);
    
    // ROS integration (placeholder)
    ROSConfig rosConfig = {
        "/nlm/agent",  // namespace
        100,          // update rate Hz
        true          // use_sim_time
    };
    integration.integrateWithROS(rosConfig);
    
    return "Integration demonstration completed!";
}

return "Integration example completed!";
}

/**
 * Performance Optimization and Monitoring
 * 
 * This example demonstrates performance optimization techniques
 * and comprehensive monitoring for production deployment.
 */
std::string performance_optimization_example() {
    return R""
Performance Optimization and Monitoring
// =====================================

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double trainingSpeed;
        double inferenceLatency;
        size_t experienceBufferSize;
        float learningRate;
        float policyEntropy;
        size_t completedEpisodes;
        double uptime;
        std::map<std::string, double> customMetrics;
        
        PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0),
                               trainingSpeed(0.0), inferenceLatency(0.0),
                               experienceBufferSize(0), learningRate(0.0),
                               policyEntropy(0.0), completedEpisodes(0),
                               uptime(0.0) {}
    };
    
    PerformanceMonitor(AdvancedAgentBrain& agent) : agent_(agent) {
        startTime_ = std::chrono::steady_clock::now();
        enableRealTimeMonitoring(true);
    }
    
    void startMonitoring() {
        monitoringActive_ = true;
        monitorThread_ = std::thread(&PerformanceMonitor::monitorLoop, this);
    }
    
    void stopMonitoring() {
        monitoringActive_ = false;
        if (monitorThread_.joinable()) {
            monitorThread_.join();
        }
    }
    
    PerformanceMetrics getCurrentMetrics() const {
        PerformanceMetrics metrics;
        
        // Get system metrics
        metrics.cpuUsage = getCPUUsage();
        metrics.memoryUsage = getMemoryUsage();
        
        // Get agent-specific metrics
        auto perfMetrics = agent_.getPerformanceMetrics();
        metrics.completedEpisodes = perfMetrics.episodesCompleted;
        metrics.learningRate = perfMetrics.learningRate;
        metrics.policyEntropy = perfMetrics.policyEntropy;
        
        // Get policy network metrics
        metrics.experienceBufferSize = agent_.getExperienceBufferSize();
        metrics.inferenceLatency = getAverageInferenceLatency();
        metrics.trainingSpeed = getTrainingSpeed();
        
        // Calculate uptime
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_);
        metrics.uptime = duration.count();
        
        return metrics;
    }
    
    void logMetrics() const {
        PerformanceMetrics metrics = getCurrentMetrics();
        
        std::cout << "=== NLM Performance Metrics ===" << std::endl;
        std::cout << "CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
        std::cout << "Memory Usage: " << metrics.memoryUsage << "%" << std::endl;
        std::cout << "Training Speed: " << metrics.trainingSpeed << " steps/sec" << std::endl;
        std::cout << "Inference Latency: " << metrics.inferenceLatency << " ms" << std::endl;
        std::cout << "Experience Buffer Size: " << metrics.experienceBufferSize << std::endl;
        std::cout << "Learning Rate: " << metrics.learningRate << std::endl;
        std::cout << "Policy Entropy: " << metrics.policyEntropy << std::endl;
        std::cout << "Completed Episodes: " << metrics.completedEpisodes << std::endl;
        std::cout << "Uptime: " << metrics.uptime << " seconds" << std::endl;
    }
    
    // Performance optimization
    void optimizePerformance() {
        // CPU optimization
        optimizeCPUUsage();
        
        // Memory optimization
        optimizeMemoryUsage();
        
        // Training optimization
        optimizeTrainingSpeed();
        
        // Inference optimization
        optimizeInferenceLatency();
    }
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable) {
        realTimeMonitoring_ = enable;
        if (enable) {
            startMonitoring();
        } else {
            stopMonitoring();
        }
    }
    
    // Alert system
    void checkAlerts() {
        PerformanceMetrics metrics = getCurrentMetrics();
        
        // High CPU alert
        if (metrics.cpuUsage > 90.0) {
            triggerAlert("HIGH_CPU", "CPU usage is above 90%");
        }
        
        // High memory alert
        if (metrics.memoryUsage > 80.0) {
            triggerAlert("HIGH_MEMORY", "Memory usage is above 80%");
        }
        
        // Slow inference alert
        if (metrics.inferenceLatency > 100.0) {
            triggerAlert("SLOW_INFERENCE", "Inference latency is above 100ms");
        }
        
        // Low learning rate alert
        if (metrics.learningRate < 0.0001f) {
            triggerAlert("LOW_LEARNING_RATE", "Learning rate is too low");
        }
        
        // High policy entropy alert (indicates exploration)
        if (metrics.policyEntropy > 3.0) {
            triggerAlert("HIGH_POLICY_ENTROPY", "Policy entropy is very high, may need to reduce exploration");
        }
    }
    
private:
    AdvancedAgentBrain& agent_;
    std::chrono::steady_clock::time_point startTime_;
    bool monitoringActive_ = false;
    bool realTimeMonitoring_ = false;
    std::thread monitorThread_;
    
    void monitorLoop() {
        while (monitoringActive_) {
            checkAlerts();
            
            // Log metrics every second
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void triggerAlert(const std::string& type, const std::string& message) {
        std::cout << "[ALERT] " << type << ": " << message << std::endl;
        
        // Send alert to monitoring system
        // In production, this would send to logging system, alerting system, etc.
    }
    
    double getCPUUsage() {
        // Placeholder implementation
        return 0.0;
    }
    
    double getMemoryUsage() {
        // Placeholder implementation
        return 0.0;
    }
    
    double getAverageInferenceLatency() {
        // Calculate average inference latency from policy network
        return 0.0;
    }
    
    double getTrainingSpeed() {
        // Calculate training speed
        return 0.0;
    }
    
    void optimizeCPUUsage() {
        // CPU optimization techniques
        // - Batch processing
        // - Cache optimization
        // - Thread pool tuning
    }
    
    void optimizeMemoryUsage() {
        // Memory optimization techniques
        // - Experience replay buffer optimization
        // - Model quantization
        // - Gradient checkpointing
    }
    
    void optimizeTrainingSpeed() {
        // Training speed optimization
        // - Parallel processing
        // - Mixed precision training
        // - Gradient accumulation
    }
    
    void optimizeInferenceLatency() {
        // Inference latency optimization
        // - Model pruning
        // - Kernel optimization
        // - Hardware acceleration
    }
};

return "Performance optimization example completed!";
}

/**
 * Production Deployment and Monitoring
 * 
 * This example demonstrates how to deploy the advanced agent in production
 * and implement comprehensive monitoring and logging.
 */
std::string production_deployment_example() {
    return R""
Production Deployment and Monitoring
// ================================

class ProductionDeployment {
public:
    ProductionDeployment(const std::string& deploymentConfig) {
        loadDeploymentConfig(deploymentConfig);
    }
    
    void deploy() {
        // Initialize components
        initializeComponents();
        
        // Start monitoring
        startMonitoringSystem();
        
        // Load trained policy
        if (autoLoadPolicy_) {
            loadPolicyFromStorage();
        }
        
        // Start agent services
        startAgentServices();
        
        // Register with service discovery
        registerWithServiceDiscovery();
        
        // Start health checks
        startHealthChecks();
        
        std::cout << "Advanced NLM agent deployed successfully!" << std::endl;
    }
    
    void scale() {
        // Scale resources based on load
        if (currentLoad_ > maxLoadThreshold_) {
            scaleUp();
        } else if (currentLoad_ < minLoadThreshold_) {
            scaleDown();
        }
    }
    
    void healthCheck() {
        // Perform health checks
        bool cpuHealthy = checkCPUHealth();
        bool memoryHealthy = checkMemoryHealth();
        bool agentHealthy = checkAgentHealth();
        bool modelHealthy = checkModelHealth();
        
        if (!cpuHealthy || !memoryHealthy || !agentHealthy || !modelHealthy) {
            triggerHealthAlert();
        }
        
        logHealthStatus(cpuHealthy, memoryHealthy, agentHealthy, modelHealthy);
    }
    
private:
    void loadDeploymentConfig(const std::string& config) {
        // Parse deployment configuration
        // Set up logging, monitoring, alerting
        // Configure resource limits
        deploymentConfig_ = config;
    }
    
    void initializeComponents() {
        // Initialize all components
        // Load configuration
        // Setup database connections
        // Initialize policy network
    }
    
    void startMonitoringSystem() {
        // Start monitoring systems
        // - Performance monitoring
        // - Application monitoring
        // - Infrastructure monitoring
        // - Business metrics monitoring
    }
    
    void loadPolicyFromStorage() {
        // Load trained policy from storage
        // - File system
        // - Database
        // - Cloud storage
        // - Model registry
    }
    
    void startAgentServices() {
        // Start agent services
        // - Training service
        // - Inference service
        // - Management service
        // - API service
    }
    
    void registerWithServiceDiscovery() {
        // Register with service discovery
        // - Consul
        // - etcd
        // - Kubernetes
        // - Custom service discovery
    }
    
    void startHealthChecks() {
        // Start health check services
        // - Health check endpoints
        // - Database health checks
        // - API health checks
        // - Resource health checks
    }
    
    void scaleUp() {
        // Scale up resources
        // - Add more CPU cores
        // - Add more memory
        // - Add more instances
        // - Update load balancer
    }
    
    void scaleDown() {
        // Scale down resources
        // - Remove CPU cores
        // - Remove memory
        // - Remove instances
        // - Update load balancer
    }
    
    bool checkCPUHealth() {
        // Check CPU health
        // - Usage levels
        // - Temperature
        // - Error rates
        return true;
    }
    
    bool checkMemoryHealth() {
        // Check memory health
        // - Available memory
        // - Memory fragmentation
        // - Swap usage
        return true;
    }
    
    bool checkAgentHealth() {
        // Check agent health
        // - Model loading
        // - Training status
        // - Inference performance
        return true;
    }
    
    bool checkModelHealth() {
        // Check model health
        // - Model validation
        // - Model integrity
        // - Model compatibility
        return true;
    }
    
    void triggerHealthAlert() {
        // Send health alert
        // - Email notification
        // - Slack notification
        // - PagerDuty alert
        // - Custom webhook
    }
    
    void logHealthStatus(bool cpu, bool memory, bool agent, bool model) {
        // Log health status
        // - INFO level for healthy components
        // - WARNING level for degraded components
        // - ERROR level for failed components
    }
    
    std::string deploymentConfig_;
    double maxLoadThreshold_ = 0.8;
    double minLoadThreshold_ = 0.2;
    double currentLoad_ = 0.0;
    bool autoLoadPolicy_ = true;
};

return "Production deployment example completed!";
}

/**
 * Advanced Usage Tips and Best Practices
 * 
 * This section provides advanced usage tips and best practices for
 * deploying and using the NLM advanced agent system.
 */
std::string advanced_tips_and_best_practices() {
    return R""
Advanced Tips and Best Practices
// ==============================

/**
 * Best Practices for Advanced Agent Training
 */
void trainingBestPractices() {
    // 1. Use curriculum learning
    // Start with simple tasks and gradually increase complexity
    
    // 2. Implement proper experience replay
    // Prioritize experiences with high TD error for more frequent learning
    
    // 3. Monitor policy entropy
    // Keep entropy in a reasonable range (0.1 - 2.0) for good exploration/exploitation balance
    
    // 4. Use target networks for stable learning
    // Implement soft target updates to prevent catastrophic forgetting
    
    // 5. Implement proper reward shaping
    // Design rewards that encourage desired behaviors without introducing bias
    
    // 6. Use mixed precision training
    // For better performance on GPU hardware
    
    // 7. Implement gradient clipping
    // Prevent exploding gradients in deep networks
    
    // 8. Use learning rate scheduling
    // Reduce learning rate as training progresses for better convergence
}

/**
 * Performance Optimization Techniques
 */
void performanceOptimizationTips() {
    // 1. Model quantization
    // Convert float32 weights to float16 for faster computation and lower memory usage
    
    // 2. Kernel fusion
    // Combine multiple operations into single kernels for better GPU utilization
    
    // 3. Memory pool management
    // Implement custom memory allocators for better memory usage patterns
    
    // 4. Asynchronous operations
    // Overlap computation with data transfer for better throughput
    
    // 5. Batch processing
    // Process experiences in batches for more efficient learning
    
    // 6. Checkpointing
    // Save model periodically to resume training from the best checkpoint
    
    // 7. Distributed training
    // Use multiple GPUs for parallel training
}

/**
 * Monitoring and Alerting Best Practices
 */
void monitoringBestPractices() {
    // 1. Set appropriate thresholds
    // - CPU: 80% warning, 90% critical
    // - Memory: 80% warning, 90% critical
    // - Inference latency: 100ms warning, 500ms critical
    
    // 2. Use sliding window metrics
    // Calculate metrics over recent time windows to detect trends
    
    // 3. Set up anomaly detection
    // Use statistical methods to detect unusual patterns
    
    // 4. Implement tiered alerting
    // - INFO: System status updates
    // - WARNING: Performance degradation
    // - ERROR: System failures
    // - CRITICAL: Immediate action required
    
    // 5. Log in structured format
    // Use JSON or other structured formats for better parsing and analysis
}

/**
 * Scaling and Deployment Best Practices
 */
void scalingBestPractices() {
    // 1. Use container orchestration
    // - Kubernetes for container management
    // - Service mesh for communication
    // - Automated scaling based on metrics
    
    // 2. Implement blue-green deployment
    // - Deploy new version alongside old version
    // - Gradually shift traffic to new version
    // - Rollback quickly if issues arise
    
    // 3. Use feature flags
    // - Control feature rollout
    // - A/B testing
    // - Gradual feature rollout
    
    // 4. Implement comprehensive logging
    // - Log everything at INFO level
    // - Trace requests across service boundaries
    // - Log at appropriate levels (DEBUG, INFO, WARN, ERROR, FATAL)
    
    // 5. Set up comprehensive monitoring
    // - Application metrics
    // - Infrastructure metrics
    // - Business metrics
    // - Custom business metrics
}

return "Advanced tips and best practices completed!";
}

/**
 * Migration Guide from Basic to Advanced Agent
 */
std::string migration_guide() {
    return R""
Migration Guide from Basic to Advanced Agent
// ============================================

/**
 * Step 1: Understand Advanced Agent Capabilities
 */
void step1_understand_capabilities() {
    // Read documentation for:
    // - Advanced learning algorithms
    // - Policy gradient methods
    // - Experience replay
    // - Meta-learning
    // - Neuromodulation
    // - Performance optimization
}

/**
 * Step 2: Plan Your Advanced Agent Setup
 */
void step2_plan_setup() {
    // Define your use case:
    // - What tasks do you want the agent to learn?
    // - What performance metrics matter?
    // - What hardware resources are available?
    // - What are your deployment requirements?
    
    // Design your agent architecture:
    // - Network architecture
    // - Training algorithm
    // - Experience replay strategy
    // - Neuromodulation approach
    
    // Set up your development environment:
    // - Hardware requirements
    // - Software dependencies
    // - Development tools
}

/**
 * Step 3: Implement Your Advanced Agent
 */
void step3_implement_agent() {
    // Create configuration
    // Configure learning parameters
    // Set up training environment
    // Implement monitoring and logging
}

/**
 * Step 4: Train Your Agent
 */
void step4_train_agent() {
    // Collect training data
    // Set up training pipeline
    // Monitor training progress
    // Save checkpoints periodically
}

/**
 * Step 5: Evaluate and Optimize
 */
void step5_evaluate_and_optimize() {
    // Evaluate agent performance
    // Analyze training metrics
    // Optimize hyperparameters
    // Fine-tune architecture
}

/**
 * Step 6: Deploy in Production
 */
void step6_deploy_production() {
    // Prepare for deployment
    // Set up monitoring
    // Implement alerting
    // Configure scaling
    // Set up backup and recovery
}

/**
 * Step 7: Maintain and Improve
 */
void step7_maintain_and_improve() {
    // Monitor performance
    // Update models
    // Fix bugs
    // Add new features
    // Optimize performance
}

return "Migration guide completed!";
}

/**
 * FAQ for Advanced NLM Users
 */
std::string advanced_faq() {
    return R""
Advanced FAQ
// ==========

Q: What are the key differences between basic and advanced agents?
A: Advanced agents provide:
   - Sophisticated learning algorithms (policy gradient, actor-critic, etc.)
   - Experience replay for continual learning
   - Meta-learning and adaptation capabilities
   - Advanced neuromodulation systems
   - Better performance optimization
   - Comprehensive monitoring and logging

Q: How much training data do I need for advanced agents?
A: Advanced agents typically require:
   - More diverse experiences
   - Better exploration strategies
   - Longer training sessions
   - More computational resources

Q: What hardware do I need for advanced agents?
A: Recommended hardware:
   - GPU (at least 8GB VRAM for basic advanced training)
   - Fast SSD storage
   - Sufficient RAM (16GB+)
   - Multiple CPU cores for parallel processing

Q: How do I tune hyperparameters for advanced agents?
A: Common hyperparameters to tune:
   - Learning rate
   - Discount factor
   - Exploration rate
   - Network architecture
   - Experience replay parameters

Q: How do I monitor advanced agent performance?
A: Use comprehensive monitoring:
   - Training metrics
   - Inference performance
   - System resource usage
   - Custom business metrics

Q: How do I debug issues with advanced agents?
A: Debug advanced agents:
   - Check training metrics
   - Monitor policy entropy
   - Analyze exploration/exploitation balance
   - Check for catastrophic forgetting
   - Review logging and monitoring

Q: How do I deploy advanced agents in production?
A: Deploy advanced agents:
   - Use container orchestration
   - Implement comprehensive monitoring
   - Set up alerting and notifications
   - Use blue-green deployment
   - Implement gradual rollout

Q: How do I maintain and update advanced agents?
A: Maintain advanced agents:
   - Monitor performance continuously
   - Update models based on new data
   - Fix bugs promptly
   - Optimize for changing requirements
   - Keep dependencies updated

Q: What are the common pitfalls when using advanced agents?
A: Common pitfalls:
   - Too much exploration or too little
   - Learning rate that doesn't decay
   - Experience replay buffer that fills up
   - Insufficient training data diversity
   - Poor hyperparameter choices
   - Inadequate monitoring and logging

Q: How do I compare different advanced agent implementations?
A: Compare implementations:
   - Performance metrics
   - Training efficiency
   - Resource usage
   - Deployment complexity
   - Maintainability

return "Advanced FAQ completed!";
}

return "Advanced usage documentation completed successfully!";
}

/**
 * Main function demonstrating advanced usage
 */
std::string demonstrate_advanced_usage() {
    std::cout << "=== Advanced NLM Usage Demonstration ===" << std::endl;
    std::cout << "This demonstration shows advanced agent capabilities." << std::endl;
    
    // Run various advanced examples
    std::cout << "\n1. Advanced Agent Example:" << std::endl;
    std::cout << advanced_agent_example();
    
    std::cout << "\n2. Policy Gradient Example:" << std::endl;
    std::cout << policy_gradient_example();
    
    std::cout << "\n3. Meta-Learning Example:" << std::endl;
    std::cout << meta_learning_example();
    
    std::cout << "\n4. Action Selection Strategies Example:" << std::endl;
    std::cout << action_selection_strategies_example();
    
    std::cout << "\n5. Advanced Neuromodulation Example:" << std::endl;
    std::cout << advanced_neuromodulation_example();
    
    std::cout << "\n6. Policy Network Architecture Example:" << std::endl;
    std::cout << policy_network_architecture_example();
    
    std::cout << "\n7. Advanced Training Techniques Example:" << std::endl;
    std::cout << advanced_training_techniques_example();
    
    std::cout << "\n8. Integration Example:" << std::endl;
    std::cout << integration_example();
    
    std::cout << "\n9. Performance Optimization Example:" << std::endl;
    std::cout << performance_optimization_example();
    
    std::cout << "\n10. Production Deployment Example:" << std::endl;
    std::cout << production_deployment_example();
    
    std::cout << "\n11. Advanced Tips and Best Practices:" << std::endl;
    std::cout << advanced_tips_and_best_practices();
    
    std::cout << "\n12. Migration Guide:" << std::endl;
    std::cout << migration_guide();
    
    std::cout << "\n13. Advanced FAQ:" << std::endl;
    std::cout << advanced_faq();
    
    return "All advanced usage examples completed successfully!";
}

/**
 * Entry point for advanced usage examples
 */
int main() {
    try {
        std::cout << "=== NLM Advanced Usage Examples ===" << std::endl;
        std::cout << "This application demonstrates advanced NLM usage capabilities." << std::endl;
        
        // Set up development environment
        std::cout << "Setting up development environment..." << std::endl;
        
        // Run advanced usage demonstration
        std::string result = demonstrate_advanced_usage();
        
        std::cout << "\n" << result << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
";
}

/**
 * Create advanced documentation file
 */
void create_advanced_documentation() {
    std::ofstream file("ADVANCED_DOCUMENTATION.md");
    if (file.is_open()) {
        file << "# Advanced NLM Documentation\n\n";
        file << "This document provides comprehensive documentation for advanced NLM users." << std::endl;
        file << "\n\n";
        file << "[This file contains the advanced documentation]" << std::endl;
        file.close();
    }
}

/**
 * Main function
 */
int main() {
    // Create advanced documentation
    create_advanced_documentation();
    
    // Run advanced usage examples
    std::string result = demonstrate_advanced_usage();
    
    std::cout << result << std::endl;
    return 0;
}
";
}

// Export advanced usage function
std::string get_advanced_usage_examples() {
    return advanced_usage_examples();
}

// Function to check if advanced documentation exists
bool has_advanced_documentation() {
    std::ifstream file("ADVANCED_DOCUMENTATION.md");
    return file.good();
}

} // namespace nlm

#endif // ADVANCED_DOCUMENTATION_HPP