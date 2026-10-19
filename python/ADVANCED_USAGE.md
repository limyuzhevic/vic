# NLM - Advanced Usage Guide

This guide covers advanced features and patterns for experienced NLM users.

## Part 1: Advanced Brain Management

### 1.1 Brain Checkpoints and Persistence

```python
import pynlm
import time

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run some steps
for i in range(100):
    brain.step(i)

# Save checkpoint
checkpoint_file = f"nlm_checkpoint_{int(time.time())}.bin"
brain.save(checkpoint_file)
print(f"Saved checkpoint to {checkpoint_file}")

# Later... load the checkpoint
brain2 = pynlm.createBrain(config)
brain2.initialize()
if brain2.load(checkpoint_file):
    print("Successfully loaded checkpoint")
else:
    print("Failed to load checkpoint")
```

### 1.2 Brain Analysis and Introspection

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation to populate the brain
for i in range(500):
    brain.step(i)

# Analyze brain connectivity and statistics
analysis = pynlm.analyzeBrainConnectivity(brain)
print("Brain Analysis:")
for item in analysis:
    print(f"  {item}")

# Get memory usage information
memory_info = pynlm.analyzeMemoryUsage(brain)
print(f"\nMemory Usage:")
for key, value in memory_info.items():
    print(f"  {key}: {value}")

# Validate brain state
validation = pynlm.validateBrainState(brain)
print(f"\nValidation:")
for key, is_valid in validation.items():
    print(f"  {key}: {'✓' if is_valid else '✗'}")
```

### 1.3 Advanced Simulation Control

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

def progress_callback(step, time):
    if step % 100 == 0:
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        print(f"  Step {step}: {firing} firing neurons, {rate:.2f} Hz avg rate")

print("Running advanced simulation with progress callback...")
pynlm.runSimulation(brain, 2000, progress_callback)
print("Simulation complete!")

# Get performance statistics
stats = pynlm.getPerformanceStats()
print(f"\nPerformance Stats:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

## Part 2: Complex Environment Setup

### 2.1 Batch Processing Multiple Brains

```python
import pynlm

# Create multiple brains for batch processing
brains = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500 + i * 100)  # Different sizes
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

print(f"Created {len(brains)} brains for batch processing")

# Define processing function
def analyzeBrain(brain):
    for i in range(100):
        brain.step(i)
    
    firing = brain.getFiringNeuronCount()
    rate = brain.getAverageFiringRate()
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons, {firing} firing, {rate:.2f} Hz")

# Process all brains
print("Processing all brains...")
pynlm.batchProcessBrains(brains, analyzeBrain)

# Clean up (Python will handle garbage collection)
print("Batch processing complete!")
```

### 2.2 Advanced Configuration Builder

```python
import pynlm

# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Modify configuration for specific experiment
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.learning_rate", 0.002f)
config.set("plasticity.hebbian.learning_rate", 0.001f)
config.set("memory.working.capacity", 200)
config.set("memory.episodic.capacity", 2000)

# Save advanced configuration
config.saveToFile("advanced_config.json")
print("Saved advanced configuration to advanced_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("advanced_config.json")
print("Loaded configuration with neuron count:", config2.getOr<int64_t>("brain.neuron_count", 0))
```

## Part 3: Integration Patterns

### 3.1 Complete Agent with Advanced Features

```python
import pynlm

def run_advanced_agent_simulation(num_steps=2000):
    """Run a complete agent simulation with advanced features."""
    
    # 1. Create advanced configuration
    config = pynlm.createAdvancedConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Statistics collection
    stats = {
        'steps': 0,
        'total_reward': 0.0,
        'max_firing': 0,
        'min_firing': float('inf'),
        'avg_curiosity': 0.0,
        'avg_novelty': 0.0,
        'avg_dopamine': 0.0,
        'dev_stage_changes': 0
    }
    
    prev_stage = brain.getDevelopmentalStage()
    
    # 8. Run simulation loop with advanced features
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation with prediction
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted = 0.0  # Could implement actual prediction
        agent.applyRewardModulation(reward, predicted)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        stats['steps'] = step + 1
        stats['total_reward'] += reward
        firing = brain.getFiringNeuronCount()
        stats['max_firing'] = max(stats['max_firing'], firing)
        stats['min_firing'] = min(stats['min_firing'], firing)
        stats['avg_curiosity'] += agent.getCuriosityLevel()
        stats['avg_novelty'] += agent.getNoveltyLevel()
        stats['avg_dopamine'] += agent.getNeuromodulationLevel()
        
        # Track developmental stage changes
        current_stage = brain.getDevelopmentalStage()
        if current_stage != prev_stage:
            stats['dev_stage_changes'] += 1
            prev_stage = current_stage
        
        # Print progress
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {stats['total_reward'] / (step + 1):.3f}")
            print(f"  Firing: {firing} neurons")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development: {agent.getDevelopmentalStage()}")
    
    # Calculate averages
    stats['avg_curiosity'] /= num_steps
    stats['avg_novelty'] /= num_steps
    stats['avg_dopamine'] /= num_steps
    if stats['min_firing'] == float('inf'):
        stats['min_firing'] = 0
    
    return stats

# Run the advanced simulation
print("=== Advanced Agent Simulation ===")
stats = run_advanced_agent_simulation(2000)

print("\n=== FINAL STATISTICS ===")
print(f"Steps completed: {stats['steps']}")
print(f"Total reward: {stats['total_reward']:.2f}")
print(f"Average reward per step: {stats['total_reward'] / stats['steps']:.4f}")
print(f"Firing neurons - Max: {stats['max_firing']}, Min: {stats['min_firing']}")
print(f"Average curiosity: {stats['avg_curiosity']:.3f}")
print(f"Average novelty: {stats['avg_novelty']:.3f}")
print(f"Average dopamine: {stats['avg_dopamine']:.3f}")
print(f"Developmental stage changes: {stats['dev_stage_changes']}")
```

## Part 4: Error Handling and Validation

### 4.1 Robust Error Handling Pattern

```python
import pynlm

def robustSimulation(brain, world, agent, num_steps):
    """Simulation with comprehensive error handling."""
    errors = []
    
    try:
        for step in range(num_steps):
            # Validate components at each step
            if not brain:
                errors.append(f"Step {step}: Brain is null")
                break
                
            if not agent:
                errors.append(f"Step {step}: Agent is null")
                break
            
            # Update world
            world.update(0.1)
            
            # Get and validate sensory input
            percept = world.getSensoryPercept()
            if not percept:
                errors.append(f"Step {step}: Invalid sensory percept")
                continue
            
            # Process sensory input
            try:
                agent.processSensoryInput(percept)
            except Exception as e:
                errors.append(f"Step {step}: Error processing sensory input: {str(e)}")
                continue
            
            # Run brain step
            try:
                brain.step(step)
            except Exception as e:
                errors.append(f"Step {step}: Error running brain step: {str(e)}")
                continue
            
            # Get action
            try:
                action = agent.decodeMotorCommand()
            except Exception as e:
                errors.append(f"Step {step}: Error decoding motor command: {str(e)}")
                continue
            
            # Apply action
            try:
                world.applyMotorCommand(action, world.getSimulationTime())
            except Exception as e:
                errors.append(f"Step {step}: Error applying motor command: {str(e)}")
                continue
        
    except Exception as e:
        errors.append(f"Simulation error: {str(e)}")
    
    # Print results
    print(f"Simulation completed with {len(errors)} errors")
    if errors:
        print("\nErrors encountered:")
        for error in errors[:10]:  # Show first 10 errors
            print(f"  {error}")
        if len(errors) > 10:
            print(f"  ... and {len(errors) - 10} more errors")
    
    return len(errors) == 0

# Usage example
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

success = robustSimulation(brain, world, agent, 1000)
print(f"Simulation {'successful' if success else 'completed with errors'}")
```

### 4.2 Validation Utilities

```python
import pynlm

# Validate brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Check if configuration has required settings
required_keys = ["brain.neuron_count", "plasticity.stdp.enable"]
for key in required_keys:
    if config.has(key):
        print(f"✓ Configuration has key: {key}")
    else:
        print(f"✗ Configuration missing key: {key}")

# Validate brain state
brain = pynlm.createBrain(config)
brain.initialize()

# Check various brain properties
checks = {
    "brain_exists": brain is not None,
    "initialized": brain.getTotalNeuronCount() > 0,
    "has_regions": brain.getRegionCount() > 0,
    "has_neurons": brain.getTotalNeuronCount() > 0,
    "can_step": True  # Will be tested by attempting to step
}

print("\nBrain validation:")
for check_name, is_valid in checks.items():
    if check_name == "can_step":
        # Actually test stepping
        try:
            brain.step(0)
            print(f"✓ Can perform simulation steps")
        except:
            print(f"✗ Cannot perform simulation steps")
    else:
        print(f"{'✓' if is_valid else '✗'} {check_name}")

# Validate agent components
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent_checks = {
    "agent_exists": agent is not None,
    "brain_connected": agent.getBrain() is not None,
    "can_process_input": True,
    "can_decode_action": True
}

print("\nAgent validation:")
for check_name, is_valid in agent_checks.items():
    if check_name == "can_process_input":
        try:
            # Create a simple percept for testing
            agent.processSensoryInput(percept)
            print(f"✓ Can process sensory input")
        except:
            print(f"✗ Cannot process sensory input")
    elif check_name == "can_decode_action":
        try:
            action = agent.decodeMotorCommand()
            print(f"✓ Can decode motor commands")
        except:
            print(f"✗ Cannot decode motor commands")
    else:
        print(f"{'✓' if is_valid else '✗'} {check_name}")
```

## Part 5: Performance Optimization

### 5.1 Simulation Optimization Pattern

```python
import pynlm
import time

def optimizedSimulation(brain, num_steps, use_optimization=True):
    """Optimized simulation with performance considerations."""
    
    if use_optimization:
        print("Running optimized simulation...")
        
        # Optimization 1: Batch statistics collection
        stats_interval = 100
        last_log_time = time.time()
        
        for step in range(num_steps):
            brain.step(step)
            
            # Log statistics at intervals
            if step % stats_interval == 0:
                current_time = time.time()
                if current_time - last_log_time >= 1.0:  # Log at most once per second
                    firing = brain.getFiringNeuronCount()
                    rate = brain.getAverageFiringRate()
                    print(f"Step {step}: {firing} firing, {rate:.2f} Hz")
                    last_log_time = current_time
        
    else:
        print("Running standard simulation...")
        
        # Run without optimizations (original pattern)
        for step in range(num_steps):
            brain.step(step)
            if step % 500 == 0:
                print(f"Step {step}")
    
    return brain

# Example usage with optimization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Comparing optimized vs standard simulation...")

# Run optimized
start = time.time()
optimizedSimulation(brain, 5000, use_optimization=True)
opt_time = time.time() - start
print(f"Optimized simulation took {opt_time:.2f} seconds")

# Reset brain for standard simulation
brain.reset()
brain.initialize()

# Run standard
start = time.time()
optimizedSimulation(brain, 5000, use_optimization=False)
std_time = time.time() - start
print(f"Standard simulation took {std_time:.2f} seconds")

if opt_time > 0:
    speedup = std_time / opt_time
    print(f"Optimized simulation is {speedup:.2f}x faster")
```

## Common Issues and Solutions

### A. Brain Not Learning

**Issue**: Brain shows no learning (weights not changing)

**Solution**:
```python
# Check if plasticity is enabled
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)  # Ensure STDP is enabled
config.set("plasticity.hebbian.enable", True)  # Ensure Hebbian learning is enabled
config.set("plasticity.structural.enable", True)  # Ensure structural plasticity is enabled

brain = pynlm.createBrain(config)
brain.initialize()

# Test with reward modulation
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
```

### B. Agent Not Moving

**Issue**: Agent's motor neurons are not firing

**Solution**:
```python
# Check brain activity
brain = agent.getBrain()
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Motor neuron count: {agent.getMotorOutputSize()}")

# Run simulation to allow learning
for i in range(1000):
    brain.step(i)
    if brain.getFiringNeuronCount() > 0:
        print(f"Step {i}: Brain is active!")
        break
```

### C. Memory System Not Working

**Issue**: Memory systems not storing or retrieving information

**Solution**:
```python
# Check if memory systems are enabled
brain = agent.getBrain()

# Check working memory
working_mem = brain.getWorkingMemory()
if working_mem:
    print(f"Working memory active traces: {working_mem.getActiveTraces()}")
else:
    print("Working memory not available")

# Check episodic memory
episodic_mem = brain.getEpisodicMemory()
if episodic_mem:
    print(f"Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
else:
    print("Episodic memory not available")

# Test memory integration
for i in range(100):
    brain.step(i)
    if working_mem and working_mem.getActiveTraces() > 0:
        print(f"Working memory is functional at step {i}")
        break
```

## Further Reading

- **Performance Monitoring**: Use `getPerformanceStats()` for real-time performance monitoring
- **Brain Introspection**: Use `analyzeBrainConnectivity()` for detailed brain analysis
- **Validation**: Use `validateBrainState()` to check system integrity
- **Memory Analysis**: Use `analyzeMemoryUsage()` to understand memory distribution

For more complex integrations, consider using the Phase 6 integration experiment as a template for connecting all brain systems.
```
