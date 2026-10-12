# NLM Cheat Sheet

## Quick Reference Guide

This cheat sheet provides essential commands and patterns for working with the NLM system.

## Brain Initialization

### Basic Setup
```cpp
#include "Brain.hpp"
#include "Config.hpp"

// Create configuration
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 1000);
config->set("region_count", 1);
config->set("connection_probability", 0.1f);

// Create and initialize brain
nlm::Brain brain(config);
if (!brain.initialize()) {
    // Handle initialization error
    return 1;
}
```

### Advanced Configuration
```cpp
// Complete configuration for cognitive brain
config->set("neuron_count", 10000);
config->set("region_count", 5);
config->set("connection_probability", 0.05f);
config->set("simulation_timestep", 0.001);
config->set("stdp_ltp_weight", 0.01f);
config->set("stdp_ltd_weight", 0.012f);
config->set("stdp_tau", 20.0f);
config->set("working_memory_capacity", 1000);
config->set("max_episodes", 5000);
config->set("dopamine_baseline", 0.1f);
config->set("curiosity_rate", 0.01f);
```

## Core Operations

### Simulation Loop
```cpp
// Main simulation loop
const size_t total_steps = 10000;
nlm::Timestamp time = 0.0;
nlm::SimulationStep step = 0;

for (step = 0; step < total_steps && !stop_condition; ++step) {
    time = step * timestep;
    
    // Process brain dynamics
    brain.step(step, time);
    
    // Occasionally save checkpoint
    if (step % checkpoint_interval == 0) {
        brain.save(f"checkpoint_{step}.nlm");
    }
    
    // Log progress
    if (step % 1000 == 0) {
        brain.logStatus();
    }
}
```

### Input/Output Handling
```cpp
// Sensory input processing
void processEnvironment(nlm::Brain& brain) {
    // Get sensor readings
    std::vector<float> sensor_data = read_sensors();
    
    if (!sensor_data.empty()) {
        // Create sensory input
        nlm::SensoryInput input(sensor_data);
        
        // Send to brain
        brain.receiveSensoryInput(input);
    }
}

// Action generation and execution
void executeAction(nlm::Brain& brain) {
    // Generate motor command
    auto action = brain.produceAction();
    
    if (action) {
        // Execute action based on type
        switch (action->type) {
            case nlm::ActionType::MoveForward:
                move_forward(action->parameters.speed);
                break;
            case nlm::ActionType::TurnLeft:
                turn_left(action->parameters.angle);
                break;
            case nlm::ActionType::TurnRight:
                turn_right(action->parameters.angle);
                break;
            case nlm::ActionType::Wait:
                // Do nothing
                break;
        }
    }
}
```

### Memory Operations
```cpp
// Working memory inspection
void inspect_working_memory(nlm::Brain& brain) {
    auto* working_memory = brain.getWorkingMemory();
    if (working_memory) {
        std::cout << "Working memory active items: " 
                  << working_memory->getActiveCount() << std::endl;
        std::cout << "Working memory capacity: " 
                  << working_memory->getCapacity() << std::endl;
    }
}

// Episodic memory statistics
void inspect_episodic_memory(nlm::Brain& brain) {
    auto* episodic_memory = brain.getEpisodicMemory();
    if (episodic_memory) {
        size_t episodes = episodic_memory->getEpisodeCount();
        size_t replayable = episodic_memory->getReplayableEpisodes();
        std::cout << "Episodes stored: " << episodes << std::endl;
        std::cout << "Episodes ready for replay: " << replayable << std::endl;
    }
}
```

## Accessor Patterns

### Common Accessor Usage
```cpp
// Access plasticity systems
void use_plasticity_systems(nlm::Brain& brain) {
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structural = brain.getStructuralPlasticity();
    
    // Configure STDP
    if (stdp) {
        stdp->configure(0.01f, 0.012f, 20.0f);
    }
    
    // Record activity
    if (hebbian) {
        // Hebbian learning happens automatically
    }
}

// Access neuromodulation
void use_neuromodulation(nlm::Brain& brain) {
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    // Monitor neuromodulator levels
    if (dopamine) {
        float reward = dopamine->getLevel();
        std::cout << "Reward signal: " << reward << std::endl;
    }
}

// Access cognition systems
void use_cognition(nlm::Brain& brain) {
    auto* planner = brain.getPlanner();
    auto* concepts = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    // Use planner for action planning
    if (planner) {
        planner->setPlanningDepth(5);
    }
    
    // Use attention system
    if (attention) {
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
}
```

## Development and Lifecycle

### Development Management
```cpp
// Development control
void manage_development(nlm::Brain& brain) {
    auto* development = brain.getDevelopmentSystem();
    
    if (development) {
        // Check developmental stage
        auto stage = brain.getDevelopmentalStage();
        std::cout << "Developmental stage: " 
                  << static_cast<int>(stage) << std::endl;
        
        // Manually trigger development
        brain.develop();
        
        // Set specific stage
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    }
}
```

### State Management
```cpp
// Checkpoint management
void checkpoint_management(nlm::Brain& brain) {
    // Save brain state
    if (brain.save("my_brain_state.nlm")) {
        std::cout << "Brain state saved successfully" << std::endl;
    } else {
        std::cerr << "Failed to save brain state" << std::endl;
    }
    
    // Load brain state (clears current state)
    if (brain.load("previous_brain_state.nlm")) {
        std::cout << "Brain state loaded successfully" << std::endl;
    } else {
        std::cerr << "Failed to load brain state" << std::endl;
    }
    
    // Reset to initial state
    brain.reset();
}
```

## Statistical Monitoring

### System Statistics
```cpp
// Brain statistics and metrics
void monitor_brain(nlm::Brain& brain) {
    // Basic statistics
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "Firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    
    // Network properties
    float ei_ratio = brain.getExcitationInhibitionRatio();
    std::cout << "E/I ratio: " << ei_ratio << std::endl;
    
    // Spike system status
    size_t pending_spikes = brain.getPendingSpikeEventCount();
    std::cout << "Pending spikes: " << pending_spikes << std::endl;
}

// Log brain status
void log_system_status(nlm::Brain& brain) {
    brain.logStatus();
    
    // Log detailed information
    std::cout << "=== NLM Brain Status ===" << std::endl;
    std::cout << "Simulation step: " << brain.getStepCount() << std::endl;
    std::cout << "Current time: " << brain.getCurrentTime() << " s" << std::endl;
    std::cout << "Developmental stage: " 
              << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
}
```

## Error Handling

### Basic Error Handling
```cpp
// Safe brain operations
bool safe_brain_operation(nlm::Brain& brain) {
    try {
        // Perform operation
        brain.step(1, 0.001);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

// Check brain state
bool is_brain_healthy(nlm::Brain& brain) {
    // Check critical systems
    if (!brain.getSpikeSystem()) {
        std::cerr << "Spike system not initialized" << std::endl;
        return false;
    }
    
    if (!brain.getSTDP()) {
        std::cerr << "STDP system not initialized" << std::endl;
        return false;
    }
    
    return true;
}
```

## Performance Monitoring

### Timing and Performance
```cpp
// Performance monitoring
class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    size_t steps_processed = 0;
    
public:
    void start() { start_time = std::chrono::high_resolution_clock::now(); }
    
    void step_completed() {
        ++steps_processed;
    }
    
    void report() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double time_per_step = duration.count() / static_cast<double>(steps_processed);
        double steps_per_second = steps_processed / (duration.count() / 1000.0);
        
        std::cout << "Performance report:" << std::endl;
        std::cout << "  Total steps: " << steps_processed << std::endl;
        std::cout << "  Total time: " << duration.count() << " ms" << std::endl;
        std::cout << "  Time per step: " << time_per_step << " ms" << std::endl;
        std::cout << "  Steps per second: " << steps_per_second << std::endl;
    }
};
```

## Phase-Specific Patterns

### Phase 2: Basic LIF Neurons
```cpp
// Phase 2: Focus on neuron dynamics and basic plasticity
void phase2_experiment(nlm::Brain& brain) {
    // Configure basic plasticity
    auto* stdp = brain.getSTDP();
    if (stdp) {
        stdp->configure(0.01f, 0.012f, 20.0f);
    }
    
    // Test spike generation
    std::cout << "Phase 2: Testing LIF neuron dynamics" << std::endl;
    std::cout << "Total spikes generated: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
}
```

### Phase 3: World Interaction
```cpp
// Phase 3: Sensory-motor loop with reward
void phase3_experiment(nlm::Brain& brain) {
    // Reset for clean experiment
    brain.reset();
    
    // Run simulation with environment interaction
    for (size_t step = 0; step < 1000; ++step) {
        // Process environment
        auto reward = process_environment(brain);
        
        // Apply reward as neuromodulation
        nlm::DopamineSignal dopamine_reward(reward);
        brain.applyNeuromodulation(dopamine_reward);
        
        // Step brain
        brain.step(step, step * 0.001);
        
        // Produce action
        auto action = brain.produceAction();
        
        // Record reward history
        record_reward(reward);
    }
}
```

### Phase 4: Cognitive Abilities
```cpp
// Phase 4: Attention, planning, and concept formation
void phase4_experiment(nlm::Brain& brain) {
    // Configure cognition systems
    auto* attention = brain.getAttention();
    auto* planner = brain.getPlanner();
    auto* concepts = brain.getConceptFormation();
    
    if (attention) {
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    if (planner) {
        planner->setPlanningDepth(3);
    }
    
    if (concepts) {
        concepts->setConceptLearningRate(0.1f);
    }
    
    // Run cognitive experiment
    std::cout << "Phase 4: Testing cognitive abilities" << std::endl;
    brain.logStatus();
}
```

### Phase 6: Integration
```cpp
// Phase 6: Complete integrated brain
void phase6_experiment(nlm::Brain& brain) {
    // Full integration experiment
    brain.reset();
    
    // Monitor all integrated systems
    for (size_t step = 0; step < 5000; ++step) {
        // Process sensory input
        brain.receiveSensoryInput(current_sensors);
        
        // Step complete brain loop
        brain.step(step, step * 0.001);
        
        // Monitor memory systems
        monitor_all_mandatory_systems(brain);
        
        // Check developmental progress
        if (step % 1000 == 0) {
            brain.develop();
        }
        
        // Periodic checkpoint for long experiment
        if (step % 1000 == 0) {
            brain.save(f"phase6_checkpoint_{step}.nlm");
        }
    }
    
    std::cout << "Phase 6 integration complete" << std::endl;
    monitor_brain(brain);
}
```

## Command Line Interface

### Common NLM CLI Commands
```bash
# Run Phase 6 demo (integration test)
./nlm_phase6_demo

# Run specific experiment
./nlm --experiment phase3_reward_learning --steps 10000

# Create and run custom experiment
./nlm --create-experiment my_experiment --type cognitive --duration 3600

# Check available experiments
./nlm --list-experiments

# Monitor running experiment
./nlm --monitor --log-level info --output-file experiment.log

# Restore from checkpoint
./nlm --load previous_brain_state.nlm --continue

# Help and documentation
./nlm --help
./nlm --show-config
./nlm --documentation-api
```

## Common Troubleshooting

### Common Issues and Solutions

#### Issue: Brain initialization fails
**Cause:** Invalid configuration parameters
**Solution:** Check configuration values and ensure they are within valid ranges

#### Issue: Simulation becomes unstable
**Cause:** Time step too large for numerical stability
**Solution:** Reduce simulation_timestep (try 0.0005)

#### Issue: Memory usage too high
**Cause:** Too many neurons or memory entries
**Solution:** Reduce neuron_count or max_episodes

#### Issue: No action produced
**Cause:** All motor neurons inactive
**Solution:** Check motor neuron inputs and action selection logic

#### Issue: Checkpoint save fails
**Cause:** Directory not writable or disk full
**Solution:** Check permissions and disk space

### Debugging Commands
```bash
# Enable debug logging
./nlm --log-level debug --log-file debug.log

# Check system resources
./nlm --monitor --resource-usage

# Profile performance
./nlm --profile --profile-output perf.txt

# Validate configuration
./nlm --validate-config

# Check for memory leaks
./nlm --memory-debug --heap-profile
```

## See Also

- [API Reference](API_REFERENCE.md)
- [Configuration Reference](CONFIG_REFERENCE.md)
- [Tutorial Documentation](TUTORIALS/README.md)
- [Examples](EXAMPLES/README.md)
- [FAQ](FAQ.md)
- [Implementation Notes](IMPLEMENTATION_NOTES.md)
- [Coding Guidelines](CODING_GUIDELINES.md)
