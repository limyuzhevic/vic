#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World error: " + msg) {}
};

class InputError : public NLMError {
public:
    explicit InputError(const std::string& msg) : NLMError("Input error: " + msg) {}
};

// Pythonic wrapper for Config class
class PyConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    explicit PyConfig(std::shared_ptr<Config> config) : config_(config) {}
    PyConfig() : config_(std::make_shared<Config>()) {}
    
    // Load from JSON file
    void loadFromFile(const std::string& filepath) {
        if (!config_->loadFromFile(filepath)) {
            throw ConfigError("Failed to load config from " + filepath);
        }
    }
    
    // Load from command line args
    bool loadFromArgs(int argc, char** argv) {
        try {
            return config_->loadFromArgs(argc, argv);
        } catch (...) {
            throw ConfigError("Failed to load config from args");
        }
    }
    
    // Save to JSON file
    void saveToFile(const std::string& filepath) {
        if (!config_->saveToFile(filepath)) {
            throw ConfigError("Failed to save config to " + filepath);
        }
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get configuration value
    template<typename T>
    T get(const std::string& key, const T& default_value = T{}) const {
        return config_->get<T>(key, default_value);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Clear configuration
    void clear() {
        config_->clear();
    }
    
    // Get summary
    std::string summary() const {
        return config_->summary();
    }
    
    // Context manager for temporary configuration
    class ScopedConfig {
    private:
        std::shared_ptr<PyConfig> config_;
        std::vector<std::pair<std::string, std::string>> saved_values_;
        
    public:
        explicit ScopedConfig(PyConfig& config) : config_(std::make_shared<PyConfig>(config)) {
            // Save current state
            for (const auto& key : config_.get()->getKeys()) {
                saved_values_.emplace_back(key, config_.get()->get<std::string>(key, ""));
            }
        }
        
        ~ScopedConfig() {
            // Restore state
            for (const auto& [key, value] : saved_values_) {
                config_.get()->getInternal()->set(key, value);
            }
        }
        
        ScopedConfig& set(const std::string& key, const std::string& value) {
            config_.get()->getInternal()->set(key, value);
            return *this;
        }
        
        ScopedConfig& setInt(const std::string& key, int value) {
            config_.get()->getInternal()->set(key, std::to_string(value));
            return *this;
        }
        
        ScopedConfig& setFloat(const std::string& key, float value) {
            config_.get()->getInternal()->set(key, std::to_string(value));
            return *this;
        }
    };
    
    std::unique_ptr<ScopedConfig> scoped() {
        return std::make_unique<ScopedConfig>(*this);
    }
    
    // Operator overloads for Pythonic access
    std::string operator[](const std::string& key) const {
        return get<std::string>(key, "");
    }
    
    template<typename T>
    PyConfig& operator()(const std::string& key, const T& value) {
        config_->set(key, value);
        return *this;
    }
    
    // Python repr
    std::string repr() const {
        std::stringstream ss;
        ss << "<Config: " << config_->summary() << ">";
        return ss.str();
    }
    
    // Python str
    std::string __str__() const {
        return summary();
    }
    
    // Fluent interface methods
    PyConfig& setValue(const std::string& key, const std::string& value) {
        config_->set(key, value);
        return *this;
    }
    
    PyConfig& setInt(const std::string& key, int value) {
        config_->set(key, std::to_string(value));
        return *this;
    }
    
    PyConfig& setFloat(const std::string& key, float value) {
        config_->set(key, std::to_string(value));
        return *this;
    }
    
    PyConfig& setBool(const std::string& key, bool value) {
        config_->set(key, value ? "true" : "false");
        return *this;
    }
    
    // Batch operations
    PyConfig& setValues(const std::unordered_map<std::string, std::string>& values) {
        for (const auto& [key, value] : values) {
            config_->set(key, value);
        }
        return *this;
    }
    
    // Validation
    bool validate(const std::vector<std::string>& required_keys) const {
        for (const auto& key : required_keys) {
            if (!has(key)) return false;
        }
        return true;
    }
    
    // Get as string with default
    std::string getString(const std::string& key, const std::string& default_value = "") const {
        return get<std::string>(key, default_value);
    }
    
    // Get as integer with default
    int getInt(const std::string& key, int default_value = 0) const {
        return get<int>(key, default_value);
    }
    
    // Get as float with default
    float getFloat(const std::string& key, float default_value = 0.0f) const {
        return get<float>(key, default_value);
    }
    
    // Get as boolean with default
    bool getBool(const std::string& key, bool default_value = false) const {
        return get<bool>(key, default_value);
    }
    
    // Check equality
    bool equals(const PyConfig& other) const {
        return config_ == other.config_;
    }
    
    friend bool operator==(const PyConfig& lhs, const PyConfig& rhs) {
        return lhs.config_ == rhs.config_;
    }
    
    friend bool operator!=(const PyConfig& lhs, const PyConfig& rhs) {
        return !(lhs == rhs);
    }
    
    // Get underlying config pointer for advanced usage
    std::shared_ptr<Config> getInternal() const {
        return config_;
    }
    
    // Iterator support
    class Iterator {
    private:
        std::vector<std::string> keys_;
        size_t index_;
        const PyConfig* config_;
        
    public:
        Iterator(const PyConfig* config) : config_(config) {
            keys_ = config->getKeys();
            index_ = 0;
        }
        
        struct Entry {
            std::string key;
            std::string value;
        };
        
        Entry next() {
            if (index_ >= keys_.size()) {
                throw py::stop_iteration();
            }
            const auto& key = keys_[index_++];
            return {key, config_->getString(key, "")};
        }
        
        bool hasNext() const {
            return index_ < keys_.size();
        }
    };
    
    Iterator begin() const {
        return Iterator(this);
    }
    
    Iterator end() const {
        return Iterator(nullptr);
    }
};

// Bindings for IDs
void bind_ids(py::module_& m) {
    // NeuronId
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Provides safe neuron identification with automatic hash support
        for use as dictionary keys and set membership.
        
        Examples:
            >>> neuron_id = NeuronId(42)
            >>> neuron_id.value
            42
            >>> hash(neuron_id)
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), 
             "Create NeuronId from uint64_t value")
        .def_readwrite("value", &NeuronId::value,
                      "The internal uint64_t value of the neuron ID")
        .def("index", &NeuronId::index,
             "Get the neuron index (alias for value)")
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::hash(), [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .def_static("from_index", [](uint64_t index) {
            return NeuronId(index);
        }, py::arg("index"),
        "Create NeuronId from index");
    
    // SynapseId
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Provides safe synapse identification with automatic hash support.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::hash(), [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });
    
    // RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Provides safe region identification.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::hash(), [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        });
    
    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Provides safe population identification.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def(py::self == py::self)
        .def(py::self != py::self);
    
    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;
}

// Bindings for numpy integration
void bind_numpy(py::module_& m) {
    // Convert vector to numpy array
    m.def("to_numpy", [](const std::vector<float>& vec) {
        return py::array_t<float>(vec);
    }, py::arg("vec"), "Convert std::vector<float> to numpy array");
    
    m.def("from_numpy", [](py::array_t<float> arr) {
        std::vector<float> vec;
        vec.assign(arr.data(), arr.data() + arr.size());
        return vec;
    }, py::arg("arr"), "Convert numpy array to std::vector<float>");
    
    m.def("to_numpy", [](const std::vector<int>& vec) {
        return py::array_t<int>(vec);
    }, py::arg("vec"), "Convert std::vector<int> to numpy array");
    
    m.def("from_numpy", [](py::array_t<int> arr) {
        std::vector<int> vec;
        vec.assign(arr.data(), arr.data() + arr.size());
        return vec;
    }, py::arg("arr"), "Convert numpy array to std::vector<int>");
    
    // Vector operations with numpy
    m.def("add_vectors", [](py::array_t<float> a, py::array_t<float> b) {
        if (a.size() != b.size()) {
            throw InputError("Vector sizes must match for addition");
        }
        py::array_t<float> result({a.size()});
        auto a_ptr = a.mutable_data();
        auto b_ptr = b.data();
        auto result_ptr = result.mutable_data();
        for (size_t i = 0; i < a.size(); ++i) {
            result_ptr[i] = a_ptr[i] + b_ptr[i];
        }
        return result;
    }, py::arg("a"), py::arg("b"), "Add two numpy arrays");
    
    m.def("multiply_vector", [](py::array_t<float> vec, float scalar) {
        py::array_t<float> result({vec.size()});
        auto vec_ptr = vec.data();
        auto result_ptr = result.mutable_data();
        for (size_t i = 0; i < vec.size(); ++i) {
            result_ptr[i] = vec_ptr[i] * scalar;
        }
        return result;
    }, py::arg("vec"), py::arg("scalar"), "Multiply numpy array by scalar");
}

// Helper functions for demonstrations
void demo_basic_neural_network(py::module_& m) {
    m.def("demo_basic_neural_network", []() {
        std::cout << "=== NLM Basic Neural Network Demo ===" << std::endl;
        std::cout << "This demo shows a simple neural network simulation:" << std::endl;
        std::cout << "1. Create a brain with default configuration" << std::endl;
        std::cout << "2. Initialize the brain" << std::endl;
        std::cout << "3. Inject current into sensory neurons" << std::endl;
        std::cout << "4. Step the simulation" << std::endl;
        std::cout << "5. Observe neural activity" << std::endl;
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->initialize()) {
            std::cout << "✓ Brain initialized successfully" << std::endl;
            
            // Create a simple world
            auto world = std::make_shared<SimpleWorld>();
            world->configure(100, 100, 10, 10);
            
            // Create agent brain
            auto agent_brain = std::make_shared<AgentBrain>(brain);
            agent_brain->initialize(*world);
            
            std::cout << "✓ Simulation ready for execution" << std::endl;
            std::cout << "Type 'brain.step()' to advance simulation" << std::endl;
        } else {
            std::cout << "✗ Brain initialization failed" << std::endl;
        }
        
        std::cout << "\nEnd of demo." << std::endl;
    }, "Basic neural network demonstration");
}

void demo_world_interaction(py::module_& m) {
    m.def("demo_world_interaction", []() {
        std::cout << "=== NLM World Interaction Demo ===" << std::endl;
        std::cout << "This demo shows agent-world interaction:" << std::endl;
        std::cout << "1. Create world with obstacles" << std::endl;
        std::cout << "2. Set agent position" << std::endl;
        std::cout << "3. Apply motor commands" << std::endl;
        std::cout << "4. Observe sensory input" << std::endl;
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 7, 7);
        
        // Add some objects
        nlm::WorldObject resource{25.0f, 25.0f, WorldObjectType::Resource, 10.0f, 1.0f};
        world->addObject(resource);
        
        nlm::WorldObject wall{30.0f, 30.0f, WorldObjectType::Wall, 0.0f, 1.5f};
        world->addObject(wall);
        
        world->setAgentStart(10.0f, 10.0f);
        
        std::cout << "✓ World created with obstacles" << std::endl;
        std::cout << "  - Resource at (25, 25)" << std::endl;
        std::cout << "  - Wall at (30, 30)" << std::endl;
        std::cout << "  - Agent starts at (10, 10)" << std::endl;
        
        std::cout << "\nTry these actions:" << std::endl;
        std::cout << "  - world.applyMotorCommand(MotorCommand.MoveForward, time)" << std::endl;
        std::cout << "  - world.getSensoryPercept()" << std::endl;
        std::cout << "  - world.isValidPosition(x, y)" << std::endl;
        
        std::cout << "\nEnd of demo." << std::endl;
    }, "World interaction demonstration");
}

void demo_sensory_motor_loop(py::module_& m) {
    m.def("demo_sensory_motor_loop", []() {
        std::cout << "=== NLM Sensory-Motor Loop Demo ===" << std::endl;
        std::cout << "This demo shows the complete sensory-motor loop:" << std::endl;
        std::cout << "1. Sense environment" << std::endl;
        std::cout << "2. Process sensory input" << std::endl;
        std::cout << "3. Generate motor command" << std::endl;
        std::cout << "4. Execute action" << std::endl;
        
        std::cout << "\nTypical workflow in Python:" << std::endl;
        std::cout << "  brain = nlm.createBrain()" << std::endl;
        std::cout << "  agent = nlm.createAgentBrain(brain)" << std::endl;
        std::cout << "  world = nlm.createSimpleWorld()" << std::endl;
        std::cout << "  " << std::endl;
        std::cout << "  # Process sensory input" << std::endl;
        std::cout << "  agent.processSensoryInput(percept)" << std::endl;
        std::cout << "  " << std::endl;
        std::cout << "  # Generate motor command" << std::endl;
        std::cout << "  action = brain.produceAction()" << std::endl;
        std::cout << "  " << std::endl;
        std::cout << "  # Execute in world" << std::endl;
        std::cout << "  result = world.applyMotorCommand(action, time)" << std::endl;
        
        std::cout << "\nEnd of demo." << std::endl;
    }, "Sensory-motor loop demonstration");
}

void demo_advanced_features(py::module_& m) {
    m.def("demo_advanced_features", []() {
        std::cout << "=== NLM Advanced Features Demo ===" << std::endl;
        std::cout << "This demo shows advanced NLM features:" << std::endl;
        std::cout << "1. Neuromodulation (reward prediction)" << std::endl;
        std::cout << "2. Development and plasticity" << std::endl;
        std::cout << "3. Memory systems" << std::endl;
        std::cout << "4. Cognitive functions" << std::endl;
        
        std::cout << "\nAdvanced capabilities in NLM:" << std::endl;
        std::cout << "  - Reward prediction and dopamine modulation" << std::endl;
        std::cout << "  - Structural and synaptic plasticity" << std::endl;
        std::cout << "  - Working and episodic memory" << std::endl;
        std::cout << "  - Neural planning and concept formation" << std::endl;
        std::cout << "  - Curiosity-driven exploration" << std::endl;
        std::cout << "  - Developmental stages" << std::endl;
        
        std::cout << "\nAccess through AgentBrain:" << std::endl;
        std::cout << "  - brain.applyNeuromodulation()" << std::endl;
        std::cout << "  - brain.develop()" << std::endl;
        std::cout << "  - brain.getWorkingMemory()" << std::endl;
        
        std::cout << "\nEnd of demo." << std::endl;
    }, "Advanced features demonstration");
}

// Main module initialization
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        
        This module provides:
        
        - High-level APIs for neural network simulation
        - Pythonic interfaces for all core NLM classes
        - NumPy integration for efficient numerical operations
        - Context managers for resource management
        - Demonstration and educational examples
        - Advanced features for power users
        
        Key features:
        
        * Pythonic convenience methods and fluent interfaces
        * Comprehensive error handling with Python idioms
        * Rich documentation and examples
        * Type hints where possible through pybind11 inference
        * NumPy integration for performance-critical operations
        * Built-in demonstration functions
        * Consistent and intuitive API design
        
        Quick start:
        
        >>> import pynlm
        >>> brain = pynlm.createBrain()
        >>> world = pynlm.createSimpleWorld()
        >>> agent = pynlm.createAgentBrain(brain)
        >>> 
        >>> brain.initialize()
        >>> world.setAgentStart(0, 0)
        >>> 
        >>> # Run simulation
        >>> for step in range(100):
        ...     brain.step(step)
        ...     percept = world.getSensoryPercept()
        ...     agent.processSensoryInput(percept)
        ...     action = brain.produceAction()
        
        See also:
        
        * pynlm.demo_basic_neural_network()
        * pynlm.demo_world_interaction()
        * pynlm.demo_sensory_motor_loop()
        * pynlm.demo_advanced_features()
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Register ID types
    bind_ids(m);
    
    // Register numpy integration
    bind_numpy(m);
    
    // Register demonstration functions
    demo_basic_neural_network(m);
    demo_world_interaction(m);
    demo_sensory_motor_loop(m);
    demo_advanced_features(m);
    
    // Bind Config with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Provides configuration management with Pythonic access patterns,
        including dictionary-like access and method chaining.
        
        Attributes:
            config: Internal configuration dictionary
        
        Examples:
            >>> config = Config()
            >>> config.set("key", "value")
            >>> config["key"]
            'value'
            >>> config.setInt("count", 42)
            >>> config.getString("count")
            '42'
        
        Use with context managers for temporary configurations:
            >>> with config.scoped() as temp:
            ...     temp.set("debug", "true")
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // Bind PyConfig (Pythonic wrapper)
    py::class_<PyConfig>(m, "PyConfig", R"pbdoc(
        Enhanced Pythonic configuration interface for NLM.
        
        Provides a more Pythonic API for configuration management with:
        
        - Dictionary-like access
        - Method chaining for fluent interfaces
        - Context managers for temporary configurations
        - Type conversion utilities
        - Validation helpers
        
        Attributes:
            config: Internal Config instance
        
        Examples:
            >>> config = PyConfig()
            >>> config.setValue("learning_rate", 0.01).setInt("epochs", 100)
            >>> config["learning_rate"]
            '0.01'
            >>> config.getFloat("learning_rate")
            0.01
            >>> 
            >>> # Use context manager
            >>> with config.scoped() as temp:
            ...     temp.set("debug", "true")
            
            >>> # Batch operations
            >>> config.setValues({"a": "1", "b": "2"})
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &PyConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", &PyConfig::loadFromArgs, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments")
        .def("saveToFile", &PyConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &PyConfig::getKeys,
             "Get all configuration keys")
        .def("clear", &PyConfig::clear,
             "Clear all configuration entries")
        .def("summary", &PyConfig::summary,
             "Get a summary string of the configuration")
        .def("__repr__", &PyConfig::repr)
        .def("__str__", &PyConfig::__str__)
        
        // Dictionary-like access
        .def("__getitem__", &PyConfig::getString, py::arg("key"),
             "Get configuration value as string")
        .def("__setitem__", &PyConfig::setValue, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("__contains__", &PyConfig::has, py::arg("key"),
             "Check if key exists")
        
        // Method chaining and fluent interface
        .def("setValue", &PyConfig::setValue, py::arg("key"), py::arg("value"),
             "Set configuration value and return self for chaining")
        .def("setInt", &PyConfig::setInt, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("setFloat", &PyConfig::setFloat, py::arg("key"), py::arg("value"),
             "Set float configuration value")
        .def("setBool", &PyConfig::setBool, py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        
        // Getters with type conversion
        .def("getString", &PyConfig::getString, py::arg("key"), py::arg("default_value") = "",
             "Get configuration value as string with default")
        .def("getInt", &PyConfig::getInt, py::arg("key"), py::arg("default_value") = 0,
             "Get configuration value as integer with default")
        .def("getFloat", &PyConfig::getFloat, py::arg("key"), py::arg("default_value") = 0.0f,
             "Get configuration value as float with default")
        .def("getBool", [](const PyConfig& self, const std::string& key, bool default_value = false) {
            return self.get<bool>(key, default_value);
        }, py::arg("key"), py::arg("default_value") = false,
           "Get configuration value as boolean with default")
        
        // Batch operations
        .def("setValues", &PyConfig::setValues, py::arg("values"),
             "Set multiple configuration values from dictionary")
        
        // Validation
        .def("validate", &PyConfig::validate, py::arg("required_keys"),
             "Validate that required keys exist")
        
        // Context manager
        .def("scoped", &PyConfig::scoped,
             "Create a context manager for temporary configuration changes")
        
        // Equality
        .def("equals", &PyConfig::equals, py::arg("other"),
             "Check if two configurations are equal")
        .def(py::self == py::self)
        .def(py::self != py::self)
        
        // Advanced access
        .def("getInternal", &PyConfig::getInternal,
             "Get internal Config instance (advanced usage)")
        
        // Iterator support
        .def("__iter__", &PyConfig::begin)
        .def("__next__", [](PyConfig::Iterator& it) {
            auto entry = it.next();
            return py::make_tuple(entry.key, entry.value);
        });
    
    // Bind SensoryInput
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input.
        
        Abstract base class representing sensory input data from the environment.
        
        Attributes:
            type: Sensory input type
            data: Raw sensor data
            dimensions: Data dimensionality
            timestamp: Time of input generation
        
        Methods:
            getType(): Get sensory input type
            getData(): Get raw data as vector
            getDimensions(): Get data dimensionality
            getTimestamp(): Get timestamp
            setTimestamp(): Set timestamp
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
    
    // Bind Vision
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual information.
        
        Provides 2D visual sensor data with configurable resolution and color channels.
        
        Args:
            width (int): Image width in pixels
            height (int): Image height in pixels
            channels (int): Number of color channels (default: 3)
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels
            channels: Number of color channels
        
        Methods:
            setData(data): Set image data from vector
            getWidth(): Get image width
            getHeight(): Get image height
            getChannels(): Get number of channels
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create Vision sensor with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from float vector")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");
    
    // Bind Audio
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound information.
        
        Provides audio sensor data with configurable sample rate and duration.
        
        Args:
            sampleRate (int): Audio sampling rate in Hz
            numSamples (int): Number of audio samples
        
        Attributes:
            sampleRate: Audio sampling rate in Hz
            numSamples: Number of audio samples
        
        Methods:
            setData(data): Set audio data from vector
            setSampleRate(): Set sample rate
            getSampleRate(): Get sample rate
            getNumSamples(): Get number of samples
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio sensor with specified sampling rate and sample count")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from float vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");
    
    // Bind InternalSignals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing neural state.
        
        Provides internal neural signal data for self-monitoring and control.
        
        Methods:
            addSignal(value): Add a signal value
            clearSignals(): Clear all signals
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");
    
    // Bind Action
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Represents a motor command with type and parameters.
        
        Args:
            type (ActionType): Type of action
            parameters (list): Action parameters (default: empty)
        
        Attributes:
            type: Action type
            parameters: Action parameters as list
        
        Methods:
            getType(): Get action type
            setType(): Set action type
            getParameters(): Get parameters
            setParameters(): Set parameters
            getName(): Get human-readable action name
            clone(): Create copy of action
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create Action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create Action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create copy of action");
    
    // Bind WorldObject
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        Represents an object in the 2D world with position, type, and properties.
        
        Args:
            x (float): X coordinate
            y (float): Y coordinate
            type (WorldObjectType): Object type
            value (float): Object value (reward or damage)
            radius (float): Object radius (default: 0.5)
        
        Attributes:
            x: X coordinate
            y: Y coordinate
            radius: Object radius
            type: Object type
            value: Object value
            active: Object active state
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create WorldObject with position, type, and properties")
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (reward/damage)")
        .def_readwrite("active", &WorldObject::active, "Object active state");
    
    // Bind AgentBody
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state.
        
        Represents the physical state of an agent in the world.
        
        Attributes:
            x: X position
            y: Y position
            orientation: Orientation in radians
            velocityX: X velocity
            velocityY: Y velocity
            angularVelocity: Angular velocity
            energy: Energy level
            health: Health level
            age: Age in simulation steps
            isMoving: Whether agent is moving
            isTurning: Whether agent is turning
            lastActionTime: Time of last action
        
        Methods:
            reset(): Reset body to initial state
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");
    
    // Bind ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world.
        
        Represents the result of applying a motor command in the world.
        
        Args:
            reward (float): Reward received
            success (bool): Whether action was successful
            message (str): Result message
        
        Attributes:
            reward: Reward received from action
            success: Whether action was successful
            message: Result message
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create ActionResult with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Result message");
    
    // Bind SensoryPercept
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data.
        
        Represents complete sensory percept for an agent.
        
        Contains all available sensory modalities:
        - Vision: Visual sensor data
        - Touch: Proprioceptive data
        - Internal: Internal neural signals
        - Proprioception: Body position data
        - Audio: Audio sensor data
        
        Attributes:
            timestamp: Time of percept generation
        
        Methods:
            getVision(): Get vision data
            setVision(): Set vision data
            getVisionWidth(): Get vision width
            getVisionHeight(): Get vision height
            getTouch(): Get touch data
            setTouch(): Set touch data
            getInternal(): Get internal signals
            setInternal(): Set internal signals
            getProprioception(): Get proprioception data
            setProprioception(): Set proprioception data
            getAudio(): Get audio data
            setAudio(): Set audio data
            getAllSignals(): Get all signals
            getTimestamp(): Get timestamp
            setTimestamp(): Set timestamp
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");
    
    // Bind SimpleWorld
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides a simulated 2D environment for agent development with:
        - Visual and proprioceptive sensory input
        - Physics-based interactions
        - Action execution and reward calculation
        - World object management
        
        Args:
            width (float): World width
            height (float): World height
            visionWidth (int): Vision sensor width
            visionHeight (int): Vision sensor height
        
        Attributes:
            width: World width
            height: World height
            maxEnergy: Maximum agent energy
            energyDecayRate: Energy decay per timestep
        
        Methods:
            configure(width, height, visionWidth, visionHeight): Configure world dimensions
            reset(): Reset world to initial state
            setAgentStart(x, y): Set agent starting position
            update(timestep): Update world state
            applyMotorCommand(cmd, time): Apply motor command
            getSensoryPercept(): Get agent's sensory percept
            getAgentBody(): Get agent's body state
            addObject(obj): Add object to world
            removeObject(x, y): Remove object at position
            isValidPosition(x, y): Check if position is valid
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision sensors")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and get result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get agent's sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");
    
    // Bind Brain
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        
        Args:
            config (Config): Brain configuration
        
        Methods:
            initialize(): Initialize brain with configuration
            step(currentStep): Perform simulation step
            step(currentStep, currentTime): Perform step with timestamp
            receiveSensoryInput(input): Inject sensory input
            injectCurrent(neuron, current): Inject current to specific neuron
            injectCurrentToNeurons(type, current): Inject to all neurons of type
            produceAction(): Generate motor action
            reset(): Reset brain state
            save(filepath): Save brain state
            load(filepath): Load brain state
            addRegion(name): Add neural region
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status");
    
    // Bind AgentBrain
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Handles sensory transduction and motor decoding between brain and environment.
        
        Args:
            brain (Brain): Underlying brain instance
        
        Methods:
            initialize(world): Initialize with world
            getSensoryInputSize(): Get expected sensory input size
            getMotorOutputSize(): Get expected motor output size
            processSensoryInput(percept): Process sensory input
            decodeMotorCommand(): Decode brain activity to motor command
            applyRewardModulation(reward, predicted_reward): Apply reward modulation
            updateDevelopment(timestep): Update development system
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode motor command from brain")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled");
    
    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;
    
    // Demo functions
    demo_basic_neural_network(m);
    demo_world_interaction(m);
    demo_sensory_motor_loop(m);
    demo_advanced_features(m);
    
    // Version and metadata
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Team";
    m.attr("__description__") = "Neural Learning Machine Python Bindings";
    
    // Module-level helper functions
    m.def("version", []() { return std::string("1.0.0"); }, "Get module version");
    
    // NumPy type aliases (for better type hints)
    m.attr("float_array") = py::array_t<float>();
    m.attr("int_array") = py::array_t<int>();
    
    // Quick start guide (for documentation)
    m.def("quick_start", []() {
        std::cout << "=== NLM Quick Start ===" << std::endl;
        std::cout << "1. Import module: import pynlm" << std::endl;
        std::cout << "2. Create brain: brain = pynlm.createBrain()" << std::endl;
        std::cout << "3. Create world: world = pynlm.createSimpleWorld()" << std::endl;
        std::cout << "4. Initialize: brain.initialize()" << std::endl;
        std::cout << "5. Run simulation:" << std::endl;
        std::cout << "   for step in range(100):" << std::endl;
        std::cout << "       brain.step(step)" << std::endl;
        std::cout << "       percept = world.getSensoryPercept()" << std::endl;
        std::cout << "       action = brain.produceAction()" << std::endl;
    }, "Show quick start guide");
}}

} // namespace nlm