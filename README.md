# NLM — Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Summary

NLM is an experimental computational brain project that demonstrates the implementation of biologically-inspired neural computation. The system features spiking neurons with real LIF dynamics, synaptic plasticity, memory systems, neuromodulation, and developmental processes.

## Key Improvements Implemented

### 🐛 Critical Bug Fixes
- **Memory Management**: Fixed Brain move constructor/destructor to prevent double-free errors
- **Thread Safety**: Replaced non-atomic counters with `std::atomic<size_t>` for spike counting
- **Null Pointer Safety**: Added null checks throughout the codebase, especially in LIF integration and plasticity calculations
- **Error Handling**: Improved config file loading with proper error logging and graceful fallback

### ⚡ Performance Optimizations
- **LIF Integration**: Implemented bulk processing with pre-allocated pointer arrays
- **Plasticity Calculations**: Optimized synaptic processing with early exit conditions and bulk operations
- **Synaptic Connectivity**: Reduced redundant iterations and improved memory locality

### 📝 Code Quality Improvements
- **Constants**: Extracted magic numbers into named constants for better maintainability
- **Structure**: Improved code organization and readability
- **Documentation**: Added comprehensive documentation for complex methods and algorithms

## New Features & Advanced Capabilities

### 🧠 Enhanced Brain Systems
- **Phase 6 Integration**: Complete integration of all brain systems (memory, neuromodulation, prediction, cognition)
- **Parallel Processing**: Added parallel task processing capabilities
- **Memory Pools**: Implemented memory pool allocation for neurons and synapses

### ⚙️ Configuration & Flexibility
- **Advanced Config Parser**: Support for both key=value and JSON/YAML formats
- **Command Line**: Enhanced argument parsing with `--key value` and `--key=value` support
- **Runtime Configuration**: Dynamic configuration loading at runtime

### 🌐 Improved Interfaces
- **Documentation**: Comprehensive user and developer documentation
- **Examples**: Extensive code examples and project templates
- **Troubleshooting**: Detailed troubleshooting guide

## Technical Architecture

### Core Components
- **Neurons**: Leaky Integrate-and-Fire (LIF) dynamics with spike-frequency adaptation
- **Synapses**: Event-driven synaptic transmission with delays and plasticity
- **Plasticity**: STDP, Hebbian, and reward-modulated learning rules
- **Memory**: Working memory, episodic memory, and associative memory systems
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error systems

### Integration Patterns
- **Closed-Loop Control**: WORLD → SENSORY → NEURAL → INTERNAL → MEMORY → MOTIVATION → ACTION
- **Hierarchical Organization**: Core → Brain → Agent → World abstraction layers
- **Event-Driven Processing**: Efficient spike propagation and synaptic updates

## Build & Run

### C++ Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Demonstrations
```bash
# Phase 2: Basic neural computation
./nlm

# Phase 3: Simple behaviors  
./nlm_phase3_demo

# Phase 4: Cognitive experiments
./nlm_phase4_demo

# Phase 6: Integration test
./nlm_phase6_demo
```

### Python Bindings
```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Neurons firing: {brain.getFiringNeuronCount()}")
```

## Testing

### Unit Tests
```bash
cd tests
./run_tests.sh
```

### Integration Tests
```bash
./nlm_phase6_demo  # Full integration test
```

## Project Structure

```
src/
├── core/           # Core infrastructure
│   ├── Types/      # Strongly-typed identifiers
│   ├── Config/     # Configuration management
│   ├── Random/     # Deterministic random number generation
│   └── Logger/     # Structured logging
├── brain/          # Neural components
│   ├── Neuron.cpp  # LIF neuron implementation
│   ├── Synapse.cpp # Synaptic transmission
│   └── Brain.cpp   # Main brain coordinator
├── memory/         # Memory systems
├── neuromodulation/ # Neuromodulators
├── prediction/     # Prediction systems
├── cognition/      # Cognitive mechanisms
├── sensory/        # Sensory processing
├── motor/          # Motor system
└── environment/    # Environment interface
├── python/         # Python bindings
└── tests/          # Test suite
```

## Scientific Validation

### Research Questions Explored
1. Can memory systems integrate with neural dynamics?
2. Can neuromodulation coordinate plasticity effectively?
3. Can prediction become a central organizing principle?
4. Can developmental stages modulate learning?
5. Can replay reinforce memory consolidation?

### Phase Milestones
- **Phase 1-5**: Component development and validation
- **Phase 6**: Complete integration and validation

## Performance Metrics

### Simulation Speed
- **Small Networks** (<100 neurons): 10,000+ steps/second
- **Medium Networks** (100-1000 neurons): 1,000-5,000 steps/second
- **Large Networks** (>1000 neurons): 100-1,000 steps/second

### Memory Usage
- **Per Neuron**: ~1KB with memory pool optimization
- **Total Brain**: Scales linearly with neuron count
- **Plasticity Structures**: ~50% overhead for synaptic data

## Contributing

### Development Guidelines
1. Follow the existing modular architecture
2. Use the C++20 standard with strict aliasing rules
3. Implement comprehensive unit tests for new components
4. Document all public APIs with Doxygen-style comments

### Code Style
```cpp
// Use std::vector over raw arrays
std::vector<Neuron*> neurons;

// Prefer smart pointers for ownership
auto config = std::make_shared<Config>();

// Implement move semantics for efficiency
Brain(Brain&& other) noexcept = default;
```

## License

MIT License - See LICENSE file for details.

## Authors

Research project - See docs/ for scientific background and references.
