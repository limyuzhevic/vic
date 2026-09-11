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

## Quick Start Guide

To get started with NLM:

### 1. Build and Install

```bash
# Build the C++ framework
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install .
```

### 2. Basic Usage (Python)

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 3. Environment Simulation

```python
import pynlm

# Create complete agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Quick Reference Commands

| Command | Description |
|---------|-------------|
| `pynlm.createBrain()` | Create a neural brain |
| `pynlm.createAgentBrain()` | Create brain-agent interface |
| `pynlm.createSimpleWorld()` | Create simulation environment |
| `brain.initialize()` | Initialize brain state |
| `brain.step()` | Process one simulation step |
| `brain.save()` | Save brain state |
| `brain.load()` | Load brain state |

## Common Patterns

### 1. Silent Brain (Testing)
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
print("Brain test complete")
```

### 2. Learning Agent
```python
def run_learning_episode(brain, world, agent, steps=200):
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    return brain.getTotalSpikeCount()
```

### 3. Checkpointing
```python
# Save brain state
brain.save("my_brain_checkpoint.bin")

# Load saved state
brain.load("my_brain_checkpoint.bin")
```
## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Quick Start with Python

### Install Python package (from source)

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .

# Or in development mode
pip install -e .
```

### Quick Usage Examples

```python
import pynlm

# 1. Create a simple brain (silence test)
print("=== Test 1: Simple Brain ===")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(50):
    brain.step(step)

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

# 2. Complete agent in environment
print("\n=== Test 2: Learning Agent ===")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation loop
for step in range(200):
    world.update(0.1)
    
    # Agent perceives world
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes and thinks
    brain.step(step)
    
    # Agent decides action
    action = agent.decodeMotorCommand()
    
    # Environment responds
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

# 3. Save and load checkpoint (Phase 6 feature)
print("\n=== Test 3: Checkpoint Persistence ===")
brain.save("test_brain_checkpoint.bin")

# Create new brain and load checkpoint
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.load("test_brain_checkpoint.bin")
print("Successfully saved and loaded brain state")

# 4. Complex experiment with multiple agents
print("\n=== Test 4: Multi-Agent Environment ===")

def run_experiment(experiment_config):
    """Run an experiment with configurable parameters."""
    config = pynlm.createDefaultConfig()
    
    # Configure brain parameters
    config.set("brain.neuron_count", experiment_config.get("neuron_count", 500))
    config.set("brain.synapse_density", experiment_config.get("synapse_density", 0.05))
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(
        width=experiment_config.get("world_width", 30),
        height=experiment_config.get("world_height", 30),
        visionWidth=experiment_config.get("vision_width", 8),
        visionHeight=experiment_config.get("vision_height", 8)
    )
    world.reset()
    agent.initialize(world)
    
    # Enable features
    for feature in experiment_config.get("features", []):
        method = getattr(agent, f"enable{feature.title()}Plasticity", None)
        if method:
            method(True)
    
    # Run experiment
    results = []
    for step in range(experiment_config.get("steps", 100)):
        world.update(experiment_config.get("timestep", 0.1))
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics
        results.append({
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'total_spikes': brain.getTotalSpikeCount()
        })
    
    return results

# Example experiment configurations
experiments = {
    "high_plasticity": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "steps": 150,
        "features": ["RewardModulation", "StructuralPlasticity"]
    },
    "curiosity_driven": {
        "neuron_count": 800,
        "synapse_density": 0.08,
        "steps": 200,
        "features": ["RewardModulation", "Curiosity", "Development"]
    }
}

for exp_name, config in experiments.items():
    print(f"\nRunning experiment: {exp_name}")
    results = run_experiment(config)
    
    # Analyze results
    avg_firing = sum(r['firing_rate'] for r in results) / len(results)
    max_curiosity = max(r['curiosity'] for r in results)
    final_spikes = results[-1]['total_spikes']
    
    print(f"  Average firing rate: {avg_firing:.2f} Hz")
    print(f"  Max curiosity level: {max_curiosity:.3f}")
    print(f"  Final spike count: {final_spikes}")
    print(f"  Steps completed: {len(results)}")

# 5. Brain statistics and monitoring
print("\n=== Test 5: Brain Monitoring ===")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for a while
for step in range(100):
    brain.step(step)

# Get comprehensive brain statistics
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "pending_events": brain.getPendingSpikeEventCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "e_i_ratio": brain.getExcitationInhibitionRatio(),
    "developmental_stage": brain.getDevelopmentalStage(),
}

print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# 6. Utility functions for working with brains
print("\n=== Test 6: Brain Utilities ===")

# Create brains with different configurations
configs = {
    "small": pynlm.createDefaultConfig(),
    "medium": pynlm.createDefaultConfig(),
    "large": pynlm.createDefaultConfig()
}

# Configure each
configs["medium"].set("brain.neuron_count", 500)
configs["large"].set("brain.neuron_count", 2000)

brains = {name: pynlm.createBrain(config) for name, config in configs.items()}

# Initialize all brains
for name, brain in brains.items():
    brain.initialize()

# Create a utility function to process multiple brains
def batch_process(brains_dict, steps):
    """Process multiple brains in batch."""
    results = {}
    for name, brain in brains_dict.items():
        for step in range(steps):
            brain.step(step)
        results[name] = {
            "neurons": brain.getTotalNeuronCount(),
            "spikes": brain.getTotalSpikeCount(),
            "firing": brain.getFiringNeuronCount(),
            "rate": brain.getAverageFiringRate()
        }
    return results

# Process brains in batch
batch_results = batch_process(brains, 50)

print("Batch Processing Results:")
for name, result in batch_results.items():
    print(f"  {name}:")
    print(f"    Neurons: {result['neurons']}")
    print(f"    Total spikes: {result['spikes']}")
    print(f"    Current firing: {result['firing']}")
    print(f"    Average firing rate: {result['rate']:.2f}")

print("\n=== All tests completed successfully! ===")

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
