# NLM Tutorial: Getting Started with Neural Learning Machine

## Introduction

NLM (熙然, "serene flow") is a comprehensive spiking neural network simulation system that implements a brain-inspired computational model. This tutorial will guide you through the fundamental concepts and practical usage of NLM.

## Quick Start

### Installation

**Option 1: From Source**

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build using the installer (recommended)
./install.sh

# Or build manually
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**Option 2: Install Python Bindings**

```bash
# Install via pip (requires building C++ core first)
pip install .

# Or install in development mode
pip install -e .
```

### Basic Python Usage

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run basic simulation
for i in range(100):
    brain.step(i)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()}")
```

### C++ Usage

```cpp
// Create a simple brain using CMake build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make nlm

// Run the main demo executable
./nlm
```

## Core Concepts

### 1. Neural Components

NLM implements several key neural components:

- **LIF Neurons**: Leaky Integrate-and-Fire neurons with membrane potential dynamics
- **Synapses**: Event-driven synaptic connections with plasticity rules
- **Neural Regions**: Organized groups of neurons and synapses
- **Populations**: Groups of neurons with similar properties

### 2. Plasticity Mechanisms

NLM supports multiple plasticity rules:

- **STDP**: Spike-Timing Dependent Plasticity
- **Hebbian Learning**: Correlation-based learning
- **Structural Plasticity**: Synaptogenesis and pruning

### 3. Memory Systems

The NLM brain includes several memory systems:

- **Working Memory**: Persistent activity for current information
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations through Hebbian learning
- **Prediction System**: Forward models for anticipating outcomes

### 4. Neuromodulation

 Neuromodulation signals that regulate brain activity:

- **Dopamine**: Reward prediction error and motivation
- **Curiosity**: Exploration drive and novelty seeking
- **Novelty**: Response to unexpected stimuli
- **Prediction Error**: Mismatch between expected and actual outcomes

### 5. Developmental Stages

The brain develops through several stages:

- **Initial**: Primitive neural circuits
- **Critical Period**: High plasticity for learning
- **Maturation**: Refinement of connections
- **Adult**: Stable, specialized networks
- **Aging**: Gradual decline and adaptation

## Tutorial 1: Basic Brain Simulation

### Creating and Running a Simple Brain

```python
import pynlm

# Step 1: Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)

# Step 2: Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Step 3: Run simulation
print("Running brain simulation...")
for step in range(1000):
    brain.step(step)

# Step 4: Examine results
print(f"Simulation completed!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### Simulation with World Interaction

```python
import pynlm

# Setup complete agent-world system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

agent.initialize(world)

# Run interactive simulation
for step in range(500):
    # 1. Update world
    world.update(0.1)
    
    # 2. Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # 3. Brain processes and decides
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # 4. Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 5. Monitor learning
    if step % 100 == 0:
        print(f"Step {step}: Firing {brain.getFiringNeuronCount()} neurons")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Reward: {world.getLastReward():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

## Tutorial 2: Learning and Plasticity

### Exploring STDP Learning

```python
import pynlm

# Create brain with strong STDP
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Apply correlated pre-post activity to induce STDP learning
# Simulate two neurons firing with precise timing
neuron_a = 0  # Pre-synaptic neuron
neuron_b = 1  # Post-synaptic neuron

print("Inducing STDP potentiation...")
for step in range(200):
    # Correlated activity: pre before post
    brain.injectCurrent(neuron_a, 20.0)
    brain.step(step, step * 0.001)
    
    brain.injectCurrent(neuron_b, 20.0)
    brain.step(step + 1, (step + 1) * 0.001)

# Check synaptic weight changes
if auto* region = brain.getRegion(1); region != nullptr {
    for (const auto& synapse : region->getSynapses()) {
        if (synapse->getSourceNeuron().index() == neuron_a &&
            synapse->getDestinationNeuron().index() == neuron_b) {
            float weight = synapse->getWeight();
            print(f"Synaptic weight changed to: {weight:.3f}")
            if (weight > 0.6) {
                print("✓ STDP potentiation successful!")
            }
        }
    }
}
```

### Memory Integration

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with memory
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable memory features
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run exploration to build memories
for episode in range(50):
    print(f"Episode {episode + 1}")
    
    # Reset for new episode
    world.reset()
    agent.reset()
    
    # Explore environment
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Access memory statistics
        if step % 50 == 0:
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")

# Analyze learned experiences
print("\nMemory Analysis:")
print(f"Final development stage: {brain.getDevelopmentalStage()}")
print(f"Learning preference: {agent.getLearnedPreference():.3f}")
print(f"Thought trace: {agent.getThoughtTrace()}")
```

## Tutorial 3: Advanced Features

### Using Checkpoint and Replay

```python
import pynlm

# Create and train brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Train for some time
train_steps = 1000
for step in range(train_steps):
    brain.step(step)

# Save the trained brain
print("Saving trained brain state...")
brain.save("trained_brain.bin")
print("Brain saved to trained_brain.bin")

# Later, load and continue training
print("Loading trained brain...")
brain.load("trained_brain.bin")

# Continue training from saved state
for step in range(train_steps, train_steps + 500):
    brain.step(step)

print("Training completed successfully!")
```

### Text as a Sense (Phase 6 Feature)

```python
import pynlm

# Create brain with text association capability
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Encode and inject text patterns
print("Encoding text pattern: 'Hello World'")
text_pattern = agent.encodeText("Hello World")

# Create internal signals from text
from pynlm import InternalSignals
internal_signals = InternalSignals()
for neuron in text_pattern:
    internal_signals.addSignal(1.0f)  # Normalized activation

# Inject into brain for association learning
brain.receiveSensoryInput(internal_signals)
brain.step(0)

print("Text association complete!")
print(f"Encoded {len(text_pattern)} neurons")
```

## Tutorial 4: Performance Benchmarking

### Measuring Brain Performance

```python
import pynlm
import time

# Create brain for benchmarking
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run benchmark
num_steps = 5000
print(f"Running benchmark: {num_steps} simulation steps")

start_time = time.time()
for step in range(num_steps):
    brain.step(step)
end_time = time.time()

# Calculate performance metrics
duration = end_time - start_time
steps_per_second = num_steps / duration
firing_rate = brain.getAverageFiringRate()

print(f"\nBenchmark Results:")
print(f"  Duration: {duration:.2f} seconds")
print(f"  Steps per second: {steps_per_second:.0f}")
print(f"  Average firing rate: {firing_rate:.2f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Memory efficiency: {steps_per_second / brain.getTotalNeuronCount():.2f} steps/neuron/sec")

# Save benchmark results
with open("benchmark_results.txt", "w") as f:
    f.write(f"Benchmark Results\n")
    f.write(f"==================\n")
    f.write(f"Duration: {duration:.2f} seconds\n")
    f.write(f"Steps per second: {steps_per_second:.0f}\n")
    f.write(f"Average firing rate: {firing_rate:.2f} Hz\n")
    f.write(f"Total spikes: {brain.getTotalSpikeCount()}\n")
    f.write(f"Memory efficiency: {steps_per_second / brain.getTotalNeuronCount():.2f} steps/neuron/sec\n")

print("Benchmark results saved to benchmark_results.txt")
```

## Tutorial 5: Complete Example - Agent in Environment

### Full Simulation with All Features

```python
import pynlm

def run_complete_simulation(config_file=None):
    """Run a complete NLM simulation with all features"""
    
    # 1. Configuration
    if config_file:
        config = pynlm.createDefaultConfig()
        # Load from file (simplified for example)
        config = pynlm.createBrainConfig()
    else:
        config = pynlm.createDefaultConfig()
    
    # Configure for interesting dynamics
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable", True)
    
    # 2. Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    
    # Enable all advanced features
    agent.enableAllFeatures()
    
    agent.initialize(world)
    
    # 4. Run simulation
    num_episodes = 10
    steps_per_episode = 1000
    
    print("Starting complete NLM simulation...")
    print(f"Episodes: {num_episodes}, Steps per episode: {steps_per_episode}")
    
    for episode in range(num_episodes):
        print(f"\nEpisode {episode + 1}/{num_episodes}")
        
        # Reset environment and agent
        world.reset()
        agent.reset()
        brain.reset()
        
        # Run episode
        for step in range(steps_per_episode):
            # World dynamics
            world.update(0.1)
            
            # Agent perception and action
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            action = agent.decodeMotorCommand()
            
            # World response
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learning (neuromodulation)
            if result and result->reward != 0.0:
                agent.applyRewardModulation(result->reward, 0.0)
            
            # Development progression
            agent.updateDevelopment(0.1)
            
            # Progress monitoring
            if step % 200 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} firing, "
                      f"curiosity={agent.getCuriosityLevel():.3f}, "
                      f"novelty={agent.getNoveltyLevel():.3f}")
        
        # Episode summary
        print(f"  Episode complete: Reward={world.getLastReward():.3f}, "
              f"DevStage={brain.getDevelopmentalStage()}, "
              f"Learning={agent.getLearnedPreference():.3f}")
    
    # 5. Results and analysis
    print("\n" + "="*50)
    print("SIMULATION COMPLETE - FINAL RESULTS")
    print("="*50)
    
    # Print comprehensive statistics
    print(f"Brain Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    print(f"\nAgent Statistics:")
    print(f"  Learned preference: {agent.getLearnedPreference():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    # Save results
    brain.save("final_brain_state.bin")
    print(f"\nFinal brain state saved to: final_brain_state.bin")
    
    return brain, agent, world

# Run the complete simulation
if __name__ == "__main__":
    run_complete_simulation()
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain doesn't fire any neurons**
- **Solution**: Ensure `brain.initialize()` is called before `brain.step()`
- **Solution**: Check that sensory input is being provided
- **Solution**: Verify neuron parameters are in valid ranges

**Issue: Simulation is too slow**
- **Solution**: Reduce neuron count for faster testing
- **Solution**: Use debug build with optimizations disabled
- **Solution**: Check for inefficient loops in custom code

**Issue: Memory usage is high**
- **Solution**: Monitor neuron and synapse counts
- **Solution**: Use checkpointing for long simulations
- **Solution**: Consider using smaller configurations

**Issue: Python bindings not found**
- **Solution**: Ensure NLM is built with Python bindings enabled
- **Solution**: Install with `pip install .` or `pip install -e .`
- **Solution**: Check Python path with `python3 -c "import pynlm; print(pynlm.__version__)"`

### Getting Help

- **Documentation**: Check `/workspace/app/docs/` for comprehensive guides
- **Python Examples**: Look in `/workspace/app/examples/` for Python code samples
- **C++ Examples**: Check `/workspace/app/src/experiments/` for C++ demo code
- **Community**: Join the NLM community for support and discussions

## Further Reading

### Recommended Documentation

1. **NLM Architecture** (`docs/ARCHITECTURE.md`) - Detailed system overview
2. **Phase 6 Final Audit** (`docs/PHASE6_FINAL_AUDIT.md`) - Technical assessment
3. **Scientific Background** (`docs/SCIENCE.md`) - Research foundation
4. **Development Roadmap** (`docs/ROADMAP.md`) - Future plans and features

### Advanced Topics

- **Performance Optimization**: (`docs/PERFORMANCE.md`) - How to optimize NLM performance
- **Biological Fidelity**: (`docs/BIOLOGICAL_FIDELITY.md`) - Comparison with biological systems
- **Limitations and Challenges**: (`docs/LIMITATIONS.md`) - Current limitations and future work

### Research and Applications

- **Experiment Framework**: (`docs/EXPERIMENTS.md`) - How to design NLM experiments
- **Scaling Studies**: (`docs/SCALING.md`) - Performance at different scales
- **Real-world Applications**: Investigate how NLM could be applied to:
  - Robotics and control systems
  - Cognitive modeling
  - Neuromorphic computing
  - Educational AI systems

## Conclusion

This tutorial provides a comprehensive introduction to NLM (熙然), the Neural Learning Machine. You've learned how to:

1. **Set up** and configure the NLM system
2. **Create** basic neural simulations
3. **Understand** the core concepts of spiking neural networks
4. **Implement** learning and plasticity mechanisms
5. **Use** advanced memory and neuromodulation systems
6. **Analyze** simulation results and performance
7. **Troubleshoot** common issues

NLM represents a step towards creating artificial neural systems that learn, develop, and adapt through experience, much like biological brains. The modular architecture allows for easy extension and modification, while the comprehensive documentation and examples make it accessible to researchers and developers at all levels.

For the most current information and updates, visit the NLM project repository and join the community of researchers working to understand and replicate the principles of biological intelligence through computational means.