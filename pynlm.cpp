#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Observation.hpp"
#include "sensory/Vision.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/Action.hpp"
#include <string>
#include <memory>

namespace py = pybind11;

// Forward declarations
class PyBrain;
class PyAgentBrain;
class PySimpleWorld;

// Python wrapper for Config
py::class_<nlm::Config> pybind11_config(py::module_ &m) {
    return py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("set", &nlm::Config::set, py::arg("key"), py::arg("value"))
        .def("getOr", &nlm::Config::getOr, py::arg("key"), py::arg("defaultValue"))
        .def("loadFromFile", &nlm::Config::loadFromFile, py::arg("filepath"))
        .def("loadFromArgs", &nlm::Config::loadFromArgs, py::arg("argc"), py::arg("argv"));
}

// Python wrapper for Brain
py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>> pybind11_brain(py::module_ &m) {
    return py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", &nlm::Brain::step, py::arg("currentStep"), py::arg("currentTime"))
        .def("reset", &nlm::Brain::reset)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("getWorkingMemory", &nlm::Brain::getWorkingMemory)
        .def("getEpisodicMemory", &nlm::Brain::getEpisodicMemory)
        .def("getDopamine", &nlm::Brain::getDopamine)
        .def("getCuriosity", &nlm::Brain::getCuriosity)
        .def("getPredictionSystem", &nlm::Brain::getPredictionSystem)
        .def("getPlanner", &nlm::Brain::getPlanner)
        .def("getConceptFormation", &nlm::Brain::getConceptFormation)
        .def("getAttention", &nlm::Brain::getAttention)
        .def("getDevelopmentSystem", &nlm::Brain::getDevelopmentSystem)
        .def("getNeuromodulationLevel", &nlm::Brain::getNeuromodulationLevel)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("produceAction", &nlm::Brain::produceAction)
        .def("applyRewardModulation", &nlm::Brain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &nlm::Brain::develop)
        .def("save", &nlm::Brain::save, py::arg("filepath"))
        .def("load", &nlm::Brain::load, py::arg("filepath"))
        .def("logStatus", &nlm::Brain::logStatus)
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"));
}

// Python wrapper for AgentBrain
py::class_<nlm::AgentBrain> pybind11_agentbrain(py::module_ &m) {
    return py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"))
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment)
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("reset", &nlm::AgentBrain::reset);
}

// Python wrapper for SimpleWorld
py::class_<nlm::SimpleWorld> pybind11_simpleworld(py::module_ &m) {
    return py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("initialize", &nlm::SimpleWorld::initialize,
             py::arg("width"), py::arg("height"), py::arg("visionWidth") = 8, py::arg("visionHeight") = 8)
        .def("reset", &nlm::SimpleWorld::reset)
        .def("update", &nlm::SimpleWorld::update, py::arg("time"))
        .def("observe", &nlm::SimpleWorld::observe, py::arg("brain"))
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept)
        .def("getVision", &nlm::SimpleWorld::getVision)
        .def("getTouch", &nlm::SimpleWorld::getTouch)
        .def("getInternalSignals", &nlm::SimpleWorld::getInternalSignals)
        .def("getProprioception", &nlm::SimpleWorld::getProprioception)
        .def("applyAction", &nlm::SimpleWorld::applyAction,
             py::arg("brain"), py::arg("action"))
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("action"), py::arg("time"))
        .def("computeReward", &nlm::SimpleWorld::computeReward, py::arg("brain"))
        .def("getVisionWidth", &nlm::SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &nlm::SimpleWorld::getVisionHeight)
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime);
}

// Python wrapper for SensoryPercept
py::class_<nlm::SensoryPercept> pybind11_sensorypercept(py::module_ &m) {
    return py::class_<nlm::SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("getProprioception", &nlm::SensoryPercept::getProprioception)
        .def("setVision", &nlm::SensoryPercept::setVision, py::arg("vision"))
        .def("setTouch", &nlm::SensoryPercept::setTouch, py::arg("touch"))
        .def("setInternal", &nlm::SensoryPercept::setInternal, py::arg("internal"))
        .def("setProprioception", &nlm::SensoryPercept::setProprioception, py::arg("proprioception"));
}

// Python wrapper for Action
py::class_<nlm::Action> pybind11_action(py::module_ &m) {
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("Wait", nlm::ActionType::Wait)
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("LookLeft", nlm::ActionType::LookLeft)
        .value("LookRight", nlm::ActionType::LookRight)
        .value("Interact", nlm::ActionType::Interact);
    
    return py::class_<nlm::Action>(m, "Action")
        .def(py::init<nlm::ActionType>())
        .def("getType", &nlm::Action::getType)
        .def("toString", &nlm::Action::toString);
}

// Factory functions
std::shared_ptr<nlm::Config> createDefaultConfig() {
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    return config;
}

std::shared_ptr<nlm::Brain> createBrain(std::shared_ptr<nlm::Config> config) {
    return std::make_shared<nlm::Brain>(config);
}

nlm::AgentBrain createAgentBrain(std::shared_ptr<nlm::Brain> brain) {
    return nlm::AgentBrain(brain);
}

nlm::SimpleWorld createSimpleWorld() {
    return nlm::SimpleWorld();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - An Experimental Artificial Developmental Brain";
    
    // Export main classes
    pybind11_config(m);
    pybind11_brain(m);
    pybind11_agentbrain(m);
    pybind11_simpleworld(m);
    pybind11_sensorypercept(m);
    pybind11_action(m);
    
    // Export factory functions
    m.def("createDefaultConfig", &createDefaultConfig,
          "Create a default configuration for NLM brain");
    
    m.def("createBrain", &createBrain, py::arg("config"),
          "Create a virtual brain with neurons and synapses");
    
    m.def("createAgentBrain", &createAgentBrain, py::arg("brain"),
          "Create an agent brain that connects brain to world");
    
    m.def("createSimpleWorld", &createSimpleWorld,
          "Create a simple world environment for agent testing");
    
    // Export utility constants
    m.attr("NEURON_TYPES") = py::make_dict(py::module_::import("nlm"), 
        "Sensory", nlm::NeuronType::Sensory,
        "Internal", nlm::NeuronType::Internal,
        "Motor", nlm::NeuronType::Motor);
    
    m.attr("ACTION_TYPES") = py::make_dict(py::module_::import("nlm"),
        "Wait", nlm::ActionType::Wait,
        "MoveForward", nlm::ActionType::MoveForward,
        "MoveBackward", nlm::ActionType::MoveBackward,
        "TurnLeft", nlm::ActionType::TurnLeft,
        "TurnRight", nlm::ActionType::TurnRight,
        "LookLeft", nlm::ActionType::LookLeft,
        "LookRight", nlm::ActionType::LookRight,
        "Interact", nlm::ActionType::Interact);
    
    // Export version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM neural simulation framework";
}