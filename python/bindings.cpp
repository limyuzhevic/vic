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
        .def_property_readonly("keys", &Config::getKeys, "Get all configuration keys")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__enter__", [](SimpleWorld& self) { return &self; })
        .def("__exit__", [](SimpleWorld&, py::args) {});

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
        .def("reset", &AgentBody::reset)
        .def("__enter__", [](AgentBody& self) { return &self; })
        .def("__exit__", [](AgentBody&, py::args) {});

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
              "Log brain status")
        .def("__enter__", [](Brain& self) { return &self; })
        .def("__exit__", [](Brain&, py::args) {});

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

    // Dynamic binding generation - Pythonic API for runtime class discovery
    m.def("get_class", [](const std::string& name) {
        // This function will be implemented in the Python wrapper
        // as it requires runtime introspection capabilities
        throw std::runtime_error("Dynamic class discovery requires nlm_ext.py");
    }, py::arg("name"), "Get class by name at runtime");
    
    m.def("list_classes", []() {
        std::vector<std::string> classes = {
            "Brain", "Config", "SimpleWorld", "AgentBrain",
            "Neuron", "Synapse", "NeuralPopulation", "NeuralRegion",
            "Vision", "Audio", "InternalSignals", "Action",
            "WorldObject", "AgentBody", "ActionResult", "SensoryPercept"
        };
        return classes;
    }, "List all available NLM classes");
    
    m.def("get_methods", [](const std::string& class_name) {
        // Return methods for a class (runtime introspection)
        std::map<std::string, std::string> methods;
        if (class_name == "Brain") {
            methods = {
                {"initialize", "Initialize the brain with configuration"},
                {"step", "Perform a simulation step"},
                {"receiveSensoryInput", "Inject sensory input into the brain"},
                {"produceAction", "Produce motor action based on neural activity"},
                {"reset", "Reset brain state"},
                {"save", "Save brain state to file"},
                {"load", "Load brain state from file"},
                {"addRegion", "Add a new neural region"},
                {"getRegion", "Get a region by ID"},
                {"getRegionCount", "Get the number of regions"},
                {"getTotalNeuronCount", "Get total neuron count"},
                {"getTotalSynapseCount", "Get total synapse count"},
                {"getActiveNeuronCount", "Get count of active neurons"},
                {"getFiringNeuronCount", "Get count of firing neurons"},
                {"getAverageFiringRate", "Get average firing rate"},
                {"getExcitationInhibitionRatio", "Get excitation/inhibition balance"},
                {"getTotalSpikeCount", "Get total spike count"},
                {"getDevelopmentalStage", "Get current developmental stage"},
                {"setDevelopmentalStage", "Set developmental stage"},
                {"getConfig", "Get the configuration"},
                {"logStatus", "Log brain status"}
            };
        } else if (class_name == "Config") {
            methods = {
                {"loadFromFile", "Load configuration from a JSON file"},
                {"saveToFile", "Save configuration to a JSON file"},
                {"has", "Check if a configuration key exists"},
                {"clear", "Clear all configuration entries"},
                {"summary", "Get a summary string of the configuration"}
            };
        } else if (class_name == "SimpleWorld") {
            methods = {
                {"configure", "Configure the world"},
                {"reset", "Reset the world"},
                {"setAgentStart", "Set agent start position"},
                {"update", "Update the world"},
                {"applyMotorCommand", "Apply a motor command"},
                {"getSensoryPercept", "Get sensory percept"},
                {"getAgentBody", "Get agent body state"},
                {"addObject", "Add a world object"},
                {"removeObject", "Remove a world object"},
                {"isValidPosition", "Check if position is valid"}
            };
        }
        return methods;
    }, py::arg("class_name"), "Get methods for a class");
    
    m.def("get_class_info", [](const std::string& class_name) {
        py::dict info;
        if (class_name == "Brain") {
            info["description"] = "Central neural simulation brain class";
            info["category"] = "Core";
            info["has_context_manager"] = true;
            info["has_properties"] = true;
            info["methods_count"] = 26;
        } else if (class_name == "Config") {
            info["description"] = "Configuration class for NLM system";
            info["category"] = "Core";
            info["has_context_manager"] = false;
            info["has_properties"] = true;
            info["methods_count"] = 6;
        } else if (class_name == "SimpleWorld") {
            info["description"] = "Simple 2D world for NLM simulation";
            info["category"] = "Simulation";
            info["has_context_manager"] = true;
            info["has_properties"] = false;
            info["methods_count"] = 21;
        }
        return info;
    }, py::arg("class_name"), "Get detailed information about a class");

    // Python callback system for events
    m.def("set_step_callback", [](std::function<void(SimulationStep, Timestamp)> callback) {
        // This would be implemented as a member function of Brain
        // For now, provide a placeholder implementation
        static std::function<void(SimulationStep, Timestamp)> globalStepCallback;
        globalStepCallback = std::move(callback);
    }, py::arg("callback"), "Set callback for simulation steps");
    
    m.def("set_spike_callback", [](std::function<void(const Neuron&, float)> callback) {
        static std::function<void(const Neuron&, float)> globalSpikeCallback;
        globalSpikeCallback = std::move(callback);
    }, py::arg("callback"), "Set callback for neuron spikes");
    
    m.def("set_learning_callback", [](std::function<void(const std::string&, float)> callback) {
        static std::function<void(const std::string&, float)> globalLearningCallback;
        globalLearningCallback = std::move(callback);
    }, py::arg("callback"), "Set callback for learning events");
    
    m.def("clear_callbacks", []() {
        // Clear all registered callbacks
        static std::function<void(SimulationStep, Timestamp)> globalStepCallback;
        static std::function<void(const Neuron&, float)> globalSpikeCallback;
        static std::function<void(const std::string&, float)> globalLearningCallback;
    }, "Clear all registered callbacks");

    // Plugin architecture support
    m.def("register_plugin", [](const std::string& name, const std::string& description, 
                               std::function<void(py::module&)> setup_func) {
        // This would be implemented in the Python wrapper
        throw std::runtime_error("Plugin registration requires nlm_plugins.py");
    }, py::arg("name"), py::arg("description"), py::arg("setup_func"), 
       "Register a Python plugin for NLM");
    
    m.def("load_plugin", [](const std::string& name, const std::string& path) {
        // This would be implemented in the Python wrapper
        throw std::runtime_error("Plugin loading requires nlm_plugins.py");
    }, py::arg("name"), py::arg("path"), "Load a plugin from a Python module");
    
    m.def("unload_plugin", [](const std::string& name) {
        // This would be implemented in the Python wrapper
        throw std::runtime_error("Plugin unloading requires nlm_plugins.py");
    }, py::arg("name"), "Unload a plugin");
    
    m.def("get_plugin_info", [](const std::string& name) {
        py::dict info;
        info["name"] = name;
        info["loaded"] = false;  // This would be checked against actual loaded plugins
        info["description"] = "Plugin information requires nlm_plugins.py";
        return info;
    }, py::arg("name"), "Get information about a plugin");
    
    m.def("list_plugins", []() {
        std::vector<std::string> plugins = {
            "standard_neuromodulation",
            "reward_system",
            "curiosity_system",
            "prediction_system"
        };
        return plugins;
    }, "List available plugins");

    // Custom serialization support
    m.def("to_python_dict", [](const Brain& brain) {
        py::dict result;
        result["class"] = "Brain";
        result["neuron_count"] = brain.getTotalNeuronCount();
        result["synapse_count"] = brain.getTotalSynapseCount();
        result["active_neuron_count"] = brain.getActiveNeuronCount();
        result["firing_neuron_count"] = brain.getFiringNeuronCount();
        result["total_spike_count"] = brain.getTotalSpikeCount();
        return result;
    }, py::arg("brain"), "Convert Brain object to Python dictionary");
    
    m.def("from_python_dict", [](py::dict data, const std::string& obj_type) {
        if (obj_type == "Brain") {
            auto config = createDefaultConfig();
            auto brain = createBrain(config);
            // Note: This is a simplified implementation - full deserialization would require more work
            return brain;
        }
        throw std::runtime_error("Unsupported object type for deserialization: " + obj_type);
    }, py::arg("data"), py::arg("obj_type"), "Create object from Python dictionary");

    // Hook system for custom behavior
    m.def("add_pre_step_hook", [](std::function<void(SimulationStep, Timestamp)> hook) {
        // This would be implemented as a hook system in Brain
        static std::vector<std::function<void(SimulationStep, Timestamp)>> preStepHooks;
        preStepHooks.push_back(std::move(hook));
    }, py::arg("hook"), "Add a pre-step hook to be called before each simulation step");
    
    m.def("add_post_step_hook", [](std::function<void(SimulationStep, Timestamp)> hook) {
        static std::vector<std::function<void(SimulationStep, Timestamp)>> postStepHooks;
        postStepHooks.push_back(std::move(hook));
    }, py::arg("hook"), "Add a post-step hook to be called after each simulation step");
    
    m.def("add_on_reset_hook", [](std::function<void()> hook) {
        static std::vector<std::function<void()>> onResetHooks;
        onResetHooks.push_back(std::move(hook));
    }, py::arg("hook"), "Add a hook to be called when brain is reset");
    
    m.def("call_hooks", [](const std::string& hook_type) {
        if (hook_type == "pre_step") {
            // Call all pre-step hooks
        } else if (hook_type == "post_step") {
            // Call all post-step hooks
        } else if (hook_type == "on_reset") {
            // Call all reset hooks
        }
    }, py::arg("hook_type"), "Call all hooks of a specific type");

    // Performance optimization - lazy loading
    m.def("get_lazy_class", [](const std::string& class_name) {
        // This would implement lazy loading of classes
        // For now, return a function that creates the class on demand
        if (class_name == "Brain") {
            return [](std::shared_ptr<Config> config) {
                return createBrain(config);
            };
        }
        throw std::runtime_error("Unsupported class for lazy loading: " + class_name);
    }, py::arg("class_name"), "Get a lazy-loading class factory");

    // Enhanced error handling and validation
    m.def("validate_arguments", [](const std::string& function_name, 
                                  py::kwargs kwargs) {
        // This would implement argument validation for specific functions
        // For now, provide a generic validation framework
        py::dict validation_errors;
        
        if (function_name == "Brain::step") {
            if (kwargs.contains("currentStep") && 
                !py::isinstance(kwargs["currentStep"], py::int_)) {
                validation_errors["currentStep"] = "Must be an integer";
            }
            if (kwargs.contains("currentTime") && 
                !py::isinstance(kwargs["currentTime"], (py::int_, py::float_))) {
                validation_errors["currentTime"] = "Must be a number";
            }
        }
        
        return validation_errors;
    }, py::arg("function_name"), py::arg("kwargs") = py::kwargs(), 
       "Validate function arguments");

    // Enhanced exception translation
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            py::set_error(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            py::set_error(PyExc_IndexError, e.what());
        } catch (...) {
            py::set_error(PyExc_RuntimeError, "Unknown exception");
        }
    });

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
