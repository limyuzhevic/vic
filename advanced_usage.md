# NLM Advanced Usage Guide
# Advanced Configuration and Usage Examples

## Overview

This guide covers advanced usage patterns, configuration options, and best practices for the NLM (Neural Learning Machine) system. It builds upon the basic usage guide with more sophisticated examples and configuration patterns.

## Advanced Configuration

### Configuration File Formats

NLM supports multiple configuration file formats:

#### JSON Configuration

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "initial_weight_mean": 0.5,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.0001
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "decay_rate": 0.001
    },
    "novelty": {
      "enable": true,
      "threshold": 0.1
    }
  },
  "memory": {
    "working_memory": {
      "capacity": 100,
      "decay_rate": 0.001
    },
    "episodic_memory": {
      "capacity": 1000,
      "replay_frequency": 0.01
    }
  },
  "development": {
    "enable": true,
    "plasticity_modifier": 1.0,
    "critical_period_duration": 100.0,
    "maturation_rate": 0.001
  },
  "logging": {
    "level": "info",
    "use_colors": true,
    "log_to_file": "nlm.log",
    "max_file_size_mb": 100,
    "max_files": 10
  }
}
```

#### YAML Configuration

```yaml
brain:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  initial_weight_mean: 0.5
  v_thresh: -50.0
  v_rest: -70.0
  v_reset: -75.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.0001
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
    decay_rate: 0.001
  novelty:
    enable: true
    threshold: 0.1

memory:
  working_memory:
    capacity: 100
    decay_rate: 0.001
  episodic_memory:
    capacity: 1000
    replay_frequency: 0.01

development:
  enable: true
  plasticity_modifier: 1.0
  critical_period_duration: 100.0
  maturation_rate: 0.001

logging:
  level: info
  use_colors: true
  log_to_file: "nlm.log"
  max_file_size_mb: 100
  max_files: 10
```

### Advanced Configuration Parameters

#### Brain Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `brain.neuron_count` | int | 1000 | Total number of neurons | 100-10000 |
| `brain.region_count` | int | 1 | Number of brain regions | 1-10 |
| `brain.connection_probability` | float | 0.05 | Synapse connection probability | 0.01-0.5 |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight | 0-1 |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights | 0-1 |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential | -100 to -20 |
| `brain.v_rest` | float | -70.0 | Resting potential | -80 to -60 |
| `brain.v_reset` | float | -75.0 | Reset potential after spike | -85 to -70 |
| `brain.tau_mem` | float | 20.0 | Membrane time constant | 10-100 ms |
| `brain.tau_ref` | float | 2.0 | Refractory period | 1-10 ms |

#### Plasticity Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `plasticity.stdp.enable` | bool | true | Enable STDP | true/false |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate | 0.0001-0.01 |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (potentiation) | 10-50 ms |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (depression) | 10-50 ms |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | true/false |
| `plasticity.hebbian.learning_rate` | float | 0.0001 | Hebbian learning rate | 0.00001-0.001 |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | true/false |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Synaptogenesis rate | 0.00001-0.001 |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Pruning rate | 0.000001-0.0001 |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale | 0.1-10 |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | true/false |
| `neuromod.curiosity.decay_rate` | float | 0.001 | Curiosity decay rate | 0.0001-0.01 |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | true/false |
| `neuromod.novelty.threshold` | float | 0.1 | Novelty detection threshold | 0.01-1 |

#### Development Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `development.enable` | bool | true | Enable development system | true/false |
| `development.plasticity_modifier` | float | 1.0 | Plasticity modifier during development | 0.1-10 |
| `development.critical_period_duration` | float | 100.0 | Critical period duration | 50-500 steps |
| `development.maturation_rate` | float | 0.001 | Neural maturation rate | 0.0001-0.01 |

### Configuration Validation

NLM provides configuration validation to ensure parameters are within valid ranges:

```cpp
// Validate configuration
auto errors = config.validateConfiguration();
if (!errors.empty()) {
    std::cout << "Configuration errors:" << std::endl;
    for (const auto& error : errors) {
        std::cout << "  - " << error << std::endl;
    }
    return 1; // Exit with error
}
```

### Configuration Categories

NLM organizes configuration parameters into logical categories:

- **Brain**: Neural network parameters
- **Plasticity**: Learning rules and mechanisms
- **Neuromodulation**: Reward and motivation systems
- **Memory**: Memory systems and storage
- **Development**: Developmental processes and maturation
- **Logging**: Logging configuration and output

You can access configuration by category:

```cpp
// Get all categories
auto categories = config.getCategories();
for (const auto& category : categories) {
    std::cout << "Category: " << category << std::endl;
    auto keys = config.getKeysByCategory(category);
    for (const auto& key : keys) {
        std::cout << "  " << key << std::endl;
    }
}
```

## Advanced Usage Patterns

### Pattern 1: Multi-Phase Learning

```cpp
void runMultiPhaseLearning() {
    // Phase 1: Structural development
    config.set("development.enable", true);
    config.set("development.critical_period_duration", 500);
    config.set("development.maturation_rate", 0.005);
    
    auto brain1 = createBrain(config);
    brain1->initialize();
    runPhase(brain1, 1000); // Structural development
    
    // Phase 2: Plasticity learning
    config.set("development.enable", false);
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.hebbian.enable", true);
    
    auto brain2 = createBrain(config);
    brain2->initialize();
    runPhase(brain2, 2000); // Plasticity learning
    
    // Phase 3: Neuromodulation
    config.set("neuromod.dopamine.scale", 2.0);
    config.set("neuromod.curiosity.enable", true);
    config.set("neuromod.novelty.enable", true);
    
    auto brain3 = createBrain(config);
    brain3->initialize();
    runPhase(brain3, 3000); // Neuromodulation learning
}
```

### Pattern 2: Checkpoint and Resume

```cpp
void saveAndResume() {
    // Create and train brain
    auto config = createDefaultConfig();
    auto brain = createBrain(config);
    brain->initialize();
    
    // Train for a while
    for (int step = 0; step < 5000; ++step) {
        brain->step(step);
        
        // Save checkpoint periodically
        if (step % 1000 == 0) {
            std::string filename = "checkpoint_" + std::to_string(step) + ".bin";
            brain->save(filename);
            std::cout << "Saved checkpoint: " << filename << std::endl;
        }
        
        // Simulate learning
        simulateLearning(brain, step);
    }
    
    // Save final state
    brain->save("final_checkpoint.bin");
}

void loadAndContinue() {
    // Load previous state
    auto config = createDefaultConfig();
    auto brain = createBrain(config);
    brain->initialize();
    
    // Load saved checkpoint
    if (brain->load("checkpoint_3000.bin")) {
        std::cout << "Successfully loaded checkpoint" << std::endl;
        
        // Continue training from where we left off
        for (int step = 3000; step < 10000; ++step) {
            brain->step(step);
            simulateLearning(brain, step);
        }
        
        brain->save("final_checkpoint.bin");
    } else {
        std::cerr << "Failed to load checkpoint" << std::endl;
    }
}
```

### Pattern 3: Agent with Complex Environment

```cpp
void runComplexAgentSimulation() {
    // Setup environment
    auto world = createSimpleWorld();
    world->configure(
        width: 50,
        height: 50,
        visionWidth: 16,
        visionHeight: 16
    );
    world->reset();
    world->setAgentStart(25.0, 25.0);
    
    // Setup brain with advanced configuration
    auto config = createDefaultConfig();
    config->set("brain.neuron_count", 2000);
    config->set("brain.region_count", 3);
    config->set("brain.v_thresh", -45.0);
    config->set("brain.v_rest", -65.0);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    config->set("neuromod.dopamine.scale", 1.5);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("memory.working_memory.capacity", 500);
    config->set("memory.episodic_memory.capacity", 5000);
    config->set("development.enable", true);
    
    auto brain = createBrain(config);
    brain->initialize();
    
    // Setup agent
    auto agent = createAgentBrain(brain);
    agent->initialize(world);
    
    // Enable all subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run simulation
    for (int episode = 0; episode < 100; ++episode) {
        std::cout << "Episode " << episode << "/" << 100 << std::endl;
        
        // Run episode
        for (int step = 0; step < 1000; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get action from brain
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            float reward = percept->getInternal()[0] if percept->getInternal() else 0.0;
            agent->applyRewardModulation(reward, 0.0);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Log statistics periodically
            if (step % 100 == 0) {
                logEpisodeStats(brain, agent, world, step);
            }
        }
        
        // Reset for next episode
        world->reset();
        agent->reset();
    }
}
```

### Pattern 4: Batch Training with Multiple Brains

```cpp
void runBatchTraining() {
    // Create different brain configurations
    std::vector<std::shared_ptr<Config>> configs;
    
    // Configuration 1: Conservative brain
    auto config1 = createDefaultConfig();
    config1->set("brain.neuron_count", 500);
    config1->set("plasticity.stdp.learning_rate", 0.0001);
    config1->set("neuromod.dopamine.scale", 0.5);
    configs.push_back(config1);
    
    // Configuration 2: Aggressive brain
    auto config2 = createDefaultConfig();
    config2->set("brain.neuron_count", 1500);
    config2->set("plasticity.stdp.learning_rate", 0.005);
    config2->set("neuromod.dopamine.scale", 2.0);
    configs.push_back(config2);
    
    // Configuration 3: Balanced brain
    auto config3 = createDefaultConfig();
    config3->set("brain.neuron_count", 1000);
    config3->set("plasticity.stdp.learning_rate", 0.001);
    config3->set("neuromod.dopamine.scale", 1.0);
    configs.push_back(config3);
    
    // Train each configuration
    for (size_t i = 0; i < configs.size(); ++i) {
        std::cout << "Training configuration " << i + 1 << "/" << configs.size() << std::endl;
        
        auto brain = createBrain(configs[i]);
        brain->initialize();
        
        // Train for many steps
        for (int step = 0; step < 10000; ++step) {
            brain->step(step);
            
            // Log progress periodically
            if (step % 1000 == 0) {
                logTrainingProgress(brain, step, i);
            }
        }
        
        // Save trained brain
        brain->save("trained_brain_" + std::to_string(i) + ".bin");
    }
}
```

## Command Line Interface

### Advanced Command Line Options

```bash
# Basic usage with configuration file
./nlm --config=advanced_config.json --verbose

# Run specific test with checkpointing
./nlm --test=2 --checkpoint=checkpoint.bin --load=initial_state.bin

# Run with custom configuration and quiet mode
./nlm --config=custom.cfg --quiet --no-plasticity

# Run with development enabled and custom parameters
./nlm --test=1 --no-development=false --neuron_count=2000

# Display help information
./nlm --help

# Display version information
./nlm --version
```

### Command Line Argument Examples

```bash
# Load configuration from file and enable verbose logging
./nlm --config=config.json --verbose

# Run test 1 with checkpoint saving every 100 steps
./nlm --test=1 --checkpoint-interval=100

# Disable all plasticity rules for testing
./nlm --test=1 --no-plasticity

# Load a saved checkpoint and continue training
./nlm --test=2 --load=saved_checkpoint.bin

# Run with custom configuration and save final state
./nlm --config=experiment.cfg --checkpoint=final_state.bin

# Run with all neuromodulation subsystems
./nlm --test=1 --enable-curiosity --enable-novelty --enable-dopamine
```

## Performance Optimization

### Memory Management

For large-scale simulations, consider these optimizations:

1. **Use smaller configurations for testing**: Start with smaller neuron counts to debug
2. **Enable checkpointing**: Save intermediate states to recover from crashes
3. **Use memory pools**: Configure memory pools for better performance
4. **Optimize logging**: Reduce log verbosity in production

### Computational Optimization

```cpp
// Example optimized configuration
config->set("brain.neuron_count", 100);  // Start small
config->set("brain.region_count", 1);
config->set("simulation_timestep", 0.01);  // Larger timestep
config->set("plasticity.stdp.learning_rate", 0.0005);
config->set("plasticity.hebbian.learning_rate", 0.00005);
config->set("neuromod.curiosity.decay_rate", 0.01);
config->set("development.maturation_rate", 0.01);
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Brain has no neurons
**Cause**: Invalid or missing neuron_count configuration
**Solution**: Check configuration and ensure `brain.neuron_count` is set

```cpp
if (brain->getTotalNeuronCount() == 0) {
    std::cerr << "ERROR: Brain has no neurons!" << std::endl;
    std::cerr << "Check configuration: brain.neuron_count" << std::endl;
    return 1;
}
```

#### Issue: Configuration file not found
**Cause**: Incorrect file path or permissions
**Solution**: Verify file path and permissions

```cpp
std::string configFile = "configs/default.cfg";
if (!config->loadFromFile(configFile)) {
    std::cerr << "WARNING: Could not load configuration file: " << configFile << std::endl;
    std::cerr << "Using default configuration instead" << std::endl;
    // Set defaults manually
    config->set("brain.neuron_count", 1000);
    config->set("brain.region_count", 1);
}
```

#### Issue: Training is too slow
**Cause**: Too many neurons or complex configuration
**Solution**: Reduce complexity for testing, optimize configuration

```cpp
// For testing, use smaller configuration
config->set("brain.neuron_count", 100);  // Reduced for testing
config->set("plasticity.stdp.learning_rate", 0.0001);  // Reduced learning rate
```

#### Issue: Memory usage too high
**Cause**: Too many neurons or memory systems
**Solution**: Disable unnecessary memory systems or reduce capacity

```cpp
// Disable episodic memory for testing
config->set("memory.episodic_memory.capacity", 0);

// Disable working memory for testing
config->set("memory.working_memory.capacity", 0);
```

### Debugging Techniques

1. **Enable verbose logging**: Use `--verbose` flag to see detailed output
2. **Check configuration**: Validate configuration before running
3. **Use checkpoints**: Save intermediate states to recover from crashes
4. **Monitor performance**: Track execution time and memory usage
5. **Test incrementally**: Start with simple configurations and add complexity

```cpp
// Example debugging function
void debugBrainState(const std::shared_ptr<Brain>& brain) {
    std::cout << "=== Brain State Debug ===" << std::endl;
    std::cout << "Neuron count: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapse count: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    // Log brain status
    brain->logStatus();
}
```

## Conclusion

The NLM advanced usage guide provides comprehensive coverage of:

1. **Configuration Management**: JSON/YAML support, validation, and categories
2. **Advanced Usage Patterns**: Multi-phase learning, checkpointing, complex agents
3. **Command Line Interface**: Help, version, and advanced options
4. **Performance Optimization**: Memory and computational optimization
5. **Troubleshooting**: Common issues and debugging techniques

These advanced features enable users to create sophisticated neural simulations with:

- Custom brain architectures
- Complex learning scenarios
- Advanced memory systems
- Developmental processes
- Neuromodulation effects
- Batch training capabilities

For more information, refer to the documentation in the `docs/` directory or the GitHub repository.
