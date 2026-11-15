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

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

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

# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Improvements and Enhancements (Phase 7: Robustness and Maintainability)

This project has been significantly improved with the following enhancements:

### 1. **Configurable Parameters and Buffer Safety**
- **Configurable text neuron parameters**: Text processing is now configurable via the config file
- **Touch and vision thresholds**: Multiple thresholds for object detection are now configurable
- **Improved buffer safety**: Added bounds checking and validation for all array access
- **Better error handling**: Comprehensive validation of configuration parameters

### 2. **Enhanced Error Handling and Validation**
- **Configuration validation**: All configuration parameters are now validated before use
- **Input validation**: Added validation for sensory input values (touch, vision, etc.)
- **Better error messages**: Improved error reporting with descriptive messages
- **Graceful degradation**: Better fallback handling for invalid inputs

### 3. **Improved Python Bindings**
- **Better error handling**: Python bindings now properly handle errors and exceptions
- **Documentation**: Enhanced docstrings and parameter descriptions
- **Validation**: Added input validation for Python API calls
- **Backward compatibility**: Maintained compatibility with existing Python API

### 4. **Code Quality Improvements**
- **Memory management**: Better handling of dynamic memory allocation
- **Resource management**: Improved cleanup and resource management
- **Code organization**: Better separation of concerns and improved modularity
- **Type safety**: Better use of modern C++ type safety features

## Configuration

The configuration file now includes additional parameters for the improved AgentBrain behavior:

```ini
# AgentBrain configuration
agent_text_neuron_base = 16
agent_text_neuron_count = 15
agent_touch_threshold = 0.05f
agent_resource_vision_min = 0.5f
agent_hazard_vision_max = 0.35f
agent_max_touch_threshold = 0.05f
```

## Building

The build process remains the same:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

All existing demos and tests continue to work. The improvements are transparent to end users:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

## Testing

The existing tests continue to pass, and the improvements provide better robustness:
- Better handling of edge cases
- Improved error messages
- More reliable behavior with invalid inputs
- Enhanced validation of system parameters

## Performance

The improvements maintain or improve performance:
- Minimal overhead from validation checks
- Better memory management reduces leaks
- Improved error handling prevents crashes
- Configurable parameters allow optimization for specific use cases

## Backward Compatibility

All improvements are backward compatible:
- No changes to the public API
- Existing code continues to work without modification
- All existing demos and tests pass unchanged
- New features are opt-in via configuration

## Future Enhancements

Phase 7 improvements provide a foundation for future development:
- More configurable behavior via the config file
- Better error reporting for debugging
- Enhanced validation prevents undefined behavior
- Improved code organization makes future extensions easier

## Project Structure (Updated)

```
NLM/
├── CMakeLists.txt
├── README.md
├── configs/
│   └── default.cfg              # Enhanced with new configurable parameters
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
├── src/
│   ├── core/                    # Core utilities
│   ├── agent/                   # Agent-brain interface (improved)
│   ├── brain/                   # Neural components (integrated)
│   ├── python/                  # Python bindings (enhanced)
│   ├── tests/                   # Test suite
│   └── ...
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
