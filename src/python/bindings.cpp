#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/motor/Action.hpp"
#include "src/environment/Environment.hpp"
#include "src/experiments/Experiment.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM (Neural Learning Machine) - A brain-inspired spiking neural network simulator";
    m.attr("__version__") = "0.1.0";
    
    // ===== ENUMERATIONS =====
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();
    
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<nlm::LogLevel>(m, "LogLevel")
        .value("Debug", nlm::LogLevel::Debug)
        .value("Info", nlm::LogLevel::Info)
        .value("Warning", nlm::LogLevel::Warning)
        .value("Error", nlm::LogLevel::Error)
        .value("Critical", nlm::LogLevel::Critical)
        .export_values();
    
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("MoveLeft", nlm::ActionType::MoveLeft)
        .value("MoveRight", nlm::ActionType::MoveRight)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Look", nlm::ActionType::Look)
        .value("LookUp", nlm::ActionType::LookUp)
        .value("LookDown", nlm::ActionType::LookDown)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Drink", nlm::ActionType::Drink)
        .value("Rest", nlm::ActionType::Rest)
        .value("Wait", nlm::ActionType::Wait)
        .value("Custom", nlm::ActionType::Custom)
        .export_values();
    
    // Add MotorCommand enum if it's not defined
    py::enum_<nlm::MotorCommand>(m, "MotorCommand")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();
    
    // ===== CONFIGURATION SYSTEM =====
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("has", &nlm::Config::has, "Check if config has key")
        .def("get", &nlm::Config::get<nlm::ConfigValue>, "Get config value")
        .def("getOr", &nlm::Config::getOr<nlm::ConfigValue>, 
             py::arg("default_value"), "Get config value with default")
        .def("set", py::overload_cast<const std::string&, const nlm::ConfigValue&>(&nlm::Config::set),
             "Set config value")
        .def("set", py::overload_cast<const std::string&, int>(&nlm::Config::set),
             "Set config value from int")
        .def("set", py::overload_cast<const std::string&, double>(&nlm::Config::set),
             "Set config value from double")
        .def("set", py::overload_cast<const std::string&, bool>(&nlm::Config::set),
             "Set config value from bool")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&nlm::Config::set),
             "Set config value from string")
        .def("loadFromFile", &nlm::Config::loadFromFile, "Load config from file")
        .def("saveToFile", &nlm::Config::saveToFile, "Save config to file")
        .def("getKeys", &nlm::Config::getKeys, "Get all config keys")
        .def("clear", &nlm::Config::clear, "Clear all config values")
        .def("summary", &nlm::Config::summary, "Get config summary")
        .def("remove", &nlm::Config::remove, "Remove config key");
    
    // Create default config factory function
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        
        // Set Phase 2 defaults
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
        config->set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
        
        return config;
    }, "Create a default NLM configuration");
    
    // ===== BRAIN SYSTEM =====
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize, "Initialize brain")
        .def("step", py::overload_cast<size_t>(&nlm::Brain::step), "Step brain simulation")
        .def("step", py::overload_cast<size_t, double>(&nlm::Brain::step), 
             py::arg("currentStep"), py::arg("currentTime"), "Step with time")
        .def("reset", &nlm::Brain::reset, "Reset brain state")
        .def("save", &nlm::Brain::save, "Save brain state")
        .def("load", &nlm::Brain::load, "Load brain state")
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate, "Get average firing rate")
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &nlm::Brain::getPendingSpikeEventCount, "Get pending spike count")
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("produceAction", &nlm::Brain::produceAction, "Produce motor action")
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage, 
             "Set developmental stage")
        .def("getConfig", &nlm::Brain::getConfig, "Get brain config")
        .def("getWorkingMemory", &nlm::Brain::getWorkingMemory, "Get working memory")
        .def("getEpisodicMemory", &nlm::Brain::getEpisodicMemory, "Get episodic memory")
        .def("getAssociativeMemory", &nlm::Brain::getAssociativeMemory, "Get associative memory")
        .def("getPredictionSystem", &nlm::Brain::getPredictionSystem, "Get prediction system")
        .def("getPlanner", &nlm::Brain::getPlanner, "Get neural planner")
        .def("getConceptFormation", &nlm::Brain::getConceptFormation, "Get concept formation")
        .def("getAttention", &nlm::Brain::getAttention, "Get attentional selection")
        .def("getDevelopmentSystem", &nlm::Brain::getDevelopmentSystem, "Get development system")
        .def("getDopamine", &nlm::Brain::getDopamine, "Get dopamine neuromodulator")
        .def("getCuriosity", &nlm::Brain::getCuriosity, "Get curiosity neuromodulator")
        .def("getNovelty", &nlm::Brain::getNovelty, "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &nlm::Brain::getPredictionErrorSignal, "Get prediction error")
        .def("getRandomGenerator", &nlm::Brain::getRandomGenerator, "Get random generator")
        .def("logStatus", &nlm::Brain::logStatus, "Log brain status");
    
    // ===== AGENT BRAIN SYSTEM =====
    py::class_<nlm::AgentBrain, std::shared_ptr<nlm::AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("initialize", &nlm::AgentBrain::initialize, "Initialize agent with world")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput, "Process sensory input")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             "Apply reward modulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment, "Update development")
        .def("reset", &nlm::AgentBrain::reset, "Reset agent")
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
             "Enable/disable development")
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
             "Enable/disable curiosity")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError,
             "Get prediction error")
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage,
             "Get developmental stage")
        .def("getBrain", &nlm::AgentBrain::getBrain, "Get brain pointer");
    
    // ===== SIMPLE WORLD SYSTEM =====
    py::class_<nlm::SimpleWorld, std::shared_ptr<nlm::SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure,
             "Configure world parameters")
        .def("reset", &nlm::SimpleWorld::reset, "Reset world")
        .def("update", &nlm::SimpleWorld::update,
             "Update world simulation")
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
             "Get sensory percept")
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
             "Get agent body")
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             "Apply motor command")
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart,
             "Set agent start position")
        .def("getWidth", &nlm::SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &nlm::SimpleWorld::getHeight,
             "Get world height")
        .def("getVisionWidth", &nlm::SimpleWorld::getVisionWidth,
             "Get vision width")
        .def("getVisionHeight", &nlm::SimpleWorld::getVisionHeight,
             "Get vision height")
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed,
             "Set random seed")
        .def("addObject", &nlm::SimpleWorld::addObject,
             "Add object to world")
        .def("removeObject", &nlm::SimpleWorld::removeObject,
             "Remove object from world")
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition,
             "Check if position is valid");
    
    // ===== FACTORY FUNCTIONS =====
    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) {
        return std::make_shared<nlm::Brain>(config);
    }, "Create brain instance");
    
    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, "Create agent brain interface");
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create simple world simulation");
    
    // ===== UTILITY FUNCTIONS =====
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        
        // Set Phase 2 defaults
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
        
        return config;
    });
    
    // Logger utilities
    m.def("getGlobalLogger", &nlm::Logger::getGlobal, "Get global logger instance");
    m.def("setGlobalLogger", &nlm::Logger::setGlobal, "Set global logger");
    
    // Simulation constants
    m.attr("SIMULATION_VERSION") = "Phase 2.3 (Neural Computation)";
    m.attr("DEFAULT_SEED") = 42;
    m.attr("DEFAULT_TIMESTEP") = 0.001;
}
