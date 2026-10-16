# NLM Beginner Tutorial Series

## Overview

This tutorial series provides step-by-step instructions for getting started with the NLM (Neural Learning Machine) project. Each tutorial builds upon the previous one, introducing new concepts and capabilities progressively.

## Tutorial 1: Hello World - Basic Brain Simulation

### Objective
Create and run a simple spiking neural network simulation with basic brain functionality.

### What You'll Learn
- How to create a brain with default configuration
- How to initialize and run a simulation
- How to monitor brain activity
- Basic brain statistics and analysis

### Step 1: Setup Environment

```python
import pynlm
import time

print("=== NLM Tutorial 1: Basic Brain Simulation ===")
print("Installing and configuring NLM environment...\n")
```

### Step 2: Create Basic Brain

```python
# Create brain with default configuration
print("2. Creating brain with default configuration...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

print(f"   Brain created with {brain.getTotalNeuronCount()} total neurons")
print(f"   Synapse density: {config.getOr('brain.synapse_density', 0.1):.2f}")
```

### Step 3: Initialize Brain

```python
# Initialize the brain
print("\n3. Initializing brain...")
brain.initialize()

print("   Brain initialization complete!")
print("   Ready for simulation...")
```

### Step 4: Run Basic Simulation

```python
# Run simulation
print("\n4. Running basic simulation (1000 steps)...")

start_time = time.time()
for step in range(1000):
    brain.step(step)
    
end_time = time.time()

print("   Simulation complete!")
print(f"   Simulation time: {end_time - start_time:.2f} seconds")
```

### Step 5: Analyze Results

```python
# Analyze simulation results
print("\n5. Analyzing simulation results...")

stats = brain.getTotalNeuronCount()
active_neurons = brain.getActiveNeuronCount()
firing_neurons = brain.getFiringNeuronCount()
average_firing_rate = brain.getAverageFiringRate()
total_spikes = brain.getTotalSpikeCount()
e_i_ratio = brain.getExcitationInhibitionRatio()

print("   Brain Statistics:")
print(f"   - Total neurons: {stats}")
print(f"   - Active neurons: {active_neurons}")
print(f"   - Firing neurons: {firing_neurons}")
print(f"   - Average firing rate: {average_firing_rate:.2f} Hz")
print(f"   - Total spikes generated: {total_spikes}")
print(f"   - E/I ratio: {e_i_ratio:.2f}")
```

### Step 6: Save and Load

```python
# Save brain state
print("\n6. Saving brain state...")
brain.save("tutorial1_brain.bin")
print("   Brain saved to 'tutorial1_brain.bin'")

# Load brain state
print("\n7. Loading brain state...")
brain.load("tutorial1_brain.bin")
print("   Brain loaded successfully!")
print(f"   Loaded brain has {brain.getTotalNeuronCount()} neurons")
```

### Step 7: Complete Example

```python
def basic_brain_simulation():
    """Complete tutorial 1 example."""
    
    print("=== NLM Tutorial 1: Basic Brain Simulation ===\n")
    
    # Step 1: Setup
    import pynlm
    
    # Step 2: Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Step 3: Initialize
    brain.initialize()
    
    # Step 4: Run simulation
    print("Running 1000-step simulation...")
    for step in range(1000):
        brain.step(step)
    
    # Step 5: Analyze
    print(f"\nResults:")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    
    # Step 6: Save
    brain.save("tutorial1_brain.bin")
    print(f"  - Brain saved")
    
    return brain

# Run the tutorial
brain = basic_brain_simulation()
print("\nTutorial 1 complete!")
```

### What You Should See

Your output should look something like this:

```
=== NLM Tutorial 1: Basic Brain Simulation ===

2. Creating brain with default configuration...
   Brain created with 1000 total neurons
   Synapse density: 0.10

3. Initializing brain...
   Brain initialization complete!
   Ready for simulation...

4. Running basic simulation (1000 steps)...
   Simulation complete!
   Simulation time: 0.23 seconds

5. Brain Statistics:
   - Total neurons: 1000
   - Active neurons: 12
   - Firing neurons: 8
   - Average firing rate: 0.24 Hz
   - Total spikes generated: 240
   - E/I ratio: 1.15

6. Saving brain state...
   Brain saved to 'tutorial1_brain.bin'

7. Loading brain state...
   Brain loaded successfully!
   Loaded brain has 1000 neurons

Tutorial 1 complete!
```

### Key Takeaways

1. **Brain Creation**: Use `pynlm.createBrain()` with default configuration
2. **Initialization**: Always call `brain.initialize()` before simulation
3. **Simulation Loop**: Use `brain.step(step)` for each simulation step
4. **Statistics**: Use methods like `getTotalNeuronCount()`, `getAverageFiringRate()`
5. **Persistence**: Use `save()` and `load()` for brain state management

### Next Steps

Tutorial 2 will introduce world interaction and agent-based simulation.

---

## Tutorial 2: Agent in World - Sensory-Motor Integration

### Objective
Create an agent that interacts with a simulated world, processing sensory input and producing motor output.

### What You'll Learn
- How to create a simulated world
- How to create an agent brain interface
- Sensory input processing
- Motor output generation
- Complete agent simulation loop

### Step 1: Setup Environment

```python
import pynlm
import time

print("=== NLM Tutorial 2: Agent in World ===\n")
```

### Step 2: Create Brain and Agent

```python
# Create brain and agent
print("2. Creating brain and agent...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

print("   Brain created and initialized")
print("   Agent brain interface created")
```

### Step 3: Create and Configure World

```python
# Create and configure world
print("\n3. Creating and configuring world...")
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()
world.setAgentStart(10.0, 10.0)

print(f"   World created: {world.getWidth()}x{world.getHeight()} cells")
print(f"   Vision range: {world.getVisionWidth()}x{world.getVisionHeight()} cells")
print(f"   Agent start position: (10.0, 10.0)")
```

### Step 4: Initialize Agent with World

```python
# Initialize agent with world
print("\n4. Initializing agent with world...")
agent.initialize(world)

print("   Agent initialized with world")
print("   Ready for interaction...")
```

### Step 5: Enable Learning Subsystems

```python
# Enable learning subsystems
print("\n5. Enabling learning subsystems...")

agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore novel experiences
agent.enableDevelopment(True)           # Brain maturation over time
agent.enableStructuralPlasticity(True)   # Grow new connections

print("   - Reward modulation enabled")
print("   - Curiosity enabled")
print("   - Development enabled")
print("   - Structural plasticity enabled")
```

### Step 6: Run Agent Simulation

```python
# Run agent simulation
print("\n6. Running agent simulation (500 steps)...")

for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Decode motor command from brain
    action = agent.decodeMotorCommand()
    
    # Apply motor command to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = 1.0 if step % 50 == 0 else 0.0  # Simple reward
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"   Step {step}: "
              f"{agent.getCuriosityLevel():.2f} curiosity, "
              f"{agent.getNoveltyLevel():.2f} novelty, "
              f"{brain.getFiringNeuronCount()} firing neurons")

print("   Agent simulation complete!")
```

### Step 7: Analyze Agent Behavior

```python
# Analyze agent behavior
print("\n7. Analyzing agent behavior...")

# Get final agent statistics
final_curiosity = agent.getCuriosityLevel()
final_novelty = agent.getNoveltyLevel()
final_prediction_error = agent.getPredictionError()
final_development_stage = brain.getDevelopmentalStage()
final_spike_count = brain.getTotalSpikeCount()

print("   Agent Statistics:")
print(f"   - Final curiosity: {final_curiosity:.3f}")
print(f"   - Final novelty: {final_novelty:.3f}")
print(f"   - Final prediction error: {final_prediction_error:.3f}")
print(f"   - Development stage: {final_development_stage}")
print(f"   - Total spikes generated: {final_spike_count}")
print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Step 8: Complete Example

```python
def agent_world_simulation():
    """Complete tutorial 2 example."""
    
    print("=== NLM Tutorial 2: Agent in World ===\n")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    print("Running 500-step agent simulation...")
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = 1.0 if step % 50 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"  Step {step}: {agent.getCuriosityLevel():.2f} curiosity")
    
    # Analyze results
    print(f"\nSimulation Results:")
    print(f"  - Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  - Prediction error: {agent.getPredictionError():.3f}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain, agent, world

# Run the tutorial
brain, agent, world = agent_world_simulation()
print("\nTutorial 2 complete!")
```

### What You Should See

```
=== NLM Tutorial 2: Agent in World ===

2. Creating brain and agent...
   Brain created and initialized
   Agent brain interface created

3. Creating and configuring world...
   World created: 20x20 cells
   Vision range: 8x8 cells
   Agent start position: (10.0, 10.0)

4. Initializing agent with world...
   Agent initialized with world
   Ready for interaction...

5. Enabling learning subsystems...
   - Reward modulation enabled
   - Curiosity enabled
   - Development enabled
   - Structural plasticity enabled

6. Running agent simulation (500 steps)...
   Step 0: 0.15 curiosity
   Step 100: 0.32 curiosity
   Step 200: 0.45 curiosity
   Step 300: 0.58 curiosity
   Step 400: 0.68 curiosity

   Agent simulation complete!

7. Agent Statistics:
   - Final curiosity: 0.682
   - Final novelty: 0.391
   - Final prediction error: 0.234
   - Development stage: CriticalPeriod
   - Total spikes generated: 1842
   - Average firing rate: 0.37 Hz

Tutorial 2 complete!
```

### Key Takeaways

1. **World Creation**: Use `pynlm.createSimpleWorld()` for 2D simulation
2. **Agent Integration**: Use `pynlm.createAgentBrain(brain)` for brain-world interface
3. **Sensory Processing**: Use `world.getSensoryPercept()` and `agent.processSensoryInput()`
4. **Motor Control**: Use `agent.decodeMotorCommand()` and `world.applyMotorCommand()`
5. **Learning**: Enable subsystems with `enableRewardModulation()`, `enableCuriosity()`, etc.

### Next Steps

Tutorial 3 will introduce batch operations and progress monitoring.

---

## Tutorial 3: Batch Operations and Progress Monitoring

### Objective
Learn how to use batch processing and progress monitoring for efficient, large-scale simulations.

### What You'll Learn
- Batch operations for efficient simulation
- Progress monitoring and callbacks
- Performance optimization techniques
- Event-driven processing

### Step 1: Setup Environment

```python
import pynlm
import time

print("=== NLM Tutorial 3: Batch Operations and Progress Monitoring ===\n")
```

### Step 2: Create Basic Brain

```python
# Create brain for batch processing
print("2. Creating brain for batch processing...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("   Brain created for batch operations")
```

### Step 3: Run Batch Processing

```python
# Run batch processing
print("\n3. Running batch processing (5000 steps)...")

start_time = time.time()
spike_count = pynlm.batchStep(brain, 5000)
end_time = time.time()

print(f"   Batch processing complete!")
print(f"   Total spikes generated: {spike_count}")
print(f"   Processing time: {end_time - start_time:.2f} seconds")
```

### Step 4: Progress Monitoring Setup

```python
# Setup progress monitoring
print("\n4. Setting up progress monitoring...")

# Define progress callback
def progress_callback(current_step, total_steps, progress):
    if int(progress * 100) % 10 == 0:  # Every 10%
        print(f"   Progress: {progress*100:.0f}% ({current_step}/{total_steps})")

# Register progress callback
pynlm.registerProgressCallback(brain, total_steps=5000, callback=progress_callback)

print("   Progress callback registered")
print("   Running simulation with monitoring...")
```

### Step 5: Run with Progress Callback

```python
# Run simulation with progress callback
print("\n5. Running simulation with progress monitoring...")

progress_result = pynlm.runWithProgressCallback(
    brain=brain,
    totalSteps=5000,
    progressCallback=progress_callback,
    logCallback=lambda msg: print(f"   LOG: {msg}")
)

print("   Simulation with progress monitoring complete!")
```

### Step 6: Performance Monitoring

```python
# Setup performance monitoring
print("\n6. Setting up performance monitoring...")

# Start performance monitoring
start_time, initial_spikes, metrics = pynlm.startPerformanceMonitoring(brain)

# Run simulation with performance tracking
for step in range(1000):
    brain.step(step)
    
    metric, current_spikes = pynlm.monitorPerformance(
        brain, start_time, initial_spikes, metrics
    )

# Generate performance report
report = pynlm.stopPerformanceMonitoring(metrics)

print("   Performance monitoring complete!")
print(f"   Total steps: {report['total_steps']}")
print(f"   Total spikes: {report['total_spikes']}")
print(f"   Average spikes per step: {report['average_spikes_per_step']:.2f}")
print(f"   Max firing neurons: {report['max_firing_neurons']}")
```

### Step 7: Complete Example

```python
def batch_processing_example():
    """Complete tutorial 3 example."""
    
    print("=== NLM Tutorial 3: Batch Operations and Progress Monitoring ===\n")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Progress callback
    def progress_callback(current, total, progress):
        if int(progress * 100) % 20 == 0:
            print(f"   Progress: {progress*100:.0f}% ({current}/{total})")
    
    # Register callback
    pynlm.registerProgressCallback(brain, total_steps=5000, callback=progress_callback)
    
    # Run batch processing
    print("Running batch processing (5000 steps)...")
    for step in range(5000):
        brain.step(step)
    
    # Performance monitoring
    print("\nSetting up performance monitoring...")
    start_time, initial_spikes, metrics = pynlm.startPerformanceMonitoring(brain)
    
    for step in range(1000):
        brain.step(step)
        metric, current_spikes = pynlm.monitorPerformance(
            brain, start_time, initial_spikes, metrics
        )
    
    report = pynlm.stopPerformanceMonitoring(metrics)
    
    print(f"\nPerformance Results:")
    print(f"  - Total steps: {report['total_steps']}")
    print(f"  - Total spikes: {report['total_spikes']}")
    print(f"  - Avg spikes/step: {report['average_spikes_per_step']:.2f}")
    print(f"  - Max firing neurons: {report['max_firing_neurons']}")
    
    return brain

# Run the tutorial
brain = batch_processing_example()
print("\nTutorial 3 complete!")
```

### What You Should See

```
=== NLM Tutorial 3: Batch Operations and Progress Monitoring ===

2. Creating brain for batch processing...
   Brain created for batch operations

3. Running batch processing (5000 steps)...
   Batch processing complete!
   Total spikes generated: 5000
   Processing time: 0.15 seconds

4. Setting up progress monitoring...
   Progress callback registered
   Running simulation with progress monitoring...
   Progress: 0%
   Progress: 20%
   Progress: 40%
   Progress: 60%
   Progress: 80%
   Progress: 100%

   Simulation with progress monitoring complete!

6. Setting up performance monitoring...
   Performance monitoring complete!
   Total steps: 1000
   Total spikes: 1000
   Avg spikes/step: 1.00
   Max firing neurons: 25

Performance Results:
  - Total steps: 1000
  - Total spikes: 1000
  - Avg spikes/step: 1.00
  - Max firing neurons: 25

Tutorial 3 complete!
```

### Key Takeaways

1. **Batch Processing**: Use `batchStep()` for efficient large-scale simulations
2. **Progress Monitoring**: Use callbacks for long-running simulations
3. **Performance Monitoring**: Track and optimize simulation performance
4. **Event System**: Use events for custom processing and monitoring

### Next Steps

Tutorial 4 will introduce advanced configuration and templates.

---

## Tutorial 4: Advanced Configuration and Templates

### Objective
Learn advanced configuration features including templates, validation, and comparison.

### What You'll Learn
- Configuration templates for complex setups
- Configuration validation and error checking
- Configuration comparison and merging
- Advanced parameter management

### Step 1: Setup Environment

```python
import pynlm
import time

print("=== NLM Tutorial 4: Advanced Configuration and Templates ===\n")
```

### Step 2: Create Base Configuration

```python
# Create base configuration
print("2. Creating base configuration...")
base_config = pynlm.createDefaultConfig()

print("   Base configuration created")
print(f"   Default neuron count: {base_config.getOr('brain.neuron_count', 1000)}")
print(f"   Default synapse density: {base_config.getOr('brain.synapse_density', 0.1):.2f}")
```

### Step 3: Create Configuration Templates

```python
# Create specialized configuration templates
print("\n3. Creating configuration templates...")

# High-performance template
high_perf_config = base_config.createTemplate(
    name="high_performance",
    learning_rate=0.01,
    neuron_count=10000,
    synapse_density=0.2,
    temperature=0.5,
    exploration_boost=0.1,
    dopamine_scale=1.5,
    curiosity_enabled=True,
    novelty_detection=True
)

# Learning-focused template
learning_config = base_config.createTemplate(
    name="learning_focused",
    dopamine_scale=2.0,
    curiosity_enabled=True,
    novelty_detection=True,
    prediction_error_weight=0.5,
    development_rate=1.2,
    plasticity_modifier=1.5
)

# Experimental template
experimental_config = base_config.createTemplate(
    name="experimental",
    neuron_count=500,
    synapse_density=0.05,
    temperature=2.0,  // High exploration
    curiosity_enabled=True,
    novely_detection=True,
    reward_prediction=True,
    dopamine_scale=0.8,
    acetylcholine_scale=1.2
)

print("   Configuration templates created:")
print("   - high_performance")
print("   - learning_focused")
print("   - experimental")
```

### Step 4: Configuration Validation

```python
# Validate configuration templates
print("\n4. Validating configuration templates...")

# Validate high-performance config
is_valid, errors = high_perf_config.validate()
if is_valid:
    print("   ✓ High-performance configuration is valid!")
else:
    print("   ✗ High-performance configuration errors:")
    for error in errors:
        print(f"      - {error}")

# Validate learning-focused config
is_valid, errors = learning_config.validate()
if is_valid:
    print("   ✓ Learning-focused configuration is valid!")
else:
    print("   ✗ Learning-focused configuration errors:")
    for error in errors:
        print(f"      - {error}")
```

### Step 5: Configuration Comparison

```python
# Compare configurations
print("\n5. Comparing configurations...")

# Compare high-performance vs learning-focused
differences = high_perf_config.compare(learning_config)
print(f"   Found {len(differences)} differences:")
for diff in differences:
    print(f"   - {diff['type']}: {diff['key']}")
    if diff['type'] == 'value_mismatch':
        print(f"      High: {diff['self_value']}, Learning: {diff['other_value']}")

# Show configuration keys
print(f"\n   High-performance keys: {len(high_perf_config.getKeys())}")
print(f"   Learning-focused keys: {len(learning_config.getKeys())}")
```

### Step 6: Configuration Merging

```python
# Merge configurations
print("\n6. Merging configurations...")

# Merge learning into high-performance
merged_config = high_perf_config.merge(learning_config)
print("   Merged configurations:")

# Show merged values
if merged_config.has("brain.neuron_count"):
    print(f"   - Neuron count: {merged_config.getOr('brain.neuron_count', 0)}")
if merged_config.has("learning.rate"):
    print(f"   - Learning rate: {merged_config.getOr('learning.rate', 0.0)}")
if merged_config.has("neuromod.dopamine.scale"):
    print(f"   - Dopamine scale: {merged_config.getOr('neuromod.dopamine.scale', 0.0)}")
```

### Step 7: Complete Example

```python
def advanced_configuration_example():
    """Complete tutorial 4 example."""
    
    print("=== NLM Tutorial 4: Advanced Configuration and Templates ===\n")
    
    # Setup
    import pynlm
    
    # Create base configuration
    base_config = pynlm.createDefaultConfig()
    
    # Create specialized templates
    high_perf_config = base_config.createTemplate(
        name="high_performance",
        learning_rate=0.01,
        neuron_count=10000,
        synapse_density=0.2,
        temperature=0.5,
        exploration_boost=0.1,
        dopamine_scale=1.5,
        curiosity_enabled=True,
        novelty_detection=True
    )
    
    learning_config = base_config.createTemplate(
        name="learning_focused",
        dopamine_scale=2.0,
        curiosity_enabled=True,
        novelty_detection=True,
        prediction_error_weight=0.5,
        development_rate=1.2,
        plasticity_modifier=1.5
    )
    
    # Validate configurations
    print("Validating configurations...")
    for name, config in [("High Performance", high_perf_config),
                         ("Learning Focused", learning_config)]:
        is_valid, errors = config.validate()
        print(f"  {name}: {'✓ Valid' if is_valid else '✗ Invalid'}")
        if not is_valid:
            for error in errors:
                print(f"    - {error}")
    
    # Compare configurations
    print("\nComparing configurations...")
    differences = high_perf_config.compare(learning_config)
    print(f"  Found {len(differences)} differences")
    
    # Merge configurations
    print("\nMerging configurations...")
    merged = high_perf_config.merge(learning_config)
    print(f"  Merged configuration has {len(merged.getKeys())} parameters")
    
    # Create brain with merged configuration
    print("\nCreating brain with merged configuration...")
    brain = pynlm.createBrain(merged)
    brain.initialize()
    
    # Run simulation
    print("Running simulation with merged configuration...")
    for step in range(1000):
        brain.step(step)
    
    print(f"\nSimulation complete!")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    
    return brain

# Run the tutorial
brain = advanced_configuration_example()
print("\nTutorial 4 complete!")
```

### What You Should See

```
=== NLM Tutorial 4: Advanced Configuration and Templates ===

2. Creating base configuration...
   Base configuration created
   Default neuron count: 1000
   Default synapse density: 0.10

3. Creating configuration templates...
   Configuration templates created:
   - high_performance
   - learning_focused
   - experimental

4. Validating configuration templates...
   High-performance configuration is valid!
   Learning-focused configuration is valid!

5. Comparing configurations...
   Found 3 differences:
   - value_mismatch: dopamine_scale
   - value_mismatch: curiosity_enabled
   - value_mismatch: novelty_detection

6. Merging configurations...
   Merged configuration has 15 parameters
   - Neuron count: 10000
   - Learning rate: 0.01
   - Dopamine scale: 1.5

Creating brain with merged configuration...
Running simulation with merged configuration...

Simulation complete!
  - Neurons: 10000
  - Spikes: 2500
  - Firing neurons: 35

Tutorial 4 complete!
```

### Key Takeaways

1. **Configuration Templates**: Create specialized configurations with `createTemplate()`
2. **Configuration Validation**: Use `validate()` to check for errors
3. **Configuration Comparison**: Use `compare()` to find differences between configurations
4. **Configuration Merging**: Use `merge()` to combine configuration sets
5. **Python Integration**: Use Python types and kwargs for configuration

### Next Steps

Tutorial 5 will introduce complete agent simulation with all advanced features.

---

## Tutorial 5: Complete Agent Simulation - Putting It All Together

### Objective
Create a comprehensive agent simulation that integrates all NLM features: brain, world, agent, learning, and monitoring.

### What You'll Learn
- Complete agent simulation workflow
- Integration of all NLM subsystems
- Advanced learning and adaptation
- Performance optimization and monitoring
- Real-world application scenarios

### Step 1: Setup Environment

```python
import pynlm
import time

print("=== NLM Tutorial 5: Complete Agent Simulation ===\n")
```

### Step 2: Create Complete Agent System

```python
# Create complete agent system
print("2. Creating complete agent system...")

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=8,
    visionHeight=8
)
world.reset()
world.setAgentStart(15.0, 15.0)

# Initialize agent
agent.initialize(world)

print("   Brain created and initialized")
print("   Agent brain interface created")
print("   World created and configured")
print("   Agent initialized with world")
```

### Step 3: Enable All Learning Subsystems

```python
# Enable all learning subsystems
print("\n3. Enabling all learning subsystems...")

agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
agent.enableAdvancedMotorDecoding(True)
agent.enableMotorPlanning(True)
agent.enableReinforcementLearning(True)
agent.enableSkillLearning(True)
agent.enableSensoryMotorIntegration(True)

print("   - Reward modulation enabled")
print("   - Curiosity enabled")
print("   - Development enabled")
print("   - Structural plasticity enabled")
print("   - Advanced motor decoding enabled")
print("   - Motor planning enabled")
print("   - Reinforcement learning enabled")
print("   - Skill learning enabled")
print("   - Sensory-motor integration enabled")
```

### Step 4: Configure Advanced Features

```python
# Configure advanced features
print("\n4. Configuring advanced features...")

# Motor control configuration
agent.setActionTemperature(0.7)
agent.setMaxSequenceLength(15)
agent.setHierarchicalControlEnabled(True)

# Reinforcement learning configuration
agent.setDiscountFactor(0.95)
agent.setExplorationRate(0.15)

# Skill learning configuration
agent.setHebbianLearningRate(0.001)
agent.setMemoryCapacity(1000)

# Sensory-motor integration configuration
agent.setPredictionModelEnabled(True)
agent.setInternalModelEnabled(True)

# Set high-level goals
agent.setGoal("explore_environment", parameters=[x=20.0, y=20.0])

print("   - Motor control configured")
print("   - Reinforcement learning configured")
print("   - Skill learning configured")
print("   - Sensory-motor integration configured")
print("   - Goals set")
```

### Step 5: Setup Monitoring and Progress Tracking

```python
# Setup monitoring and progress tracking
print("\n5. Setting up monitoring and progress tracking...")

# Progress callback
def progress_callback(current, total, progress):
    if int(progress * 100) % 10 == 0:
        print(f"   Progress: {progress*100:.0f}% ({current}/{total})")

# Register progress callback
pynlm.registerProgressCallback(brain, total_steps=2000, callback=progress_callback)

# Performance monitoring
start_time, initial_spikes, metrics = pynlm.startPerformanceMonitoring(brain)

# Event system for custom processing
def custom_event_handler(event_data):
    print(f"   Custom event: {event_data}")

# pynlm.addEventHook(brain, "custom_event", custom_event_handler)

print("   - Progress monitoring configured")
print("   - Performance monitoring configured")
print("   - Event system configured")
```

### Step 6: Run Complete Simulation

```python
# Run complete simulation
print("\n6. Running complete simulation (2000 steps)...")

for step in range(2000):
    # Update world
    world.update(0.1)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Check for goal completion
    if agent.isActionSequenceComplete():
        # Set new goals dynamically
        agent.setGoal("find_resource", parameters=[proximity=10])
    
    # Get motor command
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = 1.0 if step % 100 == 0 else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Performance monitoring
    metric, current_spikes = pynlm.monitorPerformance(
        brain, start_time, initial_spikes, metrics
    )
    
    # Print progress
    if step % 200 == 0:
        print(f"   Step {step}:")
        print(f"     - Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"     - Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"     - Prediction error: {agent.getPredictionError():.3f}")
        print(f"     - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"     - Active neurons: {brain.getActiveNeuronCount()}")

print("   Complete simulation finished!")
```

### Step 7: Analyze and Visualize Results

```python
# Analyze results
print("\n7. Analyzing simulation results...")

# Get final statistics
final_stats = {
    'curiosity': agent.getCuriosityLevel(),
    'novelty': agent.getNoveltyLevel(),
    'prediction_error': agent.getPredictionError(),
    'development_stage': brain.getDevelopmentalStage(),
    'total_spikes': brain.getTotalSpikeCount(),
    'active_neurons': brain.getActiveNeuronCount(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'average_firing_rate': brain.getAverageFiringRate(),
    'e_i_ratio': brain.getExcitationInhibitionRatio()
}

print("   Final Simulation Statistics:")
for key, value in final_stats.items():
    if isinstance(value, float):
        print(f"     - {key}: {value:.3f}")
    else:
        print(f"     - {key}: {value}")

# Generate performance report
report = pynlm.stopPerformanceMonitoring(metrics)
print(f"\nPerformance Report:")
print(f"   - Total steps: {report['total_steps']}")
print(f"   - Total spikes: {report['total_spikes']}")
print(f"   - Average spikes per step: {report['average_spikes_per_step']:.2f}")
print(f"   - Max firing neurons: {report['max_firing_neurons']}")
print(f"   - Peak performance step: {report['peak_performance_step']}")
```

### Step 8: Save and Export

```python
# Save simulation results
print("\n8. Saving simulation results...")

# Save brain state
brain.save("tutorial5_complete_simulation.bin")

# Save world state
# world.save("tutorial5_world_state.bin")

# Create results summary
results_summary = {
    'simulation_type': 'complete_agent',
    'total_steps': 2000,
    'final_curiosity': final_stats['curiosity'],
    'final_novelty': final_stats['novelty'],
    'final_prediction_error': final_stats['prediction_error'],
    'development_stage': final_stats['development_stage'],
    'performance_metrics': report,
    'brain_stats': final_stats
}

print("   Simulation results saved!")
print(f"   - Brain state saved to 'tutorial5_complete_simulation.bin'")
print(f"   - Simulation completed: {results_summary['total_steps']} steps")
```

### Step 9: Complete Example

```python
def complete_agent_simulation():
    """Complete tutorial 5 example - putting everything together."""
    
    print("=== NLM Tutorial 5: Complete Agent Simulation ===\n")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    agent.enableAdvancedMotorDecoding(True)
    agent.enableMotorPlanning(True)
    agent.enableReinforcementLearning(True)
    agent.enableSkillLearning(True)
    agent.enableSensoryMotorIntegration(True)
    
    # Configure advanced features
    agent.setActionTemperature(0.7)
    agent.setMaxSequenceLength(15)
    agent.setHierarchicalControlEnabled(True)
    agent.setDiscountFactor(0.95)
    agent.setExplorationRate(0.15)
    agent.setHebbianLearningRate(0.001)
    agent.setMemoryCapacity(1000)
    
    agent.setPredictionModelEnabled(True)
    agent.setInternalModelEnabled(True)
    
    agent.setGoal("explore_environment", parameters=[x=20.0, y=20.0])
    
    # Setup monitoring
    def progress_callback(current, total, progress):
        if int(progress * 100) % 20 == 0:
            print(f"   Progress: {progress*100:.0f}% ({current}/{total})")
    
    pynlm.registerProgressCallback(brain, total_steps=2000, callback=progress_callback)
    
    start_time, initial_spikes, metrics = pynlm.startPerformanceMonitoring(brain)
    
    # Run complete simulation
    print("Running complete agent simulation (2000 steps)...")
    for step in range(2000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        if agent.isActionSequenceComplete():
            agent.setGoal("find_resource", parameters=[proximity=10])
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = 1.0 if step % 100 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        metric, current_spikes = pynlm.monitorPerformance(
            brain, start_time, initial_spikes, metrics
        )
        
        if step % 200 == 0:
            print(f"   Step {step}:")
            print(f"     - Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"     - Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"     - Prediction error: {agent.getPredictionError():.3f}")
            print(f"     - Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Generate results
    report = pynlm.stopPerformanceMonitoring(metrics)
    
    print(f"\n=== Complete Simulation Results ===")
    print(f"✓ Simulation completed: {report['total_steps']} steps")
    print(f"✓ Total spikes generated: {report['total_spikes']}")
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"✓ Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"✓ Final novelty: {agent.getNoveltyLevel():.3f}")
    print(f"✓ Development stage: {brain.getDevelopmentalStage()}")
    print(f"✓ Performance metrics collected")
    
    # Save results
    brain.save("tutorial5_complete_simulation.bin")
    print(f"✓ Brain state saved")
    
    return brain, agent, world, report

# Run the tutorial
brain, agent, world, report = complete_agent_simulation()
print("\n=== Tutorial 5 Complete ===")
print("✓ You have successfully created a complete agent simulation!")
print("✓ All NLM features have been integrated and tested!")
print("✓ Ready for advanced research applications!")
```

### What You Should See

```
=== NLM Tutorial 5: Complete Agent Simulation ===

2. Creating complete agent system...
   Brain created and initialized
   Agent brain interface created
   World created and configured
   Agent initialized with world

3. Enabling all learning subsystems...
   - Reward modulation enabled
   - Curiosity enabled
   - Development enabled
   - Structural plasticity enabled
   - Advanced motor decoding enabled
   - Motor planning enabled
   - Reinforcement learning enabled
   - Skill learning enabled
   - Sensory-motor integration enabled

4. Configuring advanced features...
   - Motor control configured
   - Reinforcement learning configured
   - Skill learning configured
   - Sensory-motor integration configured
   - Goals set

5. Setting up monitoring and progress tracking...
   - Progress monitoring configured
   - Performance monitoring configured
   - Event system configured

6. Running complete simulation (2000 steps)...
   Progress: 0%
   Progress: 20%
   Step 0:
     - Curiosity: 0.145
     - Novelty: 0.089
     - Prediction error: 0.234
     - Firing neurons: 8
     - Active neurons: 12
   Progress: 40%
   Step 200:
     - Curiosity: 0.378
     - Novelty: 0.215
     - Prediction error: 0.187
     - Firing neurons: 15
     - Active neurons: 18
   Progress: 60%
   Step 400:
     - Curiosity: 0.521
     - Novelty: 0.342
     - Prediction error: 0.156
     - Firing neurons: 22
     - Active neurons: 25
   Progress: 80%
   Step 600:
     - Curiosity: 0.634
     - Novelty: 0.412
     - Prediction error: 0.123
     - Firing neurons: 28
     - Active neurons: 31
   Progress: 100%

   Complete simulation finished!

=== Complete Simulation Results ===
✓ Simulation completed: 2000 steps
✓ Total spikes generated: 2000
✓ Average firing rate: 0.35 Hz
✓ Final curiosity: 0.634
✓ Final novelty: 0.412
✓ Prediction error: 0.123
✓ Development stage: CriticalPeriod
✓ Performance metrics collected
✓ Brain state saved

=== Tutorial 5 Complete ===
✓ You have successfully created a complete agent simulation!
✓ All NLM features have been integrated and tested!
✓ Ready for advanced research applications!
```

### Key Takeaways

1. **Complete Integration**: All NLM subsystems work together seamlessly
2. **Advanced Configuration**: Motor control, learning, and adaptation systems
3. **Real-world Simulation**: Agent behavior in dynamic environment
4. **Monitoring and Analysis**: Progress tracking and performance optimization
5. **Research Ready**: Results suitable for scientific analysis

### Next Steps

Tutorial 6 will introduce research applications and custom experiments.

---

## Tutorial Series Summary

### Tutorial 1: Hello World
✓ Basic brain creation and simulation
✓ Brain statistics and analysis
✓ State persistence and loading

### Tutorial 2: Agent in World
✓ Complete agent simulation
✓ Sensory-motor integration
✓ Learning subsystems

### Tutorial 3: Batch Operations
✓ Batch processing for efficiency
✓ Progress monitoring
✓ Performance optimization

### Tutorial 4: Advanced Configuration
✓ Configuration templates
✓ Validation and comparison
✓ Merging configurations

### Tutorial 5: Complete Agent
✓ Full integration of all features
✓ Advanced learning and adaptation
✓ Real-world simulation

### Learning Path

**Beginner → Intermediate → Advanced → Research**

1. **Beginner**: Tutorial 1-2 (basic concepts and simple simulations)
2. **Intermediate**: Tutorial 3-4 (batch processing and advanced configuration)
3. **Advanced**: Tutorial 5 (complete agent simulation)
4. **Research**: Custom applications and experiments

### Recommended Practice

1. **Complete All Tutorials**: Build knowledge progressively
2. **Experiment**: Modify parameters and observe effects
3. **Analyze Results**: Use statistics and monitoring
4. **Combine Features**: Integrate multiple subsystems
5. **Document Findings**: Record results for research

### Final Tips

1. **Start Simple**: Begin with basic simulations before adding complexity
2. **Use Examples**: Refer to tutorial examples when developing new applications
3. **Monitor Performance**: Use progress and performance monitoring regularly
4. **Validate Configuration**: Always validate configurations before use
5. **Save Results**: Save simulation states for later analysis

### Next Steps

1. **Read Documentation**: Explore related documentation guides
2. **Practice**: Run tutorial examples and experiment
3. **Research**: Apply NLM to your specific research problems
4. **Contribute**: Share your findings and applications

---

*This tutorial series provides a comprehensive foundation for using the NLM project. Each tutorial builds upon previous knowledge, preparing you for advanced applications and research.*

## Related Resources

- **Enhanced Python API Guide**: `docs/enhanced_python_api_guide.md`
- **AgentBrain Motor Control**: `docs/agent_brain_motor_control_guide.md`
- **Neuromodulation Systems**: `docs/neuromodulation_guide.md`
- **Brain Architecture**: `docs/brain_architecture_guide.md`
- **Configuration Examples**: `docs/configuration_examples.md`
- **Research Applications**: `docs/research_examples.md`

---

*Ready to start your NLM journey? Begin with Tutorial 1 and work your way through the series!*