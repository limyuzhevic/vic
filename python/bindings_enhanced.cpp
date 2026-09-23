// NLM Python Bindings - Enhanced Version
// Provides improved Pythonic API for NLM neural simulation framework

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <iomanip>
#include <sstream>

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

// Helper functions for Python integration
namespace py_helpers {
    // Convert C++ vector to numpy array
    template<typename T>
    py::array_t<T> vector_to_numpy(const std::vector<T>& vec) {
        if (vec.empty()) {
            return py::array_t<T>({0});
        }
        return py::array_t<T>(vec.size(), vec.data());
    }
    
    // Convert numpy array to C++ vector
    template<typename T>
    std::vector<T> numpy_to_vector(const py::array_t<T>& arr) {
        py::buffer_info info = arr.request();
        T* ptr = static_cast<T*>(info.ptr);
        size_t size = info.size;
        return std::vector<T>(ptr, ptr + size);
    }
    
    // Format float with precision
    std::string format_float(float value, int precision = 4) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
    
    // Create exception types that inherit from Python standard exceptions
    class NLMException : public std::runtime_error {
    public:
        explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    class ConfigurationError : public NLMException {
    public:
        explicit ConfigurationError(const std::string& msg) : NLMException(msg) {}
    };
    
    class SimulationError : public NLMException {
    public:
        explicit SimulationError(const std::string& msg) : NLMException(msg) {}
    };
}

// Enhanced SensoryInput with numpy support
class EnhancedSensoryInput : public SensoryInput {
public:
    // Convert from numpy arrays directly
    static std::unique_ptr<SensoryInput> FromVision(const py::array_t<float>& vision, 
                                                   size_t width, size_t height) {
        auto input = std::make_unique<Vision>();
        input->initialize(width, height);
        auto data = py_helpers::numpy_to_vector(vision);
        input->setData(data);
        return input;
    }
    
    static std::unique_ptr<SensoryInput> FromTouch(const py::array_t<float>& touch) {
        auto input = std::make_unique<InternalSignals>();
        auto data = py_helpers::numpy_to_vector(touch);
        for (float val : data) {
            input->addSignal(val);
        }
        return input;
    }
};

// NeuralRegion Pythonic wrapper
class PyNeuralRegion {
public:
    PyNeuralRegion(NeuralRegion* region) : region_(region) {}
    
    // Pythonic property access
    RegionId get_id() const { return region_->getId(); }
    std::string get_name() const { return region_->getName(); }
    
    // Pythonic iteration support
    py::array_t<float> get_neuron_activities() const {
        auto neurons = region_->getAllNeurons();
        std::vector<float> activities;
        activities.reserve(neurons.size());
        for (const auto* neuron : neurons) {
            const auto& state = neuron->getState();
            activities.push_back(std::abs(state.membranePotential - state.restingPotential));
        }
        return py_helpers::vector_to_numpy(activities);
    }
    
    py::array_t<float> get_synapse_weights() const {
        auto synapses = region_->getSynapses();
        std::vector<float> weights;
        weights.reserve(synapses.size());
        for (const auto* synapse : synapses) {
            weights.push_back(synapse->getWeight());
        }
        return py_helpers::vector_to_numpy(weights);
    }
    
    // Statistics methods
    float get_average_excitatory_weight() const {
        float total = 0.0f;
        size_t count = 0;
        auto synapses = region_->getSynapses();
        for (const auto* synapse : synapses) {
            float weight = synapse->getWeight();
            if (weight > 0) {
                total += weight;
                count++;
            }
        }
        return count > 0 ? total / count : 0.0f;
    }
    
    size_t get_spiking_neuron_count() const {
        auto neurons = region_->getAllNeurons();
        size_t count = 0;
        for (const auto* neuron : neurons) {
            if (neuron->isFiring()) {
                count++;
            }
        }
        return count;
    }
    
    // Python context manager support
    void enable_plasticity(bool enable_hebbian, bool enable_stdp, bool enable_structural) {
        auto synapses = region_->getSynapses();
        for (auto* synapse : synapses) {
            synapse->enablePlasticity(enable_hebbian, enable_stdp, enable_structural);
        }
    }
    
    NeuralRegion* get_raw_region() { return region_; }
    
private:
    NeuralRegion* region_;
};

// Advanced Brain wrapper with Pythonic enhancements
class PyBrain {
public:
    PyBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Simulation with Pythonic progress tracking
    void step_batch(size_t steps, bool verbose = false) {
        for (size_t i = 0; i < steps; ++i) {
            brain_->step(i, i * 0.001);
            if (verbose && i % 1000 == 0) {
                auto stats = get_summary();
                std::cout << "Step " << i 
                         << " | Firing rate: " << stats["avg_firing_rate"]
                         << " | Total spikes: " << stats["total_spikes"]
                         << " | Neurons: " << stats["total_neurons"]
                         << std::endl;
            }
        }
    }
    
    // Pythonic statistics collection
    py::dict get_summary() const {
        py::dict stats;
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain_->getAverageFiringRate();
        stats["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        
        if (brain_->getWorkingMemory()) {
            stats["working_memory_traces"] = brain_->getWorkingMemory()->getActiveTraces();
        }
        if (brain_->getEpisodicMemory()) {
            stats["episodes_stored"] = brain_->getEpisodicMemory()->getEpisodeCount();
        }
        if (brain_->getDopamine()) {
            stats["dopamine_level"] = brain_->getDopamine()->getLevel();
        }
        
        return stats;
    }
    
    // Enhanced plotting support
    py::dict get_neural_activity_plot_data() const {
        py::dict data;
        
        std::vector<float> neuron_activities;
        std::vector<float> synapse_weights;
        
        for (const auto& region : brain_->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (const auto* neuron : neurons) {
                const auto& state = neuron->getState();
                neuron_activities.push_back(std::abs(state.membranePotential - state.restingPotential));
            }
            
            auto synapses = region->getSynapses();
            for (const auto* synapse : synapses) {
                synapse_weights.push_back(synapse->getWeight());
            }
        }
        
        data["neuron_activities"] = py_helpers::vector_to_numpy(neuron_activities);
        data["synapse_weights"] = py_helpers::vector_to_numpy(synapse_weights);
        
        return data;
    }
    
    // Configuration shortcuts
    void configure_standard(int neuron_count = 1000, int region_count = 1) {
        auto config = brain_->getConfig();
        config->set("neuron_count", neuron_count);
        config->set("region_count", region_count);
        config->set("connection_probability", 0.1f);
        brain_->reset();
        brain_->initialize();
    }
    
    // Development stage shortcuts
    void develop_to_adult() {
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    void enable_all_plasticity() {
        auto regions = brain_->getRegions();
        for (auto* region : regions) {
            if (region) {
                region->enableAllPlasticity(true);
            }
        }
    }
    
    std::shared_ptr<Brain> get_raw_brain() { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced AgentBrain with Pythonic interfaces
class PyAgentBrain {
public:
    PyAgentBrain(std::shared_ptr<Brain> brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    
    // Pythonic sensory processing
    void process_vision(const py::array_t<float>& vision, size_t width = 16, size_t height = 16) {
        if (vision.size() < width * height) {
            throw py_helpers::ConfigurationError("Vision array size too small");
        }
        
        SensoryPercept percept;
        auto data = py_helpers::numpy_to_vector(vision);
        std::vector<float> vision_data(data.begin(), data.begin() + width * height);
        percept.setVision(vision_data);
        
        agent_brain_->processSensoryInput(percept);
    }
    
    // Learning experiment with detailed results
    py::dict run_learning_experiment(size_t steps = 1000) {
        py::dict results;
        
        // Record initial state
        auto initial_stats = get_summary();
        
        // Enable plasticity
        agent_brain_->enableAllPlasticity(true, true, false);
        
        // Run learning simulation
        for (size_t i = 0; i < steps; ++i) {
            // Simple learning pattern: inject sensory input
            if (i % 10 == 0) {
                std::vector<float> pattern(256, 0.0f);
                pattern[i % 256] = 1.0f;  // Simple pattern
                agent_brain_->process_vision(pattern_to_array(pattern, 16, 16));
            }
            
            // Brain step (assuming we have access to the brain)
            // This would need modification based on actual API
        }
        
        // Record final state
        auto final_stats = get_summary();
        
        // Calculate changes
        results["initial_weight_mean"] = initial_stats.get("avg_synaptic_weight", 0.0f);
        results["final_weight_mean"] = final_stats.get("avg_synaptic_weight", 0.0f);
        results["weight_change"] = results["final_weight_mean"].cast<float>() - 
                                   results["initial_weight_mean"].cast<float>();
        
        return results;
    }
    
    py::dict get_summary() const {
        py::dict stats;
        auto brain = agent_brain_->getBrain();
        if (brain) {
            // Use the enhanced brain wrapper
            PyBrain py_brain(brain->shared_from_this());
            stats = py_brain.get_summary();
        }
        
        // Add agent-specific stats
        stats["curiosity_level"] = agent_brain_->getCuriosityLevel();
        stats["novelty_level"] = agent_brain_->getNoveltyLevel();
        stats["dopamine_level"] = agent_brain_->getNeuromodulationLevel();
        stats["developmental_stage"] = static_cast<int>(agent_brain_->getDevelopmentalStage());
        
        return stats;
    }
    
    // Configuration methods
    void enable_all_systems() {
        agent_brain_->enableRewardModulation(true);
        agent_brain_->enableStructuralPlasticity(true);
        agent_brain_->enableDevelopment(true);
        agent_brain_->enableCuriosity(true);
    }
    
    AgentBrain* get_raw_agent_brain() { return agent_brain_.get(); }
    
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    
    // Helper for vision array
    py::array_t<float> pattern_to_array(const std::vector<float>& pattern, 
                                       size_t width, size_t height) {
        return py_helpers::vector_to_numpy(pattern);
    }
};

// Configuration enhancements
class PyConfig {
public:
    PyConfig() : config_(std::make_shared<Config>()) {}
    
    // Preset configurations
    void use_research_settings() {
        config_->clear();
        config_->set("neuron_count", 5000);
        config_->set("region_count", 3);
        config_->set("connection_probability", 0.15f);
        config_->set("simulation_timestep", 0.001);
        config_->set("stdp_ltp_weight", 0.02f);
        config_->set("stdp_ltd_weight", 0.015f);
        config_->set("synaptogenesis_rate", 0.00005f);
        config_->set("pruning_rate", 0.000005f);
    }
    
    void use_high_performance_settings() {
        config_->clear();
        config_->set("neuron_count", 10000);
        config_->set("region_count", 5);
        config_->set("connection_probability", 0.2f);
        config_->set("simulation_timestep", 0.0005);
        config_->set("stdp_ltp_weight", 0.01f);
        config_->set("stdp_ltd_weight", 0.012f);
        config_->set("synaptogenesis_rate", 0.00002f);
        config_->set("pruning_rate", 0.000002f);
    }
    
    void use_quick_test_settings() {
        config_->clear();
        config_->set("neuron_count", 100);
        config_->set("region_count", 1);
        config_->set("connection_probability", 0.05f);
        config_->set("simulation_timestep", 0.001);
        config_->set("stdp_ltp_weight", 0.01f);
        config_->set("stdp_ltd_weight", 0.012f);
        config_->set("synaptogenesis_rate", 0.0001f);
        config_->set("pruning_rate", 0.00001f);
    }
    
    std::shared_ptr<Config> get_raw_config() { return config_; }
    
    // Pythonic get/set with type conversion
    template<typename T>
    void set(const std::string& key, const T& value) {
        config_->set(key, value);
    }
    
    template<typename T>
    T get(const std::string& key, const T& default_value) const {
        auto value = config_->get<T>(key);
        return value ? *value : default_value;
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    py::dict to_dict() const {
        py::dict dict;
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            dict[key] = config_->getOr<std::string>(key, "");
        }
        return dict;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// World enhancements
class PyWorld {
public:
    PyWorld() : world_(std::make_shared<SimpleWorld>()) {}
    
    // Environment setup with Pythonic API
    void setup_town(size_t size = 32) {
        world_->initialize(size, size);
        world_->setAgentStart(size / 2, size / 2);
        
        // Add some structure
        for (size_t x = 0; x < size; x += 8) {
            for (size_t y = 0; y < size; y += 8) {
                if ((x + y) % 16 == 0) {
                    WorldObject marker;
                    marker.x = x;
                    marker.y = y;
                    marker.type = WorldObjectType::Marker;
                    marker.radius = 0.3f;
                    marker.active = true;
                    world_->addObject(marker);
                }
            }
        }
    }
    
    // Visualization helper
    py::dict get_visualization_data() const {
        py::dict data;
        
        std::vector<float> agent_positions;
        std::vector<float> object_positions;
        std::vector<int> object_types;
        
        auto body = world_->getAgentBody();
        agent_positions = {body->x, body->y};
        
        // This would need access to internal world state
        data["agent_position"] = py_helpers::vector_to_numpy(agent_positions);
        data["map_size"] = py::make_tuple(world_->getWidth(), world_->getHeight());
        
        return data;
    }
    
    SimpleWorld* get_raw_world() { return world_.get(); }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Main bindings module
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------------
        Enhanced Python bindings for NLM with improved user experience,
        numpy integration, and advanced features for expert users.
        
        Features:
        - Pythonic API design with convenience methods
        - Direct numpy array integration
        - Enhanced error handling and context
        - Preset configuration settings
        - Progress tracking and monitoring
        - Learning experiment helpers
        - Visualization data helpers
    )pbdoc";
    
    // Register enhanced exception types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const py_helpers::NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Expose core types with enhanced documentation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });
    
    // Expose enhanced config class
    py::class_<PyConfig>(m, "Config", R"pbdoc(Improved configuration system with preset settings)")
        .def(py::init<>())
        .def("set", &PyConfig::set<std::string>, "Set configuration value", 
              py::arg("key"), py::arg("value"))
        .def("get", &PyConfig::get<std::string>, "Get configuration value",
              py::arg("key"), py::arg("default_value"))
        .def("has", &PyConfig::has, "Check if configuration key exists", py::arg("key"))
        .def("use_research_settings", &PyConfig::use_research_settings, 
             "Use research-grade simulation settings")
        .def("use_high_performance_settings", &PyConfig::use_high_performance_settings,
             "Use high-performance simulation settings")
        .def("use_quick_test_settings", &PyConfig::use_quick_test_settings,
             "Use quick test simulation settings")
        .def("to_dict", &PyConfig::to_dict, "Convert config to Python dictionary");
    
    // Expose enhanced brain class
    py::class_<PyBrain>(m, "Brain", R"pbdoc(Improved brain interface with Pythonic enhancements)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("step_batch", &PyBrain::step_batch, 
             "Run multiple simulation steps with optional progress output",
             py::arg("steps"), py::arg("verbose") = false)
        .def("get_summary", &PyBrain::get_summary,
             "Get brain statistics as a Python dictionary")
        .def("get_neural_activity_plot_data", &PyBrain::get_neural_activity_plot_data,
             "Get neural activity data for visualization")
        .def("configure_standard", &PyBrain::configure_standard,
             "Configure brain with standard settings", 
             py::arg("neuron_count") = 1000, py::arg("region_count") = 1)
        .def("develop_to_adult", &PyBrain::develop_to_adult,
             "Fast-forward development to adult stage")
        .def("enable_all_plasticity", &PyBrain::enable_all_plasticity,
             "Enable all forms of plasticity");
    
    // Expose enhanced agent brain
    py::class_<PyAgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("process_vision", &PyAgentBrain::process_vision,
             "Process vision input from numpy array", 
             py::arg("vision"), py::arg("width") = 16, py::arg("height") = 16)
        .def("run_learning_experiment", &PyAgentBrain::run_learning_experiment,
             "Run a learning experiment and return results", py::arg("steps") = 1000)
        .def("get_summary", &PyAgentBrain::get_summary,
             "Get agent brain summary as Python dictionary")
        .def("enable_all_systems", &PyAgentBrain::enable_all_systems,
             "Enable all neuromodulation and development systems");
    
    // Expose enhanced world
    py::class_<PyWorld>(m, "World", R"pbdoc(Improved world interface)")
        .def(py::init<>())
        .def("setup_town", &PyWorld::setup_town,
             "Set up a structured test world", py::arg("size") = 32)
        .def("get_visualization_data", &PyWorld::get_visualization_data,
             "Get world data for visualization");
    
    // Factory functions
    m.def("create_config", []() { return PyConfig(); }, 
          "Create a new configuration object");
    
    m.def("create_world", []() { return PyWorld(); },
          "Create a new world object");
    
    // Legacy compatibility (original bindings)
    py::class_<Brain>(m, "BrainLegacy", R"pbdoc(Legacy brain interface for backward compatibility)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize)
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"))
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"))
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"))
        .def("produceAction", &Brain::produceAction);
    
    // Convenience functions
    m.def("create_simple_experiment", []() {
        py::dict experiment;
        experiment["description"] = "Simple learning experiment";
        experiment["steps"] = 1000;
        experiment["pattern_size"] = 256;
        return experiment;
    }, "Create a simple experiment configuration");
    
    // Performance monitoring
    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "enhanced-v1.0";
        info["features"] = std::vector<std::string>{
            "numpy_integration",
            "pythonic_api",
            "enhanced_config",
            "learning_helpers",
            "visualization_support"
        };
        return info;
    }, "Get system information and available features");
    
    // Error classes
    py::class_<py_helpers::NLMException>(m, "NLMException", 
        py::module_local(), py::base<std::runtime_error>())
        .def(py::init<const std::string&>());
    
    py::class_<py_helpers::ConfigurationError>(m, "ConfigurationError",
        py::module_local(), py::base<py_helpers::NLMException>())
        .def(py::init<const std::string&>());
    
    py::class_<py_helpers::SimulationError>(m, "SimulationError",
        py::module_local(), py::base<py_helpers::NLMException>())
        .def(py::init<const std::string&>());
    
    // Mathematical constants and utilities
    m.attr("PI") = 3.141592653589793f;
    m.attr("E") = 2.718281828459045f;
    
    m.def("clamp", [](float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }, "Clamp value between min and max", py::arg("value"), py::arg("min"), py::arg("max"));
    
    m.def("normalize", [](float value, float min, float max) {
        if (max == min) return 0.5f;
        return (value - min) / (max - min);
    }, "Normalize value to [0, 1] range", py::arg("value"), py::arg("min"), py::arg("max"));
    
    // Performance utilities
    m.def("benchmark", [](std::function<void()> func, const std::string& description) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start).count();
        
        py::dict result;
        result["description"] = description;
        result["duration"] = duration;
        result["seconds"] = duration;
        result["milliseconds"] = duration * 1000.0;
        result["microseconds"] = duration * 1000000.0;
        
        return result;
    }, "Benchmark function execution time", py::arg("func"), py::arg("description"));
}

} // namespace nlm
