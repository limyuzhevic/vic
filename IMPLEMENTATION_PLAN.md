# NLM Python Bindings Implementation Plan

## Overview

This document outlines the specific implementation changes needed to improve the NLM Python bindings for better usability, documentation, and functionality.

## Implementation Priority

### Priority 1: API Design and Usability (Weeks 1-4)

#### 1.1 Add Pythonic Properties and Convenience Methods

**Current Problem:**
- Methods like `brain.getTotalNeuronCount()` are not Pythonic
- Missing properties for common attributes

**Implementation:**

```cpp
// Add property read-only access
py::class_<Brain>(m, "Brain")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
    // Existing methods...
    .def_property_readonly("neuron_count", &Brain::getTotalNeuronCount)
    .def_property_readonly("synapse_count", &Brain::getTotalSynapseCount)
    .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount)
    .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount)
    .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate)
    .def_property_readonly("regions", &Brain::getRegions)
    .def_property_readonly("region_count", &Brain::getRegionCount)
    .def_property_readonly("total_spike_count", &Brain::getTotalSpikeCount);

// Add convenience methods
.brain.def("step", [](Brain& self, int steps) {
    for (int i = 0; i < steps; ++i) {
        self.step(i);
    }
}, py::arg("steps"), "Run multiple simulation steps");

// Add brain status as property
.brain.def_property_readonly("status", [](const Brain& self) {
    std::ostringstream oss;
    oss << "Neurons: " << self.getTotalNeuronCount()
        << ", Active: " << self.getActiveNeuronCount()
        << ", Firing: " << self.getFiringNeuronCount()
        << ", Spikes: " << self.getTotalSpikeCount();
    return oss.str();
});
```

#### 1.2 Add Context Manager Support

**Implementation:**

```cpp
// For Brain class with automatic cleanup
py::class_<Brain>(m, "Brain")
    // ... existing bindings ...
    .def("__enter__", [](py::object self) {
        return self;
    })
    .def("__exit__", [](py::object self, py::object, py::object, py::object) {
        self.attr("reset")();
    });

// For Config class
py::class_<Config>(m, "Config")
    // ... existing bindings ...
    .def("__enter__", [](py::object self) {
        return self;
    })
    .def("__exit__", [](py::object self, py::object, py::object, py::object) {
        self.attr("clear")();
    });
```

#### 1.3 Add Builder Pattern for Brain Creation

**Implementation:**

```cpp
class BrainBuilder {
public:
    BrainBuilder& neuron_count(size_t n) {
        config_->set("neuron_count", static_cast<int64_t>(n));
        return *this;
    }
    
    BrainBuilder& region_count(size_t n) {
        config_->set("region_count", static_cast<int64_t>(n));
        return *this;
    }
    
    BrainBuilder& connection_probability(float p) {
        config_->set("connection_probability", p);
        return *this;
    }
    
    BrainBuilder& enable_plasticity(bool enable) {
        config_->set("enable_plasticity", enable);
        return *this;
    }
    
    std::shared_ptr<Brain> create() {
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }
    
private:
    std::shared_ptr<Config> config_ = std::make_shared<Config>();
    friend class BrainBuilderFactory;
};

class BrainBuilderFactory {
public:
    static BrainBuilder create() {
        return BrainBuilder();
    }
};

// Add to module
.m.def("BrainBuilder", &BrainBuilderFactory::create);
```

#### 1.4 Add High-Level Convenience Functions

**Implementation:**

```cpp
// Add convenient simulation runner
.m.def("run_simulation", [](std::shared_ptr<Brain> brain, 
                         std::shared_ptr<SimpleWorld> world,
                         std::shared_ptr<AgentBrain> agent,
                         size_t steps, float timestep) {
    for (size_t i = 0; i < steps; ++i) {
        world->update(timestep);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(i);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
}, py::arg("brain"), py::arg("world"), py::arg("agent"),
     py::arg("steps"), py::arg("timestep"),
     "Run a complete simulation loop");

// Add quick brain creation
.m.def("quick_brain", [](size_t neuron_count, size_t region_count) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuron_count));
    config->set("region_count", static_cast<int64_t>(region_count));
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize quick brain");
    }
    return brain;
}, py::arg("neuron_count"), py::arg("region_count"),
     "Create a brain with specified parameters");
```

### Priority 2: Enhanced Documentation and Examples (Weeks 5-6)

#### 2.1 Create Documentation System

**Implementation:**

```cpp
// Add documentation strings to key classes
py::class_<Brain>(m, "Brain")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
         "Create a brain with the given configuration. The brain will automatically "
         "initialize neural networks and prepare for simulation.")
    .def("step", &Brain::step, py::arg("currentStep"),
         "Advance the brain by one simulation step. Each step processes "
         "neuron activity, synaptic connections, and plasticity rules.")
    .def("initialize", &Brain::initialize,
         "Initialize the brain with the configured parameters. This sets up "
         "neurons, synapses, and prepares the neural networks for operation.")
    .def("produce_action", &Brain::produceAction,
         "Generate a motor action based on current neural activity. The brain "
         "analyzes motor neuron states to produce appropriate actions.");
```

#### 2.2 Create Comprehensive Example Suite

**File: `/workspace/app/python/examples/`**

1. **Basic Usage** (`basic_examples.py`):
```python
"""Basic usage examples for NLM Python bindings."""
import pynlm

# Example 1: Simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print(f"Brain created with {brain.neuron_count} neurons")

# Example 2: Brain with builder pattern
brain = (pynlm.BrainBuilder()
         .neuron_count(100)
         .region_count(2)
         .connection_probability(0.1)
         .create())

# Example 3: Complete simulation
brain = pynlm.quick_brain(50, 1)
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for i in range(10):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

2. **Advanced Usage** (`advanced_examples.py`):
```python
"""Advanced usage examples demonstrating NLM capabilities."""
import pynlm

# Memory system usage
brain = pynlm.quick_brain(200, 3)
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

# Neuromodulation
brain.getDopamine()->set_level(0.5)
brain.getCuriosity()->set_strength(0.8)

# Development
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
```

3. **Integration Examples** (`integration_examples.py`):
```python
"""Examples of NLM integration with Python scientific stack."""
import pynlm
import numpy as np
import matplotlib.pyplot as plt

# NumPy integration
brain = pynlm.quick_brain(100, 2)
# Use numpy arrays for sensory input
sensory_data = np.random.randn(8, 8, 3)
```

### Priority 3: Error Handling and Type Safety (Weeks 7-8)

#### 3.1 Custom Exception System

**Implementation:**

```cpp
// Define custom exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory Error: " + msg) {}
};

// Register exceptions with Python
void registerExceptions(py::module_& m) {
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
}
```

#### 3.2 Input Validation

**Implementation:**

```cpp
// Enhanced Config validation
bool Config::validate() const {
    auto neuron_count_opt = get<int64_t>("neuron_count");
    if (neuron_count_opt && *neuron_count_opt <= 0) {
        throw ConfigurationError("Neuron count must be positive");
    }
    
    auto region_count_opt = get<int64_t>("region_count");
    if (region_count_opt && *region_count_opt <= 0) {
        throw ConfigurationError("Region count must be positive");
    }
    
    return true;
}

// Add validation to Python bindings
py::class_<Config>(m, "Config")
    .def("validate", &Config::validate, "Validate configuration parameters")
    .def("loadFromFile", [](Config& self, const std::string& filepath) {
        if (!self.loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from " + filepath);
        }
    }, py::arg("filepath"), "Load configuration from a JSON file");
```

### Priority 4: Performance and Memory Management (Weeks 9-10)

#### 4.1 Memory Pool Support

**Implementation:**

```cpp
// Add memory pool management
class MemoryPool {
public:
    MemoryPool() {
        // Initialize memory pools
    }
    
    void* allocateNeuron(size_t size) {
        return neuron_pool_.allocate(size);
    }
    
    void* allocateSynapse(size_t size) {
        return synapse_pool_.allocate(size);
    }
    
    void* allocateRegion(size_t size) {
        return region_pool_.allocate(size);
    }
    
    void freeNeuron(void* ptr) {
        neuron_pool_.free(ptr);
    }
    
    void freeSynapse(void* ptr) {
        synapse_pool_.free(ptr);
    }
    
    void freeRegion(void* ptr) {
        region_pool_.free(ptr);
    }
    
private:
    // Implementation details
};

// Add memory pool to Brain
.m.def("createMemoryPool", []() {
    return std::make_shared<MemoryPool>();
});
```

#### 4.2 Bulk Operations

**Implementation:**

```cpp
// Add bulk neuron injection
.brain.def("inject_current_batch", [](Brain& self, 
                                     const std::vector<NeuronId>& neurons,
                                     float current) {
    for (const auto& neuron : neurons) {
        self.injectCurrent(neuron, current);
    }
}, py::arg("neurons"), py::arg("current"),
     "Inject current into multiple neurons at once");

// Add batch action production
.brain.def("produce_actions_batch", [](Brain& self, size_t count) {
    std::vector<std::unique_ptr<Action>> actions;
    for (size_t i = 0; i < count; ++i) {
        actions.push_back(self.produceAction());
    }
    return actions;
}, py::arg("count"),
     "Produce multiple actions in a batch");
```

### Priority 5: Python Ecosystem Integration (Weeks 11-12)

#### 5.1 NumPy Integration

**Implementation:**

```cpp
// Add NumPy array conversion functions
.m.def("to_numpy", [](const std::vector<float>& vec) {
    return py::array(vec);
}, "Convert std::vector<float> to NumPy array");

.m.def("from_numpy", [](py::array arr) {
    py::buffer_info buf = arr.request();
    float* ptr = static_cast<float*>(buf.ptr);
    size_t size = buf.size;
    return std::vector<float>(ptr, ptr + size);
}, "Convert NumPy array to std::vector<float>");

// Vision class NumPy integration
py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
    .def(py::init<>())
    .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
         py::arg("channels") = 3)
    .def("set_data", [](Vision& self, py::array arr) {
        py::buffer_info buf = arr.request();
        if (buf.ndim != 3) {
            throw std::invalid_argument("Vision data must be 3D array");
        }
        size_t width = buf.shape[0];
        size_t height = buf.shape[1];
        size_t channels = buf.shape[2];
        
        if (width != self.getWidth() || height != self.getHeight() || 
            channels != self.getChannels()) {
            throw std::invalid_argument("Array dimensions don't match Vision dimensions");
        }
        
        float* ptr = static_cast<float*>(buf.ptr);
        std::vector<float> data(ptr, ptr + buf.size);
        self.setData(data);
    }, py::arg("data"), "Set vision data from NumPy array");
```

#### 5.2 Type Hints and Documentation

**Implementation:**

```cpp
// Add comprehensive type hints and documentation
py::class_<Brain>(m, "Brain")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
         "Create a brain with the given configuration. The brain will automatically "
         "initialize neural networks and prepare for simulation.")
    
    // Type hints for return values
    .def_property_readonly("neuron_count", &Brain::getTotalNeuronCount,
                          "The total number of neurons in the brain (int)")
    .def_property_readonly("synapse_count", &Brain::getTotalSynapseCount,
                          "The total number of synapses in the brain (int)")
    .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount,
                          "The number of neurons currently firing (int)")
    
    // Documentation for methods
    .def("step", [](Brain& self, int currentStep) { self.step(currentStep); },
         py::arg("currentStep"), "Advance the brain by one simulation step")
    .def("step_with_time", [](Brain& self, int currentStep, float currentTime) {
             self.step(currentStep, currentTime);
         }, py::arg("currentStep"), py::arg("currentTime"),
         "Advance the brain by one simulation step with explicit time")
    
    // Context manager support
    .def("__enter__", [](py::object self) { return self; })
    .def("__exit__", [](py::object self, py::object, py::object, py::object) {
        self.attr("reset")();
    });
```

## Additional Files to Create

### 1. Python Package Structure

**File: `/workspace/app/python/__init__.py`**
```python
"""
NLM Python bindings for neural simulation.

This module provides Python interfaces to the NLM (Neural Learning Machine) framework,
a computational model of artificial brains with neurons, synapses, and learning mechanisms.

Key components:
- Brain: The central neural simulation engine
- World: The environment containing the brain
- Agent: Interface between brain and world
- Config: Configuration management for NLM systems

Examples:
    import pynlm
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
```

**File: `/workspace/app/python/examples/__init__.py`**
```python
"""
NLM Python examples.

This package contains examples demonstrating various aspects of the NLM framework.
"""
from . import basic_examples
from . import advanced_examples
from . import integration_examples

__all__ = ['basic_examples', 'advanced_examples', 'integration_examples']
```

### 2. Development and Testing Files

**File: `/workspace/app/python/test_bindings.py`**
```python
"""Test suite for NLM Python bindings."""
import pytest
import pynlm

class TestBrainCreation:
    def test_create_default_brain(self):
        """Test creating a brain with default configuration."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        assert brain is not None
        brain.initialize()
        assert brain.neuron_count > 0

    def test_quick_brain(self):
        """Test quick brain creation with parameters."""
        brain = pynlm.quick_brain(100, 2)
        assert brain.neuron_count == 100
        assert brain.region_count == 2

    def test_brain_builder(self):
        """Test BrainBuilder pattern."""
        brain = (pynlm.BrainBuilder()
                .neuron_count(50)
                .region_count(1)
                .connection_probability(0.1)
                .create())
        assert brain.neuron_count == 50
        assert brain.region_count == 1

class TestSimulation:
    def test_simple_step(self):
        """Test basic simulation step."""
        brain = pynlm.quick_brain(50, 1)
        for i in range(10):
            brain.step(i)
            assert brain.neuron_count > 0

    def test_complete_simulation(self):
        """Test complete simulation with world and agent."""
        brain = pynlm.quick_brain(100, 2)
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for i in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify results
        assert brain.neuron_count == 100
        assert world.width == 10
        assert world.height == 10

class TestErrorHandling:
    def test_invalid_configuration(self):
        """Test error handling for invalid configurations."""
        with pytest.raises(pynlm.ConfigurationError):
            config = pynlm.Config()
            # Trigger error
```

## Build System Updates

### 1. Update pyproject.toml

Add comprehensive build configuration:

```toml
[build-system]
requires = ["scikit-build-core>=0.5.0", "pybind11>=2.11.0"]
build-backend = "scikit_build_core.build"

[project]
name = "pynlm"
version = "0.1.0"
description = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
readme = "README.md"
license = {text = "MIT"}
requires-python = ">=3.8"
authors = [
    {name = "NLM Authors"}
]
keywords = ["neural", "simulation", "spiking", "brain", "learning"]
classifiers = [
    "Development Status :: 3 - Alpha",
    "Intended Audience :: Science/Research",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Programming Language :: C++",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
]
dependencies = []

[project.optional-dependencies]
dev = [
    "pytest>=7.0",
    "numpy>=1.20",
    "sphinx>=4.0",
    "sphinx-rtd-theme>=1.0",
]

[project.urls]
Homepage = "https://github.com/nlm-project/nlm"
Documentation = "https://nlm.readthedocs.io"
Repository = "https://github.com/nlm-project/nlm.git"

[project.scripts]
pynlm-example = "python -m pynlm.examples.basic_examples"

[tool.scikit-build]
cmake.minimum-version = "3.16"
cmake.build-type = "Release"
cmake.args = [
    "-DCMAKE_CXX_STANDARD=20",
    "-DCMAKE_CXX_STANDARD_REQUIRED=ON",
    "-DPYTHON_EXECUTABLE={python}",
]

[tool.scikit-build.cmake.define]
NLM_SOURCE_DIR = "src"

[tool.pytest.ini_options]
testpaths = ["python/tests"]
python_files = ["test_*.py"]
python_classes = ["Test*"]
python_functions = ["test_*"]
```

### 2. Add CI/CD Configuration

**File: `.github/workflows/python-package.yml`**
```yaml
name: Python Package

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: ["3.8", "3.9", "3.10", "3.11", "3.12"]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install scikit-build-core pybind11 pytest
        pip install -e .
    
    - name: Run tests
      run: |
        pytest python/tests/
    
    - name: Build package
      run: |
        pip install build
        python -m build
```

## Summary

This implementation plan provides a comprehensive roadmap for improving the NLM Python bindings:

1. **API Design**: Make bindings Pythonic with properties, context managers, and builder patterns
2. **Documentation**: Create multi-level documentation with examples
3. **Error Handling**: Implement robust exception system with validation
4. **Performance**: Add memory pools and bulk operations
5. **Integration**: Integrate with NumPy and other Python scientific tools

The plan prioritizes user experience and provides clear implementation steps for each improvement area.
