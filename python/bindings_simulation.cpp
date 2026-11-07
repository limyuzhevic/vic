#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;
namespace nlm {

py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>> simpleWorldClass(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc");
simpleWorldClass
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
    .def("getWidth", &SimpleWorld::getWidth)
    .def("getHeight", &SimpleWorld::getHeight)
    .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
    .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
    .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
    .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
    .def("getSimulationTime", &SimpleWorld::getSimulationTime)
    .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
    .def("getRandomSeed", &SimpleWorld::getRandomSeed);

py::class_<Brain, std::shared_ptr<Brain>> brainClass(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc");
brainClass
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
    // getRegion is const/non-const overloaded; bind the mutable one.
    .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
         py::arg("id"),
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

py::class_<AgentBrain, std::shared_ptr<AgentBrain>> agentBrainClass(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc");
agentBrainClass
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
}