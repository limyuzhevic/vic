# Enhanced Documentation for NLM Phase 6

This document provides comprehensive documentation for NLM Phase 6 with detailed examples and advanced usage patterns.

## Quick Start: Integration Testing

The Phase 6 integration demo demonstrates all the cognitive systems working together:

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo

# Run with custom brain configuration
./nlm_phase6_demo --config config.json --steps 10000
```

## Advanced Python Examples

### 1. Custom Brain Configuration

```python
import pynlm

# Create enhanced configuration
config = pynlm.createDefaultConfig()

# Configure memory systems
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("working_memory.capacity", 500)
config.set("episodic_memory.max_episodes", 5000)
config.set("prediction.horizon", 5)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.acetylcholine.scale", 1.2)
config.set("neuromod.norepinephrine.scale", 0.8)
config.set("neuromod.serotonin.scale", 0.9)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### 2. Cognitive Development Simulation

```python
import pynlm
import time

def run_development_simulation(num_steps=20000):
    """Simulate cognitive development over time"""
    
    # Initialize brain with development enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.1)
    config.set("development.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create environment and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems for full development
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting cognitive development simulation...")
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Initial E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Track development metrics
    metrics = {
        'spike_counts': [],
        'memory_traces': [],
        'curiosity_levels': [],
        'developmental_stages': [],
        'prediction_accuracies': []
    }
    
    for step in range(num_steps):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in agent
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = 0.0
        if agent.isRewardModulationEnabled():
            reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, 0.0)
        
        # Record metrics
        if step % 100 == 0:
            metrics['spike_counts'].append(brain.getTotalSpikeCount())
            metrics['memory_traces'].append(brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0)
            metrics['curiosity_levels'].append(agent.getCuriosityLevel())
            metrics['developmental_stages'].append(brain.getDevelopmentalStage())
            
            if step % 1000 == 0:
                print(f"Step {step}:")
                print(f"  Developmental Stage: {brain.getDevelopmentalStage()}")
                print(f"  Active Traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
                print(f"  Curiosiy Level: {agent.getCuriosityLevel():.3f}")
                print(f"  Prediction Error: {agent.getPredictionError():.3f}")
                print(f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    return metrics

# Run development simulation
metrics = run_development_simulation(10000)

# Plot development progress (requires matplotlib)
import matplotlib.pyplot as plt

fig, axes = plt.subplots(3, 2, figsize=(15, 12))

# Spike counts over time
axes[0, 0].plot(metrics['spike_counts'])
axes[0, 0].set_title('Spike Count Over Time')
axes[0, 0].set_xlabel('Time (100-step intervals)')
axes[0, 0].set_ylabel('Spike Count')

# Memory traces over time
axes[0, 1].plot(metrics['memory_traces'])
axes[0, 1].set_title('Working Memory Traces Over Time')
axes[0, 1].set_xlabel('Time (100-step intervals)')
axes[0, 1].set_ylabel('Active Traces')

# Curiosity levels over time
axes[1, 0].plot(metrics['curiosity_levels'])
axes[1, 0].set_title('Curiosity Level Over Time')
axes[1, 0].set_xlabel('Time (100-step intervals)')
axes[1, 0].set_ylabel('Curiosity Level')

# Developmental stages over time
axes[1, 1].scatter(range(len(metrics['developmental_stages'])), metrics['developmental_stages'])
axes[1, 1].set_title('Developmental Stage Changes')
axes[1, 1].set_xlabel('Time (100-step intervals)')
axes[1, 1].set_ylabel('Stage')
axes[1, 1].set_yticks([0, 1, 2, 3, 4])
axes[1, 1].set_yticklabels(['Initial', 'Critical', 'Maturation', 'Adult', 'Aging'])

# Save metrics to file
import json
with open('development_metrics.json', 'w') as f:
    json.dump(metrics, f, indent=2)

plt.tight_layout()
plt.savefig('development_metrics.png', dpi=150)
print("Development metrics saved to development_metrics.json and development_metrics.png")

return metrics
```

### 3. Multi-Agent Social Learning

```python
import pynlm

def run_social_learning_experiment(num_agents=5, episodes=100):
    """Run experiment with multiple interacting agents"""
    
    # Create shared environment
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create multiple agents
    agents = []
    brains = []
    
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.05)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable social learning for all agents
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
        brains.append(brain)
    
    print(f"Created {num_agents} agents for social learning experiment")
    
    # Track social learning metrics
    social_metrics = {
        'imitation_success': [],
        'shared_knowledge': [],
        'group_performance': [],
        'individual_improvements': []
    }
    
    for episode in range(episodes):
        # Reset world
        world.reset()
        
        # Run one step for each agent
        for i, agent in enumerate(agents):
            # Each agent observes others
            if i > 0:
                # Simple observation of other agents
                # In a real implementation, this would involve actual observation
                pass
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process and act
            agent.processSensoryInput(percept)
            brain.step(episode)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward and learning
            reward = world.computeReward(agent)
            agent.applyRewardModulation(reward, 0.0)
        
        # Evaluate social learning
        if episode % 10 == 0:
            # Simple imitation success metric
            imitation_success = sum(1 for agent in agents 
                                  if agent.getPredictionError() < 0.1)
            social_metrics['imitation_success'].append(imitation_success / num_agents)
            
            # Group performance
            group_performance = sum(world.computeReward(agent) for agent in agents) / num_agents
            social_metrics['group_performance'].append(group_performance)
            
            print(f"Episode {episode}:")
            print(f"  Imitation Success Rate: {social_metrics['imitation_success'][-1]:.2f}")
            print(f"  Average Group Performance: {social_metrics['group_performance'][-1]:.2f}")
    
    return social_metrics

# Run social learning experiment
social_metrics = run_social_learning_experiment(num_agents=5, episodes=200)
```

### 4. Prediction System Analysis

```python
import pynlm

def analyze_prediction_system(brain, world, agent, steps=5000):
    """Analyze prediction system performance"""
    
    predictions = []
    errors = []
    confidences = []
    
    for step in range(steps):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept_before = world.getSensoryPercept()
        
        # Agent processes input
        agent.processSensoryInput(percept_before)
        
        # Brain makes prediction based on current state
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get next sensory state
        percept_after = world.getSensoryPercept()
        
        # Analysis (simplified)
        if brain.getPredictionSystem():
            # In a real implementation, we would track actual predictions
            predictions.append(percept_before.getData())
            errors.append(agent.getPredictionError())
            confidences.append(1.0 - agent.getPredictionError())  # Simplified confidence
    
    # Statistical analysis
    import numpy as np
    
    print("Prediction System Analysis Results:")
    print(f"Total predictions: {len(predictions)}")
    print(f"Average prediction error: {np.mean(errors):.3f}")
    print(f"Prediction error std: {np.std(errors):.3f}")
    print(f"Average confidence: {np.mean(confidences):.3f}")
    
    # Error distribution
    error_bins = np.histogram(errors, bins=10)[0]
    print("\nPrediction Error Distribution:")
    for i, count in enumerate(error_bins):
        if count > 0:
            print(f"  Error range {i*0.1:.1f}-{(i+1)*0.1:.1f}: {count} occurrences")
    
    return {
        'predictions': predictions,
        'errors': errors,
        'confidences': confidences,
        'mean_error': np.mean(errors),
        'std_error': np.std(errors),
        'mean_confidence': np.mean(confidences)
    }
```

### 5. Memory Consolidation Analysis

```python
import pynlm

def analyze_memory_consolidation(brain, agent, steps=10000):
    """Analyze memory consolidation patterns"""
    
    memory_metrics = {
        'episode_counts': [],
        'replay_events': [],
        'consolidation_events': [],
        'memory_strength': [],
        'forgetting_rates': []
    }
    
    for step in range(steps):
        # Record memory state before step
        if brain.getWorkingMemory():
            memory_metrics['episode_counts'].append(brain.getWorkingMemory().getActiveTraces())
        
        if brain.getEpisodicMemory():
            memory_metrics['memory_strength'].append(brain.getEpisodicMemory().getAverageStrength())
        
        # Run brain step (includes consolidation)
        brain.step(step)
        
        # Record consolidation events
        if step % 1000 == 0 and step > 0:
            if brain.getEpisodicMemory():
                before_count = len(memory_metrics['episode_counts']) - 1
                current_strength = brain.getEpisodicMemory().getAverageStrength()
                memory_metrics['memory_strength'].append(current_strength)
                memory_metrics['consolidation_events'].append(step)
    
    # Analyze consolidation patterns
    print("Memory Consolidation Analysis:")
    
    # Episode growth
    initial_episodes = memory_metrics['episode_counts'][0] if memory_metrics['episode_counts'] else 0
    final_episodes = memory_metrics['episode_counts'][-1] if memory_metrics['episode_counts'] else 0
    print(f"Working memory traces: {initial_episodes} → {final_episodes}")
    
    # Memory strength trends
    if len(memory_metrics['memory_strength']) > 10:
        strength_first_half = memory_metrics['memory_strength'][:len(memory_metrics['memory_strength'])//2]
        strength_second_half = memory_metrics['memory_strength'][len(memory_metrics['memory_strength'])//2:]
        
        avg_strength_first = np.mean(strength_first_half)
        avg_strength_second = np.mean(strength_second_half)
        
        print(f"Memory strength (first half): {avg_strength_first:.3f}")
        print(f"Memory strength (second half): {avg_strength_second:.3f}")
        print(f"Strength change: {avg_strength_second - avg_strength_first:.3f}")
    
    # Consolidation events
    print(f"Consolidation events at steps: {memory_metrics['consolidation_events']}")
    
    return memory_metrics
```

## Advanced Configuration Options

### 1. Brain Size and Complexity

```python
import pynlm

# Large brain configuration
large_config = pynlm.createDefaultConfig()
large_config.set("brain.neuron_count", 50000)
large_config.set("brain.region_count", 10)
large_config.set("brain.synapse_density", 0.05)

# Small brain configuration  
small_config = pynlm.createDefaultConfig()
small_config.set("brain.neuron_count", 100)
small_config.set("brain.region_count", 1)
small_config.set("brain.synapse_density", 0.2)

# Memory-intensive configuration
memory_config = pynlm.createDefaultConfig()
memory_config.set("brain.neuron_count", 1000)
memory_config.set("working_memory.capacity", 1000)
memory_config.set("episodic_memory.max_episodes", 10000)
```

### 2. Neuromodulation Calibration

```python
import pynlm

# High novelty seeking configuration
novelty_config = pynlm.createDefaultConfig()
novelty_config.set("neuromod.novelty.enable", True)
novelty_config.set("neuromod.curiosity.enable", True)
novelty_config.set("neuromod.curiosity.sensitivity", 2.0)
novelty_config.set("neuromod.novelty.sensitivity", 1.5)

# High reward sensitivity configuration
reward_config = pynlm.createDefaultConfig()
reward_config.set("neuromod.dopamine.scale", 2.0)
reward_config.set("neuromod.prediction_error.sensitivity", 1.5)

# High attention configuration
attention_config = pynlm.createDefaultConfig()
attention_config.set("neuromod.acetylcholine.scale", 1.5)
attention_config.set("neuromod.norepinephrine.scale", 1.2)
```

### 3. Development Timing

```python
import pynlm

# Fast development
development_config = pynlm.createDefaultConfig()
development_config.set("development.critical_period.length", 1000)
development_config.set("development.maturation.threshold", 5000)
development_config.set("development.adult.threshold", 10000)

# Slow development
slow_development_config = pynlm.createDefaultConfig()
slow_development_config.set("development.critical_period.length", 5000)
slow_development_config.set("development.maturation.threshold", 20000)
slow_development_config.set("development.adult.threshold", 50000)
```

## Debugging and Monitoring

### 1. Brain Status Monitoring

```python
def monitor_brain_status(brain, interval=1000):
    """Monitor brain status over time"""
    
    status_log = []
    
    for step in range(0, 10000, interval):
        status = {
            'step': step,
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
        
        if brain.getWorkingMemory():
            status['working_memory_traces'] = brain.getWorkingMemory().getActiveTraces()
        
        if brain.getEpisodicMemory():
            status['episodic_memory_count'] = brain.getEpisodicMemory().getEpisodeCount()
        
        status_log.append(status)
        
        # Log to console
        print(f"Step {step}: "
              f"Firing: {status['firing_rate']:.2f}, "
              f"E/I: {status['e_i_ratio']:.2f}, "
              f"Dev Stage: {status['developmental_stage']}")
    
    return status_log
```

### 2. Performance Profiling

```python
import time
import pynlm

def profile_brain_performance(brain, world, agent, steps=1000):
    """Profile brain performance"""
    
    # Time brain step
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    brain_time = time.time() - start_time
    
    # Time sensory processing
    start_time = time.time()
    
    for step in range(steps):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
    
    sensory_time = time.time() - start_time
    
    # Time action decoding
    start_time = time.time()
    
    for step in range(steps):
        action = agent.decodeMotorCommand()
    
    action_time = time.time() - start_time
    
    print("Performance Profile:")
    print(f"  Brain step time: {brain_time/steps*1000:.2f} ms/step")
    print(f"  Sensory processing time: {sensory_time/steps*1000:.2f} ms/step")
    print(f"  Action decoding time: {action_time/steps*1000:.2f} ms/step")
    print(f"  Total time: {(brain_time+sensory_time+action_time)/steps*1000:.2f} ms/step")
    
    return {
        'brain_step_time': brain_time/steps,
        'sensory_time': sensory_time/steps,
        'action_time': action_time/steps,
        'total_time': (brain_time+sensory_time+action_time)/steps
    }
```

## Configuration File Examples

### 1. Basic Configuration

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "synapse_density": 0.1
    },
    "working_memory": {
        "capacity": 100
    },
    "episodic_memory": {
        "max_episodes": 1000
    },
    "prediction": {
        "horizon": 3
    },
    "neuromod": {
        "dopamine": {"scale": 1.0},
        "curiosity": {"enable": true},
        "novelty": {"enable": true}
    },
    "development": {
        "enable": true,
        "critical_period_length": 1000
    }
}
```

### 2. High-Performance Configuration

```json
{
    "brain": {
        "neuron_count": 10000,
        "region_count": 4,
        "synapse_density": 0.05,
        "connection_probability": 0.02
    },
    "working_memory": {
        "capacity": 1000,
        "decay_rate": 0.01
    },
    "episodic_memory": {
        "max_episodes": 10000,
        "replay_interval": 50,
        "consolidation_interval": 500
    },
    "prediction": {
        "horizon": 10,
        "confidence_threshold": 0.7
    },
    "neuromod": {
        "dopamine": {"scale": 1.2},
        "curiosity": {"enable": true, "decay_rate": 0.005},
        "novelty": {"enable": true, "decay_rate": 0.01},
        "acetylcholine": {"scale": 1.0},
        "norepinephrine": {"scale": 0.8},
        "serotonin": {"scale": 0.9}
    },
    "development": {
        "enable": true,
        "critical_period_length": 2000,
        "maturation_threshold": 8000
    },
    "performance": {
        "checkpoint_interval": 1000,
        "max_checkpoints": 10
    }
}
```

## Running Experiments

### 1. Phase 6 Integration Demo

```bash
# Basic demo
./nlm_phase6_demo

# Demo with custom configuration
./nlm_phase6_demo --config custom_config.json

# Demo with verbose output
./nlm_phase6_demo --verbose --log-level debug

# Demo with specific metrics
./nlm_phase6_demo --metrics memory prediction cognition
```

### 2. Automated Experiment Runner

```python
import pynlm
import json
import os

def run_experiment_suite(experiments, output_dir="experiment_results"):
    """Run a suite of experiments"""
    
    os.makedirs(output_dir, exist_ok=True)
    
    results = {}
    
    for experiment_name, config in experiments.items():
        print(f"Running experiment: {experiment_name}")
        
        # Create brain with configuration
        brain_config = pynlm.createDefaultConfig()
        
        # Apply experiment configuration
        if "brain" in config:
            for key, value in config["brain"].items():
                brain_config.set(f"brain.{key}", value)
        
        if "neuromod" in config:
            for key, value in config["neuromod"].items():
                brain_config.set(f"neuromod.{key}.scale", value)
        
        # Create brain
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        # Run experiment
        experiment_results = {}
        
        if "development" in config and config["development"].get("enabled", False):
            # Run development simulation
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            for step in range(config.get("steps", 1000)):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if step % 100 == 0:
                    experiment_results[f"step_{step}"] = {
                        "firing_rate": brain.getAverageFiringRate(),
                        "e_i_ratio": brain.getExcitationInhibitionRatio(),
                        "developmental_stage": brain.getDevelopmentalStage()
                    }
        
        results[experiment_name] = experiment_results
        
        # Save results
        with open(f"{output_dir}/{experiment_name}_results.json", "w") as f:
            json.dump(experiment_results, f, indent=2)
        
        print(f"  Experiment completed: {len(experiment_results)} data points")
    
    # Save summary
    with open(f"{output_dir}/experiment_summary.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"All experiments completed. Results saved to {output_dir}/")
    
    return results

# Define experiment suite
experiments = {
    "baseline": {
        "brain": {"neuron_count": 1000, "synapse_density": 0.1},
        "steps": 1000
    },
    "high_curiosity": {
        "brain": {"neuron_count": 1000, "synapse_density": 0.1},
        "neuromod": {"curiosity": 2.0, "novelty": 1.5},
        "steps": 1000
    },
    "high_reward": {
        "brain": {"neuron_count": 1000, "synapse_density": 0.1},
        "neuromod": {"dopamine": 2.0},
        "steps": 1000
    }
}

# Run experiment suite
experiment_results = run_experiment_suite(experiments)
```

## Troubleshooting

### 1. Common Issues and Solutions

**Issue: Brain not responding to sensory input**
```python
# Solution: Check sensory neuron connections
if brain.getTotalNeuronCount() == 0:
    config.set("brain.neuron_count", 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
```

**Issue: Memory not growing**
```python
# Solution: Enable memory systems
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

**Issue: Performance too slow**
```python
# Solution: Reduce brain size
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.05)
```

**Issue: Prediction errors too high**
```python
# Solution: Adjust prediction system
config.set("prediction.horizon", 1)
config.set("prediction.confidence_threshold", 0.5)
```

### 2. Debugging Tools

```python
def debug_brain_state(brain):
    """Debug current brain state"""
    
    print("=== Brain Debug Information ===")
    print(f"Neuron count: {brain.getTotalNeuronCount()}")
    print(f"Synapse count: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    if brain.getWorkingMemory():
        print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
    
    if brain.getEpisodicMemory():
        print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    if brain.getPredictionSystem():
        print(f"Prediction system active: True")
    
    print("=== Neuromodulation Status ===")
    if brain.getDopamine():
        print(f"Dopamine level: {brain.getDopamine().getLevel():.3f}")
    
    if brain.getCuriosity():
        print(f"Curiosity level: {brain.getCuriosity().getLevel():.3f}")
    
    if brain.getNovelty():
        print(f"Novelty level: {brain.getNovelty().getLevel():.3f}")
    
    if brain.getAcetylcholine():
        print(f"Acetylcholine level: {brain.getAcetylcholine().getLevel():.3f}")
    
    if brain.getNorepinephrine():
        print(f"Norepinephrine level: {brain.getNorepinephrine().getLevel():.3f}")
    
    if brain.getSerotonin():
        print(f"Serotonin level: {brain.getSerotonin().getLevel():.3f}")
    
    print("=== Development Status ===")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

## Additional Resources

### 1. Documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [BIOLOGICAL_FIDELITY.md](docs/BIOLOGICAL_FIDELITY.md) - Biological realism documentation

### 2. Example Scripts
- [examples/](examples/) - Additional example scripts
- [demo/](demo/) - Interactive demo scripts

### 3. Building and Installation
- [BUILDING.md](BUILDING.md) - Building instructions
- [INSTALLATION.md](INSTALLATION.md) - Installation guide

## Conclusion

NLM Phase 6 provides a complete, integrated artificial brain system with:

- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Prediction Systems**: Next state prediction and prediction error computation
- **Cognition Systems**: Attention, planning, and concept formation
- **Neuromodulation**: Multiple neuromodulators for behavior regulation
- **Development**: Developmental stages affecting learning and plasticity
- **Integration**: All systems working together in a coherent brain loop

This enhanced documentation provides comprehensive examples for using NLM Phase 6 in research, experimentation, and development of artificial cognitive systems.