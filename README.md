# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Current Status: Phase 6 Complete

**Phase 6: FINAL INTEGRATION** - All systems integrated into a coherent artificial brain.

NLM IS NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Key Improvements

### 1. Enhanced Command-Line Interface
- **nlm_phase6_demo**: Main production executable for Phase 6 integration testing
- **nlm**: Phase 2 compatibility executable (deprecated)
- **nlm_phase3_demo, nlm_phase4_demo, nlm_phase5_demo**: Backward compatibility
- **nlm_phase6_demo**: New Phase 6 demo executable (recommended)

### 2. Improved Configuration
- **configs/default.cfg**: Fixed duplicate settings, Phase 6 optimized
- Better configuration validation and error handling
- Support for advanced configuration options

### 3. Enhanced Python Bindings
- **pynlm** Python module with comprehensive API
- Factory functions for common setups (standard, exploration, learning agents)
- Convenience methods for brain and agent management
- Complete error handling and documentation

### 4. Better Integration
- All memory systems (working, episodic, semantic, procedural, associative) connected
- Neuromodulation systems (dopamine, curiosity, novelty, prediction error) integrated
- Prediction system integrated with learning
- Development system affecting plasticity rates
- Checkpoint and replay systems functional

### 5. Developer-Friendly Features
- Simple command-line argument parsing (no third-party dependencies)
- Visualization support options (--visualization, --viz-file, --viz-rate)
- Comprehensive logging and debugging
- Extensive documentation with examples

## Building

### From Source (C++)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Phase 6 Demo
```bash
# Main integration test (recommended)
./nlm_phase6_demo

# With custom options
./nlm_phase6_demo --neurons 1000 --steps 5000 --visualization --log-level DEBUG

# For backward compatibility
./nlm  # Phase 2 tests (deprecated)
```

### Python Bindings
```bash
pip install .
```

## Usage Examples

### Phase 6 Demo (C++)
```bash
# Quick integration test
./nlm_phase6_demo

# Full integration with custom settings
./nlm_phase6_demo --neurons 2000 --steps 10000 --no-checkpoint --visualization
```

### Python Bindings
```python
import pynlm

# Create standard agent setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Mini Projects

#### Project 1: Silent Brain
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Silent brain test done! Neurons firing: {brain.getFiringNeuronCount()}")
```

#### Project 2: Complete Agent
```python
import pynlm

# Setup with exploration focus
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization support
├── python/              # Python bindings
├── configs/             # Configuration files
├── docs/                # Technical documentation
├── CMakeLists.txt       # Build configuration
└── pyproject.toml      # Python package configuration
```

## Available Executables

| Target | Type | Description |
|--------|------|-------------|
| `nlm_phase6_demo` | Executable | Main Phase 6 integration demo (recommended) |
| `nlm` | Executable | Phase 2 compatibility (deprecated) |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase5_demo` | Executable | Phase 5 demonstration |

## Scientific Background

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains.

**Current Limitations:**
- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture

**Research Focus:**
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## License

MIT

## Authors

Research project — See docs for scientific background.

## Getting Help

For detailed information, see:
- `HOW_TO_USE.md` - Comprehensive usage guide
- `easy_usage.md` - Simple Python examples
- `docs/ARCHITECTURE.md` - Technical architecture
- `docs/SCIENCE.md` - Scientific background

The Phase 6 integration demo provides the most comprehensive test of all integrated systems.
