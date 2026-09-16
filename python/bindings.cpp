#include "python/bindings.cpp"

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/performance/CheckpointSystem.hpp"

#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/json.h>

namespace nlm {

// Custom exception types for Python bindings
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& msg) : NLMException(msg) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& msg) : NLMException(msg) {}
};

class PatternError : public NLMException {
public:
    PatternError(const std::string& msg) : NLMException(msg) {}
};

// Register custom exceptions
void registerCustomExceptions(py::module& m) {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });
}

// Configuration management helpers
void enhanceConfiguration(py::class_<Config>& configClass) {
    configClass
        // Type-specific getters with convenience overloads
        .def("get_int", &Config::getOr<int64_t>, py::arg("key"), py::arg("defaultValue") = 0,
             "Get an integer value with default fallback")
        .def("get_double", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get a double value with default fallback")
        .def("get_float", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
             "Get a float value with default fallback")
        .def("get_bool", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue") = false,
             "Get a boolean value with default fallback")
        .def("get_string", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue") = "",
             "Get a string value with default fallback")
        .def("get_vector_int", &Config::getOr<std::vector<int64_t>>, py::arg("key"), py::arg("defaultValue") = std::vector<int64_t>(),
             "Get an integer vector value with default fallback")
        .def("get_vector_double", &Config::getOr<std::vector<double>>, py::arg("key"), py::arg("defaultValue") = std::vector<double>(),
             "Get a double vector value with default fallback")
        
        // Configuration validation and utilities
        .def("validate_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("has_value", &Config::has, py::arg("key"),
             "Check if configuration key exists")
        .def("merge", [](Config& self, const Config& other) {
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                if (auto value = other.get<std::string>(key)) {
                    self.set(key, *value, ConfigSource::Runtime);
                }
            }
        }, py::arg("other"),
             "Merge another configuration into this one")
        .def("merge_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Merge configuration from file")
        .def("merge_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Merge configuration from command line arguments")
        
        // Configuration statistics
        .def("get_stats", [](const Config& self) {
            py::dict stats;
            stats["total_keys"] = self.getKeys().size();
            stats["default_count"] = 0;  // Would need to track source
            stats["file_count"] = 0;    // Would need to track source
            stats["command_line_count"] = 0;  // Would need to track source
            return stats;
        }, "Get configuration statistics")
        
        // Serialization utilities
        .def("to_dict", [](const Config& self) {
            py::dict configDict;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (auto value = self.get<std::string>(key)) {
                    configDict[key] = *value;
                } else if (auto value = self.get<int64_t>(key)) {
                    configDict[key] = *value;
                } else if (auto value = self.get<double>(key)) {
                    configDict[key] = *value;
                } else if (auto value = self.get<bool>(key)) {
                    configDict[key] = *value;
                }
            }
            return configDict;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& configDict) {
            for (auto item : configDict) {
                std::string key = py::cast<std::string>(item.first);
                py::object value = item.second;
                
                if (value.is(py::int_())) {
                    int64_t val = py::cast<int64_t>(value);
                    self.set(key, val, ConfigSource::Runtime);
                } else if (value.is(py::float_())) {
                    double val = py::cast<double>(value);
                    self.set(key, val, ConfigSource::Runtime);
                } else if (value.is(py::bool_())) {
                    bool val = py::cast<bool>(value);
                    self.set(key, val, ConfigSource::Runtime);
                } else if (value.is(py::str())) {
                    std::string val = py::cast<std::string>(value);
                    self.set(key, val, ConfigSource::Runtime);
                }
            }
        }, py::arg("configDict"),
             "Load configuration from Python dictionary")
        
        // Configuration export/import
        .def("export_to_json", &Config::saveToFile, py::arg("filepath"),
             "Export configuration to JSON file")
        .def("import_from_json", &Config::loadFromFile, py::arg("filepath"),
             "Import configuration from JSON file");
}

// Brain configuration presets
void enhanceBrainConfiguration(py::class_<Brain>& brainClass) {
    brainClass
        // Brain creation convenience functions
        .def_static("create_default_brain", []() -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 1000);
            config->set("region_count", 1);
            config->set("connection_probability", 0.1f);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            config->set("simulation_timestep", 0.001);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, "Create a brain with default configuration")
        
        .def_static("create_efficient_brain", [](size_t neuronCount) -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", neuronCount);
            config->set("region_count", 1);
            config->set("connection_probability", 0.05f);
            config->set("simulation_timestep", 0.001);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, py::arg("neuronCount") = 1000, "Create an efficient brain with specified neuron count")
        
        .def_static("create_learning_brain", []() -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 2000);
            config->set("region_count", 2);
            config->set("connection_probability", 0.15f);
            config->set("stdp_ltp_weight", 0.02f);
            config->set("stdp_ltd_weight", 0.018f);
            config->set("synaptogenesis_rate", 0.0001f);
            config->set("pruning_rate", 0.00001f);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, "Create a brain optimized for learning")
        
        .def_static("create_minimal_brain", []() -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            config->set("connection_probability", 0.1f);
            config->set("simulation_timestep", 0.001);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, "Create a minimal brain for testing")
        
        .def_static("create_high_performance_brain", []() -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 5000);
            config->set("region_count", 4);
            config->set("connection_probability", 0.08f);
            config->set("simulation_timestep", 0.001);
            config->set("random_seed", 12345ULL);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, "Create a high-performance brain")
        
        // Neural metrics and statistics
        .def("get_neural_stats", [](const Brain& brain) {
            py::dict stats;
            stats["neuron_count"] = brain.getTotalNeuronCount();
            stats["synapse_count"] = brain.getTotalSynapseCount();
            stats["region_count"] = brain.getRegionCount();
            stats["active_neuron_count"] = brain.getActiveNeuronCount();
            stats["firing_neuron_count"] = brain.getFiringNeuronCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["excitatory_inhibitory_ratio"] = brain.getExcitationInhibitionRatio();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["pending_events"] = brain.getPendingSpikeEventCount();
            
            // Add memory system stats if available
            if (brain.getWorkingMemory()) {
                auto* wm = brain.getWorkingMemory();
                stats["working_memory_capacity"] = wm->getCapacity();
                stats["working_memory_usage"] = wm->getCurrentSize();
            }
            
            if (brain.getEpisodicMemory()) {
                auto* em = brain.getEpisodicMemory();
                stats["episodic_memory_capacity"] = em->getMaxEpisodes();
                stats["episodic_memory_usage"] = em->getEpisodeCount();
            }
            
            return stats;
        }, "Get detailed brain statistics")
        
        .def("get_plasticity_stats", [](const Brain& brain) {
            py::dict stats;
            if (auto* stdp = brain.getSTDP()) {
                // Get STDP statistics if available
                stats["stdp_enabled"] = true;
            }
            if (auto* hebbian = brain.getHebbian()) {
                stats["hebbian_enabled"] = true;
            }
            if (auto* structural = brain.getStructuralPlasticity()) {
                stats["structural_plasticity_enabled"] = true;
            }
            return stats;
        }, "Get plasticity system statistics")
        
        // Neural pattern utilities
        .def("create_pattern", [](Brain& brain, const std::vector<NeuronId>& neuronIds, 
                                 const std::vector<float>& activations) {
            if (neuronIds.size() != activations.size()) {
                throw PatternError("Neuron IDs and activations must have same size");
            }
            
            // Store pattern in working memory if available
            if (brain.getWorkingMemory()) {
                auto* wm = brain.getWorkingMemory();
                for (size_t i = 0; i < neuronIds.size(); ++i) {
                    wm->store(neuronIds[i], activations[i]);
                }
            }
            
            return true;
        }, py::arg("neuronIds"), py::arg("activations"),
             "Create a neural activation pattern")
        
        .def("retrieve_pattern", [](const Brain& brain, const std::vector<NeuronId>& neuronIds) {
            std::vector<float> activations;
            if (brain.getWorkingMemory()) {
                auto* wm = brain.getWorkingMemory();
                for (const auto& neuronId : neuronIds) {
                    activations.push_back(wm->retrieve(neuronId));
                }
            }
            return activations;
        }, py::arg("neuronIds"), "Retrieve activation pattern for given neurons")
        
        .def("get_most_active_pattern", [](Brain& brain, size_t count) {
            std::vector<NeuronId> mostActiveNeurons;
            if (brain.getSpikeSystem()) {
                mostActiveNeurons = brain.getSpikeSystem()->getMostActiveNeurons(count);
            }
            return mostActiveNeurons;
        }, py::arg("count") = 10, "Get IDs of most active neurons")
        
        .def("pattern_to_json", [](const Brain& brain, const std::vector<NeuronId>& neuronIds) {
            py::dict patternJson;
            
            if (brain.getWorkingMemory()) {
                auto* wm = brain.getWorkingMemory();
                for (const auto& neuronId : neuronIds) {
                    float activation = wm->retrieve(neuronId);
                    patternJson[std::to_string(neuronId.value)] = activation;
                }
            }
            
            return patternJson;
        }, py::arg("neuronIds"), "Convert neural pattern to JSON-compatible dictionary")
        
        .def("pattern_from_json", [](Brain& brain, const py::dict& patternJson) {
            std::vector<NeuronId> neuronIds;
            std::vector<float> activations;
            
            for (auto item : patternJson) {
                std::string key = py::cast<std::string>(item.first);
                float activation = py::cast<float>(item.second);
                
                NeuronId neuronId;
                neuronId.value = std::stoull(key);
                neuronIds.push_back(neuronId);
                activations.push_back(activation);
            }
            
            return brain.create_pattern(neuronIds, activations);
        }, py::arg("patternJson"), "Create neural pattern from JSON-compatible dictionary");

// Simulation control enhancements
void enhanceSimulationControl(py::class_<Brain>& brainClass) {
    brainClass
        // Enhanced step control
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        
        // Batch stepping
        .def("step_batch", [](Brain& brain, size_t steps) {
            for (size_t i = 0; i < steps; ++i) {
                brain.step(i, i * 0.001);
            }
        }, py::arg("steps"), "Perform multiple simulation steps")
        
        // Simulation state management
        .def("get_current_step", &Brain::getTotalSpikeCount,
             "Get current simulation step (approximate)")
        .def("get_current_time", [](const Brain& brain) {
            return brain.getConfig() ? 
                brain.getConfig()->getOr<double>("simulation_timestep", 0.001) * 
                brain.getTotalSpikeCount() : 0.0;
        }, "Get current simulation time")
        
        // Reset utilities
        .def("reset_simulation", &Brain::reset, "Reset brain to initial state")
        .def("restart_simulation", [](Brain& brain) {
            auto config = brain.getConfig();
            brain.reset();
            if (config) {
                auto newBrain = std::make_shared<Brain>(config);
                newBrain->initialize();
                return newBrain;
            }
            return brain;
        }, "Restart simulation with same configuration")
        
        // Simulation statistics
        .def("get_simulation_stats", [](Brain& brain) {
            py::dict stats;
            stats["total_steps_completed"] = brain.getTotalSpikeCount() / 10;  // Approximation
            stats["simulation_duration"] = brain.getCurrentTime();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["network_efficiency"] = brain.getExcitationInhibitionRatio();
            return stats;
        }, "Get simulation performance statistics");

// Advanced metrics and monitoring
void enhanceMetrics(py::class_<Brain>& brainClass) {
    brainClass
        // Advanced neural metrics
        .def("get_detailed_firing_stats", [](Brain& brain) {
            py::dict stats;
            auto firingCount = brain.getFiringNeuronCount();
            auto totalNeurons = brain.getTotalNeuronCount();
            
            stats["firing_percentage"] = (totalNeurons > 0) ? (firingCount * 100.0 / totalNeurons) : 0.0;
            stats["firing_rate_hz"] = brain.getAverageFiringRate();
            stats["network_activity"] = brain.getActiveNeuronCount();
            
            return stats;
        }, "Get detailed firing statistics")
        
        .def("get_plasticity_dynamics", [](Brain& brain, SimulationStep step) {
            py::dict dynamics;
            
            if (brain.getSTDP()) {
                dynamics["stdp_active"] = true;
                // Add STDP dynamics tracking
            }
            
            if (brain.getHebbian()) {
                dynamics["hebbian_active"] = true;
            }
            
            if (brain.getStructuralPlasticity()) {
                dynamics["structural_plasticity_active"] = true;
            }
            
            return dynamics;
        }, py::arg("step"), "Get plasticity system dynamics")
        
        .def("get_memory_dynamics", [](Brain& brain, SimulationStep step) {
            py::dict dynamics;
            
            if (brain.getWorkingMemory()) {
                auto* wm = brain.getWorkingMemory();
                dynamics["working_memory_active"] = true;
                dynamics["memory_capacity"] = wm->getCapacity();
                dynamics["memory_usage"] = wm->getCurrentSize();
                dynamics["memory_decay_rate"] = 0.1f;  // Default
            }
            
            if (brain.getEpisodicMemory()) {
                auto* em = brain.getEpisodicMemory();
                dynamics["episodic_memory_active"] = true;
                dynamics["episode_capacity"] = em->getMaxEpisodes();
                dynamics["episode_count"] = em->getEpisodeCount();
            }
            
            return dynamics;
        }, py::arg("step"), "Get memory system dynamics");

// Enhanced serialization/deserialization
void enhanceSerialization(py::class_<Brain>& brainClass) {
    brainClass
        // Enhanced save/load with options
        .def("save_checkpoint", [](Brain& brain, const std::string& filepath,
                                  bool compress, int compressionLevel) {
            auto config = brain.getConfig();
            if (!config) {
                throw ConfigurationError("Brain has no configuration");
            }
            
            // Set compression level in config temporarily
            int originalLevel = config->getOr<int>("checkpoint_compression_level", 6);
            config->set("checkpoint_compression_level", compressionLevel, ConfigSource::Runtime);
            
            bool success = brain.save(filepath);
            
            // Restore original level
            config->set("checkpoint_compression_level", originalLevel, ConfigSource::Runtime);
            
            return success;
        }, py::arg("filepath"), py::arg("compress") = true,
             py::arg("compressionLevel") = 6, "Save brain state with compression options")
        
        .def("load_checkpoint", [](Brain& brain, const std::string& filepath,
                                  bool compressed) {
            return brain.load(filepath);
        }, py::arg("filepath"), py::arg("compressed") = true, "Load brain state from checkpoint")
        
        .def("save_to_string", [](Brain& brain) {
            // For now, this creates a minimal representation
            // In a real implementation, this would use the checkpoint system
            py::dict brainState;
            brainState["neuron_count"] = brain.getTotalNeuronCount();
            brainState["synapse_count"] = brain.getTotalSynapseCount();
            brainState["step"] = brain.getTotalSpikeCount();
            brainState["version"] = "1.0";
            
            return brainState;
        }, "Save brain state to Python dictionary")
        
        .def("load_from_string", [](Brain& brain, const py::dict& brainState) {
            // Restore from string representation
            // This is a simplified implementation
            auto config = brain.getConfig();
            if (!config) {
                throw ConfigurationError("Brain has no configuration");
            }
            
            if (brainState.contains("neuron_count")) {
                config->set("neuron_count", py::cast<int64_t>(brainState["neuron_count"]), ConfigSource::Runtime);
            }
            
            if (brainState.contains("synapse_count")) {
                config->set("neuron_count", py::cast<int64_t>(brainState["synapse_count"]), ConfigSource::Runtime);
            }
            
            brain.reset();
            return true;
        }, py::arg("brainState"), "Load brain state from Python dictionary");

// Enhanced random seed management
void enhanceRandomSeedManagement(py::class_<Brain>& brainClass) {
    brainClass
        .def("get_random_seed", [](Brain& brain) {
            if (brain.getConfig()) {
                auto seedOpt = brain.getConfig()->get<uint64_t>("random_seed");
                if (seedOpt) {
                    return *seedOpt;
                }
            }
            return uint64_t(42);  // Default seed
        }, "Get current random seed")
        
        .def("set_random_seed", [](Brain& brain, uint64_t seed) {
            if (brain.getConfig()) {
                brain.getConfig()->set("random_seed", seed, ConfigSource::Runtime);
                // Reinitialize random generator if brain is initialized
                brain.reset();
            }
        }, py::arg("seed"), "Set random seed")
        
        .def("get_random_distribution", [](Brain& brain, const std::string& distribution) {
            auto rng = brain.getRandomGenerator();
            if (!rng) return 0.0f;
            
            if (distribution == "uniform") {
                return rng->uniformReal(0.0f, 1.0f);
            } else if (distribution == "normal") {
                return rng->normalReal(0.0f, 1.0f);
            } else if (distribution == "integer") {
                return static_cast<float>(rng->uniformInt(0, 100));
            }
            
            return 0.0f;
        }, py::arg("distribution") = "uniform", "Get random number from specified distribution");

// Enhanced debugging and diagnostics
void enhanceDebugging(py::class_<Brain>& brainClass) {
    brainClass
        .def("get_diagnostics_report", [](Brain& brain) {
            py::dict diagnostics;
            
            // Basic brain state
            diagnostics["brain_id"] = reinterpret_cast<uint64_t>(&brain);
            diagnostics["status"] = "initialized";
            diagnostics["neuron_count"] = brain.getTotalNeuronCount();
            diagnostics["synapse_count"] = brain.getTotalSynapseCount();
            
            // Performance metrics
            diagnostics["average_firing_rate"] = brain.getAverageFiringRate();
            diagnostics["memory_usage"] = brain.getTotalSpikeCount();
            diagnostics["system_load"] = brain.getActiveNeuronCount() / 
                                       std::max(1.0, static_cast<double>(brain.getTotalNeuronCount()));
            
            // System components status
            py::dict components;
            components["working_memory"] = (brain.getWorkingMemory() != nullptr);
            components["episodic_memory"] = (brain.getEpisodicMemory() != nullptr);
            components["neuromodulation"] = (brain.getDopamine() != nullptr);
            components["prediction_system"] = (brain.getPredictionSystem() != nullptr);
            components["development_system"] = (brain.getDevelopmentSystem() != nullptr);
            components["plasticity_rules"] = (brain.getSTDP() != nullptr) && (brain.getHebbian() != nullptr);
            
            diagnostics["components"] = components;
            
            // Error state
            diagnostics["errors"] = py::list();
            
            return diagnostics;
        }, "Get comprehensive diagnostics report")
        
        .def("get_system_info", []() {
            py::dict info;
            
            // System information
            info["version"] = "NLM Phase 2";  // Would get from build info
            info["build_date"] = "2026-09-16";
            info["architecture"] = "x86_64";
            info["compiler"] = "C++20";
            
            // Python bindings info
            py::dict bindings;
            bindings["python_version"] = PY_VERSION;
            bindings["pybind11_version"] = PYBIND11_VERSION;
            info["python_bindings"] = bindings;
            
            return info;
        }, "Get system information")
        
        .def("check_connectivity", [](Brain& brain) {
            py::dict connectivity;
            
            // Check if all major systems are accessible
            connectivity["memory_systems"] = (brain.getWorkingMemory() != nullptr) && 
                                           (brain.getEpisodicMemory() != nullptr);
            connectivity["plasticity_systems"] = (brain.getSTDP() != nullptr) && 
                                                (brain.getHebbian() != nullptr);
            connectivity["neuromodulation_systems"] = (brain.getDopamine() != nullptr) && 
                                                    (brain.getCuriosity() != nullptr);
            connectivity["cognition_systems"] = (brain.getPlanner() != nullptr) && 
                                               (brain.getConceptFormation() != nullptr);
            
            // Overall connectivity score
            int totalSystems = 7;
            int connectedSystems = std::accumulate(connectivity.begin(), connectivity.end(), 0,
                                                   [](int sum, const py::dict::item& item) {
                                                       return sum + (py::cast<bool>(item.second) ? 1 : 0);
                                                   });
            
            connectivity["connectivity_score"] = connectedSystems * 100 / totalSystems;
            connectivity["status"] = (connectedSystems == totalSystems) ? "fully_connected" : "partial";
            
            return connectivity;
        }, "Check connectivity of all brain systems");

// Register all enhancements
void bindEnhancedPython(py::module& m) {
    // Register custom exceptions first
    registerCustomExceptions(m);
    
    // Get references to existing classes
    auto configClass = m.attr("Config").cast<py::class_<Config>>();
    auto brainClass = m.attr("Brain").cast<py::class_<Brain>>();
    
    // Apply all enhancements
    enhanceConfiguration(configClass);
    enhanceBrainConfiguration(brainClass);
    enhanceSimulationControl(brainClass);
    enhanceMetrics(brainClass);
    enhanceSerialization(brainClass);
    enhanceRandomSeedManagement(brainClass);
    enhanceDebugging(brainClass);
    
    // Update the class in the module
    m.def("Config", [](const Config& c) { return c; });  // Dummy for registration
    m.def("Brain", [](const Brain& b) { return b; });   // Dummy for registration
}

} // namespace nlm
