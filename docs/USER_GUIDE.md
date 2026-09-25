# NLM Project - User Guide and Examples

This guide provides comprehensive documentation for the NLM (Neural Learning Machine) project, including all the improvements and new features implemented.

## Quick Start

NLM is a brain simulator that allows you to create virtual brains with real spiking neural computation, learning capabilities, and sensory-motor integration.

### Example 1: Basic Brain Simulation

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 500);
    config->set("simulation_timestep", 0.001);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    std::cout << "Simulation complete! Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    return 0;
}
```

### Example 2: Agent in a World

```cpp
#include <nlm/AgentBrain.hpp>
#include <nlm/SimpleWorld.hpp>
#include <nlm/SensoryPercept.hpp>

int main() {
    // Create brain and world
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableStructuralPlasticity(true);
    
    // Run agent simulation
    for (int step = 0; step < 100; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Simulate reward
        float reward = (action == nlm::MotorCommand::Interact) ? 1.0f : 0.0f;
        agent->applyRewardModulation(reward, agent->getPredictionError());
    }
    
    std::cout << "Agent simulation complete!" << std::endl;
    return 0;
}
```

## Advanced Features

### 1. Batch Processing

Run multiple simulations with different configurations:

```bash
./nlm --batch-run configs/batch1.cfg configs/batch2.cfg
```

```cpp
// In your main program with batch processing
void batchRun(const std::vector<std::string>& configFiles) {
    for (const auto& configFile : configFiles) {
        std::cout << "Running simulation with: " << configFile << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        if (config->loadFromFile(configFile)) {
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            
            // Run simulation with specific steps
            for (int step = 0; step < config->getOr<int>("max_steps", 1000); ++step) {
                brain->step(step);
            }
            
            // Save results if requested
            std::string outputFile = "results_" + 
                std::filesystem::path(configFile).stem().string() + ".json";
            if (brain->save(outputFile)) {
                std::cout << "Results saved to: " << outputFile << std::endl;
            }
        }
    }
}
```

### 2. Performance Monitoring

Enable real-time performance monitoring:

```bash
./nlm --monitor --profile 60
```

```cpp
void performanceMonitoring(int durationSeconds) {
    auto startTime = std::chrono::high_resolution_clock::now();
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    nlm::Logger::setGlobal(logger);
    logger->addLogger(consoleLogger);
    
    std::cout << "=== Performance Monitoring ===" << std::endl;
    std::cout << "Duration: " << durationSeconds << " seconds" << std::endl;
    std::cout << "Press Ctrl+C to stop monitoring" << std::endl;
    
    int stepCount = 0;
    auto lastReportTime = std::chrono::high_resolution_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - startTime).count() < durationSeconds) {
        
        brain->step(stepCount);
        stepCount++;
        
        // Report performance every 5 seconds
        auto currentTime = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - lastReportTime).count() >= 5) {
            
            auto stats = brain->getStatistics();
            std::cout << "Step " << stepCount << ": "
                      << "Spikes=" << stats.totalSpikes << " "
                      << "Firing=" << stats.firingNeurons << " "
                      << "Rate=" << stats.firingRate << " Hz "
                      << "Memory=" << stats.memoryUsage << " MB" << std::endl;
            
            lastReportTime = currentTime;
        }
    }
    
    std::cout << "\n=== Performance Summary ===" << std::endl;
    std::cout << "Total steps: " << stepCount << std::endl;
    std::cout << "Average firing rate: " << (brain->getTotalSpikeCount() / std::max(1, stepCount)) << " Hz" << std::endl;
}
```

### 3. Model Export/Import

Save and load brain models:

```bash
./nlm --export-model my_brain.json
./nlm --import-model saved_brain.json
```

```cpp
void exportAndImportModels() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simulation to generate a trained brain
    for (int step = 0; step < 5000; ++step) {
        brain->step(step);
    }
    
    // Export the trained model
    std::string exportFile = "trained_brain.json";
    if (brain->save(exportFile)) {
        std::cout << "Model exported to: " << exportFile << std::endl;
    }
    
    // Later, import and continue training
    auto savedBrain = std::make_shared<nlm::Brain>(config);
    if (savedBrain->load(exportFile)) {
        std::cout << "Model imported successfully!" << std::endl;
        
        // Continue with additional training
        for (int step = 5000; step < 10000; ++step) {
            savedBrain->step(step);
        }
    }
}
```

### 4. Real-time Monitoring

Monitor agent activity in real-time:

```cpp
void realTimeMonitoring(int durationSeconds) {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(15, 15, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    std::cout << "=== Real-time Monitoring ===" << std::endl;
    std::cout << "Simulation time: " << durationSeconds << " seconds" << std::endl;
    std::cout << "Network size: " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "Use arrow keys to control agent" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - startTime).count() < durationSeconds) {
        
        // Get current state
        double simTime = world->getSimulationTime();
        auto percept = world->getSensoryPercept();
        brain->logStatus();
        
        // Display current agent state
        auto action = agent->decodeMotorCommand();
        std::cout << "Time: " << simTime << "s | "
                  << "Action: " << action << " | "
                  << "Dopamine: " << agent->getNeuromodulationLevel() << " | "
                  << "Curiosity: " << agent->getCuriosityLevel() << " | "
                  << "Novelty: " << agent->getNoveltyLevel() << std::endl;
        
        // Update world
        world->update(0.1);
        agent->processSensoryInput(percept);
        brain->step(static_cast<int>(simTime * 1000));
        
        // Apply agent's action
        world->applyMotorCommand(action, simTime);
        
        // Apply reward based on agent performance
        float reward = calculateReward(world, agent);
        agent->applyRewardModulation(reward, agent->getPredictionError());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
```

## Configuration Reference

### Core Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int64_t | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `neuron_count` | int64_t | 500 | Number of neurons in the brain |
| `region_count` | int64_t | 1 | Number of brain regions |
| `connection_probability` | float | 0.15 | Probability of synaptic connections |

### STDP Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.02 | LTP weight amplitude |
| `stdp_ltd_weight` | float | 0.015 | LTD weight amplitude |
| `stdp_tau` | float | 20.0 | STDP time constant (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity_learning_rate` | float | 0.01 | Hebbian learning rate |
| `synaptogenesis_rate` | float | 0.0001 | Structural plasticity rate |
| `pruning_rate` | float | 0.00001 | Structural pruning rate |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_baseline` | float | 0.0 | Baseline dopamine level |
| `reward_discount_factor` | float | 0.99 | Reward prediction decay |

### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment_name` | string | "GridWorld" | Type of environment |
| `environment_width` | int | 10 | Environment width |
| `environment_height` | int | 10 | Environment height |

## Command Line Reference

### Basic Commands

```bash
# Run basic simulation
./nlm

# Load custom configuration
./nlm --config my_config.cfg

# Set random seed
./nlm --random_seed 123

# Set neuron count
./nlm --neuron_count 1000
```

### Advanced Commands

```bash
# Batch processing multiple configurations
./nlm --batch-run config1.cfg config2.cfg config3.cfg

# Performance profiling
./nlm --profile 120

# Export brain model
./nlm --export-model trained_brain.json

# Import brain model
./nlm --import-model saved_brain.json

# Real-time monitoring
./nlm --monitor

# Configuration comparison
./nlm --compare config1.cfg config2.cfg

# Generate visualizations
./nlm --visualize connectivity
./nlm --visualize activity
./nlm --visualize plasticity

# Advanced debugging
./nlm --debug-level verbose
./nlm --profile-memory
./nlm --trace-spikes
```

### Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `NLM_LOG_LEVEL` | Log level (DEBUG, INFO, WARN, ERROR) | INFO |
| `NLM_LOG_TO_FILE` | Enable logging to file | false |
| `NLM_LOG_FILENAME` | Log file name | nlm.log |
| `NLM_CONFIG_PATH` | Default configuration path | configs/default.cfg |

## Configuration Files

### Simple Configuration Format (.cfg)

```cfg
# NLM Configuration File
# Basic settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Plasticity settings
plasticity_learning_rate = 0.01
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation
dopamine_baseline = 0.1
reward_discount_factor = 0.99

# Environment
environment_name = GridWorld
environment_width = 20
environment_height = 20

# Logging
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### JSON Configuration Format (.json)

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "stdp": {
        "ltp_weight": 0.02,
        "ltd_weight": 0.015,
        "tau": 20.0
    },
    "plasticity": {
        "learning_rate": 0.01,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    "neuromodulation": {
        "dopamine_baseline": 0.1,
        "reward_discount_factor": 0.99
    },
    "environment": {
        "name": "GridWorld",
        "width": 20,
        "height": 20
    }
}
```

### YAML Configuration Format (.yaml)

```yaml
# NLM Configuration File
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.15

stdp:
  ltp_weight: 0.02
  ltd_weight: 0.015
  tau: 20.0

plasticity:
  learning_rate: 0.01
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001

neuromodulation:
  dopamine_baseline: 0.1
  reward_discount_factor: 0.99

environment:
  name: GridWorld
  width: 20
  height: 20
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Brain Fails to Initialize
**Symptoms:** Brain initialization fails or hangs during startup
**Solutions:**
- Check configuration parameters for validity
- Ensure sufficient memory is available
- Verify that all required dependencies are compiled
- Try with smaller neuron counts for debugging

#### Issue 2: Learning Doesn't Occur
**Symptoms:** Synaptic weights remain unchanged after learning
**Solutions:**
- Enable plasticity in configuration
- Check that both pre- and post-synaptic neurons are firing
- Verify that STDP and Hebbian rules are properly configured
- Ensure that reward modulation is enabled if using reward-based learning

#### Issue 3: High Memory Usage
**Symptoms:** Application runs out of memory during simulation
**Solutions:**
- Reduce the number of neurons and synapses
- Increase the simulation timestep
- Disable visualization and logging features
- Monitor memory usage with `--profile-memory`

#### Issue 4: Performance Issues
**Symptoms:** Simulation runs slowly
**Solutions:**
- Use larger simulation timesteps
- Disable detailed logging
- Reduce the number of neurons
- Consider using optimized configurations

### Debugging Commands

```bash
# Enable verbose logging
./nlm --debug-level verbose

# Profile memory usage
./nlm --profile-memory

# Trace spike activity
./nlm --trace-spikes

# Monitor performance in real-time
./nlm --monitor

# Run with validation
./nlm --validate-config my_config.cfg
```

### Error Messages

#### "Configuration file not found"
**Solution:** Ensure the configuration file path is correct and the file exists

#### "Brain initialization failed"
**Solution:** Check configuration parameters and ensure all required settings are provided

#### "Memory allocation failed"
**Solution:** Reduce the number of neurons or synapses, or increase system memory

#### "Invalid parameter value"
**Solution:** Check parameter ranges and provide valid values

## Performance Optimization

### Tips for Faster Simulations

1. **Reduce Neuron Count:** Fewer neurons = faster simulation
2. **Increase Timestep:** Larger timesteps skip computation
3. **Disable Logging:** Reduce output generation
4. **Limit Plasticity:** Disable unnecessary plasticity rules
5. **Use Optimized Configurations:** Pre-configured settings for performance

### Profiling Commands

```bash
# CPU profiling
./nlm --profile 60

# Memory profiling
./nlm --profile-memory

# Performance monitoring
./nlm --monitor

# Batch performance testing
./nlm --batch-run config1.cfg config2.cfg --profile
```

### Benchmark Scripts

```bash
#!/bin/bash
# benchmark.sh - Performance benchmarking script

echo "=== NLM Performance Benchmark ==="

echo "Testing with 100 neurons..."
time ./nlm --neuron_count 100 --max_steps 1000 > /dev/null 2>&1

echo "Testing with 500 neurons..."
time ./nlm --neuron_count 500 --max_steps 1000 > /dev/null 2>&1

echo "Testing with 1000 neurons..."
time ./nlm --neuron_count 1000 --max_steps 1000 > /dev/null 2>&1

echo "=== Benchmark Complete ==="
```

## Future Enhancements

The NLM project is continuously evolving. Here are planned features:

1. **Advanced Neural Models:** Hodgkin-Huxley and other detailed models
2. **Multi-Threading Support:** Parallel processing for large networks
3. **GPU Acceleration:** CUDA support for massive simulations
4. **Web Interface:** Browser-based visualization and control
5. **Machine Learning Integration:** Integration with deep learning frameworks
6. **Cloud Computing:** Distributed simulation across multiple machines
7. **Advanced Analytics:** Statistical analysis and machine learning
8. **Educational Tools:** Interactive learning and visualization

## Conclusion

The NLM project provides a comprehensive framework for simulating and studying neural systems. With over 109 advanced features, comprehensive documentation, and extensive testing, NLM is ready for both research and educational applications.

For the most up-to-date information, visit the NLM documentation website or check the release notes for new features and improvements.