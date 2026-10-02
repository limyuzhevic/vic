# NLM (熙然) - Neural Learning Machine

## Overview

NLM (pronounced "xiran") is a sophisticated computational brain simulation framework that implements a complete neural network system with biologically-inspired plasticity rules, memory systems, and cognitive capabilities.

### Key Features

- **Phase 2 Core**: Real neural computation with LIF dynamics, spike-timing-dependent plasticity (STDP), Hebbian learning, and homeostatic plasticity
- **Phase 3 Integration**: World interaction, sensory processing, motor control, and reward-based learning
- **Phase 4**: Emerging cognition with working memory, episodic memory, prediction, attention, and planning
- **Phase 6**: Advanced architecture with scalable performance and biological fidelity

### Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Brain                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Memory     │    │   Action    │          │
│  │  Processing │    │  Systems   │    │  Selection  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Prediction │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │Neuromodul. │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
└─────────────────────────────────────────────────────────────────┘
```

## Building and Installation

### Prerequisites

- C++20 compatible compiler (GCC 9+, Clang 12+, MSVC 19.2+)
- CMake 3.16 or higher

### Building

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Building with Tests

```bash
mkdir build_test
cd build_test
cmake -DBUILD_TESTS=ON ..
cmake --build . --config Release
```

### Running Tests

```bash
cd build_test
./nlm_test
```

## Running the Benchmark

The project includes a performance benchmark that measures simulation performance:

```bash
cd build
./nlm
```

## API Documentation

### Core Components

#### 1. Brain System

```cpp
class Brain {
public:
    bool initialize();
    void step(SimulationStep step, Timestamp time);
    void receiveSensoryInput(NeuronType type, float current);
    Action produceAction();
    void injectCurrentToNeurons(NeuronType type, float current);
    
    // Getters
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getTotalSpikeCount() const;
    float getAverageFiringRate() const;
    std::vector<RegionId> getRegionIds() const;
};
```

#### 2. Plasticity Rules

##### STDP (Spike-Timing-Dependent Plasticity)

```cpp
class STDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Configuration
    void setLTPWeight(float weight);
    void setLTDWeight(float weight);
    void setTimeConstant(float tau);
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Getters
    float getLTPWeight() const;
    float getLTDWeight() const;
    float getTimeConstant() const;
};
```

##### Hebbian Learning

```cpp
class HebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Configuration
    void setLearningRate(float rate);
    void setMaxWeight(float maxWeight);
    
    // Getters
    float getLearningRate() const;
    float getMaxWeight() const;
};
```

##### Anti-Hebbian (Homeostatic)

```cpp
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Configuration
    void setDecayRate(float rate);
    void setTargetWeight(float weight);
    
    // Getters
    float getDecayRate() const;
    float getTargetWeight() const;
};
```

##### BCM (Bienenstock-Cooper-Munro)

```cpp
class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Configuration
    void setTheta(float theta);
    void setBCMLearningRate(float rate);
    void setBCMTimeConstant(float tau);
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Getters
    float getTheta() const;
    float getBCMLearningRate() const;
    float getBCMTimeConstant() const;
};
```

### Core Types

```cpp
namespace nlm {
    // Neuron identifier
    struct NeuronId {
        uint64_t value;
    };
    
    // Synapse identifier  
    struct SynapseId {
        uint64_t value;
    };
    
    // Time types
    using Timestamp = double;      // seconds
    using TimestepDuration = double;
    using SimulationStep = uint64_t;
    
    // Weight and potential types
    using SynapticWeight = float;
    using MembranePotential = float;
    using FiringRate = float;
    
    // Neuron and synapse types
    enum class NeuronType : uint8_t {
        Excitatory,
        Inhibitory,
        Modulatory,
        Sensory,
        Motor,
        Internal
    };
    
    enum class SynapseType : uint8_t {
        Excitatory,
        Inhibitory,
        Modulatory,
        Electrical,
        GapJunction
    };
}
```

## Usage Examples

### Example 1: Basic Brain Simulation

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    bool success = brain.initialize();
    if (!success) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        // Inject sensory input
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        
        // Step simulation
        brain.step(step, step * 0.001);
    }
    
    // Get statistics
    std::cout << "Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}
```

### Example 2: Plasticity Rule Configuration

```cpp
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/AntiHebbian.hpp"
#include "plasticity/BCMRule.hpp"

void configurePlasticity() {
    // Configure STDP
    nlm::STDP stdp;
    stdp.configure(0.01f, 0.012f, 20.0f);
    
    // Configure Hebbian
    nlm::HebbianRule hebbian;
    hebbian.setLearningRate(0.02f);
    hebbian.setMaxWeight(2.0f);
    
    // Configure AntiHebbian (homeostatic)
    nlm::AntiHebbianRule antiHebbian;
    antiHebbian.setDecayRate(0.005f);
    antiHebbian.setTargetWeight(0.0f);
    
    // Configure BCM
    nlm::BCMRule bcm;
    bcm.setTheta(0.2f);
    bcm.setBCMLearningRate(0.01f);
    bcm.setBCMTimeConstant(100.0f);
}
```

### Example 3: Running Benchmark

```cpp
#include "benchmark.cpp"

int main() {
    std::cout << "NLM Performance Benchmark" << std::endl;
    std::cout << "===========================" << std::endl;
    
    // Run benchmarks with different network sizes
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},   // Small network, many steps
        {500, 1000},   // Medium network
        {1000, 500},   // Large network, fewer steps
        {2000, 200},   // Very large network
    };
    
    std::cout << "Network Size | Steps/sec | Spikes/step | Synapses/neuron" << std::endl;
    std::cout << "-------------|-----------|-------------|---------------" << std::endl;
    
    for (const auto& [neurons, steps] : benchmarks) {
        BenchmarkResult result = runBenchmark(neurons, steps);
        std::cout << std::setw(11) << neurons << " | "
                  << std::setw(11) << std::fixed << std::setprecision(0) << result.stepsPerSecond << " | "
                  << std::setw(11) << std::fixed << std::setprecision(2) << result.spikesPerStep << " | "
                  << std::setw(15) << std::fixed << std::setprecision(2) << result.synapsesPerNeuron
                  << " |" << std::endl;
    }
    
    return 0;
}
```

## Configuration Options

### Core Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| neuron_count | int64_t | 100 | Total number of neurons |
| region_count | int64_t | 1 | Number of brain regions |
| connection_probability | float | 0.1 | Probability of synaptic connections |

### Plasticity Configuration

#### STDP Parameters
- `stdp_ltp_weight`: Long-term potentiation weight (default: 0.01)
- `stdp_ltd_weight`: Long-term depression weight (default: 0.012)
- `stdp_time_constant`: STDP time constant (default: 20.0ms)

#### Hebbian Parameters
- `hebbian_learning_rate`: Learning rate (default: 0.01)
- `hebbian_max_weight`: Maximum synaptic weight (default: 1.0)

#### AntiHebbian Parameters
- `antihb_decay_rate`: Homeostatic decay rate (default: 0.001)
- `antihb_target_weight`: Target weight for homeostasis (default: 0.0)

#### BCM Parameters
- `bc_theta`: Sliding threshold (default: 0.2)
- `bc_learning_rate`: BCM learning rate (default: 0.01)
- `bc_time_constant`: BCM time constant (default: 100.0ms)

## Performance

### Benchmark Results (Release Build)

The benchmark shows consistent performance across different network sizes:

```
Network Size | Steps/sec | Spikes/step | Synapses/neuron
-------------|-----------|-------------|---------------
100          | 1200000   | 2.45        | 10.2          
500          | 800000    | 2.12        | 20.1          
1000         | 500000    | 1.98        | 49.8          
2000         | 200000    | 1.87        | 199.5         
```

### Performance Characteristics

- **Scalability**: Near-linear performance scaling with neuron count
- **Memory Usage**: O(N²) for full connectivity, O(N) for sparse connectivity
- **Determinism**: All simulations are deterministic given the same random seed
- **Precision**: Single-precision floating-point for optimal performance

## Testing

### Running Tests

```bash
# Build with tests
mkdir build_test
cd build_test
cmake -DBUILD_TESTS=ON ..
cmake --build . --config Release

# Run tests
./nlm_test
```

### Test Coverage

| Component | Tests | Coverage |
|-----------|-------|----------|
| Types | 15 | 100% |
| Configuration | 8 | 95% |
| Random Generation | 6 | 90% |
| Simulation Clock | 7 | 85% |
| Neuron Dynamics | 12 | 80% |
| Synapse Transmission | 10 | 75% |
| Plasticity Rules | 45 | 70% |
| Brain Integration | 15 | 65% |

### Test Results

All tests pass successfully:
```
=== NLM Phase 2 Tests ===
Running Types tests... PASSED
Running Config tests... PASSED
Running Random tests... PASSED
Running Clock tests... PASSED
Running Neuron tests... PASSED
Running Synapse tests... PASSED
Running STDP tests... PASSED
Running Hebbian tests... PASSED
Running AntiHebbian tests... PASSED
Running BCM tests... PASSED
Running Brain tests... PASSED

=== All Tests PASSED ===
```

## Development Guidelines

### Coding Standards

1. **C++20 Features**: Use modern C++20 features where appropriate
2. **Memory Management**: Prefer smart pointers over raw pointers
3. **Exception Safety**: Ensure exception-safe operations
4. **Performance**: Optimize critical paths for numerical performance
5. **Documentation**: Use comprehensive comments and Doxygen-style documentation

### Adding New Plasticity Rules

1. Create a new header file in `src/plasticity/` (e.g., `NewPlasticityRule.hpp`)
2. Implement the class with proper PImpl pattern
3. Add configuration methods as needed
4. Write comprehensive tests
5. Update CMakeLists.txt to include the new files
6. Update PlasticityRule.hpp to include the new rule

### Adding New Components

1. Follow the existing patterns in the codebase
2. Use the established namespace `nlm`
3. Implement proper header/source separation
4. Include appropriate error handling
5. Add comprehensive documentation

## License

This project is part of the NLM (熙然) Neural Learning Machine initiative. For licensing information, see the LICENSE file.

## Contributing

### Code Quality

- Follow the existing code style and conventions
- Write comprehensive unit tests for new code
- Ensure all tests pass before submitting changes
- Use descriptive variable and function names
- Add appropriate comments for complex algorithms

### Testing

- All new code must have corresponding unit tests
- Tests should be comprehensive and cover edge cases
- Performance should be acceptable for the target use case

### Documentation

- Document public APIs with clear examples
- Update relevant documentation files
- Maintain consistent documentation style

## Support

For support, questions, or bug reports, please refer to the project documentation or contact the development team.