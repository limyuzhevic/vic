# Phase 4 Tutorial: Cognitive Abilities, Prediction

## Learning Objectives

After completing this tutorial, you will:

- Understand predictive coding and forward models
- Implement working memory and episodic memory systems
- Configure concept formation and attentional selection
- Set up neural prediction and planning systems
- Create cognitive experiments with memory and prediction
- Analyze cognitive performance and memory capacity

## Prerequisites

- Complete Phase 3 tutorial (understanding of sensorimotor loops and learning)
- Basic understanding of cognitive science concepts
- Familiarity with C++ memory management and data structures

## Setup

### Required Files

- `src/brain/Brain.hpp` - Brain coordinator
- `src/memory/NeuralWorkingMemory.hpp` - Working memory implementation
- `src/memory/NeuralEpisodicMemory.hpp` - Episodic memory implementation
- `src/memory/NeuralAssociativeMemory.hpp` - Associative memory implementation
- `src/memory/ConceptFormation.hpp` - Concept formation system
- `src/cognition/NeuralPlanner.hpp` - Neural planning system
- `src/cognition/AttentionalSelection.hpp` - Attention system
- `src/prediction/NeuralPrediction.hpp` - Prediction system

### Dependencies

```cpp
#include "Brain.hpp"
#include "SensoryInput.hpp"
#include "Action.hpp"
#include "MemoryItem.hpp"
```

## Core Concepts

### Cognitive Architecture

Phase 4 implements cognitive abilities that emerge from neural dynamics:

```
          +-------------------+
          |  SENSORY INPUT    |
          +-------------------+
                  ↓
          +-------------------+
          |    BRAIN CORE    |
          |   (LIF Dynamics) |
          +-------------------+
                  ↓
    +--------------+---------------+
    |   MEMORY     |  PREDICTION   |
    |  SYSTEMS     |  SYSTEMS     |
    |------------- |------------- |
    | Working Mem  | Neural Pred  |
    | Episodic Mem | Action Pred  |
    | Associative  | Multi-step   |
    |              | Prediction   |
    |------------- |------------- |
    |   CONCEPT   |  PLANNING    |
    |  FORMATION   |  SYSTEMS     |
    |              |------------- |
    |   ATTENTION |              |
    |  SELECTION  |              |
    +--------------+--------------+
                  ↓
          +-------------------+
          |   MOTOR OUTPUT    |
          |   (ACTION)        |
          +-------------------+
```

### Working Memory

- **Purpose:** Maintain information temporarily for ongoing tasks
- **Mechanism:** Persistent neural activity with winner-take-all competition
- **Capacity:** Limited, with competitive selection for important items
- **Dynamics:** Activity decays without reinforcement, maintained by recurrent connections

### Episodic Memory

- **Purpose:** Store experience as neural patterns
- **Encoding:** Current brain state captured as episode
- **Replay:** Periodic reactivation for consolidation
- **Retrieval:** Access to past experiences for guidance

### Concept Formation

- **Purpose:** Extract patterns and categories from experience
- **Mechanism:** Statistical learning from neural activity patterns
- **Output:** Abstract concepts without labels
- **Usage:** Guide attention and planning

### Neural Prediction

- **Forward Model:** Predict sensory consequences of actions
- **Prediction Error:** Detect mismatches between prediction and reality
- **Learning:** Update predictions based on errors
- **Planning:** Use predictions to evaluate action outcomes

### Neural Planning

- **Action Sequences:** Evaluate sequences of actions
- **Outcome Prediction:** Simulate consequences of actions
- **Value Estimation:** Assess expected rewards
- **Selection:** Choose optimal action sequences

### Attentional Selection

- **Winner-Take-All:** Select most relevant information
- **Competition:** Neural populations compete for attention
- **Gain Modulation:** Enhance relevant processing
- **Filtering:** Suppress irrelevant inputs

## Implementation

### Step 1: Create Cognitive Configuration

```cpp
// Create cognitive brain configuration
auto config = std::make_shared<nlm::Config>();

// Core configuration (from previous phases)
config->set("neuron_count", 3000);           // More neurons for cognition
config->set("region_count", 3);              // Multiple regions for specialization
config->set("connection_probability", 0.03); // Moderate connectivity
config->set("simulation_timestep", 0.001);    // Fine time step

// Working memory configuration
config->set("working_memory_capacity", 1500);  // Large working memory
config->set("working_memory_decay", 0.95f);    // Moderate decay

// Episodic memory configuration
config->set("max_episodes", 5000);            // Many episodes
config->set("replay_interval", 50);           // Frequent replay
config->set("consolidation_interval", 200);   // Regular consolidation

// Prediction configuration
config->set("prediction_horizon", 100);       // Look 100 steps ahead
config->set("prediction_update_rate", 0.1f);  // Update rate

// Concept formation configuration
config->set("concept_learning_rate", 0.05f);  // Learning rate
config->set("min_concept_samples", 10);       // Minimum samples per concept

// Planning configuration
config->set("planning_depth", 5);             // Plan 5 steps ahead
config->set("planning_window", 20);           // 20-step planning horizon

// Attention configuration
config->set("attention_inhibition", 0.6f);   // Attention strength
config->set("attention_excitation", 1.5f);   // Selection strength

// Development (critical for cognitive maturation)
config->set("critical_period_start", 200);   // Start cognitive development
config->set("critical_period_end", 1000);     // Extended cognitive period
config->set("development_plasticity_factor", 2.0f);  // Strong cognitive development

// Checkpoint for long cognitive experiments
config->set("checkpoint_interval", 200);
config->set("max_checkpoints", 50);
```

### Step 2: Initialize Cognitive Brain

```cpp
// Create and initialize cognitive brain
nlm::Brain brain(config);
if (!brain.initialize()) {
    std::cerr << "Cognitive brain initialization failed!" << std::endl;
    return 1;
}

// Initialize all cognitive systems
auto* working_memory = brain.getWorkingMemory();
if (working_memory) {
    working_memory->initialize(&brain);
    working_memory->setCapacity(1500);
    std::cout << "Working memory initialized with capacity 1500" << std::endl;
}

auto* episodic_memory = brain.getEpisodicMemory();
if (episodic_memory) {
    episodic_memory->initialize(&brain);
    episodic_memory->setMaxEpisodes(5000);
    std::cout << "Episodic memory initialized with capacity 5000 episodes" << std::endl;
}

auto* associative_memory = brain.getAssociativeMemory();
if (associative_memory) {
    associative_memory->initialize(&brain);
    std::cout << "Associative memory initialized" << std::endl;
}
```

### Step 3: Configure Cognitive Systems

```cpp
// Configure prediction system
auto* prediction_system = brain.getPredictionSystem();
if (prediction_system) {
    prediction_system->configure(100, 0.1f);  // horizon, update_rate
    std::cout << "Prediction system configured: horizon=100 steps, update_rate=0.1" << std::endl;
}

// Configure concept formation
auto* concept_formation = brain.getConceptFormation();
if (concept_formation) {
    concept_formation->initialize(&brain);
    concept_formation->setLearningRate(0.05f);
    concept_formation->setMinSamples(10);
    std::cout << "Concept formation configured: rate=0.05, min_samples=10" << std::endl;
}

// Configure neural planner
auto* planner = brain.getPlanner();
if (planner) {
    planner->initialize(&brain);
    planner->setPlanningDepth(5);
    planner->setPlanningWindow(20);
    std::cout << "Neural planner configured: depth=5, window=20" << std::endl;
}

// Configure attentional selection
auto* attention = brain.getAttention();
if (attention) {
    attention->initialize(&brain);
    attention->setInhibitionStrength(0.6f);
    attention->setExcitationStrength(1.5f);
    std::cout << "Attentional selection configured: inhibition=0.6, excitation=1.5" << std::endl;
}
```

### Step 4: Run Cognitive Experiment

```cpp
// Cognitive experiment with memory and prediction
const size_t cognitive_steps = 5000;
nlm::Timestamp time = 0.0;

std::cout << "\nRunning Cognitive Experiment..." << std::endl;
std::cout << "This experiment tests prediction, memory, and planning abilities." << std::endl;

// Initialize experiment tracking
std::vector<MemoryItem> concept_timeline;
std::vector<PredictionError> prediction_errors;

for (nlm::SimulationStep step = 0; step < cognitive_steps; ++step) {
    time = step * 0.001;
    
    // 1. Process sensory input
    SensoryInput current_input = generate_sensor_input(step);
    brain.receiveSensoryInput(current_input);
    
    // 2. Run brain simulation
    brain.step(step, time);
    
    // 3. Update prediction system
    if (step % 50 == 0) {
        auto* prediction = brain.getPredictionSystem();
        if (prediction) {
            float prediction_error = prediction->update(current_input);
            if (std::abs(prediction_error) > 0.1f) {
                prediction_errors.push_back({step, prediction_error});
            }
        }
    }
    
    // 4. Update concept formation
    if (step % 100 == 0) {
        auto* concepts = brain.getConceptFormation();
        if (concepts) {
            concepts->update(time);
            
            // Record new concepts
            auto new_concepts = concepts->getNewConcepts();
            concept_timeline.insert(concept_timeline.end(), 
                                   new_concepts.begin(), new_concepts.end());
        }
    }
    
    // 5. Attentional selection and working memory update
    if (step % 25 == 0) {
        auto* attention = brain.getAttention();
        auto* working_memory = brain.getWorkingMemory();
        
        if (attention && working_memory) {
            // Get current neural activity for attention
            std::vector<nlm::NeuronId> current_activity = get_active_neurons(brain);
            
            // Apply attentional selection
            attention->processCompetition(current_activity);
            
            // Update working memory
            working_memory->update(0.001);
            
            // Store important information in working memory
            if (step % 100 == 0) {
                for (nlm::NeuronId neuron : current_activity) {
                    if (should_store_in_working_memory(neuron, brain)) {
                        working_memory->storeToNeuron(neuron, get_neuron_activation(neuron, brain));
                    }
                }
            }
        }
    }
    
    // 6. Planning and action selection
    if (step % 75 == 0) {
        auto* planner = brain.getPlanner();
        if (planner) {
            // Get current state for planning
            BrainState current_state = capture_brain_state(brain);
            
            // Generate action plans
            auto* action = generate_action_plan(planner, current_state, step);
            
            if (action) {
                // Execute planned action
                execute_planned_action(*action, step);
                
                // Record action outcome
                record_action_outcome(*action, step);
            }
        }
    }
    
    // 7. Memory consolidation (periodically)
    if (step % 200 == 0 && step > 0) {
        auto* episodic_memory = brain.getEpisodicMemory();
        if (episodic_memory) {
            // Store current experience
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = get_last_reward();
            
            // Capture current neural state
            capture_neural_state_for_episode(brain, episode);
            
            // Store episode
            episodic_memory->storeEpisode(episode);
            
            // Trigger consolidation
            episodic_memory->consolidate(0.3f);
            
            std::cout << "Step " << step << ": Stored episode, consolidation completed" << std::endl;
        }
    }
    
    // 8. Log cognitive metrics
    if (step % 500 == 0) {
        log_cognitive_metrics(brain, step, concept_timeline.size(), 
                             prediction_errors.size());
    }
}
```

### Step 5: Analyze Cognitive Performance

```cpp
// Analyze cognitive experiment results
void analyze_cognitive_performance(nlm::Brain& brain,
                                   const std::vector<MemoryItem>& concepts,
                                   const std::vector<PredictionError>& errors,
                                   size_t total_steps) {
    auto* working_memory = brain.getWorkingMemory();
    auto* episodic_memory = brain.getEpisodicMemory();
    auto* concept_formation = brain.getConceptFormation();
    auto* prediction_system = brain.getPredictionSystem();
    auto* planner = brain.getPlanner();
    auto* attention = brain.getAttention();
    
    std::cout << "\n=== Cognitive Performance Analysis ===" << std::endl;
    
    // 1. Concept Formation Analysis
    std::cout << "\n1. Concept Formation:" << std::endl;
    if (concept_formation) {
        int concept_count = concept_formation->getConceptCount();
        std::cout << "   Total concepts formed: " << concept_count << std::endl;
        
        if (concept_count > 0) {
            float concept_stability = concept_formation->getConceptStability();
            std::cout << "   Concept stability: " << concept_stability << std::endl;
            
            // Analyze concept drift
            float concept_drift = analyze_concept_drift(concept_timeline);
            std::cout << "   Concept drift: " << concept_drift << std::endl;
        }
    }
    
    // 2. Working Memory Analysis
    std::cout << "\n2. Working Memory:" << std::endl;
    if (working_memory) {
        int active_items = working_memory->getActiveCount();
        int capacity = working_memory->getCapacity();
        float utilization = (active_items * 100.0f) / capacity;
        
        std::cout << "   Active items: " << active_items << " / " << capacity 
                  << " (" << utilization << "% utilization)" << std::endl;
        
        // Check for memory capacity limits
        if (utilization > 0.9f) {
            std::cout << "   Warning: Working memory near capacity limit" << std::endl;
        }
        
        // Analyze memory stability
        float memory_stability = working_memory->getStability();
        std::cout << "   Memory stability: " << memory_stability << std::endl;
    }
    
    // 3. Episodic Memory Analysis
    std::cout << "\n3. Episodic Memory:" << std::endl;
    if (episodic_memory) {
        int episode_count = episodic_memory->getEpisodeCount();
        int replayable_count = episodic_memory->getReplayableEpisodes();
        
        std::cout << "   Stored episodes: " << episode_count << std::endl;
        std::cout << "   Ready for replay: " << replayable_count << std::endl;
        
        if (episode_count > 0) {
            float consolidation_efficiency = episodic_memory->getConsolidationEfficiency();
            std::cout << "   Consolidation efficiency: " << consolidation_efficiency << std::endl;
            
            // Check memory decay
            float memory_decay = episodic_memory->getMemoryDecay();
            std::cout << "   Memory decay rate: " << memory_decay << std::endl;
        }
    }
    
    // 4. Prediction System Analysis
    std::cout << "\n4. Prediction System:" << std::endl;
    if (prediction_system) {
        float prediction_accuracy = prediction_system->getAccuracy();
        std::cout << "   Prediction accuracy: " << prediction_accuracy << std::endl;
        
        float prediction_confidence = prediction_system->getConfidence();
        std::cout << "   Prediction confidence: " << prediction_confidence << std::endl;
        
        if (!prediction_errors.empty()) {
            float avg_prediction_error = calculate_average_error(prediction_errors);
            std::cout << "   Average prediction error: " << avg_prediction_error << std::endl;
            
            // Analyze error patterns
            analyze_prediction_error_patterns(prediction_errors);
        }
    }
    
    // 5. Planning System Analysis
    std::cout << "\n5. Planning System:" << std::endl;
    if (planner) {
        int plan_success_rate = planner->getSuccessRate();
        float planning_efficiency = planner->getEfficiency();
        
        std::cout << "   Plan success rate: " << plan_success_rate << "%" << std::endl;
        std::cout << "   Planning efficiency: " << planning_efficiency << std::endl;
        
        // Analyze planning depth utilization
        int avg_depth = planner->getAveragePlanningDepth();
        std::cout << "   Average planning depth: " << avg_depth << std::endl;
    }
    
    // 6. Attention System Analysis
    std::cout << "\n6. Attention System:" << std::endl;
    if (attention) {
        float attention_focus = attention->getFocusStrength();
        std::cout << "   Attention focus: " << attention_focus << std::endl;
        
        // Analyze selective attention
        auto attention_stats = attention->getStatistics();
        std::cout << "   Selective attention ratio: " << attention_stats.selective << std::endl;
        std::cout << "   Distraction rejection ratio: " << attention_stats.rejection << std::endl;
    }
    
    // 7. Integration Analysis
    std::cout << "\n7. System Integration:" << std::endl;
    
    // Check developmental stage
    auto developmental_stage = brain.getDevelopmentalStage();
    std::cout << "   Developmental stage: " << static_cast<int>(developmental_stage) << std::endl;
    
    if (developmental_stage == nlm::DevelopmentalStage::Adult) {
        std::cout << "   Cognitive maturity achieved" << std::endl;
    }
    
    // Overall performance metrics
    float total_performance = calculate_overall_performance(brain, concepts, errors);
    std::cout << "\nOverall cognitive performance: " << total_performance << std::endl;
    
    if (total_performance > 0.8f) {
        std::cout << "   Excellent cognitive performance!" << std::endl;
    } else if (total_performance > 0.6f) {
        std::cout << "   Good cognitive performance with room for improvement" << std::endl;
    } else {
        std::cout << "   Cognitive performance needs improvement" << std::endl;
    }
}
```

## Advanced Concepts

### Forward Models and Prediction

```cpp
// Implement forward model learning
void forward_model_learning(nlm::Brain& brain, EnvironmentInterface& env) {
    auto* prediction = brain.getPredictionSystem();
    if (!prediction) return;
    
    // Learn action-effect mappings
    std::map<nlm::Action, PredictedOutcome> action_outcomes;
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        // Get current state
        WorldState state = env.getCurrentState();
        
        // Generate action
        auto action = brain.produceAction();
        
        // Execute and observe outcome
        env.executeAction(action.get());
        WorldState next_state = env.getCurrentState();
        
        // Learn prediction
        float predicted_outcome = prediction->predict(action, state);
        float actual_outcome = calculate_outcome(next_state);
        
        prediction->learn(action, state, predicted_outcome, actual_outcome);
        
        // Update action-outcome mapping
        action_outcomes[action] = {actual_outcome, step};
    }
    
    // Analyze prediction accuracy
    analyze_forward_model_accuracy(prediction, action_outcomes);
}
```

### Working Memory Capacity

```cpp
// Study working memory capacity limits
void working_memory_capacity_experiment(nlm::Brain& brain) {
    brain.reset();
    
    // Test different memory loads
    std::vector<int> load_levels = {10, 50, 100, 200, 500};
    
    for (int load : load_levels) {
        std::cout << "\nTesting working memory capacity with " << load << " items..." << std::endl;
        
        auto* working_memory = brain.getWorkingMemory();
        if (working_memory) {
            working_memory->clear();
            working_memory->setCapacity(load);
        }
        
        // Fill memory with random items
        for (int i = 0; i < load; ++i) {
            nlm::NeuronId neuron_id = generate_random_neuron(brain);
            float activation = generate_random_activation();
            working_memory->storeToNeuron(neuron_id, activation);
        }
        
        // Test recall performance
        int recall_success = test_recall_performance(brain, load);
        float recall_accuracy = (recall_success * 100.0f) / load;
        
        std::cout << "  Recall accuracy: " << recall_accuracy << "%" << std::endl;
        
        // Allow memory to decay
        for (nlm::SimulationStep step = 0; step < 500; ++step) {
            brain.step(step, step * 0.001);
            if (working_memory) {
                working_memory->update(0.001);
            }
            
            if (step % 100 == 0) {
                int remaining = working_memory->getActiveCount();
                std::cout << "  Step " << step << ": " << remaining << " items remaining" << std::endl;
            }
        }
    }
}
```

### Concept Formation Dynamics

```cpp
// Study concept formation and categorization
void concept_formation_experiment(nlm::Brain& brain) {
    brain.reset();
    
    // Generate patterns for concept learning
    std::vector<PatternSet> pattern_sets = {
        {"Red Circles", { {1.0f, 0.0f}, {1.2f, 0.1f}, {0.9f, -0.1f} }},
        {"Blue Squares", { {0.0f, 1.0f}, {0.1f, 1.2f}, {-0.1f, 0.9f} }},
        {"Green Triangles", { {-1.0f, 0.0f}, {-1.1f, 0.1f}, {-0.9f, -0.1f} }},
        {"Yellow Rectangles", { {0.0f, -1.0f}, {0.1f, -1.1f}, {-0.1f, -0.9f} }}
    };
    
    std::cout << "Concept Formation Experiment" << std::endl;
    std::cout << "Learning to categorize visual patterns..." << std::endl;
    
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        // Present patterns
        PatternSet current_pattern = select_pattern(pattern_sets);
        
        // Encode pattern in neural activity
        encode_pattern_in_neural_activity(brain, current_pattern);
        
        // Run brain simulation
        brain.step(step, step * 0.001);
        
        // Update concept formation
        auto* concepts = brain.getConceptFormation();
        if (concepts) {
            concepts->processInput(current_pattern.values);
            
            // Check for new concepts
            auto new_concepts = concepts->getNewConcepts();
            if (!new_concepts.empty()) {
                std::cout << "Step " << step << ": New concepts detected:" << std::endl;
                for (const auto& concept : new_concepts) {
                    std::cout << "  - " << concept.name << std::endl;
                }
            }
        }
        
        // Periodically test categorization
        if (step % 300 == 0) {
            test_concept_categorization(brain, pattern_sets);
        }
    }
}
```

## Extensions and Custom Experiments

### Hierarchical Cognitive Architecture

```cpp
// Implement multi-level cognitive architecture
class HierarchicalCognitiveSystem {
private:
    nlm::Brain& brain;
    std::vector<std::unique_ptr<HierarchicalCognitiveSystem>> subsystems;
    
public:
    HierarchicalCognitiveSystem(nlm::Brain& b) : brain(b) {
        // Create cognitive hierarchy
        subsystems.push_back(std::make_unique<PrimaryCognitiveSystem>(brain));
        subsystems.push_back(std::make_unique<SecondaryCognitiveSystem>(brain));
        subsystems.push_back(std::make_unique<TertiaryCognitiveSystem>(brain));
    }
    
    void update() {
        // Update from bottom to top
        for (auto& subsystem : subsystems) {
            subsystem->update();
        }
        
        // Integrate information
        integrate_subsystem_outputs();
    }
};
```

### Memory-Epidemic Interaction

```cpp
// Study interaction between working and episodic memory
void memory_interaction_experiment(nlm::Brain& brain) {
    brain.reset();
    
    std::cout << "Memory Interaction Experiment" << std::endl;
    std::cout << "Testing working/episodic memory interaction..." << std::endl;
    
    auto* working_memory = brain.getWorkingMemory();
    auto* episodic_memory = brain.getEpisodicMemory();
    
    for (nlm::SimulationStep step = 0; step < 3000; ++step) {
        // Generate and process sensory input
        brain.receiveSensoryInput(generate_input(step));
        brain.step(step, step * 0.001);
        
        // Working memory dynamics
        if (working_memory) {
            working_memory->update(0.001);
            
            // Store in working memory based on attention
            if (step % 50 == 0) {
                auto* attention = brain.getAttention();
                if (attention) {
                    auto focused_neuron = attention->getFocusedNeuron();
                    if (focused_neuron) {
                        working_memory->storeToNeuron(*focused_neuron, 
                                                   get_neuron_activation(*focused_neuron, brain));
                    }
                }
            }
        }
        
        // Episodic memory formation
        if (step % 200 == 0 && step > 0) {
            if (episodic_memory) {
                // Create episode from working memory content
                EpisodicMemoryItem episode;
                episode.timestamp = step;
                
                // Capture working memory state
                if (working_memory) {
                    auto active_items = working_memory->getMemoryNeurons();
                    for (nlm::NeuronId neuron : active_items) {
                        episode.activeNeurons.push_back(neuron);
                        episode.neuronActivations.push_back(
                            working_memory->getNeuronActivation(neuron));
                    }
                }
                
                episodic_memory->storeEpisode(episode);
                
                // Replay and consolidate
                auto replays = episodic_memory->getEpisodesForReplay(3);
                for (const auto* replay_episode : replays) {
                    episodic_memory->replayEpisode(replay_episode);
                }
            }
        }
        
        // Log interaction metrics
        if (step % 500 == 0) {
            log_memory_interaction_metrics(brain);
        }
    }
}
```

## Results and Interpretation

### Expected Cognitive Development

1. **Initial Phase (0-200 steps):** Random activity, basic memory formation
2. **Learning Phase (200-800 steps):** Concept emergence, prediction refinement
3. **Optimization Phase (800-2000 steps):** Efficient planning, stable concepts
4. **Integration Phase (2000-5000 steps):** Coordinated cognitive function

### Key Performance Indicators

- **Concept Formation Rate:** New concepts per step
- **Prediction Accuracy:** Correctness of forward models
- **Planning Success:** Effectiveness of action sequences
- **Memory Retention:** Durability of stored information
- **Attentional Focus:** Quality of selective attention
- **Cognitive Integration:** Coordination between systems

## Troubleshooting

### Common Issues

**Problem:** No concepts emerge
**Solution:** Check concept formation parameters, ensure sufficient input variation

**Problem:** Poor prediction accuracy
**Solution:** Verify prediction system configuration, check training data quality

**Problem:** Inefficient planning
**Solution:** Review planning depth and window settings, check state representation

**Problem:** Working memory overload
**Solution:** Adjust capacity, review attentional selection parameters

### Debugging Commands

```cpp
// Debug cognitive systems
void debug_cognitive_systems(nlm::Brain& brain) {
    std::cout << "=== Cognitive System Debug ===" << std::endl;
    
    // Check each cognitive system
    auto* working_memory = brain.getWorkingMemory();
    if (working_memory) {
        std::cout << "Working Memory: " << working_memory->getActiveCount() << "/" 
                  << working_memory->getCapacity() << " items active" << std::endl;
    }
    
    auto* episodic_memory = brain.getEpisodicMemory();
    if (episodic_memory) {
        std::cout << "Episodic Memory: " << episodic_memory->getEpisodeCount() 
                  << " episodes stored" << std::endl;
    }
    
    auto* concept_formation = brain.getConceptFormation();
    if (concept_formation) {
        std::cout << "Concept Formation: " << concept_formation->getConceptCount()
                  << " concepts formed" << std::endl;
    }
    
    auto* prediction = brain.getPredictionSystem();
    if (prediction) {
        std::cout << "Prediction System: accuracy=" << prediction->getAccuracy() << std::endl;
    }
    
    auto* planner = brain.getPlanner();
    if (planner) {
        std::cout << "Planning System: success_rate=" << planner->getSuccessRate() << "%" << std::endl;
    }
    
    auto* attention = brain.getAttention();
    if (attention) {
        std::cout << "Attention System: focus_strength=" << attention->getFocusStrength() << std::endl;
    }
}
```

## Conclusion

Phase 4 implements the full cognitive architecture of the NLM system. You've learned:

- Working memory mechanisms and capacity limits
- Episodic memory encoding and replay
- Concept formation and categorization
- Neural prediction and forward modeling
- Action planning and sequence evaluation
- Attentional selection and focus
- Integration of multiple cognitive systems

These cognitive abilities enable the brain to learn, remember, plan, and reason about its environment and actions.

## See Also

- [API Reference](API_REFERENCE.md)
- [Configuration Reference](CONFIG_REFERENCE.md)
- [Phase 2 Tutorial](PHASE2/README.md)
- [Phase 3 Tutorial](PHASE3/README.md)
- [Phase 6 Tutorial](PHASE6/README.md)
