# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**Important Notes:**
- This is a research project in Phase 6: FINAL INTEGRATION
- Results are NOT claimed to reproduce biological brains accurately
- This is an experimental artificial neural system, NOT a transformer or LLM

## Quick Start

### Installation (from source)

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Build C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings (optional)
cd ..
python3 -m pip install scikit-build-core pybind11

# Run Phase 6 demo
./nlm_phase6_demo

# For Python bindings (if built)
python3 -c "import pynlm; print(pynlm.__doc__)"
```

### Running Demos

**Phase 6 Integration Demo**:
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

**Phase 3 Demo**:
```bash
./nlm_phase3_demo
```

**Phase 4 Demo**:
```bash
./nlm_phase4_demo
```

## Python Bindings Usage

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world for interaction
world = pynlm.createSimpleWorld()

# Create agent brain interface
agent_brain = pynlm.createAgentBrain(brain)

# Main simulation loop
for step in range(1000):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input
    agent_brain.processSensoryInput(percept)
    
    # Produce action
    action = brain.produceAction()
    
    # Apply to world
    result = world.applyMotorCommand(action, step * 0.001)
    
    # Apply reward modulation if successful
    if result.success:
        agent_brain.applyRewardModulation(result.reward, 1.0)  # Assuming perfect prediction
```

## Project Structure

```
NLM/
├── src/                    # C++ source code
│   ├── core/              # Core utilities
│   ├── brain/             # Neural computation
│   ├── plasticity/        # Learning mechanisms
│   ├── neuromodulation/    # Emotional/motivational systems
│   ├── memory/            # Working and episodic memory
│   ├── cognition/         # Attention, planning, concept formation
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor output
│   ├── environment/       # World simulation
│   ├── experiments/       # Experiment frameworks
│   └── visualization/      # Visualization
├── python/                 # Python bindings
│   ├── bindings.cpp        # Pybind11 implementation
│   └── CMakeLists.txt      # Python bindings build
├── tests/                  # Unit tests
├── docs/                   # Documentation
├── configs/               # Configuration files
└── build/                 # Build artifacts
```

## Scientific Background

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition

**Phase 6 does NOT claim:**
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

**Phase 6 DOES investigate:**
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

## Development Guidelines

### Coding Standards
- **Language**: C++20 with modern C++ practices
- **Style**: Consistent with existing codebase (pImpl pattern, type-safe IDs)
- **Documentation**: Comprehensive docstrings for public APIs
- **Testing**: Unit tests for all major components

### Contribution Process
1. Fork the repository
2. Create feature branch with descriptive name
3. Implement changes following existing patterns
4. Add unit tests for new functionality
5. Run existing test suite
6. Submit pull request with proper documentation

### Performance Considerations
- Use memory pools for frequently allocated objects
- Implement event-driven simulation for efficiency
- Leverage SIMD vectorization where possible
- Use sparse connectivity for large networks
- Implement checkpoint/restart for long simulations

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background and phase documentation.

## References

For detailed scientific background, see:
- `docs/SCIENCE.md` - Scientific foundations
- `docs/ARCHITECTURE.md` - Technical architecture
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration results
- `docs/EXPERIMENTS.md` - Experimental methodology
- `docs/ROADMAP.md` - Future development roadmap
- `docs/LIMITATIONS.md` - Current scientific limitations
