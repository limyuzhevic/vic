# NLM Developer Guide

## Development Overview

This guide provides comprehensive information for developers who want to extend, modify, or contribute to the NLM (Neural Learning Machine) framework. It covers development setup, coding practices, testing strategies, and ways to extend the system.

## 1. Development Environment Setup

### 1.1 Prerequisites

**Required Software:**
- **Compiler**: C++20 compatible (GCC 9+, Clang 10+, MSVC 2022+)
- **Build System**: CMake 3.16 or higher
- **Package Manager**: For dependencies (apt, brew, chocolatey)

**Optional but Recommended:**
- **Python**: For testing automation and scripts
- **Doxygen**: For API documentation generation
- **Git**: Version control
- **Text Editor**: With C++ support (VS Code, CLion, etc.)

### 1.2 Installation

#### Standard Installation

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)
```

#### Development Installation (with Tests)

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create test build directory
mkdir build_tests
cd build_tests

# Configure with tests enabled
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON

# Build with tests
make -j$(nproc)
```

#### Python Environment Setup

```bash
# Install Python dependencies
pip install pybind11 pytest numpy

# Install NLM Python bindings (if available)
pip install -e .
```

### 1.3 Running the Development Environment

#### Basic Commands

```bash
# Run main NLM demo
./nlm

# Run agent-based experiments
./nlm_phase3_demo

# Run advanced experiments
./nlm_phase4_demo

# Run tests (from test build)
cd build_tests
./nlm_test_runner
```

#### Configuration Management

```bash
# Create custom configuration
mkdir -p configs
echo '{"simulation": {"neuron-count": 1000}}' > configs/custom.cfg

# Backup current configuration
./nlm --backup-config ./backups/config_$(date +%Y%m%d).cfg

# Test configuration
./nlm --config configs/custom.cfg --dry-run
```

## 2. Build System Configuration

### 2.1 CMake Configuration Options

The NLM project uses CMake with the following key options:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=[Debug|Release|RelWithDebInfo] \
  -DBUILD_TESTS=[ON|OFF] \
  -DENABLE_PYTHON_BINDINGS=[ON|OFF] \
  -DENABLE_VISUALIZATION=[ON|OFF] \
  -DENABLE_PERFORMANCE_OPTIMIZATIONS=[ON|OFF] \
  -DTEST_COVERAGE=[ON|OFF] \
  -DDOCUMENTATION=[ON|OFF]
```

#### Detailed Options

| Option | Description | Default | Recommended for Development |
|--------|-------------|---------|---------------------------|
| `CMAKE_BUILD_TYPE` | Build configuration | `Release` | `Debug` for development |
| `BUILD_TESTS` | Build test suite | `OFF` | `ON` for development |
| `ENABLE_PYTHON_BINDINGS` | Python bindings | `OFF` | `ON` for Python development |
| `ENABLE_VISUALIZATION` | Visualization support | `OFF` | `ON` for GUI development |
| `ENABLE_PERFORMANCE_OPTIMIZATIONS` | SIMD and parallel processing | `ON` | `ON` |
| `TEST_COVERAGE` | Test coverage analysis | `OFF` | `ON` for testing |
| `DOCUMENTATION` | Build documentation | `OFF` | `ON` for API docs |

### 2.2 Build Targets

#### Core Library Targets

```bash
# Main neural computation library
make nlm_core

# Agent system library
make nlm_agent

# World simulation library
make nlm_world

# Phase 3 experiments
make nlm_phase3

# Phase 4 experiments
make nlm_phase4
```

#### Executable Targets

```bash
# Basic neural computation demo
make nlm

# Agent-based learning demo
make nlm_phase3_demo

# Advanced cognitive experiments
make nlm_phase4_demo

# Testing
make nlm_test_runner
```

#### Testing Targets

```bash
# Run all tests
make test

# Run specific test targets
make test_neuron
make test_synapse
make test_brain
make test_integration

# Run performance benchmarks
make benchmark
```

## 3. Coding Standards and Best Practices

### 3.1 Code Style

#### Naming Conventions

- **Files**: snake_case (e.g., `neuron.cpp`, `config_manager.cpp`)
- **Classes**: PascalCase (e.g., `NeuralRegion`, `ConfigManager`)
- **Functions**: snake_case (e.g., `getMembranePotential()`)
- **Variables**: snake_case (e.g., `membrane_potential_`, `simulation_step_`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_SPIKE_HISTORY`, `DEFAULT_THRESHOLD`)

#### C++ Standards

- **C++20**: Use modern C++ features (modules, concepts, coroutines)
- **RAII**: Use resource acquisition is initialization pattern
- **Smart Pointers**: Prefer `std::shared_ptr` and `std::unique_ptr`
- **Exception Safety**: Ensure strong exception guarantee where possible
- **Thread Safety**: Document thread safety of public methods

### 3.2 File Structure

#### Source Code Organization

```
src/
├── core/                    # Core utilities and base classes
│   ├── Config/            # Configuration management
│   ├── Random/            # Random number generation
│   ├── Logger/            # Logging framework
│   ├── SimulationClock/   # Simulation timing
│   └── Types/             # Fundamental types and constants
│
├── brain/                  # Neural computation components
│   ├── Brain.cpp          # Main brain coordinator
│   ├── Neuron.cpp         # LIF neuron implementation
│   ├── Synapse.cpp         # Synaptic connections
│   ├── NeuralRegion.cpp   # Brain regions
│   └── NeuralPopulation.cpp # Neuron groups
│
├── memory/                 # Memory systems
│   ├── WorkingMemory.cpp
│   ├── EpisodicMemory.cpp
│   └── AssociativeMemory.cpp
│
├── neuromodulation/         # Neuromodulation systems
│   ├── Dopamine.cpp
│   ├── Curiosity.cpp
│   ├── Novelty.cpp
│   └── PredictionError.cpp
│
├── ... (other system components)
│
├── agent/                  # Agent and world interface
│   ├── AgentBrain.cpp
│   ├── SensoryPercept.cpp
│   └── AgentBody.cpp
│
└── experiments/            # Experiment framework
    ├── Experiment.cpp
    ├── ExperimentRunner.cpp
    └── Metrics.cpp
```

#### Header File Organization

```
src/
├── **/*.hpp               # Public API headers
├── **/*.h                 # Legacy C headers (if any)
└── test/                  # Test headers
```

### 3.3 Code Quality Guidelines

#### Documentation

- **Header Files**: Document all public classes, methods, and parameters
- **Source Files**: Document complex algorithms and design decisions
- **Comments**: Use Doxygen-compatible comments for API documentation
- **README**: Keep README files up-to-date with usage instructions

#### Error Handling

- **Exceptions**: Use standard exceptions for error conditions
- **Error Codes**: Use error codes for recoverable errors
- **Logging**: Log errors with appropriate severity levels
- **Validation**: Validate input parameters and provide clear error messages

#### Memory Management

- **Smart Pointers**: Use `std::shared_ptr` and `std::unique_ptr` for ownership
- **Raw Pointers**: Document when raw pointers are used for performance
- **Circular References**: Break circular references with weak pointers
- **Resource Cleanup**: Ensure proper cleanup in destructors

#### Performance

- **Avoid Large Allocations**: Use object pools for frequently created objects
- **Cache Locality**: Organize data to improve cache performance
- **Vectorization**: Use SIMD instructions for numerical operations
- **Parallel Processing**: Utilize multi-core capabilities where appropriate

## 4. Testing Strategy

### 4.1 Testing Framework

#### Google Test Integration

NLM uses Google Test (gtest) for unit testing:

```bash
# CMake integration (see CMakeLists.txt)
find_package(GTest REQUIRED)
include(GoogleTest)

# Test discovery and execution
gtest_discover_tests(nlm_test_runner)
```

#### Test Structure

```cpp
// test_neuron.cpp
#include <gtest/gtest.h>
#include "../src/brain/Neuron.hpp"

class NeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test neurons
        neuron1 = std::make_unique<Neuron>(NeuronId(1));
        neuron2 = std::make_unique<Neuron>(NeuronId(2));
    }
    
    Neuron& getNeuron(std::unique_ptr<Neuron>& neuron) {
        return *neuron;
    }
    
    std::unique_ptr<Neuron> neuron1;
    std::unique_ptr<Neuron> neuron2;
};

TEST_F(NeuronTest, BasicConstruction) {
    EXPECT_EQ(neuron1->getId().index(), 1);
    EXPECT_EQ(neuron1->getType(), NeuronType::Internal);
}

TEST_F(NeuronTest, MembranePotential) {
    neuron1->setMembranePotential(-65.0f);
    EXPECT_FLOAT_EQ(neuron1->getMembranePotential(), -65.0f);
}

TEST_F(NeuronTest, SpikeGeneration) {
    neuron1->setThreshold(-55.0f);
    neuron1->setMembranePotential(-60.0f);
    EXPECT_FALSE(neuron1->checkThreshold());
    
    neuron1->setMembranePotential(-50.0f);
    EXPECT_TRUE(neuron1->checkThreshold());
}
```

### 4.2 Test Categories

#### Unit Tests

- **Component Testing**: Test individual classes in isolation
- **Algorithm Testing**: Test mathematical algorithms and rules
- **Edge Case Testing**: Test boundary conditions and error cases
- **Random Testing**: Test with deterministic random seeds

#### Integration Tests

- **System Testing**: Test component interactions
- **Cross-Component Testing**: Test data flow between components
- **Configuration Testing**: Test configuration loading and validation
- **State Management Testing**: Test save/load functionality

#### Performance Tests

- **Benchmark Testing**: Measure execution time of critical paths
- **Memory Testing**: Measure memory usage and allocation patterns
- **Scalability Testing**: Test with different system sizes
- **Stress Testing**: Test with extreme parameter values

### 4.3 Test Coverage

#### Code Coverage Goals

- **Unit Tests**: 90%+ coverage for core components
- **Integration Tests**: 80%+ coverage for system interactions
- **Performance Tests**: Comprehensive benchmarking
- **Edge Cases**: 100% coverage of boundary conditions

#### Coverage Metrics

```bash
# Generate coverage report
cd build
cmake --build . --target coverage

# View coverage details
./coverage_report.html
```

### 4.4 Test Writing Guidelines

#### Test Naming Conventions

```cpp
// Good test names
TEST(NeuronTest, ConstructionWithValidId)
TEST(NeuronTest, MembranePotentialAfterInjection)
TEST(NeuronTest, SpikeGenerationAtThreshold)
TEST_F(SynapseTest, STDPWithPreBeforePost)
TEST_F(SynapseTest, HebbianLearningWithCoActivity)
TEST_F(BrainIntegration, SensoryInputProcessing)
```

#### Test Structure

```cpp
class ComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test fixtures
    }
    
    void TearDown() override {
        // Clean up test fixtures
    }
    
    // Test data
    std::unique_ptr<Component> component_;
    RandomGenerator rng_;
};

// Parameterized test
class SynapseParameterTest :
    public ::testing::TestWithParam<std::tuple<float, float, float>> {
protected:
    void SetUp() override {
        auto params = GetParam();
        ltp_weight_ = std::get<0>(params);
        ltd_weight_ = std::get<1>(params);
        tau_ = std::get<2>(params);
    }
    
    float ltp_weight_;
    float ltd_weight_;
    float tau_;
};

TEST_P(SynapseParameterTest, STDPApplication) {
    // Test with different parameter combinations
}

INSTANTIATE_TEST_SUITE_P(
    SynapseParameterTestInstantiation,
    SynapseParameterTest,
    ::testing::Values(
        std::make_tuple(0.02f, 0.015f, 20.0f),
        std::make_tuple(0.05f, 0.01f, 30.0f),
        std::make_tuple(0.01f, 0.02f, 15.0f)
    )
);
```

## 5. Extending NLM

### 5.1 Adding New Neuron Types

#### Creating Custom Neuron Classes

```cpp
// Custom neuron implementation
class CustomNeuron : public Neuron {
public:
    CustomNeuron(NeuronId id) : Neuron(id) {
        setType(NeuronType::Custom);
        setThreshold(-45.0f);  // Different threshold
        setLeakConductance(15.0f);  // Different dynamics
    }
    
    bool stepLIF(Timestamp currentTime, TimestepDuration dt) override {
        // Custom LIF dynamics
        // Your implementation here
        return Neuron::stepLIF(currentTime, dt);
    }
    
    void applyCustomPlasticity() {
        // Custom plasticity rules
        // Your implementation here
    }
};
```

#### Integration with Brain

```cpp
// Register custom neuron type in Brain
void registerCustomNeuronType(Brain& brain) {
    // Add custom neuron population
    auto* region = brain.getRegion(RegionId(1));
    if (region) {
        // Create custom neuron population
        // Add neurons to brain
    }
}
```

### 5.2 Adding New Plasticity Rules

#### Custom STDP Implementation

```cpp
class CustomSTDP : public STDP {
public:
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                float timestep) override {
        // Custom STDP implementation
        // Your STDP rule here
        STDP::update(synapse, preSpikes, postSpikes, 
                    timestep, customLTP, customLTD, customTau);
    }
    
private:
    float customLTP = 0.03f;
    float customLTD = 0.01f;
    float customTau = 25.0f;
};
```

### 5.3 Extending Memory Systems

#### Custom Memory Implementation

```cpp
class CustomWorkingMemory : public NeuralWorkingMemory {
public:
    bool storeToNeuron(NeuronId neuron, float activation) override {
        // Custom memory storage logic
        // Your implementation here
        return NeuralWorkingMemory::storeToNeuron(neuron, activation);
    }
    
    float getTraceStrength(NeuronId neuron) const override {
        // Custom trace strength calculation
        // Your implementation here
        return NeuralWorkingMemory::getTraceStrength(neuron);
    }
};
```

### 5.4 New Environment Models

#### Custom World Implementation

```cpp
class CustomWorld : public SimpleWorld {
public:
    CustomWorld(float width, float height) : SimpleWorld(width, height) {
        // Custom world properties
    }
    
    Observation getObservation(const AgentBody& agent) const override {
        // Custom observation generation
        // Your implementation here
        return SimpleWorld::getObservation(agent);
    }
    
    ActionResult applyAction(const Action& action, 
                            AgentBody& agent) override {
        // Custom action effects
        // Your implementation here
        return SimpleWorld::applyAction(action, agent);
    }
};
```

## 6. Performance Optimization

### 6.1 Profiling and Benchmarking

#### Using Performance Tools

```bash
# GProf profiling
make profiled
./nlm --profiling-output profile.out

# Intel VTune profiling
make vtune
./nlm --vtune-profiling

# Custom timing measurements
./nlm --timing --timing-output timing_results.csv
```

#### Performance Metrics

```cpp
// Performance measurement utilities
class PerformanceProfiler {
public:
    void startTimer(const std::string& name);
    void stopTimer(const std::string& name);
    double getElapsedTime(const std::string& name) const;
    void printReport() const;
    
private:
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timers_;
    std::unordered_map<std::string, double> elapsed_times_;
};
```

### 6.2 Memory Optimization

#### Memory Pool Implementation

```cpp
// Memory pool for frequent allocations
class NeuronMemoryPool {
public:
    static Neuron* allocate() {
        return pool_.allocate();
    }
    
    static void deallocate(Neuron* neuron) {
        pool_.deallocate(neuron);
    }
    
    static void clear() {
        pool_.clear();
    }
    
private:
    static std::vector<Neuron*> pool_;
    static std::mutex mutex_;
};
```

#### Cache Optimization

```cpp
// Cache-friendly data structures
class CacheFriendlyNeuralRegion {
public:
    // Store related data in contiguous memory
    std::vector<Neuron> neurons_;      // Neurons in this region
    std::vector<Synapse> synapses_;    // Synapses from this region
    std::vector<Population> populations_; // Populations in this region
    
    // Access patterns optimized for cache locality
    Neuron* getNeuron(NeuronId id) {
        // Linear search through neurons vector
        // Cache-friendly if neurons accessed sequentially
    }
};
```

### 6.3 SIMD Vectorization

#### SIMD Implementation

```cpp
// SIMD operations for neural computations
class SIMDNeuronProcessor {
public:
    static void updateMembranePotentialSIMD(
        float* membrane_potentials,
        const float* inputs,
        const float* weights,
        int count) {
        // Process multiple neurons simultaneously
        // Using SIMD instructions (SSE, AVX, etc.)
    }
    
    static void applyPlasticitySIMD(
        float* weights,
        const float* eligibility_traces,
        const float* learning_rates,
        int count) {
        // Apply plasticity rules using SIMD
    }
};
```

## 7. Documentation and Code Comments

### 7.1 Doxygen Documentation

#### Comment Format

```cpp
/**
 * @class Neuron
 * @brief Represents a single biological neuron implementing LIF dynamics
 * 
 * The Neuron class models the Leaky Integrate-and-Fire (LIF) neuron model,
 * including membrane potential integration, spike generation, synaptic inputs,
 * and plasticity mechanisms.
 * 
 * @details This class implements the core neural computation unit in NLM,
 * handling both passive (LIF dynamics) and active (plasticity) properties.
 * 
 * @note Memory management uses RAII with smart pointers for safety.
 * @warning This class is not thread-safe for concurrent modifications.
 * 
 * @example
 * @code
 * Neuron neuron(NeuronId(1));
 * neuron.setThreshold(-55.0f);
 * neuron.setMembranePotential(-60.0f);
 * bool fired = neuron.stepLIF(0.0, 0.001);
 * @endcode
 * 
 * @see Synapse, PlasticityRule, NeuralPopulation
 */

/**
 * @brief Creates a new neuron with specified ID
 * 
 * @param id Unique identifier for the neuron
 * 
 * @post Neuron is initialized with:
 *   - Type set to Internal
 *   - Membrane potential set to resting potential (-70 mV)
 *   - Threshold set to typical value (-55 mV)
 *   - All other parameters set to biological defaults
 * 
 * @note Constructor sets up all biological parameters for a realistic neuron.
 */
Neuron::Neuron(NeuronId id);
```

### 7.2 Inline Documentation

```cpp
// Algorithm documentation
void Synapse::applySTDP(...) {
    /**
     * @brief Applies spike-timing dependent plasticity (STDP)
     * 
     * STDP is a learning rule where the precise timing of pre- and 
     * post-synaptic spikes determines synaptic weight changes:
     * 
     * - Pre-before-post: Δw = +A_plus * exp(-Δt/τ_plus)  (potentiation)
     * - Post-before-pre: Δw = -A_minus * exp(-Δt/τ_minus) (depression)
     * - Simultaneous: Δw = 0 (no change)
     * 
     * @param preSpikes Vector of presynaptic spike times
     * @param postSpikes Vector of postsynaptic spike times  
     * @param timestep Current simulation timestep
     * @param ltpWeight STDP potentiation weight (default: 0.02)
     * @param ltdWeight STDP depression weight (default: 0.015)
     * @param tau STDP time constant (default: 20.0 ms)
     * 
     * @note Based on Bi & Poo (1998) STDP formulation
     * @see applyHebbian() for complementary learning rule
     */
    
    // Implementation here
}
```

## 8. Contributing to NLM

### 8.1 Code Contributions

#### Pull Request Process

1. **Fork the Repository**: Create your own fork
2. **Create Branch**: Work on feature-specific branches
3. **Write Tests**: Implement tests for your changes
4. **Update Documentation**: Document new features
5. **Submit Pull Request**: Create PR against main branch
6. **Code Review**: Address feedback and refine changes
7. **Merge**: Your contribution becomes part of NLM

#### Coding Standards for Contributions

```cpp
// Check style with clang-format
clang-format -i src/brain/Neuron.cpp

// Check lint with cppcheck
cppcheck --enable=all src/brain/Neuron.cpp

// Check documentation with doxygen
doxygen Doxyfile
```

### 8.2 Documentation Contributions

#### Adding Documentation

- **New Tutorials**: Add step-by-step guides for new features
- **API Documentation**: Update Doxygen comments for new classes
- **Examples**: Add code examples demonstrating usage
- **Architecture**: Document new system components

#### Documentation Format

```markdown
## Feature: Custom Neuron Types

### Overview
Allows users to extend NLM with custom neuron implementations.

### Usage
```cpp
// Example: Custom LIF neuron with adaptation
class AdaptiveNeuron : public Neuron {
    // Implementation
};
```

### API
- `AdaptiveNeuron(NeuronId)`: Constructor
- `setAdaptationRate(float)`: Set adaptation rate
- `getAdaptationVariable()`: Get current adaptation variable

### Examples
See `examples/custom_neuron.cpp` for complete example.

### References
- Model biological neuron adaptation
- Based on Harris et al. (2012) for spike-frequency adaptation
```
```

### 8.3 Performance Contributions

#### Benchmark Contributions

```cpp
// Add performance benchmark
BENCHMARK_REGISTER_F(NeuronBenchmark, CustomNeuronPerformance) {
    CustomNeuron neuron(NeuronId(1));
    // Benchmark your implementation
}
```

#### Optimization Contributions

```cpp
// Document optimization techniques
/**
 * @optimization CacheFriendlyDataLayout
 * 
 * This optimization improves cache locality by storing related data
 * in contiguous memory blocks.
 * 
 * Benefits:
 * - Reduced cache misses
 * - Improved memory bandwidth utilization
 * - Better performance on modern CPUs
 * 
 * Trade-offs:
 * - Increased memory usage
 * - Less flexible data structures
 * 
 * Usage:
 *   Replace scattered member variables with contiguous arrays
 *   when accessing patterns are predictable
 */
```

## 9. Troubleshooting Development Issues

### 9.1 Common Build Problems

#### Problem: CMake Configuration Errors

```bash
# Error: CMakeLists.txt not found
# Solution: Run from project root directory
mkdir build
cd build
cmake ..
```

#### Problem: Missing Dependencies

```bash
# Error: 'X' command not found
# Solution: Install missing packages
sudo apt-get install build-essential cmake libgtest-dev
```

#### Problem: Compilation Errors

```bash
# Error: 'expected constructor, destructor, or type conversion'
# Solution: Check for missing headers or incorrect namespace
sudo apt-get install libstdc++-9-dev  # Ensure C++20 support
```

### 9.2 Debugging Techniques

#### Debug Build

```bash
# Create debug build with symbols
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_DEBUG_SYMBOLS=ON
make -j$(nproc)

# Run with debug symbols
./nlm --debug
```

#### Memory Debugging

```bash
# Use Valgrind for memory leak detection
valgrind --leak-check=full ./nlm

# Use AddressSanitizer for memory errors
cd build
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address"
make -j$(nproc)
./nlm
```

### 9.3 Performance Debugging

#### Performance Profiling

```bash
# Generate performance profile
./nlm --performance-profile --output profile.txt

# Analyze performance data
python analyze_performance.py profile.txt
```

## 10. Future Development Directions

### 10.1 Planned Features

#### Phase 8: Advanced Neuromodulation
- **Expectancy-based learning**: Learn to predict future rewards
- **Inverse reinforcement learning**: Infer agent goals from behavior
- **Meta-learning**: Learn how to learn more efficiently

#### Phase 9: Enhanced Cognition
- **Theory of mind**: Model other agents' mental states
- **Social learning**: Learn from observing others
- **Abstract reasoning**: Work with symbolic representations

#### Phase 10: Biological Fidelity
- **Channelrhodopsin**: Optogenetic control of neurons
- **Dopamine neurons**: Realistic dopamine neuron modeling
- **Gap junctions**: Electrical coupling between neurons

### 10.2 Research Applications

#### Scientific Research
- **Neural development**: Study how brains develop and adapt
- **Learning mechanisms**: Investigate different learning algorithms
- **Behavior emergence**: Understand how complex behaviors arise
- **Neural coding**: Study how information is encoded in neural activity

#### Computational Neuroscience
- **Brain simulation**: Model specific brain regions
- **Neurodegenerative diseases**: Study disease mechanisms
- **Cognitive enhancement**: Improve learning algorithms
- **Neuroprosthetics**: Interface computers with biological systems

### 10.3 Industry Applications

#### AI Development
- **Neural network training**: Train deep learning models
- **Reinforcement learning**: Develop adaptive AI systems
- **Robotics**: Control intelligent robots
- **Game development**: Create adaptive game opponents

#### Medical Applications
- **Neurofeedback**: Treat neurological disorders
- **Brain-computer interfaces**: Enable direct neural control
- **Cognitive enhancement**: Improve learning and memory
- **Neurorehabilitation**: Support recovery from brain injury

## 11. Community and Support

### 11.1 Online Resources

#### GitHub
- **Repository**: https://github.com/nlm-project/nlm
- **Issues**: Report bugs and request features
- **Discussions**: Community forum for ideas and questions
- **Pull Requests**: Submit code contributions

#### Documentation
- **Website**: https://nlm.readthedocs.io
- **API Reference**: Complete class and method documentation
- **Tutorials**: Step-by-step guides and examples
- **API Examples**: Working code snippets

#### Community
- **Mailing List**: Subscribe for updates and announcements
- **Slack/Discord**: Real-time discussion with other developers
- **Stack Overflow**: Questions with [nlm] tag

### 11.2 Support Channels

#### Technical Support
- **Bug Reports**: File issues with detailed reproduction steps
- **Feature Requests**: Request new features with use cases
- **Code Questions**: Ask about implementation details
- **Performance Issues**: Report performance problems and solutions

#### Development Support
- **Code Review**: Request reviews of pull requests
- **Mentorship**: Get guidance from experienced developers
- **Testing Help**: Get assistance with test writing
- **Documentation Help**: Get help with documentation improvements

### 11.3 Contribution Guidelines

#### Code Quality Standards

- **Peer Review**: All code changes require at least one peer review
- **Testing**: All new code must have corresponding tests
- **Documentation**: All new code must have documentation
- **Style**: Code must follow project coding standards

#### Review Process

1. **Initial Review**: Maintainers check for consistency and completeness
2. **Testing Review**: Ensure tests pass and cover new functionality
3. **Documentation Review**: Verify documentation is accurate and complete
4. **Final Approval**: Maintainers approve for merge to main branch

---

**Developer Guide Complete!** 🛠️

This guide provides comprehensive information for developers working on the NLM project, including:

- **Environment Setup**: Complete development environment configuration
- **Build System**: CMake configuration and build targets
- **Coding Standards**: Code style, structure, and quality guidelines
- **Testing Strategy**: Comprehensive testing framework and best practices
- **Extension Points**: How to add new neuron types, plasticity rules, and systems
- **Performance Optimization**: Profiling, benchmarking, and optimization techniques
- **Documentation**: Doxygen format and inline documentation standards
- **Contribution Process**: Code review, testing, and merge workflow
- **Troubleshooting**: Common issues and debugging techniques
- **Future Directions**: Planned features and research applications

**Ready to contribute?** Check the [CONTRIBUTING.md](CONTRIBUTING.md) file for detailed contribution guidelines and then start extending NLM with your own features!

**Need help?** Contact the development team or join the community discussions.

---

*Last updated: Current session*
*For the most recent updates, check the GitHub repository*
*Need immediate assistance? Check the QUICKSTART.md for emergency setup instructions*