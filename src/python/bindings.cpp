#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/MotorSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/WorkingMemory.hpp"
#include "../memory/EpisodicMemory.hpp"
#include "../world/WorldObject.hpp"
#include <string>
#include <vector>

namespace py = pybind11;

// Helper functions for module initialization

// Config wrapper functions
void bindConfig(py::module_ &m) {
    py::class_<Config, std::shared_ptr<Config>>(m, "Config")
        .def(py::init<>())
        .def("has", &Config::has, "Check if config has key")
        .def("set", &Config::set, "Set config value", py::arg("key"), py::arg("value"))
        .def("getOr", py::overload_cast<const std::string&, const int64_t&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("getOr", py::overload_cast<const std::string&, const double&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("getOr", py::overload_cast<const std::string&, const float&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("loadFromFile", &Config::loadFromFile, "Load config from file")
        .def("saveToFile", &Config::saveToFile, "Save config to file");
}

// Brain wrapper functions
void bindBrain(py::module_ &m) {
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain")
        .def("step", py::overload_cast<uint64_t, double>(&Brain::step),
             py::arg("step"), py::arg("time"))
        .def("step", py::overload_cast<uint64_t>(&Brain::step),
             py::arg("step"))
        .def("reset", &Brain::reset, "Reset brain state")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation-inhibition ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getSimulationClock", &Brain::getSimulationClock, "Get simulation clock")
        .def("getRandomGenerator", &Brain::getRandomGenerator, "Get random generator")
        .def("save", py::overload_cast<const std::string&>(&Brain::save),
             py::arg("filepath"), "Save brain state to file")
        .def("load", py::overload_cast<const std::string&>(&Brain::load),
             py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, "Add a region", py::arg("name"))
        .def("getRegion", &Brain::getRegion, "Get region by ID", py::arg("region_id"))
        .def("getRegions", &Brain::getRegions, "Get all regions")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("getWorkingMemory", &Brain::getWorkingMemory, "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, "Get episodic memory")
        .def("getPredictionSystem", &Brain::getPredictionSystem, "Get prediction system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, "Get development system")
        .def("getNeuromodulationSystem", &Brain::getNeuromodulationSystem,
             "Get neuromodulation system")
        .def("getPlanner", &Brain::getPlanner, "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation, "Get concept formation")
        .def("getAttention", &Brain::getAttention, "Get attention system")
        .def("getDopamine", &Brain::getDopamine, "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity, "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty, "Get novelty system")
        .def("getPredictionError", &Brain::getPredictionError, "Get prediction error")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             "Inject current to neurons", py::arg("neuron_type"), py::arg("current"))
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             "Set developmental stage", py::arg("stage"));
}

// AgentBrain wrapper functions
void bindAgentBrain(py::module_ &m) {
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, "Initialize agent brain",
             py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             "Process sensory input", py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             "Apply reward modulation", py::arg("reward"), py::arg("predicted_reward"))
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             "Update development", py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             "Enable reward modulation", py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             "Enable structural plasticity", py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             "Enable development", py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             "Enable curiosity", py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity enabled")
        .def("getBrain", &AgentBrain::getBrain, "Get brain pointer");
}

// SimpleWorld wrapper functions
void bindSimpleWorld(py::module_ &m) {
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             "Configure world", py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("initialize", &SimpleWorld::initialize,
             "Initialize world", py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &SimpleWorld::reset, "Reset world")
        .def("update", py::overload_cast<double>(&SimpleWorld::update),
             "Update world", py::arg("timestep"))
        .def("update", py::overload_cast<SimulationStep, double>(&SimpleWorld::update),
             "Update world", py::arg("step"), py::arg("timestep"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody, "Get agent body")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             "Apply motor command", py::arg("command"), py::arg("current_time"))
        .def("getVisionWidth", &SimpleWorld::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SimpleWorld::getVisionHeight, "Get vision height")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             "Set agent start position", py::arg("x"), py::arg("y"))
        .def("addObject", &SimpleWorld::addObject,
             "Add world object", py::arg("object"))
        .def("removeObject", &SimpleWorld::removeObject,
             "Remove world object", py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             "Check valid position", py::arg("x"), py::arg("y"))
        .def("getObject", &SimpleWorld::getObject, "Get object at position",
             py::arg("x"), py::arg("y"))
        .def("getRandomGenerator", &SimpleWorld::getRandomGenerator,
             "Get random generator")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed,
             "Set random seed", py::arg("seed"))
        .def("computeReward", &SimpleWorld::computeReward,
             "Compute reward", py::arg("brain"))
        .def("observe", &SimpleWorld::observe,
             "Observe world from brain", py::arg("brain"));
}

// MotorCommand enum
void bindMotorCommand(py::module_ &m) {
    py::enum_<MotorCommand>(m, "MotorCommand", "Motor command types")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
}

// NeuronType enum
void bindNeuronType(py::module_ &m) {
    py::enum_<NeuronType>(m, "NeuronType", "Neuron types")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Internal", NeuronType::Internal)
        .export_values();
}

// DevelopmentalStage enum
void bindDevelopmentalStage(py::module_ &m) {
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stages")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
}

// WorldObjectType enum
void bindWorldObjectType(py::module_ &m) {
    py::enum_<WorldObjectType>(m, "WorldObjectType", "World object types")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
}

// WorldObject struct
void bindWorldObject(py::module_ &m) {
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>(), "Default constructor")
        .def(py::init<double, double, WorldObjectType>(),
             py::arg("x"), py::arg("y"), py::arg("type"))
        .def(py::init<double, double, WorldObjectType, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value"))
        .def("getX", &WorldObject::getX, "Get X coordinate")
        .def("getY", &WorldObject::getY, "Get Y coordinate")
        .def("getType", &WorldObject::getType, "Get object type")
        .def("getValue", &WorldObject::getValue, "Get object value")
        .def("setValue", &WorldObject::setValue, "Set object value",
             py::arg("value"));
}

// Helper functions for module initialization
void createDefaultConfig(py::module_ &m) {
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<Config>();
        // Set default values as in Phase 2
        config->set("random_seed", 42);
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        return config;
    }, "Create default configuration");
}

void createBrain(py::module_ &m) {
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create and initialize a brain", py::arg("config"));
}

void createAgentBrain(py::module_ &m) {
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, "Create agent brain interface", py::arg("brain"));
}

void createSimpleWorld(py::module_ &m) {
    m.def("createSimpleWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        // Configure with reasonable defaults
        world->initialize(20, 20, 8, 8);
        return world;
    }, "Create a simple world with default settings");
}

PYBIND11_MODULE(pynlm, m) {
    // Set module version
    m.doc() = "Python bindings for NLM (Neural Learning Machine)";
    m.attr("__version__") = "0.1.0";
    
    // Bind enums
    bindMotorCommand(m);
    bindNeuronType(m);
    bindDevelopmentalStage(m);
    bindWorldObjectType(m);
    
    // Bind main classes
    bindConfig(m);
    bindBrain(m);
    bindAgentBrain(m);
    bindSimpleWorld(m);
    
    // Bind WorldObject struct
    bindWorldObject(m);
    
    // Create helper functions
    createDefaultConfig(m);
    createBrain(m);
    createAgentBrain(m);
    createSimpleWorld(m);
    
    // Create convenience aliases for backward compatibility
    m.def("createDefaultConfig", createDefaultConfig,
          "Create default configuration (alias)");
    m.def("createBrain", createBrain,
          "Create and initialize brain (alias)");
    m.def("createAgentBrain", createAgentBrain,
          "Create agent brain (alias)");
    m.def("createSimpleWorld", createSimpleWorld,
          "Create simple world (alias)");
}
