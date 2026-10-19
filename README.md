# NLM — 熙然 (Neural Learning Machine)

**An Experimental Artificial Developmental Brain**

## Current Status: Phase 6 - Final Integration

### Implemented Systems (Operational)

#### Core Neural Computation
- ✅ Leaky Integrate-and-Fire (LIF) neuron dynamics
- ✅ Event-driven spike propagation with synaptic delays
- ✅ STDP and Hebbian plasticity rules
- ✅ Structural plasticity (synaptogenesis/pruning)

#### Memory Systems
- ✅ **Working Memory** - Transient active information storage
- ✅ **Episodic Memory** - Experience encoding with replay capability
- ✅ **Associative Memory** - Pattern associations
- ✅ All memory systems properly integrated with neural processing

#### Neuromodulation
- ✅ **Dopamine** - Reward prediction error signaling
- ✅ **Novelty** - Novelty detection and arousal
- ✅ **Curiosity** - Exploration motivation
- ✅ **Acetylcholine** - Attention and memory consolidation (Phase 2 implementation)
- ✅ **Norepinephrine** - Arousal and vigilance (Phase 2 implementation)
- ✅ **Serotonin** - Mood, impulsivity, and social behavior (Phase 2 implementation)

#### Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error computed and used for learning
- ⚠️ Confidence tracking partially implemented

#### Cognition Systems
- ✅ **Neural Planner** - Action planning with goal evaluation
- ✅ **Concept Formation** - Pattern discovery from experience
- ✅ **Attention** - Competitive selection dynamics
- ✅ All cognition systems connected to perception and action

#### Development
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity changes with age
- ✅ Neural excitability modulated by development

#### Persistence
- ✅ Checkpoint save/load implemented via CheckpointSystem
- ✅ Brain state serialization working
- ✅ Can resume from saved checkpoints

#### Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ Sleep/rest cycle with enhanced consolidation

## Key Improvements Summary

### Integration Achievements

1. **Memory Integration**
   - Working memory captures active neural patterns
   - Episodic memory stores complete experiences
   - Both systems connected to neural processing for real-time access

2. **Neuromodulation Integration**
   - Complete neuromodulation suite (DA, ACH, NE, 5-HT, Novelty, Curiosity)
   - Neuromodulators affect attention, memory consolidation, arousal
   - Reward prediction error signaling integrated into learning

3. **Cognition Integration**
   - Neural planner evaluates action sequences based on predictions
   - Concept formation processes experiences to discover patterns
   - Attention selects focus based on novelty and goals

4. **Performance Optimizations**
   - Event-driven spike processing with ring buffers
   - SIMD vectorization support for batch neural updates
   - Memory pools for efficient allocation

### Usage Examples

#### Basic Usage
```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run basic processing
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Complete Agent Simulation
```python
import pynlm

# Setup agent in environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableNoveltyDetection(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete. Firing neurons: {brain.getFiringNeuronCount()}")
```

#### Checkpoint and Persistence
```python
import pynlm
import os

# Save brain state
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

# Save checkpoint
brain.save("my_checkpoint.nlc")
print("Brain state saved to checkpoint")

# Load saved state
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load("my_checkpoint.nlc")
print("Brain state loaded from checkpoint")
```

## Advanced Features

### Neural Planner
- Multi-step action planning based on predictive models
- Goal-directed behavior with reward evaluation
- Plan quality updates based on experience

### Concept Formation
- Pattern discovery from sensory experience
- Prototype formation through averaging
- Category emergence from shared properties

### Neuromodulation
- **Dopamine**: Reward prediction error signaling
- **Acetylcholine**: Attention modulation and memory consolidation
- **Norepinephrine**: Arousal and stress response
- **Serotonin**: Mood regulation and social behavior
- **Curiosity**: Exploration motivation
- **Novelty**: Change detection and alerting

### Sleep and Consolidation
- Automatic sleep/rest cycles based on activity
- Enhanced memory consolidation during sleep
- Synaptic downscaling and pruning

## API Functions

### Brain Operations
- `brain.initialize()` - Initialize neural system
- `brain.step(step)` - Process one simulation step
- `brain.receiveSensoryInput(input)` - Process sensory data
- `brain.produceAction()` - Generate motor command
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state

### Agent Interface
- `agent.processSensoryInput(percept)` - Process environmental input
- `agent.decodeMotorCommand()` - Convert neural activity to action
- `agent.applyRewardModulation(reward, predicted)` - Update based on outcomes
- `agent.updateDevelopment(time)` - Update developmental stage

### Configuration
- `createDefaultConfig()` - Create default configuration
- `config.set(key, value)` - Set configuration parameters
- `config.getOr(key, default)` - Get configuration value with fallback

## System Architecture

```
WORLD → SENSORY INPUT → SENSORY NEURONS → WORKING MEMORY
    ↓                      ↓                    ↓
  EPISODIC MEMORY ← ACTION ← INTERNAL STATE ← MOTOR NEURONS
    ↑                      ↑                    ↑
  PREDICTION ← COGNITION ← NEUROMODULATION ← PLASTICITY
    ↑                      ↑                    ↑
  DEVELOPMENT ← CONSOLIDATION ← REPLAY ← SLEEP/REST
```

## Performance Features

### Event-Driven Processing
- Ring buffer for immediate spike events
- Time-bucketed delayed spike delivery
- Lock-free queue operations for maximum performance

### SIMD Vectorization
- Automatic detection of available SIMD instructions (AVX-512, AVX2, SSE)
- Batch processing of neuron state updates
- Aligned memory operations for optimal cache utilization

### Memory Management
- Pre-allocated object pools for efficient allocation
- Memory-efficient slab allocation
- Thread-safe operations for concurrent processing

## Getting Started

### Installation
```bash
# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Usage
```python
import pynlm

# Quick test
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(10):
    brain.step(i)
print(f"Test completed. Spikes: {brain.getTotalSpikeCount()}")
```

### C++ Usage
```cpp
#include <nlm/Brain.h>

int main() {
    nlm::Config config;
    nlm::Brain brain(config);
    
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    for (size_t i = 0; i < 1000; ++i) {
        brain.step(i);
    }
    
    brain.logStatus();
    return 0;
}
```

## Files

### Core Libraries
- `src/brain/` - Neural computation engine
- `src/cognition/` - Cognitive processing systems
- `src/memory/` - Memory management systems
- `src/neuromodulation/` - Neuromodulation systems
- `src/performance/` - Performance optimization libraries

### Python Bindings
- `python/bindings.cpp` - Python API implementation
- `setup.py` - Python package setup

### Documentation
- `README.md` - Project overview and usage
- `docs/` - Detailed technical documentation
- `easy_usage.md` - Beginner-friendly guide
- `HOW_TO_USE.md` - Comprehensive usage instructions

## Limitations

### Current Constraints
- Simplified LIF neuron model (not biologically detailed)
- Limited ion channel dynamics
- No cortical architecture or connectivity patterns
- Simulation scale limited by available computational resources

### Scientific Scope
- **No claims of:** Consciousness, human-like intelligence, subjective experience
- **Investigates:** Neural integration, emergent cognition, developmental learning
- **Focus:** Computational principles of brain-like systems

## Future Directions

### Phase 7: Enhanced Integration
- Full integration of all neuromodulators
- Advanced learning mechanisms
- Social interaction capabilities
- Complex environmental adaptation

### Phase 8: Biological Fidelity
- More realistic neuron models
- Detailed synaptic mechanisms
- Structured cortical architectures
- Biologically plausible learning rules

### Phase 9: Applications
- Benchmark testing against biological systems
- Practical applications in robotics
- Educational and research tools
- Real-world problem solving

## Support and Community

### For Questions and Issues
- GitHub repository with issue tracking
- Documentation and API references
- Community forums for discussion
- Example projects and tutorials

### Research Collaboration
- Open-source research platform
- Reproducible experiments
- Benchmarking framework
- Collaborative development tools

---

## Conclusion

NLM represents a significant step toward creating an artificial developmental brain with:

1. **Complete integration** of all major brain systems
2. **Realistic learning** through experience and interaction
3. **Adaptive behavior** through developmental processes
4. **Efficient performance** through modern optimization techniques

The system demonstrates the feasibility of integrating multiple neural subsystems into a coherent, functioning artificial brain architecture capable of learning, memory, and goal-directed behavior.

**Ready for research, experimentation, and application development!**
