# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is a computational research project implementing a brain-inspired neural system. It creates artificial developmental brains that begin in primitive states and acquire increasingly complex abilities through environmental interaction. Unlike traditional AI, NLM implements a true developmental brain with neural dynamics, plasticity, and memory systems.

## Current Status

**PHASE 6: FINAL INTEGRATION - ACTIVE**

Phase 6 successfully integrates all major cognitive systems into a coherent artificial brain architecture. The Phase 6 demo program (`nlm_phase6_demo`) provides a comprehensive integration test.

## Quick Start

### Python Installation and Usage (Recommended)

```bash
# Install the Python bindings
pip install .

# Or for development (with debugging)
pip install -e .
```

### Python API Example

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Neurons firing: {brain.getFiringNeuronCount()}")
```

## Building from Source

### Prerequisites

- C++17 compiler (GCC 11+, Clang 12+, or MSVC 2019+)
- CMake 3.16+
- Python 3.8+ (for bindings)

### Build Steps

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Build Outputs

After compilation, you'll have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |
| `nlm_test` | Executable | Unit test suite |

## Running Tests and Demos

### Phase 6 Integration Demo

```bash
./nlm_phase6_demo
```

This comprehensive demo verifies all brain systems are properly connected and reports integration status.

### Unit Tests

```bash
cd build
ctest --output-on-failure
```

### Python Tests

```bash
pytest
```

## Project Structure

### Core Architecture

```
NLM Project
├── src/
│   ├── brain/              # Neural core (LIF neurons, synapses, plasticity)
│   ├── agent/              # Agent integration layer
│   ├── memory/            # Working memory, episodic memory
│   ├── neuromodulation/    # Dopamine, serotonin, norepinephrine, ACh
│   ├── prediction/         # Prediction system
│   ├── cognition/         # Neural planner, concept formation, self-model
│   ├── development/        # Synaptogenesis, pruning, maturation
│   ├── sensory/           # Vision, touch, internal signals
│   ├── motor/             # Action selection
│   ├── environment/       # Simple world simulation
│   └── experiments/       # Experiment framework
├── python/                 # Python bindings (pynlm module)
│   └── pynlm/             # Pybind11 module source
├── docs/                   # Comprehensive documentation
├── tests/                 # Unit tests
└── configs/               # Configuration files
```

## Phase 6 Integration Results

Based on extensive Phase 6 testing and the audit in `docs/PHASE6_FINAL_AUDIT.md`, here's the current integration status:

### Successfully Integrated Systems

| System | Status | Key Features |
|--------|--------|--------------|
| **Neural Core** | ✅ WORKING | LIF neurons, spike propagation, STDP, Hebbian, structural plasticity |
| **Working Memory** | ✅ CONNECTED | Persistent activity, competition, updated each step |
| **Episodic Memory** | ✅ CONNECTED | Stores experiences, replays every 100 steps |
| **Neuromodulation** | ✅ MOSTLY CONNECTED | Dopamine modulates plasticity, curiosity drives exploration |
| **Prediction System** | ✅ CONNECTED | Integrated with sensory processing and learning |
| **Attention** | ✅ CONNECTED | Competitive selection dynamics |
| **Development** | ✅ CONNECTED | Age-based plasticity modulation, synaptogenesis |
| **Replay** | ✅ CONNECTED | Episodic memory replay during sleep/rest |
| **Checkpointing** | ⚠️ PARTIAL | System exists but limited functionality |

### Still Disconnected Systems

| System | Status | Integration Priority |
|--------|--------|---------------------|
| Neural Planner | ❌ NOT USED | Medium priority |
| Concept Formation | ❌ NOT ACTIVE | Medium priority |
| Self-Model | ❌ NOT INTEGRATED | Low priority |
| Social Learning | ❌ NOT ACTIVE | Low priority |

## Scientific Evaluation (Phase 6 Results)

From `docs/PHASE6_FINAL_RESULTS.md`, Phase 6 achieved:

- **Average Firing Rate**: 0.1-1.0 Hz (sparse, event-driven)
- **Working Memory Traces**: 10-50 items (depends on activity)
- **Episodic Episodes**: 100-500 per 10,000 steps
- **Synaptic Updates**: ~1 million per step with 1K neurons
- **Integration Score**: 35.8% (43/120), up from 0%

### What's Working Well

✅ **Neural Dynamics**
- LIF neurons respond appropriately to inputs
- Spike propagation with delays
- Excitation/inhibition balance maintained
- Event-driven, sparse processing

✅ **Basic Learning**
- STDP modifies synapses appropriately
- Hebbian learning strengthens correlations
- Structural plasticity adds/removes connections
- Reward-based learning occurs

### Areas Requiring Further Work

⚠️ **Memory**
- Working memory traces decay quickly
- Episodic memories remain sparse
- Long-term memory consolidation limited

⚠️ **Cognition**
- No goal-directed planning
- Limited concept formation
- Little generalization capability

⚠️ **Integration**
- Systems don't strongly influence each other
- Limited recurrent connectivity for maintenance
- More sophisticated modulation needed

## Architecture Philosophy

NLM implements a closed-loop brain architecture where:

1. **Sensory Input** from environment drives neural activity
2. **Neural Processing** (spiking dynamics, plasticity) transforms input
3. **Working Memory** maintains relevant information
4. **Prediction** anticipates future states
5. **Neuromodulation** (dopamine, curiosity) guides exploration
6. **Action Selection** produces motor commands
7. **World Interaction** provides feedback
8. **Plasticity** updates neural connections
9. **Development** modulates learning rates

## Development Status

### Current State

- **Phase 6 Integration**: Mostly complete (65% functional systems)
- **Code Quality**: Sophisticated architecture with extensive documentation
- **Testing**: Comprehensive test suite including Phase 6 integration tests
- **Python Bindings**: Full API available via `pynlm` module
- **Documentation**: Extensive docs in `/docs/` directory

### Next Steps

Priority 1: **Refine Integration**

1. Connect remaining disconnected systems (planner, concept formation, self-model)
2. Implement proper neuromodulation for all transmitters (ACh, NE, 5-HT)
3. Add sleep/wake cycle for memory consolidation
4. Implement synaptic scaling and metaplasticity
5. Connect planner to action selection

Priority 2: **Validation**

1. Test memory retention over time
2. Measure prediction accuracy improvement
3. Test continual learning without catastrophic forgetting
4. Validate developmental progression
5. Test multi-system interaction effects

## Python API Usage

### Basic Examples

#### Example 1: Silent Brain

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
```

#### Example 2: Complete Agent with World

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### Key Configuration Options

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total neurons |
| `brain.synapse_density` | float | 0.1 | Connection probability |
| `plasticity.stdp.enable` | bool | true | STDP learning |
| `plasticity.hebbian.enable` | bool | true | Hebbian learning |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |

### Python Classes

- **Brain**: Neural computation, plasticity, memory management
- **AgentBrain**: Sensory processing, motor decoding, neuromodulation
- **SimpleWorld**: Environment simulation, agent interaction
- **Config**: Configuration management, parameter setting

## Documentation

### Available Documentation

- [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) - System architecture overview
- [`docs/SCIENCE.md`](docs/SCIENCE.md) - Scientific background and foundations
- [`docs/PHASE6_FINAL_AUDIT.md`](docs/PHASE6_FINAL_AUDIT.md) - Detailed Phase 6 integration audit
- [`docs/PHASE6_FINAL_RESULTS.md`](docs/PHASE6_FINAL_RESULTS.md) - Phase 6 experimental results
- [`docs/EXPERIMENTS.md`](docs/EXPERIMENTS.md) - Experiment descriptions
- [`docs/LIMITATIONS.md`](docs/LIMITATIONS.md) - Current limitations and challenges

### Additional Resources

- [`easy_usage.md`](easy_usage.md) - Beginner-friendly usage guide
- [`HOW_TO_USE.md`](HOW_TO_USE.md) - Comprehensive API documentation

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. It is NOT:

- A transformer or large language model
- A chatbot or conversation system
- A pretrained AI system
- Claims human intelligence or consciousness
- Claims accurate reproduction of biological brains

NLM DOES investigate:

- Neural memory system integration with dynamics
- Coordinated neuromodulation effects on plasticity
- Prediction as central organizing principle
- Developmental modulation of learning
- Memory replay for consolidation
- Coherence of complete brain loop

## Technical Details

### Neural Model

- **Neuron Type**: Leaky Integrate-and-Fire (LIF)
- **Dynamics**: Event-driven, spike-based propagation
- **Synaptic Model**: Excitatory/inhibitory with delays
- **Plasticity**: STDP, Hebbian, structural (synaptogenesis/pruning)
- **Scale**: Event-driven for efficiency

### Neuromodulation Systems

| Neuromodulator | Status | Functions |
|----------------|--------|-----------|
| Dopamine | ✅ Working | Plasticity modulation, learning rate |
| Curiosity | ✅ Working | Exploration driver, novelty response |
| Novelty | ✅ Working | Pattern change detection |
| Serotonin | ⚠️ Stub | Implemented but not integrated |
| Norepinephrine | ⚠️ Stub | Implemented but not integrated |
| Acetylcholine | ⚠️ Stub | Implemented but not integrated |

### Performance

Based on benchmarks in `docs/PERFORMANCE.md`:

- **Efficiency**: Event-driven processing for sparse activity
- **Memory**: ~50 MB with 1K neurons
- **Scalability**: SIMD vectorization, parallel processing
- **Simulation Speed**: ~1M synaptic updates per second (1K neurons)

## License

MIT License. See `LICENSE` file for details.

## Authors and Contributors

Research project by the NLM team. See `docs/RESEARCH_METHODS.md` for detailed research background.

**Key Contributors:**
- Core developers: Brain architecture, neural dynamics
- Cognitive scientists: Memory, attention, development systems
- Computer scientists: Integration, optimization, testing

## Support and Community

For questions, issues, or collaboration:

- GitHub Issues: Project repository issues
- Documentation: Extensive docs in `/docs/` directory
- Examples: Python demos and C++ executables

---

*Last updated: 2026-09-16*
*Phase 6 integration active*
