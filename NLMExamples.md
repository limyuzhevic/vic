# NLM — Examples Documentation

This document provides comprehensive examples for the NLM codebase, covering basic usage, advanced scenarios, configuration examples, and troubleshooting.

---

## Basic Examples

### Example 1: Simple Neural Network

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Complete Agent Simulation

```python
import pynlm

def run_complete_simulation(num_steps=1000):
    """Run a complete simulation with brain, world, and agent."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run simulation
brain, agent, world = run_complete_simulation(1000)
```

### Example 3: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

---

## Advanced Examples

### Example 1: Configuration Management

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.08)
config.set("plasticity.stdp.learning_rate", 0.002)

# Save configuration to file
config.saveToFile("custom_config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("custom_config.json")

# Validate configuration
try:
    loaded_config.validate()
    print("Configuration is valid")
except Exception as e:
    print(f"Configuration error: {e}")
```

### Example 2: Error Handling

```cpp
#include "core/Error/ErrorHandling.hpp"
#include "core/Config/Config.hpp"

void example() {
    nlm::Config config;
    
    try {
        config.loadFromFile("config.json");
        config.validate();
    } catch (const nlm::ConfigError& e) {
        NLM_THROW_CONFIG("Failed to load configuration", e.getKey());
    }
    
    // Use NLM error handling macros
    NLM_CHECK_CONDITION(config.has("brain.neuron_count"), 
                      "Neuron count not specified", 
                      "brain.neuron_count", 
                      nlm::ValidationError::ErrorType::MissingRequired);
    
    int neuron_count = config.getInt("brain.neuron_count");
    NLM_CHECK_CONDITION(neuron_count > 0,
                      "Neuron count must be positive",
                      "brain.neuron_count",
                      nlm::ValidationError::ErrorType::InvalidRange);
}
```

### Example 3: Performance Optimization

```python
import pynlm

# Create brain with optimized configuration
config = pynlm.createDefaultConfig()

# Enable performance optimizations
brain = pynlm.createBrain(config)
brain.initialize()

# Enable SIMD vectorization
brain.enableSIMD(True)

# Enable parallel processing
brain.enableParallelProcessing(True)

# Enable memory pools
brain.enableMemoryPools(True)

# Enable event queues
brain.enableEventQueues(True)

# Run optimized simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Example 4: Memory Management

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for a few steps
for i in range(100):
    brain.step(i)

# Save checkpoint
brain.save("checkpoint_step_100.bin")

# Continue simulation from checkpoint
brain.load("checkpoint_step_100.bin")
for i in range(100, 200):
    brain.step(i)

# Monitor memory usage
print(f"Memory usage: {brain.getMemoryUsage()} bytes")
print(f"Average step time: {brain.getAverageStepTime()} ms")
```

### Example 5: Learning Features

```python
import pynlm

def run_learning_simulation(num_steps=1000):
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print learning statistics
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, agent, world

# Run learning simulation
brain, agent, world = run_learning_simulation(1000)
```

---

## Configuration Examples

### Example 1: Basic Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with basic configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Example 2: Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.structural.enable", true)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.novelty.enable", true)

# Save custom configuration
config.saveToFile("custom_config.json")

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Example 3: Preset Configuration

```bash
# Use basic preset
./nlm --preset=basic

# Use advanced preset
./nlm --preset=advanced

# Use debug preset
./nlm --preset=debug

# Use production preset
./nlm --preset=production
```

### Example 4: Template Configuration

```bash
# Use default template with overrides
./nlm --template=default --override="neuron_count=1000,--plasticity_learning_rate=0.05"

# Use debug template
./nlm --template=debug --override="neuron_count=100,--log_level=DEBUG"

# Use simple template
./nlm --template=simple --override="neuron_count=500"
```

---

## Error Handling Examples

### Example 1: Configuration Error Handling

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Save configuration
try:
    config.saveToFile("config.json")
    print("Configuration saved successfully")
except Exception as e:
    print(f"Error saving configuration: {e}")
```

### Example 2: Validation Error Handling

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Add invalid configuration
config.set("brain.neuron_count", -1)

# Validate configuration
try:
    config.validate()
    print("Configuration is valid")
except Exception as e:
    print(f"Configuration validation error: {e}")
```

### Example 3: File Error Handling

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Try to load non-existent configuration file
try:
    config.loadFromFile("non_existent_file.json")
    print("Configuration loaded successfully")
except Exception as e:
    print(f"Error loading configuration: {e}")
```

---

## Performance Examples

### Example 1: Optimized Simulation

```python
import pynlm

# Create optimized configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Enable performance optimizations
brain.enableSIMD(True)
brain.enableParallelProcessing(True)
brain.enableMemoryPools(True)
brain.enableEventQueues(True)

# Run optimized simulation
import time
start_time = time.time()

for step in range(1000):
    brain.step(step)

end_time = time.time()
elapsed_time = end_time - start_time

print(f"Simulation completed in {elapsed_time:.2f} seconds")
print(f"Average step time: {elapsed_time/1000*1000:.2f} ms")
```

### Example 2: Memory Management

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with checkpointing
checkpoint_interval = 100

for step in range(0, 1000, checkpoint_interval):
    for i in range(checkpoint_interval):
        brain.step(step + i)
    
    # Save checkpoint
    checkpoint_file = f"checkpoint_step_{step + checkpoint_interval}.bin"
    brain.save(checkpoint_file)
    print(f"Saved checkpoint: {checkpoint_file}")
    
    # Monitor memory usage
    memory_usage = brain.getMemoryUsage()
    print(f"Memory usage: {memory_usage} bytes")
```

---

## Command Line Examples

### Example 1: Basic Commands

```bash
# Display help
./nlm --help
./nlm -h

# Run Phase 6 experiments
./nlm --run-phase-6
./nlm -p6

# Validate configuration
./nlm --validate-config
./nlm -v

# Export configuration
./nlm --export-config --format=json --file=exported_config.json

# Show configuration information
./nlm --info
./nlm -i

# List presets
./nlm --list-presets
./nlm -lp

# List templates
./nlm --list-templates
./nlm -lt
```

### Example 2: Configuration Commands

```bash
# Load configuration from file
./nlm --config=config.json
./nlm --config=config.cfg

# Load configuration preset
./nlm --preset=basic
./nlm --preset=advanced

# Load configuration template
./nlm --template=default --override="neuron_count=1000"

# Load configuration schema
./nlm --schema=custom_schema.json

# Disable environment variables
./nlm --no-env
```

### Example 3: Environment Variables

```bash
# Set environment variables
export NLM_brain_neuron_count=1000
export NLM_brain_synapse_density=0.1
export NLM_log_level=DEBUG

# Run with environment variables
./nlm --preset=basic

# Override with command line
./nlm --config=basic.cfg --brain_neuron_count=500
```

---

## References

1. **NLMUserGuide.md** - User guide and tutorials
2. **NLMApiReference.md** - API documentation
3. **NLMConfigurationReference.md** - Configuration documentation
4. **NLMMigrationGuide.md** - Migration guide
5. **NLMBestPractices.md** - Best practices
6. **NLMTroubleshooting.md** - Troubleshooting guide

---

*Last Updated: Phase 6 Completion*
*Version: 1.0.0*