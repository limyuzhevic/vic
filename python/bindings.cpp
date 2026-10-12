#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

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
             "Load configuration from a file (JSON/YAML format auto-detected)")
        .def("loadFromString", &Config::loadFromString, py::arg("data"),
             "Load configuration from a string (JSON/YAML format auto-detected)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file (JSON/YAML format)")
        .def("loadFromJSONFile", &Config::loadFromJSONFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromJSONString", &Config::loadFromJSONString, py::arg("jsonString"),
             "Load configuration from a JSON string")
        .def("loadFromYAMLFile", &Config::loadFromYAMLFile, py::arg("filepath"),
             "Load configuration from a YAML file")
        .def("loadFromYAMLString", &Config::loadFromYAMLString, py::arg("yamlString"),
             "Load configuration from a YAML string")
        .def("toJSON", &Config::toJSON,
             "Convert configuration to JSON string")
        .def("toYAML", &Config::toYAML,
             "Convert configuration to YAML string")
        .def("toJSONFile", &Config::toJSONFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("toYAMLFile", &Config::toYAMLFile, py::arg("filepath"),
             "Save configuration to a YAML file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getEntries", &Config::getEntries,
             "Get all configuration entries")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("exportJSON", &Config::exportJSON,
             "Export configuration as JSON string")
        .def("exportYAML", &Config::exportYAML,
             "Export configuration as YAML string")
        .def("getParserErrors", &Config::getParserErrors,
             "Get configuration parser errors")
        .def("clearParserErrors", &Config::clearParserErrors,
             "Clear configuration parser errors")
        .def("compareJSON", &Config::compareJSON, py::arg("jsonString"),
             "Compare configuration with JSON string")
        .def("diffJSON", &Config::diffJSON, py::arg("jsonString"),
             "Find differences between configuration and JSON string")
        .def("getJSONSchema", &Config::getJSONSchema,
             "Get JSON schema for configuration")
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
    
    // Add bindings for ColorMap enum
    py::enum_<nlm::ColorMap>(m, "ColorMap", R"pbdoc(Color map types for visualization)pbdoc")
        .value("Default", nlm::ColorMap::Default)
        .value("Hot", nlm::ColorMap::Hot)
        .value("Cool", nlm::ColorMap::Cool)
        .value("Rainbow", nlm::ColorMap::Rainbow)
        .value("Viridis", nlm::ColorMap::Viridis)
        .value("Plasma", nlm::ColorMap::Plasma)
        .value("Inferno", nlm::ColorMap::Inferno)
        .value("Magma", nlm::ColorMap::Magma)
        .value("Cividis", nlm::ColorMap::Cividis)
        .value("Grayscale", nlm::ColorMap::Grayscale)
        .value("RedGreenBlue", nlm::ColorMap::RedGreenBlue)
        .value("Thermal", nlm::ColorMap::Thermal)
        .export_values();

    // Add bindings for VisualizationMode enum
    py::enum_<nlm::VisualizationMode>(m, "VisualizationMode", R"pbdoc(Visualization mode types)pbdoc")
        .value("NeuronView", nlm::VisualizationMode::NeuronView)
        .value("SynapseView", nlm::VisualizationMode::SynapseView)
        .value("PopulationView", nlm::VisualizationMode::PopulationView)
        .value("NetworkView", nlm::VisualizationMode::NetworkView)
        .value("MemoryView", nlm::VisualizationMode::MemoryView)
        .value("CognitionView", nlm::VisualizationMode::CognitionView)
        .value("NeuromodulationView", nlm::VisualizationMode::NeuromodulationView)
        .value("DevelopmentView", nlm::VisualizationMode::DevelopmentView)
        .export_values();

    // Add bindings for CameraSettings struct
    py::class_<nlm::CameraSettings>(m, "CameraSettings", R"pbdoc(Camera settings for visualization)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &nlm::CameraSettings::x)
        .def_readwrite("y", &nlm::CameraSettings::y)
        .def_readwrite("scale", &nlm::CameraSettings::scale)
        .def_readwrite("rotation", &nlm::CameraSettings::rotation)
        .def_readwrite("zoom", &nlm::CameraSettings::zoom);

    // Add bindings for RenderingSettings struct
    py::class_<nlm::RenderingSettings>(m, "RenderingSettings", R"pbdoc(Rendering settings for visualization)pbdoc")
        .def(py::init<>())
        .def_readwrite("enableLighting", &nlm::RenderingSettings::enableLighting)
        .def_readwrite("enableShading", &nlm::RenderingSettings::enableShading)
        .def_readwrite("enableAntiAliasing", &nlm::RenderingSettings::enableAntiAliasing)
        .def_readwrite("enableLOD", &nlm::RenderingSettings::enableLOD)
        .def_readwrite("enableCaching", &nlm::RenderingSettings::enableCaching)
        .def_readwrite("enableCompression", &nlm::RenderingSettings::enableCompression)
        .def_readwrite("alpha", &nlm::RenderingSettings::alpha)
        .def_readwrite("shininess", &nlm::RenderingSettings::shininess)
        .def_readwrite("specularIntensity", &nlm::RenderingSettings::specularIntensity)
        .def_readwrite("ambientIntensity", &nlm::RenderingSettings::ambientIntensity);

    // Add bindings for PerformanceStats struct
    py::class_<nlm::PerformanceStats>(m, "PerformanceStats", R"pbdoc(Performance statistics for visualization)pbdoc")
        .def(py::init<>())
        .def_readwrite("frameTime", &nlm::PerformanceStats::frameTime)
        .def_readwrite("updateTime", &nlm::PerformanceStats::updateTime)
        .def_readwrite("renderTime", &nlm::PerformanceStats::renderTime)
        .def_readwrite("memoryUsage", &nlm::PerformanceStats::memoryUsage)
        .def_readwrite("vertexCount", &nlm::PerformanceStats::vertexCount)
        .def_readwrite("triangleCount", &nlm::PerformanceStats::triangleCount)
        .def_readwrite("fps", &nlm::PerformanceStats::fps);

    py::class_<nlm::VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Comprehensive visualization interface for brain simulation)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::VisualizationInterface::initialize,
             "Initialize visualization system (GLFW, SDL, OpenGL setup)")
        .def("shutdown", &nlm::VisualizationInterface::shutdown,
             "Clean up visualization resources")
        .def("update", &nlm::VisualizationInterface::update,
             "Update visualization with current brain state")
        .def("render", &nlm::VisualizationInterface::render,
             "Render current visualization frame")
        .def("beginFrame", &nlm::VisualizationInterface::beginFrame,
             "Start new frame")
        .def("endFrame", &nlm::VisualizationInterface::endFrame,
             "End current frame")
        .def("setBrain", &nlm::VisualizationInterface::setBrain, py::arg("brain"),
             "Set brain reference for visualization")
        // Visualization mode methods
        .def("enableNeuronView", &nlm::VisualizationInterface::enableNeuronView,
             "Enable neuron activity visualization")
        .def("disableNeuronView", &nlm::VisualizationInterface::disableNeuronView,
             "Disable neuron activity visualization")
        .def("isNeuronViewEnabled", &nlm::VisualizationInterface::isNeuronViewEnabled,
             "Check if neuron view is enabled")
        .def("enableSynapseView", &nlm::VisualizationInterface::enableSynapseView,
             "Enable synaptic connections visualization")
        .def("disableSynapseView", &nlm::VisualizationInterface::disableSynapseView,
             "Disable synaptic connections visualization")
        .def("isSynapseViewEnabled", &nlm::VisualizationInterface::isSynapseViewEnabled,
             "Check if synapse view is enabled")
        .def("enablePopulationView", &nlm::VisualizationInterface::enablePopulationView,
             "Enable neural population statistics visualization")
        .def("disablePopulationView", &nlm::VisualizationInterface::disablePopulationView,
             "Disable neural population statistics visualization")
        .def("isPopulationViewEnabled", &nlm::VisualizationInterface::isPopulationViewEnabled,
             "Check if population view is enabled")
        .def("enableNetworkView", &nlm::VisualizationInterface::enableNetworkView,
             "Enable network connectivity visualization")
        .def("disableNetworkView", &nlm::VisualizationInterface::disableNetworkView,
             "Disable network connectivity visualization")
        .def("isNetworkViewEnabled", &nlm::VisualizationInterface::isNetworkViewEnabled,
             "Check if network view is enabled")
        .def("enableMemoryView", &nlm::VisualizationInterface::enableMemoryView,
             "Enable memory system activity visualization")
        .def("disableMemoryView", &nlm::VisualizationInterface::disableMemoryView,
             "Disable memory system activity visualization")
        .def("isMemoryViewEnabled", &nlm::VisualizationInterface::isMemoryViewEnabled,
             "Check if memory view is enabled")
        .def("enableCognitionView", &nlm::VisualizationInterface::enableCognitionView,
             "Enable cognitive system activity visualization")
        .def("disableCognitionView", &nlm::VisualizationInterface::disableCognitionView,
             "Disable cognitive system activity visualization")
        .def("isCognitionViewEnabled", &nlm::VisualizationInterface::isCognitionViewEnabled,
             "Check if cognition view is enabled")
        .def("enableNeuromodulationView", &nlm::VisualizationInterface::enableNeuromodulationView,
             "Enable neuromodulator levels visualization")
        .def("disableNeuromodulationView", &nlm::VisualizationInterface::disableNeuromodulationView,
             "Disable neuromodulator levels visualization")
        .def("isNeuromodulationViewEnabled", &nlm::VisualizationInterface::isNeuromodulationViewEnabled,
             "Check if neuromodulation view is enabled")
        .def("enableDevelopmentView", &nlm::VisualizationInterface::enableDevelopmentView,
             "Enable developmental stage visualization")
        .def("disableDevelopmentView", &nlm::VisualizationInterface::disableDevelopmentView,
             "Disable developmental stage visualization")
        .def("isDevelopmentViewEnabled", &nlm::VisualizationInterface::isDevelopmentViewEnabled,
             "Check if development view is enabled")
        .def("enableAllViews", &nlm::VisualizationInterface::enableAllViews,
             "Enable all visualization modes")
        .def("disableAllViews", &nlm::VisualizationInterface::disableAllViews,
             "Disable all visualization modes")
        // Rendering methods
        .def("renderNeurons", &nlm::VisualizationInterface::renderNeurons,
             "Render neuron populations")
        .def("renderSynapses", &nlm::VisualizationInterface::renderSynapses,
             "Render synaptic connections")
        .def("renderPopulations", &nlm::VisualizationInterface::renderPopulations,
             "Render neural population statistics")
        .def("renderNetworkTopology", &nlm::VisualizationInterface::renderNetworkTopology,
             "Render network connectivity")
        .def("renderMemorySystems", &nlm::VisualizationInterface::renderMemorySystems,
             "Render memory system activity")
        .def("renderCognitiveSystems", &nlm::VisualizationInterface::renderCognitiveSystems,
             "Render cognitive system activity")
        .def("renderNeuromodulators", &nlm::VisualizationInterface::renderNeuromodulators,
             "Render neuromodulator levels")
        .def("renderDevelopment", &nlm::VisualizationInterface::renderDevelopment,
             "Render developmental stage")
        // Feature methods
        .def("setScale", &nlm::VisualizationInterface::setScale, py::arg("scale"),
             "Set visualization scale")
        .def("setPosition", &nlm::VisualizationInterface::setPosition, py::arg("x"), py::arg("y"),
             "Set visualization position")
        .def("setColorMap", &nlm::VisualizationInterface::setColorMap, py::arg("colormap"),
             "Set color map (string)")
        .def("setColorMap", &nlm::VisualizationInterface::setColorMap, py::arg("colormap_enum"),
             "Set color map (ColorMap enum)")
        .def("setAlpha", &nlm::VisualizationInterface::setAlpha, py::arg("alpha"),
             "Set transparency")
        .def("setLighting", &nlm::VisualizationInterface::setLighting, py::arg("enable"),
             "Enable/disable lighting")
        .def("setShading", &nlm::VisualizationInterface::setShading, py::arg("enable"),
             "Enable/disable shading")
        .def("setAntiAliasing", &nlm::VisualizationInterface::setAntiAliasing, py::arg("enable"),
             "Enable/disable anti-aliasing")
        .def("setLOD", &nlm::VisualizationInterface::setLOD, py::arg("enable"),
             "Set level of detail")
        .def("setCaching", &nlm::VisualizationInterface::setCaching, py::arg("enable"),
             "Enable/disable caching")
        .def("setCompression", &nlm::VisualizationInterface::setCompression, py::arg("enable"),
             "Enable/disable compression")
        // Interaction methods
        .def("setMouseCallback", &nlm::VisualizationInterface::setMouseCallback,
             "Set mouse callback")
        .def("setKeyboardCallback", &nlm::VisualizationInterface::setKeyboardCallback,
             "Set keyboard callback")
        .def("setScrollCallback", &nlm::VisualizationInterface::setScrollCallback,
             "Set scroll callback")
        .def("handleMouseClick", &nlm::VisualizationInterface::handleMouseClick,
             "Handle mouse clicks")
        .def("handleKeyPress", &nlm::VisualizationInterface::handleKeyPress,
             "Handle key presses")
        .def("handleMouseScroll", &nlm::VisualizationInterface::handleMouseScroll,
             "Handle mouse scroll")
        // Export and save methods
        .def("saveScreenshot", &nlm::VisualizationInterface::saveScreenshot, py::arg("filename"),
             "Save current frame as image")
        .def("exportData", &nlm::VisualizationInterface::exportData, py::arg("filename"),
             "Export visualization data")
        .def("saveConfiguration", &nlm::VisualizationInterface::saveConfiguration, py::arg("filename"),
             "Save visualization configuration")
        .def("loadConfiguration", &nlm::VisualizationInterface::loadConfiguration, py::arg("filename"),
             "Load visualization configuration")
        // Performance methods
        .def("getMemoryUsage", &nlm::VisualizationInterface::getMemoryUsage,
             "Get memory usage")
        .def("getPerformanceStats", &nlm::VisualizationInterface::getPerformanceStats,
             "Get performance statistics")
        .def("getFPS", &nlm::VisualizationInterface::getFPS,
             "Get current FPS")
        .def("getFrameTime", &nlm::VisualizationInterface::getFrameTime,
             "Get frame time in milliseconds")
        // Utility methods
        .def("isActive", &nlm::VisualizationInterface::isActive,
             "Check if visualization is active")
        .def("setUpdateRate", &nlm::VisualizationInterface::setUpdateRate, py::arg("hz"),
             "Set update rate")
        .def("getUpdateRate", &nlm::VisualizationInterface::getUpdateRate,
             "Get update rate")
        .def("getBrain", &nlm::VisualizationInterface::getBrain,
             "Get current brain reference")
        .def("clearScreen", &nlm::VisualizationInterface::clearScreen, py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a") = 1.0f,
             "Clear screen with specific color")
        .def("setViewport", &nlm::VisualizationInterface::setViewport, py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"),
             "Set viewport");
    
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
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
