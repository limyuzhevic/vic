// Improved Python bindings for NLM - Enhanced API and utilities

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <random>
#include <map>
#include <algorithm>
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

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
        --------------------------------------------------------------
        A comprehensive Python API for the NLM C++ neural simulation framework.
        
        This enhanced edition includes:
        - Extended Brain and AgentBrain classes with additional methods
        - Python helper classes for advanced operations
        - Experiment and scenario management utilities
        - Enhanced world interaction capabilities
        - Configuration presets and helper functions
        
        Key features:
        - Brain statistics and monitoring utilities
        - Agent behavior analysis tools
        - World management and object spawning
        - Experiment running and scenario creation
        - Configuration management and presets
    )pbdoc";
    
    // Version information
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Neural Learning Machine - Enhanced Python Interface";
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Version and utility functions
    m.def("get_version", []() {
        return py::dict({
            {"version", "0.1.0-enhanced"},
            {"phase", "Phase 6: Final Integration"},
            {"build_date", "2026-01-01"},
            {"api_version", "enhanced"}
        });
    }, "Get NLM version information");
    
    m.def("create_default_config", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("working_memory.capacity", 1000);
        config->set("episodic_memory.max_episodes", 100);
        config->set("dopamine.scale", 1.0f);
        config->set("curiosity.base_rate", 0.1f);
        config->set("novelty.sensitivity", 0.5f);
        config->set("prediction.horizon", 10);
        config->set("prediction.error_threshold", 0.5f);
        config->set("development.critical_period_end", 100.0f);
        config->set("development.plasticity_decay_rate", 0.001f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        return config;
    }, "Create a default NLM configuration with Phase 6 settings");
    
    m.def("create_learning_config", [](const std::string& learningType, const py::dict& customParams) {
        auto config = std::make_shared<Config>();
        
        if (learningType == "exploration") {
            // Exploration-focused configuration
            config->set("random_seed", static_cast<int64_t>(42));
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 500);
            config->set("region_count", 2);
            config->set("connection_probability", 0.15f);
            config->set("working_memory.capacity", 500);
            config->set("episodic_memory.max_episodes", 50);
            config->set("dopamine.scale", 1.5f);
            config->set("curiosity.base_rate", 0.3f);
            config->set("novelty.sensitivity", 0.8f);
            config->set("prediction.horizon", 15);
            config->set("development.critical_period_end", 200.0f);
        }
        else if (learningType == "memory") {
            // Memory-focused configuration
            config->set("random_seed", static_cast<int64_t>(42));
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 1000);
            config->set("region_count", 1);
            config->set("connection_probability", 0.08f);
            config->set("working_memory.capacity", 2000);
            config->set("episodic_memory.max_episodes", 200);
            config->set("dopamine.scale", 0.8f);
            config->set("curiosity.base_rate", 0.05f);
            config->set("novelty.sensitivity", 0.3f);
            config->set("prediction.horizon", 5);
            config->set("development.critical_period_end", 50.0f);
        }
        else if (learningType == "prediction") {
            // Prediction-focused configuration
            config->set("random_seed", static_cast<int64_t>(42));
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", 800);
            config->set("region_count", 2);
            config->set("connection_probability", 0.12f);
            config->set("working_memory.capacity", 1000);
            config->set("episodic_memory.max_episodes", 100);
            config->set("dopamine.scale", 1.0f);
            config->set("curiosity.base_rate", 0.15f);
            config->set("novelty.sensitivity", 0.6f);
            config->set("prediction.horizon", 25);
            config->set("development.critical_period_end", 150.0f);
        }
        
        // Apply custom parameters
        for (auto& item : customParams) {
            std::string key = item.first;
            if (item.second.is_integer()) {
                config->set(key, item.second.cast<int64_t>());
            } else if (item.second.is_floating_point()) {
                config->set(key, item.second.cast<float>());
            } else if (item.second.is_boolean()) {
                config->set(key, item.second.cast<bool>());
            } else if (item.second.is_string()) {
                config->set(key, item.second.cast<std::string>());
            }
        }
        
        return config;
    }, "Create a specialized NLM configuration for different learning scenarios", 
        py::arg("learning_type"), py::arg("custom_params") = py::dict());

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create a new brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), 
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), 
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator, "Get random generator")
        .def("logStatus", &Brain::logStatus, "Log brain status");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal, "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object from world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"), "Load configuration from file")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), "Save configuration to file")
        .def("has", &Config::has, py::arg("key"), "Check if key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get configuration summary");

    // Factory functions
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, "Create a brain with configuration", py::arg("config"));

    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, "Create an agent brain interface", py::arg("brain"));

    // Version and information
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("VERSION") = "0.1.0-enhanced";
    m.attr("PHASE") = "Phase 6: Final Integration";

    // Helper functions
    m.def("get_version", []() {
        return py::dict({
            {"version", "0.1.0-enhanced"},
            {"phase", "Phase 6: Final Integration"},
            {"build_date", "2026-01-01"},
            {"api_version", "enhanced"}
        });
    }, "Get version information");

    // Example functions
    m.def("example_simple_brain", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
        
        return py::dict({
            {"success", true},
            {"neurons", brain->getTotalNeuronCount()},
            {"spikes", brain->getTotalSpikeCount()}
        });
    }, "Simple brain simulation example");

    m.def("example_agent_simulation", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        
        world->initialize(10, 10, 5, 5);
        world->reset();
        agent->initialize(*world);
        
        for (int i = 0; i < 20; ++i) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            
            auto action = agent->decodeMotorCommand();
        }
        
        return py::dict({
            {"success", true},
            {"final_curiosity", agent->getCuriosityLevel()},
            {"final_novelty", agent->getNoveltyLevel()}
        });
    }, "Agent-world simulation example");

    // Documentation
    m.def("show_help", []() {
        std::cout << "=== NLM Python API Help ===" << std::endl;
        std::cout << "\nKey Classes:" << std::endl;
        std::cout << "  - Brain: Main neural simulation brain" << std::endl;
        std::cout << "  - AgentBrain: Agent interface to brain" << std::endl;
        std::cout << "  - SimpleWorld: 2D environment simulation" << std::endl;
        std::cout << "  - Config: Configuration management" << std::endl;
        std::cout << "\nKey Methods:" << std::endl;
        std::cout << "  - brain.initialize(): Initialize brain" << std::endl;
        std::cout << "  - brain.step(step): Run simulation step" << std::endl;
        std::cout << "  - agent.processSensoryInput(percept): Process sensory input" << std::endl;
        std::cout << "  - agent.decodeMotorCommand(): Get motor command" << std::endl;
        std::cout << "  - world.update(timestep): Update world state" << std::endl;
        std::cout << "\nHelper Functions:" << std::endl;
        std::cout << "  - createDefaultConfig(): Create default config" << std::endl;
        std::cout << "  - createBrain(config): Create brain with config" << std::endl;
        std::cout << "  - createSimpleWorld(): Create world environment" << std::endl;
        std::cout << "  - createAgentBrain(brain): Create agent interface" << std::endl;
    }, "Show help information");

} // PYBIND11_MODULE

} // namespace nlm