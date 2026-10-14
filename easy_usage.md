# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Advanced Command Line Usage

```bash
# Run different execution modes
./nlm --mode basic          # Test basic neural connectivity
./nlm --mode plasticity     # Test learning and plasticity
./nlm --mode stdp          # Test only STDP learning
./nlm --mode full_test     # Run complete integration test (default)

# Configure brain parameters
./nlm --neurons 2000 --steps 500 --timestep 0.001
./nlm --enable-plasticity --stdp-only

# Save and load brain states
./nlm --save brain_state.bin
./nlm --load brain_state.bin

# Advanced configuration
./nlm --neurons 5000 --enable-replay --development-stage adult
./nlm --min-weight -0.5 --max-weight 0.5
./nlm --verbose --log simulation.log
```

## Python API Advanced Usage

### Configuration Templates

#### Quick Start (Copy & Paste)

```python
import pynlm

# Step 1: Make a brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("memory.enable", True)
config.set("neuromodulation.enable", True)

# Step 2: Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Advanced Brain Configuration

```python
import pynlm

# Create different brain configurations

def create_research_brain():
    """Create a brain optimized for research"""
    config = pynlm.createDefaultConfig()
    
    # High capacity for complex learning
    config.set("brain.neuron_count", 10000)
    config.set("brain.region_count", 4)
    config.set("brain.connection_probability", 0.2)
    
    # Enable all learning systems
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.threshold", 0.5)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    # Memory systems
    config.set("memory.enable", True)
    config.set("memory.episodic_max_episodes", 50000)
    config.set("memory.working_memory_capacity", 1000)
    
    # Neuromodulation
    config.set("neuromodulation.enable", True)
    config.set("neuromodulation.curiosity.enable", True)
    config.set("neuromodulation.curiosity.exploration_rate", 0.1)
    config.set("neuromodulation.novelty.enable", True)
    config.set("neuromodulation.novelty.sensitivity", 0.5)
    
    # Development
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.0)
    config.set("development.aging_rate", 0.01)
    
    # Performance optimization
    config.set("performance.optimization", "aggressive")
    config.set("performance.simulation_timestep", 0.0001)
    config.set("checkpoint_count", 20)
    
    return config

def create_experimental_brain():
    """Create a brain for experimental studies"""
    config = pynlm.createDefaultConfig()
    
    # Small brain for controlled experiments
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 2)
    config.set("brain.connection_probability", 0.1)
    
    # Specific plasticity configurations
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    config.set("plasticity.hebbian.enable", False)  # Only STDP
    config.set("plasticity.structural.enable", True)
    
    # Memory for experience recording
    config.set("memory.enable", True)
    config.set("memory.episodic_max_episodes", 1000)
    config.set("memory.enable_replay", True)
    
    # Limited neuromodulation for focused study
    config.set("neuromodulation.enable", True)
    config.set("neuromodulation.curiosity.enable", False)
    config.set("neuromodulation.novelty.enable", False)
    
    # No development for stable experiments
    config.set("development.enable", False)
    
    return config

# Usage
research_brain_config = create_research_brain()
experimental_brain_config = create_experimental_brain()
```

### Advanced Simulation Patterns

#### Pattern 1: Learning Experiment

```python
import pynlm
import time

def run_learning_experiment(brain_config, world_config, steps=1000):
    """Run a comprehensive learning experiment"""
    
    # Create brain
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track metrics
    metrics = {
        'total_reward': 0.0,
        'max_firing_rate': 0.0,
        'memory_episodes': 0,
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': []
    }
    
    start_time = time.time()
    
    # Run simulation
    for step in range(steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.computeReward(agent.getBrain().getRegions()[0].get())
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        metrics['total_reward'] += reward
        metrics['max_firing_rate'] = max(metrics['max_firing_rate'], brain.getAverageFiringRate())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['novelty_levels'].append(agent.getNoveltyLevel())
        metrics['development_stages'].append(brain.getDevelopmentalStage())
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: Reward={reward:.3f}, "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    end_time = time.time()
    metrics['wall_clock_time'] = end_time - start_time
    metrics['memory_episodes'] = brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
    metrics['avg_firing_rate'] = metrics['max_firing_rate'] / steps
    metrics['avg_reward'] = metrics['total_reward'] / steps
    
    return metrics, brain, agent, world

# Run experiment
experiment_config = create_research_brain()
world_config = {
    'width': 30,
    'height': 30,
    'visionWidth': 8,
    'visionHeight': 8
}

metrics, brain, agent, world = run_learning_experiment(experiment_config, world_config, 500)

print(f"\nExperiment Results:")
print(f"Total reward: {metrics['total_reward']:.2f}")
print(f"Average reward: {metrics['avg_reward']:.4f}")
print(f"Max firing rate: {metrics['max_firing_rate']:.4f}")
print(f"Average firing rate: {metrics['avg_firing_rate']:.4f}")
print(f"Memory episodes: {metrics['memory_episodes']}")
print(f"Wall clock time: {metrics['wall_clock_time']:.2f}s")
```

#### Pattern 2: Comparative Experiment

```python
import pynlm
from collections import defaultdict

def run_comparative_experiment():
    """Compare different brain configurations"""
    
    # Define brain configurations to compare
    configurations = {
        'Minimal': create_minimal_brain(),
        'Balanced': create_balanced_brain(),
        'HighPerformance': create_high_performance_brain(),
        'Research': create_research_brain()
    }
    
    results = defaultdict(dict)
    
    for name, config in configurations.items():
        print(f"\nRunning configuration: {name}")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record results
        results[name]['final_firing_rate'] = brain.getAverageFiringRate()
        results[name]['total_spikes'] = brain.getTotalSpikeCount()
        results[name]['curiosity_level'] = agent.getCuriosityLevel()
        results[name]['novelty_level'] = agent.getNoveltyLevel()
        results[name]['memory_episodes'] = brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
        
        print(f"  Firing rate: {results[name]['final_firing_rate']:.4f}")
        print(f"  Total spikes: {results[name]['total_spikes']}")
        print(f"  Curiosity: {results[name]['curiosity_level']:.3f}")
        print(f"  Novelty: {results[name]['novelty_level']:.3f}")
        print(f"  Memory episodes: {results[name]['memory_episodes']}")
    
    return results

# Analyze results
results = run_comparative_experiment()

print(f"\nComparative Analysis:")
print("=" * 50)

best_firing = max(results.items(), key=lambda x: x[1]['final_firing_rate'])
worst_firing = min(results.items(), key=lambda x: x[1]['final_firing_rate'])

print(f"Best firing rate: {best_firing[0]} ({best_firing[1]['final_firing_rate']:.4f})")
print(f"Worst firing rate: {worst_firing[0]} ({worst_firing[1]['final_firing_rate']:.4f})")
print(f"Firing rate ratio: {best_firing[1]['final_firing_rate'] / worst_firing[1]['final_firing_rate']:.2f}x")

best_memory = max(results.items(), key=lambda x: x[1]['memory_episodes'])
print(f"Most memory episodes: {best_memory[0]} ({best_memory[1]['memory_episodes']})")
```

### Troubleshooting

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if `enableRewardModulation()` is enabled for learning

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Make sure the agent is configured to produce actions

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if `enableCuriosity()` is enabled for exploration

#### Performance Issues
```python
# For large simulations, optimize configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)  # Reduced for performance
config.set("performance.optimization", "aggressive")
config.set("memory.enable", False)  # Disable memory if not needed
config.set("checkpoint_count", 5)
```

#### Debugging Tips
```python
# Add debugging output
print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
print(f"Brain state: {brain.getDevelopmentalStage()}")
print(f"Working memory: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0} traces")

# Check system health
if brain.getTotalSynapseCount() == 0:
    print("ERROR: No synapses created!")
elif brain.getAverageFiringRate() == 0:
    print("WARNING: No neurons are firing. Check connectivity.")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more technical details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the advanced experimental patterns in this guide

That's it! You're now ready to use NLM at an advanced level.
