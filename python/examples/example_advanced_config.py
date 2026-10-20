# NLM Examples - Advanced Configuration

This document provides advanced configuration examples that go beyond basic setup and demonstrate how to optimize NLM for specific use cases.

## Table of Contents

1. [Custom Configuration Templates](#custom-configuration-templates)
2. [Region-Based Brain Organization](#region-based-brain-organization)
3. [Advanced Plasticity Configuration](#advanced-plasticity-configuration)
4. [Neuromodulation Tuning](#neuromodulation-tuning)
5. [Performance Optimization](#performance-optimization)
6. [Scalability Examples](#scalability-examples)
7. [Special Use Cases](#special-use-cases)

---

## Custom Configuration Templates

### Template 1: Fast Learning Agent

This template creates a brain optimized for rapid learning in dynamic environments.

```python
import pynlm

def create_fast_learning_config():
    """Create configuration for fast learning."""
    config = pynlm.createDefaultConfig()
    
    # Brain configuration
    config.set("brain.neuron_count", 500)  # More neurons for capacity
    config.set("brain.synapse_density", 0.2)  # Higher connectivity
    config.set("brain.v_thresh", -45.0)  # Lower threshold for easier firing
    config.set("brain.tau_mem", 15.0)  # Faster membrane time constant
    config.set("brain.tau_ref", 1.0)   # Shorter refractory period
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)  # Stronger STDP
    config.set("plasticity.stdp.tau_plus", 10.0)   # Faster LTP
    config.set("plasticity.stdp.tau_minus", 15.0)  # Slower LTD
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.003)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0002)  # Faster growth
    
    # Neuromodulation configuration
    config.set("neuromod.dopamine.scale", 1.5)  # Stronger reward signals
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.sensitivity", 2.0)  # Higher curiosity sensitivity
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.3)  # Lower novelty threshold
    
    # Development configuration
    config.set("development.enable", True)
    config.set("development.critical_period_length", 200)  # Longer critical period
    config.set("development.maturation_rate", 0.8)  # Slower maturation
    
    return config

# Usage
fast_config = create_fast_learning_config()
brain = pynlm.createBrain(fast_config)
brain.initialize()
```

### Template 2: Stable Memory Agent

This template creates a brain optimized for stable memory storage and recall.

```python
import pynlm

def create_stable_memory_config():
    """Create configuration for stable memory."""
    config = pynlm.createDefaultConfig()
    
    # Brain configuration
    config.set("brain.neuron_count", 300)  # Moderate size
    config.set("brain.synapse_density", 0.08)  # Lower connectivity for stability
    config.set("brain.v_thresh", -55.0)  # Standard threshold
    config.set("brain.v_rest", -75.0)    # Lower resting potential for stability
    config.set("brain.tau_mem", 25.0)  # Slower membrane dynamics
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)  # Weaker learning
    config.set("plasticity.stdp.tau_plus", 30.0)  # Slower LTP
    config.set("plasticity.stdp.tau_minus", 35.0)  # Slower LTD
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.0005)
    config.set("plasticity.structural.enable", False)  # No structural changes
    
    # Memory configuration
    config.set("memory.working_memory.capacity", 100)  # Large working memory
    config.set("memory.episodic_memory.max_episodes", 1000)  # High episodic capacity
    config.set("memory.associative_memory.pattern_count", 500)  # Many associations
    
    # Development configuration
    config.set("development.enable", True)
    config.set("development.maturation_rate", 0.3)  # Very slow maturation
    config.set("development.enable_pruning", False)  # No pruning for stability
    
    return config

# Usage
stable_config = create_stable_memory_config()
brain = pynlm.createBrain(stable_config)
brain.initialize()
```

### Template 3: Predictive Agent

This template creates a brain optimized for prediction and planning.

```python
import pynlm

def create_predictive_config():
    """Create configuration for prediction and planning."""
    config = pynlm.createDefaultConfig()
    
    # Brain configuration
    config.set("brain.neuron_count", 400)
    config.set("brain.synapse_density", 0.12)
    
    # Prediction system configuration
    config.set("prediction.enable", True)
    config.set("prediction.horizon", 10)  # Prediction horizon
    config.set("prediction.error_tolerance", 0.1)  # Prediction error sensitivity
    config.set("prediction.learning_rate", 0.02)  # Prediction learning
    
    # Cognition configuration
    config.set("cognition.planner.enable", True)
    config.set("cognition.planner.horizon", 5)  # Planning horizon
    config.set("cognition.planner.exploration_rate", 0.3)  # Balance exploration vs exploitation
    config.set("cognition.concept_formation.enable", True)
    config.set("cognition.concept_formation.formation_rate", 0.01)
    config.set("cognition.attention.enable", True)
    config.set("cognition.attention.competition_strength", 0.8)
    
    # Neuromodulation configuration
    config.set("neuromod.prediction_error.enable", True)
    config.set("neuromod.prediction_error.weight", 2.0)  # Strong prediction error signals
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.sensitivity", 1.5)
    
    return config

# Usage
predictive_config = create_predictive_config()
brain = pynlm.createBrain(predictive_config)
brain.initialize()
```

## Region-Based Brain Organization

### Example 1: Hierarchical Brain Structure

```python
import pynlm

def create_hierarchical_brain():
    """Create a brain with multiple specialized regions."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("brain.region_count", 4)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create specialized regions
    regions = {
        "sensory_cortex": brain.addRegion("sensory_cortex"),
        "motor_cortex": brain.addRegion("motor_cortex"),
        "hippocampus": brain.addRegion("hippocampus"),
        "prefrontal_cortex": brain.addRegion("prefrontal_cortex")
    }
    
    # Configure region-specific parameters
    # Sensory cortex: high sensory responsiveness
    # Motor cortex: high motor output capability
    # Hippocampus: episodic memory formation
    # Prefrontal cortex: planning and decision making
    
    return brain, regions

# Usage
hierarchical_brain, regions = create_hierarchical_brain()
print("Hierarchical brain created with regions:")
for name, region_id in regions.items():
    region = hierarchical_brain.getRegion(region_id)
    print(f"  {name}: {region.getName()}")
```

### Example 2: Distributed Processing

```python
import pynlm

def create_distributed_brain(num_processors=8, neurons_per_processor=100):
    """Create a brain with distributed processing architecture."""
    total_neurons = num_processors * neurons_per_processor
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", total_neurons)
    config.set("brain.region_count", num_processors)
    config.set("connection_probability", 0.05)  # Sparse connectivity
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create processing modules
    processors = []
    for i in range(num_processors):
        region_id = brain.addRegion(f"processor_{i}")
        processors.append({
            'id': region_id,
            'region': brain.getRegion(region_id),
            'type': 'processor'
        })
    
    # Create communication channels between processors
    for i in range(num_processors):
        for j in range(i + 1, num_processors):
            # Add inter-region connections for communication
            brain.addInterRegionConnection(
                processors[i]['id'],
                processors[j]['id'],
                weight=0.1,
                delay=2
            )
    
    return brain, processors

# Usage
distributed_brain, processors = create_distributed_brain()
print(f"Distributed brain created with {len(processors)} processors")
```

## Advanced Plasticity Configuration

### Example: Adaptive Plasticity

```python
import pynlm

def create_adaptive_plasticity_config():
    """Create configuration with adaptive plasticity mechanisms."""
    config = pynlm.createDefaultConfig()
    
    # Adaptive STDP
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    config.set("plasticity.stdp.adaptive", True)  # Adaptive learning rate
    config.set("plasticity.stdp.min_rate", 0.0001)
    config.set("plasticity.stdp.max_rate", 0.01)
    config.set("plasticity.stdp.adaptation_target", 0.01)  # Target stability
    
    # Homeostatic plasticity
    config.set("plasticity.homeostatic.enable", True)
    config.set("plasticity.homeostatic.target_firing_rate", 2.0)  # Hz
    config.set("plasticity.homeostatic.sensitivity", 0.5)
    config.set("plasticity.homeostatic.slow_component", 1000.0)  # Time constant
    
    # Structural plasticity with adaptive growth
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    config.set("plasticity.structural.adaptive_growth", True)
    config.set("plasticity.structural.max_density", 0.3)
    
    return config

# Usage
adaptive_config = create_adaptive_plasticity_config()
brain = pynlm.createBrain(adaptive_config)
brain.initialize()
```

## Neuromodulation Tuning

### Example: Custom Neuromodulation System

```python
import pynlm

def create_custom_neuromodulation_config():
    """Create configuration with custom neuromodulation parameters."""
    config = pynlm.createDefaultConfig()
    
    # Dopamine system
    config.set("neuromod.dopamine.baseline", 0.1)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.dopamine.response_time", 5.0)  # Time constant
    config.set("neuromod.dopamine.reaction_curve", "sigmoid")  # Response type
    
    # Curiosity system
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.baseline", 0.2)
    config.set("neuromod.curiosity.sensitivity", 1.0)
    config.set("neuromod.curiosity.exploration_threshold", 0.5)  # Min curiosity for exploration
    config.set("neuromod.curiosity.decay_rate", 0.95)  # Decay when not exploring
    
    # Novelty system
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.baseline", 0.0)
    config.set("neuromod.novelty.sensitivity", 0.8)
    config.set("neuromod.novelty.threshold", 0.3)  # Novelty detection threshold
    config.set("neuromod.novelty.memory_window", 100)  # Novelty memory
    
    # Prediction error system
    config.set("neuromod.prediction_error.enable", True)
    config.set("neuromod.prediction_error.sensitivity", 1.2)
    config.set("neuromod.prediction_error.adaptation_rate", 0.1)
    config.set("neuromod.prediction_error.min_error", 0.001)
    
    return config

# Usage
neuromod_config = create_custom_neuromodulation_config()
brain = pynlm.createBrain(neuromod_config)
brain.initialize()
```

## Performance Optimization

### Example: Optimized Configuration for Speed

```python
import pynlm

def create_performance_config():
    """Create configuration optimized for performance."""
    config = pynlm.createDefaultConfig()
    
    # Reduce computational load
    config.set("brain.neuron_count", 200)  # Smaller brain for speed
    config.set("brain.synapse_density", 0.05)  # Fewer synapses
    config.set("simulation.timestep", 0.005)  # Smaller timesteps
    config.set("max_simulation_steps", 1000)  # Fewer steps
    
    # Optimize plasticity calculations
    config.set("plasticity.stdp.enable", False)  # Disable STDP for speed
    config.set("plasticity.hebbian.enable", False)  # Disable Hebbian for speed
    config.set("plasticity.structural.enable", False)  # Disable structural for speed
    
    # Optimize memory systems
    config.set("memory.working_memory.capacity", 20)  # Small working memory
    config.set("memory.episodic_memory.max_episodes", 50)  # Few episodes
    config.set("memory.associative_memory.pattern_count", 20)  # Few associations
    
    return config

# Usage
perf_config = create_performance_config()
brain = pynlm.createBrain(perf_config)
brain.initialize()

# Fast simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()
print(f"Simulated 1000 steps in {end_time - start_time:.2f} seconds")
```

### Example: Optimized Configuration for Accuracy

```python
import pynlm

def create_accuracy_config():
    """Create configuration optimized for accuracy."""
    config = pynlm.createDefaultConfig()
    
    # Increase computational accuracy
    config.set("brain.neuron_count", 1000)  # Larger brain for capacity
    config.set("brain.synapse_density", 0.2)  # More synapses for connectivity
    config.set("simulation.timestep", 0.001)  # Smaller timesteps for accuracy
    config.set("max_simulation_steps", 5000)  # More steps for convergence
    
    # Enable all plasticity systems for learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
    
    # Enable all memory systems for storage
    config.set("memory.working_memory.capacity", 500)
    config.set("memory.episodic_memory.max_episodes", 2000)
    config.set("memory.associative_memory.pattern_count", 1000)
    
    # Enable all neuromodulation systems
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.enable", True)
    
    # Enable development for maturation
    config.set("development.enable", True)
    config.set("development.maturation_rate", 0.5)
    
    return config

# Usage
accuracy_config = create_accuracy_config()
brain = pynlm.createBrain(accuracy_config)
brain.initialize()

# High-accuracy simulation
start_time = time.time()
for step in range(5000):
    brain.step(step)
end_time = time.time()
print(f"Simulated 5000 steps in {end_time - start_time:.2f} seconds")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Scalability Examples

### Example 1: Mini Brain (For Testing)

```python
import pynlm

def create_mini_brain():
    """Create a minimal brain for quick testing."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", False)  # Disable for speed
    config.set("memory.working_memory.capacity", 5)
    config.set("memory.episodic_memory.max_episodes", 10)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

# Usage
mini_brain = create_mini_brain()
print(f"Mini brain created: {mini_brain.getTotalNeuronCount()} neurons")
```

### Example 2: Large Brain (For Research)

```python
import pynlm

def create_large_brain():
    """Create a large brain for research purposes."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.region_count", 10)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("memory.working_memory.capacity", 5000)
    config.set("memory.episodic_memory.max_episodes", 20000)
    config.set("memory.associative_memory.pattern_count", 10000)
    config.set("development.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

# Usage
large_brain = create_large_brain()
print(f"Large brain created: {large_brain.getTotalNeuronCount()} neurons")
print(f"Memory capacity: {large_brain.getWorkingMemory().getMaxTraceCount()} traces")
```

## Special Use Cases

### Example 1: Real-time Simulation

```python
import pynlm
import time

def create_realtime_config(target_fps=30):
    """Create configuration for real-time simulation."""
    timestep = 1.0 / target_fps  # Time per step
    config = pynlm.createDefaultConfig()
    config.set("simulation.timestep", timestep)
    config.set("brain.neuron_count", 100)  # Small for real-time
    config.set("plasticity.stdp.enable", False)  # Disable for speed
    config.set("memory.working_memory.capacity", 20)
    config.set("memory.episodic_memory.max_episodes", 100)
    
    return config

def run_realtime_simulation(brain, max_duration=60):
    """Run simulation in real-time."""
    start_time = time.time()
    step = 0
    
    while time.time() - start_time < max_duration:
        frame_start = time.time()
        
        # Update brain
        brain.step(step)
        step += 1
        
        # Calculate sleep time to maintain target FPS
        frame_time = time.time() - frame_start
        sleep_time = (1.0 / 30.0) - frame_time  # Target 30 FPS
        
        if sleep_time > 0:
            time.sleep(sleep_time)
    
    print(f"Real-time simulation completed: {step} steps in {max_duration} seconds")
    return step

# Usage
realtime_config = create_realtime_config()
brain = pynlm.createBrain(realtime_config)
brain.initialize()

# Run real-time simulation
steps = run_realtime_simulation(brain, max_duration=30)
```

### Example 2: Batch Processing

```python
import pynlm
import time

def create_batch_config(batch_size=100):
    """Create configuration for batch processing."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("memory.working_memory.capacity", 100)
    config.set("memory.episodic_memory.max_episodes", 500)
    
    return config

def run_batch_simulation(config, num_simulations, steps_per_simulation):
    """Run multiple simulations in batch."""
    results = []
    
    for sim in range(num_simulations):
        print(f"Running simulation {sim + 1}/{num_simulations}")
        
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps_per_simulation):
            brain.step(step)
        
        end_time = time.time()
        
        result = {
            'simulation': sim + 1,
            'total_time': end_time - start_time,
            'steps_per_second': steps_per_simulation / (end_time - start_time),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'final_firing_rate': brain.getFiringNeuronCount(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        results.append(result)
    
    # Calculate summary statistics
    total_time = sum(r['total_time'] for r in results)
    avg_steps_per_second = sum(r['steps_per_second'] for r in results) / len(results)
    avg_total_spikes = sum(r['total_spikes'] for r in results) / len(results)
    
    summary = {
        'total_simulations': num_simulations,
        'total_time': total_time,
        'avg_steps_per_second': avg_steps_per_second,
        'avg_total_spikes': avg_total_spikes,
        'individual_results': results
    }
    
    return summary

# Usage
batch_config = create_batch_config()
summary = run_batch_simulation(batch_config, num_simulations=10, steps_per_simulation=1000)

print(f"\n=== Batch Processing Results ===")
print(f"Total simulations: {summary['total_simulations']}")
print(f"Total time: {summary['total_time']:.2f} seconds")
print(f"Average performance: {summary['avg_steps_per_second']:.0f} steps/second")
print(f"Average spikes: {summary['avg_total_spikes']:.0f}")
```

### Example 3: Environment Simulation

```python
import pynlm

def create_environment_simulation_config(world_size=30, food_rate=0.1, hazard_rate=0.05):
    """Create configuration for environment simulation."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("brain.synapse_density", 0.08)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    return config

def run_environment_simulation(brain, world, num_steps=1000):
    """Run simulation in a dynamic environment."""
    print(f"Starting environment simulation: {num_steps} steps")
    print("Step | Food | Hazards | Agent Energy | Agent Health | Curiosity | Novelty")
    print("-" * 90)
    
    food_collected = 0
    hazards_avoided = 0
    
    for step in range(num_steps):
        # Update world (environment changes)
        world.update(0.1)
        
        # Get agent's state
        body = world.getAgentBody()
        initial_energy = body.energy
        initial_health = body.health
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        world.getAgentBrain().processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action and apply
        action = world.getAgentBrain().decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for food collection
        percept_after = world.getSensoryPercept()
        vision = percept_after.getVision()
        if vision.size() > 0:
            max_vision = max(vision)
            if max_vision > 0.7:  # Food detected
                food_collected += 1
                # Agent gains energy
                body.energy = min(1.0, body.energy + 0.1)
        
        # Check for hazards
        if initial_health < body.health:  # Health improved
            if initial_energy > body.energy:  # Energy decreased
                hazards_avoided += 1
        
        # Log progress every 100 steps
        if (step + 1) % 100 == 0:
            agent_brain = world.getAgentBrain()
            print(f"{step+1:4d} | {food_collected:4d} | {hazards_avoided:7d} | "
                  f"{body.energy:10.2f} | {body.health:10.2f} | "
                  f"{agent_brain.getCuriosityLevel():8.3f} | {agent_brain.getNoveltyLevel():7.3f}")
    
    print("-" * 90)
    print(f"Environment simulation completed:")
    print(f"  Food collected: {food_collected}")
    print(f"  Hazards avoided: {hazards_avoided}")
    print(f"  Final agent energy: {world.getAgentBody().energy:.2f}")
    print(f"  Final agent health: {world.getAgentBody().health:.2f}")
    print(f"  Learning metrics: Curiosity={world.getAgentBrain().getCuriosityLevel():.3f}, "
          f"Novelty={world.getAgentBrain().getNoveltyLevel():.3f}")
    
    return {
        'food_collected': food_collected,
        'hazards_avoided': hazards_avoided,
        'final_energy': world.getAgentBody().energy,
        'final_health': world.getAgentBody().health,
        'curiosity': world.getAgentBrain().getCuriosityLevel(),
        'novelty': world.getAgentBrain().getNoveltyLevel()
    }

# Usage
env_config = create_environment_simulation_config()
brain = pynlm.createBrain(env_config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=15, visionHeight=15)
world.reset()
world.setAgentStart(15.0, 15.0)

agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Enable learning
agent_brain.enableRewardModulation(True)
agent_brain.enableCuriosity(True)
agent_brain.enableDevelopment(True)

# Run environment simulation
results = run_environment_simulation(brain, world, num_steps=500)
```

---

## Quick Reference Guide

### Configuration Categories

| Category | Key Pattern | Example | Description |
|----------|------------|---------|-------------|
| Brain | `brain.neuron_count` | 500 | Total number of neurons |
| | `brain.synapse_density` | 0.1 | Synapse connectivity density |
| | `brain.connection_probability` | 0.15 | Probability of connection |
| Plasticity | `plasticity.stdp.enable` | true | Enable STDP learning |
| | `plasticity.stdp.learning_rate` | 0.001 | STDP learning rate |
| | `plasticity.hebbian.enable` | true | Enable Hebbian learning |
| | `plasticity.structural.enable` | true | Enable structural plasticity |
| Neuromodulation | `neuromod.dopamine.scale` | 1.0 | Dopamine modulation scale |
| | `neuromod.curiosity.enable` | true | Enable curiosity-driven exploration |
| | `neuromod.novelty.enable` | true | Enable novelty detection |
| Development | `development.enable` | true | Enable developmental processes |
| | `development.maturation_rate` | 0.5 | Development maturation rate |
| Memory | `memory.working_memory.capacity` | 100 | Working memory capacity |
| | `memory.episodic_memory.max_episodes` | 1000 | Maximum episodic memories |

### Common Patterns

1. **Fast Learning**: Higher plasticity rates, lower memory limits
2. **Stable Memory**: Lower plasticity, higher memory limits, slower development
3. **Predictive Agent**: Strong prediction systems, high planning horizons
4. **Real-time**: Small brain, disabled plasticity, frequent simulation steps
5. **Research**: Large brain, all systems enabled, comprehensive memory

### Performance Tips

1. **For Speed**: Disable structural plasticity, use smaller brain, reduce simulation steps
2. **For Accuracy**: Enable all plasticity, use larger brain, increase simulation steps
3. **For Memory**: Limit working memory, use episodic memory efficiently
4. **For Learning**: Enable all neuromodulation, use adaptive plasticity
5. **For Stability**: Use homeostatic plasticity, slow development, conservative learning

### Troubleshooting

| Problem | Likely Cause | Solution |
|---------|--------------|----------|
| Agent doesn't learn | Plasticity disabled | Enable plasticity systems |
| Agent stuck in one place | No exploration | Enable curiosity system |
| Too many spikes | Low threshold | Increase neuron threshold |
| Memory full | Large memory capacity | Clear memory periodically |
| Slow performance | Large brain | Reduce neuron count, disable plasticity |
| Instability | High learning rates | Reduce plasticity learning rates |

### Next Steps

1. **Beginner**: Use basic examples from `easy_usage.md`
2. **Intermediate**: Try advanced examples from this guide
3. **Advanced**: Create custom configurations based on templates
4. **Research**: Use large brain configurations for experiments
5. **Production**: Optimize for speed or accuracy based on use case

---

This comprehensive guide provides examples for all major aspects of the NLM system, from basic usage to advanced configurations. Each example includes complete, runnable code that can be modified for specific use cases.