# Phase 3 Tutorial: World Interaction, Reward Learning

## Learning Objectives

After completing this tutorial, you will:

- Implement sensorimotor loops between brain and environment
- Understand reward prediction error and dopamine signaling
- Configure novelty detection and curiosity systems
- Set up developmental stages and plasticity modulation
- Create world interaction experiments
- Analyze learning curves and behavior patterns

## Prerequisites

- Complete Phase 2 tutorial (understanding of neural dynamics and plasticity)
- Basic understanding of sensorimotor control concepts
- Familiarity with C++ programming and memory management

## Setup

### Required Files

- `src/brain/Brain.hpp` - Brain coordinator
- `src/neuromodulation/Dopamine.hpp` - Reward signaling
- `src/neuromodulation/Curiosity.hpp` - Exploration motivation
- `src/neuromodulation/Novelty.hpp` - Novelty detection
- `src/environment/SimpleWorld.hpp` - Environment interface
- `src/sensory/SensoryInput.hpp` - Sensory processing
- `src/motor/Action.hpp` - Motor output

### Dependencies

```cpp
#include "Brain.hpp"
#include "SimpleWorld.hpp"
#include "SensoryInput.hpp"
#include "Action.hpp"
#include "DopamineSignal.hpp"
```

## Core Concepts

### Sensorimotor Loop

The sensorimotor loop connects brain with environment:

```
WORLD → SENSORY INPUT → BRAIN → ACTION → WORLD
     ↑                          ↓
   REWARD ←——————— NEUROMODULATION ————————
```

#### Components:

1. **Sensory Input:** Encodes environmental state into neural current
2. **Motor Action:** Converts neural activity into environmental interactions
3. **Reward Signal:** Provides teaching signal based on environmental outcomes
4. **Neuromodulation:** Modulates neural excitability and plasticity

### Reward Prediction Error (RPE)

Dopamine signals implement RPE:

```
RPE = actual_reward - predicted_reward
```

- **Positive RPE:** Reward better than expected → strengthen winning paths
- **Negative RPE:** Reward worse than expected → weaken losing paths
- **Zero RPE:** Reward as expected → no learning

### Neuromodulation Systems

#### Dopamine
- Signals reward prediction error
- Modulates neural excitability
- Drives reinforcement learning

#### Curiosity
- Signals unexpected events
- Promotes exploration
- Maximizes information gain

#### Novelty Detection
- Detects unfamiliar patterns
- Triggers learning interest
- Guides exploration

### Developmental Stages

1. **Initial:** High plasticity, rapid learning
2. **Critical Period:** Sensitive to specific experiences
3. **Maturation:** Refining connections
4. **Adult:** Stable, focused learning

Plasticity rates decrease through development.

## Implementation

### Step 1: Create Environment Configuration

```cpp
// Create environment configuration
auto config = std::make_shared<nlm::Config>();

// Core brain configuration
config->set("neuron_count", 1000);
config->set("region_count", 2);
config->set("connection_probability", 0.05);
config->set("simulation_timestep", 0.001);

// Neuromodulation configuration
config->set("dopamine_baseline", 0.1f);
config->set("dopamine_gain", 1.5f);
config->set("curiosity_rate", 0.02f);
config->set("novelty_threshold", 0.3f);
config->set("novelty_sensitivity", 1.0f);

// Development configuration
config->set("critical_period_start", 100);
config->set("critical_period_end", 500);
config->set("development_plasticity_factor", 1.5f);

// Checkpoint for long experiments
config->set("checkpoint_interval", 100);
config->set("max_checkpoints", 100);
```

### Step 2: Initialize Environment and Brain

```cpp
// Create world environment
auto world = std::make_unique<SimpleWorld>();
world->setSize({10.0f, 10.0f});  // 10x10 meter world

// Create brain with config
nlm::Brain brain(config);
if (!brain.initialize()) {
    std::cerr << "Brain initialization failed!" << std::endl;
    return 1;
}

// Create environment interface
auto environment = std::make_unique<EnvironmentInterface>(&brain, world.get());

std::cout << "Phase 3 Environment Setup Complete" << std::endl;
std::cout << "  World size: " << world->getSize().x << " × " << world->getSize().y << " m" << std::endl;
std::cout << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
std::cout << "  Regions: " << brain.getRegionCount() << std::endl;
```

### Step 3: Configure Neuromodulation

```cpp
// Configure dopamine system
auto* dopamine = brain.getDopamine();
if (dopamine) {
    dopamine->configure(0.1f, 1.5f);  // baseline, gain
    std::cout << "Dopamine configured: baseline=0.1, gain=1.5" << std::endl;
}

// Configure curiosity system
auto* curiosity = brain.getCuriosity();
if (curiosity) {
    curiosity->configure(0.02f);  // exploration rate
    std::cout << "Curiosity configured: rate=0.02" << std::endl;
}

// Configure novelty detection
auto* novelty = brain.getNovelty();
if (novelty) {
    novelty->configure(0.3f, 1.0f);  // threshold, sensitivity
    std::cout << "Novelty configured: threshold=0.3, sensitivity=1.0" << std::endl;
}
```

### Step 4: Run Sensorimotor Experiment

```cpp
// Complete sensorimotor loop simulation
const size_t experiment_steps = 3000;
nlm::Timestamp time = 0.0;

std::cout << "\nRunning Sensorimotor Loop Experiment..." << std::endl;

for (nlm::SimulationStep step = 0; step < experiment_steps; ++step) {
    time = step * 0.001;
    
    // 1. Get current world state
    WorldState world_state = environment->getCurrentState();
    
    // 2. Convert to sensory input
    SensoryInput sensory_input = world_state.toSensoryInput();
    
    // 3. Process sensory input through brain
    brain.receiveSensoryInput(sensory_input);
    
    // 4. Run brain simulation step
    brain.step(step, time);
    
    // 5. Produce motor action
    auto action = brain.produceAction();
    
    // 6. Execute action in world
    float reward = environment->executeAction(action.get());
    
    // 7. Apply reward as neuromodulation
    DopamineSignal reward_signal(reward);
    brain.applyNeuromodulation(reward_signal);
    
    // 8. Log statistics periodically
    if (step % 500 == 0) {
        std::cout << "Step " << step << ": "
                  << "Action: " << (action ? action->toString() : "Wait") << ", "
                  << "Reward: " << reward << ", "
                  << "Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    }
}
```

### Step 5: Analyze Learning and Behavior

```cpp
// Analyze learning progress and behavior patterns
void analyze_sensorimotor_learning(nlm::Brain& brain, 
                                   EnvironmentInterface& environment,
                                   size_t total_steps) {
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    std::cout << "\n=== Sensorimotor Learning Analysis ===" << std::endl;
    
    // Analyze reward processing
    if (dopamine) {
        float final_reward = dopamine->getLevel();
        std::cout << "Final reward signal level: " << final_reward << std::endl;
        
        // Calculate reward prediction error statistics
        // (Would need access to reward history)
        std::cout << "Dopamine system active for reinforcement learning" << std::endl;
    }
    
    // Analyze exploration vs exploitation
    if (curiosity) {
        float curiosity_level = curiosity->getLevel();
        std::cout << "Curiosity level: " << curiosity_level << std::endl;
        
        if (curiosity_level > 0.5f) {
            std::cout << "High exploration behavior detected" << std::endl;
        } else {
            std::cout << "Stable exploration/exploitation balance" << std::endl;
        }
    }
    
    // Analyze novelty sensitivity
    if (novelty) {
        float novelty_sensitivity = novelty->getSensitivity();
        std::cout << "Novelty sensitivity: " << novelty_sensitivity << std::endl;
        
        if (novelty_sensitivity > 1.0f) {
            std::cout << "High sensitivity to novel stimuli" << std::endl;
        }
    }
    
    // Analyze action patterns
    std::cout << "\nAction Pattern Analysis:" << std::endl;
    
    // Count action frequencies
    std::map<std::string, size_t> action_counts;
    
    // (In real implementation, would track action history)
    action_counts["MoveForward"] = total_steps / 3;
    action_counts["TurnLeft"] = total_steps / 4;
    action_counts["TurnRight"] = total_steps / 4;
    action_counts["Wait"] = total_steps / 6;
    
    for (const auto& [action, count] : action_counts) {
        float percentage = (count * 100.0f) / total_steps;
        std::cout << "  " << action << ": " << count << " times (" << percentage << "%)" << std::endl;
    }
    
    // Check developmental stage
    auto developmental_stage = brain.getDevelopmentalStage();
    std::cout << "\nDevelopmental Stage: " << static_cast<int>(developmental_stage) << std::endl;
    
    if (developmental_stage == nlm::DevelopmentalStage::Adult) {
        std::cout << "  Mature behavior patterns established" << std::endl;
    } else if (developmental_stage == nlm::DevelopmentalStage::CriticalPeriod) {
        std::cout << "  Critical period for learning" << std::endl;
    }
}
```

## Advanced Concepts

### Reward Prediction Error Learning

```cpp
// Experiment with different reward patterns
void explore_rpe_learning(nlm::Brain& brain, EnvironmentInterface& environment) {
    brain.reset();
    
    // Test different reward schedules
    std::vector<RewardSchedule> schedules = {
        {"Fixed Reward", 0.5f, 100},
        {"Variable Reward", 0.5f, std::uniform_real_distribution<float>(0.2f, 0.8f)},
        {"Delayed Reward", 0.5f, 500},
        {"Punishment", -0.5f, 100}
    };
    
    for (const auto& schedule : schedules) {
        std::cout << "\nTesting " << schedule.name << " schedule:" << std::endl;
        
        // Reset environment
        environment.reset();
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            // Execute action and get reward
            auto action = brain.produceAction();
            float reward = environment.executeAction(action.get());
            
            // Apply reward
            DopamineSignal signal(reward);
            brain.applyNeuromodulation(signal);
            
            // Step brain
            brain.step(step, step * 0.001);
        }
        
        // Analyze learning outcome
        analyze_rpe_effect(brain, schedule);
    }
}
```

### Curiosity-Driven Exploration

```cpp
// Experiment with curiosity-driven behavior
void explore_curiosity_behavior(nlm::Brain& brain, EnvironmentInterface& environment) {
    brain.reset();
    
    std::cout << "Testing curiosity-driven exploration..." << std::endl;
    
    // Track environment exploration
    std::set<WorldPosition> visited_positions;
    std::set<WorldObject> explored_objects;
    
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        // Get current state
        WorldState state = environment->getCurrentState();
        
        // Record exploration
        visited_positions.insert(state.position);
        explored_objects.insert(state.objects.begin(), state.objects.end());
        
        // Process through brain
        brain.receiveSensoryInput(state.toSensoryInput());
        brain.step(step, step * 0.001);
        
        // Get action
        auto action = brain.produceAction();
        
        // Execute
        float reward = environment->executeAction(action.get());
        
        // Apply reward
        brain.applyNeuromodulation(DopamineSignal(reward));
        
        // Log exploration every 200 steps
        if (step % 200 == 0) {
            std::cout << "Step " << step << ": "
                      << "Positions visited: " << visited_positions.size() << ", "
                      << "Objects explored: " << explored_objects.size() << std::endl;
        }
    }
    
    // Calculate exploration metrics
    float exploration_efficiency = (visited_positions.size() * 100.0f) / world_area;
    std::cout << "Exploration efficiency: " << exploration_efficiency << "%" << std::endl;
}
```

## Extensions and Custom Experiments

### Custom Neuromodulation

```cpp
// Create custom neuromodulator
class CustomNeuromodulator : public Neuromodulator {
public:
    void configure(float baseline, float gain) override {
        // Custom configuration
    }
    
    void update(float timestep) override {
        // Custom update logic
    }
    
    float getLevel() const override {
        // Custom level calculation
        return 0.5f;
    }
};

// Use custom neuromodulator
void custom_neuromodulation_experiment() {
    auto config = std::make_shared<nlm::Config>();
    // Add custom neuromodulator configuration
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Add custom neuromodulator to brain
    // Run experiment
}
```

### Multi-environment Adaptation

```cpp
// Test adaptation across different environments
void environment_adaptation_experiment() {
    // Create different world configurations
    std::vector<std::unique_ptr<WorldInterface>> worlds;
    
    worlds.push_back(std::make_unique<SimpleWorld>(10.0f, 10.0f));  // Open field
    worlds.push_back(std::make_unique<ComplexWorld>(5.0f, 5.0f));     // Maze
    worlds.push_back(std::make_unique<DynamicWorld>(20.0f, 20.0f));   // Moving obstacles
    
    for (size_t world_idx = 0; world_idx < worlds.size(); ++world_idx) {
        std::cout << "\nAdaptation to World " << world_idx << ": " 
                  << worlds[world_idx]->getDescription() << std::endl;
        
        // Create brain with fresh state
        auto config = create_optimal_config(world_idx);
        nlm::Brain brain(config);
        brain.initialize();
        
        // Create environment interface
        auto environment = std::make_unique<EnvironmentInterface>(&brain, worlds[world_idx].get());
        
        // Run adaptation simulation
        for (nlm::SimulationStep step = 0; step < 1500; ++step) {
            // Run sensorimotor loop
            auto state = environment->getCurrentState();
            brain.receiveSensoryInput(state.toSensoryInput());
            brain.step(step, step * 0.001);
            
            auto action = brain.produceAction();
            float reward = environment->executeAction(action.get());
            brain.applyNeuromodulation(DopamineSignal(reward));
        }
        
        // Analyze adaptation performance
        analyze_adaptation_performance(brain, *worlds[world_idx]);
    }
}
```

## Results and Interpretation

### Expected Learning Patterns

1. **Initial Random Behavior:** High exploration, low success rate
2. **Learning Phase:** Reward signals strengthen successful actions
3. **Optimization:** Curiosity-driven exploration discovers better strategies
4. **Stabilization:** Efficient behavior with minimal trial-and-error

### Measurement Metrics

- **Success Rate:** Percentage of successful actions
- **Reward Accumulation:** Total reward over time
- **Exploration Rate:** Frequency of novel actions
- **Learning Curve:** Performance improvement over time
- **Action Diversity:** Range of behaviors exhibited

## Troubleshooting

### Common Issues

**Problem:** No meaningful behavior emerges
**Solution:** Check neuromodulation configuration, ensure proper reward signals

**Problem:** Rapid performance degradation
**Solution:** Check dopamine gain settings, verify reward scheduling

**Problem:** Excessive exploration
**Solution:** Adjust curiosity rate, increase novelty threshold

**Problem:** Stable but suboptimal behavior
**Solution:** Check if developmental stage is appropriate for task

### Debugging Commands

```cpp
// Debug neuromodulation
void debug_neuromodulation(nlm::Brain& brain) {
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    if (dopamine) {
        std::cout << "Dopamine: level=" << dopamine->getLevel() 
                  << ", plasticity_factor=" << dopamine->getPlasticityFactor() << std::endl;
    }
    
    if (curiosity) {
        std::cout << "Curiosity: level=" << curiosity->getLevel() << std::endl;
    }
    
    if (novelty) {
        std::cout << "Novelty: sensitivity=" << novelty->getSensitivity() 
                  << ", threshold=" << novelty->getThreshold() << std::endl;
    }
}
```

## Conclusion

Phase 3 extends basic neural dynamics to interact with the environment. You've learned:

- Complete sensorimotor loop implementation
- Reward prediction error signaling via dopamine
- Curiosity-driven exploration behavior
- Developmental modulation of learning
- Analysis of learned behaviors and adaptation patterns

These capabilities enable the brain to learn from environmental feedback and develop effective behaviors.

## See Also

- [API Reference](API_REFERENCE.md)
- [Configuration Reference](CONFIG_REFERENCE.md)
- [Phase 2 Tutorial](PHASE2/README.md)
- [Phase 4 Tutorial](PHASE4/README.md)
- [Phase 6 Tutorial](PHASE6/README.md)
