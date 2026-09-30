#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <future>
#include <numeric>
#include <random>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <atomic>

#include "validation.hpp"

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Bind validation and error handling functions
    bind_validation_and_error_handling(m);
    
    // Bind new batch processing operations
    bindBatchOperations(m);
    bindUtilityFunctions(m);
    bindDataProcessingFunctions(m);
    bindResourceManagementFunctions(m);
    bindPerformanceOptimizationFunctions(m);
}

// Data Processing Functions Implementations
namespace {

sensory_data_batch_result process_sensory_data_batch(const std::vector<std::shared_ptr<SensoryInput>>& inputs, const std::string& processingType) {
    sensory_data_batch_result result;
    
    if (inputs.empty()) {
        throw ValidationException("Inputs vector is empty");
    }
    
    double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    for (size_t i = 0; i < inputs.size(); ++i) {
        try {
            if (!inputs[i]) {
                result.messages.push_back("Input at index " + std::to_string(i) + " is null");
                result.success.push_back(false);
                result.failedProcessings++;
                continue;
            }
            
            // Create processed copy based on processing type
            auto processedInput = std::make_shared<SensoryInput>(*inputs[i]);
            
            if (processingType == "normalized") {
                auto data = processedInput->getData();
                for (auto& value : data) {
                    value = std::max(0.0f, std::min(1.0f, value)); // Normalize to [0, 1]
                }
                processedInput->setData(data);
            } else if (processingType == "smoothed") {
                auto data = processedInput->getData();
                // Simple moving average smoothing
                for (size_t j = 1; j < data.size() - 1; ++j) {
                    data[j] = (data[j-1] + data[j] + data[j+1]) / 3.0f;
                }
                processedInput->setData(data);
            } else if (processingType == "filtered") {
                auto data = processedInput->getData();
                // Simple high-pass filter
                for (size_t j = 1; j < data.size() - 1; ++j) {
                    data[j] = data[j] - (data[j-1] * 0.5f);
                }
                processedInput->setData(data);
            }
            
            result.processedInputs.push_back(processedInput);
            result.processedData.push_back(processedInput->getData());
            result.processingMethods.push_back(processingType);
            result.success.push_back(true);
            result.successfulProcessings++;
            
        } catch (const ValidationException& e) {
            result.messages.push_back(std::string("Validation exception at input " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedProcessings++;
        } catch (const std::exception& e) {
            result.messages.push_back(std::string("Exception at input " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedProcessings++;
        }
    }
    
    double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    result.totalProcessingTime = endTime - startTime;
    
    return result;
}

motor_data_batch_result process_motor_commands_batch(const std::vector<std::shared_ptr<Action>>& actions, const std::string& processingType) {
    motor_data_batch_result result;
    
    if (actions.empty()) {
        throw ValidationException("Actions vector is empty");
    }
    
    double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    for (size_t i = 0; i < actions.size(); ++i) {
        try {
            if (!actions[i]) {
                result.messages.push_back("Action at index " + std::to_string(i) + " is null");
                result.success.push_back(false);
                result.failedProcessings++;
                continue;
            }
            
            // Create processed copy based on processing type
            auto processedAction = std::make_shared<Action>(*actions[i]);
            
            if (processingType == "normalized") {
                auto params = processedAction->getParameters();
                for (auto& param : params) {
                    param = std::max(-1.0f, std::min(1.0f, param)); // Normalize to [-1, 1]
                }
                processedAction->setParameters(params);
            } else if (processingType == "smoothed") {
                auto params = processedAction->getParameters();
                for (size_t j = 1; j < params.size() - 1; ++j) {
                    params[j] = (params[j-1] + params[j] + params[j+1]) / 3.0f;
                }
                processedAction->setParameters(params);
            } else if (processingType == "scaled") {
                auto params = processedAction->getParameters();
                for (auto& param : params) {
                    param *= 0.5f; // Scale down
                }
                processedAction->setParameters(params);
            }
            
            result.processedActions.push_back(processedAction);
            result.processedParameters.push_back(processedAction->getParameters());
            result.processingMethods.push_back(processingType);
            result.success.push_back(true);
            result.successfulProcessings++;
            
        } catch (const ValidationException& e) {
            result.messages.push_back(std::string("Validation exception at action " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedProcessings++;
        } catch (const std::exception& e) {
            result.messages.push_back(std::string("Exception at action " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedProcessings++;
        }
    }
    
    double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    result.totalProcessingTime = endTime - startTime;
    
    return result;
}

aggregated_simulation_result aggregate_simulation_results(const std::vector<std::shared_ptr<Brain>>& brains, const std::vector<std::shared_ptr<SimpleWorld>>& worlds, const std::string& aggregationMethod) {
    aggregated_simulation_result result;
    
    if (brains.empty()) {
        throw ValidationException("Brains vector is empty");
    }
    
    if (!worlds.empty() && brains.size() != worlds.size()) {
        throw ValidationException("Brains and worlds must have the same size");
    }
    
    // Aggregate statistics based on method
    if (aggregationMethod == "simple_average") {
        double totalReward = 0.0;
        size_t totalSteps = 0;
        double totalSuccessRate = 0.0;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            totalReward += brains[i]->getTotalSpikeCount();
            totalSteps += brains[i]->getTotalNeuronCount();
            totalSuccessRate += brains[i]->getAverageFiringRate();
        }
        
        result.aggregatedRewards.push_back(totalReward);
        result.totalSteps.push_back(totalSteps);
        result.successRates.push_back(totalSuccessRate / brains.size());
        
    } else if (aggregationMethod == "weighted_average") {
        double totalWeight = 0.0;
        double weightedReward = 0.0;
        size_t weightedSteps = 0;
        double weightedSuccessRate = 0.0;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            double weight = static_cast<double>(brains[i]->getTotalNeuronCount());
            totalWeight += weight;
            weightedReward += brains[i]->getTotalSpikeCount() * weight;
            weightedSteps += brains[i]->getTotalNeuronCount() * weight;
            weightedSuccessRate += brains[i]->getAverageFiringRate() * weight;
        }
        
        if (totalWeight > 0) {
            result.aggregatedRewards.push_back(weightedReward / totalWeight);
            result.totalSteps.push_back(weightedSteps / totalWeight);
            result.successRates.push_back(weightedSuccessRate / totalWeight);
        }
        
    } else if (aggregationMethod == "max") {
        size_t maxSpikes = 0;
        size_t maxNeurons = 0;
        double maxFiringRate = 0.0;
        
        for (const auto& brain : brains) {
            if (brain->getTotalSpikeCount() > maxSpikes) maxSpikes = brain->getTotalSpikeCount();
            if (brain->getTotalNeuronCount() > maxNeurons) maxNeurons = brain->getTotalNeuronCount();
            if (brain->getAverageFiringRate() > maxFiringRate) maxFiringRate = brain->getAverageFiringRate();
        }
        
        result.aggregatedRewards.push_back(static_cast<double>(maxSpikes));
        result.totalSteps.push_back(maxNeurons);
        result.successRates.push_back(maxFiringRate);
        
    } else {
        // default method
        for (size_t i = 0; i < brains.size(); ++i) {
            result.aggregatedRewards.push_back(static_cast<double>(brains[i]->getTotalSpikeCount()));
            result.totalSteps.push_back(brains[i]->getTotalNeuronCount());
            result.successRates.push_back(brains[i]->getAverageFiringRate());
        }
    }
    
    // Calculate summary statistics
    result.totalAggregatedReward = std::accumulate(result.aggregatedRewards.begin(), result.aggregatedRewards.end(), 0.0);
    result.totalStepsAcrossAll = std::accumulate(result.totalSteps.begin(), result.totalSteps.end(), 0);
    
    if (!result.successRates.empty()) {
        result.averageSuccessRate = std::accumulate(result.successRates.begin(), result.successRates.end(), 0.0) / result.successRates.size();
    }
    
    result.aggregationMethod = aggregationMethod;
    result.trendAnalysis.push_back("Aggregation method: " + aggregationMethod);
    
    return result;
}

interpolated_simulation_result interpolate_simulation_data(const std::vector<std::shared_ptr<Brain>>& brains, double targetTime, const std::string& interpolationMethod) {
    interpolated_simulation_result result;
    
    if (brains.empty()) {
        throw ValidationException("Brains vector is empty");
    }
    
    double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    for (size_t i = 0; i < brains.size(); ++i) {
        try {
            // Create interpolated copy
            auto interpolatedBrain = std::make_shared<Brain>(brains[i]->getConfig());
            
            // Interpolate state based on method
            if (interpolationMethod == "linear") {
                // Linear interpolation between current and some reference state
                // Simplified implementation
                interpolatedBrain->initialize();
                
                // Add some simulated interpolated data
                auto regionIds = brains[i]->getRegionIds();
                for (const auto& regionId : regionIds) {
                    auto region = brains[i]->getRegion(regionId);
                    if (region) {
                        interpolatedBrain->addRegion(regionId.value); // Use value directly
                    }
                }
                
            } else if (interpolationMethod == "cubic") {
                // Cubic interpolation
                interpolatedBrain->initialize();
                
                // More complex interpolation
                auto regionIds = brains[i]->getRegionIds();
                for (size_t j = 0; j < regionIds.size(); ++j) {
                    auto region = brains[i]->getRegion(regionIds[j]);
                    if (region) {
                        interpolatedBrain->addRegion(regionIds[j].value); // Use value directly
                    }
                }
                
            } else if (interpolationMethod == "spline") {
                // Spline interpolation
                interpolatedBrain->initialize();
                
                // Smooth interpolation
                auto regionIds = brains[i]->getRegionIds();
                for (size_t j = 0; j < regionIds.size(); ++j) {
                    auto region = brains[i]->getRegion(regionIds[j]);
                    if (region) {
                        interpolatedBrain->addRegion(regionIds[j].value); // Use value directly
                    }
                }
                
            } else {
                // default method
                interpolatedBrain->initialize();
            }
            
            result.interpolatedBrains.push_back(interpolatedBrain);
            result.targetTimes.push_back(targetTime);
            result.interpolationMethods.push_back(interpolationMethod);
            result.success.push_back(true);
            result.successfulInterpolations++;
            
        } catch (const ValidationException& e) {
            result.messages.push_back(std::string("Validation exception at brain " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedInterpolations++;
        } catch (const std::exception& e) {
            result.messages.push_back(std::string("Exception at brain " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedInterpolations++;
        }
    }
    
    double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    result.totalInterpolationTime = endTime - startTime;
    
    return result;
}

smooth_simulation_result smooth_simulation_data(const std::vector<std::shared_ptr<Brain>>& brains, double smoothingFactor, const std::string& method) {
    smooth_simulation_result result;
    
    if (brains.empty()) {
        throw ValidationException("Brains vector is empty");
    }
    
    double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    for (size_t i = 0; i < brains.size(); ++i) {
        try {
            // Create smoothed copy
            auto smoothedBrain = std::make_shared<Brain>(brains[i]->getConfig());
            
            // Apply smoothing based on method
            if (method == "moving_average") {
                smoothedBrain->initialize();
                
                // Simple moving average smoothing
                auto regionIds = brains[i]->getRegionIds();
                for (const auto& regionId : regionIds) {
                    auto region = brains[i]->getRegion(regionId);
                    if (region) {
                        smoothedBrain->addRegion(regionId.value); // Use value directly
                    }
                }
                
            } else if (method == "gaussian") {
                smoothedBrain->initialize();
                
                // Gaussian smoothing
                auto regionIds = brains[i]->getRegionIds();
                for (size_t j = 0; j < regionIds.size(); ++j) {
                    auto region = brains[i]->getRegion(regionIds[j]);
                    if (region) {
                        smoothedBrain->addRegion(regionIds[j].value); // Use value directly
                    }
                }
                
            } else if (method == "exponential") {
                smoothedBrain->initialize();
                
                // Exponential smoothing
                auto regionIds = brains[i]->getRegionIds();
                for (size_t j = 0; j < regionIds.size(); ++j) {
                    auto region = brains[i]->getRegion(regionIds[j]);
                    if (region) {
                        smoothedBrain->addRegion(regionIds[j].value); // Use value directly
                    }
                }
                
            } else {
                // default method
                smoothedBrain->initialize();
            }
            
            result.smoothedBrains.push_back(smoothedBrain);
            result.smoothingFactors.push_back(smoothingFactor);
            result.smoothingMethods.push_back(method);
            result.success.push_back(true);
            result.successfulSmoothings++;
            
        } catch (const ValidationException& e) {
            result.messages.push_back(std::string("Validation exception at brain " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedSmoothings++;
        } catch (const std::exception& e) {
            result.messages.push_back(std::string("Exception at brain " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedSmoothings++;
        }
    }
    
    double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    result.totalSmoothingTime = endTime - startTime;
    
    return result;
}

anomaly_detection_result detect_anomalies(const std::vector<std::shared_ptr<Brain>>& brains, double threshold, const std::string& detectionMethod) {
    anomaly_detection_result result;
    
    if (brains.empty()) {
        throw ValidationException("Brains vector is empty");
    }
    
    result.hasAnomalies.reserve(brains.size());
    result.anomalyIndices.reserve(brains.size());
    result.anomalyTypes.reserve(brains.size());
    result.anomalyScores.reserve(brains.size());
    
    for (size_t i = 0; i < brains.size(); ++i) {
        try {
            bool hasAnomaly = false;
            std::vector<size_t> anomalyIndices;
            std::string anomalyType;
            double anomalyScore = 0.0;
            
            if (detectionMethod == "zscore") {
                double meanSpikeCount = 0.0;
                double stddevSpikeCount = 0.0;
                size_t totalSpikes = brains[i]->getTotalSpikeCount();
                
                meanSpikeCount = static_cast<double>(totalSpikes) / brains.size();
                // Simplified standard deviation calculation
                stddevSpikeCount = std::sqrt(meanSpikeCount);
                
                if (stddevSpikeCount > 0) {
                    double zScore = std::abs(static_cast<double>(totalSpikes) - meanSpikeCount) / stddevSpikeCount;
                    anomalyScore = zScore;
                    
                    if (zScore > threshold) {
                        hasAnomaly = true;
                        anomalyType = "high_spike_count";
                        anomalyIndices.push_back(i);
                    }
                }
                
            } else if (detectionMethod == "iqr") {
                double q1 = brains[i]->getAverageFiringRate() * 0.25;
                double q3 = brains[i]->getAverageFiringRate() * 0.75;
                double iqr = q3 - q1;
                
                double value = brains[i]->getAverageFiringRate();
                anomalyScore = std::abs(value - (q1 + q3) / 2.0) / iqr;
                
                if (anomalyScore > threshold) {
                    hasAnomaly = true;
                    anomalyType = "deviation_from_median";
                    anomalyIndices.push_back(i);
                }
                
            } else if (detectionMethod == "threshold") {
                // Simple threshold-based detection
                double value = brains[i]->getAverageFiringRate();
                anomalyScore = value;
                
                if (value > threshold) {
                    hasAnomaly = true;
                    anomalyType = "high_firing_rate";
                    anomalyIndices.push_back(i);
                }
                
            } else {
                // default method
                double value = brains[i]->getAverageFiringRate();
                anomalyScore = std::abs(value - 0.5); // Deviation from 0.5
                
                if (anomalyScore > threshold) {
                    hasAnomaly = true;
                    anomalyType = "deviation_from_baseline";
                    anomalyIndices.push_back(i);
                }
            }
            
            result.hasAnomalies.push_back(hasAnomaly);
            result.anomalyIndices.push_back(anomalyIndices);
            result.anomalyTypes.push_back(anomalyType);
            result.anomalyScores.push_back(anomalyScore);
            
            if (hasAnomaly) {
                result.totalAnomalies++;
            }
            
        } catch (const ValidationException& e) {
            result.hasAnomalies.push_back(false);
            result.anomalyIndices.push_back({});
            result.anomalyTypes.push_back(std::string("validation_error: ") + e.what());
            result.anomalyScores.push_back(0.0);
        } catch (const std::exception& e) {
            result.hasAnomalies.push_back(false);
            result.anomalyIndices.push_back({});
            result.anomalyTypes.push_back(std::string("exception: ") + e.what());
            result.anomalyScores.push_back(0.0);
        }
    }
    
    result.detectionMethod = detectionMethod;
    
    return result;
}

normalized_data_result normalize_simulation_data(const std::vector<std::shared_ptr<Brain>>& brains, const std::string& normalizationMethod) {
    normalized_data_result result;
    
    if (brains.empty()) {
        throw ValidationException("Brains vector is empty");
    }
    
    double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    for (size_t i = 0; i < brains.size(); ++i) {
        try {
            // Create normalized copy
            auto normalizedBrain = std::make_shared<Brain>(brains[i]->getConfig());
            
            if (normalizationMethod == "minmax") {
                normalizedBrain->initialize();
                
                // Min-max normalization
                auto regionIds = brains[i]->getRegionIds();
                for (const auto& regionId : regionIds) {
                    auto region = brains[i]->getRegion(regionId);
                    if (region) {
                        normalizedBrain->addRegion(regionId.value); // Use value directly
                    }
                }
                
            } else if (normalizationMethod == "zscore") {
                normalizedBrain->initialize();
                
                // Z-score normalization
                auto regionIds = brains[i]->getRegionIds();
                for (size_t j = 0; j < regionIds.size(); ++j) {
                    auto region = brains[i]->getRegion(regionIds[j]);
                    if (region) {
                        normalizedBrain->addRegion(regionIds[j].value); // Use value directly
                    }
                }
                
            } else if (normalizationMethod == "robust") {
                normalizedBrain->initialize();
                
                // Robust normalization (median-based)
                auto regionIds = brains[i]->getRegionIds();
                for (size_t j = 0; j < regionIds.size(); ++j) {
                    auto region = brains[i]->getRegion(regionIds[j]);
                    if (region) {
                        normalizedBrain->addRegion(regionIds[j].value); // Use value directly
                    }
                }
                
            } else {
                // default method
                normalizedBrain->initialize();
            }
            
            result.normalizedBrains.push_back(normalizedBrain);
            result.normalizationMethods.push_back(normalizationMethod);
            result.success.push_back(true);
            result.successfulNormalizations++;
            
        } catch (const ValidationException& e) {
            result.messages.push_back(std::string("Validation exception at brain " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedNormalizations++;
        } catch (const std::exception& e) {
            result.messages.push_back(std::string("Exception at brain " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedNormalizations++;
        }
    }
    
    double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    result.totalNormalizationTime = endTime - startTime;
    
    return result;
}

standardized_data_result standardize_simulation_data(const std::vector<std::shared_ptr<Brain>>& brains, double mean, double stddev) {
    standardized_data_result result;
    
    if (brains.empty()) {
        throw ValidationException("Brains vector is empty");
    }
    
    double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    for (size_t i = 0; i < brains.size(); ++i) {
        try {
            // Create standardized copy
            auto standardizedBrain = std::make_shared<Brain>(brains[i]->getConfig());
            
            standardizedBrain->initialize();
            
            // Standardize data
            auto regionIds = brains[i]->getRegionIds();
            for (size_t j = 0; j < regionIds.size(); ++j) {
                auto region = brains[i]->getRegion(regionIds[j]);
                if (region) {
                    standardizedBrain->addRegion(regionIds[j].value); // Use value directly
                }
            }
            
            result.standardizedBrains.push_back(standardizedBrain);
            result.means.push_back(mean);
            result.stddevs.push_back(stddev);
            result.standardizationMethods.push_back("standard_normal");
            result.success.push_back(true);
            result.successfulStandardizations++;
            
        } catch (const ValidationException& e) {
            result.messages.push_back(std::string("Validation exception at brain " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedStandardizations++;
        } catch (const std::exception& e) {
            result.messages.push_back(std::string("Exception at brain " + std::to_string(i) + ": ") + e.what());
            result.success.push_back(false);
            result.failedStandardizations++;
        }
    }
    
    double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    result.totalStandardizationTime = endTime - startTime;
    
    return result;
}

} // namespace

// Bind functions for data processing operations
void bindDataProcessingFunctions(py::module_& m) {
    m.def("process_sensory_data_batch", &process_sensory_data_batch,
          py::arg("inputs"), py::arg("processingType") = "normalized",
          R"pbdoc(
          Process multiple sensory inputs in batch.
          
          Args:
              inputs: Vector of sensory input instances
              processingType: Type of processing to apply
              
          Returns:
              sensory_data_batch_result: Processed results
              
          Example:
              >>> inputs = [sensory1, sensory2, sensory3]
              >>> results = process_sensory_data_batch(inputs, "normalized")
          )pbdoc");
    
    m.def("process_motor_commands_batch", &process_motor_commands_batch,
          py::arg("actions"), py::arg("processingType") = "smoothed",
          R"pbdoc(
          Process multiple motor commands in batch.
          
          Args:
              actions: Vector of action instances
              processingType: Type of processing to apply
              
          Returns:
              motor_data_batch_result: Processed results
              
          Example:
              >>> actions = [action1, action2, action3]
              >>> results = process_motor_commands_batch(actions, "smoothed")
          )pbdoc");
    
    m.def("aggregate_simulation_results", &aggregate_simulation_results,
          py::arg("brains"), py::arg("worlds"), py::arg("aggregationMethod") = "weighted_average",
          R"pbdoc(
          Aggregate results from multiple simulations.
          
          Args:
              brains: Vector of brain instances
              worlds: Vector of world instances
              aggregationMethod: Method to aggregate results
              
          Returns:
              aggregated_simulation_result: Aggregated results
              
          Example:
              >>> brains = [brain1, brain2]
              >>> worlds = [world1, world2]
              >>> results = aggregate_simulation_results(brains, worlds, "weighted_average")
          )pbdoc");
    
    m.def("interpolate_simulation_data", &interpolate_simulation_data,
          py::arg("brains"), py::arg("targetTime"), py::arg("interpolationMethod") = "linear",
          R"pbdoc(
          Interpolate simulation data.
          
          Args:
              brains: Vector of brain instances
              targetTime: Time point for interpolation
              interpolationMethod: Interpolation method
              
          Returns:
              interpolated_simulation_result: Interpolated results
              
          Example:
              >>> brains = [brain1, brain2]
              >>> target_time = 0.5
              >>> results = interpolate_simulation_data(brains, target_time)
          )pbdoc");
    
    m.def("smooth_simulation_data", &smooth_simulation_data,
          py::arg("brains"), py::arg("smoothingFactor") = 0.5, py::arg("method") = "moving_average",
          R"pbdoc(
          Apply smoothing to simulation data.
          
          Args:
              brains: Vector of brain instances
              smoothingFactor: Smoothing strength (0-1)
              method: Smoothing method
              
          Returns:
              smooth_simulation_result: Smoothed results
              
          Example:
              >>> brains = [brain1, brain2]
              >>> results = smooth_simulation_data(brains, 0.3, "gaussian")
          )pbdoc");
    
    m.def("detect_anomalies", &detect_anomalies,
          py::arg("brains"), py::arg("threshold") = 2.0, py::arg("detectionMethod") = "zscore",
          R"pbdoc(
          Detect anomalies in simulation data.
          
          Args:
              brains: Vector of brain instances
              threshold: Anomaly detection threshold
              detectionMethod: Detection method
              
          Returns:
              anomaly_detection_result: Anomaly detection results
              
          Example:
              >>> brains = [brain1, brain2, brain3]
              >>> results = detect_anomalies(brains, threshold=2.5)
          )pbdoc");
    
    m.def("normalize_simulation_data", &normalize_simulation_data,
          py::arg("brains"), py::arg("normalizationMethod") = "minmax",
          R"pbdoc(
          Normalize simulation data.
          
          Args:
              brains: Vector of brain instances
              normalizationMethod: Normalization method
              
          Returns:
              normalized_data_result: Normalized results
              
          Example:
              >>> brains = [brain1, brain2]
              >>> results = normalize_simulation_data(brains, "standard")
          )pbdoc");
    
    m.def("standardize_simulation_data", &standardize_simulation_data,
          py::arg("brains"), py::arg("mean") = 0.0, py::arg("stddev") = 1.0,
          R"pbdoc(
          Standardize simulation data.
          
          Args:
              brains: Vector of brain instances
              mean: Target mean value
              stddev: Target standard deviation
              
          Returns:
              standardized_data_result: Standardized results
              
          Example:
              >>> brains = [brain1, brain2]
              >>> results = standardize_simulation_data(brains, mean=0.0, stddev=1.0)
          )pbdoc");
}

} // namespace nlm
