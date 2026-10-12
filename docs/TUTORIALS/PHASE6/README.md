# Phase 6 Tutorial: Complete Integrated Brain

## Learning Objectives

After completing this tutorial, you will:

- Understand and implement the complete integrated brain architecture
- Master all cognitive, memory, and neuromodulation systems
- Configure and run comprehensive experiments
- Analyze emergent behaviors and cognitive abilities
- Troubleshoot integration issues and performance problems
- Design custom brain experiments

## Prerequisites

- Complete Phase 4 tutorial (understanding of all cognitive systems)
- Basic understanding of neuroscience concepts
- Familiarity with C++ advanced features
- Experience with NLM configuration and debugging

## Setup

### Required Files

- `src/brain/Brain.hpp` - Complete brain coordinator
- `src/neuromodulation/` - All neuromodulation systems
- `src/memory/` - All memory systems
- `src/cognition/` - All cognition systems
- `src/prediction/` - Prediction systems
- `src/environment/` - Environment interfaces
- `src/experiments/` - Experiment framework

### Dependencies

```cpp
#include "Brain.hpp"
#include "ExperimentFramework.hpp"
#include "SensoryInput.hpp"
#include "Action.hpp"
#include "MemoryItem.hpp"
#include "WorldState.hpp"
```

## Core Concepts

### Phase 6: Integrated Brain Architecture

Phase 6 represents the final integration of all NLM systems into a coherent artificial brain:

```
+----------------------------------------------------------------------------+
|                           PHASE 6: INTEGRATED BRAIN                        |
+----------------------------------------------------------------------------+
|                                                                            |
|  SENSORY PROCESSING → NEURAL COMPUTATION → COGNITIVE PROCESSING → ACTION     |
|                                                                            |
|   ↑                  ↑                  ↑                  ↑              |
|   |                  |                  |                  |              |
|  WORLD FEEDBACK    BRAIN DYNAMICS    MEMORY & PLANNING   NEUROMODULATION |
|  (REWARD, ERROR)  (LIF, SPIKES)    (WORKING, EPISODIC) ( DOPAMINE,    ) |
|                                              (CONCEPT, PREDICTION)  CURIOSITY |
|                                                                            |
|                    +-----------+           +-----------+                 |
|                    |           |           |           |                 |
|                    |  PLANNING |           |  MEMORY   |                 |
|                    |  SYSTEM   |           |  SYSTEMS  |                 |
|                    |           |           |           |                 |
|                    +-----------+           +-----------+                 |
|                                                                            |
|                 +-------------------+    +-------------------+            |
|                 |                   |    |                   |            |
|                 |  DEVELOPMENT      |    |  CHECKPOINTING    |            |
|                 |  SYSTEM           |    |  SYSTEM          |            |
|                 |                   |    |                   |            |
|                 +-------------------+    +-------------------+            |
+----------------------------------------------------------------------------+
```

### Integration Challenges and Solutions

1. **Memory-Cognition Integration:** Episodic memory provides context for planning
2. **Neuromodulation-Control:** Dopamine guides attention and action selection
3. **Prediction-Experience:** Forward models learn from episodic replay
4. **Development-Maturation:** Plasticity decreases with experience
5. **Attention-Focus:** Competition balances exploration and exploitation

### Brain Loop Architecture

The complete brain operates in a closed loop:

1. **Perception:** Sensory input → Neural processing
2. **Memory:** Working memory captures current state
3. **Prediction:** Forward model anticipates outcomes
4. **Cognition:** Planning and decision-making
5. **Action:** Motor output → Environmental interaction
6. **Feedback:** Reward and error signals
7. **Learning:** Neuromodulation updates all systems
8. **Consolidation:** Memory replay and development

## Implementation

### Step 1: Create Integration Configuration

```cpp
// Create comprehensive configuration for Phase 6 integration
auto config = std::make_shared<nlm::Config>();

// Ultra-realistic brain configuration
config->set("neuron_count", 50000);                    // Large-scale brain
config->set("region_count", 10);                       // Multiple specialized regions
config->set("connection_probability", 0.02);           // Dense but structured connectivity
config->set("simulation_timestep", 0.0005);            // High temporal resolution

// Working memory (large capacity)
config->set("working_memory_capacity", 10000);         // Extensive working memory
config->set("working_memory_decay", 0.98f);            // Moderate decay
config->set("working_memory_window", 500);             // Long memory window

// Episodic memory (rich experience recording)
config->set("max_episodes", 50000);                    // Extensive episodic storage
config->set("replay_interval", 25);                    // Frequent replay
config->set("consolidation_interval", 200);            // Regular consolidation
config->set("consolidation_threshold", 0.5f);          // Aggressive consolidation

// Associative memory (pattern completion)
config->set("associative_memory_size", 1000000);       // Large associative space

// Prediction systems (forward models)
config->set("prediction_horizon", 500);                // Long-term prediction
config->set("prediction_accuracy", 0.9f);              // High accuracy requirements
config->set("prediction_update_rate", 0.05f);          // Slow, stable learning

// Concept formation (abstract reasoning)
config->set("concept_learning_rate", 0.01f);           // Moderate learning
config->set("concept_hierarchy_depth", 5);             // Multiple abstraction levels
config->set("min_concept_samples", 20);                // High concept standards

// Planning systems (sequential reasoning)
config->set("planning_depth", 10);                     // Multi-step planning
config->set("planning_window", 1000);                  // Extended planning horizon
config->set("planning_efficiency", 0.8f);               // High efficiency requirement

// Attentional selection (competitive processing)
config->set("attention_inhibition", 0.7f);            // Strong attention
config->set("attention_excitation", 2.0f);            // High selectivity
config->set("attention_update_rate", 0.1f);            // Fast attention updates

// Neuromodulation (sophisticated control)
config->set("dopamine_baseline", 0.2f);               // Higher baseline
config->set("dopamine_gain", 2.5f);                   // Strong reward sensitivity
config->set("dopamine_plasticity", 0.8f);            // High plasticity effect
config->set("curiosity_rate", 0.03f);                 // Moderate exploration
config->set("novelty_threshold", 0.4f);                // Sensitive to novelty
config->set("novelty_sensitivity", 1.5f);             // Enhanced novelty detection

// Development (mature learning profile)
config->set("critical_period_start", 500);            // Late critical period
config->set("critical_period_end", 2000);             // Extended critical period
config->set("development_plasticity_factor", 1.2f);   // Slight developmental boost
config->set("maturation_rate", 0.01f);                 // Gradual maturation

// Checkpointing (long-term persistence)
config->set("checkpoint_interval", 500);              // Regular checkpointing
config->set("max_checkpoints", 100);                  // Multiple versions
config->set("compression_level", "best");              // Maximum compression
config->set("checkpoint_dir", "./brain_checkpoints"); // Persistent storage
```

### Step 2: Initialize Integrated Brain

```cpp
// Create and initialize the complete integrated brain
nlm::Brain brain(config);
if (!brain.initialize()) {
    std::cerr << "Failed to initialize integrated brain!" << std::endl;
    return 1;
}

std::cout << "=== PHASE 6 INTEGRATION COMPLETE ===" << std::endl;
std::cout << "Brain initialized with comprehensive cognitive architecture:" << std::endl;
std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
std::cout << "  Brain regions: " << brain.getRegionCount() << std::endl;
std::cout << "  Working memory capacity: " << brain.getWorkingMemory()->getCapacity() << std::endl;
std::cout << "  Episodic memory episodes: " << brain.getEpisodicMemory()->getMaxEpisodes() << std::endl;

// Configure development stage
brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
std::cout << "Developmental stage: Critical Period" << std::endl;
```

### Step 3: Setup Experiment Framework

```cpp
// Create comprehensive experiment framework
class IntegratedBrainExperiment {
private:
    nlm::Brain& brain;
    std::unique_ptr<EnvironmentInterface> environment;
    ExperimentLogger logger;
    
    // Experiment tracking
    struct ExperimentState {
        size_t step;
        nlm::Timestamp time;
        float cumulative_reward;
        size_t concepts_formed;
        size_t episodes_stored;
        std::vector<MemoryPerformance> memory_metrics;
        std::vector<CognitivePerformance> cognitive_metrics;
    } state;
    
public:
    IntegratedBrainExperiment(nlm::Brain& b, std::unique_ptr<EnvironmentInterface> env)
        : brain(b), environment(std::move(env)) {
        
        // Initialize experiment tracking
        state.step = 0;
        state.time = 0.0f;
        state.cumulative_reward = 0.0f;
        state.concepts_formed = 0;
        state.episodes_stored = 0;
        
        // Setup experiment environment
        setup_environment();
        
        // Initialize experiment systems
        initialize_experiment_systems();
    }
    
    void setup_environment() {
        // Create complex environment
        environment->setWorldComplexity(ComplexWorld({20.0f, 20.0f}, 100));
        
        // Add environmental challenges
        environment->addHazardType(HazardType::MovingObstacles);
        environment->addRewardType(RewardType::GoalAchievement);
        environment->addRewardType(RewardType::Novelty);
        environment->addRewardType(RewardType::Efficiency);
        
        // Set task objectives
        environment->setPrimaryObjective(PrimaryObjective::NavigateToGoal);
        environment->setSecondaryObjectives({
            SecondaryObjective::AvoidHazards,
            SecondaryObjective::ExploreEnvironment,
            SecondaryObjective::LearnPaths
        });
    }
    
    void initialize_experiment_systems() {
        // Initialize all brain systems for experiment
        auto* prediction = brain.getPredictionSystem();
        if (prediction) {
            prediction->configure(500, 0.05f);
        }
        
        auto* concepts = brain.getConceptFormation();
        if (concepts) {
            concepts->initialize(&brain);
            concepts->setLearningRate(0.01f);
        }
        
        auto* planner = brain.getPlanner();
        if (planner) {
            planner->initialize(&brain);
            planner->setPlanningDepth(10);
            planner->setPlanningWindow(1000);
        }
        
        auto* attention = brain.getAttention();
        if (attention) {
            attention->initialize(&brain);
            attention->setInhibitionStrength(0.7f);
            attention->setExcitationStrength(2.0f);
        }
        
        // Setup development monitoring
        development_monitor.startMonitoring(&brain);
    }
    
    void run() {
        const size_t total_steps = 20000;
        std::cout << "\nStarting Phase 6 Integration Experiment" << std::endl;
        std::cout << "Total duration: " << total_steps << " steps" << std::endl;
        std::cout << "Environment: " << environment->getDescription() << std::endl;
        
        for (state.step = 0; state.step < total_steps; ++state.step) {
            state.time = state.step * 0.0005;
            
            // Execute one complete brain cycle
            execute_brain_cycle();
            
            // Log experiment progress
            if (state.step % 500 == 0) {
                log_experiment_status();
            }
            
            // Save checkpoints
            if (state.step % 2000 == 0) {
                save_experiment_checkpoint();
            }
        }
        
        // Finalize experiment
        finalize_experiment();
    }
    
private:
    void execute_brain_cycle() {
        // 1. PERCEPTION: Process sensory input
        WorldState world_state = environment->getCurrentState();
        SensoryInput sensory_input = world_state.toSensoryInput();
        brain.receiveSensoryInput(sensory_input);
        
        // 2. COGNITIVE PROCESSING: Run brain simulation
        brain.step(state.step, state.time);
        
        // 3. MEMORY UPDATE: Store experiences
        update_memory_systems();
        
        // 4. PREDICTION: Update forward models
        update_prediction_systems();
        
        // 5. CONCEPT FORMATION: Extract patterns
        update_concept_formation();
        
        // 6. PLANNING: Generate action sequences
        generate_and_execute_action();
        
        // 7. NEUROMODULATION: Apply learning signals
        apply_learning_signals();
        
        // 8. DEVELOPMENT: Age-related changes
        if (state.step % 1000 == 0) {
            brain.develop();
        }
        
        // Update experiment state
        state.cumulative_reward += get_last_reward();
    }
    
    void update_memory_systems() {
        auto* working_memory = brain.getWorkingMemory();
        auto* episodic_memory = brain.getEpisodicMemory();
        
        if (working_memory) {
            // Update working memory with current state
            auto current_neural_state = capture_current_neural_state();
            working_memory->update(0.0005);
            
            // Store important information
            if (should_store_in_working_memory(current_neural_state, brain)) {
                working_memory->storeToMemory(current_neural_state);
            }
        }
        
        if (episodic_memory) {
            // Create episode every 25 steps (rich recording)
            if (state.step % 25 == 0) {
                create_episodic_memory_entry();
            }
        }
    }
    
    void update_prediction_systems() {
        auto* prediction = brain.getPredictionSystem();
        if (prediction) {
            // Update forward model
            float prediction_error = prediction->update(get_current_sensory_input());
            
            if (std::abs(prediction_error) > 0.05f) {
                // Significant prediction error - trigger learning
                trigger_prediction_learning(prediction_error);
            }
        }
    }
    
    void update_concept_formation() {
        auto* concepts = brain.getConceptFormation();
        if (concepts) {
            // Process current neural activity for concept formation
            auto current_pattern = extract_current_neural_pattern(brain);
            concepts->processInput(current_pattern);
            
            // Check for new concepts
            auto new_concepts = concepts->getNewConcepts();
            if (!new_concepts.empty()) {
                state.concepts_formed += new_concepts.size();
                log_concept_formation(new_concepts);
            }
        }
    }
    
    void generate_and_execute_action() {
        auto* planner = brain.getPlanner();
        if (planner) {
            // Get current brain state for planning
            BrainState current_state = capture_brain_state_for_planning();
            
            // Generate action plan
            auto action_plan = planner->planAction(current_state, 5);  // 5-step horizon
            
            if (action_plan) {
                // Execute first action in plan
                execute_action(action_plan->actions[0]);
                
                // Record action outcome
                record_action_outcome(action_plan->actions[0]);
            }
        }
    }
    
    void apply_learning_signals() {
        // Get environmental feedback
        float reward = get_last_reward();
        float novelty = calculate_environmental_novelty();
        
        // Apply dopamine (reward prediction error)
        DopamineSignal reward_signal(reward);
        brain.applyNeuromodulation(reward_signal);
        
        // Apply curiosity (exploration drive)
        if (novelty > 0.3f) {
            DopamineSignal curiosity_signal(novelty);
            brain.applyNeuromodulation(curiosity_signal);
        }
        
        // Apply novelty detection
        auto* novelty_detector = brain.getNovelty();
        if (novelty_detector) {
            float novelty_value = novelty_detector->update(0.0005);
            if (novelty_value > novelty_detector->getThreshold()) {
                log_novelty_event(novelty_value);
            }
        }
    }
    
    void log_experiment_status() {
        std::cout << "\n=== Experiment Status (Step " << state.step << ") ===" << std::endl;
        std::cout << "Time: " << state.time << " s" << std::endl;
        std::cout << "Cumulative Reward: " << state.cumulative_reward << std::endl;
        std::cout << "Concepts Formed: " << state.concepts_formed << std::endl;
        std::cout << "Episodic Episodes: " << state.episodes_stored << std::endl;
        
        // Log system performance
        auto system_metrics = calculate_system_metrics();
        std::cout << "System Performance:" << std::endl;
        std::cout << "  Prediction Accuracy: " << system_metrics.prediction_accuracy << std::endl;
        std::cout << "  Memory Utilization: " << system_metrics.memory_utilization << std::endl;
        std::cout << "  Planning Efficiency: " << system_metrics.planning_efficiency << std::endl;
        std::cout << "  Attention Focus: " << system_metrics.attention_focus << std::endl;
    }
    
    void save_experiment_checkpoint() {
        std::string filename = "experiment_checkpoint_step_" + 
                              std::to_string(state.step) + ".nlm";
        if (brain.save(filename)) {
            std::cout << "Checkpoint saved: " << filename << std::endl;
        } else {
            std::cerr << "Failed to save checkpoint: " << filename << std::endl;
        }
    }
    
    void finalize_experiment() {
        std::cout << "\n=== Experiment Complete ===" << std::endl;
        std::cout << "Total steps: " << state.step << std::endl;
        std::cout << "Final cumulative reward: " << state.cumulative_reward << std::endl;
        std::cout << "Concepts formed: " << state.concepts_formed << std::endl;
        std::cout << "Episodic episodes: " << state.episodes_stored << std::endl;
        
        // Generate final analysis
        generate_final_analysis();
        
        // Save experiment data
        save_experiment_results();
    }
    
    // Additional helper methods implementation would go here
};
```

### Step 4: Run Integration Experiment

```cpp
// Main function to run Phase 6 integration experiment
int main() {
    try {
        // Create configuration
        auto config = create_phase6_config();
        
        // Create environment
        auto world = std::make_unique<ComplexWorld>(WorldSize{20.0f, 20.0f}, 100);
        
        // Create brain
        nlm::Brain brain(config);
        if (!brain.initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        // Create environment interface
        auto environment = std::make_unique<EnvironmentInterface>(&brain, world.get());
        
        // Create and run experiment
        auto experiment = std::make_unique<IntegratedBrainExperiment>(brain, std::move(environment));
        experiment->run();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Step 5: Analyze Integration Results

```cpp
// Analyze complete integration experiment results
void analyze_integration_results(nlm::Brain& brain, 
                                const ExperimentResults& results) {
    std::cout << "\n=== INTEGRATION ANALYSIS ===" << std::endl;
    
    // 1. Cognitive Integration Analysis
    std::cout << "\n1. Cognitive Integration:" << std::endl;
    
    auto* prediction = brain.getPredictionSystem();
    if (prediction) {
        float accuracy = prediction->getAccuracy();
        std::cout << "   Prediction System: " << (accuracy > 0.8f ? "EXCELLENT" : 
                          accuracy > 0.6f ? "GOOD" : "NEEDS IMPROVEMENT") 
                  << " (accuracy=" << accuracy << ")" << std::endl;
    }
    
    auto* concepts = brain.getConceptFormation();
    if (concepts) {
        int concept_count = concepts->getConceptCount();
        float concept_stability = concepts->getConceptStability();
        std::cout << "   Concept Formation: " << concept_count << " concepts, "
                  << (concept_stability > 0.8f ? "STABLE" : "DRIFTING") 
                  << " (stability=" << concept_stability << ")" << std::endl;
    }
    
    auto* planner = brain.getPlanner();
    if (planner) {
        int success_rate = planner->getSuccessRate();
        float efficiency = planner->getEfficiency();
        std::cout << "   Planning System: " << success_rate << "% success, "
                  << (efficiency > 0.7f ? "HIGH" : "MEDIUM") 
                  << " efficiency (efficiency=" << efficiency << ")" << std::endl;
    }
    
    // 2. Memory Integration Analysis
    std::cout << "\n2. Memory Integration:" << std::endl;
    
    auto* working_memory = brain.getWorkingMemory();
    if (working_memory) {
        float utilization = (working_memory->getActiveCount() * 100.0f) / 
                           working_memory->getCapacity();
        std::cout << "   Working Memory: " << utilization << "% utilization, "
                  << (working_memory->getStability() > 0.8f ? "STABLE" : "UNSTABLE")
                  << " (stability=" << working_memory->getStability() << ")" << std::endl;
    }
    
    auto* episodic_memory = brain.getEpisodicMemory();
    if (episodic_memory) {
        float consolidation_efficiency = episodic_memory->getConsolidationEfficiency();
        std::cout << "   Episodic Memory: " << consolidation_efficiency << " consolidation efficiency" << std::endl;
    }
    
    // 3. Neuromodulation Integration Analysis
    std::cout << "\n3. Neuromodulation Integration:" << std::endl;
    
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        float plasticity_effect = dopamine->getPlasticityFactor();
        std::cout << "   Dopamine System: " << (plasticity_effect > 0.7f ? "STRONG" : 
                             plasticity_effect > 0.4f ? "MODERATE" : "WEAK") 
                  << " plasticity effect (factor=" << plasticity_effect << ")" << std::endl;
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        float exploration_level = curiosity->getLevel();
        std::cout << "   Curiosity System: " << (exploration_level > 0.5f ? "HIGH" : 
                             exploration_level > 0.2f ? "MODERATE" : "LOW") 
                  << " exploration (level=" << exploration_level << ")" << std::endl;
    }
    
    // 4. Development Integration Analysis
    std::cout << "\n4. Development Integration:" << std::endl;
    
    auto developmental_stage = brain.getDevelopmentalStage();
    std::cout << "   Developmental Stage: " << static_cast<int>(developmental_stage) << " (";
    switch (developmental_stage) {
        case nlm::DevelopmentalStage::Initial:
            std::cout << "Early Development";
            break;
        case nlm::DevelopmentalStage::CriticalPeriod:
            std::cout << "Critical Period";
            break;
        case nlm::DevelopmentalStage::Maturation:
            std::cout << "Maturation";
            break;
        case nlm::DevelopmentalStage::Adult:
            std::cout << "Adult";
            break;
    }
    std::cout << ")" << std::endl;
    
    // 5. Overall Integration Score
    std::cout << "\n5. Overall Integration:" << std::endl;
    
    float integration_score = calculate_overall_integration_score(brain, results);
    std::cout << "   Integration Score: " << (integration_score > 0.8f ? "EXCELLENT" : 
                         integration_score > 0.6f ? "GOOD" : "NEEDS WORK") 
              << " (" << integration_score << ")" << std::endl;
    
    if (integration_score > 0.8f) {
        std::cout << "   ✓ Brain systems are well integrated and functional" << std::endl;
    } else if (integration_score > 0.6f) {
        std::cout << "   ⚠ Brain systems are partially integrated with room for improvement" << std::endl;
    } else {
        std::cout << "   ✗ Brain systems are poorly integrated and need significant work" << std::endl;
    }
}
```

## Advanced Integration Concepts

### Emergent Behaviors

```cpp
// Study emergent behaviors from integrated brain
void study_emergent_behaviors(nlm::Brain& brain, EnvironmentInterface& env) {
    brain.reset();
    
    std::cout << "Studying Emergent Behaviors..." << std::endl;
    
    // Track behavioral patterns
    std::map<std::string, size_t> behavior_patterns;
    std::vector<std::vector<nlm::Action>> behavioral_sequences;
    
    for (nlm::SimulationStep step = 0; step < 5000; ++step) {
        // Run complete brain cycle
        execute_complete_brain_cycle(brain, env, step);
        
        // Record behavior
        auto action = get_last_action();
        auto world_state = env.getCurrentState();
        
        behavior_patterns[action.toString()]++;
        
        // Analyze sequence patterns
        if (step % 100 == 0) {
            analyze_behavioral_sequences(brain, env, behavioral_sequences);
        }
        
        // Check for emergent properties
        if (step % 1000 == 0) {
            check_for_emergent_properties(brain, env);
        }
    }
    
    // Analyze emergent behavior patterns
    analyze_emergent_behavior_results(behavior_patterns, behavioral_sequences);
}

void check_for_emergent_properties(nlm::Brain& brain, EnvironmentInterface& env) {
    // 1. Behavioral Synergy: Actions that work together better than expected
    auto synergy_score = calculate_behavioral_synergy(brain, env);
    if (synergy_score > 0.7f) {
        std::cout << "✓ Emergent behavioral synergy detected!" << std::endl;
    }
    
    // 2. Cognitive Integration: Smooth coordination between systems
    auto integration_score = calculate_cognitive_integration(brain);
    if (integration_score > 0.8f) {
        std::cout << "✓ High cognitive integration achieved!" << std::endl;
    }
    
    // 3. Memory-Episodic Coupling: Working and episodic memory coordination
    auto memory_coupling = calculate_memory_coupling(brain);
    if (memory_coupling > 0.6f) {
        std::cout << "✓ Strong memory-episodic coupling!" << std::endl;
    }
    
    // 4. Novelty-Driven Learning: Adaptation to new patterns
    auto novelty_learning = calculate_novelty_learning(brain);
    if (novelty_learning > 0.5f) {
        std::cout << "✓ Novelty-driven learning detected!" << std::endl;
    }
}
```

### Custom Integration Experiments

```cpp
// Create custom integration experiments
class CustomIntegrationExperiment {
public:
    virtual ~CustomIntegrationExperiment() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual size_t getDuration() const = 0;
    
    virtual void setup(nlm::Brain& brain, EnvironmentInterface& env) = 0;
    virtual bool execute_step(nlm::Brain& brain, EnvironmentInterface& env, 
                             nlm::SimulationStep step) = 0;
    virtual void analyze_results(nlm::Brain& brain, EnvironmentInterface& env) = 0;
    
    // Common experiment utilities
protected:
    void setup_standard_brain(nlm::Brain& brain) {
        // Apply standard Phase 6 configuration
    }
    
    void setup_standard_environment(EnvironmentInterface& env) {
        // Apply standard environment setup
    }
    
    bool is_experiment_complete(nlm::Brain& brain, EnvironmentInterface& env) {
        // Check if experiment completion criteria are met
        return false;
    }
};

// Example: Navigation Learning Experiment
class NavigationLearningExperiment : public CustomIntegrationExperiment {
public:
    std::string getName() const override {
        return "Navigation Learning";
    }
    
    std::string getDescription() const override {
        return "Test brain's ability to learn and navigate complex environments";
    }
    
    size_t getDuration() const override {
        return 10000;
    }
    
    void setup(nlm::Brain& brain, EnvironmentInterface& env) override {
        // Setup for navigation experiment
        env.setPrimaryObjective(PrimaryObjective::NavigateToGoal);
        env.addHazardType(HazardType::MovingObstacles);
        env.addRewardType(RewardType::GoalAchievement);
        env.addRewardType(RewardType::EfficientPath);
        
        // Configure brain for navigation
        auto* planner = brain.getPlanner();
        if (planner) {
            planner->setPlanningDepth(15);  // Long-range planning
            planner->setPlanningWindow(2000); // Extended planning horizon
        }
    }
    
    bool execute_step(nlm::Brain& brain, EnvironmentInterface& env,
                     nlm::SimulationStep step) override {
        // Execute one step of navigation experiment
        execute_complete_brain_cycle(brain, env, step);
        
        // Track navigation metrics
        if (step % 100 == 0) {
            track_navigation_metrics(brain, env, step);
        }
        
        // Check for goal achievement
        if (has_achieved_goal(env)) {
            log_success("Navigation goal achieved");
            return true; // Experiment complete
        }
        
        return false;
    }
    
    void analyze_results(nlm::Brain& brain, EnvironmentInterface& env) override {
        analyze_navigation_performance(brain, env);
    }
    
private:
    void track_navigation_metrics(nlm::Brain& brain, EnvironmentInterface& env,
                                 nlm::SimulationStep step) {
        // Track path efficiency, obstacle avoidance, goal proximity
        auto path_metrics = calculate_path_metrics(env);
        auto avoidance_metrics = calculate_avoidance_metrics(env);
        
        log_navigation_metrics(step, path_metrics, avoidance_metrics);
    }
    
    bool has_achieved_goal(EnvironmentInterface& env) {
        return env.getCurrentState().has_achieved_objective();
    }
    
    void analyze_navigation_performance(nlm::Brain& brain, EnvironmentInterface& env) {
        // Analyze navigation learning curves and strategies
        auto learning_curve = get_navigation_learning_curve();
        auto strategy_analysis = analyze_navigational_strategies(brain, env);
        
        std::cout << "Navigation Performance Analysis:" << std::endl;
        std::cout << "  Learning curve: " << analyze_learning_curve(learning_curve) << std::endl;
        std::cout << "  Strategy diversity: " << strategy_analysis.diversity << std::endl;
        std::cout << "  Path efficiency: " << strategy_analysis.efficiency << std::endl;
    }
};
```

## Results and Interpretation

### Expected Integration Outcomes

1. **Phase 1-2 Systems:** LIF neurons, basic STDP, simple sensorimotor loops
2. **Phase 3 Integration:** Neuromodulation, reward learning, curiosity-driven exploration
3. **Phase 4 Cognitive Emergence:** Working memory, episodic memory, concept formation
4. **Phase 5 Prediction & Planning:** Forward models, action sequences, attentional control
5. **Phase 6 Complete Integration:** Coordinated cognitive function, adaptive behavior

### Success Metrics

- **Cognitive Performance:** Integration score > 0.8
- **Memory Efficiency:** Working memory utilization < 0.9, episodic memory > 0.7 efficiency
- **Prediction Accuracy:** > 0.8 in forward models
- **Planning Success:** > 70% success rate in action sequences
- **Learning Speed:** Rapid adaptation to novel environments
- **Behavioral Diversity:** Multiple strategies for problem solving
- **Developmental Progress:** Movement through developmental stages

### Analysis Framework

```cpp
// Comprehensive integration analysis framework
struct IntegrationAnalysis {
    float cognitive_integration;
    float memory_efficiency;
    float neuromodulation_effectiveness;
    float predictive_accuracy;
    float planning_success_rate;
    float developmental_progress;
    float behavioral_adaptability;
    float overall_integration_score;
    
    IntegrationAnalysis(nlm::Brain& brain, const ExperimentResults& results) {
        // Calculate all metrics
        cognitive_integration = calculate_cognitive_integration(brain);
        memory_efficiency = calculate_memory_efficiency(brain);
        neuromodulation_effectiveness = calculate_neuromodulation_effectiveness(brain);
        predictive_accuracy = calculate_predictive_accuracy(brain);
        planning_success_rate = calculate_planning_success_rate(brain);
        developmental_progress = calculate_developmental_progress(brain);
        behavioral_adaptability = calculate_behavioral_adaptability(brain, results);
        
        // Calculate weighted overall score
        overall_integration_score = (
            0.25f * cognitive_integration +
            0.20f * memory_efficiency +
            0.15f * neuromodulation_effectiveness +
            0.15f * predictive_accuracy +
            0.10f * planning_success_rate +
            0.10f * developmental_progress +
            0.05f * behavioral_adaptability
        );
    }
    
    std::string get_grade() const {
        if (overall_integration_score > 0.9f) return "EXCELLENT";
        if (overall_integration_score > 0.8f) return "GOOD";
        if (overall_integration_score > 0.7f) return "SATISFACTORY";
        if (overall_integration_score > 0.6f) return "NEEDS IMPROVEMENT";
        return "REQUIRES MAJOR WORK";
    }
};
```

## Troubleshooting

### Common Integration Issues

**Problem:** Systems not communicating properly
**Solution:** Check message passing interfaces, verify event queues, debug system initialization

**Problem:** Memory overload or underflow
**Solution:** Adjust memory system capacities, review memory management algorithms

**Problem:** Prediction system failing
**Solution:** Verify prediction configuration, check sensory input quality, debug learning rates

**Problem:** Planning system stuck
**Solution:** Review planning parameters, check state representation, debug action selection

**Problem:** Development not progressing
**Solution:** Check developmental stage transitions, verify plasticity parameters

### Integration Debugging

```cpp
// Debug complete brain integration
void debug_integration(nlm::Brain& brain) {
    std::cout << "=== INTEGRATION DEBUG ===" << std::endl;
    
    // Check all major systems
    std::cout << "\n1. Core Systems:" << std::endl;
    std::cout << "   Spike System: " << (brain.getSpikeSystem() ? "OK" : "MISSING") << std::endl;
    std::cout << "   STDP: " << (brain.getSTDP() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Hebbian: " << (brain.getHebbian() ? "OK" : "MISSING") << std::endl;
    
    std::cout << "\n2. Memory Systems:" << std::endl;
    std::cout << "   Working Memory: " << (brain.getWorkingMemory() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Episodic Memory: " << (brain.getEpisodicMemory() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Associative Memory: " << (brain.getAssociativeMemory() ? "OK" : "MISSING") << std::endl;
    
    std::cout << "\n3. Cognitive Systems:" << std::endl;
    std::cout << "   Prediction System: " << (brain.getPredictionSystem() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Concept Formation: " << (brain.getConceptFormation() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Neural Planner: " << (brain.getPlanner() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Attentional Selection: " << (brain.getAttention() ? "OK" : "MISSING") << std::endl;
    
    std::cout << "\n4. Neuromodulation Systems:" << std::endl;
    std::cout << "   Dopamine: " << (brain.getDopamine() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Curiosity: " << (brain.getCuriosity() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Novelty: " << (brain.getNovelty() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Prediction Error: " << (brain.getPredictionErrorSignal() ? "OK" : "MISSING") << std::endl;
    
    std::cout << "\n5. Development System:" << std::endl;
    std::cout << "   Development System: " << (brain.getDevelopmentSystem() ? "OK" : "MISSING") << std::endl;
    std::cout << "   Current Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
}
```

## Conclusion

Phase 6 represents the culmination of the NLM project, integrating all systems into a complete artificial brain. You've mastered:

- **Complete Brain Architecture:** All components working together seamlessly
- **Sensorimotor Integration:** Coordinated perception-action loops
- **Memory Systems:** Working, episodic, and associative memory
- **Cognitive Abilities:** Prediction, concept formation, and planning
- **Neuromodulation:** Reward, curiosity, and novelty signaling
- **Development:** Age-dependent learning and maturation
- **Integration Testing:** Comprehensive experiment framework
- **Analysis and Troubleshooting:** Debugging and performance optimization

This integrated brain demonstrates the full potential of computational brain research, combining neural dynamics, learning algorithms, and cognitive architectures into a coherent artificial intelligence system.

## See Also

- [API Reference](API_REFERENCE.md)
- [Configuration Reference](CONFIG_REFERENCE.md)
- [Phase 2 Tutorial](PHASE2/README.md)
- [Phase 3 Tutorial](PHASE3/README.md)
- [Phase 4 Tutorial](PHASE4/README.md)
