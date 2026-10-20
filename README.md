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

### Plugin System (Phase 7: Extensibility)
- Plugin architecture for extensibility
- Dynamic plugin loading and management
- Hook system for extending core functionality
- Event-driven plugin communication
- Plugin security and sandboxing
- Comprehensive plugin examples
- Plugin-based plasticity enhancement
- Plugin-based neuromodulation enhancement

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

## Plugin System (Phase 7: Extensibility)

The NLM project now includes a comprehensive plugin system that enables extensibility and customization of the neural simulation platform. This plugin system provides:

### Core Plugin Components

1. **PluginBase** - Abstract base class for all plugins
2. **PluginManager** - Central coordinator for plugin lifecycle management
3. **PluginRegistry** - Plugin catalog and metadata management
4. **HookSystem** - Extension points for NLM core systems
5. **ComponentFactory** - Factory pattern for plugin instance creation
6. **EventSystem** - Inter-plugin communication through events
7. **PluginConfiguration** - Plugin-specific configuration management
8. **PluginSecurityManager** - Security sandboxing and permission management
9. **PluginUpdateManager** - Plugin version management and updates

### Plugin Types

- **PlasticityRulePlugin** - Extends plasticity mechanisms (STDP, Hebbian, etc.)
- **NeuromodulatorPlugin** - Extends neuromodulation systems (dopamine, serotonin, etc.)
- **DevelopmentPlugin** - Extends developmental processes (synaptogenesis, pruning, maturation)
- **MemoryPlugin** - Extends memory systems (working, episodic, associative)
- **PredictionPlugin** - Extends prediction capabilities
- **CognitionPlugin** - Extends cognitive mechanisms (planning, attention, concept formation)
- **SensoryPlugin** - Extends sensory processing
- **MotorPlugin** - Extends motor output
- **IntegrationPlugin** - Integrates with external systems

### Plugin Features

- **Dynamic Loading**: Load plugins from shared libraries at runtime
- **Thread-Safe Operations**: Thread-safe plugin management
- **Lifecycle Management**: Complete plugin lifecycle (load, initialize, run, shutdown, unload)
- **Dependency Resolution**: Automatic dependency resolution between plugins
- **Version Management**: Plugin version compatibility checking
- **Event-Driven Communication**: Inter-plugin communication through events
- **Security Sandboxing**: Security sandboxing for untrusted plugins
- **Hook System**: Extension points throughout NLM core systems
- **Configuration Management**: Plugin-specific configuration
- **Update Management**: Plugin updates and compatibility

### Example Plugins

#### Enhanced Plasticity Plugin

An enhanced plasticity system that provides:
- Adaptive learning rates based on neural activity
- Homeostatic plasticity for stability
- Metaplasticity (plasticity of plasticity)
- Integration with existing STDP and Hebbian rules

#### Neuromodulation Enhancement Plugin

An advanced neuromodulation system that provides:
- Enhanced dopamine-based reward processing
- Serotonin-based behavior modulation
- Novelty-based curiosity and exploration
- Integration with existing neuromodulation systems

### Plugin Development

To create a new plugin:

1. **Inherit** from appropriate base class
2. **Implement** lifecycle methods
3. **Register** with plugin factory
4. **Configure** plugin-specific parameters
5. **Test** plugin functionality
6. **Distribute** as shared library

### Building with Plugins

To build NLM with plugin system:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j4
```

### Usage Examples

Load and use plugins in your NLM application:

```cpp
#include "PluginSystem.hpp"

int main() {
    // Initialize plugin system
    auto pluginSystem = std::make_shared<nlm::NLMPluginSystem>();
    
    // Load enhanced plasticity plugin
    pluginSystem->loadPlugin("EnhancedPlasticity");
    
    // Initialize and start plugin
    pluginSystem->initializePlugin("EnhancedPlasticity");
    pluginSystem->startPlugin("EnhancedPlasticity");
    
    // Run simulation with plugin support
    // ... simulation code ...
    
    // Stop and shutdown plugin
    pluginSystem->stopPlugin("EnhancedPlasticity");
    pluginSystem->shutdownPlugin("EnhancedPlasticity");
    
    return 0;
}
```

### Plugin System Architecture

The plugin system is designed with the following architecture:

- **Separation of Concerns**: Core system and plugins are separate
- **Encapsulation**: Plugins encapsulate their functionality
- **Abstraction**: Abstract interfaces for plugin operations
- **Polymorphism**: Different plugin types through inheritance
- **Composition**: Plugin composition through factory pattern
- **Communication**: Event-driven communication
- **Security**: Security sandboxing and permission management

### Future Enhancements

Planned enhancements for the plugin system:

- Plugin marketplace
- Plugin documentation
- Plugin templates
- Plugin dependency resolution
- Plugin version compatibility checking
- Plugin rollback and recovery
- Plugin performance monitoring

## License

MIT

## Authors

Research project — See docs for scientific background.
