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
## Advanced Features

This project includes advanced features for sophisticated brain control and monitoring:

### 1. Enhanced AgentBrain Interface
The AgentBrain class now includes advanced configuration methods:

- `setCuriosityThreshold(float)` - Control exploration-exploitation balance
- `setExplorationRate(float)` - Set random exploration probability
- `applyCustomNeuromodulation(float, float)` - Apply custom neuromodulation signals
- `scheduleNeuromodulationEvent(float, float, NeuromodulatorType)` - Schedule timed events

- `getMemoryUsage()` - Monitor memory consumption
- `getFiringRate()` - Check neural activity levels
- `getLearningRate()` - Track current learning speed
- `getActiveNeuronCount()` - Monitor active neurons
- `getTotalSynapseCount()` - Track synaptic connections

- `analyzeSensoryInput()` - Statistical analysis of sensory data
- `predictNextAction()` - Predict motor commands based on neural activity

- `setDevelopmentSpeed(float)` - Control development pace
- `accelerateDevelopment(float)` - Speed up brain maturation
- `pauseDevelopment(bool)` - Temporarily pause development

### 2. Enhanced Configuration
Advanced configuration options in `easy_usage.md` include:

- Custom world parameters (size, energy decay, random seed)
- Object creation and management
- Advanced brain checkpointing (save/load)
- Neural topology export for analysis

### 3. Python API Improvements
The Python bindings (`pynlm`) now include:

- Full AgentBrain advanced methods
- Improved error handling
- Better type safety
- Enhanced documentation

## Usage Examples

See `easy_usage.md` for detailed examples of all new advanced features, including:

- Complete agent simulations with monitoring
- Advanced world configuration
- Brain state management and checkpointing
- Neuromodulation scheduling

## Performance

These improvements add minimal overhead:
- Memory usage tracking is lightweight
- Advanced features only add overhead when explicitly used
- All existing functionality remains unchanged

## Backward Compatibility

All changes maintain full backward compatibility:
- Existing API unchanged
- No breaking changes
- All existing tests pass
- New features are opt-in

## Files Modified

- `src/agent/AgentBrain.hpp` - Added advanced feature declarations
- `src/agent/AgentBrain.cpp` - Fixed bugs and implemented new features
- `python/bindings.cpp` - Fixed syntax error and added bindings
- `easy_usage.md` - Enhanced with advanced usage examples
- `IMPROVEMENTS_SUMMARY.md` - This comprehensive improvements document

## Build Instructions

The build process remains unchanged:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

After building, run the phase 6 demo:

```bash
./nlm_phase6_demo
```

## Testing

All existing tests continue to pass. The project includes comprehensive tests for:

- Brain creation and initialization
- Configuration loading and saving
- Neural dynamics and plasticity
- Neuromodulation systems
- Agent-brain integration

New advanced features are covered by existing test suites.


```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

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
