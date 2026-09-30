# Advanced Agent Tutorial

## Creating Sophisticated NLM Agents

This tutorial covers advanced techniques for creating intelligent agents using the NLM framework. It builds on the basic concepts and explores complex agent architectures, learning strategies, and behaviors.

## Overview of Advanced Agent Design

Advanced NLM agents combine multiple cognitive systems to create intelligent, adaptive behavior. Key components include:

1. **Sensory Processing**: Converting environmental input to neural signals
2. **Working Memory**: Maintaining active goals and plans
3. **Episodic Memory**: Learning from past experiences
4. **Prediction Systems**: Anticipating future states
5. **Decision Making**: Selecting actions based on learned patterns
6. **Neuromodulation**: Adjusting behavior based on rewards and curiosity
7. **Development**: Maturing and improving over time

## Building an Advanced Cognitive Agent

### 1. Agent Architecture

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include "nlm/brain/Brain.hpp"
#include <iostream>
#include <memory>

class AdvancedCognitiveAgent {
public:
    AdvancedCognitiveAgent(const std::string& name, const nlm::Config& config)
        : agentName_(name) {
        
        // Create the brain with advanced configuration
        brain_ = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
        
        // Initialize brain with development
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Create agent brain
        agent_ = std::make_unique<nlm::AgentBrain>(brain_);
        
        // Enable all advanced features
        enableAdvancedFeatures();
        
        std::cout << "Advanced agent '" << agentName_ << "' created successfully" << std::endl;
    }
    
    void enableAdvancedFeatures() {
        // Core cognitive features
        agent_->enableRewardModulation(true);      // Learn from rewards
        agent_->enableStructuralPlasticity(true);  // Grow new connections
        agent_->enableDevelopment(true);           // Mature over time
        agent_->enableCuriosity(true);             // Explore novelties
        
        // Advanced cognitive subsystems
        enableMemorySystems();
        enablePredictionSystem();
        enablePlanningSystem();
        enableAttentionSystem();
        enableConceptFormation();
    }
    
    void enableMemorySystems() {
        // Configure memory systems for optimal performance
        auto workingMem = brain_->getWorkingMemory();
        if (workingMem) {
            workingMem->setDecayRate(0.02f);  // 2% decay per step
            workingMem->setCapacity(200);   // 200 working memory slots
        }
        
        auto episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->setConsolidationThreshold(0.7f);  // Consolidate >70% relevance
            episodicMem->setCapacity(1000);              // 1000 episodes
        }
        
        auto semanticMem = brain_->getSemanticMemory();
        if (semanticMem) {
            semanticMem->setAutonomyFactor(0.1f);  // Auto-learn new facts
        }
    }
    
    void enablePredictionSystem() {
        auto predictionSys = brain_->getPredictionSystem();
        if (predictionSys) {
            predictionSys->setPredictionHorizon(10);     // Predict 10 steps ahead
            predictionSys->setPredictionConfidence(0.5f); // Initial confidence
            predictionSys->enableErrorCorrection(true);   // Learn from prediction errors
        }
    }
    
    void enablePlanningSystem() {
        auto planner = brain_->getPlanner();
        if (planner) {
            planner->setPlanningHorizon(5);        // Plan 5 steps ahead
            planner->setActionSelectionMethod(
                nlm::PlanningMethod::THOMSON_SAMPLING
            );
            planner->enableHierarchicalPlanning(true);
        }
    }
    
    void enableAttentionSystem() {
        auto attention = brain_->getAttention();
        if (attention) {
            attention->setAttentionWindow(8);     // Focus on 8 features
            attention->setSelectionThreshold(0.3f); // Select features >30%
            attention->enableDynamicAllocation(true);
        }
    }
    
    void enableConceptFormation() {
        auto conceptFormation = brain_->getConceptFormation();
        if (conceptFormation) {
            conceptFormation->setConceptThreshold(0.6f);  // New concept threshold
            conceptFormation->enableConceptMerging(true);
            conceptFormation->enableConceptHierarchies(true);
        }
    }
    
    void run(int steps) {
        if (!world_) {
            throw std::runtime_error("Agent not initialized with world");
        }
        
        std::cout << "Running agent '" << agentName_ << "' for " << steps << " steps" << std::endl;
        
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            updateAgent(step);
            
            if (step % 100 == 0) {
                logAgentState(step);
            }
        }
        
        std::cout << "Agent simulation completed" << std::endl;
    }
    
protected:
    void updateAgent(nlm::SimulationStep step) {
        // 1. Update world environment
        world_->update(0.1);  // 100ms time step
        
        // 2. Get sensory perception
        nlm::SensoryPercept percept = world_->getSensoryPercept();
        
        // 3. Process sensory input through agent
        agent_->processSensoryInput(percept);
        
        // 4. Brain cognitive processing
        brain_->step(step);
        
        // 5. Advanced decision making
        nlm::MotorCommand action = makeAdvancedDecision(step);
        
        // 6. Execute action in world
        world_->applyMotorCommand(action, world_->getSimulationTime());
        
        // 7. Apply neuromodulation based on experience
        applyAdvancedNeuromodulation(step);
        
        // 8. Update development system
        agent_->updateDevelopment(0.1);
    }
    
    nlm::MotorCommand makeAdvancedDecision(nlm::SimulationStep step) {
        // Get current brain predictions
        float predictionError = getPredictionError();
        float curiosityLevel = getCuriosityLevel();
        float noveltyLevel = getNoveltyLevel();
        
        // Check if we're in a novel situation
        if (noveltyLevel > 0.8f && curiosityLevel > 0.5f) {
            // High novelty and curiosity - explore
            return generateExploratoryAction();
        }
        
        // Check prediction error
        if (predictionError > 0.5f) {
            // High prediction error - learn more
            return generateLearningAction();
        }
        
        // Normal operation - use planned actions
        nlm::MotorCommand plannedAction = getPlannedAction();
        
        // Add some stochasticity for exploration
        if (shouldExplore(step)) {
            return combineWithExploration(plannedAction);
        }
        
        return plannedAction;
    }
    
    void applyAdvancedNeuromodulation(nlm::SimulationStep step) {
        // Calculate complex neuromodulation signals
        float reward = calculateAdaptiveReward(step);
        float predictedReward = predictFutureReward(step);
        float novelty = calculateNovelty(step);
        float uncertainty = calculateUncertainty(step);
        
        // Apply dopamine-based reward prediction
        float dopamineSignal = (reward - predictedReward) * 0.5f;
        agent_->applyRewardModulation(dopamineSignal, predictedReward);
        
        // Apply curiosity-driven exploration
        float curiositySignal = novelty * uncertainty * 0.3f;
        if (curiositySignal > 0.3f) {
            // Boost exploration
            agent_->enableCuriosity(true);
        }
        
        // Apply attention-based modulation
        float attentionSignal = calculateAttentionLevel(step);
        if (attentionSignal > 0.7f) {
            // Focus on relevant features
            focusOnRelevantFeatures(step);
        }
    }
    
    void initializeWorld(const nlm::WorldConfig& worldConfig) {
        world_ = std::make_unique<nlm::SimpleWorld>();
        world_->configure(worldConfig);
        world_->reset();
        
        agent_->initialize(*world_);
        
        // Initialize agent's knowledge of the world
        initializeWorldKnowledge();
    }
    
    void initializeWorldKnowledge() {
        // Load basic world knowledge
        auto semanticMem = brain_->getSemanticMemory();
        if (semanticMem) {
            // Basic physics facts
            semanticMem->storeFact("gravity", "9.8 m/s² downward");
            semanticMem->storeFact("friction", "surface-dependent resistance");
            
            // Object properties
            semanticMem->storeFact("solid_objects", "maintain shape");
            semanticMem->storeFact("liquid_objects", "flow and conform");
        }
        
        // Create initial episodic memory
        auto episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            nlm::EpisodicMemoryItem initialExperience;
            initialExperience.timestamp = 0;
            initialExperience.metadata = "Agent initialized in world";
            episodicMem->storeEpisode(initialExperience);
        }
    }
    
    // Helper methods (simplified for brevity)
    nlm::MotorCommand generateExploratoryAction() { return nlm::MotorCommand{}; }
    nlm::MotorCommand generateLearningAction() { return nlm::MotorCommand{}; }
    nlm::MotorCommand getPlannedAction() { return nlm::MotorCommand{}; }
    bool shouldExplore(nlm::SimulationStep step) { return (step % 20) == 0; }
    nlm::MotorCommand combineWithExploration(const nlm::MotorCommand& action) { return action; }
    float getPredictionError() { return agent_->getPredictionError(); }
    float getCuriosityLevel() { return agent_->getCuriosityLevel(); }
    float getNoveltyLevel() { return agent_->getNoveltyLevel(); }
    float calculateAdaptiveReward(nlm::SimulationStep step) { return (rand() % 100) / 100.0f; }
    float predictFutureReward(nlm::SimulationStep step) { return 0.5f; }
    float calculateNovelty(nlm::SimulationStep step) { return (rand() % 100) / 100.0f; }
    float calculateUncertainty(nlm::SimulationStep step) { return (rand() % 100) / 100.0f; }
    float calculateAttentionLevel(nlm::SimulationStep step) { return (rand() % 100) / 100.0f; }
    void focusOnRelevantFeatures(nlm::SimulationStep step) {}
    void logAgentState(nlm::SimulationStep step) {
        std::cout << "Step " << step 
                 << ", Stage: " << static_cast<int>(agent_->getDevelopmentalStage())
                 << ", Curiosity: " << agent_->getCuriosityLevel()
                 << ", Novelty: " << agent_->getNoveltyLevel()
                 << ", Error: " << agent_->getPredictionError() << std::endl;
    }
    void simulateWorldInteraction(nlm::AgentBrain& agent, int episode) {
        // Simplified world interaction
    }
    
private:
    std::string agentName_;
    std::shared_ptr<nlm::Brain> brain_;
    std::unique_ptr<nlm::AgentBrain> agent_;
    std::unique_ptr<nlm::SimpleWorld> world_;
};
```

### 2. Advanced Agent Configuration

```cpp
void createAdvancedAgentConfig() {
    nlm::Config config;
    
    // Core brain configuration
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("region_count", static_cast<int64_t>(5));
    config.set("connection_probability", 0.15f);
    
    // Memory system optimization
    config.set("working_memory_capacity", 300);
    config.set("episodic_memory_capacity", 2000);
    config.set("semantic_memory_capacity", 10000);
    config.set("associative_memory_capacity", 5000);
    
    // Plasticity settings for advanced learning
    config.set("stdp_learning_rate", 0.03f);
    config.set("hebbian_learning_rate", 0.015f);
    config.set("structural_plasticity_rate", 0.002f);
    config.set("plasticity_eligibility_decay", 0.98f);
    
    // Development configuration
    config.set("development_enabled", true);
    config.set("developmental_stage_count", 6);
    config.set("developmental_stage_duration", 1000);
    config.set("developmental_plasticity_modifier", 2.0f);
    
    // Prediction and planning
    config.set("prediction_system_enabled", true);
    config.set("prediction_horizon", 15);
    config.set("prediction_error_threshold", 0.2f);
    
    config.set("planning_system_enabled", true);
    config.set("planning_horizon", 8);
    config.set("action_selection_method", "thompson_sampling");
    
    // Neuromodulation
    config.set("dopamine_enabled", true);
    config.set("dopamine_learning_rate", 0.2f);
    config.set("curiosity_enabled", true);
    config.set("curiosity_threshold", 0.4f);
    config.set("novelty_enabled", true);
    config.set("novelty_decay_rate", 0.9f);
    
    // Advanced cognitive features
    config.set("concept_formation_enabled", true);
    config.set("concept_threshold", 0.7f);
    config.set("attention_enabled", true);
    config.set("attention_window_size", 12);
    
    return config;
}
```

### 3. Complex Agent Behaviors

#### A. Hierarchical Planning Agent

```cpp
class HierarchicalPlanningAgent : public AdvancedCognitiveAgent {
public:
    HierarchicalPlanningAgent(const std::string& name) 
        : AdvancedCognitiveAgent(name, createAdvancedAgentConfig()) {
        
        // Enable hierarchical planning
        auto planner = brain_->getPlanner();
        if (planner) {
            planner->enableHierarchicalPlanning(true);
            planner->setMaxPlanningDepth(4);
        }
        
        // Configure for goal-based behavior
        setupGoalBasedBehavior();
    }
    
    void setupGoalBasedBehavior() {
        // Define high-level goals
        goals_.push_back({"explore", 0.8f});
        goals_.push_back({"learn", 0.9f});
        goals_.push_back({"survive", 1.0f});
        
        // Create sub-goals for each high-level goal
        createSubGoals();
        
        // Enable goal management in episodic memory
        auto episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->setGoalTrackingEnabled(true);
        }
    }
    
    nlm::MotorCommand makeAdvancedDecision(nlm::SimulationStep step) override {
        // Get current goals
        auto activeGoals = getActiveGoals(step);
        
        if (!activeGoals.empty()) {
            // Select goal based on priority and feasibility
            Goal selectedGoal = selectBestGoal(activeGoals);
            
            // Get sub-goals for selected goal
            auto subGoals = getSubGoals(selectedGoal);
            
            // Execute sub-goal planning
            return executeHierarchicalPlanning(subGoals);
        }
        
        // Fall back to regular decision making
        return AdvancedCognitiveAgent::makeAdvancedDecision(step);
    }
    
private:
    struct Goal {
        std::string name;
        float priority;
        float progress;
    };
    
    std::vector<Goal> goals_;
    
    Goal selectBestGoal(const std::vector<Goal>& goals) {
        // Select goal with highest priority * progress
        Goal bestGoal = goals[0];
        float bestScore = goals[0].priority * goals[0].progress;
        
        for (const auto& goal : goals) {
            float score = goal.priority * goal.progress;
            if (score > bestScore) {
                bestScore = score;
                bestGoal = goal;
            }
        }
        
        return bestGoal;
    }
    
    nlm::MotorCommand executeHierarchicalPlanning(const std::vector<Goal>& subGoals) {
        // Simple hierarchical planning implementation
        // In practice, this would use the brain's planning system
        return nlm::MotorCommand{};
    }
};
```

#### B. Social Learning Agent
n
```cpp
class SocialLearningAgent : public AdvancedCognitiveAgent {
public:
    SocialLearningAgent(const std::string& name) 
        : AdvancedCognitiveAgent(name, createAdvancedAgentConfig()) {
        
        // Enable social learning features
        setupSocialLearning();
    }
    
    void setupSocialLearning() {
        // Enable observation-based learning
        auto episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->setObservationLearningEnabled(true);
            episodicMem->setSocialMemoryEnabled(true);
        }
        
        // Enable imitation learning
        auto associativeMem = brain_->getAssociativeMemory();
        if (associativeMem) {
            associativeMem->setImitationLearningEnabled(true);
        }
        
        // Configure for social curiosity
        curiosityConfig_.socialLearningFactor = 0.8f;
        curiosityConfig_..observationFactor = 0.9f;
    }
    
    void processSocialExperience(const SocialExperience& experience) {
        // Store observed behavior in episodic memory
        auto episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            nlm::EpisodicMemoryItem observation;
            observation.timestamp = getCurrentStep();
            observation.metadata = "Observed: " + experience.description;
            
            // Store observed actions
            for (const auto& action : experience.observedActions) {
                observation.neurons.push_back(getActionNeuronId(action));
                observation.values.push_back(0.7f);  // High confidence in observation
            }
            
            episodicMem->storeEpisode(observation);
        }
        
        // Learn from observation (imitation)
        learnFromObservation(experience);
        
        // Update social knowledge
        updateSocialKnowledge(experience);
    }
    
private:
    struct SocialExperience {
        std::string description;
        std::vector<nlm::MotorCommand> observedActions;
        float observerConfidence;
        std::string actorIdentity;
    };
    
    void learnFromObservation(const SocialExperience& experience) {
        auto associativeMem = brain_->getAssociativeMemory();
        if (!associativeMem) return;
        
        // Create associations between observed actions and contexts
        for (size_t i = 0; i < experience.observedActions.size(); ++i) {
            if (i + 1 < experience.observedActions.size()) {
                NeuronId action1 = getActionNeuronId(experience.observedActions[i]);
                NeuronId action2 = getActionNeuronId(experience.observedActions[i + 1]);
                
                associativeMem->associate(action1, action2, 
                                         experience.observerConfidence);
            }
        }
    }
    
    NeuronId getActionNeuronId(const nlm::MotorCommand& action) {
        // Map motor actions to neuron IDs
        // This would depend on your specific action representation
        return static_cast<NeuronId>(action.actionType + 1000);
    }
};
```

#### C. Adaptive Environment Agent

```cpp
class AdaptiveEnvironmentAgent : public AdvancedCognitiveAgent {
public:
    AdaptiveEnvironmentAgent(const std::string& name) 
        : AdvancedCognitiveAgent(name, createAdvancedAgentConfig()) {
        
        // Enable environment adaptation
        setupEnvironmentAdaptation();
    }
    
    void setupEnvironmentAdaptation() {
        // Configure for dynamic environment response
        environmentConfig_.dynamicResponseEnabled = true;
        environmentConfig_.predictiveModelingEnabled = true;
        environmentConfig_.adaptiveStrategyEnabled = true;
        
        // Enable environment monitoring
        auto episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->setEnvironmentTrackingEnabled(true);
        }
    }
    
    void updateEnvironmentModel() {
        // Update internal model of environment
        auto episodicMem = brain_->getEpisodicMemory();
        if (!episodicMem) return;
        
        // Get recent environmental data
        auto recentExperiences = episodicMem->getRecentEpisodes(20);
        
        // Build environment model
        environmentModel_ = buildEnvironmentModel(recentExperiences);
        
        // Predict environmental changes
        predictEnvironmentalChanges();
    }
    
    nlm::MotorCommand makeAdvancedDecision(nlm::SimulationStep step) override {
        // Update environment model
        updateEnvironmentModel();
        
        // Get environment predictions
        auto predictions = getEnvironmentPredictions();
        
        // Select action based on environment predictions
        return selectActionBasedOnEnvironment(predictions);
    }
    
private:
    struct EnvironmentConfig {
        bool dynamicResponseEnabled = false;
        bool predictiveModelingEnabled = false;
        bool adaptiveStrategyEnabled = false;
    };
    
    struct EnvironmentModel {
        std::vector<float> hazardLocations;
        std::vector<float> resourceLocations;
        std::vector<std::string> environmentalPatterns;
        float volatility;  // How changeable the environment is
    };
    
    EnvironmentConfig environmentConfig_;
    EnvironmentModel environmentModel_;
    
    EnvironmentModel buildEnvironmentModel(const std::vector<nlm::EpisodicMemoryItem>& experiences) {
        EnvironmentModel model;
        
        for (const auto& experience : experiences) {
            // Analyze experience for environmental patterns
            analyzeEnvironmentalPattern(experience, model);
        }
        
        return model;
    }
    
    void predictEnvironmentalChanges() {
        // Implement environment change prediction
        // This would use the brain's prediction system
    }
    
    std::vector<EnvironmentPrediction> getEnvironmentPredictions() {
        // Return predicted environmental states
        return std::vector<EnvironmentPrediction>{};
    }
    
    nlm::MotorCommand selectActionBasedOnEnvironment(const std::vector<EnvironmentPrediction>& predictions) {
        // Select actions based on predicted environmental states
        // This would use the brain's planning system
        return nlm::MotorCommand{};
    }
    
    void analyzeEnvironmentalPattern(const nlm::EpisodicMemoryItem& experience, EnvironmentModel& model) {
        // Extract environmental information from experience
        // Update model based on analysis
    }
};
```

### 4. Advanced Agent Testing and Evaluation

#### Performance Metrics

```cpp
struct AdvancedAgentMetrics {
    // Learning metrics
    float learningCurve[100];           // Performance over time
    float explorationRate;              // How often agent explores
    float exploitationRate;             // How often agent exploits
    float curiosityScore;               // Novelty-driven exploration
    
    // Decision quality metrics
    float actionSuccessRate;            // Percentage of successful actions
    float planningAccuracy;             // How accurate planning is
    float predictionError;              // Prediction system accuracy
    
    // Adaptation metrics
    float adaptationSpeed;              // How quickly agent adapts
    float stability;                     // How stable performance is over time
    float transferLearning;              // Ability to apply learned skills
    
    // Resource efficiency
    float memoryUsage;                   // Memory consumption
    float computationalCost;             // CPU usage
    float energyEfficiency;              // Power efficiency
};
```

#### Comprehensive Testing Framework

```cpp
class AdvancedAgentTester {
public:
    static void runComprehensiveTests(AdvancedCognitiveAgent& agent) {
        std::cout << "Running comprehensive agent tests..." << std::endl;
        
        // Test 1: Basic functionality
        testBasicFunctionality(agent);
        
        // Test 2: Memory system integration
        testMemoryIntegration(agent);
        
        // Test 3: Learning capabilities
        testLearningCapabilities(agent);
        
        // Test 4: Decision making
        testDecisionMaking(agent);
        
        // Test 5: Adaptation
        testAdaptation(agent);
        
        // Test 6: Stress testing
        testStress(agent);
        
        // Generate performance report
        generatePerformanceReport(agent);
    }
    
    static void testBasicFunctionality(AdvancedCognitiveAgent& agent) {
        // Verify agent can initialize and run
        // Check all systems are enabled
        // Verify basic action production
    }
    
    static void testMemoryIntegration(AdvancedCognitiveAgent& agent) {
        // Test memory systems work together
        // Verify episodic, semantic, and working memory integration
        // Check memory consolidation and retrieval
    }
    
    static void testLearningCapabilities(AdvancedCognitiveAgent& agent) {
        // Test reinforcement learning
        // Test curiosity-driven learning
        // Test social learning (if implemented)
        // Test transfer learning
    }
    
    static void testDecisionMaking(AdvancedCognitiveAgent& agent) {
        // Test planning system
        // Test prediction-based decisions
        // Test action selection strategies
        // Test exploration-exploitation balance
    }
    
    static void testAdaptation(AdvancedCognitiveAgent& agent) {
        // Test adaptation to changing environments
        // Test recovery from failures
        // Test learning new tasks
        // Test generalization
    }
    
    static void testStress(AdvancedCognitiveAgent& agent) {
        // Test under resource constraints
        // Test with limited memory
        // Test with high noise
        // Test under time pressure
    }
    
    static void generatePerformanceReport(AdvancedCognitiveAgent& agent) {
        // Generate comprehensive performance report
        // Include metrics, graphs, and recommendations
    }
};
```

---

## Configuration Guide

### Advanced Configuration Parameters

```cpp
struct AdvancedAgentConfiguration {
    // Core brain parameters
    int neuronCount = 1000;
    int regionCount = 5;
    float connectionProbability = 0.15f;
    
    // Memory system parameters
    int workingMemoryCapacity = 300;
    int episodicMemoryCapacity = 2000;
    int semanticMemoryCapacity = 10000;
    int associativeMemoryCapacity = 5000;
    
    // Plasticity parameters
    float stdpLearningRate = 0.03f;
    float hebbianLearningRate = 0.015f;
    float structuralPlasticityRate = 0.002f;
    float plasticityEligibilityDecay = 0.98f;
    
    // Development parameters
    bool developmentEnabled = true;
    int developmentalStageCount = 6;
    int developmentalStageDuration = 1000;
    float developmentalPlasticityModifier = 2.0f;
    
    // Prediction parameters
    bool predictionSystemEnabled = true;
    int predictionHorizon = 15;
    float predictionErrorThreshold = 0.2f;
    
    // Planning parameters
    bool planningSystemEnabled = true;
    int planningHorizon = 8;
    std::string actionSelectionMethod = "thompson_sampling";
    
    // Neuromodulation parameters
    bool dopamineEnabled = true;
    float dopamineLearningRate = 0.2f;
    bool curiosityEnabled = true;
    float curiosityThreshold = 0.4f;
    bool noveltyEnabled = true;
    float noveltyDecayRate = 0.9f;
    
    // Cognitive feature parameters
    bool conceptFormationEnabled = true;
    float conceptThreshold = 0.7f;
    bool attentionEnabled = true;
    int attentionWindowSize = 12;
    
    // Agent behavior parameters
    float explorationRate = 0.3f;
    float exploitationRate = 0.7f;
    float rewardSensitivity = 1.0f;
    float punishmentSensitivity = 0.8f;
};
```

### Configuration Creation Helper

```cpp
nlm::Config createAdvancedAgentConfig(const AdvancedAgentConfiguration& config) {
    nlm::Config nlmConfig;
    
    // Core brain
    nlmConfig.set("neuron_count", static_cast<int64_t>(config.neuronCount));
    nlmConfig.set("region_count", static_cast<int64_t>(config.regionCount));
    nlmConfig.set("connection_probability", config.connectionProbability);
    
    // Memory systems
    nlmConfig.set("working_memory_capacity", config.workingMemoryCapacity);
    nlmConfig.set("episodic_memory_capacity", config.episodicMemoryCapacity);
    nlmConfig.set("semantic_memory_capacity", config.semanticMemoryCapacity);
    nlmConfig.set("associative_memory_capacity", config.associativeMemoryCapacity);
    
    // Plasticity
    nlmConfig.set("stdp_learning_rate", config.stdpLearningRate);
    nlmConfig.set("hebbian_learning_rate", config.hebbianLearningRate);
    nlmConfig.set("structural_plasticity_rate", config.structuralPlasticityRate);
    nlmConfig.set("plasticity_eligibility_decay", config.plasticityEligibilityDecay);
    
    // Development
    nlmConfig.set("development_enabled", config.developmentEnabled);
    nlmConfig.set("developmental_stage_count", config.developmentalStageCount);
    nlmConfig.set("developmental_stage_duration", config.developmentalStageDuration);
    nlmConfig.set("developmental_plasticity_modifier", config.developmentalPlasticityModifier);
    
    // Prediction
    nlmConfig.set("prediction_system_enabled", config.predictionSystemEnabled);
    nlmConfig.set("prediction_horizon", config.predictionHorizon);
    nlmConfig.set("prediction_error_threshold", config.predictionErrorThreshold);
    
    // Planning
    nlmConfig.set("planning_system_enabled", config.planningSystemEnabled);
    nlmConfig.set("planning_horizon", config.planningHorizon);
    nlmConfig.set("action_selection_method", config.actionSelectionMethod);
    
    // Neuromodulation
    nlmConfig.set("dopamine_enabled", config.dopamineEnabled);
    nlmConfig.set("dopamine_learning_rate", config.dopamineLearningRate);
    nlmConfig.set("curiosity_enabled", config.curiosityEnabled);
    nlmConfig.set("curiosity_threshold", config.curiosityThreshold);
    nlmConfig.set("novelty_enabled", config.noveltyEnabled);
    nlmConfig.set("novelty_decay_rate", config.noveltyDecayRate);
    
    // Cognitive features
    nlmConfig.set("concept_formation_enabled", config.conceptFormationEnabled);
    nlmConfig.set("concept_threshold", config.conceptThreshold);
    nlmConfig.set("attention_enabled", config.attentionEnabled);
    nlmConfig.set("attention_window_size", config.attentionWindowSize);
    
    return nlmConfig;
}
```

---

## Performance Optimization Guide

### Optimizing Advanced Agent Performance

```cpp
class PerformanceOptimizer {
public:
    static void optimizeBrainPerformance(nlm::Brain& brain) {
        // Optimize neuron organization
        optimizeNeuronOrganization(brain);
        
        // Optimize memory usage
        optimizeMemoryUsage(brain);
        
        // Optimize plasticity
        optimizePlasticity(brain);
        
        // Optimize development
        optimizeDevelopment(brain);
    }
    
    static void optimizeNeuronOrganization(nlm::Brain& brain) {
        // Implement efficient neuron distribution
        // Use sparse connectivity where possible
        // Optimize neuron types and densities
        
        auto* spikeSystem = brain.getSpikeSystem();
        if (spikeSystem) {
            // Configure spike system for efficiency
            spikeSystem->setMaxNeurons(brain.getTotalNeuronCount() * 0.9f);
            spikeSystem->setRefractoryPeriod(2);
        }
    }
    
    static void optimizeMemoryUsage(nlm::Brain& brain) {
        // Configure memory systems for optimal performance
        auto* workingMem = brain.getWorkingMemory();
        if (workingMem) {
            // Set optimal working memory size
            size_t optimalWorkingMem = std::min(
                brain.getTotalNeuronCount() / 10, 
                static_cast<size_t>(200)
            );
            // workingMem->setCapacity(optimalWorkingMem); // If API available
        }
        
        auto* episodicMem = brain.getEpisodicMemory();
        if (episodicMem) {
            // Configure episodic memory consolidation
            // episodicMem->setConsolidationThreshold(0.6f); // If API available
        }
    }
    
    static void optimizePlasticity(nlm::Brain& brain) {
        // Fine-tune plasticity parameters
        auto* stdp = brain.getSTDP();
        if (stdp) {
            // stdp->setLearningRate(0.025f); // If API available
            // stdp->setTimeWindow(15); // If API available
        }
        
        auto* hebbian = brain.getHebbian();
        if (hebbian) {
            // hebbian->setLearningRate(0.0125f); // If API available
        }
    }
    
    static void optimizeDevelopment(nlm::Brain& brain) {
        // Configure development for optimal maturation
        brain.setDevelopmentalStage(1); // Start in early development
        
        // Set developmental parameters
        // brain.setPlasticityModifier(1.5f); // If API available
        // brain.setGrowthRate(0.3f); // If API available
    }
};
```

### Monitoring Agent Performance

```cpp
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage = 0.0;
        double memoryUsage = 0.0;
        double simulationTime = 0.0;
        size_t activeNeurons = 0;
        size_t firingNeurons = 0;
        float averageFiringRate = 0.0f;
        float predictionAccuracy = 0.0f;
        float learningRate = 0.0f;
        float stabilityScore = 0.0f;
    };
    
    static PerformanceMetrics monitorPerformance(const AdvancedCognitiveAgent& agent) {
        PerformanceMetrics metrics;
        
        // Get brain metrics
        auto brainMetrics = getBrainMetrics(agent.getBrain());
        metrics.activeNeurons = brainMetrics.activeNeurons;
        metrics.firingNeurons = brainMetrics.firingNeurons;
        metrics.averageFiringRate = brainMetrics.averageFiringRate;
        
        // Get agent metrics
        auto agentMetrics = getAgentMetrics(agent);
        metrics.predictionAccuracy = agentMetrics.predictionAccuracy;
        metrics.learningRate = agentMetrics.learningRate;
        metrics.stabilityScore = agentMetrics.stabilityScore;
        
        // Get system metrics
        metrics.cpuUsage = getCPUUsage();
        metrics.memoryUsage = getMemoryUsage();
        metrics.simulationTime = getSimulationTime();
        
        return metrics;
    }
    
    static void logPerformance(const PerformanceMetrics& metrics) {
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  CPU Usage: " << metrics.cpuUsage * 100 << "%" << std::endl;
        std::cout << "  Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
        std::cout << "  Simulation Time: " << metrics.simulationTime << " ms" << std::endl;
        std::cout << "  Active Neurons: " << metrics.activeNeurons << std::endl;
        std::cout << "  Firing Neurons: " << metrics.firingNeurons << std::endl;
        std::cout << "  Average Firing Rate: " << metrics.averageFiringRate << " Hz" << std::endl;
        std::cout << "  Prediction Accuracy: " << metrics.predictionAccuracy * 100 << "%" << std::endl;
        std::cout << "  Learning Rate: " << metrics.learningRate << std::endl;
        std::cout << "  Stability Score: " << metrics.stabilityScore << std::endl;
    }
    
    static void alertOnPerformanceIssues(const PerformanceMetrics& metrics) {
        // Check for performance issues and log alerts
        if (metrics.cpuUsage > 0.9f) {
            std::cerr << "ALERT: High CPU usage!" << std::endl;
        }
        
        if (metrics.memoryUsage > 500.0f) {
            std::cerr << "ALERT: High memory usage!" << std::endl;
        }
        
        if (metrics.simulationTime > 100.0f) {
            std::cerr << "ALERT: Simulation running slow!" << std::endl;
        }
        
        if (metrics.averageFiringRate < 1.0f) {
            std::cerr << "ALERT: Low neural activity!" << std::endl;
        }
        
        if (metrics.predictionAccuracy < 0.3f) {
            std::cerr << "ALERT: Poor prediction accuracy!" << std::endl;
        }
        
        if (metrics.stabilityScore < 0.5f) {
            std::cerr << "ALERT: Unstable performance!" << std::endl;
        }
    }
    
private:
    static PerformanceMetrics getBrainMetrics(nlm::Brain* brain) {
        PerformanceMetrics metrics;
        if (brain) {
            metrics.activeNeurons = brain->getActiveNeuronCount();
            metrics.firingNeurons = brain->getFiringNeuronCount();
            metrics.averageFiringRate = brain->getAverageFiringRate();
        }
        return metrics;
    }
    
    static PerformanceMetrics getAgentMetrics(const AdvancedCognitiveAgent& agent) {
        PerformanceMetrics metrics;
        // Get agent-specific metrics
        return metrics;
    }
    
    static double getCPUUsage() { return 0.5; }  // Placeholder
    static double getMemoryUsage() { return 256.0; }  // Placeholder
    static double getSimulationTime() { return 10.0; }  // Placeholder
};
```

---

## Summary

### Key Takeaways for Advanced Agent Development

1. **Architecture**: Use a comprehensive agent architecture with multiple cognitive systems
2. **Memory Integration**: Integrate all memory systems (working, episodic, semantic, procedural, associative)
3. **Advanced Features**: Enable prediction, planning, attention, and concept formation
4. **Neuromodulation**: Use dopamine, curiosity, and novelty for adaptive behavior
5. **Development**: Enable developmental stages for maturation and improvement
6. **Testing**: Use comprehensive testing to ensure agent performance
7. **Optimization**: Continuously optimize performance through monitoring and tuning

### Next Steps

1. Start with basic agent configuration
2. Add memory systems
3. Enable prediction and planning
4. Add neuromodulation
5. Test with various scenarios
6. Optimize based on performance metrics
7. Deploy in real-world applications

By following this tutorial, you can create sophisticated NLM agents capable of complex learning, adaptive behavior, and intelligent decision-making.