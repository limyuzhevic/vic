# NLM Python Enhanced API Guide

This guide covers the enhanced Python API for NLM (Neural Learning Machine), including new convenience methods, advanced features, and debugging tools.

## Overview

The NLM Python API has been significantly enhanced to provide:
- **Convenience methods** for easier usage
- **Advanced debugging and monitoring** tools
- **Enhanced configuration** options
- **Better error handling** and validation
- **Developer-friendly APIs** for complex use cases

## Enhanced Brain Class

### Core Methods

#### Basic Brain Operations
```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Run simulation steps
for i in range(100):
    brain.step(i)

# Get comprehensive statistics
stats = brain.getBrainStatistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Total spikes: {stats['total_spikes']}")
```

#### Enhanced Step Methods
```python
# Basic step
brain.step(step_number)

# Step with timestamp
brain.step(step_number, time)

# Step with automatic metrics collection
firing_count, firing_rate, total_spikes = brain.stepWithMetrics(step_number, time)

# Run complete simulation episode with results
simulation_results = brain.simulateEpisode(1000)
for step_data in simulation_results:
    print(f"Step {step_data['step']}: {step_data['firing_neurons']} neurons firing")
```

#### Action Production with Debug Info
```python
# Basic action production
action = brain.produceAction()

# Action with debug info
(action_name, action_obj) = brain.produceActionWithDebug()
print(f"Action: {action_name}")
```

### Memory and Development
```python
# Development stage with human-readable name
stage = brain.getDevelopmentalStage()
stage_name = brain.getDevelopmentalStageName()
print(f"Development stage: {stage_name} (progress: {stage * 20:.0f}%)")

# Set development stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)

# Get learned preference (-1 to 1, survives checkpoint)
preference = brain.getLearnedPreference()
print(f"Learned preference: {preference:.3f}")

brain.setLearnedPreference(0.5)

# Get developmental stage progress (0-1)
progress = brain.getStageProgress()
```

### Region Management
```python
# Add a new region
region_id = brain.addRegion("cortex")

# Get region by ID
region = brain.getRegion(region_id)

# Get region by name (convenience method)
region = brain.getRegionByName("cortex")

# Get all regions
regions = brain.getRegions()

# Add simple synapse between regions
success = brain.addSimpleSynapse(region1_id, region2_id, weight=1.0)
```

### Convenience Configuration
```python
# Configure brain with multiple parameters in one go
brain.getConfig().set("brain.neuron_count", 2000)
brain.getConfig().set("brain.synapse_density", 0.15)
brain.getConfig().set("plasticity.stdp.enable", True)

# Check if configuration exists
if brain.getConfig().has("brain.neuron_count"):
    neurons = brain.getConfig().getOr("brain.neuron_count", 1000)
```

## Enhanced AgentBrain Class

### Agent Initialization and Features
```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configureWorld(30, 30, 16, 16)  # 30x30 world, 16x16 vision
world.populateWorld()  # Add default resources and hazards

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features (convenience method)
agent.enableAll()

# Disable all learning features
agent.disableAll()

# Check enabled features
features = agent.getAllEnabledFeatures()
print(f"Reward modulation: {features['reward_modulation']}")
print(f"Structural plasticity: {features['structural_plasticity']}")
print(f"Development: {features['development']}")
print(f"Curiosity: {features['curiosity']}")
```

### Enhanced Motor Command Decoding
```python
# Basic motor command decoding
action = agent.decodeMotorCommand()

# Motor command with human-readable name
(action_name, action) = agent.getMotorCommandDistribution()
print(f"Action: {action_name}")

# Detailed motor group activities
motor_groups = agent.getMotorGroupDebug()
for group in motor_groups:
    print(f"{group['name']}: {group['firing_rate']:.2f} Hz (activity: {group['activity']:.3f})")
```

### Neuromodulation and Development
```python
# Get development stage with progress
stage = agent.getDevelopmentalStage()
stage_name = agent.getDevelopmentalStageName()
print(f"Developmental stage: {stage_name}")

# Get curiosity as percentage
curiosity_pct = agent.getCuriosityPercentage()
print(f"Curiosity level: {curiosity_pct:.1f}%")

# Get prediction error
prediction_error = agent.getPredictionError()
print(f"Prediction error: {prediction_error:.4f}")

# Get thought trace for debugging
thought_trace = agent.getThoughtTrace()
print(f"Thought: {thought_trace}")
```

### Advanced Agent Features
```python
# Get concept count and self-model confidence
concept_count = agent.getConceptCount()
confidence = agent.getSelfModelConfidence()
print(f"Concepts learned: {concept_count}")
print(f"Self-model confidence: {confidence:.3f}")

# Get thought trace with complete information
thought_info = agent.getThoughtTrace()
if thought_info and thought_info != "none":
    print(f"Agent thought trace: {thought_info}")
```

## Enhanced SimpleWorld Class

### World Configuration and Management
```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configureWorld(20, 20, 8, 8)  # 20x20 world, 8x8 vision

# Populate with default objects
world.populateWorld()

# Add specific objects
world.addResource(5, 5, value=50.0)  # Resource with 50 energy
world.addHazard(15, 15, damage=-25.0)  # Hazard with 25 damage

# Get comprehensive world information
world_info = world.getWorldInfo()
print(f"World size: {world_info['width']}x{world_info['height']}")
print(f"Agent position: ({world_info['agent_x']:.1f}, {world_info['agent_y']:.1f})")
print(f"Agent energy: {world_info['agent_energy']:.1f}/{world_info['max_energy']}")
print(f"Simulation time: {world_info['simulation_time']:.3f}s")
```

### Enhanced Movement and Safety
```python
# Check if position is safe to move
is_safe = world.isSafeToMove(10.5, 10.5)
print(f"Position (10.5, 10.5) is safe: {is_safe}")

# Set agent start position
world.setStartingPosition(5.0, 5.0)  # Alias for setAgentStart

# Reset world (re-populates with default objects)
world.reset()

# Get/set agent body
agent_body = world.getAgentBody()
print(f"Agent orientation: {agent_body.orientation:.3f} radians")

# Get/set agent state (alias)
agent_state = world.getAgentState()
```

### World Updates and Actions
```python
# Update world (alias for update)
world.update(0.1)  # 100ms step
world.step(0.1)    # Same as update, alternative name

# Apply motor command
result = world.applyMotorCommand(pynlm.MotorCommand.MoveForward, world.getCurrentTime())
print(f"Action result: reward={result.reward}, success={result.success}, message={result.message}")

# Apply action (alias for applyMotorCommand)
result = world.applyAction(pynlm.MotorCommand.TurnLeft, world.getCurrentTime())
```

## Advanced Usage Examples

### Complete Agent Simulation
```python
import pynlm

def run_advanced_simulation(steps=500):
    """Run a complete agent simulation with monitoring."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.08)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configureWorld(40, 40, 16, 16)
    world.populateWorld()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all features
    agent.enableAll()
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        
        # Get percept and process
        percept = world.getPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step, step * 0.1)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyAction(action, world.getCurrentTime())
        
        # Apply reward modulation
        reward = agent.getCuriosityLevel() * 0.1  # Simple reward based on curiosity
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 100 steps
        if step % 100 == 0:
            stats = brain.getBrainStatistics()
            print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
                  f"curiosity: {agent.getCuriosityPercentage():.1f}%")
    
    # Final statistics
    final_stats = {
        "total_steps": steps,
        "final_firing_rate": brain.getAverageFiringRate(),
        "total_spikes": brain.getTotalSpikeCount(),
        "final_curiosity": agent.getCuriosityLevel(),
        "final_novelty": agent.getNoveltyLevel(),
        "final_prediction_error": agent.getPredictionError(),
        "learned_preference": agent.getLearnablePreference()
    }
    
    return final_stats

# Run the simulation
results = run_advanced_simulation(500)
print("\n=== Simulation Complete ===")
for key, value in results.items():
    print(f"{key}: {value}")
```

### Development Stage Simulation
```python
import pynlm

def simulate_development_stages():
    """Simulate brain through different developmental stages."""
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    stages = [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    stage_durations = [50, 100, 200, 300, 100]  # Steps per stage
    
    for stage, duration in zip(stages, stage_durations):
        brain.setDevelopmentalStage(stage)
        stage_name = brain.getDevelopmentalStageName()
        progress = brain.getStageProgress()
        
        print(f"\n=== {stage_name} Stage (Progress: {progress:.2f}) ===")
        
        for step in range(duration):
            brain.step(step, step * 0.01)
            
            if step % 50 == 0:
                firing_rate = brain.getAverageFiringRate()
                print(f"  Step {step}: {firing_rate:.2f} Hz firing rate")
    
    return brain.getBrainStatistics()

# Run development simulation
stats = simulate_development_stages()
print(f"\nFinal statistics: {stats['total_neurons']} neurons, "
      f"{stats['total_spikes']} total spikes")
```

### Configuration Management
```python
import pynlm

# Create and configure brain with advanced settings
config = pynlm.createDefaultConfig()

# Set multiple configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.12)
config.set("brain.connection_probability", 0.08)
config.set("brain.v_thresh", -45.0)  # Less negative threshold
config.set("brain.tau_mem", 15.0)    # Faster membrane time constant

# Plasticity settings
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation settings
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Save configuration
config.saveToFile("advanced_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("advanced_config.json")

brain = pynlm.createBrain(config2)
brain.initialize()

# Verify configuration
print(f"Neuron count: {brain.getConfig().getOr('brain.neuron_count', 1000)}")
print(f"STDP enabled: {brain.getConfig().getOr('plasticity.stdp.enable', False)}")
```

### Text Association (Phase 6 Feature)
```python
import pynlm

# Create agent with Phase 6 text association
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Encode text into neural patterns
"""This creates sparse patterns in the internal neurons for text association.
The brain learns associations between these patterns and visual scenes.
"""
apple_pattern = AgentBrain.encodeText("apple")
banana_pattern = AgentBrain.encodeText("banana")

print(f"Apple pattern: {apple_pattern}")
print(f"Banana pattern: {banana_pattern}")

# Get internal neuron firing rates for recall
internal_rates = agent.getInternalNeuronsFiring()
print(f"Internal neurons: {len(internal_rates)} patterns")

# Phase 6 allows the brain to associate words with visual concepts
# through plasticity, not string matching
```

## Error Handling and Validation

### Basic Error Handling
```python
import pynlm
import traceback

def safe_simulation():
    """Run simulation with proper error handling."""
    try:
        # Validate world bounds before moving
        world = pynlm.createSimpleWorld()
        world.configureWorld(10, 10, 8, 8)
        
        # Create agent
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            
            # Validate position before processing
            percept = world.getPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyAction(action, world.getCurrentTime())
            
        return True
        
    except Exception as e:
        print(f"Simulation error: {e}")
        traceback.print_exc()
        return False

# Run safe simulation
if safe_simulation():
    print("Simulation completed successfully")
else:
    print("Simulation failed")
```

## Performance Monitoring

### Monitoring Brain Performance
```python
import pynlm
import time

def monitor_performance(steps=1000):
    """Monitor brain performance during simulation."""
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configureWorld(20, 20, 16, 16)
    world.populateWorld()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAll()
    
    start_time = time.time()
    step_times = []
    
    for step in range(steps):
        step_start = time.time()
        
        world.update(0.1)
        percept = world.getPercept()
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.1)
        action = agent.decodeMotorCommand()
        world.applyAction(action, world.getCurrentTime())
        agent.applyRewardModulation(agent.getCuriosityLevel() * 0.1, 0.0)
        agent.updateDevelopment(0.1)
        
        step_time = time.time() - step_start
        step_times.append(step_time)
    
    total_time = time.time() - start_time
    
    # Performance statistics
    avg_step_time = sum(step_times) / len(step_times)
    steps_per_second = steps / total_time
    
    print(f"Performance Summary:")
    print(f"  Total time: {total_time:.2f}s")
    print(f"  Steps per second: {steps_per_second:.1f}")
    print(f"  Average step time: {avg_step_time * 1000:.2f}ms")
    print(f"  Min step time: {min(step_times) * 1000:.2f}ms")
    print(f"  Max step time: {max(step_times) * 1000:.2f}ms")
    
    # Memory usage (approximate)
    stats = brain.getBrainStatistics()
    print(f"  Brain memory: {stats['total_neurons']} neurons, {stats['total_synapses']} synapses")
    
    return {
        "total_time": total_time,
        "steps_per_second": steps_per_second,
        "avg_step_time": avg_step_time,
        "min_step_time": min(step_times),
        "max_step_time": max(step_times),
        "final_stats": stats
    }

# Run performance monitoring
perf_results = monitor_performance(500)
```

## Migration Guide

### From Old API to Enhanced API

#### Old Style (Minimal API)
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

#### New Style (Enhanced API)
```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get comprehensive statistics
stats = brain.getBrainStatistics()
print(f"Simulation: {stats['total_spikes']} spikes in {stats['total_neurons']} neurons")

# Run simulation with monitoring
for step in range(100):
    brain.stepWithMetrics(step)  # Gets automatic metrics
    if step % 20 == 0:
        firing_rate = brain.getAverageFiringRate()
        print(f"Step {step}: {firing_rate:.2f} Hz")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain doesn't fire neurons**
- Solution: Check if brain is initialized (`brain.initialize()`)
- Solution: Add sensory input (`agent.processSensoryInput(percept)`)
- Solution: Enable neuromodulation for learning

**Issue: Agent doesn't move**
- Solution: Check world update is called (`world.update(0.1)`)
- Solution: Verify agent is initialized (`agent.initialize(world)`)
- Solution: Check motor neurons are present

**Issue: Performance is slow**
- Solution: Reduce world size or simulation steps
- Solution: Disable learning features if not needed (`agent.disableAll()`)
- Solution: Use simpler configurations

**Issue: Configuration doesn't apply**
- Solution: Set configuration before creating brain
- Solution: Verify configuration keys are correct
- Solution: Use configuration convenience methods

### Debugging Tips

```python
# Enable debug logging for brain
brain.logStatus()

# Get detailed motor group activities
motor_groups = agent.getMotorGroupDebug()
for group in motor_groups:
    print(f"Debug: {group['name']} - activity={group['activity']:.3f}, "
          f"firing_rate={group['firing_rate']:.2f}Hz")

# Check all enabled features
features = agent.getAllEnabledFeatures()
print("Enabled features:", features)

# Get world and brain info for debugging
world_info = world.getWorldInfo()
brain_info = brain.getBrainStatistics()
```

## Summary

The enhanced NLM Python API provides:

- **50+ new convenience methods** for easier usage
- **Advanced debugging and monitoring** tools
- **Better error handling** and validation
- **Performance monitoring** capabilities
- **Phase 6 features** for text association and advanced cognition
- **Comprehensive documentation** and examples

The API maintains full backward compatibility while adding powerful new features for both beginners and advanced users.

---

*Last updated: 2026*
*Version: Enhanced API v2.0*