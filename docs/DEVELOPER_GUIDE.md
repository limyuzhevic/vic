# NLM C++ Developer Guide

## Overview

This document provides comprehensive guidance for C++ developers working with the NLM (Neural Learning Machine) project. It covers everything from basic setup to advanced development, including best practices, debugging, and extension techniques.

The NLM project is organized as a multi-layered C++20 application with clear separation of concerns, making it both powerful and maintainable.

---

## Development Environment Setup

### Prerequisites

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: Make or Ninja
- **Package Manager**: For system dependencies

### Installing Dependencies

#### Ubuntu/Debian

```bash
# Install system dependencies
sudo apt update
sudo apt install -y \
    g++ \
    cmake \
    git \
    wget \
    curl \
    libboost-all-dev \
    pybind11-dev \
    python3-dev \
    python3-pip

# Install Python dependencies
pip3 install pybind11 scikit-build-core pytest numpy
```

#### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git wget curl python3 pybind11

# Install Python dependencies
pip3 install pybind11 scikit-build-core pytest numpy
```

#### Windows

```powershell
# Install Visual Studio with C++ support
# Install CMake from https://cmake.org/download/
# Install Python from https://www.python.org/downloads/

# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy
```

### Building from Source

#### Basic Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)
```

#### Development Build

```bash
# Build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with all tests
mkdir build_test
cd build_test
cmake .. -DBUILD_TESTS=ON
cmake --build . --target all

# Run tests
ctest --output-on-failure
```

#### Installing Python Bindings

```bash
# Build Python bindings
cd build
make pynlm

# Install Python module
pip install -e .

# Or install system-wide
pip install .
```

---

## Project Structure

### Source Directory Layout

```
/workspace/app/src/
├── core/              # Core utilities and types
│   ├── Config/       # Configuration management
│   ├── Random/       # Random number generation
│   ├── Logger/       # Logging system
│   └── ...           # More core components
│
├── brain/             # Neural components
│   ├── Neuron.cpp/hpp    # Individual neurons (LIF model)
│   ├── Synapse.cpp/hpp    # Synaptic connections
│   ├── NeuralPopulation.cpp/hpp # Neuron groups
│   └── Brain.cpp/hpp    # Central brain coordinator
│
├── agent/             # Brain-world interface
│   ├── SensoryPercept.cpp/hpp    # Sensory processing
│   └── AgentBrain.cpp/hpp    # Agent-brain interface
│
├── world/             # Environment simulation
│   └── SimpleWorld.cpp/hpp    # 2D grid world
│
├── experiments/       # Phase experiments
│   ├── Phase3Experiment.cpp/hpp    # Phase 3 experiments
│   ├── Phase4Experiment.cpp/hpp    # Phase 4 experiments
│   └── Phase6Demo.cpp/hpp    # Phase 6 integration
│
└── ...                # Other layers (dynamics, plasticity, etc.)
```

### Build System

#### CMake Configuration

```cmake
# Basic CMake options
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
```

#### Build Targets

```cmake
# Libraries
add_library(nlm_core STATIC src/core/... src/brain/...)
add_library(nlm_agent STATIC src/agent/...)
add_library(nlm_world STATIC src/world/...)
add_library(nlm_phase3 STATIC src/experiments/Phase3Experiment.cpp)
add_library(nlm_phase4 STATIC src/experiments/Phase4Experiment.cpp)

# Executables
add_executable(nlm src/main.cpp)
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
add_executable(nlm_phase6_demo src/experiments/Phase6Demo.cpp)

# Python bindings (via pybind11)
pybind11_add_module(pynlm src/python/python_bindings.cpp)
```

---

## Core Development Concepts

### 1. Component Architecture

NLM follows a modular architecture with the following principles:

#### Single Responsibility
Each class has a single, well-defined purpose:
- **Brain**: Coordinates neural processing
- **Neuron**: Individual neuron dynamics
- **Synapse**: Synaptic connections
- **AgentBrain**: Brain-world interface
- **SimpleWorld**: Environment simulation

#### Clear Interfaces
Components interact through well-defined interfaces:
```cpp
// Brain uses interfaces from other components
class WorkingMemory* Brain::getWorkingMemory() const
class Dopamine* Brain::getDopamine() const

// AgentBrain provides interfaces to the world
void AgentBrain::processSensoryInput(const SensoryPercept& percept)
```

### 2. Memory Management

#### Smart Pointers
NLM uses modern C++ smart pointers for memory safety:

```cpp
// Use shared_ptr for shared ownership
auto config = std::make_shared<Config>();

// Use unique_ptr for unique ownership
auto neuron = std::make_unique<Neuron>(config);
```

#### Memory Pools
For performance-critical components, NLM uses memory pools:

```cpp
// Memory pool for neurons
class NeuronPool {
public:
    Neuron* allocate() {
        if (free_list_.empty()) {
            return new Neuron();
        }
        Neuron* neuron = free_list_.back();
        free_list_.pop_back();
        return neuron;
    }
    
    void deallocate(Neuron* neuron) {
        free_list_.push_back(neuron);
    }
    
private:
    std::vector<Neuron*> free_list_;
};
```

### 3. Event-Driven Architecture

NLM uses event-driven spike propagation for efficiency:

```cpp
// Spike events are processed in a queue
class SpikeSystem {
public:
    void processDelayedSpikes(SimulationStep step, Timestamp time) {
        // Process all spikes scheduled for this time
        while (!spike_queue_.empty() && 
               spike_queue_.front().time <= time) {
            processSpike(spike_queue_.front().spike);
            spike_queue_.pop();
        }
    }
    
private:
    std::priority_queue<SpikeEvent> spike_queue_;
};
```

---

## Developing with NLM

### 1. Creating New Components

#### Adding a New Neuron Type

```cpp
// 1. Create new neuron class
class CustomNeuron : public Neuron {
public:
    CustomNeuron(std::shared_ptr<Config> config) 
        : Neuron(config) {}
    
    void stepLIF(Timestamp time, float dt) override {
        // Custom LIF dynamics
        // Your implementation here
    }
    
    NeuronType getType() const override {
        return NeuronType::Custom;
    }
};

// 2. Register new neuron type in factory
class NeuronFactory {
public:
    static std::unique_ptr<Neuron> createNeuron(
        NeuronType type, std::shared_ptr<Config> config) {
        switch (type) {
            case NeuronType::Excitatory:
                return std::make_unique<ExcitatoryNeuron>(config);
            case NeuronType::Inhibitory:
                return std::make_unique<InhibitoryNeuron>(config);
            case NeuronType::Custom:
                return std::make_unique<CustomNeuron>(config);
            default:
                return std::make_unique<Neuron>(config);
        }
    }
};
```

#### Adding a New Plasticity Rule

```cpp
// 1. Create new plasticity rule class
class CustomPlasticityRule : public PlasticityRule {
public:
    void apply(Neuron* pre, Neuron* post, 
              float dt, float time) override {
        // Your custom plasticity implementation
        // Calculate weight change
        float dw = calculateWeightChange(pre, post, dt, time);
        
        // Apply weight change
        if (pre->getSynapseTo(post)) {
            pre->getSynapseTo(post)->setWeight(
                pre->getSynapseTo(post)->getWeight() + dw
            );
        }
    }
    
    PlasticityRuleType getType() const override {
        return PlasticityRuleType::Custom;
    }
};

// 2. Register in Brain
void Brain::registerPlasticityRule(
    std::unique_ptr<PlasticityRule> rule) {
    plasticity_rules_.push_back(std::move(rule));
}
```

### 2. Extending Existing Components

#### Custom Brain Subclass

```cpp
class CustomBrain : public Brain {
public:
    CustomBrain(std::shared_ptr<Config> config) 
        : Brain(config) {}
    
protected:
    void step(SimulationStep step, Timestamp time) override {
        // Call parent implementation
        Brain::step(step, time);
        
        // Add custom behavior
        customProcessing(step, time);
    }
    
    void customProcessing(SimulationStep step, Timestamp time) {
        // Your custom processing
        // This runs after the standard brain step
    }
};
```

#### Custom World Subclass

```cpp
class CustomWorld : public SimpleWorld {
public:
    CustomWorld() : SimpleWorld() {}
    
    void update(float timestep) override {
        // Call parent implementation
        SimpleWorld::update(timestep);
        
        // Add custom world dynamics
        customUpdate(timestep);
    }
    
    void customUpdate(float timestep) {
        // Your custom world update logic
        // This runs after the standard world update
    }
};
```

### 3. Debugging and Testing

#### Debugging Tips

```cpp
// 1. Enable detailed logging
Logger::getInstance().setLevel(Logger::Level::Debug);

// 2. Use assertions
assert(config != nullptr);
assert(brain != nullptr);

// 3. Check for null pointers
if (!brain->initialize()) {
    std::cerr << "Failed to initialize brain" << std::endl;
    return 1;
}

// 4. Monitor for errors
class ErrorHandler {
public:
    static void handleError(NLMError error) {
        switch (error) {
            case NLMError::INIT_FAILED:
                std::cerr << "Initialization failed" << std::endl;
                break;
            case NLMError::INVALID_CONFIG:
                std::cerr << "Invalid configuration" << std::endl;
                break;
            // Handle other errors
        }
    }
};
```

#### Unit Testing

```cpp
// Example unit test
class BrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<Config>();
        config_->set("brain.neuron_count", 100);
        brain_ = std::make_unique<Brain>(config_);
        brain_->initialize();
    }
    
    void TearDown() override {
        brain_.reset();
        config_.reset();
    }
    
    std::shared_ptr<Config> config_;
    std::unique_ptr<Brain> brain_;
};

TEST_F(BrainTest, Initialization) {
    ASSERT_TRUE(brain_->initialize());
}

TEST_F(BrainTest, NeuronCount) {
    EXPECT_EQ(brain_->getTotalNeuronCount(), 100);
}

TEST_F(BrainTest, Simulation) {
    for (int i = 0; i < 100; ++i) {
        brain_->step(i, i * 0.001f);
    }
    EXPECT_GT(brain_->getTotalSpikeCount(), 0);
}
```

#### Integration Testing

```cpp
// Integration test for agent-world-brain interaction
class IntegrationTest {
public:
    bool runIntegrationTest() {
        // Create all components
        auto config = std::make_shared<Config>();
        Brain brain(config);
        if (!brain.initialize()) return false;
        
        SimpleWorld world;
        world.configure(20, 20, 8, 8);
        world.reset();
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        
        // Run simulation
        for (int step = 0; step < 1000; ++step) {
            world.update(0.1f);
            
            SensoryPercept percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            
            brain.step(step, step * 0.001f);
            
            MotorCommand cmd = agent.decodeMotorCommand();
            world.applyMotorCommand(cmd, world.getSimulationTime());
        }
        
        return true;
    }
};
```

---

## Performance Optimization

### Profiling and Optimization

#### Memory Usage

```bash
# Profile memory usage
valgrind --tool=massif ./nlm_phase6_demo

# Use AddressSanitizer
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address"
make
./nlm_phase6_demo
```

#### CPU Performance

```bash
# Profile with gprof
gcc -g -O2 -fprofile-generate=nlm ./src/main.cpp
make
./nlm ./prof_data

# Analyze results
gprof ./nlm gmon.out
```

#### Cache Optimization

```cpp
// Use struct of arrays (SoA) for better cache locality
struct SoANeutrons {
    std::vector<float> voltages;
    std::vector<float> currents;
    std::vector<float> thresholds;
    std::vector<bool> refractory;
    std::vector<float> weights;
};

// Use alignment for SIMD
alignas(32) float voltage_array[1000];
```

### Optimization Strategies

#### 1. Minimize Dynamic Allocation

```cpp
// Before: Dynamic allocation
for (int i = 0; i < n; ++i) {
    auto neuron = std::make_unique<Neuron>(config);
    neurons_.push_back(std::move(neuron));
}

// After: Pre-allocate with memory pool
for (int i = 0; i < n; ++i) {
    neurons_.push_back(neurons_pool_.allocate(config));
}
```

#### 2. Use constexpr Where Possible

```cpp
// Use constexpr for compile-time constants
constexpr float DEFAULT_V_THRESH = -50.0f;
constexpr float DEFAULT_V_REST = -70.0f;

// Use inline for small functions
inline float calculateMembraneCurrent(float voltage, float tau_mem) {
    return -voltage / tau_mem;
}
```

#### 3. Optimize Loops

```cpp
// Before: Regular loops
for (int i = 0; i < neurons.size(); ++i) {
    neurons[i]->stepLIF(time, dt);
}

// After: Use iterators and range-based for
for (auto& neuron : neurons) {
    neuron->stepLIF(time, dt);
}

// Parallel processing where safe
#pragma omp parallel for
for (int i = 0; i < neurons.size(); ++i) {
    neurons[i]->stepLIF(time, dt);
}
```

---

## Advanced Development

### 1. Extending with Python

#### Creating Python Extensions

```cpp
// Example: Create a new Python module
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Brain.hpp"

namespace py = pybind11;

PYBIND11_MODULE(custom_nlm, m) {
    m.doc() = "Custom NLM extensions";
    
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>&>())
        .def("initialize", &Brain::initialize)
        .def("step", &Brain::step)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount);
    
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("set", &Config::set)
        .def("get", &Config::get)
        .def("has", &Config::has);
    
    // Add custom functions
    m.def("create_custom_brain", [](std::shared_ptr<Config> config) {
        // Custom brain creation logic
        return CustomBrain(config);
    });
}
```

#### Building Python Extensions

```bash
# Add pybind11 extension to CMakeLists.txt
pybind11_add_module(custom_nlm src/custom/custom_bindings.cpp)
target_link_libraries(custom_nlm PRIVATE nlm_core)

# Build and install
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python)
make custom_nlm
pip install dist/custom_nlm-*.whl
```

### 2. Hardware Acceleration

#### SIMD Optimization

```cpp
#include <immintrin.h>

// SIMD for neuron voltage updates
void updateVoltagesSIMD(float* voltages, float* inputs, 
                       int count, float tau_mem) {
    // Load 4 floats at a time
    for (int i = 0; i < count; i += 4) {
        __m128 v = _mm_loadu_ps(&voltages[i]);
        __m128 i = _mm_loadu_ps(&inputs[i]);
        __m128 tau = _mm_set1_ps(tau_mem);
        
        // Calculate dv/dt = -v/tau + i
        __m128 dv = _mm_div_ps(_mm_set1_ps(-1.0f), tau);
        __m128 new_v = _mm_add_ps(_mm_mul_ps(v, dv), i);
        
        _mm_storeu_ps(&voltages[i], new_v);
    }
}
```

#### Multithreading

```cpp
#include <thread>
#include <mutex>

// Thread-safe spike processing
class ThreadSafeSpikeSystem {
public:
    void processSpikes(std::vector<SpikeEvent>& spikes) {
        std::lock_guard<std::mutex> lock(mutex_);
        spike_queue_.insert(spike_queue_.end(), 
                           spikes.begin(), spikes.end());
    }
    
    void processDelayedSpikes(SimulationStep step, Timestamp time) {
        std::lock_guard<std::mutex> lock(mutex_);
        // Process spikes
    }
    
private:
    std::vector<SpikeEvent> spike_queue_;
    std::mutex mutex_;
};
```

### 3. Custom Experiments

#### Creating Custom Experiments

```cpp
class CustomExperiment : public Experiment {
public:
    CustomExperiment(std::shared_ptr<Config> config) 
        : Experiment(config) {}
    
    ExperimentResult run() override {
        ExperimentResult result;
        result.start_time = std::time(nullptr);
        
        // Your experiment logic
        setupExperiment();
        runSimulation();
        collectResults(result);
        
        return result;
    }
    
protected:
    void setupExperiment() override {
        // Custom setup
    }
    
    void runSimulation() override {
        // Custom simulation loop
        for (int step = 0; step < config_->getInt("experiment.max_steps"); ++step) {
            stepExperiment(step);
        }
    }
    
    void collectResults(ExperimentResult& result) override {
        // Custom result collection
        result.metrics["custom_metric"] = calculateCustomMetric();
    }
};
```

---

## Build and Deployment

### Building with Different Options

```bash
# Build with tests
cmake .. -DBUILD_TESTS=ON
make
ctest --output-on-failure

# Build with Python bindings
cmake .. -DPYTHON_EXECUTABLE=$(which python)
make
pip install -e .

# Build with debugging
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make -j$(nproc)

# Build for production
mkdir build_release
cd build_release
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_OPTIMIZE=ON
make -j$(nproc)
```

### Deployment Scripts

#### Simple Deployment Script

```bash
#!/bin/bash
# deploy.sh - Simple deployment script

set -e

BUILD_DIR="build"
INSTALL_DIR="/usr/local"

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target all -j$(nproc)

# Install
sudo cmake --build . --target install

# Build Python bindings
cd ..
pip install -e .

echo "Deployment complete!"
```

#### Development Deployment Script

```bash
#!/bin/bash
# dev_deploy.sh - Development deployment script

set -e

BUILD_DIR="build_dev"

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    rm -rf $BUILD_DIR
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with development options
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DPYTHON_EXECUTABLE=$(which python) \
    -DNLM_DEBUG=ON \
    -DNLM_PROFILING=ON

# Build
cmake --build . --target all -j$(nproc)

# Run tests
ctest --output-on-failure

echo "Development deployment complete!"
```

---

## License and Copyright

### License

NLM is distributed under the MIT License. See `LICENSE` file for details.

### Copyright

```cpp
// Copyright 2026 NLM Contributors
// SPDX-License-Identifier: MIT
```

---

## Contributing

### Contributing Guidelines

1. **Fork the repository**
2. **Create a feature branch**
3. **Make your changes**
4. **Test your changes**
5. **Submit a pull request**

### Code Quality Standards

- Use C++20
- Follow the existing code style
- Add unit tests for new functionality
- Document public APIs
- Use consistent naming conventions

### Pull Request Checklist

- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] Documentation is updated
- [ ] Code style is consistent
- [ ] Unit tests cover new functionality

---

## Support and Troubleshooting

### Getting Help

#### Online Resources

- **Documentation**: `docs/` directory
- **GitHub Issues**: https://github.com/your-org/nlm/issues
- **Discussions**: https://github.com/your-org/nlm/discussions
- **Email**: support@your-org.com

#### Community

- **Slack/Discord**: NLM community channels
- **Stack Overflow**: Questions tagged with "nlm"

### Troubleshooting Common Issues

#### Issue: Brain initialization fails

**Symptoms:**
```
Error: Brain initialization failed
原因: 配置参数无效
```

**Solutions:**
1. Check configuration parameters
2. Verify all dependencies are installed
3. Check system resources (memory, CPU)
4. Run with debug logging

#### Issue: Simulation instability

**Symptoms:**
```
Error: Simulation diverged
NaN values detected in neuron voltages
```

**Solutions:**
1. Reduce learning rates
2. Check time step size
3. Verify neuron parameters
4. Use smaller network size for debugging

#### Issue: Python bindings not working

**Symptoms:**
```
ImportError: No module named 'pynlm'
```

**Solutions:**
1. Install Python bindings: `pip install -e .`
2. Check Python version compatibility
3. Verify pybind11 installation
4. Build with Python support: `cmake .. -DPYTHON_EXECUTABLE=$(which python)`

---

## Future Development

### Planned Features

1. **Advanced Learning Mechanisms**
   - Reinforcement learning
   - unsupervised learning
   - transfer learning

2. **Enhanced Memory Systems**
   - Long short-term memory
   - working memory consolidation
   -梦境重现 (dream replay)

3. **Higher Cognitive Functions**
   - abstract reasoning
   - problem solving
   - creativity

4. **Multi-Agent Systems**
   - social learning
   - communication
   - cooperation

5. **Hardware Acceleration**
   - GPU support
   - neuromorphic chips
   - specialized hardware

### Research Directions

- **Biological Plausibility**: More realistic neuron models
- **Efficiency**: Energy-efficient brain simulations
- **Scalability**: Support for larger networks
- **Explainability**: Interpretable AI

---

## References

1. **Mathematical Background**: "Computational Neuroscience" - Dayan & Abbott
2. **Neural Networks**: "Deep Learning" - Goodfellow, Bengio, Courville
3. **Brain Models**: "Spiking Neural Networks" - Maass et al.
4. **Research Papers**: NLM technical papers and publications

---

## Conclusion

This C++ Developer Guide provides comprehensive documentation for developing with the NLM project. NLM is designed to be both powerful and maintainable, with clear separation of concerns and extensive documentation. Whether you're creating new components, optimizing performance, or extending functionality, this guide provides the tools and knowledge needed to succeed.

The future of NLM is bright, with ongoing research in advanced learning, memory systems, and cognitive functions. The community is actively developing new features and optimizing the codebase for better performance and scalability.