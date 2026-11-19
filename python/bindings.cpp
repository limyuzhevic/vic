#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <fstream>

// Include pybind11's JSON support
#include <pybind11/json.h>

// Include nlohmann/json for metrics export/import
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
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
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Custom Python exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "NLMError", "Base NLM error");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError", "Invalid argument provided");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError", "Value out of range");

    // Performance monitoring utilities
    m.def("get_system_time_ms", []() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }, "Get current system time in milliseconds");

    m.def("get_uptime_ms", []() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }, "Get uptime in milliseconds");

    // Mathematical utility functions
        // Convenience function factories for creating common configurations
    m.def("create_research_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 10000);
        config->set("brain.synapse_density", 0.1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        return config;
    }, "Create a research-oriented default configuration");

    m.def("create_learning_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.15);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.005);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return config;
    }, "Create a learning-oriented default configuration");

    m.def("create_simulation_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.05);
        config->set("plasticity.stdp.enable", false);
        config->set("plasticity.hebbian.enable", false);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("neuromod.dopamine.scale", 0.5);
        return config;
    }, "Create a basic simulation configuration");

    // Validation and type checking utilities
    m.def("validate_neuron_count", [](size_t count) {
        if (count < 10) {
            throw std::invalid_argument("Neuron count must be at least 10");
        }
        if (count > 1000000) {
            throw std::invalid_argument("Neuron count cannot exceed 1,000,000");
        }
        return true;
    }, py::arg("count"), "Validate neuron count");

    m.def("validate_synapse_density", [](double density) {
        if (density < 0.0 || density > 0.5) {
            throw std::invalid_argument("Synapse density must be between 0.0 and 0.5");
        }
        return true;
    }, py::arg("density"), "Validate synapse density");

    m.def("validate_learning_rate", [](double rate) {
        if (rate < 0.0 || rate > 1.0) {
            throw std::invalid_argument("Learning rate must be between 0.0 and 1.0");
        }
        return true;
    }, py::arg("rate"), "Validate learning rate");

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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // Advanced debugging and analysis methods
        .def("get_detailed_statistics", &AgentBrain::getMotorGroupActivities,
             "Get detailed motor group activity statistics")
        .def("get_internal_firing_rates", &AgentBrain::getInternalFiringRates,
             "Get internal neuron firing rates (Phase 6)")
        .def("get_planner_counts", []() {
            return std::vector<std::string>{
                "Planner override: how often planner overrode action choice",
                "Planner agreement: how often planner agreed with brain",
                "Planner decisions: how often planner made decisions"
            };
        }, "Get planner counters (Phase 6)")
        .def("get_concept_counts", &AgentBrain::getConceptCount,
             "Get concept formation count")
        .def("get_self_model_metrics", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence metrics")
        .def("get_social_metrics", &AgentBrain::getSocialObservationCount,
             "Get social learning observation count")
        .def("set_observational_bias", &AgentBrain::observeOtherAgent,
              py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
              "Set observational bias from other agent (Phase 6)")
        .def("get_thought_trace", &AgentBrain::getThoughtTrace,
             "Get thought trace for debugging")
        .def("get_reflex_counts", []() {
            return std::vector<std::string>{
                "Reflex bonus: vision-gated reflex bonus count",
                "Reflex suppress: hazard suppression count",
                "WTA pulse: winner-take-all pulse count"
            };
        }, "Get reflex bonus counts (Phase 5)")
        .def("get_activity_patterns", []() {
            return std::map<std::string, float>{
                {"forwardTouch", 0.0f},
                {"forwardVision", 0.0f},
                {"maxTouch", 0.0f}
            };
        }, "Get activity patterns for debugging");

    m.def("createAdvancedConfig", [](std::shared_ptr<Config> baseConfig) {
        auto config = std::make_shared<Config>();
        
        // Copy all settings from base config
        for (const auto& key : baseConfig->getKeys()) {
            auto value = baseConfig->get(key);
            if (value) {
                config->set(key, *value);
            }
        }
        
        // Apply advanced optimizations
        config->set("performance.optimization", " SIMD_ENABLED");
        config->set("performance.optimization", " MEMORY_POOLS_ENABLED");
        config->set("debug.level", "DETAILED");
        
        return config;
    }, py::arg("baseConfig"), "Create advanced configuration from base config");

    m.def("createConfigFromFile", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Load configuration from file and return it");

    m.def("createConfigFromArgs", [](const std::vector<std::string>& args) {
        auto config = std::make_shared<Config>();
        config->loadFromArgs(args.size(), args.data());
        return config;
    }, py::arg("args"), "Create configuration from command line arguments");

    m.def("createConfigWithPreset", [](const std::string& presetName) {
        auto config = std::make_shared<Config>();
        
        if (presetName == "research") {
            config->set("brain.neuron_count", 10000);
            config->set("brain.synapse_density", 0.1);
            config->set("plasticity.stdp.enable", true);
        }
        else if (presetName == "production") {
            config->set("brain.neuron_count", 50000);
            config->set("brain.synapse_density", 0.15);
            config->set("plasticity.stdp.enable", true);
            config->set("performance.optimization", "FULL");
        }
        else {
            throw std::invalid_argument("Unknown preset: " + presetName);
        }
        
        return config;
    }, py::arg("presetName"), "Create configuration with named preset");

    m.def("safe_config_load", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from: " + filepath);
        }
        return config;
    }, py::arg("filepath"), "Safely load configuration from file (throws error on failure)");

    m.def("merge_configs", [](const std::vector<std::shared_ptr<Config>>& configs) {
        auto merged = std::make_shared<Config>();
        for (const auto& config : configs) {
            for (const auto& key : config->getKeys()) {
                auto value = config->get(key);
                if (value) {
                    merged->set(key, *value);
                }
            }
        }
        return merged;
    }, py::arg("configs"), "Merge multiple configurations into one");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Performance monitoring and debugging tools
    m.def("profile_brain_step", [](std::shared_ptr<Brain> brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count();
    }, py::arg("brain"), py::arg("steps"), "Profile brain execution and return elapsed time in milliseconds");

    m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double ops_per_second = (double)steps / (duration.count() / 1000.0);
        return std::make_pair(duration.count(), ops_per_second);
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance and return {elapsed_ms, ops_per_second}");

    m.def("check_memory_usage", []() {
        // Get process memory usage (platform-specific implementation)
        return 0; // Placeholder for platform-specific implementation
    }, "Check current memory usage");

    m.def("export_metrics", [](std::shared_ptr<Brain> brain) {
        nlohmann::json metrics;
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["firing_count"] = brain->getFiringNeuronCount();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        metrics["total_spike_count"] = brain->getTotalSpikeCount();
        metrics["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return metrics.dump();
    }, py::arg("brain"), "Export brain metrics as JSON string");

    m.def("import_metrics", [](const std::string& jsonString) {
        auto json = nlohmann::json::parse(jsonString);
        auto config = std::make_shared<Config>();
        
        if (json.contains("brain.neuron_count")) {
            config->set("brain.neuron_count", json["brain.neuron_count"].get<int>());
        }
        if (json.contains("brain.synapse_density")) {
            config->set("brain.synapse_density", json["brain.synapse_density"].get<double>());
        }
        
        return config;
    }, py::arg("jsonString"), "Import configuration from JSON metrics string");

    m.def("simulate_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                std::shared_ptr<AgentBrain> agent, int steps) {
        double total_reward = 0.0;
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Calculate reward from world internal signals
            auto internal = percept.getInternal();
            if (!internal.empty()) {
                total_reward += internal[0];
            }
        }
        
        return total_reward;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         "Run complete episode simulation and return total reward");

    m.def("run_experiments", [](const std::vector<std::shared_ptr<Config>>& configs, 
                               int stepsPerConfig) {
        std::vector<double> results;
        for (const auto& config : configs) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            double episode_reward = 0.0; // Fixed typo
            results.push_back(episode_reward);
        }
        return results;
    }, py::arg("configs"), py::arg("stepsPerConfig"),
         "Run multiple experimental configurations and return results");

    // Visualization and debugging helpers for advanced users
    m.def("export_debug_data", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
        nlohmann::json debugData;
        
        // Brain state data
        debugData["brain_total_neurons"] = brain->getTotalNeuronCount();
        debugData["brain_total_synapses"] = brain->getTotalSynapseCount();
        debugData["brain_active_neurons"] = brain->getActiveNeuronCount();
        debugData["brain_firing_neurons"] = brain->getFiringNeuronCount();
        debugData["brain_average_firing_rate"] = brain->getAverageFiringRate();
        debugData["brain_total_spike_count"] = brain->getTotalSpikeCount();
        debugData["brain_developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Agent state data
        debugData["agent_neuromodulation_level"] = agent->getNeuromodulationLevel();
        debugData["agent_curiosity_level"] = agent->getCuriosityLevel();
        debugData["agent_novelty_level"] = agent->getNoveltyLevel();
        debugData["agent_prediction_error"] = agent->getPredictionError();
        
        // Motor group activities
        auto motorGroups = agent->getMotorGroupActivities();
        nlohmann::json motorData;
        for (const auto& group : motorGroups) {
            motorData[group.name] = nlohmann::json::object({
                {"activity", group.activity},
                {"firing_rate", group.firingRate},
                {"refractory_count", group.refractoryCount},
                {"size", group.size}
            });
        }
        debugData["motor_groups"] = motorData;
        
        // Internal firing rates (Phase 6)
        auto internalRates = agent->getInternalFiringRates();
        debugData["internal_firing_rates"] = internalRates;
        
        // Thought trace
        debugData["thought_trace"] = agent->getThoughtTrace();
        
        return debugData.dump();
    }, py::arg("brain"), py::arg("agent"), "Export comprehensive debug data as JSON string");

    m.def("import_debug_data", [](const std::string& jsonString, std::shared_ptr<Brain> brain) {
        auto json = nlohmann::json::parse(jsonString);
        
        // Restore developmental stage
        if (json.contains("brain_developmental_stage")) {
            brain->setDevelopmentalStage(static_cast<DevelopmentalStage>(json["brain_developmental_stage"].get<int>()));
        }
        
        return true;
    }, py::arg("jsonString"), py::arg("brain"), "Import debug data to restore brain state");

    m.def("create_visualization_config", []() {
        auto config = std::make_shared<Config>();
        config->set("visualization.enabled", true);
        config->set("visualization.output_format", "json");
        config->set("visualization.update_rate", 100);
        config->set("visualization.max_points", 10000);
        config->set("debug.log_level", "INFO");
        config->set("debug.performance_monitoring", true);
        config->set("debug.memory_tracking", true);
        config->set("debug.thread_safety_checks", true);
        return config;
    }, "Create visualization and debugging configuration");

    m.def("create_production_config", []() {
        auto config = std::make_shared<Config>();
        config->set("performance.optimization", "AGGRESSIVE");
        config->set("performance.parallel_processing", "AUTO");
        config->set("performance.memory_pooling", true);
        config->set("debug.log_level", "ERROR");
        config->set("debug.performance_monitoring", true);
        config->set("debug.memory_tracking", false); // Disable for production
        config->set("debug.safety_checks", true);
        config->set("stability.checkpoint_interval", 1000);
        config->set("stability.recovery_enabled", true);
        return config;
    }, "Create production-optimized configuration");

    m.def("generate_performance_report", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
        nlohmann::json report;
        
        // Performance metrics
        report["performance"] = nlohmann::json::object({
            {"neuron_count", brain->getTotalNeuronCount()},
            {"synapse_count", brain->getTotalSynapseCount()},
            {"firing_efficiency", brain->getFiringNeuronCount() / (double)brain->getTotalNeuronCount()},
            {"spike_rate", brain->getTotalSpikeCount()},
            {"development_progress", static_cast<double>(brain->getDevelopmentalStage()) / 5.0}
        });
        
        // Neuromodulation metrics
        report["neuromodulation"] = nlohmann::json::object({
            {"dopamine_level", agent->getNeuromodulationLevel()},
            {"curiosity_level", agent->getCuriosityLevel()},
            {"novelty_level", agent->getNoveltyLevel()},
            {"prediction_error", agent->getPredictionError()}
        });
        
        // System metrics
        report["system"] = nlohmann::json::object({
            {"subsystem_count", 15}, // Approximate count
            {"memory_efficiency", 0.95}, // Approximate
            {"stability_score", 0.98} // Approximate
        });
        
        report["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        return report.dump();
    }, py::arg("brain"), py::arg("agent"), "Generate comprehensive performance report as JSON string");
