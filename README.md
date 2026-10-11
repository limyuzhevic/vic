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

### Phase 3 Demo (Basic World Interaction)
```bash
./nlm_phase3_demo 10 500
```

This demonstrates the complete loop: brain → actions → world changes → sensory input → reward → plasticity.

### Phase 4 Demo (Cognitive Experiments)
```bash
./nlm_phase4_demo 50
```

This runs Phase 4 cognitive experiments testing memory, prediction, attention, and learning.

## Quick Start

For beginners, here are the most common ways to get started:

### 1. Minimal C++ Example

```cpp
#include <pynlm.h>
#include <iostream>

int main() {
    // Create and initialize brain
    pynlm::Brain brain;
    brain.initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step 
                      << ": " << brain.getFiringNeuronCount() << " neurons firing" 
                      << std::endl;
        }
    }
    
    return 0;
}
```

### 2. Python API Example

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)
    
print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### 3. World Interaction Example

```python
import pynlm

# Setup brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Decode motor command
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Advanced Usage

### Custom Configuration

```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()

# Modify key parameters
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 0.5)
config.set("development.stage", pynlm.DevelopmentalStage.CriticalPeriod)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Multiple Brains and Regions

```cpp
// Create brain with multiple regions
auto config = std::make_shared<Config>();
config->set("neuron_count", 5000);
config->set("region_count", 3);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Add regions with different properties
brain->addRegion("Visual_Cortex");
brain->addRegion("Motor_Cortex"); 
brain->addRegion("Association_Cortex");

// Create agents for each region
auto visualAgent = std::make_shared<AgentBrain>(brain);
auto motorAgent = std::make_shared<AgentBrain>(brain);
```

### Memory Management

```cpp
#include <pynlm.h>

// Save and load brain state
brain.save("checkpoint.bin");

// Later...
brain.load("checkpoint.bin");
```

## Command Line Options

### Basic Usage
```bash
# Use custom config file
./nlm --config=my_config.cfg

# Override parameters
./nlm --neuron_count=2000 --plasticity_learning_rate=0.05

# Run specific demo
./nlm --demo=phase3 --steps=1000
```

### Configuration File Format

NLM supports multiple configuration file formats:

#### Simple Format (.cfg)
```
# Configuration comments are supported
brain.neuron_count = 1000
plasticity.stdp.enable = true
simulation_timestep = 0.001
```

#### JSON Format (.json)
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01
        }
    }
}
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Build system
├── README.md                          # This documentation
├── pyproject.toml                     # Python bindings build
├── docs/                              # Documentation
│   ├── ARCHITECTURE.md                # Architecture details
│   ├── SCIENCE.md                     # Scientific background
│   ├── ROADMAP.md                     # Development roadmap
│   ├── EXPERIMENTS.md                 # Experiment descriptions
│   └── PHASE6_FINAL_AUDIT.md          # Phase 6 integration status
├── src/                               # Source code
│   ├── core/                          # Core utilities
│   ├── brain/                         # Neural components
│   ├── dynamics/                      # Neural dynamics
│   ├── plasticity/                    # Plasticity rules
│   ├── development/                   # Developmental system
│   ├── neuromodulation/                # Neuromodulators
│   ├── memory/                        # Memory systems
│   ├── prediction/                    # Prediction systems
│   ├── cognition/                     # Cognitive mechanisms
│   ├── sensory/                       # Sensory processing
│   ├── motor/                         # Motor system
│   ├── environment/                   # Environment interface
│   └── experiments/                   # Experiment framework
├── tests/                             # Unit and integration tests
├── python/                            # Python bindings
├── configs/                           # Configuration files
└── build/                             # Build directory (ignored)
```

## Phase 6 Integration Status

**Phase 6: FINAL INTEGRATION** - All systems integrated into coherent artificial brain:

### ✓ Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

### ✓ Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

### ✓ Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

### ✓ Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

### ✓ Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

### ✓ Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

### ✓ Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Performance Considerations

### Memory Usage
- **Neuron Memory**: ~100-200 bytes per neuron
- **Synapse Memory**: ~40-80 bytes per synapse (weight, delay, type)
- **Working Memory**: Additional ~10-20 bytes per active neuron
- **Episodic Memory**: ~500-1000 bytes per episode

### Computational Complexity
- **Neuron Updates**: O(N) where N is neuron count
- **Synapse Updates**: O(S) where S is synapse count
- **Spike Processing**: O(P) where P is pending spike events
- **Plasticity Updates**: O(S) for each plasticity rule

### Scaling Recommendations

#### For Small Brains (< 1,000 neurons)
```bash
# Debug build with validation
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

#### For Medium Brains (1,000 - 100,000 neurons)
```bash
# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -j4
```

#### For Large Brains (> 100,000 neurons)
```bash
# Release build with SIMD and caching
cmake .. -DCMAKE_BUILD_TYPE=Release -j$(nproc)
```

## Troubleshooting

### Common Issues

**Issue: "ImportError: No module named 'pynlm'"**

```bash
# Install the Python bindings
pip install pynlm
# or install from source
pip install -e .
```

**Issue: "Brain::initialize() failed"**

```bash
# Check if required files exist
ls -la configs/
```

**Issue: "Simulation runs too slowly"**

```bash
# Reduce brain size for faster execution
./nlm --neuron_count=500 --region_count=1
```

**Issue: "Memory usage too high"**

```bash
# Use a lighter configuration
./nlm --plasticity_learning_rate=0.001 --stdp_time_constant=10
```

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration details

## License

MIT License - See LICENSE file for details.

## Authors

Research project — See docs for scientific background.

## Support

For issues and questions:
1. Check the documentation in the `docs/` directory
2. Review experiment results in `docs/PHASE6_FINAL_RESULTS.md`
3. Join the NLM research community for discussions

**Note**: NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include simplified neuron models and limited scale compared to biological systems.

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
