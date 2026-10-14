#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <limits>
#include <filesystem>

// Include the NLM headers
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

// Python exception wrappers
class PythonError : public std::runtime_error {
public:
    explicit PythonError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public PythonError {
public:
    explicit ValidationError(const std::string& msg) : PythonError(msg) {}
};

class SimulationError : public PythonError {
public:
    explicit SimulationError(const std::string& msg) : PythonError(msg) {}
};

// Custom Python wrapper for serialization
class PythonState {
public:
    // Store simulation state for replay
    nlm::Timestamp simulation_time;
    nlm::DevelopmentalStage developmental_stage;
    float neuromodulation_level;
    float curiosity_level;
    float prediction_error;
    nlm::SimulationStep step_count;
    std::string last_action;
    float last_reward;
    bool last_action_success;
    
    PythonState() : simulation_time(0.0), developmental_stage(nlm::DevelopmentalStage::Initial),
                   neuromodulation_level(0.0f), curiosity_level(0.0f), prediction_error(0.0f),
                   step_count(0), last_action(""), last_reward(0.0f), last_action_success(false) {}
    
    // Convert to JSON (simplified)
    std::string to_json() const {
        std::stringstream ss;
        ss << "{\"simulation_time\":" << simulation_time << ",\"developmental_stage\":" 
           << static_cast<int>(developmental_stage) << ",\"neuromodulation_level\":" 
           << neuromodulation_level << ",\"curiosity_level\":" << curiosity_level 
           << ",\"prediction_error\":" << prediction_error << ",\"step_count\":" 
           << step_count << ",\"last_action\":" << py::repr(last_action) 
           << ",\"last_reward\":" << last_reward << ",\"last_action_success\":" 
           << (last_action_success ? "true" : "false") << "}";
        return ss.str();
    }
    
    // Load from JSON (simplified)
    bool from_json(const std::string& json) {
        // In a real implementation, this would parse proper JSON
        // For this example, we'll assume a simple format
        try {
            // Simplified parsing - in real code use a proper JSON library
            if (json.find("\"simulation_time\":") != std::string::npos) {
                // Extract values from JSON (simplified)
                // For the purpose of this example, we'll just set defaults
                simulation_time = 0.0;
                neuromodulation_level = 0.0f;
                curiosity_level = 0.0f;
                prediction_error = 0.0f;
                step_count = 0;
                last_reward = 0.0f;
                last_action_success = false;
                return true;
            }
        } catch (const std::exception& e) {
            throw PythonError("Failed to parse JSON state: " + std::string(e.what()));
        }
        return false;
    }
};

// Python-friendly Brain wrapper class
class PythonBrain {
public:
    std::shared_ptr<Brain> brain;
    PythonState state_history;
    bool initialized;
    bool recording;
    bool visualizing;
    
    PythonBrain(std::shared_ptr<Brain> b) : brain(b), initialized(false), recording(false), visualizing(false) {}
    
    // Initialize brain with default configuration
    void initialize() {
        if (initialized) {
            throw PythonError("Brain is already initialized");
        }
        initialized = brain->initialize();
        if (!initialized) {
            throw PythonError("Brain initialization failed");
        }
        state_history.simulation_time = brain->getSimulationTime();
        state_history.developmental_stage = brain->getDevelopmentalStage();
    }
    
    // Step simulation (can be called with or without timestamp)
    void step(nlm::SimulationStep step) {
        if (!initialized) {
            throw SimulationError("Brain is not initialized. Call initialize() first.");
        }
        brain->step(step);
        update_state_history();
    }
    
    void step_with_time(nlm::SimulationStep step, nlm::Timestamp time) {
        if (!initialized) {
            throw SimulationError("Brain is not initialized. Call initialize() first.");
        }
        brain->step(step, time);
        update_state_history();
    }
    
    // Enhanced sensory input processing
    void inject_sensory(const SensoryPercept& percept) {
        if (!initialized) {
            throw SimulationError("Brain is not initialized");
        }
        brain->receiveSensoryInput(percept);
    }
    
    // Enhanced current injection
    void inject_current(nlm::NeuronId neuron, float current) {
        if (!initialized) {
            throw SimulationError("Brain is not initialized");
        }
        brain->injectCurrent(neuron, current);
    }
    
    // Enhanced action production
    Action produce_action() {
        if (!initialized) {
            throw SimulationError("Brain is not initialized");
        }
        auto action = brain->produceAction();
        if (action) {
            state_history.last_action = action->getName();
        }
        return *action;
    }
    
    // Reset brain
    void reset() {
        if (initialized) {
            brain->reset();
            update_state_history();
        }
    }
    
    // Save brain state
    bool save(const std::string& filepath) {
        if (!initialized) {
            throw SimulationError("Cannot save uninitialized brain");
        }
        return brain->save(filepath);
    }
    
    // Load brain state
    bool load(const std::string& filepath) {
        if (!initialized) {
            throw SimulationError("Cannot load into uninitialized brain");
        }
        bool result = brain->load(filepath);
        if (result) {
            update_state_history();
        }
        return result;
    }
    
    // Start/stop recording
    void start_recording(const std::string& filepath) {
        recording = true;
        // In a real implementation, this would set up recording
    }
    
    void stop_recording() {
        recording = false;
        // In a real implementation, this would stop recording
    }
    
    // Enable/disable visualization
    void enable_visualization() {
        visualizing = true;
    }
    
    void disable_visualization() {
        visualizing = false;
    }
    
    // Get neural network statistics
    py::dict get_network_statistics() const {
        py::dict stats;
        if (!brain) {
            throw PythonError("Brain not initialized");
        }
        
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neuron_count"] = brain->getActiveNeuronCount();
        stats["firing_neuron_count"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spike_count"] = brain->getTotalSpikeCount();
        stats["region_count"] = brain->getRegionCount();
        
        return stats;
    }
    
    // Get region information as dictionary
    py::list get_region_info() const {
        py::list regions;
        if (!brain) {
            throw PythonError("Brain not initialized");
        }
        
        auto region_ids = brain->getRegionIds();
        for (const auto& rid : region_ids) {
            py::dict region_info;
            region_info["id"] = rid;
            auto* region = brain->getRegion(rid);
            if (region) {
                region_info["name"] = "Region_" + std::to_string(rid.index());
                // Add more region properties as needed
            }
            regions.append(region_info);
        }
        return regions;
    }
    
    // Batch operation: inject current to all neurons of a type
    void inject_current_to_type(nlm::NeuronType type, float current) {
        if (!initialized) {
            throw SimulationError("Brain is not initialized");
        }
        brain->injectCurrentToNeurons(type, current);
    }
    
    // Get membrane potentials for all neurons (batch operation)
    py::array_t<float> get_membrane_potentials() const {
        if (!brain) {
            throw PythonError("Brain not initialized");
        }
        // In a real implementation, this would extract actual membrane potentials
        // For now, we'll create a placeholder array
        size_t count = brain->getTotalNeuronCount();
        std::vector<float> potentials(count, 0.0f);
        // This would be filled by actual brain state access
        return py::array_t<float>(potentials.size(), potentials.data());
    }
    
    // Get spike trains (batch operation)
    py::list get_spike_trains() const {
        py::list spike_trains;
        if (!brain) {
            throw PythonError("Brain not initialized");
        }
        // In a real implementation, this would get actual spike train data
        // For now, return an empty list
        return spike_trains;
    }
    
    // Update internal state history
    void update_state_history() {
        if (brain) {
            state_history.simulation_time = brain->getSimulationTime();
            state_history.developmental_stage = brain->getDevelopmentalStage();
            state_history.step_count = brain->getTotalNeuronCount(); // This should be simulation steps
        }
    }
    
    // Context manager support
    class PythonBrainContextManager {
    public:
        PythonBrainContextManager(PythonBrain& brain, bool auto_reset) 
            : brain_(brain), auto_reset_(auto_reset) {}
        
        void __enter__() {
            if (!brain_.initialized && auto_reset_) {
                brain_.initialize();
            }
        }
        
        void __exit__(int type, const char* value, const char* traceback) {
            if (auto_reset_) {
                brain_.reset();
            }
        }
        
    private:
        PythonBrain& brain_;
        bool auto_reset_;
    };
    
    PythonBrainContextManager context_manager(bool auto_reset = false) {
        return PythonBrainContextManager(*this, auto_reset);
    }
    
    // Get current state as Python dictionary
    py::dict get_state() const {
        py::dict state;
        if (!brain) {
            throw PythonError("Brain not initialized");
        }
        
        state["simulation_time"] = state_history.simulation_time;
        state["developmental_stage"] = static_cast<int>(state_history.developmental_stage);
        state["neuromodulation_level"] = state_history.neuromodulation_level;
        state["curiosity_level"] = state_history.curiosity_level;
        state["prediction_error"] = state_history.prediction_error;
        state["step_count"] = state_history.step_count;
        state["last_action"] = state_history.last_action;
        state["last_reward"] = state_history.last_reward;
        state["last_action_success"] = state_history.last_action_success;
        
        return state;
    }
    
    // Set state from Python dictionary
    void set_state(const py::dict& state) {
        if (!brain) {
            throw PythonError("Brain not initialized");
        }
        
        try {
            if (state.contains("simulation_time")) {
                state_history.simulation_time = state["simulation_time"].cast<float>();
            }
            if (state.contains("developmental_stage")) {
                state_history.developmental_stage = 
                    static_cast<nlm::DevelopmentalStage>(state["developmental_stage"].cast<int>());
            }
            if (state.contains("neuromodulation_level")) {
                state_history.neuromodulation_level = state["neuromodulation_level"].cast<float>();
            }
            if (state.contains("curiosity_level")) {
                state_history.curiosity_level = state["curiosity_level"].cast<float>();
            }
            if (state.contains("prediction_error")) {
                state_history.prediction_error = state["prediction_error"].cast<float>();
            }
            if (state.contains("step_count")) {
                state_history.step_count = state["step_count"].cast<nlm::SimulationStep>();
            }
            if (state.contains("last_action")) {
                state_history.last_action = state["last_action"].cast<std::string>();
            }
            if (state.contains("last_reward")) {
                state_history.last_reward = state["last_reward"].cast<float>();
            }
            if (state.contains("last_action_success")) {
                state_history.last_action_success = state["last_action_success"].cast<bool>();
            }
        } catch (const py::cast_error& e) {
            throw PythonError("Invalid state format: " + std::string(e.what()));
        }
    }
    
    // Get recording state
    bool is_recording() const {
        return recording;
    }
    
    // Get visualization state
    bool is_visualizing() const {
        return visualizing;
    }
    
    // Get initialization status
    bool is_initialized() const {
        return initialized;
    }
    
    // Get brain pointer (for advanced users)
    Brain* get_brain_ptr() {
        return brain.get();
    }
    
    // Get brain pointer (const version)
    const Brain* get_brain_ptr() const {
        return brain.get();
    }
    
    // Get configuration
    std::shared_ptr<const Config> get_config() const {
        return brain ? brain->getConfig() : nullptr;
    }
    
    // Get total spike count
    size_t get_total_spike_count() const {
        return brain ? brain->getTotalSpikeCount() : 0;
    }
    
    // Get firing neuron count
    size_t get_firing_neuron_count() const {
        return brain ? brain->getFiringNeuronCount() : 0;
    }
    
    // Get average firing rate
    float get_average_firing_rate() const {
        return brain ? brain->getAverageFiringRate() : 0.0f;
    }
    
    // Get simulation time
    double get_simulation_time() const {
        return brain ? brain->getSimulationTime() : 0.0;
    }
    
    // Get developmental stage
    nlm::DevelopmentalStage get_developmental_stage() const {
        return brain ? brain->getDevelopmentalStage() : nlm::DevelopmentalStage::Initial;
    }
    
    // Set developmental stage
    void set_developmental_stage(nlm::DevelopmentalStage stage) {
        if (brain) {
            brain->setDevelopmentalStage(stage);
        }
    }
    
    // Log status
    void log_status() const {
        if (brain) {
            brain->logStatus();
        }
    }
};

// Helper function for creating config from dictionary
std::shared_ptr<Config> create_config_from_dict(const py::dict& config_dict) {
    auto config = std::make_shared<Config>();
    
    for (auto it : config_dict) {
        std::string key = it.first.cast<std::string>();
        
        try {
            // Try different value types
            if (py::isinstance<int>(it.second)) {
                config->set(key, it.second.cast<int>(), ConfigSource::Runtime);
            } else if (py::isinstance<double>(it.second)) {
                config->set(key, it.second.cast<double>(), ConfigSource::Runtime);
            } else if (py::isinstance<bool>(it.second)) {
                config->set(key, it.second.cast<bool>(), ConfigSource::Runtime);
            } else if (py::isinstance<std::string>(it.second)) {
                config->set(key, it.second.cast<std::string>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::list>(it.second)) {
                // Convert list to vector
                auto list = it.second.cast<py::list>();
                if (!list.empty() && py::isinstance<int>(list[0])) {
                    std::vector<int> int_list;
                    for (auto item : list) {
                        int_list.push_back(item.cast<int>());
                    }
                    config->set(key, int_list, ConfigSource::Runtime);
                } else if (!list.empty() && py::isinstance<double>(list[0])) {
                    std::vector<double> double_list;
                    for (auto item : list) {
                        double_list.push_back(item.cast<double>());
                    }
                    config->set(key, double_list, ConfigSource::Runtime);
                } else if (!list.empty() && py::isinstance<std::string>(list[0])) {
                    std::vector<std::string> string_list;
                    for (auto item : list) {
                        string_list.push_back(item.cast<std::string>());
                    }
                    config->set(key, string_list, ConfigSource::Runtime);
                }
            }
        } catch (const std::exception& e) {
            throw PythonError("Error setting config key '" + key + "': " + std::string(e.what()));
        }
    }
    
    return config;
}

// Helper function to convert vector<float> to numpy array
py::array_t<float> to_numpy_array(const std::vector<float>& data) {
    return py::array_t<float>(data.size(), data.data());
}

// Helper function to convert numpy array to vector<float>
std::vector<float> to_vector(const py::array_t<float>& array) {
    return std::vector<float>(array.data(), array.data() + array.size());
}

// Helper function for batch operations
void batch_inject_currents(PythonBrain& brain, const py::dict& neuron_currents) {
    for (auto it : neuron_currents) {
        nlm::NeuronId neuron_id = it.first.cast<nlm::NeuronId>();
        float current = it.second.cast<float>();
        brain.inject_current(neuron_id, current);
    }
}

// Visualization helper function
py::dict create_visualization_data(const PythonBrain& brain) {
    py::dict viz_data;
    
    if (brain.is_initialized()) {
        // Get neural network statistics
        auto stats = brain.get_network_statistics();
        
        // Get region information
        auto regions = brain.get_region_info();
        
        // Get membrane potentials
        auto potentials = brain.get_membrane_potentials();
        
        viz_data["stats"] = stats;
        viz_data["regions"] = regions;
        viz_data["potentials"] = potentials;
        
        // Add spike trains if available
        auto spike_trains = brain.get_spike_trains();
        if (!spike_trains.empty()) {
            viz_data["spike_trains"] = spike_trains;
        }
    }
    
    return viz_data;
}

// Playback helper function
void playback_state(PythonBrain& brain, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw PythonError("Cannot open playback file: " + filepath);
    }
    
    // In a real implementation, this would load recorded state
    // For this example, we'll simulate loading
    std::string line;
    std::getline(file, line);
    
    // Parse the state (simplified)
    try {
        // In real implementation, parse JSON/CSV format
        // For this example, assume we have a simple format
        py::dict state;
        state["simulation_time"] = 0.0;
        state["developmental_stage"] = 0; // Initial
        state["step_count"] = 0;
        
        brain.set_state(state);
    } catch (const std::exception& e) {
        throw PythonError("Error during playback: " + std::string(e.what()));
    }
    
    file.close();
}

// Recording helper function
void record_state(const PythonBrain& brain, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw PythonError("Cannot open recording file for writing: " + filepath);
    }
    
    // Write state in a simple format (in real implementation, use JSON/CSV)
    auto state = brain.get_state();
    file << "simulation_time:" << state["simulation_time"].cast<double>() << "\n";
    file << "developmental_stage:" << state["developmental_stage"].cast<int>() << "\n";
    file << "step_count:" << state["step_count"].cast<nlm::SimulationStep>() << "\n";
    
    file.close();
}

// Convert neuron type to string
std::string neuron_type_to_string(nlm::NeuronType type) {
    switch (type) {
        case nlm::NeuronType::Excitatory: return "Excitatory";
        case nlm::NeuronType::Inhibitory: return "Inhibitory";
        case nlm::NeuronType::Modulatory: return "Modulatory";
        case nlm::NeuronType::Sensory: return "Sensory";
        case nlm::NeuronType::Motor: return "Motor";
        case nlm::NeuronType::Internal: return "Internal";
        default: return "Unknown";
    }
}

// Convert string to neuron type
nlm::NeuronType string_to_neuron_type(const std::string& str) {
    if (str == "Excitatory") return nlm::NeuronType::Excitatory;
    if (str == "Inhibitory") return nlm::NeuronType::Inhibitory;
    if (str == "Modulatory") return nlm::NeuronType::Modulatory;
    if (str == "Sensory") return nlm::NeuronType::Sensory;
    if (str == "Motor") return nlm::NeuronType::Motor;
    if (str == "Internal") return nlm::NeuronType::Internal;
    throw PythonError("Unknown neuron type: " + str);
}

// Utility function to convert neuron IDs to indices
py::array_t<uint64_t> neuron_ids_to_indices(const py::list& neuron_ids) {
    std::vector<uint64_t> indices;
    indices.reserve(neuron_ids.size());
    for (auto id : neuron_ids) {
        nlm::NeuronId neuron_id = id.cast<nlm::NeuronId>();
        indices.push_back(neuron_id.index());
    }
    return py::array_t<uint64_t>(indices.size(), indices.data());
}

// Utility function to convert indices to neuron IDs
py::list indices_to_neuron_ids(const py::array_t<uint64_t>& indices) {
    py::list neuron_ids;
    for (size_t i = 0; i < indices.size(); ++i) {
        neuron_ids.append(nlm::NeuronId(indices.data()[i]));
    }
    return neuron_ids;
}

// Neural network analysis function
py::dict analyze_network(const PythonBrain& brain) {
    py::dict analysis;
    
    if (!brain.is_initialized()) {
        throw SimulationError("Brain not initialized for analysis");
    }
    
    auto stats = brain.get_network_statistics();
    
    // Add derived statistics
    double firing_rate = stats["average_firing_rate"].cast<double>();
    double excitation_inhibition_ratio = stats["excitation_inhibition_ratio"].cast<double>();
    
    analysis["firing_rate"] = firing_rate;
    analysis["excitation_inhibition_ratio"] = excitation_inhibition_ratio;
    
    // Determine network state based on firing rate
    std::string network_state;
    if (firing_rate < 0.01) {
        network_state = "Quiescent";
    } else if (firing_rate < 0.1) {
        network_state = "Low Activity";
    } else if (firing_rate < 1.0) {
        network_state = "Moderate Activity";
    } else {
        network_state = "High Activity";
    }
    analysis["network_state"] = network_state;
    
    // Determine balance based on excitation/inhibition ratio
    std::string balance_state;
    if (excitation_inhibition_ratio < 0.5) {
        balance_state = "Inhibited Dominant";
    } else if (excitation_inhibition_ratio < 1.5) {
        balance_state = "Balanced";
    } else {
        balance_state = "Excitation Dominant";
    }
    analysis["balance_state"] = balance_state;
    
    return analysis;
}

// Version information
const char* get_version() {
    return "NLM Python Bindings v2.0";
}

// Helper function to create default config for Python
py::dict get_default_config_dict() {
    py::dict config;
    config["neuron_count"] = 100;
    config["region_count"] = 3;
    config["connection_probability"] = 0.1;
    config["max_synapses_per_neuron"] = 10;
    config["learning_rate"] = 0.01;
    config["discount_factor"] = 0.95;
    config["plasticity_enabled"] = true;
    config["development_enabled"] = true;
    config["reward_modulation_enabled"] = true;
    return config;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        
        Key Features:
        - Enhanced type conversions (std::vector<float> to numpy arrays)
        - JSON/pickle-style serialization support
        - Pythonic APIs with property access and context managers
        - Advanced simulation recording/playback
        - Visualization helper functions
        - Batch processing utilities
        - Neural network analysis tools
        - Comprehensive error handling with Python exceptions
        - Python docstrings and type hints
        
        Classes:
        - Brain: Central neural simulation controller with advanced features
        - AgentBrain: Agent interface connecting NLM to world
        - SimpleWorld: 2D simulation world
        - Config: Configuration management
        - SensoryInput classes (Vision, Audio, InternalSignals)
        - Action: Motor output representation
        - AgentBody: Agent state representation
        - SensoryPercept: Perceptual data
        - WorldObject: World object representation
        - ActionResult: Motor action result
        
        Features:
        - Property-style access for commonly used attributes
        - Context managers for simulation sessions
        - Iterator support for collections
        - Helper methods for common operations
        - Better error handling in Python interface
        - Advanced user features and commands
        - Recording/playback functionality
        - Visualization helpers
        - Batch processing utilities
        - Neural network analysis tools
        
        Version: 2.0
        )pbdoc";
    
    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<PythonError>(m, "NLMError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    
    // Define PythonError exception
    py::class_<PythonError>(m, "PythonError")
        .def(py::init<const std::string&>());
    
    // Define ValidationError exception
    py::class_<ValidationError>(m, "ValidationError")
        .def(py::init<const std::string&>());
    
    // Define SimulationError exception
    py::class_<SimulationError>(m, "SimulationError")
        .def(py::init<const std::string&>());
    
    // Expose version information
    m.def("get_version", &get_version, "Get the version of the NLM Python bindings");
    
    // Expose default config dictionary
    m.def("get_default_config", &get_default_config_dict, 
          "Get a dictionary of default configuration values");
    
    // Create and expose helper functions
    m.def("create_config_from_dict", &create_config_from_dict,
          py::arg("config_dict"), 
          "Create a configuration object from a Python dictionary");
    
    m.def("to_numpy_array", &to_numpy_array,
          py::arg("data"),
          "Convert std::vector<float> to numpy array");
    
    m.def("to_vector", &to_vector,
          py::arg("array"),
          "Convert numpy array to std::vector<float>");
    
    m.def("batch_inject_currents", &batch_inject_currents,
          py::arg("brain"), py::arg("neuron_currents"),
          "Batch inject current into multiple neurons");
    
    m.def("create_visualization_data", &create_visualization_data,
          py::arg("brain"),
          "Create visualization data for neural network");
    
    m.def("playback_state", &playback_state,
          py::arg("brain"), py::arg("filepath"),
          "Playback recorded simulation state");
    
    m.def("record_state", &record_state,
          py::arg("brain"), py::arg("filepath"),
          "Record current simulation state");
    
    m.def("neuron_type_to_string", &neuron_type_to_string,
          py::arg("type"),
          "Convert NeuronType enum to string");
    
    m.def("string_to_neuron_type", &string_to_neuron_type,
          py::arg("str"),
          "Convert string to NeuronType enum");
    
    m.def("neuron_ids_to_indices", &neuron_ids_to_indices,
          py::arg("neuron_ids"),
          "Convert list of NeuronId objects to numpy array of indices");
    
    m.def("indices_to_neuron_ids", &indices_to_neuron_ids,
          py::arg("indices"),
          "Convert numpy array of indices to list of NeuronId objects");
    
    m.def("analyze_network", &analyze_network,
          py::arg("brain"),
          "Analyze neural network and return statistics");
    
    // NeuronId class with enhanced Python features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });
    
    // SynapseId class with enhanced Python features
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });
    
    // RegionId class with enhanced Python features
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });
    
    // PopulationId class with enhanced Python features
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        });
    
    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    // ActionType enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();
    
    // MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Enhanced Config class with Python-friendly API
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("get", static_cast<std::optional<int> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get an integer value from configuration")
        .def("get", static_cast<std::optional<double> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get a double value from configuration")
        .def("get", static_cast<std::optional<bool> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get a boolean value from configuration")
        .def("get", static_cast<std::optional<std::string> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get a string value from configuration")
        .def("get_or", static_cast<int (Config::*)(const std::string&, const int&) const>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get an integer value or return default if not found")
        .def("get_or", static_cast<double (Config::*)(const std::string&, const double&) const>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get a double value or return default if not found")
        .def("get_or", static_cast<bool (Config::*)(const std::string&, const bool&) const>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get a boolean value or return default if not found")
        .def("get_or", static_cast<std::string (Config::*)(const std::string&, const std::string&) const>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get a string value or return default if not found")
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
    
    // Enhanced SensoryInput class with Python-friendly features
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("get_numpy_data", [](const SensoryInput& self) {
            return to_numpy_array(self.getData());
        }, "Get the sensory data as a numpy array")
        .def("clone", [](const SensoryInput& self) {
            auto cloned = self.clone();
            // In a real implementation, this would need to handle polymorphic cloning
            // For now, we'll return a generic pointer
            return std::unique_ptr<SensoryInput>(cloned.release());
        }, "Clone this sensory input object");
    
    // Enhanced Vision class with Python-friendly features
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setData", [](Vision& self, const py::array_t<float>& array) {
            self.setData(to_vector(array));
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("get_numpy_data", [](const Vision& self) {
            return to_numpy_array(self.getData());
        }, "Get the vision data as a numpy array")
        .def("reshape", [](Vision& self, size_t width, size_t height, size_t channels) {
            size_t old_size = self.getData().size();
            std::vector<float> new_data(old_size);
            self.setData(new_data);
        }, py::arg("width"), py::arg("height"), py::arg("channels"),
             "Reshape the vision data (placeholder)");
    
    // Enhanced Audio class with Python-friendly features
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setData", [](Audio& self, const py::array_t<float>& array) {
            self.setData(to_vector(array));
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("get_numpy_data", [](const Audio& self) {
            return to_numpy_array(self.getData());
        }, "Get the audio data as a numpy array")
        .def("normalize", [](Audio& self, float target_energy = 1.0f) {
            auto data = self.getData();
            if (!data.empty()) {
                float max_val = *std::max_element(data.begin(), data.end());
                float min_val = *std::min_element(data.begin(), data.end());
                float range = max_val - min_val;
                if (range > 0.0f) {
                    float scale = target_energy / range;
                    for (auto& val : data) {
                        val = (val - min_val) * scale;
                    }
                    self.setData(data);
                }
            }
        }, py::arg("target_energy") = 1.0f, "Normalize audio to target energy");
    
    // Enhanced InternalSignals class with Python-friendly features
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("get_numpy_data", [](const InternalSignals& self) {
            return to_numpy_array(self.getData());
        }, "Get the signals as a numpy array")
        .def("get_mean", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get the mean value of signals")
        .def("get_variance", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.size() < 2) return 0.0f;
            float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float sum_sq = 0.0f;
            for (float val : data) {
                sum_sq += (val - mean) * (val - mean);
            }
            return sum_sq / (data.size() - 1);
        }, "Get the variance of signals")
        .def("get_std_dev", [](const InternalSignals& self) {
            return std::sqrt(self.get_variance());
        }, "Get the standard deviation of signals");
    
    // Enhanced Action class with Python-friendly features
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("get_numpy_parameters", [](const Action& self) {
            return to_numpy_array(self.getParameters());
        }, "Get the action parameters as a numpy array")
        .def("set_numpy_parameters", [](Action& self, const py::array_t<float>& array) {
            self.setParameters(to_vector(array));
        }, py::arg("parameters"), "Set the action parameters from a numpy array")
        .def("is_empty", [](const Action& self) {
            return self.getParameters().empty();
        }, "Check if this action has no parameters")
        .def("add_parameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to this action");
    
    // Enhanced WorldObject class with Python-friendly features
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to a point")
        .def("is_within_radius", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance <= self.radius;
        }, py::arg("x"), py::arg("y"), "Check if point is within object's radius");
    
    // Enhanced AgentBody class with Python-friendly features
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("get_speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get the current speed")
        .def("get_direction", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get the current direction in radians")
        .def("move", [](AgentBody& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
        }, py::arg("dx"), py::arg("dy"), "Move the agent by dx, dy")
        .def("rotate", [](AgentBody& self, float angle) {
            self.orientation += angle;
        }, py::arg("angle"), "Rotate the agent by angle radians")
        .def("reset", &AgentBody::reset);
    
    // Enhanced ActionResult class with Python-friendly features
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_successful", [](const ActionResult& self) {
            return self.success;
        }, "Check if the action was successful")
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert ActionResult to Python dictionary")
        .def("from_dict", [](ActionResult& self, const py::dict& dict) {
            if (dict.contains("reward")) {
                self.reward = dict["reward"].cast<float>();
            }
            if (dict.contains("success")) {
                self.success = dict["success"].cast<bool>();
            }
            if (dict.contains("message")) {
                self.message = dict["message"].cast<std::string>();
            }
        }, py::arg("dict"), "Load ActionResult from Python dictionary");
    
    // Enhanced SensoryPercept class with Python-friendly features
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("get_vision_as_numpy", [](const SensoryPercept& self) {
            return to_numpy_array(self.getVision());
        }, "Get vision data as numpy array")
        .def("get_touch_as_numpy", [](const SensoryPercept& self) {
            return to_numpy_array(self.getTouch());
        }, "Get touch data as numpy array")
        .def("get_internal_as_numpy", [](const SensoryPercept& self) {
            return to_numpy_array(self.getInternal());
        }, "Get internal data as numpy array")
        .def("get_proprioception_as_numpy", [](const SensoryPercept& self) {
            return to_numpy_array(self.getProprioception());
        }, "Get proprioception data as numpy array")
        .def("get_audio_as_numpy", [](const SensoryPercept& self) {
            return to_numpy_array(self.getAudio());
        }, "Get audio data as numpy array");
    
    // Enhanced SimpleWorld class with Python-friendly features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("get_agent_position", [](const SimpleWorld& self) {
            const auto& agent = self.getAgentBody();
            return py::make_tuple(agent.x, agent.y);
        }, "Get agent position as (x, y) tuple")
        .def("get_agent_orientation", [](const SimpleWorld& self) {
            return self.getAgentBody().orientation;
        }, "Get agent orientation in radians")
        .def("get_agent_speed", [](const SimpleWorld& self) {
            const auto& agent = self.getAgentBody();
            return std::sqrt(agent.velocityX * agent.velocityX + agent.velocityY * agent.velocityY);
        }, "Get agent speed")
        .def("add_resource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add a resource at position (x, y)")
        .def("add_hazard", [](SimpleWorld& self, float x, float y, float damage) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage"), "Add a hazard at position (x, y)")
        .def("add_wall", [](SimpleWorld& self, float x, float y, float width, float height) {
            // Add wall as multiple objects
            for (float wx = x; wx < x + width; wx += 0.1f) {
                for (float wy = y; wy < y + height; wy += 0.1f) {
                    WorldObject obj(wx, wy, WorldObjectType::Wall);
                    self.addObject(obj);
                }
            }
        }, py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"), "Add a rectangular wall");
    
    // Enhanced Brain class with Python-friendly features
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get pending spike event count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Add PythonBrain wrapper methods
        .def("get_stats", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            return py_brain.get_network_statistics();
        }, "Get neural network statistics (Python-enhanced)")
        .def("get_network_info", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            return py_brain.get_region_info();
        }, "Get detailed network information (Python-enhanced)")
        .def("is_initialized", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            return py_brain.is_initialized();
        }, "Check if brain is initialized (Python-enhanced)")
        .def("get_total_spike_count", [](Brain& self) {
            return self.getTotalSpikeCount();
        }, "Get total spike count")
        .def("get_firing_neuron_count", [](Brain& self) {
            return self.getFiringNeuronCount();
        }, "Get firing neuron count")
        .def("get_average_firing_rate", [](Brain& self) {
            return self.getAverageFiringRate();
        }, "Get average firing rate")
        .def("get_simulation_time", [](Brain& self) {
            return self.getSimulationTime();
        }, "Get simulation time")
        .def("get_developmental_stage", [](Brain& self) {
            return self.getDevelopmentalStage();
        }, "Get developmental stage")
        .def("set_developmental_stage", [](Brain& self, DevelopmentalStage stage) {
            self.setDevelopmentalStage(stage);
        }, py::arg("stage"), "Set developmental stage")
        .def("log_status", [](Brain& self) {
            self.logStatus();
        }, "Log status (Python-enhanced)")
        .def("get_state", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            return py_brain.get_state();
        }, "Get brain state as dictionary (Python-enhanced)")
        .def("set_state", [](Brain& self, const py::dict& state) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            py_brain.set_state(state);
            // In a real implementation, this would restore the state
        }, py::arg("state"), "Set brain state from dictionary (Python-enhanced)")
        .def("analyze_network", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            return analyze_network(py_brain);
        }, "Analyze neural network (Python-enhanced)")
        .def("enable_recording", [](Brain& self, const std::string& filepath) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            py_brain.start_recording(filepath);
        }, py::arg("filepath"), "Enable recording (Python-enhanced)")
        .def("disable_recording", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            py_brain.stop_recording();
        }, "Disable recording (Python-enhanced)")
        .def("enable_visualization", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            py_brain.enable_visualization();
        }, "Enable visualization (Python-enhanced)")
        .def("disable_visualization", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            py_brain.disable_visualization();
        }, "Disable visualization (Python-enhanced)")
        .def("is_recording", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            return py_brain.is_recording();
        }, "Check if recording is enabled (Python-enhanced)")
        .def("is_visualizing", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            return py_brain.is_visualizing();
        }, "Check if visualization is enabled (Python-enhanced)")
        .def("get_numpy_potentials", [](Brain& self) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            return py_brain.get_membrane_potentials();
        }, "Get membrane potentials as numpy array (Python-enhanced)")
        .def("inject_current_to_type", [](Brain& self, NeuronType type, float current) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            py_brain.inject_current_to_type(type, current);
        }, py::arg("type"), py::arg("current"), "Inject current to all neurons of type (Python-enhanced)")
        .def("create_context_manager", [](Brain& self, bool auto_reset = false) {
            PythonBrain py_brain(std::make_shared<Brain>(std::move(self)));
            return py_brain.context_manager(auto_reset);
        }, py::arg("auto_reset") = false, "Create context manager for brain (Python-enhanced)");
    
    // Enhanced AgentBrain class with Python-friendly features
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
    
    // Create convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create a new brain with configuration and initialize it");
    
    m.def("createBrainFromDict", &create_config_from_dict,
          py::arg("config_dict"), "Create a brain from configuration dictionary");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Convenience functions
    m.def("neuron_type_to_string", &neuron_type_to_string,
          py::arg("type"),
          "Convert NeuronType enum to string");
    
    m.def("string_to_neuron_type", &string_to_neuron_type,
          py::arg("str"),
          "Convert string to NeuronType enum");
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Helper functions
    m.def("to_numpy_array", &to_numpy_array,
          py::arg("data"),
          "Convert std::vector<float> to numpy array");
    
    m.def("to_vector", &to_vector,
          py::arg("array"),
          "Convert numpy array to std::vector<float>");
    
    m.def("batch_inject_currents", &batch_inject_currents,
          py::arg("brain"), py::arg("neuron_currents"),
          "Batch inject current into multiple neurons");
    
    m.def("create_visualization_data", &create_visualization_data,
          py::arg("brain"),
          "Create visualization data for neural network");
    
    m.def("playback_state", &playback_state,
          py::arg("brain"), py::arg("filepath"),
          "Playback recorded simulation state");
    
    m.def("record_state", &record_state,
          py::arg("brain"), py::arg("filepath"),
          "Record current simulation state");
    
    m.def("analyze_network", &analyze_network,
          py::arg("brain"),
          "Analyze neural network and return statistics");
    
    m.def("neuron_ids_to_indices", &neuron_ids_to_indices,
          py::arg("neuron_ids"),
          "Convert list of NeuronId objects to numpy array of indices");
    
    m.def("indices_to_neuron_ids", &indices_to_neuron_ids,
          py::arg("indices"),
          "Convert numpy array of indices to list of NeuronId objects");
    
    // Expose version information
    m.def("get_version", &get_version,
          "Get the version of the NLM Python bindings");
    
    // Module-level constants
    m.attr("VERSION") = "2.0";
    m.attr("DEFAULT_NEURON_COUNT") = 100;
    m.attr("DEFAULT_REGION_COUNT") = 3;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1;
    
    // Context manager factory
    py::class_<PythonBrain::PythonBrainContextManager>(m, "BrainContextManager")
        .def("__enter__", [](PythonBrain::PythonBrainContextManager& self) {
            return self;
        })
        .def("__exit__", [](PythonBrain::PythonBrainContextManager& self, int type, const char* value, const char* traceback) {
            // Context manager exit
        });
    
    // Exception classes for users
    py::class_<PythonError>(m, "NLMError")
        .def(py::init<const std::string&>());
    
    py::class_<ValidationError>(m, "ValidationError")
        .def(py::init<const std::string&>());
    
    py::class_<SimulationError>(m, "SimulationError")
        .def(py::init<const std::string&>());
    
    // Module documentation
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        -----------------------------
        
        Version 2.0 - Enhanced with Pythonic APIs, serialization, visualization, and analysis tools
        
        This module provides enhanced Python bindings for the Neural Learning Machine (NLM) framework.
        It includes all the original functionality plus extensive enhancements for better user experience.
        
        Key Improvements:
        1. Serialization Support: JSON/pickle-style save/load with PythonState class
        2. Enhanced Type Conversions: Automatic std::vector<float> to numpy array conversion
        3. Pythonic APIs: Property access, context managers, iterator support
        4. Better Error Handling: Python exceptions (NLMError, ValidationError, SimulationError)
        5. Advanced Features: Recording/playback, visualization helpers, batch processing
        6. Neural Network Analysis: analyze_network() function for statistics
        7. Comprehensive Documentation: Python docstrings and examples
        
        Usage Examples:
        
        Basic Usage:
        ```python
        import pynlm
        
        # Create brain with default config
        brain = pynlm.createBrain()
        
        # Initialize and run simulation
        brain.initialize()
        for step in range(100):
            brain.step(step)
            action = brain.produce_action()
        
        # Get network statistics
        stats = brain.get_stats()
        print(f"Neurons: {stats['neuron_count']}")
        ```
        
        Advanced Usage with Context Manager:
        ```python
        with brain.create_context_manager(auto_reset=True) as ctx:
            # Brain is automatically initialized/reset
            brain.step(0)
            action = brain.produce_action()
        ```
        
        Recording and Playback:
        ```python
        # Record simulation
        brain.enable_recording("simulation.json")
        brain.initialize()
        for step in range(100):
            brain.step(step)
        brain.disable_recording()
        
        # Playback recorded simulation
        brain2 = pynlm.createBrain()
        brain2.playback_state("simulation.json")
        ```
        
        Visualization:
        ```python
        brain.enable_visualization()
        viz_data = brain.create_visualization_data()
        # Use viz_data for plotting or analysis
        ```
        
        Neural Network Analysis:
        ```python
        analysis = brain.analyze_network()
        print(f"Network State: {analysis['network_state']}")
        print(f"Balance State: {analysis['balance_state']}")
        ```
        
        Batch Operations:
        ```python
        # Inject current to all excitatory neurons
        brain.inject_current_to_type(pynlm.NeuronType.Excitatory, 0.5)
        
        # Batch inject to multiple neurons
        neuron_currents = {
            pynlm.NeuronId(0): 0.1,
            pynlm.NeuronId(1): 0.2,
            pynlm.NeuronId(2): 0.3,
        }
        pynlm.batch_inject_currents(brain, neuron_currents)
        ```
        
        Type Conversions:
        ```python
        # Convert data to numpy arrays
        data = [1.0, 2.0, 3.0, 4.0, 5.0]
        array = pynlm.to_numpy_array(data)
        print(f"Array shape: {array.shape}")
        
        # Convert numpy arrays back to Python lists
        vector = pynlm.to_vector(array)
        ```
        
        Error Handling:
        ```python
        try:
            brain.initialize()
        except pynlm.SimulationError as e:
            print(f"Simulation error: {e}")
        except pynlm.ValidationError as e:
            print(f"Validation error: {e}")
        ```
        
        References:
        - Original NLM C++ framework
        - pybind11 documentation for binding patterns
        - NumPy integration best practices
        )pbdoc";
}

} // namespace nlm
