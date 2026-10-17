# NLM (熙然) - Improved Usage Guide

**An Experimental Artificial Developmental Brain**

## Quick Start (10 minutes to get running)

### Option 1: Quick Demo (Phase 6 Integration Test)

```bash
# Build and run Phase 6 integration demo
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make nlm_phase6_demo
./nlm_phase6_demo
```

### Option 2: Python-based Usage

```python
import pynlm

# Quick setup - One line
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Done! Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Spiked {brain.getTotalSpikeCount()} times")
```

## What NLM Is

NLM (熙然, meaning "serene flow") is a brain simulator that creates virtual brains with:

- **Neurons** = Brain cells that send electrical signals
- **Synapses** = Connections between brain cells that strengthen/weakening
- **Learning** = When connections change based on experience
- **Memory** = The brain remembering patterns

**NOT**: A transformer, chatbot, or LLM - This is a spiking neural network!

## The 3 Essential Components

1. **Brain** - Virtual brain that thinks (contains neurons and synapses)
2. **World** - Environment where the brain operates
3. **Agent** - Bridge connecting brain to world

## Advanced Usage Examples

### Example 1: Learning in Environment

```python
def run_learning_episode(num_steps=500):
    """Complete agent learning cycle"""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.0, 7.0)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # Run episode
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from results
        reward = world.computeReward(brain)
        agent.applyRewardModulation(reward, 0.0)
    
    return brain, world, agent

# Use it
brain, world, agent = run_learning_episode(1000)
print(f"Learning complete: {agent.getCuriosityLevel():.2f} curiosity")
```

### Example 2: Advanced Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Fine-tune parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 2.0)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Custom brain: {brain.getTotalNeuronCount()} neurons")
```

### Example 3: Multiple Brains Simulation

```python
def run_multiple_brains(num_brains=5):
    """Run simulation with multiple independent brains"""
    
    brains = []
    for i in range(num_brains):
        # Each brain gets a different random seed
        config = pynlm.createDefaultConfig()
        config.set("random_seed", 42 + i)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Evolve all brains in parallel
    for step in range(100):
        for i, brain in enumerate(brains):
            # Inject different sensory patterns
            if i % 3 == 0:
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
            elif i % 3 == 1:
                brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 5.0)
            else:
                brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 3.0)
            
            brain.step(step)
    
    # Compare results
    for i, brain in enumerate(brains):
        print(f"Brain {i}: {brain.getTotalSpikeCount()} spikes, "
              f"{brain.getFiringNeuronCount()} firing neurons")
    
    return brains

# Run simulation
brains = run_multiple_brains(5)
```

## Power User Features

### 1. Checkpoint and Save/Load

```python
# Save brain state (checkpoint)
brain.save("my_checkpoint.bin")

# Later, load brain state
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.initialize()
new_brain.load("my_checkpoint.bin")

print("Brain state restored from checkpoint")
```

### 2. Advanced Brain Manipulation

```python
# Get detailed brain statistics
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "firing_rate": brain.getAverageFiringRate(),
    "e_i_ratio": brain.getExcitationInhibitionRatio(),
    "development_stage": brain.getDevelopmentalStage(),
    "working_memory": brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
    "episodic_memory": brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
}

# Modify brain parameters
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
brain.develop()  # Apply development effects
```

### 3. Command Line Interface

Use the C++ executable with advanced options:

```bash
./nlm_phase6_demo --help

# Example: Custom configuration
./nlm_phase6_demo --config=./my_config.cfg --neurons=2000 --steps=5000

# Example: Performance monitoring
./nlm_phase6_demo --neurons=1000 --steps=2000 --output=performance_test.bin
```

## Building from Source

### C++ Build (Recommended for Advanced Users)

```bash
# Full build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j$(nproc)

# Run Phase 6 integration demo
./nlm_phase6_demo

# Run tests
./nlm_test
```

### Python Build

```bash
# Install Python dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install Python module
pip install -e .

# Run Python examples
python -c "import pynlm; print(pynlm.__version__)"
```

## Performance Tips

### 1. For Large Networks

```python
# Use larger network for complex tasks
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.08)

# Optimize simulation steps
for step in range(0, 5000, 10):  # Every 10 steps
    brain.step(step)
```

### 2. For Memory Efficiency

```python
# Use smaller networks for quick testing
config.set("brain.neuron_count", 100)
brain = pynlm.createBrain(config)

# Limit simulation time
for step in range(100):
    brain.step(step)
```

### 3. Parallel Processing

```python
# For multiple independent simulations (C++)
# See the agent-based example for parallel execution patterns
```

## Troubleshooting

### "My brain isn't doing anything"

- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected

### "The agent isn't moving"

- Did you call `world.applyMotorCommand()`?
- Ensure `world.update()` is being called
- Check agent configuration

### "Everything is 0"

- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check initial conditions and configuration

### "Performance issues"

- Use smaller networks for testing
- Optimize simulation timestep
- Check for infinite loops

## Advanced Examples

### 1. Neuroevolution (Brain Evolution)

```python
def evolve_brains(population_size=10, generations=50):
    """Simple neuroevolution example"""
    
    best_brain = None
    best_score = -float('inf')
    
    for gen in range(generations):
        population = []
        scores = []
        
        # Create population
        for i in range(population_size):
            config = pynlm.createDefaultConfig()
            config.set("random_seed", gen * 100 + i)
            config.set("brain.neuron_count", 200)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            population.append(brain)
        
        # Evaluate each brain
        for i, brain in enumerate(population):
            # Simple test: run simulation and count spikes
            for step in range(100):
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
                brain.step(step)
            
            score = brain.getTotalSpikeCount()
            scores.append(score)
            
            if score > best_score:
                best_score = score
                best_brain = brain
        
        print(f"Generation {gen}: Best score = {best_score}")
        
        # Simple selection (keep best)
        # (Full neuroevolution would include crossover and mutation)
    
    return best_brain

# Run evolution
evolved_brain = evolve_brains()
print(f"Evolution complete! Best brain: {evolved_brain.getTotalSpikeCount()} spikes")
```

### 2. Multi-Agent Environment

```python
def run_multi_agent_simulation(num_agents=5, steps=200):
    """Simulation with multiple agents in shared world"""
    
    # Shared environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    
    # Create multiple agents
    agents = []
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        config.set("random_seed", i * 42)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Each agent has different personality
        agent.enableCuriosity(True if i % 2 == 0 else False)
        agent.enableRewardModulation(True)
        
        agents.append(agent)
    
    # Set different starting positions
    for i, agent in enumerate(agents):
        start_x = 3 + i * 3
        start_y = 3 + i * 2
        world.setAgentStart(start_x, start_y)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        
        # Each agent senses and acts
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learn from shared environment
            reward = world.computeReward(agent.getBrain())
            agent.applyRewardModulation(reward, 0.0)
    
    return agents

# Run multi-agent simulation
agents = run_multi_agent_simulation(5, 100)
for i, agent in enumerate(agents):
    print(f"Agent {i}: Curiously={agent.getCuriosityLevel():.2f}, "
          f"Dopamine={agent.getNeuromodulationLevel():.2f}")
```

## Next Steps

When you're comfortable:

1. **Read `docs/ARCHITECTURE.md`** - Understand the system architecture
2. **Read `docs/SCIENCE.md`** - Scientific background
3. **Experiment with different configurations** - Try various parameter sets
4. **Study `docs/EXPERIMENTS.md`** - Learn about different experiments
5. **Join the research community** - Contribute to the project

## Quick Reference

| What you want | Code |
|---------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

## Command Line Options

```bash
./nlm_phase6_demo [options]

Options:
  --config=file.cfg        Load configuration from file
  --help, -h              Show help message
  --version               Show version info
  --neurons=N             Set neuron count (default: 500)
  --steps=N               Set simulation steps (default: 1000)
  --seed=N                Set random seed (default: 42)
  --output=file            Save brain state to file
```

That's it! You're now ready to use NLM for advanced brain simulation and research.