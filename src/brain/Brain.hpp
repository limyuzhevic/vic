# Brain.hpp - NLM Brain Class Documentation

## Overview
The `Brain` class represents the core computational unit of the NLM (Neural Learning Machine) system. It integrates all neural components and computational systems to create an artificial developmental brain capable of real-time neural computation, learning, and adaptation.

## Key Features
- **Real Neural Computation**: Implements Leaky Integrate-and-Fire (LIF) neuron dynamics
- **Event-Driven Processing**: Real-time spike propagation with synaptic delays
- **Multi-System Integration**: Combines memory, neuromodulation, cognition, and development systems
- **Plastic Learning**: STDP, Hebbian, and structural plasticity mechanisms
- **Developmental Timeline**: Progressive maturation through developmental stages
- **Memory Systems**: Working, episodic, and associative memory integration
- **Prediction & Planning**: Neural prediction systems and cognitive planning
- **Neuromodulation**: Dopamine, curiosity, prediction error, and novelty signals
- **Attention Control**: Competitive attentional selection mechanisms
- **Checkpoint/Restore**: State persistence and recovery capabilities

## Architecture
The brain is structured around a core `Impl` class (pImpl pattern) that contains:
- Neural regions with populations and neurons
- Synaptic connectivity and plasticity systems
- Integrated memory subsystems
- Prediction and cognition modules
- Neuromodulation systems
- Development and learning mechanisms
- Checkpoint management

## Usage
```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);

if (brain->initialize()) {
    // Brain is ready for simulation
    brain->step(step, time);
    // Access brain state and statistics
}
```

## Thread Safety
The brain is designed for single-threaded operation during simulation steps. Concurrent access should be properly synchronized.

## Error Handling
- Returns boolean values for initialization and operation success
- Uses NLM logging macros for error reporting
- Provides detailed error messages through `getErrorMessage()`

## Performance Considerations
- Optimized for real-time neural simulation
- Memory-efficient with unique_ptr management
- Event-driven processing minimizes unnecessary computations
- Configurable for different scale requirements

## Files to Include
- `brain/Brain.hpp` - Main brain interface
- `brain/BrainInitializer.hpp` - Standardized initialization (recommended)
- `core/Config/Config.hpp` - Configuration management

## Configuration Keys
Essential configuration parameters:
- `random_seed` - Random number generator seed
- `simulation_timestep` - Integration timestep (seconds)
- `neuron_count` - Total number of neurons
- `region_count` - Number of brain regions
- `connection_probability` - Random connectivity initialization
- `stdp_ltp_weight` - STDP potentiation weight
- `stdp_ltd_weight` - STDP depression weight
- `stdp_tau` - STDP time constant

## Methods

### Core Brain Operations
- `initialize()` - Initialize brain with configuration
- `step(SimulationStep, Timestamp)` - Process one simulation step
- `reset()` - Reset brain to initial state
- `receiveSensoryInput()` - Process sensory input
- `produceAction()` - Select and produce motor actions
- `logStatus()` - Log current brain status and statistics

### System Access
- `getRegion()`, `getTotalNeuronCount()`, `getTotalSynapseCount()`
- `getSpikeSystem()`, `getSTDP()`, `getHebbian()`
- `getWorkingMemory()`, `getEpisodicMemory()`, `getAssociativeMemory()`
- `getPredictionSystem()`, `getPlanner()`, `getAttention()`
- `getDevelopmentSystem()`, `getNeuromodulators()`
- `getRandomGenerator()`

### Plasticity and Learning
- `injectCurrent()` - Direct current injection to neuron
- `injectCurrentToNeurons()` - Inject to neurons by type
- `applyNeuromodulation()` - Apply neuromodulation signals

### State Management
- `save()`, `load()` - Persist/load brain state
- `getExcitationInhibitionRatio()` - Get E/I balance
- `getAverageFiringRate()`, `getFiringNeuronCount()`

### Memory System Access
- `getWorkingMemory()` - Access working memory
- `getEpisodicMemory()` - Access episodic memory
- `getAssociativeMemory()` - Access associative memory

## Example: Basic Usage with BrainInitializer
```cpp
#include "brain/BrainInitializer.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create and configure brain using BrainInitializer
    auto initializer = std::make_shared<BrainInitializer>();
    
    if (initializer->initialize()) {
        auto brain = initializer->getBrain();
        // Brain is ready for simulation
        // Use brain->step(), brain->produceAction(), etc.
    }
    
    return 0;
}
```

## Performance Notes
- Initialization can be time-consuming for large networks
- Spike delivery complexity is O(N) with optimized SpikeSystem
- Memory usage scales with neuron and synapse count
- Plasticity computations scale with spike history size

## Development Status
**Status**: Production-ready (Phase 6)
**Dependencies**: All NLM core systems integrated
**Testing**: Comprehensive integration tests available
**Documentation**: Complete API documentation provided

## See Also
- `BrainInitializer.hpp` - Alternative initialization method
- `SpikeSystem.hpp` - Spike event handling
- `Neuromodulator.hpp` - Neuromodulation systems
- `MemorySystem.hpp` - Memory subsystems
- `PlasticityRule.hpp` - Plasticity mechanisms

## Version History
- **v6.0.0**: Complete integrated artificial brain implementation
- **v5.0.0**: Initial Phase 6 architecture
- **v4.0.0**: Added neuromodulation integration
- **v3.0.0**: Memory system integration
- **v2.0.0**: Basic neural computation
- **v1.0.0**: Initial skeleton implementation

The Brain class represents the culmination of NLM development, providing a comprehensive artificial developmental brain system with all Phase 6 capabilities integrated and functioning.