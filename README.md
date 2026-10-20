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

## Enhanced Python Bindings

NLM now includes enhanced Python bindings (`pynlm`) that provide comprehensive neuroscience research and experimentation capabilities:

### Enhanced Features

The enhanced Python bindings extend the original basic interface with advanced functionality:

#### 🎯 Advanced Plasticity Control
- Configure and control STDP, Hebbian, and structural plasticity rules
- Dynamic plasticity rule switching during simulation
- Spatial, temporal, and input-specific plasticity patterns
- Fine-grained control over plasticity parameters

#### 🧠 Neuromodulation System
- Reward prediction error computation
- Curiosity-driven exploration control
- Novelty detection and processing
- Dopamine system simulation
- Motivation and drive systems

#### 💾 State Management
- Brain state serialization and deserialization
- Checkpoint/restart functionality
- Version control for brain states
- State validation and comparison

#### 📈 Development System
- Precise developmental stage control
- Critical period management
- Plasticity rate modulation with age
- Maturation tracking and aging simulation

#### ⚙️ Advanced Configuration
- Hierarchical configuration systems
- Configuration presets for different scenarios
- Runtime configuration modification
- Configuration validation and error handling

#### 🧪 Experiment Management
- Experiment parameter management
- Multi-experiment scheduling
- Data collection and analysis
- Result validation and comparison

#### 📊 Statistical Analysis
- Time-series analysis
- Statistical hypothesis testing
- Pattern recognition
- Correlation analysis

#### 📈 Visualization and Output
- Scientific plotting (neural activity, learning curves)
- Data export (CSV, JSON, HDF5 formats)
- Interactive visualization capabilities
- Multi-dimensional data analysis
- Report generation

#### ⚡ Performance Monitoring
- Execution timing and benchmarking
- Memory usage tracking
- Performance profiling
- Scalability analysis
- Resource monitoring

#### 🔧 Error Handling and Validation
- Comprehensive error handling
- Input validation
- Recovery actions
- Error statistics

### Backward Compatibility

All existing Python APIs remain unchanged. The enhanced bindings extend the original `pynlm` module:

```python
# Original usage (still works)
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)

# Enhanced usage (new features)
import pynlm

# Create enhanced controllers
plasticity = pynlm.create_enhanced_plasticity_controller()
neuromod = pynlm.create_enhanced_neuromodulation_controller()
state = pynlm.create_enhanced_state_manager()
development = pynlm.create_enhanced_development_controller()
config = pynlm.create_enhanced_configuration_manager()

# Use enhanced features
plasticity.enable_stdp(True)
neuromod.enable_curiosity(True)
state.save_brain_state("checkpoint.bin")
```

### Getting Started with Enhanced Bindings

#### Installation

```bash
# Install enhanced bindings
pip install pynlm

# For development (latest changes)
pip install -e .
```

#### Basic Usage

```python
import pynlm

# 1. Create enhanced configuration
config = pynlm.create_advanced_config()

# 2. Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Create enhanced controllers
plasticity = pynlm.create_enhanced_plasticity_controller()
neuromod = pynlm.create_enhanced_neuromodulation_controller()
state = pynlm.create_enhanced_state_manager()
development = pynlm.create_enhanced_development_controller()

# 4. Run enhanced simulation
for step in range(1000):
    brain.step(step)
    neuromod.update_neuromodulators(0.1)
    if step % 100 == 0:
        state.create_checkpoint(f"checkpoint_{step}")

# 5. Save final state
state.save_brain_state("final_state.bin")
```

#### Example: Enhanced Agent Simulation

```python
import pynlm

def run_enhanced_agent_simulation():
    # Setup enhanced configuration
    config = pynlm.create_advanced_config()
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Create enhanced controllers
    plasticity = pynlm.create_enhanced_plasticity_controller()
    neuromod = pynlm.create_enhanced_neuromodulation_controller()
    state = pynlm.create_enhanced_state_manager()
    
    # Enable enhanced features
    plasticity.enable_stdp(True)
    neuromod.enable_curiosity(True)
    neuromod.enable_novelty_detection(True)
    
    # Run enhanced simulation
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        neuromod.update_neuromodulators(0.1)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 200 == 0:
            state.save_incremental(f"episode_{step//200}.bin", f"episode_{step//200}")
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("Enhanced agent simulation completed!")

run_enhanced_agent_simulation()
```

### Documentation

For detailed documentation of all enhanced features, see:

- **README_ENHANCED.md** - Comprehensive guide to all enhanced features
- **docs/ARCHITECTURE.md** - System architecture overview
- **docs/SCIENCE.md** - Scientific background
- **docs/EXPERIMENTS.md** - Experiment descriptions

### Version Information

- **Version**: 2.0.0
- **API Version**: 2.0.0
- **Enhanced Features**: All 10 feature categories added
- **Backward Compatibility**: Full backward compatibility maintained
- **Installation**: `pip install pynlm (>=2.0.0)`

### Advanced Features Summary

| Feature Category | Classes | Key Methods |
|------------------|---------|-------------|
| **Plasticity Control** | PlasticityController | enable_stdp(), set_stdp_params(), switch_rule() |
| **Neuromodulation** | NeuromodulationController | enable_curiosity(), update_neuromodulators() |
| **State Management** | StateManager | save_brain_state(), create_checkpoint(), validate_state() |
| **Development System** | DevelopmentController | set_developmental_stage(), get_plasticity_rate() |
| **Advanced Configuration** | ConfigurationManager | register_preset(), apply_preset(), validate_configuration() |
| **Experiment Management** | ExperimentManager | create_experiment(), run_experiment(), collect_data() |
| **Statistical Analysis** | StatisticalAnalyzer | analyze_time_series(), perform_t_test() |
| **Visualization** | VisualizationExporter | plot_neural_activity(), export_to_csv() |
| **Performance Monitoring** | PerformanceMonitor | start_timer(), profile_neural_computation() |
| **Error Handling** | ErrorHandler | validate_input(), handle_error(), recover_from_error() |

### Migration Guide

For users transitioning from older versions:

1. **Minimal changes needed**: All existing code continues to work
2. **Enhanced features**: Use new enhanced controllers for advanced functionality
3. **Performance**: Enhanced controllers have minimal overhead
4. **Learning curve**: Gradually adopt new features as needed

### Examples and Tutorials

The enhanced bindings include comprehensive examples:

- **Basic usage**: Simple brain simulation with enhanced controllers
- **Research scenarios**: Predefined configurations for different experiments
- **Advanced features**: Demonstrations of all enhanced capabilities
- **Performance benchmarks**: Comprehensive performance testing

## Scientific Limitations

The enhanced Python bindings provide advanced interfaces to NLM, but the same scientific limitations apply as described above. The enhanced features enable more comprehensive research but do not change the fundamental scientific model or claims.

## License

MIT

## Authors

Enhanced Python bindings developed by the NLM research team.

Phase 6 Final Integration with Enhanced Python Bindings.

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
