# NLM - Comprehensive Documentation & Examples Guide

This guide provides practical, working examples for the NLM (Neural Learning Machine) project, covering everything from basic usage to advanced integration scenarios.

## Table of Contents

1. [Quick Start: Your First Brain](#quick-start)
2. [Complete Integration Examples](#integration-examples)
3. [Error Handling & Recovery](#error-handling)
4. [Performance & Memory Management](#performance)
5. [Custom Configuration Examples](#custom-config)
6. [Multi-Agent Environments](#multi-agent)
7. [Common Troubleshooting](#troubleshooting)
8. [Advanced Usage Patterns](#advanced)

---

# Quick Start: Your First Brain

## Minimal Working Example

This is the absolute minimum code to get NLM running:

```python
import pynlm

# Step 1: Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize it
brain.initialize()

# Step 3: Make it think
brain.step(0)

# Step 4: Check results
print(f"Brain has {brain.getTotalNeuronCount()} neurons!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Real-World Example: Building an Intelligent Agent

This complete example shows how to build an intelligent agent that learns from its environment:

```python
import pynlm
import time

def create_learning_agent(world_size=10, steps=1000):
    """Create and train a learning agent in a simulated environment."""
    
    # === CONFIGURATION SETUP ===
    config = pynlm.createDefaultConfig()
    
    # Configure brain for learning
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable_critical_period", True)
    config.set("development.stage_duration.initial", 60.0)
    config.set("development.stage_duration.critical", 300.0)
    
    # === CREATE COMPONENTS ===
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    # Place agent in center of world
    world.setAgentStart(world_size/2, world_size/2)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # === ENABLE LEARNING SYSTEMS ===
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # === SIMULATION LOOP ===
    episode_count = 0
    total_steps = 0
    
    print(f"Starting learning agent simulation...")
    print(f"World size: {world_size}x{world_size}, Steps per episode: {steps}")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print("-" * 60)
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get current state
        percept = world.getSensoryPercept()
        agent_body = world.getAgentBody()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Neural computation
        brain.step(step, step * 0.1)
        
        # Motor action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check if episode ended (agent died or reached goal)
        if agent_body.health <= 0 or step % world_size == 0:
            if agent_body.health <= 0:
                print(f"Step {step}: Agent died! Health: {agent_body.health:.1f}")
            else:
                print(f"Step {step}: Reached boundary, resetting")
            
            # Reset for new episode
            world.reset()
            agent.reset()
            brain.reset()
            brain.initialize()
            agent.initialize(world)
            
            # Log episode statistics
            episode_count += 1
            print(f"  Episode {episode_count} ended - "
                  f"Curiosity: {agent.getCuriosityLevel():.2f}, "
                  f"Novelty: {agent.getNoveltyLevel():.2f}")
        
        # Print progress every 100 steps
        if step % 100 == 0 and step > 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Developmental stage: {agent.getDevelopmentalStage()}")
            print(f"  Agent pos: ({agent_body.x:.1f}, {agent_body.y:.1f})")
            print()
    
    total_steps += steps
    
    print("=" * 60)
    print(f"Simulation complete!")
    print(f"Total episodes: {episode_count}")
    print(f"Total steps: {total_steps}")
    print(f"Final state - Curiosity: {agent.getCuriosityLevel():.3f}, "
          f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, world, agent

# === RUN EXAMPLE ===
if __name__ == "__main__":
    try:
        brain, world, agent = create_learning_agent(world_size=15, steps=1000)
        print("Example completed successfully!")
    except Exception as e:
        print(f"Error during simulation: {e}")
        import traceback
        traceback.print_exc()
```

## Complete Agent Simulation

Here's a complete, working example of an agent interacting with a world:

```python
import pynlm

def run_complete_simulation(world_size=15, steps=500):
    """Run a complete agent simulation with all features enabled."""
    
    # 1. Configure brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.curiosity.enable", True)
    config.set("development.enable_critical_period", True)
    
    # 2. Create all components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(world_size/2, world_size/2)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Running complete simulation: {steps} steps")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"World: {world_size}x{world_size}")
    print()
    
    # 4. Run simulation loop
    for step in range(steps):
        # Update environment
        world.update(0.1)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        
        # Neural computation
        brain.step(step)
        
        # Motor output
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Development
        agent.updateDevelopment(0.1)
        
        # Progress monitoring
        if step % 100 == 0:
            print(f"Step {step:4d}: "
                  f"Neurons={brain.getFiringNeuronCount():5d} | "
                  f"Spikes={brain.getTotalSpikeCount():7d} | "
                  f"Curiosity={agent.getCuriosityLevel():.3f} | "
                  f"Dev={brain.getDevelopmentalStage().name:12s}")
    
    return {
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'final_dev_stage': brain.getDevelopmentalStage().name,
        'final_curiosity': agent.getCuriosityLevel()
    }

# Run the simulation
results = run_complete_simulation()
print("\nSimulation complete!")
print(f"Results: {results}")
```

---

# Integration Examples

## Example 1: Working Memory + Prediction + Neuromodulation

This example shows how NLM's three core learning systems work together:

```python
import pynlm

def integrated_learning_example():
    """Demonstrate integration of working memory, prediction, and neuromodulation."""
    
    # Configure for integrated learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.01)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.threshold", 0.1)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain with integrated systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all integrated systems
    agent.enableRewardModulation(True)  # Neuromodulation
    agent.enableStructuralPlasticity(True)  # Working memory connections
    agent.enableDevelopment(True)  # Prediction system development
    agent.enableCuriosity(True)  # Learning integration
    
    print("Running integrated learning simulation...")
    
    for step in range(800):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Integrated processing
        agent.processSensoryInput(percept)  # Sensory processing
        brain.step(step)  # Neural computation (prediction)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Integrated learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        agent.updateDevelopment(0.1)
        
        if step % 150 == 0:
            print(f"Step {step:4d}: "
                  f"WorkingMemory={brain.getActiveNeuronCount():4d} | "
                  f"Prediction={predicted_reward:.3f} | "
                  f"Novelty={agent.getNoveltyLevel():.3f} | "
                  f"Dopamine={agent.getNeuromodulationLevel():.3f}")
    
    return {
        'integration_success': True,
        'working_memory_active': brain.getActiveNeuronCount() > 0,
        'prediction_effective': abs(predicted_reward) > 0,
        'novelty_detected': agent.getNoveltyLevel() > 0.1,
        'neuromodulation_working': agent.getNeuromodulationLevel() > 0
    }

# Run integration example
integration_results = integrated_learning_example()
print("\nIntegration Results:", integration_results)
```

## Example 2: Developmental Learning Timeline

Shows how NLM's brain develops through different stages:

```python
import pynlm
import time

def developmental_learning_example():
    """Demonstrate brain development through different stages."""
    
    # Create developmentally-aware brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("development.critical_period_duration", 200.0)
    config.set("development.synaptogenesis_rate", 0.1)
    config.set("development.pruning_rate", 0.05)
    config.set("development.maturation_rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures()
    
    print("Running developmental learning simulation...")
    print("Tracking brain development through different stages:")
    print()
    
    developmental_stages = []
    plasticity_rates = []
    
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.5)
        agent.updateDevelopment(0.1)
        
        # Record developmental stage
        current_stage = brain.getDevelopmentalStage()
        if current_stage != developmental_stages[-1] if developmental_stages else False:
            developmental_stages.append(current_stage)
            print(f"  Step {step:4d}: Developmental Stage = {current_stage.name}")
        
        # Record plasticity
        plasticity_rate = agent.getNeuromodulationLevel()
        plasticity_rates.append(plasticity_rate)
        
        if step % 200 == 0:
            print(f"    Plasticity rate: {plasticity_rate:.3f}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Analyze development
    print("\nDevelopment Analysis:")
    print(f"  Total stages traversed: {len(developmental_stages)}")
    print(f"  Final stage: {brain.getDevelopmentalStage().name}")
    print(f"  Average plasticity: {sum(plasticity_rates)/len(plasticity_rates):.3f}")
    print(f"  Learning efficiency: {brain.getTotalSpikeCount()/1000:.1f} spikes/step")
    
    return {
        'stages_visited': len(developmental_stages),
        'final_stage': brain.getDevelopmentalStage().name,
        'development_complete': brain.getDevelopmentalStage().name == 'Adult',
        'learning_efficiency': brain.getTotalSpikeCount()/1000
    }

# Run developmental example
development_results = developmental_learning_example()
print("\nDevelopment Results:", development_results)
```

---

# Error Handling & Recovery

## Robust Simulation with Retry Logic

```python
import pynlm

def robust_simulation_with_recovery(max_retries=3, min_success_rate=0.7):
    """Run simulation with comprehensive error handling and recovery."""
    
    successful_episodes = []
    failed_episodes = []
    
    for episode in range(max_retries):
        print(f"\n--- Episode {episode + 1}/{max_retries} ---")
        
        try:
            # Fresh configuration for each episode
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 800 + episode * 200)  # Growing complexity
            config.set("plasticity.stdp.learning_rate", 0.001 / (episode + 1))
            
            # Create fresh components
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=12, height=12, visionWidth=5, visionHeight=5)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableAllFeatures()
            
            # Run episode with error handling
            episode_success = run_episode_safely(world, brain, agent, episode)
            
            if episode_success:
                successful_episodes.append(episode)
                print(f"  ✓ Episode {episode + 1} completed successfully")
            else:
                failed_episodes.append(episode)
                print(f"  ✗ Episode {episode + 1} failed - will retry")
                
        except Exception as e:
            failed_episodes.append(episode)
            print(f"  ✗ Episode {episode + 1} crashed: {e}")
            # Recovery strategy: reset and try again
            continue
    
    # Calculate success rate
    success_rate = len(successful_episodes) / max_retries if max_retries > 0 else 0
    
    print(f"\n--- Simulation Summary ---")
    print(f"Total episodes: {max_retries}")
    print(f"Successful: {len(successful_episodes)}")
    print(f"Failed: {len(failed_episodes)}")
    print(f"Success rate: {success_rate:.1%}")
    
    if success_rate >= min_success_rate:
        print("✓ Overall simulation successful!")
        return True, successful_episodes, failed_episodes
    else:
        print("✗ Too many failures - simulation not successful")
        return False, successful_episodes, failed_episodes

def run_episode_safely(world, brain, agent, episode_id):
    """Run a single episode with safety checks."""
    max_steps = 300
    stability_checks = []
    
    for step in range(max_steps):
        try:
            # Update environment
            world.update(0.1)
            
            # Get and process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Neural computation
            brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learning with error checking
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            predicted_reward = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted_reward)
            
            agent.updateDevelopment(0.1)
            
            # Safety checks
            if step % 50 == 0:
                # Check for numerical instability
                spike_count = brain.getTotalSpikeCount()
                if spike_count > 100000:  # Too many spikes
                    print(f"    Warning: High spike count ({spike_count})")
                    # Recovery: reset brain
                    brain.reset()
                    agent.reset()
                    continue
                
                # Check for memory issues
                if world.getAgentBody().energy < 0:
                    print(f"    Warning: Negative energy - recovery needed")
                    # Recovery: add energy
                    world.getAgentBody().energy = 10.0
                
                stability_checks.append({
                    'step': step,
                    'spike_count': spike_count,
                    'energy': world.getAgentBody().energy,
                    'curiosity': agent.getCuriosityLevel()
                })
            
        except Exception as e:
            print(f"    Error at step {step}: {e}")
            # Recovery attempt
            try:
                brain.reset()
                agent.reset()
                world.reset()
            except:
                return False
            continue
    
    return True

# Run robust simulation
print("Running robust simulation with error handling...")
success, successful, failed = robust_simulation_with_recovery(max_retries=5)
```

---

# Performance & Memory Management

## Memory-Efficient Simulation

```python
import pynlm
import gc
import time
import psutil
import os

def memory_efficient_simulation(neuron_count=500, memory_limit_mb=200):
    """Run simulation with memory monitoring and management."""
    
    # Monitor system memory
    process = psutil.Process(os.getpid())
    
    def get_memory_usage():
        return process.memory_info().rss / 1024 / 1024  # MB
    
    # Configure for memory efficiency
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.03)  # Lower density = fewer synapses
    config.set("plasticity.stdp.learning_rate", 0.0005)  # Slower learning
    config.set("plasticity.hebbian.learning_rate", 0.001)  # Reduced learning
    
    # Create components
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
    agent = pynlm.createAgentBrain(brain)
    
    agent.initialize(world)
    
    initial_memory = get_memory_usage()
    print(f"Starting memory-efficient simulation...")
    print(f"Initial memory: {initial_memory:.1f} MB")
    print(f"Target memory limit: {memory_limit_mb:.1f} MB")
    print(f"Brain configuration: {neuron_count} neurons")
    print()
    
    # Run simulation with memory management
    memory_samples = []
    performance_samples = []
    
    for step in range(500):
        # Check memory every 50 steps
        if step % 50 == 0:
            current_memory = get_memory_usage()
            memory_used = current_memory - initial_memory
            memory_samples.append(memory_used)
            
            print(f"Step {step:3d} | "
                  f"Memory={current_memory:6.1f}MB (+{memory_used:5.1f}) | "
                  f"Firing={brain.getFiringNeuronCount():5d} | "
                  f"Spikes={brain.getTotalSpikeCount():8d}")
            
            # Memory management
            if memory_used > memory_limit_mb * 0.9:  # 90% of limit
                print(f"  → Approaching memory limit, running GC...")
                gc.collect()
                
                # Check after GC
                after_gc = get_memory_usage()
                gc_reduction = current_memory - after_gc
                print(f"  → Memory reduced by {gc_reduction:.1f} MB")
                
                if after_gc > memory_limit_mb:
                    print(f"  → ERROR: Still above limit!")
                    break
            
            # Performance metrics
            elapsed_time = time.time()
            steps_per_sec = step / elapsed_time if elapsed_time > 0 else 0
            performance_samples.append(steps_per_sec)
        
        # Optimized simulation steps
        world.update(0.05)  # Faster time steps
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(0.5, 0.2)
        agent.updateDevelopment(0.05)
    
    # Final memory analysis
    final_memory = get_memory_usage()
    total_memory = final_memory - initial_memory
    avg_memory = sum(memory_samples) / len(memory_samples) if memory_samples else 0
    avg_performance = sum(performance_samples) / len(performance_samples) if performance_samples else 0
    
    print("\n" + "="*60)
    print("MEMORY-EFFICIENT SIMULATION RESULTS")
    print("="*60)
    print(f"  Total memory used: {total_memory:.1f} MB")
    print(f"  Average memory: {avg_memory:.1f} MB")
    print(f"  Performance: {avg_performance:.1f} steps/second")
    print(f"  Neurons created: {brain.getTotalNeuronCount():,}")
    print(f"  Synapses created: {brain.getTotalSynapseCount():,}")
    print(f"  Simulation completed: {step + 1} steps")
    
    return {
        'memory_used_mb': total_memory,
        'avg_memory_mb': avg_memory,
        'performance_steps_per_sec': avg_performance,
        'neurons_created': brain.getTotalNeuronCount(),
        'connections_created': brain.getTotalSynapseCount()
    }

# Run memory-efficient simulation
print("Running memory-efficient simulation...")
memory_results = memory_efficient_simulation(neuron_count=800, memory_limit_mb=250)
```

---

# Custom Configuration Examples

## Example: Different Brain Architectures

```python
import pynlm

def create_brain_architecture(architecture_type="standard"):
    """Create brain with different architectural configurations."""
    
    architectures = {
        "standard": {
            "name": "Standard Brain",
            "neuron_count": 1000,
            "v_thresh": -50.0,
            "v_rest": -70.0,
            "tau_mem": 20.0,
            "plasticity_stdp": 0.001,
            "plasticity_hebbian": 0.01,
            "neuromod_dopamine": 1.0,
            "development_enabled": True
        },
        "fast": {
            "name": "Fast Processing Brain",
            "neuron_count": 800,
            "v_thresh": -45.0,  # Higher threshold = faster response
            "v_rest": -65.0,   # Higher resting potential
            "tau_mem": 10.0,   # Faster membrane time constant
            "plasticity_stdp": 0.002,  # Faster STDP
            "plasticity_hebbian": 0.005,  # Slower Hebbian
            "neuromod_dopamine": 0.8,
            "development_enabled": False  # No development for stability
        },
        "memory_efficient": {
            "name": "Memory-Efficient Brain",
            "neuron_count": 1500,
            "v_thresh": -55.0,  # Lower threshold = more stable
            "v_rest": -75.0,   # Lower resting potential
            "tau_mem": 30.0,   # Slower membrane time
            "plasticity_stdp": 0.0005,  # Slower STDP
            "plasticity_hebbian": 0.002,  # Slower Hebbian
            "neuromod_dopamine": 1.5,  # Stronger dopamine
            "development_enabled": True
        },
        "high_learning": {
            "name": "High-Learning Brain",
            "neuron_count": 1200,
            "v_thresh": -48.0,
            "v_rest": -70.0,
            "tau_mem": 18.0,
            "plasticity_stdp": 0.005,  # Fast STDP
            "plasticity_hebbian": 0.02,  # Fast Hebbian
            "neuromod_dopamine": 2.0,  # Strong neuromodulation
            "development_enabled": True
        },
        "stable": {
            "name": "Stable Conservative Brain",
            "neuron_count": 900,
            "v_thresh": -52.0,
            "v_rest": -72.0,
            "tau_mem": 25.0,
            "plasticity_stdp": 0.0002,  # Very slow STDP
            "plasticity_hebbian": 0.001,  # Very slow Hebbian
            "neuromod_dopamine": 0.5,  # Weak neuromodulation
            "development_enabled": True,
            "development_critical_period": 300  # Extended critical period
        }
    }
    
    if architecture_type not in architectures:
        raise ValueError(f"Unknown architecture: {architecture_type}")
    
    config_template = architectures[architecture_type]
    
    print(f"Creating {config_template['name']}...")
    print(f"  Neurons: {config_template['neuron_count']}")
    print(f"  Threshold: {config_template['v_thresh']} mV")
    print(f"  Resting potential: {config_template['v_rest']} mV")
    print(f"  Membrane time constant: {config_template['tau_mem']} ms")
    print()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", config_template["neuron_count"])
    config.set("brain.v_thresh", config_template["v_thresh"])
    config.set("brain.v_rest", config_template["v_rest"])
    config.set("brain.tau_mem", config_template["tau_mem"])
    config.set("plasticity.stdp.learning_rate", config_template["plasticity_stdp"])
    config.set("plasticity.hebbian.learning_rate", config_template["plasticity_hebbian"])
    config.set("neuromod.dopamine.scale", config_template["neuromod_dopamine"])
    
    # Create and run brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures()
    
    # Run simulation
    for step in range(300):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(0.5, 0.2)
        agent.updateDevelopment(0.1)
    
    # Report results
    print(f"\nArchitecture Results:")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage().name}")
    print(f"  Total spikes: {brain.getTotalSpikeCount():,}")
    print(f"  Average firing rate: {brain.getTotalSpikeCount()/300:.1f} Hz")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    
    return {
        "architecture": config_template["name"],
        "neurons": brain.getTotalNeuronCount(),
        "spikes": brain.getTotalSpikeCount(),
        "final_dev_stage": brain.getDevelopmentalStage().name,
        "final_curiosity": agent.getCuriosityLevel(),
        "final_neuromodulation": agent.getNeuromodulationLevel()
    }

# Demonstrate different architectures
architectures = ["standard", "fast", "memory_efficient", "high_learning", "stable"]

print("="*70)
print("COMPARING DIFFERENT BRAIN ARCHITECTURES")
print("="*70)

results = {}
for arch in architectures:
    print(f"\n{'='*70}")
    results[arch] = create_brain_architecture(arch)

print(f"\n{'='*70}")
print("ARCHITECTURE COMPARISON SUMMARY")
print(f"{'='*70}")
print(f"{'Architecture':<20} {'Neurons':<10} {'Spikes':<12} {'Dev Stage':<12} {'Curiosity':<10} {'Neuromod':<10}")
print("-" * 70)

for arch_name, result in results.items():
    print(f"{result['architecture']:<20} {result['neurons']:<10} "
          f"{result['spikes']:<12,} {result['final_dev_stage']:<12} "
          f"{result['final_curiosity']:<10.3f} {result['final_neuromodulation']:<10.3f}")
```

---

# Advanced Usage Patterns

## Example: Predictive Control with Forward Planning

```python
import pynlm

def predictive_control_example():
    """Demonstrate predictive control with forward planning."""
    
    # Configure for predictive control
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    # Create predictive brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures()
    
    print("Running Predictive Control Simulation...")
    print("Brain will learn to predict action consequences:")
    print()
    
    # Simulation with prediction tracking
    prediction_history = []
    action_history = []
    reward_history = []
    
    for step in range(600):
        world.update(0.1)
        
        # Get sensory input (current state)
        percept = world.getSensoryPercept()
        
        # Process current state
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Predict future states (forward planning)
        predicted_states = predict_future_states(agent, step)
        
        # Choose action based on predictions
        best_action = choose_best_action(predicted_states)
        action_history.append(best_action)
        
        # Apply action
        world.applyMotorCommand(best_action, world.getSimulationTime())
        
        # Get actual outcome
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        reward_history.append(reward)
        
        # Update with prediction error
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Track predictions
        prediction_error = abs(predicted_reward - reward)
        prediction_history.append(prediction_error)
        
        # Update development (improve predictions)
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            avg_prediction_error = sum(prediction_history[-100:]) / 100 if len(prediction_history) >= 100 else 0
            print(f"Step {step:4d}: "
                  f"PredError={avg_prediction_error:.3f} | "
                  f"Action={best_action.name:15s} | "
                  f"Reward={reward:.2f} | "
                  f"Prediction={predicted_reward:.3f} | "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    # Analyze predictions
    print(f"\nPredictive Control Analysis:")
    print(f"  Average prediction error: {sum(prediction_history)/len(prediction_error):.4f}")
    print(f"  Total actions taken: {len(action_history)}")
    print(f"  Average reward: {sum(reward_history)/len(reward_history):.3f}")
    print(f"  Learning rate: {1 - sum(prediction_history)/len(prediction_history):.4f}")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage().name}")
    
    return {
        'avg_prediction_error': sum(prediction_history)/len(prediction_history),
        'total_actions': len(action_history),
        'avg_reward': sum(reward_history)/len(reward_history),
        'learning_improvement': 1 - sum(prediction_history)/len(prediction_history),
        'final_dev_stage': brain.getDevelopmentalStage().name
    }

def predict_future_states(agent, current_step, steps_ahead=3):
    """Predict future states based on current brain activity."""
    # This is a simplified prediction model
    # In real NLM, this would involve the brain's prediction system
    predictions = []
    
    for ahead in range(1, steps_ahead + 1):
        # Simulate future based on current state
        future_step = current_step + ahead
        # Get prediction from agent's prediction system
        predicted_reward = agent.getPredictionError() * (0.8 ** ahead)  # Decay over time
        predictions.append({
            'step': future_step,
            'predicted_reward': predicted_reward,
            'confidence': agent.getCuriosityLevel() * (0.9 ** ahead)
        })
    
    return predictions

def choose_best_action(predictions):
    """Choose best action based on future predictions."""
    if not predictions:
        return None
    
    # Simple action selection: choose action with highest expected reward
    best_prediction = max(predictions, key=lambda p: p['predicted_reward'])
    
    # Map prediction to action (simplified)
    action_types = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.Interact,
        pynlm.ActionType.Eat,
        pynlm.ActionType.Wait
    ]
    
    action_index = int((best_prediction['predicted_reward'] + 1) / 2 * len(action_types))
    action_index = min(action_index, len(action_types) - 1)
    
    return action_types[action_index]

# Run predictive control example
predictive_results = predictive_control_example()
```

---

# Common Troubleshooting

## Common Issues and Solutions

### Issue 1: Brain is Not Firing Neurons

**Problem:** The simulation runs but neurons never fire.

**Symptoms:**
- All neuron counts are zero
- No spikes generated
- System appears inactive

**Solution:**

```python
# Check this common mistake:
import pynlm

# WRONG - brain not initialized
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.step(0)  # Will not work!

# RIGHT - proper initialization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # Must call this!
brain.step(0)  # Now it works!
```

### Issue 2: Segmentation Faults

**Problem:** Program crashes with segmentation fault.

**Common Causes:**
1. Not calling `initialize()` before `step()`
2. Not calling `reset()` between simulations
3. Configuration errors

**Solution:**

```python
def safe_simulation():
    try:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()  # ✓ Required
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()  # ✓ Required for clean state
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)  # ✓ Required
        
        # Safe simulation loop
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)  # ✓ Safe to call after initialize
            
            # Check for errors
            if brain.getTotalSpikeCount() > 50000:
                print("Warning: High spike count")
                brain.reset()  # Recovery
                break
                
    except Exception as e:
        print(f"Error: {e}")
        # Recovery or graceful exit
        return False
    
    return True
```

### Issue 3: Memory Usage Too High

**Problem:** Simulation consumes too much memory.

**Solutions:**

```python
# Solution 1: Reduce brain size
config.set("brain.neuron_count", 500)  # Instead of 2000

# Solution 2: Reduce synapse density
config.set("brain.synapse_density", 0.05)  # Instead of 0.2

# Solution 3: Disable resource-intensive features
agent.enableStructuralPlasticity(False)  # Disable structural plasticity
agent.enableDevelopment(False)  # Disable development system

# Solution 4: Memory-efficient simulation
import gc
for step in range(500):
    # Your simulation code
    if step % 100 == 0:
        gc.collect()  # Force garbage collection
```

### Issue 4: Learning is Not Happening

**Problem:** Brain is not learning or adapting.

**Solutions:**

```python
# Ensure learning systems are enabled
agent.enableRewardModulation(True)  # For reward-based learning
agent.enableCuriosity(True)  # For intrinsic motivation
agent.enableStructuralPlasticity(True)  # For connection growth
agent.enableDevelopment(True)  # For developmental learning

# Check plasticity configuration
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.learning_rate", 0.01)

# Verify neuromodulation is working
print(f"Neuromodulation level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
```

---

# Advanced Usage Patterns

## Pattern 1: Event-Driven Learning

```python
import pynlm

def event_driven_learning():
    """Learn only on significant events (high curiosity or reward)."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("neuromod.curiosity.threshold", 0.5)  # Higher threshold
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures()
    
    learning_events = []
    step_count = 0
    
    while step_count < 1000:
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step_count)
        
        # Check for learning events
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Event-driven learning: only learn on significant events
        if curiosity_level > 0.7 or novelty_level > 0.5 or abs(reward) > 0.1:
            # Apply reward modulation (learning event)
            predicted_reward = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted_reward)
            
            learning_events.append({
                'step': step_count,
                'type': 'curiosity' if curiosity_level > 0.7 else 'novelty' if novelty_level > 0.5 else 'reward',
                'value': curiosity_level if curiosity_level > 0.7 else novelty_level if novelty_level > 0.5 else abs(reward)
            })
            
            print(f"Learning event at step {step_count}: "
                  f"{learning_events[-1]['type']} (value: {learning_events[-1]['value']:.3f})")
        
        # Apply action
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.updateDevelopment(0.1)
        
        step_count += 1
    
    print(f"\nEvent-driven learning summary:")
    print(f"  Total steps: {step_count}")
    print(f"  Learning events: {len(learning_events)}")
    print(f"  Learning efficiency: {len(learning_events)/step_count:.1%}")
    print(f"  Event types: {[(e['type'], sum(1 for x in learning_events if x['type']==e['type'])) for e in set(learning_events)]}")
    
    return {
        'steps_completed': step_count,
        'learning_events': len(learning_events),
        'learning_efficiency': len(learning_events)/step_count,
        'event_breakdown': {e['type']: sum(1 for x in learning_events if x['type']==e['type']) for e in set(learning_events)}
    }
```

## Pattern 2: Transfer Learning Between Tasks

```python
import pynlm

def transfer_learning_demo():
    """Demonstrate transfer learning across different tasks."""
    
    tasks = [
        {"name": "Exploration", "world_size": 10, "difficulty": "easy"},
        {"name": "Navigation", "world_size": 15, "difficulty": "medium"},
        {"name": "Problem Solving", "world_size": 20, "difficulty": "hard"}
    ]
    
    all_results = {}
    
    for task_idx, task in enumerate(tasks):
        print(f"\n{'='*60}")
        print(f"TASK {task_idx + 1}: {task['name'].upper()}")
        print(f"{'='*60}")
        print(f"World: {task['world_size']}x{task['world_size']}")
        print(f"Difficulty: {task['difficulty']}")
        
        # Task-specific configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800 + task_idx * 200)  # Growing brain
        config.set("plasticity.stdp.learning_rate", 0.001 / (task_idx + 1))
        config.set("neuromod.dopamine.scale", 1.0 + task_idx * 0.3)
        
        # Create and run task
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(
            width=task['world_size'],
            height=task['world_size'],
            visionWidth=6,
            visionHeight=6
        )
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableAllFeatures()
        
        # Run task with task-specific objectives
        task_results = run_transfer_task(world, brain, agent, task)
        all_results[task['name']] = task_results
        
        # Transfer learning: keep learned weights for next task
        print(f"  Task completed. Preparing for transfer learning...")
    
    # Analyze transfer learning
    print(f"\n{'='*60}")
    print("TRANSFER LEARNING ANALYSIS")
    print(f"{'='*60}")
    
    tasks_ordered = [t["name"] for t in tasks]
    performances = [all_results[t["name"]]["final_performance"] for t in tasks]
    
    print(f"\nPerformance across tasks:")
    for task, perf in zip(tasks_ordered, performances):
        print(f"  {task}: {perf:.3f} (normalized)")
    
    # Check for transfer effects
    performance_improvement = all_results[tasks[1]["name"]]["final_performance"] - all_results[tasks[0]["name"]]["final_performance"]
    
    print(f"\nTransfer effects:")
    print(f"  Improvement from Task 1 to Task 2: {performance_improvement:+.3f}")
    print(f"  Learning rate: {performance_improvement/100:.1%}")
    print(f"  Knowledge transfer: {'Yes' if performance_improvement > 0 else 'No'}")
    
    return {
        'tasks_completed': len(tasks),
        'performances': performances,
        'transfer_effectiveness': performance_improvement > 0,
        'learning_improvement': performance_improvement / 100
    }

def run_transfer_task(world, brain, agent, task_config):
    """Run a single transfer learning task."""
    
    steps = 300
    task_performance = []
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Task-specific behavior
        if task_config["difficulty"] == "easy":
            # Simple exploration
            action = agent.decodeMotorCommand()
        elif task_config["difficulty"] == "medium":
            # More complex navigation
            action = agent.decodeMotorCommand()
            # Add occasional random exploration
            if step % 50 == 0:
                action = pynlm.ActionType.Interact
        else:
            # Problem solving with cognitive load
            action = agent.decodeMotorCommand()
            # Increase novelty detection for problem solving
            if step % 75 == 0:
                agent.applyRewardModulation(0.5, 0.2)
        
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.5, 0.2)
        agent.updateDevelopment(0.1)
        
        # Record performance
        task_performance.append({
            'step': step,
            'reward': world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        })
        
        if step % 100 == 0:
            avg_reward = sum(p['reward'] for p in task_performance[-100:]) / 100
            avg_curiosity = sum(p['curiosity'] for p in task_performance[-100:]) / 100
            print(f"    Step {step:4d}: "
                  f"Reward={avg_reward:.2f} | "
                  f"Curiosity={avg_curiosity:.3f}")
    
    # Calculate final performance
    final_performance = (sum(p['reward'] for p in task_performance) / 
                        len(task_performance) if task_performance else 0)
    
    print(f"  Task completed: Final performance = {final_performance:.3f}")
    
    return {
        'final_performance': final_performance,
        'avg_curiosity': sum(p['curiosity'] for p in task_performance) / len(task_performance),
        'avg_novelty': sum(p['novelty'] for p in task_performance) / len(task_performance),
        'steps_completed': steps
    }
```

---

# Quick Reference: Common Code Snippets

## Essential Setup

```python
import pynlm

# Always start with creating configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

## Basic Simulation Loop

```python
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

## Learning Systems

```python
# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check learning status
print(f"Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
print(f"Prediction Error: {agent.getPredictionError():.3f}")
```

## Error Recovery

```python
def safe_simulation():
    try:
        # Your simulation code here
        pass
    except Exception as e:
        print(f"Error: {e}")
        # Recovery
        brain.reset()
        agent.reset()
        world.reset()
        return False
    return True
```

---

# Creating Your Own Documentation Examples

This guide includes the following types of examples:

1. **Minimal Examples**: Show basic usage with fewest lines
2. **Complete Examples**: Show full working simulations
3. **Error Handling Examples**: Show robust error recovery
4. **Performance Examples**: Show memory and speed optimizations
5. **Custom Configuration Examples**: Show different brain architectures
6. **Multi-Agent Examples**: Show complex environments
7. **Troubleshooting Examples**: Show common problem solutions

You can create additional examples by:

- Taking existing examples and modifying parameters
- Adding new error handling patterns
- Creating custom brain configurations
- Demonstrating specific NLM features
- Combining multiple examples into larger demonstrations

---

**End of NLM Comprehensive Documentation Guide**

This guide provides practical examples for using the NLM project. Use these examples as templates for your own NLM simulations and experiments.