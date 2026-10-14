#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine)";
    
    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";
    
    // Configuration class
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("load_from_file", &nlm::Config::loadFromFile, "Load configuration from file")
        .def("load_from_args", &nlm::Config::loadFromArgs, "Load configuration from command line arguments")
        .def("save_to_file", &nlm::Config::saveToFile, "Save configuration to file")
        .def("set", py::overload_cast<const std::string&, const nlm::ConfigValue&, nlm::ConfigSource>(&nlm::Config::set),
             "Set configuration value")
        .def("get", py::overload_cast<const std::string&, int64_t>(&nlm::Config::get<int64_t>), "Get configuration value")
        .def("get", py::overload_cast<const std::string&, double>(&nlm::Config::get<double>), "Get configuration value")
        .def("get", py::overload_cast<const std::string&, bool>(&nlm::Config::get<bool>), "Get configuration value")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&nlm::Config::get<std::string>), "Get configuration value")
        .def("get_or", py::overload_cast<const std::string&, int64_t, int64_t>(&nlm::Config::getOr<int64_t>, "Get configuration value or default")
        .def("get_or", py::overload_cast<const std::string&, double, double>(&nlm::Config::getOr<double>, "Get configuration value or default")
        .def("get_or", py::overload_cast<const std::string&, bool, bool>(&nlm::Config::getOr<bool>, "Get configuration value or default")
        .def("get_or", py::overload_cast<const std::string&, const std::string&, const std::string&>(&nlm::Config::getOr<std::string>, "Get configuration value or default")
        .def("has", &nlm::Config::has, "Check if configuration has key")
        .def("remove", &nlm::Config::remove, "Remove configuration key")
        .def("get_keys", &nlm::Config::getKeys, "Get all configuration keys")
        .def("clear", &nlm::Config::clear, "Clear all configuration")
        .def("summary", &nlm::Config::summary, "Get configuration summary")
        .def("validate_against_schema", &nlm::Config::validateAgainstSchema, "Validate configuration against schema")
        .def("is_valid", &nlm::Config::isValid, "Check if configuration is valid")
        .def("get_validation_errors", &nlm::Config::getValidationErrors, "Get validation errors");
    
    // Create brain function
    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) {
        return std::make_shared<nlm::Brain>(config);
    }, "Create a brain with the given configuration", py::arg("config") = nullptr);
    
    // Create default config function
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        nlm::ConfigManager::getInstance().initializeDefaultConfig();
        return config;
    }, "Create a default configuration with schema validation");
    
    // Brain class
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>(), "Create brain with configuration")
        .def("initialize", &nlm::Brain::initialize, "Initialize brain")
        .def("step", py::overload_cast<nlm::SimulationStep>(&nlm::Brain::step), "Step brain with step count")
        .def("step", py::overload_cast<nlm::SimulationStep, nlm::Timestamp>(&nlm::Brain::step), "Step brain with step count and timestamp")
        .def("receive_sensory_input", &nlm::Brain::receiveSensoryInput, "Receive sensory input")
        .def("inject_current", &nlm::Brain::injectCurrent, "Inject current into specific neuron")
        .def("inject_current_to_neurons", &nlm::Brain::injectCurrentToNeurons, "Inject current into neurons of specific type")
        .def("get_spike_system", &nlm::Brain::getSpikeSystem, "Get spike system")
        .def("get_stdp", &nlm::Brain::getSTDP, "Get STDP plasticity system")
        .def("get_hebbian", &nlm::Brain::getHebbian, "Get Hebbian plasticity system")
        .def("get_structural_plasticity", &nlm::Brain::getStructuralPlasticity, "Get structural plasticity system")
        .def("get_total_spike_count", &nlm::Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_firing_neuron_count", &nlm::Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("get_active_neuron_count", &nlm::Brain::getActiveNeuronCount, "Get active neuron count")
        .def("get_total_neuron_count", &nlm::Brain::getTotalNeuronCount, "Get total neuron count")
        .def("get_total_synapse_count", &nlm::Brain::getTotalSynapseCount, "Get total synapse count")
        .def("get_excitation_inhibition_ratio", &nlm::Brain::getExcitationInhibitionRatio, "Get excitation/inhibition ratio")
        .def("get_average_firing_rate", &nlm::Brain::getAverageFiringRate, "Get average firing rate")
        .def("produce_action", &nlm::Brain::produceAction, "Produce motor action")
        .def("apply_neuromodulation", &nlm::Brain::applyNeuromodulation, "Apply neuromodulatory signal")
        .def("update_plasticity", &nlm::Brain::updatePlasticity, "Update plasticity rules")
        .def("develop", &nlm::Brain::develop, "Apply developmental changes")
        .def("reset", &nlm::Brain::reset, "Reset brain state")
        .def("save", &nlm::Brain::save, "Save brain state to file")
        .def("load", &nlm::Brain::load, "Load brain state from file")
        .def("add_region", &nlm::Brain::addRegion, "Add neural region")
        .def("get_region", &nlm::Brain::getRegion, "Get neural region by ID")
        .def("get_region_count", &nlm::Brain::getRegionCount, "Get number of regions")
        .def("get_region_ids", &nlm::Brain::getRegionIds, "Get all region IDs")
        .def("get_working_memory", &nlm::Brain::getWorkingMemory, "Get working memory system")
        .def("get_episodic_memory", &nlm::Brain::getEpisodicMemory, "Get episodic memory system")
        .def("get_associative_memory", &nlm::Brain::getAssociativeMemory, "Get associative memory system")
        .def("get_prediction_system", &nlm::Brain::getPredictionSystem, "Get prediction system")
        .def("get_planner", &nlm::Brain::getPlanner, "Get neural planner")
        .def("get_concept_formation", &nlm::Brain::getConceptFormation, "Get concept formation system")
        .def("get_attention", &nlm::Brain::getAttention, "Get attentional selection system")
        .def("get_development_system", &nlm::Brain::getDevelopmentSystem, "Get development system")
        .def("get_dopamine", &nlm::Brain::getDopamine, "Get dopamine system")
        .def("get_curiosity", &nlm::Brain::getCuriosity, "Get curiosity system")
        .def("get_novelty", &nlm::Brain::getNovelty, "Get novelty detection system")
        .def("get_prediction_error_signal", &nlm::Brain::getPredictionErrorSignal, "Get prediction error signal")
        .def("get_config", &nlm::Brain::getConfig, "Get brain configuration")
        .def("get_random_generator", &nlm::Brain::getRandomGenerator, "Get random generator")
        .def("log_status", &nlm::Brain::logStatus, "Log brain status");
    
    // Command Manager functions
    py::class_<nlm::CommandManager>(m, "CommandManager")
        .def("execute", &nlm::CommandManager::execute, "Execute a command")
        .def("undo", &nlm::CommandManager::undo, "Undo last command")
        .def("redo", &nlm::CommandManager::redo, "Redo last undone command")
        .def("clear_history", &nlm::CommandManager::clearHistory, "Clear command history")
        .def("can_undo", &nlm::CommandManager::canUndo, "Check if undo is available")
        .def("can_redo", &nlm::CommandManager::canRedo, "Check if redo is available")
        .def("get_command", &nlm::CommandManager::getCommand, "Get command by name")
        .def("get_commands_by_category", &nlm::CommandManager::getCommandsByCategory, "Get commands by category")
        .def("get_history", &nlm::CommandManager::getHistory, "Get command history")
        .def("get_command_manager", &nlm::Brain::getCommandManager, "Get command manager from brain");
    
    // Command types
    py::enum_<nlm::CommandCategory>(m, "CommandCategory")
        .value("Configuration", nlm::CommandCategory::Configuration)
        .value("Simulation", nlm::CommandCategory::Simulation)
        .value("Memory", nlm::CommandCategory::Memory)
        .value("SensorMotor", nlm::CommandCategory::SensorMotor)
        .value("Plasticity", nlm::CommandCategory::Plasticity)
        .value("Cognitive", nlm::CommandCategory::Cognitive)
        .value("Development", nlm::CommandCategory::Development)
        .value("Neuromodulation", nlm::CommandCategory::Neuromodulation)
        .export_values();
    
    // Enum types
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Interneuron", nlm::NeuronType::Interneuron)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();
    
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Embryonic", nlm::DevelopmentalStage::Embryonic)
        .value("Juvenile", nlm::DevelopmentalStage::Juvenile)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .export_values();
    
    py::enum_<nlm::LogLevel>(m, "LogLevel")
        .value("Trace", nlm::LogLevel::Trace)
        .value("Debug", nlm::LogLevel::Debug)
        .value("Info", nlm::LogLevel::Info)
        .value("Warn", nlm::LogLevel::Warn)
        .value("Error", nlm::LogLevel::Error)
        .value("Critical", nlm::LogLevel::Critical)
        .value("Off", nlm::LogLevel::Off)
        .export_values();
    
    // Neural region functions
    m.def("add_region", &nlm::Brain::addRegion, "Add region to brain");
    m.def("get_region", &nlm::Brain::getRegion, "Get region by ID");
    
    // Helper functions
    m.def("create_default_brain", []() {
        auto config = std::make_shared<nlm::Config>();
        nlm::ConfigManager::getInstance().initializeDefaultConfig();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a brain with default configuration and initialize it");
    
    m.def("run_phase2_demo", []() {
        // Simulate running the main.cpp demo
        auto config = std::make_shared<nlm::Config>();
        nlm::ConfigManager::getInstance().initializeDefaultConfig();
        auto brain = std::make_shared<nlm::Brain>(config);
        
        if (brain->initialize()) {
            // Run basic connectivity test
            brain->reset();
            brain->initialize();
            
            // Inject current into sensory neurons
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 50.0f);
            
            // Run simulation steps
            for (nlm::SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            // Get statistics
            auto spikeCount = brain->getTotalSpikeCount();
            auto neuronCount = brain->getTotalNeuronCount();
            auto firingRate = brain->getAverageFiringRate();
            
            return nlm::py::dict(
                "spike_count", spikeCount,
                "neuron_count", neuronCount,
                "firing_rate", firingRate,
                "success", true
            );
        }
        
        return nlm::py::dict(
            "success", false,
            "error", "Brain initialization failed"
        );
    }, "Run Phase 2 demo (LIF neurons with plasticity)");
    
    // Performance profiling utilities
    m.def("start_profile", &nlm::profiling::start, "Start profiling an operation");
    m.def("end_profile", &nlm::profiling::end, "End profiling an operation");
    m.def("get_profiling_results", &nlm::profiling::getResults, "Get profiling results");
    m.def("set_profiling_level", &nlm::profiling::setLevel, "Set profiling level");
    m.def("save_profiling_results", &nlm::profiling::saveToFile, "Save profiling results to file");
}