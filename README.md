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
  ↓

## Phase 6: Enhanced Integration

Phase 6 focuses on enhancing the integration with improved architecture, configurability, and code quality.

### Improvements Implemented

#### 1. **AgentBrain Architecture Refactoring**
- **Before**: 350+ line monolithic `AgentBrain.cpp` class
- **After**: Clean orchestrator with 5 specialized component classes:
  - `MotorDecoder` - Handles motor neuron activity and curiosity-based exploration
  - `SensoryProcessor` - Processes sensory input and computes novelty
  - `NeuromodulationManager` - Manages reward, dopamine, prediction error
  - `DevelopmentManager` - Handles developmental stages and plasticity
  - `CuriosityManager` - Manages exploration motivation

#### 2. **Comprehensive Configuration System**
- **AgentConfig**: Centralized configuration for all agent parameters
  - Motor scaling factors (configurable vision, touch, internal, proprioception)
  - Exploration behavior (curiosity thresholds, exploration probability)
  - Development stages (customizable timing and plasticity rates)
  - Neuromodulation parameters (reward decay, dopamine clamping)
  - Structural plasticity rates

- **SensoryConfig**: Enhanced sensory modality configuration
  - Customizable sensor counts (vision: configurable width/height)
  - Signal ranges and processing parameters
  - Temporal parameters and timestamps

#### 3. **Enhanced SensoryPercept**
- **Before**: 2-line implementation (constructor, getAllSignals)
- **After**: Full-featured sensory system with:
  - Configurable sensory modalities and validation
  - Signal dimension checking and error handling
  - Timestamp support and time tracking
  - Comprehensive logging and debugging
  - Extensible architecture for new senses

#### 4. **Code Quality Improvements**
- **Reduced Complexity**: Broke down monolithic classes into focused components
- **Eliminated Magic Numbers**: Replaced hardcoded values (5.0f, 0.5f, etc.) with configurable parameters
- **Enhanced Error Handling**: Added validation, null checks, and exception safety
- **Improved Documentation**: Added comprehensive comments and docstrings
- **Better Separation of Concerns**: Each component has a single responsibility

#### 5. **New Demo Program**
- **EnhancedAgentDemo.cpp**: Comprehensive demonstration of all improvements
- Shows configurable parameter usage
- Demonstrates component integration
- Validates system functionality
- Provides educational examples

#### 6. **Build System Enhancements**
- Added `nlm_enhanced_agent_demo` executable for demonstration
- Updated `AgentConfig.cpp` build integration
- Added `SensoryPercept.cpp` build integration
- Enhanced target dependencies for better modularity

### Configuration Examples

#### Basic Usage (Backward Compatible)
```cpp
// AgentBrain with default configuration
AgentBrain agentBrain(brain, AgentConfig());
```

#### Custom Agent Configuration
```cpp
AgentConfig customConfig;
customConfig.visionSensoryScale = 8.0f;
customConfig.touchSensoryScale = 12.0f;
customConfig.curiosityThreshold = 0.2f;
customConfig.explorationProbability = 0.5f;
customConfig.developmentalStage1End = 30.0;
AgentBrain agentBrain(brain, customConfig);
```

#### Enhanced Sensory Configuration
```cpp
SensoryConfig sensoryConfig;
sensoryConfig.visionWidth = 32;
sensoryConfig.visionHeight = 32;
sensoryConfig.touchSensors = 12;
sensoryConfig.audioSampleRate = 44100;
sensoryConfig.audioEnabled = true;
SensoryPercept sensory(sensoryConfig);
```

### Build Instructions

#### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Build and Run Enhanced Demo
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_enhanced_agent_demo
```

### Performance and Memory Benefits

1. **Better Cache Locality**: Smaller, focused components improve memory access patterns
2. **Reduced Compilation Time**: Modular design allows incremental compilation
3. **Enhanced Configurability**: No recompilation needed for parameter changes
4. **Improved Debugging**: Isolated components make debugging easier
5. **Future Extensibility**: Easy to add new components without modifying existing code

### Integration with Existing Systems

- **Backward Compatible**: All existing APIs preserved
- **Phase 6 Integration**: Works seamlessly with existing Phase 6 experiment framework
- **Memory Systems**: Fully compatible with working memory, episodic memory, associative memory
- **Neuromodulation**: Integrated with dopamine, curiosity, novelty, prediction error systems
- **Development**: Connected to developmental stages and structural plasticity

### Testing and Validation

- **EnhancedDemo**: Comprehensive demonstration of all improvements
- **Phase 6 Demo**: Existing integration tests continue to work
- **Configuration Validation**: Built-in validation for config parameters
- **Signal Validation**: Dimensional consistency checking for sensory inputs

### Summary

The Phase 6 enhancements transform the NLM from a monolithic implementation into a modular, configurable, and maintainable cognitive architecture. The code quality improvements, enhanced configurability, and better separation of concerns make the system more accessible to researchers and developers while maintaining full compatibility with existing functionality.

## License

MIT

## Authors

Research project — See docs for scientific background.
