# Python Bindings Improvement Plan

## Overview
This plan outlines the improvements to make the pynlm Python bindings more Pythonic, user-friendly, and feature-complete.

## Current Issues

### 1. Code Organization
- Monolithic 425-line file in bindings.cpp
- All classes manually defined with repetitive pybind11 boilerplate
- Poor separation of concerns
- Difficult to maintain and extend

### 2. Python API Design
- Missing context managers for resource cleanup
- No property accessors (relying on getters)
- No operator overloading for natural Python usage
- Limited convenience methods for common operations
- No batch operations for efficiency

### 3. Error Handling
- Basic std::runtime_error registration only
- No specific Python exception types
- Limited error context information
- No graceful handling of edge cases

### 4. Documentation & Usability
- Basic docstrings only
- No examples in documentation
- Missing high-level API documentation
- No parameter validation or documentation

### 5. Advanced Features
- Missing high-level convenience functions
- No introspection capabilities
- Limited configuration management APIs
- No simulation management utilities

## Proposed Improvements

### 1. Refactored Code Structure

#### Modular Binding Design
```cpp
// bindings.cpp will be reorganized into logical sections:
namespace py = pybind11;
namespace nlm {
    
    // Section 1: Core Type Bindings
    void bind_core_types(py::module& m);
    
    // Section 2: Basic System Bindings
    void bind_basic_systems(py::module& m);
    
    // Section 3: Advanced System Bindings
    void bind_advanced_systems(py::module& m);
    
    // Section 4: Convenience Functions
    void bind_convenience_functions(py::module& m);
    
    // Section 5: Advanced Python Features
    void bind_advanced_features(py::module& m);
    
    PYBIND11_MODULE(pynlm, m) {
        // Initialize Python exceptions
        register_python_exceptions(m);
        
        // Bind all modules in logical order
        bind_core_types(m);
        bind_basic_systems(m);
        bind_advanced_systems(m);
        bind_convenience_functions(m);
        bind_advanced_features(m);
        
        // Set module documentation
        m.doc() = R"pbdoc(
            NLM (Neural Learning Machine) Python Bindings
            -----------------------------------------------
            An experimental artificial developmental brain simulation framework.
            
            This module provides Python bindings for the NLM C++ neural simulation framework,
            enabling Python developers to create and experiment with artificial brains.
            
            Key Features:
            - Context managers for resource management
            - Pythonic API with properties and operators
            - Comprehensive simulation control
            - Advanced configuration management
            - Batch operations for efficiency
            - Rich introspection and analysis tools
        )pbdoc";
    }
}
```

### 2. Pythonic API Improvements

#### Context Managers for Resource Management
```cpp
class Brain {
public:
    // Existing constructor
    explicit Brain(std::shared_ptr<Config> config);
    
    // Context manager support
    class BrainContext {
        Brain& brain_;
    public:
        BrainContext(Brain& brain) : brain_(brain) {
            brain_.initialize();
        }
        ~BrainContext() {
            brain_.reset();
        }
        Brain& operator*() { return brain_; }
        Brain* operator->() { return &brain_; }
    };
    
    BrainContext as_context() {
        return BrainContext(*this);
    }
    
    // Make Brain usable as context manager
    static Brain create() {
        auto config = std::make_shared<Config>();
        return Brain(config);
    }
    
    static Brain create(const std::string& name) {
        auto config = std::make_shared<Config>();
        config->set("name", name);
        return Brain(config);
    }
};

// Python binding:
py::class_<Brain>(m, "Brain", R"pbdoc(
    The central neural simulation brain class.
    
    This class implements the complete integrated brain simulation with neural
    dynamics, memory systems, prediction, cognition, and neuromodulation.
    
    The Brain class supports context manager usage for automatic resource cleanup:
    
    Example:
        with Brain.create() as brain:
            brain.initialize()
            brain.step(0)
        # Brain automatically reset and cleaned up here
    )pbdoc")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
    .def("initialize", &Brain::initialize, "Initialize the brain")
    .def("step", &Brain::step, py::arg("step"), "Perform a simulation step")
    .def("step", [](Brain& self, size_t step) {
        self.step(step, static_cast<Timestamp>(step) * 0.001);
    }, py::arg("step"), "Perform a simulation step with time")
    // ... more bindings
    .def("__enter__", [](Brain& self) { return self.as_context(); })
    .def("__exit__", [](Brain& self, py::args) {
        self.reset();
    });
```

#### Property Accessors and Pythonic Methods
```cpp
// Enhanced Brain class with Python properties
class Brain {
public:
    // Properties for Pythonic access
    size_t get_neuron_count() const { return get_total_neuron_count(); }
    size_t get_synapse_count() const { return get_total_synapse_count(); }
    float get_ei_ratio() const { return get_excitation_inhibition_ratio(); }
    float get_firing_rate() const { return get_average_firing_rate(); }
    
    // Convenience methods
    bool is_healthy() const {
        return get_total_neuron_count() > 0 && 
               get_average_firing_rate() < 100.0f;
    }
    
    std::string get_status_summary() const {
        std::ostringstream oss;
        oss << "Brain: " << get_total_neuron_count() << " neurons, "
            << get_total_synapse_count() << " synapses, "
            << get_active_neuron_count() << " active, "
            << get_average_firing_rate() << " Hz avg rate";
        return oss.str();
    }
    
    // Batch operations for efficiency
    void step_batch(const std::vector<size_t>& steps) {
        for (size_t step : steps) {
            step(step);
        }
    }
    
    std::vector<float> get_simulation_history(size_t steps) const {
        // Implementation would track history
        return std::vector<float>(steps, 0.0f);
    }
};
```

### 3. Enhanced Error Handling

#### Custom Python Exceptions
```cpp
namespace nlm {
    // Custom exception hierarchy
    class NLMException : public std::runtime_error {
    public:
        explicit NLMException(const std::string& message)
            : std::runtime_error("NLM Error: " + message) {}
    };
    
    class ConfigurationError : public NLMException {
    public:
        explicit ConfigurationError(const std::string& message)
            : NLMException("Configuration: " + message) {}
    };
    
    class SimulationError : public NLMException {
    public:
        explicit SimulationError(const std::string& message)
            : NLMException("Simulation: " + message) {}
    };
    
    class ResourceError : public NLMException {
    public:
        explicit ResourceError(const std::string& message)
            : NLMException("Resource: " + message) {}
    };
}

// Register exceptions with pybind11
void register_python_exceptions(py::module& m) {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const nlm::ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const nlm::SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const nlm::ResourceError& e) {
            PyErr_SetString(PyExc_ResourceWarning, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Make exceptions available to Python
    py::register_exception<nlm::ConfigurationError>(m, "ConfigurationError");
    py::register_exception<nlm::SimulationError>(m, "SimulationError");
    py::register_exception<nlm::ResourceError>(m, "ResourceError");
}
```

### 4. Advanced Convenience Functions

#### High-Level Simulation Management
```cpp
// Convenience functions for common use cases
namespace nlm {
    // Factory functions
    inline std::shared_ptr<Brain> create_brain(const std::string& name = "default") {
        auto config = std::make_shared<Config>();
        config->set("name", name);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }
    
    inline std::shared_ptr<SimpleWorld> create_world(int width, int height, 
                                                     int vision_width = 16, 
                                                     int vision_height = 16) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        return world;
    }
    
    // Simulation utilities
    inline void run_episode(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<SimpleWorld> world,
                           int max_steps = 1000,
                           float& total_reward) {
        total_reward = 0.0f;
        world->reset();
        world->set_agent_start(world->getWidth() / 2, world->getHeight() / 2);
        
        for (int step = 0; step < max_steps; ++step) {
            // Process sensory input
            auto percept = world->get_sensory_percept();
            // Would need AgentBrain here for full loop
            
            // Advance world
            world->update(0.1);
            
            // Check termination conditions
            if (!world->is_valid_position(world->get_agent_body().x, 
                                         world->get_agent_body().y)) {
                break;
            }
        }
    }
    
    // Configuration utilities
    inline std::string generate_preset_config(const std::string& preset_name) {
        // Return preset configuration as string
        if (preset_name == "small") {
            return "neuron_count = 500\nregion_count = 3\nconnection_probability = 0.1";
        } else if (preset_name == "medium") {
            return "neuron_count = 2000\nregion_count = 5\nconnection_probability = 0.05";
        } else if (preset_name == "large") {
            return "neuron_count = 10000\nregion_count = 10\nconnection_probability = 0.01";
        }
        return "neuron_count = 1000\nregion_count = 3\nconnection_probability = 0.1";
    }
}
```

### 5. Advanced Python Features

#### Introspection and Analysis
```cpp
// Introspection capabilities
class Brain {
public:
    // Neural network analysis
    struct NetworkMetrics {
        size_t total_neurons;
        size_t total_synapses;
        float connection_density;
        float excitation_inhibition_ratio;
        float average_firing_rate;
        std::vector<size_t> region_sizes;
        std::vector<size_t> population_sizes;
    };
    
    NetworkMetrics analyze_network() const {
        NetworkMetrics metrics;
        metrics.total_neurons = get_total_neuron_count();
        metrics.total_synapses = get_total_synapse_count();
        metrics.connection_density = metrics.total_synapses / 
                                    (metrics.total_neurons * (metrics.total_neurons - 1) / 2);
        metrics.excitation_inhibition_ratio = get_excitation_inhibition_ratio();
        metrics.average_firing_rate = get_average_firing_rate();
        
        // Collect region and population data
        for (const auto& region : get_regions()) {
            metrics.region_sizes.push_back(region->get_total_neuron_count());
            for (const auto& population : region->get_populations()) {
                metrics.population_sizes.push_back(population->getNeuronCount());
            }
        }
        
        return metrics;
    }
    
    // Python-accessible analysis method
    py::dict get_analysis() const {
        NetworkMetrics metrics = analyze_network();
        py::dict result;
        
        result["total_neurons"] = metrics.total_neurons;
        result["total_synapses"] = metrics.total_synapses;
        result["connection_density"] = metrics.connection_density;
        result["excitation_inhibition_ratio"] = metrics.excitation_inhibition_ratio;
        result["average_firing_rate"] = metrics.average_firing_rate;
        result["region_sizes"] = py::cast(metrics.region_sizes);
        result["population_sizes"] = py::cast(metrics.population_sizes);
        
        return result;
    }
};
```

#### Batch Operations for Performance
```cpp
// Optimized batch operations
class Brain {
public:
    // Batch step for performance
    void step_batch(const std::vector<size_t>& steps, 
                   const std::vector<Timestamp>& times = {}) {
        if (times.empty()) {
            times.resize(steps.size());
            for (size_t i = 0; i < steps.size(); ++i) {
                times[i] = static_cast<Timestamp>(steps[i]) * 0.001;
            }
        }
        
        for (size_t i = 0; i < steps.size(); ++i) {
            step(steps[i], times[i]);
        }
    }
    
    // Bulk configuration operations
    void configure_batch(const std::unordered_map<std::string, ConfigValue>& settings) {
        for (const auto& setting : settings) {
            std::visit([&](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    config->set(setting.first, value);
                } else if constexpr (std::is_same_v<T, int>) {
                    config->set(setting.first, value);
                } else if constexpr (std::is_same_v<T, double>) {
                    config->set(setting.first, value);
                } else if constexpr (std::is_same_v<T, bool>) {
                    config->set(setting.first, value);
                }
            }, setting.second);
        }
    }
};
```

### 6. Implementation Details

#### Backward Compatibility
- Maintain existing API for existing users
- Add new features as extensions
- Deprecate old patterns gradually

#### Performance Considerations
- Use move semantics where possible
- Minimize pybind11 overhead
- Cache frequently used bindings
- Optimize for common use cases

#### Memory Management
- Implement reference counting for Python objects
- Use smart pointers appropriately
- Ensure proper cleanup in destructors
- Support for Python's garbage collection

## Testing and Validation

### Unit Tests
- Python-specific unit tests for new API
- Integration tests with C++ backend
- Performance benchmarks
- Memory leak detection

### Example Validation
```python
# Test that new features work correctly
import pynlm as nlm

# Test context manager
with nlm.Brain.create() as brain:
    brain.initialize()
    brain.step(0)
    assert brain.is_healthy()

# Test properties
config = brain.get_config()
config.set("neuron_count", 100)
brain.initialize()
assert brain.get_neuron_count() == 100

# Test error handling
try:
    invalid_brain = nlm.Brain.create("invalid")
except nlm.ConfigurationError:
    pass  # Expected

print("All tests passed!")
```

## Expected Benefits

1. **Improved Usability**: Pythonic API that's intuitive and easy to use
2. **Better Resource Management**: Context managers prevent resource leaks
3. **Enhanced Debugging**: Rich error messages and analysis tools
4. **Performance**: Batch operations for efficient processing
5. **Maintainability**: Modular, well-organized code structure
6. **Extensibility**: Easy to add new features and functionality
7. **Documentation**: Comprehensive documentation and examples

## Implementation Timeline

1. **Phase 1**: Refactored code structure and basic Pythonic features
2. **Phase 2**: Context managers, properties, and error handling
3. **Phase 3**: Convenience functions and batch operations
4. **Phase 4**: Advanced features and introspection capabilities
5. **Phase 5**: Testing, validation, and documentation

This plan provides a comprehensive roadmap for improving the pynlm Python bindings while maintaining backward compatibility and adding significant new functionality that will make NLM more accessible and powerful for Python users.