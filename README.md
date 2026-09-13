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
│   ├── cognition/     # Cognitive mechanisms (integrated)
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

## Bug Fixes and Improvements (Phase 6)

### Critical Bug Fixes

#### 1.1 Fixed AgentBrain Off-by-One Error
**File**: `src/agent/AgentBrain.cpp` (line 221)
**Issue**: Random index selection included invalid value 7
**Fix**: Change `uniformInt(0, 7)` to `uniformInt(0, 6)`

#### 1.2 Added Missing Motor Commands
**File**: `src/agent/AgentBrain.hpp`
**Issue**: MotorCommand enum missing LookLeft and LookRight commands
**Fix**: Added these commands to enum and updated switch cases
- `LookLeft = 4` - New: Pan sensor to the left
- `LookRight = 5` - New: Pan sensor to the right

#### 1.3 Fixed Constructor Initialization
**File**: `src/agent/AgentBrain.cpp` (lines 8-22)
**Issue**: previousVision_ and neural group vectors not initialized
**Fix**: Added proper member initialization and null pointer validation

#### 1.4 Added Destructor
**File**: `src/agent/AgentBrain.cpp` (lines 1-3)
**Issue**: No cleanup in destructor
**Fix**: Implement destructor to clear all neural group vectors

#### 1.5 Fixed Sensory Processing
**File**: `src/agent/AgentBrain.cpp` (lines 44-76)
**Issue**: Variable naming inconsistencies
**Fix**: Use correct neural group names and add input validation

### Memory and Safety Improvements

#### 2.1 Removed Unused Include
**File**: `src/agent/AgentBrain.hpp` (line 3)
**Issue**: AgentBody.hpp not used
**Fix**: Remove unused include

#### 2.2 Fixed getSensoryInputSize()
**File**: `src/agent/AgentBrain.cpp` (lines 15-23)
**Issue**: Hardcoded values don't match actual neural groups
**Fix**: Dynamic calculation based on actual neural group sizes

#### 2.3 Fixed getMotorOutputSize()
**File**: `src/agent/AgentBrain.cpp` (lines 25-37)
**Issue**: Hardcoded value
**Fix**: Dynamic calculation from actual motor group sizes

### Performance Optimizations

#### 3.1 Optimized getAllSignals()
**File**: `src/agent/SensoryPercept.cpp` (lines 17-36)
**Issue**: Repeated vector reallocation
**Fix**: Preallocate and use insert with iterators for better performance

#### 3.2 Fixed Vision Dimension Bug
**File**: `src/agent/SensoryPercept.hpp` (lines 6-7, 20)
**Issue**: visionWidth_ and visionHeight_ never updated when vision_ changes
**Fix**: Make them derived from vector size

### Enhanced Functionality

#### 4.1 Added Dimension Getters
**File**: `src/agent/SensoryPercept.hpp`
**Issue**: No dimension getters for all signals
**Fix**: Add getTouchSize(), getInternalSize(), getProprioceptionSize(), getAudioSize()

#### 4.2 Added Error Handling
**File**: Multiple files
**Issue**: Limited error handling
**Fix**: Add input validation and proper logging

## New Features

### Additional Motor Commands
The MotorCommand enum now includes:
- `LookLeft`: Pan sensor to the left (useful for exploration)
- `LookRight`: Pan sensor to the right (useful for detailed observation)

These new commands expand the agent's behavioral repertoire for environmental interaction.

### Enhanced Sensory Processing
- AgentBrain now validates input dimensions
- Better error messages for invalid inputs
- Null pointer validation throughout

### Improved Memory Management
- All neural group vectors properly initialized
- Destructor for proper cleanup
- Dynamic calculation of input/output sizes

### Performance Improvements
- Optimized vector operations in SensoryPercept
- Pre-allocation to reduce reallocations
- Fixed vision dimension tracking

## Python Bindings Features

The Python bindings provide access to all NLM functionality:

### New Motor Commands in Python
```python
# These motor commands are now available:
# MotorCommand.MoveForward = 0
# MotorCommand.MoveBackward = 1
# MotorCommand.TurnLeft = 2
# MotorCommand.TurnRight = 3
# MotorCommand.LookLeft = 4      # NEW: Pan sensors left
# MotorCommand.LookRight = 5     # NEW: Pan sensors right
# MotorCommand.Interact = 6
# MotorCommand.Wait = 7
```

### Enhanced AgentBrain API
The AgentBrain class now includes:
- `getCuriosityLevel()`: Returns curiosity level (0.0-1.0)
- `getNoveltyLevel()`: Returns novelty level (0.0-1.0)
- `getPredictionError()`: Returns prediction error signal
- `getNeuromodulationLevel()`: Returns overall neuromodulation level
- All subsystems can be individually enabled/disabled

### Configuration Examples
```python
# Example: Using new motor commands
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print(f"Available motor commands: LookLeft, LookRight")
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
```

## Performance Improvements

### Memory Management
- Fixed memory leaks with proper destructor implementation
- Dynamic calculation of neural group sizes
- Null pointer validation prevents crashes

### Computational Efficiency
- Pre-allocated vectors in SensoryPercept
- Optimized bulk insert operations
- Reduced unnecessary memory copies

## Testing and Validation

### Test Suite
All changes are validated by:
1. **Compilation**: CMake builds all targets successfully
2. **Unit Tests**: Run `./nlm_test` for comprehensive testing
3. **Python Bindings**: Verify `pip install -e .` works correctly
4. **Integration Tests**: Run `./nlm` or `./nlm_phase6_demo`

### Test Coverage
New tests added for:
- Null pointer scenarios
- Input validation
- Motor command functionality (LookLeft, LookRight)
- Performance measurements
- Memory management

## Architecture and Design

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

## License

MIT

## Authors

Research project — See docs for scientific background.
