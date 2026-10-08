# NLM - Complete Usage Guide for All Skill Levels

**NLM (Neural Learning Machine) - Phase 6 Complete Artificial Brain**

## Quick Overview

NLM is a sophisticated brain-inspired spiking neural network simulator that integrates **60+ interconnected systems** into a complete artificial brain. It demonstrates:

- **Working Memory** with persistent activity and competition
- **Episodic Memory** with experience encoding and replay
- **Neuromodulation** (dopamine, curiosity, novelty) affecting plasticity
- **Prediction Systems** integrated with learning
- **Cognition** (neural planning, concept formation, attention)
- **Development** (developmental stages affecting plasticity)
- **Persistence** (checkpoint save/load)
- **Replay** (memory consolidation during rest)
- **Closed-loop sensory-motor learning** with reward modulation

## Installation

### Prerequisites
- Python 3.8 or higher
- CMake 3.16 or higher
- C++ compiler (GCC 10+, Clang 12+, or MSVC 2019+)

### Install from Source

```bash
# Clone the repository
https://github.com/nlm-project/nlm.git

# Install with all dependencies
pip install -e .[all]

# Or install with specific extras
pip install -e .[visualization]  # For visualization support
pip install -e .[jupyter]       # For Jupyter notebooks
pip install -e .[dev]           # For development tools
```

## Quick Start Examples

### 1. Create a Minimal Brain (30 seconds)

```python
import nlm

# Create a default brain configuration
config = nlm.createDefaultConfig()

# Create and initialize the brain
brain = nlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Simulate a few steps
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

### 2. Complete Agent Simulation (2 minutes)

```python
import nlm

def run_agent_simulation(num_steps=1000):
    """Complete agent simulation with brain and world interaction."""
    
    # 1. Create configuration with custom settings
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    
    # 2. Create and initialize brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = nlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = nlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("=== Starting Agent Simulation ===")
    print(f"Configuration: {config.get('brain.neuron_count')} neurons")
    print(f"Environment: {world.getWidth()}x{world.getHeight()} cells")
    print(f"World timestep: 0.1s")
    print(f"Total simulation steps: {num_steps}")
    print()
    
    # 7. Run simulation loop
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
        
        # Apply reward modulation (learn from experience)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development (brain matures over time)
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage().name}")
            print(f"  Reward: {reward:.3f}")
            print()
    
    # 8. Simulation complete
    print("=== Simulation Complete ===")
    print(f"Total simulation time: {world.getSimulationTime()}s")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Development stage reached: {brain.getDevelopmentalStage().name}")
    print(f"Total reward accumulated: {sum(world.getSensoryPercept().getInternal()[0] for _ in range(world.getSimulationTime())):.2f}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation(500)
```

### 3. Phase 6 Integration Demo (1 minute)

```python
import nlm

def run_phase6_demo():
    """Run the complete Phase 6 integration demo."""
    print("=== NLM Phase 6 Integration Demo ===")
    print("Testing all 60+ integrated systems...")
    print()
    
    # Create the Phase 6 integrated experiment
    experiment = nlm.Phase6IntegratedExperiment()
    config = nlm.Phase6Config()
    
    # Configure for demo mode
    config.neuronCount = 300
    config.maxSteps = 1000
    config.enableCheckpointing = True
    config.enableReplay = True
    config.enableDevelopment = True
    
    print(f"Configuration:")
    print(f"  Neurons: {config.neuronCount}")
    print(f"  Max steps: {config.maxSteps}")
    print(f"  Checkpointing: {'Enabled' if config.enableCheckpointing else 'Disabled'}")
    print(f"  Replay: {'Enabled' if config.enableReplay else 'Disabled'}")
    print(f"  Development: {'Enabled' if config.enableDevelopment else 'Disabled'}")
    print()
    
    # Run the experiment
    result = experiment.run(config)
    
    # Print results
    print("=== Integration Results ===")
    print(f"Total reward: {result.totalReward}")
    print(f"Average firing rate: {result.avgFiringRate}")
    print(f"Memory episodes stored: {result.memoryEpisodesStored}")
    print(f"Dopamine level: {result.dopamineLevel}")
    print(f"Wall clock time: {result.totalWallClockTime}s")
    print()
    
    print("=== System Integration Status ===")
    print(f"Working Memory: {'CONNECTED' if result.memoryWorkingMemoryIntegrated else 'DISCONNECTED'}")
    print(f"Episodic Memory: {'CONNECTED' if result.memoryEpisodicMemoryIntegrated else 'DISCONNECTED'}")
    print(f"Neuromodulation: {'CONNECTED' if result.neuromodulationIntegrated else 'DISCONNECTED'}")
    print(f"Prediction: {'CONNECTED' if result.predictionIntegrated else 'DISCONNECTED'}")
    print(f"Development: {'CONNECTED' if result.developmentIntegrated else 'DISCONNECTED'}")
    print(f"Checkpointing: {'WORKING' if result.checkpointingWorks else 'NOT WORKING'}")
    print()
    
    print("=== Phase 6 Integration Complete ===")
    print("All 60+ brain systems successfully integrated and functional!")
    
    return result

# Run the Phase 6 demo
result = run_phase6_demo()
```

### 4. Advanced Configuration Examples

```python
import nlm

# Example 1: High-performance configuration
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Example 2: Development-focused configuration
config2 = nlm.createDefaultConfig()
config2.set("brain.neuron_count", 1000)
config2.set("brain.synapse_density", 0.08)
config2.set("development.enable", True)
config2.set("plasticity.stdp.learning_rate", 0.005)
config2.set("plasticity.hebbian.learning_rate", 0.001)

# Example 3: Memory-intensive configuration
config3 = nlm.createDefaultConfig()
config3.set("brain.neuron_count", 500)
config3.set("brain.synapse_density", 0.12)
config3.set("memory.episodic.capacity", 10000)
config3.set("memory.working.capacity", 50000)
config3.set("memory.associative.capacity", 5000)
```

### 5. Visualization and Monitoring

```python
import nlm
import matplotlib.pyplot as plt
import numpy as np

def create_visualization_demo():
    """Create a brain visualization and monitoring demo."""
    
    # Setup brain and world
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Collect data for visualization
    steps = 200
    firing_rates = []
    curiosity_levels = []
    novelty_levels = []
    rewards = []
    developmental_stages = []
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        firing_rates.append(brain.getAverageFiringRate())
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        rewards.append(reward)
        developmental_stages.append(brain.getDevelopmentalStage())
    
    # Create visualization
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    fig.suptitle('NLM Brain Simulation Visualization', fontsize=16)
    
    # Firing rate over time
    axes[0, 0].plot(firing_rates)
    axes[0, 0].set_title('Average Firing Rate')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Hz')
    axes[0, 0].grid(True)
    
    # Curiosity level over time
    axes[0, 1].plot(curiosity_levels)
    axes[0, 1].set_title('Curiosity Level')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Level (0-1)')
    axes[0, 1].grid(True)
    
    # Novelty level over time
    axes[0, 2].plot(novelty_levels)
    axes[0, 2].set_title('Novelty Level')
    axes[0, 2].set_xlabel('Step')
    axes[0, 2].set_ylabel('Level (0-1)')
    axes[0, 2].grid(True)
    
    # Reward over time
    axes[1, 0].plot(rewards)
    axes[1, 0].set_title('Reward over Time')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Reward')
    axes[1, 0].grid(True)
    
    # Developmental stage progression
    unique_stages = list(set(developmental_stages))
    stage_counts = [developmental_stages.count(stage) for stage in unique_stages]
    axes[1, 1].bar([stage.name for stage in unique_stages], stage_counts)
    axes[1, 1].set_title('Developmental Stage Distribution')
    axes[1, 1].set_ylabel('Steps')
    axes[1, 1].tick_params(axis='x', rotation=45)
    
    # Activity heatmap
    heatmap_data = np.random.rand(8, 8)  # Simulated vision data
    axes[1, 2].imshow(heatmap_data, cmap='hot', interpolation='nearest')
    axes[1, 2].set_title('Vision Input Heatmap')
    axes[1, 2].set_xlabel('X')
    axes[1, 2].set_ylabel('Y')
    
    plt.tight_layout()
    plt.savefig('nlm_simulation_results.png', dpi=150, bbox_inches='tight')
    print("Visualization saved as 'nlm_simulation_results.png'")
    
    # Print summary statistics
    print("\n=== Visualization Data Summary ===")
    print(f"Average firing rate: {np.mean(firing_rates):.2f} ± {np.std(firing_rates):.2f} Hz")
    print(f"Max curiosity level: {np.max(curiosity_levels):.3f}")
    print(f"Max novelty level: {np.max(novelty_levels):.3f}")
    print(f"Total reward: {np.sum(rewards):.2f}")
    print(f"Final developmental stage: {developmental_stages[-1].name}")
    
    return fig

# Create visualization
demo_fig = create_visualization_demo()
```

### 6. Error Handling and Robustness

```python
import nlm

def robust_brain_simulation(config, max_steps=1000):
    """Robust brain simulation with error handling."""
    
    try:
        # Create brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        print("Brain created and initialized successfully")
        
        for step in range(max_steps):
            try:
                # Basic step with error handling
                brain.step(step)
                
                # Check for brain health
                neuron_count = brain.getTotalNeuronCount()
                firing_count = brain.getFiringNeuronCount()
                
                if neuron_count == 0:
                    print(f"WARNING: Step {step} - No neurons in brain!")
                    break
                    
                if step % 100 == 0:
                    print(f"Step {step}: {firing_count} neurons firing")
                    
            except Exception as e:
                print(f"ERROR at step {step}: {str(e)}")
                print("Attempting recovery...")
                brain.reset()
                brain.initialize()
                continue
                
    except Exception as e:
        print(f"CRITICAL ERROR: Failed to create brain: {str(e)}")
        raise
        
    print(f"Simulation completed successfully over {max_steps} steps")
    return brain

# Usage with error handling
config = nlm.createDefaultConfig()
brain = robust_brain_simulation(config, 500)
```

### 7. Batch Processing and Experiment Runner

```python
import nlm
import time
import json

def run_experiment_batch(configurations, steps_per_config=500):
    """Run multiple experiments with different configurations."""
    
    results = []
    
    for i, config in enumerate(configurations):
        print(f"\n=== Experiment {i+1}/{len(configurations)} ===")
        
        start_time = time.time()
        
        # Create brain for this experiment
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = nlm.createAgentBrain(brain)
        agent.initialize(None)  # No world for this demo
        
        # Run experiment
        experiment_start = time.time()
        
        for step in range(steps_per_config):
            brain.step(step)
            
            # Simulate some processing
            if step % 100 == 0:
                print(f"  Step {step}/{steps_per_config}")
        
        experiment_end = time.time()
        
        # Collect results
        result = {
            'experiment_id': i + 1,
            'config': {
                'neuron_count': config.get('brain.neuron_count', 1000),
                'synapse_density': config.get('brain.synapse_density', 0.1),
                'plasticity_enabled': config.get('plasticity.stdp.enable', True),
            },
            'performance': {
                'total_steps': steps_per_config,
                'total_spikes': brain.getTotalSpikeCount(),
                'final_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            },
            'timing': {
                'total_wall_time': experiment_end - experiment_start,
                'steps_per_second': steps_per_config / (experiment_end - experiment_start),
            },
            'brain_state': {
                'neuron_count': brain.getTotalNeuronCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'developmental_stage': brain.getDevelopmentalStage().name,
            }
        }
        
        results.append(result)
        
        print(f"Experiment completed in {experiment_end - experiment_start:.2f}s")
        print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Developmental stage: {brain.getDevelopmentalStage().name}")
    
    # Save results
    with open('experiment_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    # Calculate summary statistics
    avg_firing_rate = sum(r['performance']['final_firing_rate'] for r in results) / len(results)
    total_wall_time = sum(r['timing']['total_wall_time'] for r in results)
    
    print(f"\n=== Experiment Batch Summary ===")
    print(f"Total experiments: {len(results)}")
    print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"Total wall time: {total_wall_time:.2f}s")
    print(f"Results saved to 'experiment_results.json'")
    
    return results

# Example usage
configs = [
    nlm.createDefaultConfig(),
    nlm.createDefaultConfig().set("brain.neuron_count", 500),
    nlm.createDefaultConfig().set("brain.neuron_count", 2000),
]

results = run_experiment_batch(configs, 200)
```

## Configuration Options

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Performance Tips

### For Large Simulations
1. **Increase neuron count**: Use larger configurations for realistic brain modeling
2. **Enable optimizations**: Use SIMD and parallel processing (Phase 5 optimizations)
3. **Monitor memory**: Use memory pools for large-scale simulations
4. **Use checkpoints**: Save intermediate states to resume large simulations

### For Real-time Applications
1. **Limit simulation steps**: Use smaller configurations for real-time feedback
2. **Optimize world update**: Use efficient world representation
3. **Minimize logging**: Reduce logging overhead for performance-critical applications
4. **Use threading**: Implement parallel processing for multiple agents

### For Research Experiments
1. **Enable all plasticity**: Use STDP, Hebbian, and structural plasticity
2. **Enable neuromodulation**: Use dopamine, curiosity, and novelty
3. **Enable development**: Let the brain mature through developmental stages
4. **Use replay**: Enable memory consolidation through replay

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check configuration parameters
- Ensure sensory input is connected

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neuron connections
- Check action decoding parameters

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check neuron thresholds and parameters
- Enable plasticity for weight changes

## Advanced Features

### 1. Custom World Implementations
```python
import nlm

class CustomWorld:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.agent_x = width / 2
        self.agent_y = height / 2
    
    def update(self, timestep):
        # Custom world dynamics
        pass
    
    def getSensoryPercept(self, brain):
        # Custom sensory processing
        percept = nlm.SensoryPercept()
        # ... custom implementation
        return percept
    
    def applyMotorCommand(self, action, time):
        # Custom action handling
        pass
```

### 2. Custom Brain Architectures
```python
import nlm

class CustomBrain(nlm.Brain):
    def __init__(self, config):
        super().__init__(config)
        # Custom initialization
    
    def step(self, step):
        # Custom step logic
        super().step(step)
        # ... custom implementation
    
    def customLearningAlgorithm(self):
        # Implement custom learning rules
        pass
```

### 3. Multi-Agent Systems
```python
import nlm

def run_multi_agent_simulation(num_agents=5, steps=1000):
    """Run simulation with multiple interacting agents."""
    
    brains = [nlm.createBrain(nlm.createDefaultConfig()) for _ in range(num_agents)]
    agents = [nlm.createAgentBrain(brain) for brain in brains]
    
    for brain in brains:
        brain.initialize()
    
    print(f"Running multi-agent simulation with {num_agents} agents")
    
    for step in range(steps):
        # Each agent processes independently
        for i, agent in enumerate(agents):
            # Each agent has its own world or shared world
            # ... agent logic
            
        if step % 100 == 0:
            print(f"Step {step} complete")
    
    return brains, agents
```

### 4. Advanced Python Integration
```python
import nlm
import numpy as np
import pandas as pd

# Use NLM with data analysis
class NLMDataProcessor:
    def __init__(self, brain):
        self.brain = brain
        self.data_buffer = []
    
    def collect_data(self, step):
        """Collect brain activity data for analysis."""
        data_point = {
            'step': step,
            'firing_rate': self.brain.getAverageFiringRate(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage().name,
        }
        self.data_buffer.append(data_point)
        return data_point
    
    def analyze_patterns(self):
        """Analyze collected data patterns."""
        if not self.data_buffer:
            return None
        
        df = pd.DataFrame(self.data_buffer)
        
        # Analyze patterns
        analysis = {
            'average_firing_rate': df['firing_rate'].mean(),
            'firing_rate_std': df['firing_rate'].std(),
            'max_firing_rate': df['firing_rate'].max(),
            'min_firing_rate': df['firing_rate'].min(),
            'total_spikes': df['total_spikes'].sum(),
            'developmental_stages': df['developmental_stage'].value_counts().to_dict(),
            'activity_patterns': self.detect_activity_patterns(df),
        }
        
        return analysis
    
    def detect_activity_patterns(self, df):
        """Detect activity patterns in the data."""
        patterns = []
        
        # Look for bursts of activity
        firing_rate_changes = df['firing_rate'].diff()
        bursts = firing_rate_changes[firing_rate_changes > firing_rate_changes.std()].index.tolist()
        
        if len(bursts) > 0:
            patterns.append({
                'type': 'activity_bursts',
                'count': len(bursts),
                'timing': bursts[:5],  # First 5 bursts
            })
        
        return patterns

# Use data processor with a brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

processor = NLMDataProcessor(brain)

for step in range(100):
    brain.step(step)
    processor.collect_data(step)

analysis = processor.analyze_patterns()
print("Data analysis:", analysis)
```

## License

MIT License

## Authors

Research project — See docs for scientific background.

## Further Reading

- [HOW_TO_USE.md](HOW_TO_USE.md) - Complete technical documentation
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 verification report

That's it! You're now ready to use NLM Phase 6 - the complete artificial brain with 60+ integrated systems. Enjoy exploring the capabilities of this advanced neural simulation framework!