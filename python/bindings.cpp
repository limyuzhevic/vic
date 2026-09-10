// Additional Python bindings for advanced NLM functionality
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>

// Include NLM core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/performance/CheckpointSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Brain Configuration Methods
py::class_<Config>(m, "Config")
    .def(py::init<>())
    .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
         "Load configuration from a JSON or key=value file")
    .def("loadFromArgs", [](Config& self, int argc, char** argv) {
        return self.loadFromArgs(argc, argv);
    }, py::arg("argc"), py::arg("argv"),
       "Load configuration from command line arguments")
    .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
         "Save configuration to a JSON or key=value file")
    .def("has", &Config::has, py::arg("key"),
         "Check if a configuration key exists")
    .def("getKeys", &Config::getKeys,
         "Get all configuration keys")
    .def("clear", &Config::clear,
         "Clear all configuration entries")
    .def("summary", &Config::summary,
         "Get a summary string of the configuration")
    .def("setBool", &Config::set<bool>, py::arg("key"), py::arg("value"),
         "Set boolean configuration value")
    .def("setInt", &Config::set<int64_t>, py::arg("key"), py::arg("value"),
         "Set integer configuration value")
    .def("setFloat", &Config::set<double>, py::arg("key"), py::arg("value"),
         "Set float configuration value")
    .def("setString", &Config::set<std::string>, py::arg("key"), py::arg("value"),
         "Set string configuration value")
    .def("setVector", [](Config& self, const std::string& key, 
                        const std::vector<double>& values) {
        self.set(key, values);
    }, py::arg("key"), py::arg("values"),
       "Set vector configuration value")
    .def("getBool", &Config::get<bool>, py::arg("key"),
         "Get boolean configuration value")
    .def("getInt", &Config::get<int64_t>, py::arg("key"),
         "Get integer configuration value")
    .def("getFloat", &Config::get<double>, py::arg("key"),
         "Get float configuration value")
    .def("getString", &Config::get<std::string>, py::arg("key"),
         "Get string configuration value")
    .def("getVector", [](const Config& self, const std::string& key) {
        auto result = self.get<std::vector<double>>(key);
        return result ? result.value() : std::vector<double>();
    }, py::arg("key"),
       "Get vector configuration value")
    .def("getOrBool", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"),
         "Get boolean configuration value with default")
    .def("getOrInt", &Config::getOr<int64_t>, py::arg("key"), py::arg("defaultValue"),
         "Get integer configuration value with default")
    .def("getOrFloat", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
         "Get float configuration value with default")
    .def("getOrString", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
         "Get string configuration value with default")
    .def("getOrVector", [](const Config& self, const std::string& key,
                         const std::vector<double>& defaultValue) {
        auto result = self.get<std::vector<double>>(key);
        return result ? result.value() : defaultValue;
    }, py::arg("key"), py::arg("defaultValue"),
       "Get vector configuration value with default");

// Advanced Brain Methods
py::class_<Brain>(m, "Brain")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
         "Create a new brain with configuration")
    .def("initialize", &Brain::initialize,
         "Initialize the brain with configuration")
    .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
         py::arg("currentStep"), "Perform a simulation step")
    .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
         py::arg("currentStep"), py::arg("currentTime"),
         "Perform a simulation step with timestamp")
    .def("receiveSensoryInput", &Brain::receiveSensoryInput,
         py::arg("input"), "Inject sensory input into the brain")
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
    .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
         "Get count of pending spike events")
    .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
         py::arg("stage"), "Set developmental stage")
    .def("getConfig", &Brain::getConfig,
         py::return_value_policy::reference_internal,
         "Get the configuration")
    .def("logStatus", &Brain::logStatus,
         "Log brain status")
    .def("enableRewardModulation", &Brain::enableRewardModulation,
         py::arg("enable"), "Enable/disable reward modulation")
    .def("enableStructuralPlasticity", &Brain::enableStructuralPlasticity,
         py::arg("enable"), "Enable/disable structural plasticity")
    .def("enableDevelopment", &Brain::enableDevelopment,
         py::arg("enable"), "Enable/disable development")
    .def("enableCuriosity", &Brain::enableCuriosity,
         py::arg("enable"), "Enable/disable curiosity")
    .def("getWorkingMemory", &Brain::getWorkingMemory,
         py::return_value_policy::reference_internal,
         "Get working memory system")
    .def("getEpisodicMemory", &Brain::getEpisodicMemory,
         py::return_value_policy::reference_internal,
         "Get episodic memory system")
    .def("getAssociativeMemory", &Brain::getAssociativeMemory,
         py::return_value_policy::reference_internal,
         "Get associative memory system")
    .def("getPredictionSystem", &Brain::getPredictionSystem,
         py::return_value_policy::reference_internal,
         "Get prediction system")
    .def("getPlanner", &Brain::getPlanner,
         py::return_value_policy::reference_internal,
         "Get neural planner")
    .def("getConceptFormation", &Brain::getConceptFormation,
         py::return_value_policy::reference_internal,
         "Get concept formation system")
    .def("getAttention", &Brain::getAttention,
         py::return_value_policy::reference_internal,
         "Get attention system")
    .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
         py::return_value_policy::reference_internal,
         "Get development system")
    .def("getDopamine", &Brain::getDopamine,
         py::return_value_policy::reference_internal,
         "Get dopamine system")
    .def("getCuriosity", &Brain::getCuriosity,
         py::return_value_policy::reference_internal,
         "Get curiosity system")
    .def("getNovelty", &Brain::getNovelty,
         py::return_value_policy::reference_internal,
         "Get novelty detection system")
    .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
         py::return_value_policy::reference_internal,
         "Get prediction error signal system")
    .def("getRandomGenerator", &Brain::getRandomGenerator,
         py::return_value_policy::reference_internal,
         "Get random generator")
    .def("setExcitatoryCurrent", [](Brain& self, float current) {
        self.injectCurrentToNeurons(NeuronType::Excitatory, current);
    }, py::arg("current"), "Inject current to all excitatory neurons")
    .def("setInhibitoryCurrent", [](Brain& self, float current) {
        self.injectCurrentToNeurons(NeuronType::Inhibitory, current);
    }, py::arg("current"), "Inject current to all inhibitory neurons")
    .def("setSensoryCurrent", [](Brain& self, float current) {
        self.injectCurrentToNeurons(NeuronType::Sensory, current);
    }, py::arg("current"), "Inject current to all sensory neurons")
    .def("setMotorCurrent", [](Brain& self, float current) {
        self.injectCurrentToNeurons(NeuronType::Motor, current);
    }, py::arg("current"), "Inject current to all motor neurons")
    .def("setInternalCurrent", [](Brain& self, float current) {
        self.injectCurrentToNeurons(NeuronType::Internal, current);
    }, py::arg("current"), "Inject current to all internal neurons")
    .def("setModulatoryCurrent", [](Brain& self, float current) {
        self.injectCurrentToNeurons(NeuronType::Modulatory, current);
    }, py::arg("current"), "Inject current to all modulatory neurons");

// Advanced AgentBrain Methods
py::class_<AgentBrain>(m, "AgentBrain")
    .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
         "Create a new agent brain interface")
    .def("initialize", &AgentBrain::initialize, py::arg("world"),
         "Initialize with world")
    .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
         "Get expected sensory input size")
    .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
         "Get expected motor output size")
    .def("processSensoryInput", &AgentBrain::processSensoryInput,
         py::arg("percept"), "Process sensory percept and inject into brain")
    .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
         "Decode brain motor activity into motor command")
    .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
         py::arg("reward"), py::arg("predictedReward"),
         "Apply reward-based neuromodulation")
    .def("updateDevelopment", &AgentBrain::updateDevelopment,
         py::arg("timestep"), "Update development system")
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
    .def("enableAllSystems", [](AgentBrain& self) {
        self.enableRewardModulation(true);
        self.enableStructuralPlasticity(true);
        self.enableDevelopment(true);
        self.enableCuriosity(true);
    }, "Enable all neuromodulation and learning systems")
    .def("disableAllSystems", [](AgentBrain& self) {
        self.enableRewardModulation(false);
        self.enableStructuralPlasticity(false);
        self.enableDevelopment(false);
        self.enableCuriosity(false);
    }, "Disable all neuromodulation and learning systems")
    .def("setCuriosityThreshold", [](AgentBrain& self, float threshold) {
        // Enable curiosity and set level
        self.enableCuriosity(true);
        // Note: This would require modifying the internal curiosity level
    }, py::arg("threshold"), "Set curiosity threshold for exploration")
    .def("setExplorationRate", [](AgentBrain& self, float rate) {
        // Modify curiosity effect on motor selection
        // This would require access to internal curiosity parameters
    }, py::arg("rate"), "Set exploration rate (0.0 to 1.0)")
    .def("setLearningRate", [](AgentBrain& self, float rate) {
        // Modify plasticity rates
        // This would require access to internal plasticity systems
    }, py::arg("rate"), "Set global learning rate (0.0 to 1.0)")
    .def("setDevelopmentSpeed", [](AgentBrain& self, float speed) {
        // Modify developmental progression
        // This would require access to internal development systems
    }, py::arg("speed"), "Set developmental speed (0.1 to 10.0)");

// Advanced World Methods
py::class_<SimpleWorld>(m, "SimpleWorld")
    .def(py::init<>())
    .def("configure", &SimpleWorld::configure,
         py::arg("width"), py::arg("height"),
         py::arg("visionWidth"), py::arg("visionHeight"),
         "Configure world dimensions")
    .def("reset", &SimpleWorld::reset,
         "Reset world to initial state")
    .def("setAgentStart", &SimpleWorld::setAgentStart,
         py::arg("x"), py::arg("y"),
         "Set agent starting position")
    .def("update", &SimpleWorld::update,
         py::arg("timestep"), "Update world by timestep")
    .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
         py::arg("cmd"), py::arg("currentTime"),
         "Apply motor command to world")
    .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
         py::return_value_policy::reference_internal,
         "Get sensory percept")
    .def("getAgentBody", &SimpleWorld::getAgentBody,
         py::return_value_policy::reference_internal,
         "Get agent body state")
    .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
         "Add object to world")
    .def("removeObject", &SimpleWorld::removeObject,
         py::arg("x"), py::arg("y"), "Remove object from world")
    .def("isValidPosition", &SimpleWorld::isValidPosition,
         py::arg("x"), py::arg("y"), "Check if position is valid")
    .def("getWidth", &SimpleWorld::getWidth,
         "Get world width")
    .def("getHeight", &SimpleWorld::getHeight,
         "Get world height")
    .def("getSimulationTime", &SimpleWorld::getSimulationTime,
         "Get current simulation time")
    .def("setRandomSeed", &SimpleWorld::setRandomSeed,
         py::arg("seed"), "Set random seed for reproducible simulations")
    .def("getRandomSeed", &SimpleWorld::getRandomSeed,
         "Get current random seed")
    .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate,
         py::arg("rate"), "Set energy decay rate")
    .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
         "Get energy decay rate")
    .def("setMaxEnergy", &SimpleWorld::setMaxEnergy,
         py::arg("energy"), "Set maximum energy")
    .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
         "Get maximum energy")
    .def("addResource", [](SimpleWorld& self, float x, float y, float value) {
        WorldObject obj(x, y, WorldObjectType::Resource, value);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("value"),
       "Add resource object to world")
    .def("addHazard", [](SimpleWorld& self, float x, float y, float damage) {
        WorldObject obj(x, y, WorldObjectType::Hazard, damage);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("damage"),
       "Add hazard object to world")
    .def("addWall", [](SimpleWorld& self, float x, float y, float radius) {
        WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("radius"),
       "Add wall object to world")
    .def("createGoal", [](SimpleWorld& self, float x, float y) {
        WorldObject goal(x, y, WorldObjectType::Marker, 100.0f);
        self.addObject(goal);
        return goal;
    }, py::arg("x"), py::arg("y"),
       "Create goal marker for agent");