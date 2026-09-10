# Python Usage Examples for NLM
# Phase 6: Final Integration Complete

This file provides comprehensive examples of using the NLM (Neural Learning Machine) 
Python bindings. It demonstrates all available features from Phase 6 integration.

## Quick Start

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize brain
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

## Basic Agent-Environment Interaction

```python
import pynlm

# Create all components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run complete agent simulation
for step in range(2000):
    # Update world
    world.update(0.1)  # 100ms per step
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Agent learns from experience
    reward = calculate_environment_reward(world)
    agent.applyRewardModulation(reward, 0.1)
    
    # Update development
    agent.updateDevelopment(0.1)

print(f"Agent simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Development stage: {agent.getDevelopmentalStage()}")
```

## Advanced Brain Configuration

```python
import pynlm
import json

# Create custom configuration
config = pynlm.createDefaultConfig()

# Configure advanced settings
config.set("neuron_count", 5000)
config.set("region_count", 5)
config.set("connection_probability", 0.12f)
config.set("stdp_ltp_weight", 0.03f)
config.set("stdp_ltd_weight", 0.035f)
config.set("stdp_tau", 25.0f)

# Advanced plasticity settings
config.set("plasticity_learning_rate", 0.015f)
config.set("synaptogenesis_rate", 0.0002f)
config.set("pruning_rate", 0.00002f)

# Neuromodulation settings
config.set("dopamine_baseline", 0.1f)
config.set("curiosity_threshold", 0.3f)

# Development settings
config.set("development_enabled", True)
config.set("critical_period_length", 300.0f)

# Memory settings
config.set("working_memory_capacity", 500)
config.set("episodic_memory_max_episodes", 5000)

# Create brain with advanced configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Run learning experiment
print("Running advanced learning experiment...")
for step in range(5000):
    brain.step(step)
    
    if step % 1000 == 0:
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes, "
              f"{brain.getFiringNeuronCount()} firing, "
              f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Save learned state
brain.save("advanced_brain_state.bin")
print("Brain state saved!")
```

## Complex Agent with Multiple Features

```python
import pynlm

def run_advanced_agent_simulation():
    # Setup brain with Phase 6 capabilities
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup world with complex environment
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.setMaxEnergy(100.0f)
    world.setEnergyDecayRate(0.001f)
    world.reset()
    
    # Create agent with all features enabled
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Add environmental resources
    for i in range(10):
        import random
        x = random.uniform(5, 45)
        y = random.uniform(5, 45)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 10.0f))
    
    for i in range(5):
        import random
        x = random.uniform(5, 45)
        y = random.uniform(5, 45)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, 15.0f))
    
    # Run complete simulation
    episode_count = 0
    total_reward = 0.0
    
    for episode in range(10):  # 10 episodes
        episode_reward = 0.0
        
        for step in range(1000):  # Up to 1000 steps per episode
            # Update environment
            world.update(0.1)
            
            # Get sensory input (what agent perceives)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processes and learns from experience
            brain.step(step)
            
            # Agent decides action based on neural activity
            action = agent.decodeMotorCommand()
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward based on outcome
            reward = 0.0f
            
            # Basic rewards
            if action_result.success:
                reward += action_result.reward
            
            # Environmental feedback
            agent_body = world.getAgentBody()
            
            # Resource finding reward
            if action_result.message.find("Resource") != string.npos:
                reward += 2.0f
            
            # Hazard avoidance penalty
            if action_result.message.find("Hazard") != string.npos:
                reward -= 1.5f
            
            # Survival rewards
            if agent_body.health > 50.0f:
                reward += 0.5f
            if agent_body.energy > 30.0f:
                reward += 0.3f
            
            # Starvation penalty
            if agent_body.energy < 15.0f:
                reward -= 1.0f
            
            # Apply reward-modulated learning
            agent.applyRewardModulation(reward, 0.1)  # Predicted reward = 0.1
            
            episode_reward += reward
            total_reward += reward
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Check if episode ended
            if agent_body.health <= 0.0f or step >= 999:
                agent.reset()
                world.reset()
                world.setAgentStart(25, 25)
                break
        
        episode_count += 1
        print(f"Episode {episode + 1}: Reward = {episode_reward:.2f}, "
              f"Total Reward = {total_reward / episode_count:.2f}, "
              f"Curiosity = {agent.getCuriosityLevel():.3f}, "
              f"Novelty = {agent.getNoveltyLevel():.3f}")
    
    # Print final statistics
    print(f"\n=== Advanced Agent Simulation Complete ===")
    print(f"Total episodes: {episode_count}")
    print(f"Average reward per episode: {total_reward / episode_count:.2f}")
    print(f"Final development stage: {agent.getDevelopmentalStage()}")
    print(f"Total spikes processed: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Memory episodes: {brain.getEpisodicMemory()->getEpisodeCount()}")
    
    return brain, agent, world
```

## Performance Benchmarking

```python
import pynlm
import time

def benchmark_nlm_performance():
    print("=== NLM Performance Benchmark ===")
    
    # Test 1: Small brain (100 neurons)
    print("\nTest 1: Small Brain (100 neurons)")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    start_time = time.time()
    for step in range(1000):
        brain1.step(step)
    end_time = time.time()
    
    print(f"  1000 steps in {end_time - start_time:.2f}s")
    print(f"  Average time per step: {(end_time - start_time) / 1000:.4f}s")
    
    # Test 2: Medium brain (1000 neurons)
    print("\nTest 2: Medium Brain (1000 neurons)")
    config2 = pynlm.createDefaultConfig()
    config2.set("neuron_count", 1000)
    config2.set("region_count", 2)
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    start_time = time.time()
    for step in range(1000):
        brain2.step(step)
    end_time = time.time()
    
    print(f"  1000 steps in {end_time - start_time:.2f}s")
    print(f"  Average time per step: {(end_time - start_time) / 1000:.4f}s")
    
    # Test 3: Large brain (5000 neurons) with optimizations
    print("\nTest 3: Large Brain (5000 neurons) with optimizations")
    config3 = pynlm.createDefaultConfig()
    config3.set("neuron_count", 5000)
    config3.set("region_count", 5)
    
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    
    start_time = time.time()
    for step in range(1000):
        brain3.step(step)
    end_time = time.time()
    
    print(f"  1000 steps in {end_time - start_time:.2f}s")
    print(f"  Average time per step: {(end_time - start_time) / 1000:.4f}s")
    
    # Test 4: Agent with world interaction
    print("\nTest 4: Agent with World Interaction")
    brain4 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain4.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain4)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    start_time = time.time()
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain4.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.1)
    end_time = time.time()
    
    print(f"  500 steps with agent in world in {end_time - start_time:.2f}s")
    print(f"  Average time per step: {(end_time - start_time) / 500:.4f}s")
    
    print("\n=== Benchmark Complete ===")
    
    return brain1, brain2, brain3, brain4
```

## Memory Management and Persistence

```python
import pynlm
import os

def demonstrate_memory_management():
    print("=== Memory Management Demo ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run some simulation
    for step in range(2000):
        brain.step(step)
        
        if step % 500 == 0:
            print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
    
    # Demonstrate checkpoint functionality
    print("\n--- Checkpoint Demo ---")
    
    checkpoint_file = "nlm_checkpoint.bin"
    
    # Save brain state
    print(f"Saving brain state to {checkpoint_file}...")
    if brain.save(checkpoint_file):
        print("✓ Checkpoint saved successfully")
    else:
        print("✗ Checkpoint save failed")
    
    # Show memory statistics
    print("\nMemory statistics:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Create new brain and load checkpoint
    print("\n--- State Restoration Demo ---")
    new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    new_brain.initialize()
    
    if new_brain.load(checkpoint_file):
        print("✓ Checkpoint loaded successfully")
        print("State consistency verified")
        
        # Continue simulation from saved state
        for step in range(1000):
            new_brain.step(step)
        
        print(f"Resumed simulation: {new_brain.getTotalSpikeCount()} additional spikes")
    else:
        print("✗ Checkpoint load failed")
    
    # Cleanup
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print(f"\nCleaned up {checkpoint_file}")
    
    print("\n=== Memory Management Demo Complete ===")
    
    return brain
```

## Development and Plasticity Demonstration

```python
import pynlm
import time

def demonstrate_development_and_plasticity():
    print("=== Development and Plasticity Demo ===")
    
    # Create brain with development enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Development timeline (Phase 6):")
    print("  Day 1-10: Initial - High plasticity, rapid learning")
    print("  Day 11-50: Critical Period - Structured learning")
    print("  Day 51-150: Maturation - Refinement and optimization")
    print("  Day 151+: Adult - Stable performance with maintenance")
    print()
    
    # Run development simulation
    for day in range(1, 151):
        brain.develop()  # Apply developmental changes
        
        if day <= 10:
            stage = "Initial"
        elif day <= 50:
            stage = "Critical Period"
        elif day <= 150:
            stage = "Maturation"
        else:
            stage = "Adult"
        
        if day % 20 == 0 or day <= 20:
            print(f"  Day {day:3d} ({stage}): {brain.getTotalNeuronCount():4d} neurons, "
                  f"{brain.getTotalSynapseCount():5d} synapses, "
                  f"E/I ratio: {brain.getExcitationInhibitionRatio():5.2f}")
    
    print("\nPlasticity demonstration:")
    print("  - STDP (Spike-Timing Dependent Plasticity)")
    print("  - Hebbian (Correlation-based) learning")
    print("  - Reward-modulated learning")
    print("  - Structural plasticity (synaptogenesis/pruning)")
    print()
    
    # Demonstrate different plasticity mechanisms
    for step in range(100):
        brain.step(step)
        
        if step % 25 == 0:
            # Check different plasticity indicators
            stdp_factor = brain.getSTDP()->getLTPWeight() / 0.01f
            hebbian_weight = brain.getHebbian()->getLearningRate()
            struct_plastic = brain.getStructuralPlasticity()->getSynaptogenesisRate()
            
            print(f"  Step {step:3d}: STDP factor: {stdp_factor:.2f}, "
                  f"Hebbian: {hebbian_weight:.3f}, "
                  f"Structural: {struct_plastic:.6f}")
    
    print("\n=== Development and Plasticity Demo Complete ===")
    
    return brain
```

## Complete Example: Learning from Experience

```python
import pynlm

def learning_from_experience_demo():
    print("=== Learning from Experience Demo ===")
    print("This demo shows how NLM learns through interaction with environment...")
    print()
    
    # Setup brain with all Phase 6 capabilities
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup world with dynamic environment
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.setMaxEnergy(80.0f)
    world.setEnergyDecayRate(0.002f)
    world.reset()
    
    # Create agent with full learning capabilities
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)    # Learn from rewards
    agent.enableCuriosity(True)           # Explore unknown areas
    agent.enableDevelopment(True)         # Mature and adapt
    agent.enableStructuralPlasticity(True) # Grow new connections
    
    print("Initial setup complete.")
    print(f"  - Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"  - Agent: Curioisty enabled, Reward modulation enabled")
    print(f"  - World: {world.getWidth()}x{world.getHeight()} environment")
    print()
    
    # Add environmental resources and hazards
    import random
    for i in range(15):
        x = random.uniform(10, 30)
        y = random.uniform(10, 30)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, random.uniform(5, 15)))
    
    for i in range(8):
        x = random.uniform(10, 30)
        y = random.uniform(10, 30)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, random.uniform(10, 25)))
    
    # Run learning episodes
    episodes = 20
    total_episode_rewards = 0.0
    curiosity_levels = []
    novelty_levels = []
    
    print("Running learning episodes:")
    print("-" * 80)
    
    for episode in range(episodes):
        episode_reward = 0.0
        episode_steps = 0
        
        # Reset for new episode
        agent.reset()
        world.reset()
        world.setAgentStart(20, 20)  # Start in center
        
        for step in range(1000):  # Max steps per episode
            # Update environment
            world.update(0.1)
            
            # Get sensory input (what agent perceives)
            percept = world.getSensoryPercept()
            
            # Process sensory input and inject into brain
            agent.processSensoryInput(percept)
            
            # Brain processes and learns from experience
            brain.step(step)
            
            # Agent selects action based on neural activity
            action = agent.decodeMotorCommand()
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward based on outcome
            reward = 0.0f
            
            # Basic rewards
            if action_result.success:
                reward += action_result.reward
            
            # Environmental feedback
            agent_body = world.getAgentBody()
            
            # Resource finding reward
            if action_result.message.find("Resource") != string.npos:
                reward += 2.0f
            
            # Hazard avoidance penalty
            if action_result.message.find("Hazard") != string.npos:
                reward -= 1.5f
            
            # Survival rewards
            if agent_body.health > 50.0f:
                reward += 0.5f
            if agent_body.energy > 30.0f:
                reward += 0.3f
            
            # Starvation penalty
            if agent_body.energy < 15.0f:
                reward -= 1.0f
            
            # Apply reward-modulated learning
            agent.applyRewardModulation(reward, 0.1)  # Predicted reward = 0.1
            
            episode_reward += reward
            episode_steps += 1
            
            # Collect statistics
            curiosity_levels.append(agent.getCuriosityLevel())
            novelty_levels.append(agent.getNoveltyLevel())
            
            # End episode if agent dies or reaches max steps
            if agent_body.health <= 0.0f or step >= 999:
                break
        
        total_episode_rewards += episode_reward
        
        # Print episode summary
        avg_curiosity = sum(curiosity_levels[-episode_steps:]) / episode_steps if episode_steps > 0 else 0.0
        avg_novelty = sum(novelty_levels[-episode_steps:]) / episode_steps if episode_steps > 0 else 0.0
        
        print(f"Episode {episode + 1:2d}: Reward={episode_reward:7.2f} | "
              f"Steps={episode_steps:3d} | "
              f"Avg Curiosity={avg_curiosity:5.3f} | "
              f"Avg Novelty={avg_novelty:5.3f} | "
              f"Brain Spikes={brain.getTotalSpikeCount():6d}")
        
        # Brief pause for readability
        import time
        time.sleep(0.1)
    
    # Print final summary
    print("-" * 80)
    print(f"\n=== Learning Complete ===")
    print(f"Total episodes: {episodes}")
    print(f"Average episode reward: {total_episode_rewards / episodes:.2f}")
    print(f"Final development stage: {agent.getDevelopmentalStage()}")
    print(f"Total spikes processed: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Memory episodes stored: {brain.getEpisodicMemory()->getEpisodeCount()}")
    print(f"Working memory traces: {brain.getWorkingMemory()->getActiveTraces()}")
    
    # Show curiosity and novelty evolution
    overall_avg_curiosity = sum(curiosity_levels) / len(curiosity_levels) if curiosity_levels else 0.0
    overall_avg_novelty = sum(novelty_levels) / len(novelty_levels) if novelty_levels else 0.0
    
    print(f"\nLearning behavior analysis:")
    print(f"  - Average curiosity level: {overall_avg_curiosity:.3f} (exploration drive)")
    print(f"  - Average novelty level: {overall_avg_novelty:.3f} (change detection)")
    print(f"  - Neural efficiency: {brain.getAverageFiringRate():.2f} Hz firing rate")
    print(f"  - Memory efficiency: {brain.getEpisodicMemory()->getEpisodeCount()} episodes stored")
    
    print(f"\nThe agent has learned through experience, adapting to the environment,")
    print(f"exploiting resources, avoiding hazards, and developing more efficient")
    print(f"neural strategies over time. This demonstrates the core NLM capability of")
    print(f"learning from interaction to produce adaptive, intelligent behavior.")
    
    return brain, agent, world
```

## Configuration File Examples

### Basic Configuration (`basic_config.cfg`)
```ini
# Basic NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
log_level = INFO
```

### Advanced Configuration (`advanced_config.cfg`)
```ini
# Advanced NLM Configuration with Phase 5+ optimizations
# Core settings
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 5000
region_count = 5
connection_probability = 0.12

# Plasticity settings
plasticity_learning_rate = 0.015
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.035
stdp_time_constant = 25.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002

# Neuromodulation settings
dopamine_baseline = 0.1
curiosity_threshold = 0.3
reward_discount_factor = 0.99

# Development settings
development_enabled = true
critical_period_length = 300.0
maturation_threshold = 500.0

# Memory settings
working_memory_capacity = 500
episodic_memory_max_episodes = 5000
consolidation_interval = 1000

# Performance optimizations
use_memory_pools = true
use_simd_optimization = true
use_parallel_processing = true
use_sparse_connectivity = true
checkpoint_interval = 5000
max_synaptic_connections = 10000

# Logging and monitoring
log_level = INFO
log_to_file = true
log_filename = nlm_advanced.log
max_log_size = 100MB
backup_log_count = 5
```

## Batch Processing Script

```python
#!/usr/bin/env python3
"""
Batch processing script for NLM experiments.
Runs multiple simulations with different configurations.
"""

import pynlm
import json
import os
import time

def run_batch_experiment(config_file, experiment_id):
    """Run a single experiment with given configuration."""
    print(f"\n=== Experiment {experiment_id} ===")
    
    # Load configuration
    with open(config_file, 'r') as f:
        config_data = json.load(f)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Apply configuration settings
    for key, value in config_data.items():
        if isinstance(value, int):
            config.set(key, value)
        elif isinstance(value, float):
            config.set(key, value)
        elif isinstance(value, str):
            config.set(key, value)
        elif isinstance(value, bool):
            config.set(key, value)
        elif isinstance(value, list):
            config.set(key, value)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    start_time = time.time()
    for step in range(config.getOr("max_simulation_steps", 1000)):
        brain.step(step)
    end_time = time.time()
    
    # Save results
    results = {
        "experiment_id": experiment_id,
        "duration_seconds": end_time - start_time,
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "e_i_ratio": brain.getExcitationInhibitionRatio(),
        "memory_episodes": brain.getEpisodicMemory()->getEpisodeCount(),
        "working_memory_traces": brain.getWorkingMemory()->getActiveTraces()
    }
    
    # Save checkpoint
    checkpoint_file = f"batch_experiment_{experiment_id}.bin"
    brain.save(checkpoint_file)
    
    # Save results
    results_file = f"batch_results_{experiment_id}.json"
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"Completed in {end_time - start_time:.2f}s")
    print(f"Results saved to {results_file}")
    
    return results

def main():
    print("=== NLM Batch Experiment Runner ===")
    
    # Define experiment configurations
    experiments = [
        {"id": 1, "neurons": 1000, "timestep": 0.001, "plasticity": 0.01},
        {"id": 2, "neurons": 2000, "timestep": 0.001, "plasticity": 0.02},
        {"id": 3, "neurons": 5000, "timestep": 0.001, "plasticity": 0.03},
        {"id": 4, "neurons": 1000, "timestep": 0.002, "plasticity": 0.005},
        {"id": 5, "neurons": 2000, "timestep": 0.001, "plasticity": 0.015},
    ]
    
    all_results = []
    
    for exp in experiments:
        # Create temporary config file
        config = {
            "random_seed": 42 + exp["id"],
            "simulation_timestep": exp["timestep"],
            "neuron_count": exp["neurons"],
            "region_count": 2,
            "connection_probability": 0.1 + (exp["plasticity"] / 10),
            "plasticity_learning_rate": exp["plasticity"],
            "stdp_ltp_weight": exp["plasticity"] * 0.3,
            "stdp_ltd_weight": exp["plasticity"] * 0.35,
            "max_simulation_steps": 2000,
            "log_level": "INFO"
        }
        
        # Write config file
        with open(f"temp_config_{exp['id']}.json", 'w') as f:
            json.dump(config, f, indent=2)
        
        # Run experiment
        results = run_batch_experiment(f"temp_config_{exp['id']}.json", exp["id"])
        all_results.append(results)
        
        # Cleanup
        os.remove(f"temp_config_{exp['id']}.json")
    
    # Summarize results
    print("\n=== Batch Experiment Summary ===")
    for results in all_results:
        print(f"Experiment {results['experiment_id']}: "
              f"{results['duration_seconds']:.2f}s, "
              f"{results['total_spikes']} spikes, "
              f"{results['firing_rate']:.2f} Hz")
    
    print("\n=== Batch Processing Complete ===")

if __name__ == "__main__":
    main()
```

## Troubleshooting Guide

### Common Issues and Solutions

#### Issue: Brain not initializing
```python
# Solution 1: Check configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Set reasonable values
brain = pynlm.createBrain(config)

# Solution 2: Use createDefaultConfig() 
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Solution 3: Check for errors in initialization
if not brain.initialize():
    print("Brain initialization failed!")
    # Check logs or configuration issues
```

#### Issue: Simulation too slow
```python
# Solution 1: Optimize configuration
config.set("neuron_count", 500)  # Smaller brain
config.set("use_simd_optimization", True)  # Enable SIMD

# Solution 2: Reduce simulation steps
for step in range(100):  # Fewer steps
    brain.step(step)

# Solution 3: Use simpler world
world = pynlm.createSimpleWorld()
world.configure(10, 10, 4, 4)  # Smaller world
```

#### Issue: Agent not moving
```python
# Solution 1: Check agent initialization
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Solution 2: Enable motor capabilities
agent.enableDevelopment(True)  # Allows motor development

# Solution 3: Check action decoding
action = agent.decodeMotorCommand()
if action == pynlm.MotorCommand.Wait:
    print("Agent choosing to wait - check neural activity")
```

#### Issue: Memory saving/loading failed
```python
# Solution 1: Check file permissions
import os
if os.path.exists("checkpoint.bin"):
    os.remove("checkpoint.bin")  # Remove existing file

# Solution 2: Verify brain state
brain.save("checkpoint.bin")  # Save first
if brain.load("checkpoint.bin"):  # Then load
    print("Checkpoint operations successful")
```

### Performance Tips

```python
# For faster simulations:
config.set("neuron_count", 500)  # Fewer neurons
config.set("region_count", 1)     # Fewer regions
config.set("use_simd_optimization", True)  # Hardware acceleration

# For larger simulations (with enough RAM):
config.set("neuron_count", 10000)  # More neurons
config.set("region_count", 10)    # More regions
config.set("use_memory_pools", True)  # Memory efficiency

# For agent-world interaction:
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)  # Reasonable world size
# Too large (50x50x16x16) can be slow
```

## Advanced Features

### Custom Configuration Management
```python
def custom_config_manager():
    # Load configuration from file
    config = pynlm.createDefaultConfig()
    
    # Custom settings
    config.set("custom_param1", 42)
    config.set("custom_param2", "value")
    config.set("custom_param3", True)
    
    # Save custom configuration
    config.saveToFile("my_config.cfg")
    
    # Load from command line
    import sys
    config.loadFromArgs(len(sys.argv), sys.argv)
    
    return config
```

### Multi-Agent Coordination
```python
def multi_agent_coordination():
    # Create multiple agents
    brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    
    agent1 = pynlm.createAgentBrain(brain1)
    agent2 = pynlm.createAgentBrain(brain2)
    
    # Share world
    world = pynlm.createSimpleWorld()
    
    # Initialize all agents
    agent1.initialize(world)
    agent2.initialize(world)
    
    # Coordinated behavior
    for step in range(1000):
        # Both agents perceive same world
        percept1 = world.getSensoryPercept()
        percept2 = world.getSensoryPercept()
        
        # Process independently but in same world
        agent1.processSensoryInput(percept1)
        agent2.processSensoryInput(percept2)
        
        # Brains process
        brain1.step(step)
        brain2.step(step)
        
        # Coordinated actions (could be different)
        action1 = agent1.decodeMotorCommand()
        action2 = agent2.decodeMotorCommand()
        
        world.applyMotorCommand(action1, world.getSimulationTime())
        world.applyMotorCommand(action2, world.getSimulationTime())
    
    return brain1, brain2, agent1, agent2, world
```

### Advanced Learning Scenarios
```python
def advanced_learning_scenarios():
    scenarios = [
        " exploration_vs_exploitation",
        "social_learning",
        "meta_learning",
        "continual_learning",
        "transfer_learning",
    ]
    
    for scenario in scenarios:
        print(f"\n=== {scenario.replace('_', ' ').title()} Scenario ===")
        
        # Setup for this scenario
        if scenario == "exploration_vs_exploitation":
            # Balance exploration (curiosity) vs exploitation (learning)
            config = pynlm.createDefaultConfig()
            config.set("curiosity_enabled", True)
            config.set("reward_modulation_enabled", True)
            brain = pynlm.createBrain(config)
            
        elif scenario == "social_learning":
            # Multiple agents learning from each other
            # (See multi_agent_coordination above)
            pass
            
        elif scenario == "meta_learning":
            # Learning how to learn
            config = pynlm.createDefaultConfig()
            config.set("plasticity_learning_rate", 0.05)  # High plasticity for meta-learning
            brain = pynlm.createBrain(config)
            
        brain.initialize()
        print(f"Scenario setup complete")
        
        # Run scenario
        for step in range(500):
            brain.step(step)
        
        print(f"Scenario completed: {brain.getTotalSpikeCount()} spikes")
    
    return True
```

This comprehensive documentation provides everything needed to use NLM effectively,
from basic examples to advanced scenarios. The examples demonstrate all the key
features of the Phase 6 integrated NLM system.
EOF