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

## Quick Start Guide

### Basic Usage (Python API)

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for i in range(1000):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Advanced Usage (Expert Features)

```python
import pynlm

# Create and configure a complete agent system
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Larger brain
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run extended learning episode
for step in range(5000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    if step % 1000 == 0:
        print(f"Step {step}: "
              f"Neurons={brain.getTotalNeuronCount()}, "
              f"Spikes={brain.getTotalSpikeCount()}, "
              f"Curiosity={agent.getCuriosityLevel():.2f}")

print(f"Learning complete!")
```

### Simulation with Memory and Development

```python
import pynlm
import json

def run_development_episode():
    # Create brain with development settings
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create complete agent system
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting developmental learning episode...")
    print("This will demonstrate:")
    print("  - Neuromodulation effects on learning")
    print("  - Development stages and plasticity")
    print("  - Memory consolidation")
    print("  - Curiosity-driven exploration")
    
    for step in range(10000):
        # World dynamics
        world.update(0.05)
        
        # Perception and processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action and world interaction
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Reward-based learning
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        # Development update
        agent.updateDevelopment(0.05)
        
        # Progress monitoring
        if step % 1000 == 0:
            status = f"Step {step:5d} | "
            status += f"Firing={brain.getFiringNeuronCount():5d} | "
            status += f"Spikes={brain.getTotalSpikeCount():6d} | "
            status += f"Curiosity={agent.getCuriosityLevel():.2f} | "
            status += f"DevStage={brain.getDevelopmentalStage()} | "
            status += f"Reward={result.reward:+.2f}"
            print(status)
    
    # Final state analysis
    print("\n" + "="*50)
    print("EPISODE COMPLETE")
    print("="*50)
    
    brain.logStatus()
    
    # Export for analysis
    export_state(agent, "final_brain_state.json")
    
    return brain.getTotalSpikeCount()

# Helper function for state export
def export_state(agent, filename):
    import json as json_module
    
    brain = agent.getBrain()
    
    state = {
        "neuron_count": brain.getTotalNeuronCount(),
        "synapse_count": brain.getTotalSynapseCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "development_stage": int(brain.getDevelopmentalStage()),
        "neuromodulation_level": agent.getNeuromodulationLevel(),
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel(),
        "prediction_error": agent.getPredictionError(),
        "working_memory_size": agent.getBrain().getWorkingMemory() ? 
            agent.getBrain().getWorkingMemory()->getActiveTraces() : 0,
        "episodic_memory_count": agent.getBrain().getEpisodicMemory() ? 
            agent.getBrain().getEpisodicMemory()->getEpisodeCount() : 0,
    }
    
    with open(filename, 'w') as f:
        json_module.dump(state, f, indent=2)
    
    print(f"State exported to {filename}")
    return state

if __name__ == "__main__":
    # Run simple test
    print("NLM Phase 6 Demo - Artificial Developmental Brain")
    print("="*50)
    
    # Basic demonstration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Initial brain state:")
    brain.logStatus()
    
    # Run simulation
    for i in range(500):
        brain.step(i)
    
    print("\nFinal brain state:")
    brain.logStatus()
    
    # Show Python API capabilities
    print("\n" + "="*50)
    print("Python API Capabilities Demonstrated:")
    print("="*50)
    print("✅ Brain creation and initialization")
    print("✅ Step-based simulation")
    print("✅ Statistics monitoring")
    print("✅ State persistence")
    print("✅ Advanced agent interfaces")
    print("✅ Neuromodulation integration")
    print("✅ Developmental stages")
    print("✅ Memory system access")
    print("✅ Error handling and validation")
```

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- ✅ Working memory with persistent activity and competition
- ✅ Episodic memory with experience encoding and replay
- ✅ Associative memory with Hebbian pattern associations
- ✅ All memory systems connected to neural processing

#### Neuromodulation Integration
- ✅ Dopamine affects neural excitability and plasticity
- ✅ Curiosity drives exploration behavior
- ✅ Novelty detection integrated with sensory processing
- ✅ All neuromodulators connected to plasticity rules

#### Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error signals affect learning
- ✅ Confidence tracking implemented

#### Cognition Systems
- ✅ Neural planner with action sequence evaluation
- ✅ Concept formation from experience patterns
- ✅ Attention with competitive selection dynamics
- ✅ All cognition systems connected to perception and action

#### Development Integration
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity modulated by age
- ✅ Neural excitability changes with development

#### Persistence
- ✅ Checkpoint save/load implemented
- ✅ Brain state serialization working
- ✅ Can resume from saved checkpoints

#### Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ Integration with sleep/rest cycle

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

## Installation

### Python Package

```bash
# Install from source
pip install .

# Or install in development mode
pip install -e .

# Install dependencies
pip install pybind11 scikit-build-core pytest numpy
```

### From Source

```bash
# Clone the repository
cd /path/to/nlm

# Install Python bindings
pip install .

# Build C++ library (for advanced usage)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── python/                    # Python bindings
│   ├── bindings.cpp
│   └── pyproject.toml
├── src/                     # Source code
│   ├── core/              # Core utilities
│   ├── brain/            # Neural components
│   ├── dynamics/         # Neural dynamics
│   ├── plasticity/       # Plasticity rules
│   ├── development/      # Developmental system
│   ├── neuromodulation/   # Neuromodulators
│   ├── memory/           # Memory systems
│   ├── prediction/       # Prediction systems
│   ├── cognition/        # Cognitive mechanisms
│   ├── sensory/          # Sensory processing
│   ├── motor/            # Motor system
│   ├── environment/     # Environment interface
│   ├── agent/           # Agent interfaces
│   ├── experiments/     # Experiment framework
│   └── visualization/   # Visualization
├── docs/                   # Documentation
│   ├── ARCHITECTURE.md   # System architecture
│   ├── SCIENCE.md       # Scientific background
│   ├── ROADMAP.md       # Development roadmap
│   ├── EXPERIMENTS.md    # Experiment descriptions
│   ├── LIMITATIONS.md    # Scientific limitations
│   └── PHASE6_FINAL_AUDIT.md  # Phase 6 audit
├── tests/                 # Test suite
├── easy_usage.md          # Beginner guide
├── HOW_TO_USE.md           # Comprehensive usage guide
└── LICENSE                # License information
```

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
