#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all the NLM core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Include the improved API headers
#include "improved_api/NLMImprovedAPI.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Improved Python API
        ------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides:
        1. Enhanced configuration building with fluent APIs
        2. Context managers for automatic resource management
        3. Quick setup functions for common patterns
        4. Performance monitoring and statistics
        5. Error handling with Pythonic exceptions
        6. Simulation helpers for common workflows
        
        The API is designed to be both beginner-friendly and powerful for advanced users.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<nlm::ErrorHandling::NLMError>(m, "NLMError");
    py::register_exception<nlm::ErrorHandling::BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<nlm::ErrorHandling::ConfigurationError>(m, "ConfigurationError");
    py::register_exception<nlm::ErrorHandling::SimulationError>(m, "SimulationError");
    py::register_exception<nlm::ErrorHandling::MemoryError>(m, "MemoryError");
    py::register_exception<nlm::ErrorHandling::ValidationError>(m, "ValidationError");

    // -------------------------------------------------------------------------
    // Basic Types and Enums (unchanged for backward compatibility)
    // -------------------------------------------------------------------------

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

    // Enumerations (unchanged)
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

    // -------------------------------------------------------------------------
    // Enhanced Configuration System
    // -------------------------------------------------------------------------

    // Original Config class (backward compatibility)
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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

    // NEW: Configuration Builder
    py::class_<ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(Fluent API for creating common configurations)pbdoc")
        .def(py::init<>())
        .def("with_neural_params", &ConfigBuilder::with_neural_params,
             py::arg("neuron_count"), py::arg("gain") = 1.0f, py::arg("threshold") = -50.0f,
             "Configure neural system parameters")
        .def("with_synapse_params", &ConfigBuilder::with_synapse_params,
             py::arg("density"), py::arg("weight_mean"), py::arg("weight_std"),
             "Configure synaptic parameters")
        .def("with_plasticity", &ConfigBuilder::with_plasticity,
             py::arg("enable_stdp"), py::arg("enable_hebbian"), py::arg("enable_structural"),
             py::arg("stdp_learning_rate") = 0.001f, py::arg("hebbian_strength") = 0.5f,
             "Configure plasticity rules")
        .def("with_development", &ConfigBuilder::with_development,
             py::arg("stage"), py::arg("age_rate") = 1.0, py::arg("enable_critical_period") = true,
             "Configure developmental parameters")
        .def("with_neuromodulation", &ConfigBuilder::with_neuromodulation,
             py::arg("enable_curiosity") = true, py::arg("enable_novelty") = true,
             py::arg("enable_reward") = true, py::arg("dopamine_scale") = 1.0f,
             "Configure neuromodulation")
        .def("with_memory", &ConfigBuilder::with_memory,
             py::arg("enable_working_memory") = true, py::arg("enable_episodic_memory") = true,
             py::arg("memory_capacity") = 1000.0f,
             "Configure memory systems")
        .def("with_prediction", &ConfigBuilder::with_prediction,
             py::arg("enable_prediction") = true, py::arg("prediction_horizon") = 0.1f,
             py::arg("prediction_error_weight") = 1.0f,
             "Configure prediction system")
        .def("with_world", &ConfigBuilder::with_world,
             py::arg("width") = 20, py::arg("height") = 20,
             py::arg("vision_width") = 8, py::arg("vision_height") = 8,
             py::arg("vision_range") = 1.0f,
             "Configure world parameters")
        .def("with_agent", &ConfigBuilder::with_agent,
             py::arg("enable_learning") = true, py::arg("enable_planning") = true,
             py::arg("enable_social") = false,
             "Configure agent parameters")
        .def("build", &ConfigBuilder::build,
             "Build the configuration object");

    // NEW: Quick Setup Functions
    m.def("create_agent_world_setup", [](int width, int height, int vision_width, int vision_height,
                                        bool enable_learning) {
        return QuickSetup::create_agent_world_setup(width, height, vision_width, vision_height,
                                                    enable_learning, true, true);
    }, py::arg("width") = 10, py::arg("height") = 10,
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       py::arg("enable_learning") = true,
       "Quick setup: Create brain, world, and agent in one call");

    m.def("create_research_config", [](int width, int height) {
        return ConfigBuilder::create_research_config(width, height);
    }, py::arg("width") = 20, py::arg("height") = 20,
       "Create a research-oriented configuration");

    m.def("create_rl_config", [](float learning_rate) {
        return ConfigBuilder::create_rl_config(learning_rate);
    }, py::arg("learning_rate") = 0.01f,
       "Create a reinforcement learning configuration");

    m.def("create_developmental_config", [](double lifespan) {
        return ConfigBuilder::create_developmental_config(lifespan);
    }, py::arg("lifespan") = 60.0,
       "Create a developmental configuration");

    m.def("create_reproduction_config", []() {
        return ConfigBuilder::create_reproduction_config();
    }, "Create a reproduction configuration");

    // -------------------------------------------------------------------------
    // Enhanced Brain Interface
    // -------------------------------------------------------------------------

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
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // -------------------------------------------------------------------------
    // Enhanced Agent Interface
    // -------------------------------------------------------------------------

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

    // -------------------------------------------------------------------------
    // Context Managers (NEW - for automatic resource management)
    // -------------------------------------------------------------------------

    // Brain Context Manager
    py::class_<BrainContextManager>(m, "BrainContextManager", R"pbdoc(Context manager for automatic brain lifecycle)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config") = nullptr)
        .def("__enter__", &BrainContextManager::enter)
        .def("__exit__", &BrainContextManager::exit)
        .def("brain", &BrainContextManager::brain,
             "Get the managed brain instance")
        .def("config", &BrainContextManager::config,
             "Get the configuration used")
        .def("step", &BrainContextManager::step,
             py::arg("steps"),
             "Execute multiple steps")
        .def("run_simulation", &BrainContextManager::run_simulation,
             py::arg("steps"), py::arg("timestep") = 0.001,
             "Run a complete simulation")
        .def("run_episode", &BrainContextManager::run_episode,
             py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("timestep") = 0.1,
             "Run an agent episode");

    // Agent Context Manager
    py::class_<AgentContextManager>(m, "AgentContextManager", R"pbdoc(Context manager for automatic agent lifecycle)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"))
        .def("__enter__", &AgentContextManager::enter)
        .def("__exit__", &AgentContextManager::exit)
        .def("agent", &AgentContextManager::agent,
             "Get the managed agent instance")
        .def("brain", &AgentContextManager::brain,
             "Get the brain instance")
        .def("world", &AgentContextManager::world,
             "Get the world instance")
        .def("initialize", &AgentContextManager::initialize)
        .def("reset", &AgentContextManager::reset)
        .def("process_sensory_input", &AgentContextManager::process_sensory_input,
             py::arg("percept"),
             "Process a single sensory input")
        .def("decode_motor_command", &AgentContextManager::decode_motor_command,
             "Decode current motor command")
        .def("apply_reward", &AgentContextManager::apply_reward,
             py::arg("reward"), py::arg("predicted") = 0.0f,
             "Apply reward modulation")
        .def("update_development", &AgentContextManager::update_development,
             py::arg("timestep") = 0.1,
             "Update development system");

    // -------------------------------------------------------------------------
    // Simulation Helpers
    // -------------------------------------------------------------------------

    // Performance Monitor
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Monitor simulation performance and resources)pbdoc")
        .def(py::init<>())
        .def("start", &PerformanceMonitor::start,
             "Start performance monitoring")
        .def("stop", &PerformanceMonitor::stop,
             "Stop monitoring and return statistics")
        .def("is_running", &PerformanceMonitor::is_running,
             "Check if monitoring is active")
        .def("set_sampling_interval", &PerformanceMonitor::set_sampling_interval,
             py::arg("interval"),
             "Set monitoring sampling interval (seconds)")
        .def("enable_memory_tracking", &PerformanceMonitor::enable_memory_tracking,
             py::arg("enable"),
             "Enable/disable memory tracking")
        .def("enable_cpu_tracking", &PerformanceMonitor::enable_cpu_tracking,
             py::arg("enable"),
             "Enable/disable CPU tracking")
        .def("get_current_stats", &PerformanceMonitor::get_current_stats,
             "Get current performance statistics")
        .def("get_performance_report", &PerformanceMonitor::get_performance_report,
             "Get formatted performance report");

    // Simulation result structure
    py::class_<SimulationHelpers::SimulationResult>(m, "SimulationResult", R"pbdoc(Result of a simulation run)pbdoc")
        .def_readonly("total_steps", &SimulationHelpers::SimulationResult::total_steps)
        .def_readonly("total_spikes", &SimulationHelpers::SimulationResult::total_spikes)
        .def_readonly("avg_firing_rate", &SimulationHelpers::SimulationResult::avg_firing_rate)
        .def_readonly("final_reward", &SimulationHelpers::SimulationResult::final_reward)
        .def_readonly("memory_episodes_stored", &SimulationHelpers::SimulationResult::memory_episodes_stored)
        .def_readonly("wall_clock_time", &SimulationHelpers::SimulationResult::wall_clock_time)
        .def_readonly("active_neurons", &SimulationHelpers::SimulationResult::active_neurons)
        .def_readonly("log_messages", &SimulationHelpers::SimulationResult::log_messages);

    // Simulation helper functions
    m.def("run_brain_simulation", &SimulationHelpers::run_brain_simulation,
          py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
          "Run a brain simulation and return results");

    m.def("run_agent_simulation", &SimulationHelpers::run_agent_simulation,
          py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1,
          "Run an agent simulation and return results");

    m.def("run_learning_episode", &SimulationHelpers::run_learning_episode,
          py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("enable_learning") = true,
          "Run a learning episode with reward modulation");

    m.def("run_research_pattern", &SimulationHelpers::run_research_pattern,
          py::arg("brain"), py::arg("steps"),
          "Run a research pattern simulation");

    m.def("run_rl_pattern", &SimulationHelpers::run_rl_pattern,
          py::arg("agent"), py::arg("world"), py::arg("steps"),
          "Run a reinforcement learning pattern simulation");

    m.def("run_development_pattern", &SimulationHelpers::run_development_pattern,
          py::arg("brain"), py::arg("steps"), py::arg("lifespan") = 60.0,
          "Run a developmental pattern simulation");

    m.def("collect_firing_rates", &SimulationHelpers::collect_firing_rates,
          py::arg("brain"), py::arg("samples"), py::arg("timestep") = 0.001,
          "Collect firing rate statistics over time");

    m.def("collect_reward_history", &SimulationHelpers::collect_reward_history,
          py::arg("agent"), py::arg("world"), py::arg("steps"),
          "Collect reward history over an episode");

    // -------------------------------------------------------------------------
    // Error Handling
    // -------------------------------------------------------------------------

    // Custom exception classes
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ErrorHandling::NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Error recovery utilities
    m.def("attempt_recovery", &ErrorHandling::attempt_recovery,
          py::arg("brain"), py::arg("max_attempts") = 3,
          "Attempt to recover from brain initialization failure");

    m.def("get_error_context", &ErrorHandling::get_error_context,
          py::arg("exception"),
          "Get contextual information about an error");

    m.def("log_error_safely", &ErrorHandling::log_error_safely,
          py::arg("exception"),
          "Safely log an error without crashing");

    // -------------------------------------------------------------------------
    // Utility Functions
    // -------------------------------------------------------------------------

    py::class_<Utils>(m, "Utils", "Utility functions for string, math, and file operations")
        .def_static("trim", &Utils::trim, py::arg("str"),
                   "Remove whitespace from string")
        .def_static("to_lower", &Utils::to_lower, py::arg("str"),
                   "Convert string to lowercase")
        .def_static("format_duration", &Utils::format_duration, py::arg("seconds"),
                   "Format duration in human-readable format")
        .def_static("format_number", &Utils::format_number, py::arg("num"),
                   "Format number as string")
        .def_static("clamp", &Utils::clamp, py::arg("value"), py::arg("min"), py::arg("max"),
                   "Clamp value between min and max")
        .def_static("lerp", &Utils::lerp, py::arg("a"), py::arg("b"), py::arg("t"),
                   "Linear interpolation between a and b")
        .def_static("sigmoid", &Utils::sigmoid, py::arg("x"),
                   "Sigmoid activation function")
        .def_static("file_exists", &Utils::file_exists, py::arg("path"),
                   "Check if file exists")
        .def_static("get_temp_filename", &Utils::get_temp_filename, py::arg("prefix") = "nlm_",
                   "Generate a temporary filename")
        .def_static("safe_write_file", &Utils::safe_write_file, py::arg("path"), py::arg("content"),
                   "Safely write content to file")
        .def_static("seed_from_time", &Utils::seed_from_time,
                   "Get random seed from current time")
        .def_static("seed_from_string", &Utils::seed_from_string, py::arg("str"),
                   "Get random seed from string")
        .def_static("current_time_seconds", &Utils::current_time_seconds,
                   "Get current time in seconds since epoch")
        .def_static("timestamp_string", &Utils::timestamp_string,
                   "Get current timestamp as string")
        .def_static("neuron_type_to_string", &Utils::neuron_type_to_string, py::arg("type"),
                   "Convert neuron type to string")
        .def_static("string_to_neuron_type", &Utils::string_to_neuron_type, py::arg("str"),
                   "Convert string to neuron type")
        .def_static("action_type_to_string", &Utils::action_type_to_string, py::arg("type"),
                   "Convert action type to string")
        .def_static("string_to_action_type", &Utils::string_to_action_type, py::arg("str"),
                   "Convert string to action type");

    // -------------------------------------------------------------------------
    // Quick Setup Functions
    // -------------------------------------------------------------------------

    // Simulation shortcuts
    m.def("run_research_simulation", &QuickSetup::run_research_simulation,
          py::arg("steps") = 1000,
          "Run a complete research simulation");

    m.def("run_rl_simulation", &QuickSetup::run_rl_simulation,
          py::arg("steps") = 2000,
          "Run a complete reinforcement learning simulation");

    m.def("run_development_simulation", &QuickSetup::run_development_simulation,
          py::arg("steps") = 3000,
          "Run a complete developmental simulation");

    // Analysis shortcuts
    m.def("analyze_brain_efficiency", &QuickSetup::analyze_brain_efficiency,
          py::arg("brain"), py::arg("steps") = 1000,
          "Analyze brain simulation efficiency");

    m.def("analyze_learning_efficiency", &QuickSetup::analyze_learning_efficiency,
          py::arg("agent"), py::arg("world"), py::arg("steps") = 1000,
          "Analyze agent learning efficiency");

    // -------------------------------------------------------------------------
    // Original Functions (for backward compatibility)
    // -------------------------------------------------------------------------

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Original constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Additional utility constants
    m.def("version", []() { return "NLM Python Enhanced API v1.0"; },
          "Get the version of the enhanced API");

    m.def("get_api_info", []() {
        py::dict info;
        info["version"] = "NLM Python Enhanced API v1.0";
        info["description"] = "Enhanced Python bindings for NLM with fluent APIs, context managers, and performance monitoring";
        info["features"] = py::list({
            "ConfigurationBuilder - Fluent configuration",
            "ContextManager - Automatic resource management",
            "PerformanceMonitor - Resource usage tracking",
            "ErrorHandling - Pythonic exceptions",
            "QuickSetup - One-line convenience functions",
            "SimulationHelpers - Common workflow helpers",
            "Utils - String, math, and file utilities"
        });
        return info;
    }, "Get information about the enhanced API");

} // namespace nlm

} // namespace nlm