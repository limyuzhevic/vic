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

## Project Structure

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

## Python Bindings

### NLM Python Bindings Enhancements

The NLM project includes comprehensive Python bindings that provide production-ready access to the C++ neural simulation framework. These bindings have been enhanced with robust error handling and validation to ensure reliability and ease of use.

#### Key Features

**1. Comprehensive Exception System**
- Custom exception hierarchy: `NLMException`, `ConfigurationError`, `MemoryError`, `SimulationError`, `ValidationError`, `ResourceError`
- Detailed error messages with source location tracking
- Conversion from C++ to Python exceptions
- Error categorization and recovery suggestions

**2. Input Parameter Validation**
- Range validation for numeric parameters
- Type validation and conversion safety
- Null pointer checking
- Structure validation for complex objects
- Internal consistency validation

**3. Enhanced Error Handling**
- All core classes include comprehensive error handling
- Memory management safety with smart pointers
- Thread-safe access patterns
- Resource cleanup in error conditions

**4. Enhanced Core Classes**
- **Brain**: Neural simulation brain with error handling
- **AgentBrain**: Interface connecting NLM brain to world
- **DevelopmentSystem**: Neural development orchestration
- **NeuralPlanner**: Action planning with error handling
- **ConceptFormation**: Pattern discovery with error handling
- **SelfModel**: Self-representation with error handling
- **SocialLearning**: Learning from observation with error handling

**5. Enhanced Memory Systems**
- **WorkingMemory**: Temporary active storage with error handling
- **NeuralWorkingMemory**: Enhanced neural integration
- **NeuralEpisodicMemory**: Experience storage with error handling

**6. Enhanced Neuromodulation Systems**
- **Neuromodulator**: Abstract base with error handling
- **Dopamine**: Reward and reinforcement learning
- All other neuromodulators integrated

**7. Enhanced Prediction Systems**
- **PredictionSystem**: Future state prediction with error handling
- Confidence and error tracking
- Comprehensive prediction management

**8. Factory Functions**
- `createBrain()` - With comprehensive error handling and validation
- `createAgentBrain()` - Memory safety validation
- `createDevelopmentSystem()` - Resource management validation
- All cognitive and memory system factory functions with error handling

#### Usage Examples

```python
import pynlm

# Create a configured brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use cognitive systems
planner = brain.getPlanner()
action = planner.planAction(current_state, target_reward=0.5)

# Access memory systems
working_memory = brain.getWorkingMemory()
value = working_memory.retrieve(neuron_id)

# Use neuromodulation
dopamine = brain.getDopamine()
dopamine.signalReward(reward_value)

# Run simulation
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
```

#### Error Handling

The bindings include comprehensive error handling:

```python
try:
    brain = pynlm.createBrain(config)
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.MemoryError as e:
    print(f"Memory error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
```

#### Integration with Existing Systems

The Python bindings integrate seamlessly with:
- Brain's cognitive systems (NeuralPlanner, ConceptFormation, etc.)
- Development systems (DevelopmentSystem, Synaptogenesis, etc.)
- Neuromodulation systems (Dopamine, Curiosity, Novelty, etc.)
- Prediction systems
- All existing NLM functionality

#### Benefits

1. **Production Ready**: Comprehensive error handling prevents crashes
2. **Easy Debugging**: Detailed error messages with source locations
3. **Type Safe**: Enhanced type checking and conversion safety
4. **Memory Safe**: Proper resource management with smart pointers
5. **Well Documented**: Comprehensive docstrings for all classes and methods
6. **Backward Compatible**: No breaking changes to existing API

#### Files Modified

- `python/bindings.cpp`: All enhancements implemented in this single file
- `python/NLM_BINDINGS_ENHANCEMENTS.md`: Comprehensive documentation
- `demo_memory_bindings.py`: Usage examples for memory systems
- `demo_neuromodulation_bindings.py`: Usage examples for neuromodulation systems
- `test_memory_bindings.py`: Test framework for memory system bindings

The Python bindings are now production-ready with comprehensive error handling, validation, and debugging support, making them ideal for both research and production use.


Research project — See docs for scientific background.
