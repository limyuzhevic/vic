# NLM Python Enhanced API Examples

This file demonstrates the enhanced Python API features for NLM (Neural Learning Machine).

## Quick Start with Enhanced Features

```python
import pynlm

# Create brain with enhanced Pythonic API
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use Pythonic properties for quick access
print(f"Brain has {brain.neuron_count} neurons")
print(f"Development stage: {brain.development_stage}")

# Run enhanced simulation with convenience method
spike_count = brain.simulate(100, 0.1)
print(f"Simulation complete with {spike_count} total spikes")

# Get formatted status information
status = brain.get_status()
print(status)
```

## Example 1: Enhanced Agent with All Subsystems

```python
import pynlm

def run_advanced_agent():
    # Setup components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run episode with enhanced monitoring
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Print enhanced stats periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Dev Stage: {agent.getDevelopmentalStage()}")
    
    return agent, brain, world

# Run the advanced agent
agent, brain, world = run_advanced_agent()
```

## Example 2: Configuration Management

```python
import pynlm

# Create and modify configuration
config = pynlm.createDefaultConfig()

# Use Pythonic configuration API
print("Current configuration keys:", config.getKeys())

# Set various configuration parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("brain.connection_probability", 0.1)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("neuromod.dopamine.scale", 2.0)

# Create brain with enhanced configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.neuron_count} neurons")
print(f"STDP learning rate: {config.get('plasticity.stdp.learning_rate')}")
```

## Example 3: Error Handling and Exception Management

```python
import pynlm

def safe_simulation(brain, steps=100):
    """Enhanced error handling for brain simulation"""
    try:
        spike_counts = []
        for step in range(steps):
            try:
                brain.step(step)
                spike_counts.append(brain.getTotalSpikeCount())
            except pynlm.SimulationStepException as e:
                print(f"Simulation step {step} error: {e}")
                break
            except Exception as e:
                print(f"Unexpected error at step {step}: {e}")
                raise
        
        return spike_counts
    except Exception as e:
        print(f"Simulation failed: {e}")
        raise

# Usage with error handling
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

try:
    spikes = safe_simulation(brain, 200)
    print(f"Simulation completed successfully with {len(spikes)} steps")
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
except pynlm.BrainInitializationException as e:
    print(f"Brain initialization failed: {e}")
except pynlm.ConfigurationException as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Example 4: Enhanced Brain Statistics

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enhanced brain statistics using Pythonic API
print("=== Enhanced Brain Statistics ===")
print(f"Total Neurons: {brain.neuron_count}")
print(f"Total Synapses: {brain.synapse_count}")
print(f"Active Neurons: {brain.active_neurons}")
print(f"Firing Neurons: {brain.firing_neurons}")
print(f"Average Firing Rate: {brain.average_firing_rate:.3f} Hz")
print(f"Total Spike Count: {brain.getTotalSpikeCount()}")
print(f"Development Stage: {brain.development_stage}")
print(f"Excitatory/Inhibitory Ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Get formatted status string
print("\n=== Formatted Status ===")
print(brain.get_status())
```

## Example 5: Sensory Processing Pipeline

```python
import pynlm

# Setup brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("=== Sensory Processing Pipeline ===")
print(f"World: {world.getWidth()}x{world.getHeight()}")
print(f"Vision: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")

# Run sensory processing cycle
for step in range(10):
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    print(f"\nStep {step}:")
    print(f"  Vision signal size: {len(percept.getVision())}")
    print(f"  Touch signal size: {len(percept.getTouch())}")
    print(f"  Internal signals: {len(percept.getInternal())}")
    print(f"  Proprioception: {len(percept.getProprioception())}")
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Generate action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Example 6: Agent Development Tracking

```python
import pynlm

# Create and initialize agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable development tracking
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("=== Agent Development Tracking ===")

# Track development over time
for step in range(20):
    world.update(0.1)
    
    # Get current development state
    current_stage = agent.getDevelopmentalStage()
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    neuromodulation = agent.getNeuromodulationLevel()
    prediction_error = agent.getPredictionError()
    
    print(f"Step {step}: Stage={current_stage}, "
          f"Curiosity={curiosity_level:.3f}, "
          f"Novelty={novelty_level:.3f}, "
          f"NE={neuromodulation:.3f}, "
          f"PredErr={prediction_error:.3f}")
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward and update development
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

print(f"\nFinal agent statistics:\n{agent.get_episode_stats()}")
```

## Example 7: Simulation with Enhanced Monitoring

```python
import pynlm
import time

def enhanced_simulation(brain, world, agent, steps, timestep=0.1):
    """Enhanced simulation with comprehensive monitoring"""
    start_time = time.time()
    
    print(f"Starting enhanced simulation with {steps} steps...")
    print("Step, Time, Neurons, Firing, Rate, Curiosity, Novelty, DevStage")
    
    for step in range(steps):
        # Update world
        world.update(timestep)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Generate action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(timestep)
        
        # Print monitoring data (every 50 steps)
        if step % 50 == 0:
            elapsed = time.time() - start_time
            print(f"{step}, {elapsed:.2f}, "
                  f"{brain.neuron_count}, {brain.firing_neurons}, "
                  f"{brain.average_firing_rate:.2f}, "
                  f"{agent.getCuriosityLevel():.3f}, "
                  f"{agent.getNoveltyLevel():.3f}, "
                  f"{agent.getDevelopmentalStage()}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\nSimulation completed in {duration:.2f} seconds")
    print(f"Average time per step: {duration/steps*1000:.2f} ms")
    
    return {
        'total_time': duration,
        'steps_performed': steps,
        'final_spike_count': brain.getTotalSpikeCount(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'final_dev_stage': agent.getDevelopmentalStage()
    }

# Run enhanced simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableRewardModulation(True)

# Run enhanced simulation
results = enhanced_simulation(brain, world, agent, 200, 0.1)
print(f"\nSimulation results: {results}")
```

## Example 8: Configuration Validation and Testing

```python
import pynlm

def validate_configuration(config_dict):
    """Validate configuration parameters for brain creation"""
    print("=== Configuration Validation ===")
    
    # Required configuration keys with defaults
    defaults = {
        'brain.neuron_count': 1000,
        'brain.synapse_density': 0.1,
        'brain.connection_probability': 0.05,
        'plasticity.stdp.enable': True,
        'plasticity.hebbian.enable': True,
        'neuromod.dopamine.scale': 1.0,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True
    }
    
    # Validate provided configuration
    for key, value in config_dict.items():
        print(f"Setting {key} = {value}")
    
    # Create and validate brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Validate brain properties
    print(f"\nBrain validation:")
    print(f"  ✓ Created successfully with {brain.neuron_count} neurons")
    print(f"  ✓ Has {brain.getTotalSynapseCount()} synapses")
    print(f"  ✓ Development stage: {brain.development_stage}")
    print(f"  ✓ E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return brain

# Example configuration validation
config = {
    'brain.neuron_count': 500,
    'brain.synapse_density': 0.15,
    'plasticity.stdp.enable': True,
    'neuromod.curiosity.enable': True
}

validated_brain = validate_configuration(config)
```

## Example 9: Error Recovery and Resilience

```python
import pynlm

def resilient_simulation(brain, world, agent, steps):
    """Simulation with error recovery and resilience"""
    successful_steps = 0
    failed_steps = 0
    recovery_attempts = 0
    
    print("Starting resilient simulation...")
    
    for step in range(steps):
        try:
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step with error handling
            brain.step(step)
            
            # Generate and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply neuromodulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            successful_steps += 1
            
        except pynlm.SimulationStepException as e:
            print(f"Step {step} simulation error: {e}")
            failed_steps += 1
            recovery_attempts += 1
            
            # Attempt recovery
            try:
                agent.reset()
                brain.reset()
                world.reset()
                print(f"  Recovery attempted")
            except Exception as recovery_error:
                print(f"  Recovery failed: {recovery_error}")
                
        except pynlm.ConfigurationException as e:
            print(f"Configuration error at step {step}: {e}")
            failed_steps += 1
            break
            
        except Exception as e:
            print(f"Unexpected error at step {step}: {e}")
            failed_steps += 1
            recovery_attempts += 1
            
            # Attempt recovery
            try:
                agent.reset()
                brain.reset()
                world.reset()
                print(f"  Recovery attempted")
            except Exception as recovery_error:
                print(f"  Recovery failed: {recovery_error}")
                break
    
    print(f"\nResilient simulation results:")
    print(f"  Successful steps: {successful_steps}")
    print(f"  Failed steps: {failed_steps}")
    print(f"  Recovery attempts: {recovery_attempts}")
    print(f"  Success rate: {successful_steps/(successful_steps + failed_steps)*100:.1f}%")
    
    return {
        'successful_steps': successful_steps,
        'failed_steps': failed_steps,
        'recovery_attempts': recovery_attempts,
        'success_rate': successful_steps/(successful_steps + failed_steps) if (successful_steps + failed_steps) > 0 else 0
    }

# Run resilient simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

results = resilient_simulation(brain, world, agent, 100)
```

## Example 10: Performance Benchmarking

```python
import pynlm
import time

def benchmark_simulation(brain, world, agent, steps, repetitions=5):
    """Benchmark simulation performance"""
    print(f"Benchmarking simulation with {steps} steps over {repetitions} repetitions...")
    
    times = []
    spike_counts = []
    
    for rep in range(repetitions):
        print(f"  Repetition {rep + 1}/{repetitions}...")
        
        # Reset for each repetition
        brain.reset()
        world.reset()
        agent.reset()
        
        # Run simulation
        start_time = time.time()
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        end_time = time.time()
        duration = end_time - start_time
        
        times.append(duration)
        spike_counts.append(brain.getTotalSpikeCount())
        
        print(f"    Duration: {duration:.3f}s, Spikes: {brain.getTotalSpikeCount()}")
    
    # Calculate statistics
    avg_time = sum(times) / len(times)
    min_time = min(times)
    max_time = max(times)
    avg_spikes = sum(spike_counts) / len(spike_counts)
    
    print(f"\n=== Benchmark Results ===")
    print(f"Average time per repetition: {avg_time:.3f}s")
    print(f"Time per step (avg): {avg_time/steps*1000:.2f}ms")
    print(f"Time per step (min): {min_time/steps*1000:.2f}ms")
    print(f"Time per step (max): {max_time/steps*1000:.2f}ms")
    print(f"Average spike count: {avg_spikes:.1f}")
    print(f"Spike count std dev: {(sum((s - avg_spikes)**2 for s in spike_counts)/len(spike_counts))**0.5:.1f}")
    print(f"Performance (steps/sec): {steps/avg_time:.1f}")
    
    return {
        'average_time': avg_time,
        'min_time': min_time,
        'max_time': max_time,
        'average_spikes': avg_spikes,
        'spike_std_dev': (sum((s - avg_spikes)**2 for s in spike_counts)/len(spike_counts))**0.5,
        'performance_hz': steps/avg_time,
        'times': times,
        'spike_counts': spike_counts
    }

# Run benchmark
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

benchmark_results = benchmark_simulation(brain, world, agent, 100, 3)
```

## Summary of Enhanced Features

These examples demonstrate the enhanced Python API features:

1. **Pythonic Properties**: Quick access to brain and agent properties
2. **Enhanced Documentation**: Comprehensive docstrings and examples
3. **Error Handling**: Custom exception types and resilient error recovery
4. **Convenience Methods**: Enhanced simulation and status reporting
5. **Performance Monitoring**: Comprehensive benchmarking and statistics
6. **Configuration Management**: Flexible and validated configuration
7. **Development Tracking**: Detailed monitoring of agent development
8. **Sensory Processing**: Comprehensive sensory pipeline examples
9. **Error Recovery**: Resilient simulation with automatic recovery
10. **Performance Benchmarking**: Detailed performance analysis

These enhancements make NLM more accessible, robust, and powerful for Python users while maintaining the core scientific functionality of the neural simulation framework.
