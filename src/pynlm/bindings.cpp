# Python bindings for NLM (Neural Learning Machine)
# Complete neural simulation framework with integrated brain systems

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/sensory/SensoryPercept.hpp"
#include "src/environment/Environment.hpp"

namespace py = pybind11;

// Configuration module
void configure_config(py::module_& m) {
    py::class_<nlm::Config> config_class(m, "Config");
    
    config_class.def(py::init<>());
    
    // Configuration methods
    config_class.def("set", &nlm::Config::set,
        py::arg("key"), py::arg("value"),
        "Set configuration value with string key");
    
    config_class.def("getOr", &nlm::Config::getOr,
        py::arg("key"), py::arg("default_value"),
        "Get configuration value or return default if not found");
    
    config_class.def("loadFromFile", &nlm::Config::loadFromFile,
        py::arg("filename"), "Load configuration from file");
    
    config_class.def("saveToFile", &nlm::Config::saveToFile,
        py::arg("filename"), "Save configuration to file");
    
    config_class.def("has", &nlm::Config::has,
        py::arg("key"), "Check if configuration key exists");
    
    config_class.def("getKeys", &nlm::Config::getKeys,
        "Get all configuration keys");
}

// Brain module
void configure_brain(py::module_& m) {
    // Neuron types enum
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();
    
    // Developmental stages enum
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();
    
    // Action types enum
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("None", nlm::ActionType::None)
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("TurnAround", nlm::ActionType::TurnAround)
        .value("Jump", nlm::ActionType::Jump)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Attack", nlm::ActionType::Attack)
        .value("Wait", nlm::ActionType::Wait)
        .value("Take", nlm::ActionType::Take)
        .value("Drop", nlm::ActionType::Drop)
        .export_values();
    
    // Firing state enum
    py::enum_<nlm::FiringState>(m, "FiringState")
        .value("NotFiring", nlm::FiringState::NotFiring)
        .value("Refractory", nlm::FiringState::Refractory)
        .value("Firing", nlm::FiringState::Firing)
        .export_values();
    
    // Synapse type enum
    py::enum_<nlm::SynapseType>(m, "SynapseType")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .export_values();
    
    // Config type
    py::class_<nlm::Config, std::shared_ptr<nlm::Config>> config_type(m, "Config");
    
    // Brain class
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>> brain_class(m, "Brain");
    
    brain_class.def(py::init<std::shared_ptr<nlm::Config>>(),
        py::arg("config"), "Create brain with configuration");
    
    brain_class.def("initialize", &nlm::Brain::initialize,
        "Initialize brain with configuration");
    
    brain_class.def("step", py::overload_cast<uint64_t>(&nlm::Brain::step),
        py::arg("step"), "Run brain simulation for one step");
    
    brain_class.def("step", py::overload_cast<uint64_t, double>(&nlm::Brain::step),
        py::arg("step"), py::arg("time"), "Run brain simulation with time");
    
    brain_class.def("reset", &nlm::Brain::reset,
        "Reset brain state");
    
    brain_class.def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
        py::arg("input"), "Receive sensory input from environment");
    
    brain_class.def("injectCurrent", &nlm::Brain::injectCurrent,
        py::arg("neuron"), py::arg("current"), "Inject current directly into a neuron");
    
    brain_class.def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
        py::arg("type"), py::arg("current"), "Inject current to all neurons of a type");
    
    brain_class.def("produceAction", &nlm::Brain::produceAction,
        "Produce motor/action output based on motor neuron activity");
    
    brain_class.def("applyNeuromodulation", &nlm::Brain::applyNeuromodulation,
        py::arg("signal"), "Apply neuromodulatory signals");
    
    brain_class.def("save", &nlm::Brain::save,
        py::arg("filepath"), "Save brain state to file (checkpointing)");
    
    brain_class.def("load", &nlm::Brain::load,
        py::arg("filepath"), "Load brain state from file");
    
    // Statistics methods
    brain_class.def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount,
        "Get total number of neurons");
    
    brain_class.def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount,
        "Get total number of synapses");
    
    brain_class.def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount,
        "Get number of active neurons");
    
    brain_class.def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount,
        "Get number of currently firing neurons");
    
    brain_class.def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate,
        "Get average firing rate");
    
    brain_class.def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio,
        "Get excitation/inhibition ratio");
    
    brain_class.def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount,
        "Get total spike count");
    
    brain_class.def("getPendingSpikeEventCount", &nlm::Brain::getPendingSpikeEventCount,
        "Get pending spike event count");
    
    // Memory systems
    py::class_<nlm::NeuralWorkingMemory> working_memory_class(m, "WorkingMemory");
    working_memory_class.def("getActiveTraces", &nlm::NeuralWorkingMemory::getActiveTraces,
        "Get number of active memory traces");
    
    brain_class.def("getWorkingMemory", &nlm::Brain::getWorkingMemory,
        "Get working memory system");
    
    py::class_<nlm::NeuralEpisodicMemory> episodic_memory_class(m, "EpisodicMemory");
    episodic_memory_class.def("getEpisodeCount", &nlm::NeuralEpisodicMemory::getEpisodeCount,
        "Get number of stored episodes");
    
    brain_class.def("getEpisodicMemory", &nlm::Brain::getEpisodicMemory,
        "Get episodic memory system");
    
    py::class_<nlm::NeuralAssociativeMemory> associative_memory_class(m, "AssociativeMemory");
    brain_class.def("getAssociativeMemory", &nlm::Brain::getAssociativeMemory,
        "Get associative memory system");
    
    // Prediction system
    py::class_<nlm::PredictionSystem> prediction_system_class(m, "PredictionSystem");
    brain_class.def("getPredictionSystem", &nlm::Brain::getPredictionSystem,
        "Get prediction system");
    
    // Cognition systems
    py::class_<nlm::NeuralPlanner> planner_class(m, "Planner");
    brain_class.def("getPlanner", &nlm::Brain::getPlanner,
        "Get neural planner");
    
    py::class_<nlm::ConceptFormation> concept_formation_class(m, "ConceptFormation");
    brain_class.def("getConceptFormation", &nlm::Brain::getConceptFormation,
        "Get concept formation system");
    
    py::class_<nlm::AttentionalSelection> attention_class(m, "Attention");
    brain_class.def("getAttention", &nlm::Brain::getAttention,
        "Get attentional selection system");
    
    // Development system
    py::class_<nlm::DevelopmentSystem> development_system_class(m, "DevelopmentSystem");
    brain_class.def("getDevelopmentSystem", &nlm::Brain::getDevelopmentSystem,
        "Get development system");
    
    brain_class.def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage,
        "Get developmental stage");
    
    brain_class.def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage,
        py::arg("stage"), "Set developmental stage");
    
    // Neuromodulation systems
    py::class_<nlm::Dopamine> dopamine_class(m, "Dopamine");
    dopamine_class.def("getLevel", &nlm::Dopamine::getLevel,
        "Get dopamine level");
    brain_class.def("getDopamine", &nlm::Brain::getDopamine,
        "Get dopamine system");
    
    py::class_<nlm::Curiosity> curiosity_class(m, "Curiosity");
    curiosity_class.def("getLevel", &nlm::Curiosity::getLevel,
        "Get curiosity level");
    brain_class.def("getCuriosity", &nlm::Brain::getCuriosity,
        "Get curiosity system");
    
    py::class_<nlm::Novelty> novelty_class(m, "Novelty");
    novelty_class.def("getLevel", &nlm::Novelty::getLevel,
        "Get novelty level");
    brain_class.def("getNovelty", &nlm::Brain::getNovelty,
        "Get novelty system");
    
    py::class_<nlm::PredictionError> prediction_error_class(m, "PredictionError");
    prediction_error_class.def("getLevel", &nlm::PredictionError::getLevel,
        "Get prediction error level");
    brain_class.def("getPredictionErrorSignal", &nlm::Brain::getPredictionErrorSignal,
        "Get prediction error system");
    
    // Configuration
    brain_class.def("getConfig", &nlm::Brain::getConfig,
        "Get configuration");
}

// Agent brain module
void configure_agent(py::module_& m) {
    py::class_<nlm::AgentBrain> agent_brain_class(m, "AgentBrain");
    
    agent_brain_class.def(py::init<>());
    
    agent_brain_class.def("initialize", py::overload_cast<nlm::Environment&>(&nlm::AgentBrain::initialize),
        py::arg("world"), "Initialize agent with world");
    
    agent_brain_class.def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
        py::arg("percept"), "Process sensory input from environment");
    
    agent_brain_class.def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
        "Decode motor command from brain activity");
    
    agent_brain_class.def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
        py::arg("reward"), py::arg("predicted_reward"), "Apply reward modulation");
    
    agent_brain_class.def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
        py::arg("timestep"), "Update development system");
    
    // Enable/disable subsystems
    agent_brain_class.def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
        py::arg("enable"), "Enable/disable reward modulation");
    
    agent_brain_class.def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
        py::arg("enable"), "Enable/disable structural plasticity");
    
    agent_brain_class.def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
        py::arg("enable"), "Enable/disable development");
    
    agent_brain_class.def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
        py::arg("enable"), "Enable/disable curiosity");
    
    agent_brain_class.def("enableNovelty", &nlm::AgentBrain::enableNovelty,
        py::arg("enable"), "Enable/disable novelty detection");
    
    // Statistics
    agent_brain_class.def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
        "Get curiosity level");
    
    agent_brain_class.def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
        "Get novelty level");
    
    agent_brain_class.def("getPredictionError", &nlm::AgentBrain::getPredictionError,
        "Get prediction error");
    
    agent_brain_class.def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
        "Get neuromodulation level");
    
    // Get brain reference
    agent_brain_class.def("getBrain", &nlm::AgentBrain::getBrain,
        "Get brain reference");
}

// Simple world module
void configure_world(py::module_& m) {
    py::class_<nlm::SimpleWorld> world_class(m, "SimpleWorld");
    
    world_class.def(py::init<>());
    
    world_class.def("initialize", &nlm::SimpleWorld::initialize,
        py::arg("width"), py::arg("height"), "Initialize world with dimensions");
    
    world_class.def("configure", &nlm::SimpleWorld::configure,
        py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
        "Configure world with vision parameters");
    
    world_class.def("reset", &nlm::SimpleWorld::reset,
        "Reset world to initial state");
    
    world_class.def("update", &nlm::SimpleWorld::update,
        py::arg("timestep"), "Update world by timestep");
    
    world_class.def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed,
        py::arg("seed"), "Set random seed for reproducibility");
    
    // Agent interaction
    world_class.def("setAgentStart", &nlm::SimpleWorld::setAgentStart,
        py::arg("x"), py::arg("y"), "Set agent starting position");
    
    world_class.def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
        py::arg("cmd"), py::arg("currentTime"), "Apply motor command to agent");
    
    // State access
    world_class.def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
        "Get sensory perception from world");
    
    world_class.def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
        "Get agent body");
    
    // Properties
    world_class.def("getWidth", &nlm::SimpleWorld::getWidth,
        "Get world width");
    
    world_class.def("getHeight", &nlm::SimpleWorld::getHeight,
        "Get world height");
    
    world_class.def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime,
        "Get current simulation time");
    
    // World objects
    py::class_<nlm::WorldObject> world_object_class(m, "WorldObject");
    world_object_class.def(py::init<double, double, nlm::WorldObjectType, double, double>(),
        py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value"), py::arg("radius"), "Create world object");
    world_object_class.def_readonly("x", &nlm::WorldObject::x);
    world_object_class.def_readonly("y", &nlm::WorldObject::y);
    world_object_class.def_readonly("type", &nlm::WorldObject::type);
    world_object_class.def_readonly("value", &nlm::WorldObject::value);
    world_object_class.def_readonly("radius", &nlm::WorldObject::radius);
    
    // Action result
    py::class_<nlm::ActionResult> action_result_class(m, "ActionResult", R"pbdoc(Action result from world)pbdoc");
    action_result_class.def_readonly("reward", &nlm::ActionResult::reward);
    action_result_class.def_readonly("success", &nlm::ActionResult::success);
    action_result_class.def_readonly("message", &nlm::ActionResult::message);
    
    m.def("createWorldObject", [](double x, double y, nlm::WorldObjectType type, double value, double radius) {
        return nlm::WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value"), py::arg("radius"), "Create world object");
    
    world_class.def("addObject", &nlm::SimpleWorld::addObject,
        py::arg("obj"), "Add world object");
    
    world_class.def("removeObject", &nlm::SimpleWorld::removeObject,
        py::arg("x"), py::arg("y"), "Remove world object at position");
    
    world_class.def("isValidPosition", &nlm::SimpleWorld::isValidPosition,
        py::arg("x"), py::arg("y"), "Check if position is valid");
}

// Utility functions
void configure_utilities(py::module_& m) {
    // Factory functions
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        
        // Set default values for Phase 6 integration
        config->set("brain.neuron_count", 1000);
        config->set("brain.region_count", 2);
        config->set("brain.connection_probability", 0.1f);
        config->set("simulation.timestep", 0.001);
        config->set("random_seed", 42);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        return config;
    }, "Create default configuration");
    
    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) {
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize brain with configuration");
    
    m.def("createSimpleWorld", []() {
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->initialize(16, 16);
        return world;
    }, "Create simple world with default dimensions");
    
    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        return agent;
    }, py::arg("brain"), "Create agent brain interface");
    
    // Version
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";
}

PYBIND11_MODULE(pynlm, m) {
    // Module metadata
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - Complete neural simulation framework with integrated memory, prediction, cognition, and neuromodulation systems.";
    
    // Configure submodules
    configure_config(m);
    configure_brain(m);
    configure_agent(m);
    configure_world(m);
    configure_utilities(m);
    
    // Create aliases for convenience
    m.attr("createBrain") = py::cpp_function(
        [](std::shared_ptr<nlm::Config> config) {
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            return brain;
        },
        py::arg("config")
    );
    
    m.attr("createDefaultConfig") = py::cpp_function([]() {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.region_count", 2);
        config->set("brain.connection_probability", 0.1f);
        config->set("simulation.timestep", 0.001);
        config->set("random_seed", 42);
        return config;
    });
    
    m.attr("createSimpleWorld") = py::cpp_function([]() {
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->initialize(16, 16);
        return world;
    });
    
    m.attr("createAgentBrain") = py::cpp_function(
        [](std::shared_ptr<nlm::Brain> brain) {
            auto agent = std::make_shared<nlm::AgentBrain>(brain);
            return agent;
        }
    );
}