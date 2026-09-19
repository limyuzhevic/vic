#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Errors/Errors.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions - inherit from NLMException hierarchy
class NLMRuntimeError : public NLMException {
public:
    explicit NLMRuntimeError(const std::string& msg) : NLMException(msg, "Runtime") {}
    NLMRuntimeError(const std::string& msg, const std::string& file, int line, const std::string& function)
        : NLMException(msg, file, line, function) {}
};

class NLMConfigurationError : public NLMConfigurationException {
public:
    explicit NLMConfigurationError(const std::string& msg) : NLMConfigurationException(msg) {}
    NLMConfigurationError(const std::string& msg, const std::string& file, int line, const std::string& function)
        : NLMConfigurationException(msg, file, line, function) {}
};

class NLMValueError : public NLMValueError {
public:
    explicit NLMValueError(const std::string& msg) : NLMValueError(msg) {}
    NLMValueError(const std::string& msg, const std::string& file, int line, const std::string& function)
        : NLMValueError(msg, file, line, function) {}
};

class NLMNotInitializedError : public NLMStateException {
public:
    explicit NLMNotInitializedError(const std::string& msg) : NLMStateException(msg) {}
    NLMNotInitializedError(const std::string& msg, const std::string& file, int line, const std::string& function)
        : NLMStateException(msg, file, line, function) {}
};

class NLMMemoryError : public NLMMemoryException {
public:
    explicit NLMMemoryError(const std::string& msg) : NLMMemoryException(msg) {}
    NLMMemoryError(const std::string& msg, const std::string& file, int line, const std::string& function)
        : NLMMemoryException(msg, file, line, function) {}
};

// Configuration presets
struct ConfigPresets {
    static void apply_quick_start(std::shared_ptr<Config> config) {
        // High-speed learning with basic settings
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42);
        config->set("neural_region_count", 4);
        config->set("neurons_per_region", 100);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("reward_modulation_enabled", true);
        config->set("structural_plasticity_enabled", true);
        config->set("development_enabled", true);
        config->set("curiosity_enabled", true);
    }
    
    static void apply_research_mode(std::shared_ptr<Config> config) {
        // Detailed research configuration with extended parameters
        apply_quick_start(config);
        config->set("replay_interval", 500);
        config->set("consolidation_interval", 5000);
        config->set("novelty_threshold", 0.5f);
        config->set("curiosity_weight", 0.3f);
        config->set("prediction_error_weight", 0.7f);
        config->set("max_energy", 100.0f);
        config->set("energy_decay_rate", 0.95f);
        config->set("developmental_stage", static_cast<int>(DevelopmentalStage::CriticalPeriod));
    }
    
    static void apply_development_mode(std::shared_ptr<Config> config) {
        // Focus on developmental processes
        apply_research_mode(config);
        config->set("synaptogenesis_rate", 0.001f);
        config->set("pruning_rate", 0.0005f);
        config->set("developmental_stage", static_cast<int>(DevelopmentalStage::Maturation));
        config->set("plasticity_modifier", 2.0f);
        config->set("developmental_age_max", 100.0);
    }
    
    static void apply_production_mode(std::shared_ptr<Config> config) {
        // Optimized for stable, reliable operation
        apply_quick_start(config);
        config->set("replay_interval", 1000);
        config->set("consolidation_interval", 10000);
        config->set("novelty_threshold", 0.8f);
        config->set("energy_decay_rate", 0.99f);
        config->set("max_energy", 1000.0f);
        config->set("developmental_stage", static_cast<int>(DevelopmentalStage::Adult));
        config->set("stability_factor", 0.9f);
    }
};

// Memory management utilities
class MemoryManager {
public:
    static void optimizeMemory(std::shared_ptr<Brain> brain) {
        // Enable memory optimizations if available
        if (brain) {
            // Enable memory pools if available
            // brain->enableMemoryPools();
        }
    }
    
    static size_t getMemoryUsage(std::shared_ptr<Brain> brain) {
        // Return approximate memory usage in bytes
        // This is a placeholder - actual implementation would need 
        // access to internal memory management
        return 0;
    }
    
    static void cleanup(std::shared_ptr<Brain> brain) {
        // Force cleanup of temporary objects
        if (brain) {
            brain->reset();
        }
    }
};

// Visualization helpers
class VisualizationHelper {
public:
    static py::dict get_brain_statistics(std::shared_ptr<Brain> brain) {
        py::dict stats;
        if (!brain) {
            throw NLMNotInitializedError("Brain not initialized");
        }
        
        stats["total_neurons"] = static_cast<size_t>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<size_t>(brain->getTotalSynapseCount());
        stats["active_neurons"] = static_cast<size_t>(brain->getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<size_t>(brain->getFiringNeuronCount());
        stats["total_spikes"] = static_cast<size_t>(brain->getTotalSpikeCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        return stats;
    }
    
    static py::list get_region_info(std::shared_ptr<Brain> brain) {
        py::list regions;
        if (!brain) {
            throw NLMNotInitializedError("Brain not initialized");
        }
        
        auto regionIds = brain->getRegionIds();
        for (const auto& id : regionIds) {
            py::dict region_info;
            region_info["id"] = id.value;
            region_info["index"] = id.index();
            regions.append(region_info);
        }
        
        return regions;
    }
    
    static std::string format_statistics(py::dict stats) {
        std::stringstream ss;
        ss << "NLM Brain Statistics:" << std::endl;
        ss << "===================" << std::endl;
        
        if (stats.contains("total_neurons")) {
            ss << "Neurons: " << stats["total_neurons"].cast<size_t>() << std::endl;
        }
        if (stats.contains("total_synapses")) {
            ss << "Synapses: " << stats["total_synapses"].cast<size_t>() << std::endl;
        }
        if (stats.contains("active_neurons")) {
            ss << "Active Neurons: " << stats["active_neurons"].cast<size_t>() << std::endl;
        }
        if (stats.contains("firing_neurons")) {
            ss << "Firing Neurons: " << stats["firing_neurons"].cast<size_t>() << std::endl;
        }
        if (stats.contains("total_spikes")) {
            ss << "Total Spikes: " << stats["total_spikes"].cast<size_t>() << std::endl;
        }
        if (stats.contains("average_firing_rate")) {
            ss << "Average Firing Rate: " << stats["average_firing_rate"].cast<float>() << " Hz" << std::endl;
        }
        if (stats.contains("excitation_inhibition_ratio")) {
            ss << "E/I Ratio: " << stats["excitation_inhibition_ratio"].cast<float>() << std::endl;
        }
        
        return ss.str();
    }
};

// Context manager for automatic cleanup
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void __enter__() {
        return;
    }
    
    void __exit__(py::args) {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> get_brain() {
        return brain_;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Simulation utilities
class SimulationHelper {
public:
    static void run_simulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t steps,
        float timestep,
        bool verbose = false
    ) {
        if (!brain || !world || !agent) {
            throw NLMNotInitializedError("All components must be initialized");
        }
        
        // Reset all components
        brain->reset();
        world->reset();
        agent->reset();
        
        if (verbose) {
            // Log simulation start
        }
        
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory percept
            const auto& percept = world->getSensoryPercept();
            
            // Process through agent
            agent->processSensoryInput(percept);
            
            // Brain simulation step
            brain->step(static_cast<SimulationStep>(step), world->getSimulationTime());
            
            // Decode motor command
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply action in world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                // Log progress
            }
        }
        
        if (verbose) {
            // Log simulation completion
        }
    }
    
    static py::dict run_episodes(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t episodes,
        size_t steps_per_episode,
        float timestep,
        bool verbose = false
    ) {
        py::dict results;
        py::list episode_rewards;
        py::list episode_lengths;
        py::list final_spike_counts;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            run_simulation(brain, world, agent, steps_per_episode, timestep, verbose);
            
            // Collect episode statistics
            episode_rewards.append(0.0f); // Placeholder - would track actual reward
            episode_lengths.append(static_cast<size_t>(steps_per_episode));
            final_spike_counts.append(static_cast<size_t>(brain->getTotalSpikeCount()));
            
            if (verbose) {
                // Log episode completion
            }
        }
        
        results["episode_rewards"] = episode_rewards;
        results["episode_lengths"] = episode_lengths;
        results["final_spike_counts"] = final_spike_counts;
        results["mean_reward"] = 0.0f; // Calculate actual mean
        results["mean_episode_length"] = static_cast<size_t>(steps_per_episode);
        
        return results;
    }
};

// Export/import utilities
class SerializationHelper {
public:
    static bool save_brain_state(
        std::shared_ptr<Brain> brain,
        const std::string& filepath,
        bool compress = false
    ) {
        if (!brain) {
            throw NLMNotInitializedError("Brain not initialized");
        }
        return brain->save(filepath);
    }
    
    static bool load_brain_state(
        std::shared_ptr<Brain> brain,
        const std::string& filepath
    ) {
        if (!brain) {
            throw NLMNotInitializedError("Brain not initialized");
        }
        return brain->load(filepath);
    }
    
    static std::string export_config(std::shared_ptr<Config> config) {
        if (!config) {
            throw NLMConfigurationError("Config not initialized");
        }
        return config->summary();
    }
    
    static bool import_config(std::shared_ptr<Config> config, const std::string& json_str) {
        if (!config) {
            throw NLMConfigurationError("Config not initialized");
        }
        // TODO: Implement proper JSON parsing
        return false;
    }
};

// Exception handling utilities
class ExceptionHelper {
public:
    static std::string getExceptionInfo(const NLMException& e) {
        std::stringstream ss;
        ss << "Exception Type: " << e.getMessage() << std::endl;
        ss << "Context: " << e.getContext() << std::endl;
        ss << "File: " << e.getFile() << std::endl;
        ss << "Line: " << e.getLine() << std::endl;
        ss << "Full message: " << e.getFullMessage() << std::endl;
        return ss.str();
    }
    
    static py::list getRecentErrors(size_t limit = 100) {
        py::list errors;
        auto recent = ErrorReportingUtils::getRecentErrors(limit);
        for (const auto& error : recent) {
            py::dict error_dict;
            error_dict["type"] = error.errorType;
            error_dict["message"] = error.message;
            error_dict["file"] = error.file;
            error_dict["line"] = error.line;
            error_dict["function"] = error.function;
            errors.append(error_dict);
        }
        return errors;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action with enhanced Pythonic API.
        
        Key Features:
        - Pythonic snake_case naming (not CamelCase)
        - Rich type hints and documentation
        - Configuration presets for quick setup
        - Context managers for resource management
        - Enhanced visualization and analysis tools
        - Memory management utilities
        - Simulation utilities for common patterns
        - Export/import capabilities
        - Comprehensive error handling with proper exception translation
        
        Example:
        ```python
        import pynlm
        
        # Create a brain with research configuration
        config = pynlm.create_config()
        pynlm.apply_research_preset(config)
        
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Use context manager for automatic cleanup
        with pynlm.brain_context(brain) as ctx:
            for step in range(100):
                brain.step(step)
        
        # Get statistics
        stats = pynlm.get_brain_stats(brain)
        print(f"Total neurons: {stats['total_neurons']}")
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMRuntimeError");
    py::register_exception<NLMConfigurationException>(m, "NLMConfigurationError");
    py::register_exception<NLMValueError>(m, "NLMValueError");
    py::register_exception<NLMStateException>(m, "NLMNotInitializedError");
    py::register_exception<NLMMemoryException>(m, "NLMMemoryError");
    py::register_exception<NLMRessourceException>(m, "NLMResourceError");
    py::register_exception<NLMOperationException>(m, "NLMOperationError");
    
    // ---------------------------------------------------------------------
    // ID Types (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ---------------------------------------------------------------------
    // Enums (Pythonic naming)
    // ---------------------------------------------------------------------
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ---------------------------------------------------------------------
    // Configuration (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has_key", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Convenience function for creating default config
    m.def("create_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Configuration presets
    m.def("apply_quick_start_preset", &ConfigPresets::apply_quick_start,
          py::arg("config"), "Apply quick start configuration preset");
    
    m.def("apply_research_preset", &ConfigPresets::apply_research_mode,
          py::arg("config"), "Apply research configuration preset");
    
    m.def("apply_development_preset", &ConfigPresets::apply_development_mode,
          py::arg("config"), "Apply development configuration preset");
    
    m.def("apply_production_preset", &ConfigPresets::apply_production_mode,
          py::arg("config"), "Apply production configuration preset");

    // ---------------------------------------------------------------------
    // Sensory Inputs (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals);

    // ---------------------------------------------------------------------
    // Actions (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("action_type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("action_type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone);

    // ---------------------------------------------------------------------
    // World Objects (Pythonic naming)
    // ---------------------------------------------------------------------
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
        .def_readwrite("active", &WorldObject::active);

    // ---------------------------------------------------------------------
    // Agent Body (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    // ---------------------------------------------------------------------
    // Action Results (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ---------------------------------------------------------------------
    // Sensory Percept (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // ---------------------------------------------------------------------
    // Simple World (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("command"), py::arg("current_time"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("energy"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("rate"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed);

    // Convenience function for creating simple world
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // ---------------------------------------------------------------------
    // Brain (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron_id"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("neuron_type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status");

    // Convenience function for creating brain
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // ---------------------------------------------------------------------
    // Agent Brain (Pythonic naming)
    // ---------------------------------------------------------------------
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled);

    // Convenience function for creating agent brain
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ---------------------------------------------------------------------
    // Invalid ID constants (Pythonic naming)
    // ---------------------------------------------------------------------
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ---------------------------------------------------------------------
    // Enhanced Helper Functions (Pythonic naming)
    // ---------------------------------------------------------------------
    
    // Brain statistics and visualization helpers
    m.def("get_brain_stats", &VisualizationHelper::get_brain_statistics,
          py::arg("brain"), "Get detailed statistics about the brain");
    
    m.def("get_region_info", &VisualizationHelper::get_region_info,
          py::arg("brain"), "Get information about brain regions");
    
    m.def("format_statistics", &VisualizationHelper::format_statistics,
          py::arg("stats"), "Format statistics dictionary as string");
    
    // Context manager for automatic cleanup
    m.def("brain_context", [](std::shared_ptr<Brain> brain) {
        return py::cast(BrainContextManager(brain));
    }, py::arg("brain"), "Create a context manager for automatic brain cleanup");
    
    // Simulation utilities
    m.def("run_simulation", &SimulationHelper::run_simulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("steps"), py::arg("timestep"), py::arg("verbose") = false,
          "Run a complete simulation with brain, world, and agent");
    
    m.def("run_episodes", &SimulationHelper::run_episodes,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("episodes"), py::arg("steps_per_episode"), py::arg("timestep"),
          py::arg("verbose") = false,
          "Run multiple simulation episodes");
    
    // Memory management utilities
    m.def("optimize_memory", &MemoryManager::optimizeMemory,
          py::arg("brain"), "Optimize memory usage");
    
    m.def("get_memory_usage", &MemoryManager::getMemoryUsage,
          py::arg("brain"), "Get approximate memory usage in bytes");
    
    m.def("cleanup", &MemoryManager::cleanup,
          py::arg("brain"), "Force cleanup of temporary objects");
    
    // Serialization utilities
    m.def("save_brain_state", &SerializationHelper::save_brain_state,
          py::arg("brain"), py::arg("filepath"), py::arg("compress") = false,
          "Save brain state to file");
    
    m.def("load_brain_state", &SerializationHelper::load_brain_state,
          py::arg("brain"), py::arg("filepath"),
          "Load brain state from file");
    
    m.def("export_config", &SerializationHelper::export_config,
          py::arg("config"), "Export configuration as string");
    
    m.def("import_config", &SerializationHelper::import_config,
          py::arg("config"), py::arg("json_str"),
          "Import configuration from JSON string");
    
    // Convenience functions
    m.def("reset_all", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        if (brain) brain->reset();
        if (world) world->reset();
        if (agent) agent->reset();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Reset all components");
    
    m.def("get_combined_stats", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        py::dict stats;
        if (brain) {
            auto brain_stats = VisualizationHelper::get_brain_statistics(brain);
            for (auto item : brain_stats) {
                stats[item.first] = item.second;
            }
        }
        if (world) {
            stats["world_width"] = world->getWidth();
            stats["world_height"] = world->getHeight();
            stats["max_energy"] = world->getMaxEnergy();
            stats["simulation_time"] = world->getSimulationTime();
        }
        return stats;
    }, py::arg("brain"), py::arg("world"), "Get combined statistics from brain and world");
    
    // Error handling utilities
    m.def("safe_operation", [](py::function func, py::args args, py::kwargs kwargs) {
        try {
            return func(args, kwargs);
        } catch (const NLMException& e) {
            throw;
        } catch (...) {
            throw NLMRuntimeError("Unknown error in operation");
        }
    }, py::arg("func"), py::arg("args"), py::arg("kwargs"),
       "Execute a function safely with proper error handling");
    
    // Get recent errors
    m.def("get_recent_errors", &ExceptionHelper::getRecentErrors,
          py::arg("limit") = 100, "Get recent error messages");
}

// Global exception translator
void setupPybindExceptionTranslation(py::module& m) {
    // Register exception translators for NLM exceptions
    py::register_exception_translator<NLMException>([](const std::exception& e, py::module& m) {
        try {
            throw;
        } catch (const NLMConfigurationException& e) {
            py::register_exception<NLMConfigurationException>(m, "NLMConfigurationError");
        } catch (const NLMValueError& e) {
            py::register_exception<NLMValueError>(m, "NLMValueError");
        } catch (const NLMStateException& e) {
            py::register_exception<NLMStateException>(m, "NLMNotInitializedError");
        } catch (const NLMMemoryException& e) {
            py::register_exception<NLMMemoryException>(m, "NLMMemoryError");
        } catch (const NLMRessourceException& e) {
            py::register_exception<NLMRessourceException>(m, "NLMResourceError");
        } catch (const NLMOperationException& e) {
            py::register_exception<NLMOperationException>(m, "NLMOperationError");
        } catch (const NLMException& e) {
            py::register_exception<NLMException>(m, "NLMRuntimeError");
        }
    });
}

void setupExceptionTranslation() {
    // Setup exception translation for global use
    py::module m = py::module::import("pynlm");
    setupPybindExceptionTranslation(m);
}

// Helper to catch and translate exceptions
template<typename Func, typename... Args>
auto safeExecute(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
    try {
        return func(std::forward<Args>(args)...);
    } catch (const NLMException& e) {
        throw;
    } catch (const std::exception& e) {
        throw NLMOperationException(std::string("Operation failed: ") + e.what());
    }
}

} // namespace nlm