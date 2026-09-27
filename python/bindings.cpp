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
        })
        .def("to_dict", [](const Config& cfg) {
            py::dict d;
            auto keys = cfg.getKeys();
            for (const auto& key : keys) {
                d[key] = cfg.getValue(key);
            }
            return d;
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
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            std::stringstream ss;
            ss << "<Action type=" << static_cast<int>(action.getType())
               << " parameters=" << action.getParameters() << ">";
            return ss.str();
        })
        .def("to_dict", [](const Action& action) {
            py::dict d;
            d["type"] = static_cast<int>(action.getType());
            d["parameters"] = action.getParameters();
            d["name"] = action.getName();
            return d;
        });

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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            std::stringstream ss;
            ss << "<WorldObject x=" << obj.x << " y=" << obj.y
               << " type=" << static_cast<int>(obj.type)
               << " radius=" << obj.radius << " active=" << obj.active << ">";
            return ss.str();
        })
        .def("to_dict", [](const WorldObject& obj) {
            py::dict d;
            d["x"] = obj.x;
            d["y"] = obj.y;
            d["radius"] = obj.radius;
            d["type"] = static_cast<int>(obj.type);
            d["value"] = obj.value;
            d["active"] = obj.active;
            return d;
        });

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
        .def("__repr__", [](const AgentBody& body) {
            std::stringstream ss;
            ss << "<AgentBody x=" << body.x << " y=" << body.y
               << " orientation=" << body.orientation
               << " energy=" << body.energy
               << " health=" << body.health << ">";
            return ss.str();
        })
        .def("to_dict", [](const AgentBody& body) {
            py::dict d;
            d["x"] = body.x;
            d["y"] = body.y;
            d["orientation"] = body.orientation;
            d["velocityX"] = body.velocityX;
            d["velocityY"] = body.velocityY;
            d["angularVelocity"] = body.angularVelocity;
            d["energy"] = body.energy;
            d["health"] = body.health;
            d["age"] = body.age;
            d["isMoving"] = body.isMoving;
            d["isTurning"] = body.isTurning;
            d["lastActionTime"] = body.lastActionTime;
            return d;
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult reward=" << result.reward
               << " success=" << result.success
               << " message=\"" << result.message << "\">";
            return ss.str();
        })
        .def("to_dict", [](const ActionResult& result) {
            py::dict d;
            d["reward"] = result.reward;
            d["success"] = result.success;
            d["message"] = result.message;
            return d;
        });

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept vision=" << percept.getVisionWidth() << "x" << percept.getVisionHeight()
               << " touch=" << percept.getTouch().size() << " internal=" << percept.getInternal().size()
               << " proprioception=" << percept.getProprioception().size()
               << " audio=" << percept.getAudio().size() << ">";
            return ss.str();
        })
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict d;
            d["vision"] = percept.getVision();
            d["touch"] = percept.getTouch();
            d["internal"] = percept.getInternal();
            d["proprioception"] = percept.getProprioception();
            d["audio"] = percept.getAudio();
            d["timestamp"] = percept.getTimestamp();
            return d;
        });

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
        .def("__repr__", [](const SimpleWorld& world) {
            std::stringstream ss;
            ss << "<SimpleWorld width=" << world.getWidth() << " height=" << world.getHeight()
               << " time=" << world.getSimulationTime() << ">";
            return ss.str();
        });

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
        .def("__repr__", [](const Brain& brain) {
            std::stringstream ss;
            ss << "<Brain regions=" << brain.getRegionCount()
               << " neurons=" << brain.getTotalNeuronCount()
               << " totalSynapses=" << brain.getTotalSynapseCount()
               << ">";
            return ss.str();
        });

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("__repr__", [](const AgentBrain& agentBrain) {
            std::stringstream ss;
            ss << "<AgentBrain brain=" << agentBrain.getBrain()->getTotalNeuronCount() << " neurons>";
            return ss.str();
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

    m.def("createAgentSimulation", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world, 
                                 size_t steps, double dt) {
            if (!config) {
                throw std::invalid_argument("Config cannot be null");
            }
            if (!world) {
                throw std::invalid_argument("World cannot be null");
            }
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            agentBrain->initialize(world);
            
            world->setAgentStart(0.0f, 0.0f);
            
            return std::make_tuple(brain, agentBrain, world);
        }, 
        py::arg("config") = nullptr,
        py::arg("world") = nullptr,
        py::arg("steps") = 1000,
        py::arg("dt") = 0.1,
        "Create and run a complete agent simulation");

    m.def("runEpisode", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world, 
                          std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain,
                          size_t steps, double dt) {
            if (!config || !world || !brain || !agentBrain) {
                throw std::invalid_argument("All parameters must be non-null");
            }
            
            brain->reset();
            world->reset();
            agentBrain->reset();
            
            for (size_t i = 0; i < steps; ++i) {
                auto percept = world->getSensoryPercept();
                agentBrain->processSensoryInput(percept);
                
                auto action = brain->produceAction();
                world->applyMotorCommand(action, i * dt);
                
                world->update(dt);
            }
            
            return std::make_tuple(brain, world, agentBrain);
        }, 
        py::arg("config") = nullptr,
        py::arg("world") = nullptr,
        py::arg("brain") = nullptr,
        py::arg("agentBrain") = nullptr,
        py::arg("steps") = 1000,
        py::arg("dt") = 0.1,
        "Run a complete simulation episode with brain-world loop");

    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw std::invalid_argument("Brain cannot be null");
            }
            
            std::stringstream ss;
            ss << "=== Brain Statistics ===\n";
            ss << "Total Regions: " << brain->getRegionCount() << "\n";
            ss << "Total Neurons: " << brain->getTotalNeuronCount() << "\n";
            ss << "Total Synapses: " << brain->getTotalSynapseCount() << "\n";
            ss << "Active Neurons: " << brain->getActiveNeuronCount() << "\n";
            ss << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
            ss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
            ss << "Excitatory/Inhibitory Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
            ss << "Total Spike Count: " << brain->getTotalSpikeCount() << "\n";
            ss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
            ss << "============================";
            
            return ss.str();
        }, py::arg("brain"), "Get formatted brain statistics");

    m.def("getMemoryStats", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw std::invalid_argument("Brain cannot be null");
            }
            
            std::stringstream ss;
            ss << "Memory Usage:";
            ss << " [Brain: " << brain->getTotalNeuronCount() * 4 << " bytes]";
            ss << " [Regions: " << brain->getRegionCount() * 32 << " bytes]";
            ss << " [Synapses: " << brain->getTotalSynapseCount() * 8 << " bytes]";
            ss << " [Total: " << (brain->getTotalNeuronCount() * 4 + brain->getRegionCount() * 32 + brain->getTotalSynapseCount() * 8) << " bytes]";
            
            return ss.str();
        }, py::arg("brain"), "Get formatted brain memory statistics");

    m.def("createSimulationPreset", [](std::string presetName) {
            auto config = std::make_shared<Config>();
            
            if (presetName == "learning") {
                config->setValue("brain.learning_rate", 0.001f);
                config->setValue("brain.num_neurons", 100);
                config->setValue("brain.num_regions", 3);
                config->setValue("brain.initial_excitatory", 0.8f);
                config->setValue("brain.initial_inhibitory", 0.2f);
                config->setValue("brain.learning.enabled", true);
                config->setValue("brain.plasticity.enabled", true);
            } else if (presetName == "fast_learning") {
                config->setValue("brain.learning_rate", 0.01f);
                config->setValue("brain.num_neurons", 50);
                config->setValue("brain.num_regions", 2);
                config->setValue("brain.initial_excitatory", 0.6f);
                config->setValue("brain.initial_inhibitory", 0.4f);
                config->setValue("brain.learning.enabled", true);
                config->setValue("brain.plasticity.enabled", false);
            } else {
                config->setValue("brain.learning_rate", 0.001f);
                config->setValue("brain.num_neurons", 100);
                config->setValue("brain.num_regions", 1);
                config->setValue("brain.initial_excitatory", 0.5f);
                config->setValue("brain.initial_inhibitory", 0.5f);
                config->setValue("brain.learning.enabled", false);
                config->setValue("brain.plasticity.enabled", false);
            }
            
            return config;
        }, py::arg("presetName") = "default", "Create a configuration preset");

    m.def("batchProcessEpisodes", [](const std::vector<std::tuple<std::shared_ptr<Config>, std::shared_ptr<SimpleWorld>, std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>>>& simulations, 
                                   size_t stepsPerEpisode) {
            std::vector<std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>> results;
            
            for (const auto& sim : simulations) {
                auto [config, world, brain, agentBrain] = sim;
                if (!config || !world || !brain || !agentBrain) {
                    throw std::invalid_argument("Simulation components cannot be null");
                }
                
                brain->reset();
                world->reset();
                agentBrain->reset();
                
                for (size_t i = 0; i < stepsPerEpisode; ++i) {
                    auto percept = world->getSensoryPercept();
                    agentBrain->processSensoryInput(percept);
                    auto action = brain->produceAction();
                    world->applyMotorCommand(action, static_cast<double>(i) * 0.1);
                    world->update(0.1);
                }
                
                results.push_back(std::make_tuple(brain, world, agentBrain));
            }
            
            return results;
        }, py::arg("simulations"), py::arg("stepsPerEpisode") = 100, "Process multiple simulation episodes in batch");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
