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
        .def("loadFromJsonString", [](Config& self, const std::string& json) {
            return self.loadFromJsonString(json);
        }, py::arg("json"),
             "Load configuration from JSON string")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("saveToJsonString", [](const Config& self) {
            return self.saveToJsonString();
        }, "Save configuration to JSON string")
        .def("has", py::overload_cast<const std::string&>(&Config::has), py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("getString", py::overload_cast<const std::string&, const std::string&>(&Config::get),
             py::arg("key"), py::arg("defaultValue"),
             "Get a string value from configuration")
        .def("getInt", py::overload_cast<const std::string&, int>(&Config::get),
             py::arg("key"), py::arg("defaultValue"),
             "Get an integer value from configuration")
        .def("getFloat", py::overload_cast<const std::string&, float>(&Config::get),
             py::arg("key"), py::arg("defaultValue"),
             "Get a float value from configuration")
        .def("getBool", py::overload_cast<const std::string&, bool>(&Config::get),
             py::arg("key"), py::arg("defaultValue"),
             "Get a boolean value from configuration")
        .def("setString", &Config::set,
             py::arg("key"), py::arg("value"),
             "Set a string value in configuration")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set an integer value in configuration")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a float value in configuration")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a boolean value in configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getFormattedData", [](const SensoryInput& self) {
            std::stringstream ss;
            ss << "["; 
            const auto& data = self.getData();
            for (size_t i = 0; i < data.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << data[i];
            }
            ss << "]";
            return ss.str();
        }, "Get formatted string representation of data");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromImage", [](Vision& self, const std::vector<std::vector<float>>& image) {
            // Convert 2D array to 1D vector
            std::vector<float> data;
            for (const auto& row : image) {
                data.insert(data.end(), row.begin(), row.end());
            }
            self.setData(data);
        }, py::arg("image"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getShape", [](const Vision& self) {
            return std::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get shape as (width, height, channels)")
        .def("getAverageIntensity", &Vision::getAverageIntensity,
             "Get average intensity of vision data");

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
        .def("getName", [](const Action& self) -> std::string {
            switch (self.getType()) {
                case ActionType::MoveForward: return "MoveForward";
                case ActionType::MoveBackward: return "MoveBackward";
                case ActionType::TurnLeft: return "TurnLeft";
                case ActionType::TurnRight: return "TurnRight";
                case ActionType::Look: return "Look";
                case ActionType::LookUp: return "LookUp";
                case ActionType::LookDown: return "LookDown";
                case ActionType::Interact: return "Interact";
                case ActionType::Eat: return "Eat";
                case ActionType::Drink: return "Drink";
                case ActionType::Rest: return "Rest";
                case ActionType::Wait: return "Wait";
                case ActionType::Custom: return "Custom";
                default: return "Unknown";
            }
        })
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& self) {
            return "<Action: " + std::to_string(static_cast<int>(self.getType())) + ", " +
                   std::to_string(self.getParameters().size()) + " params>";
        });
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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        })
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"))
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        })
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("velocityX"), py::arg("velocityY"))

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccess", [](const ActionResult& self) {
            return self.success;
        })
        .def("getFormattedResult", [](const ActionResult& self) {
            std::string status = self.success ? "SUCCESS" : "FAILED";
            return "Reward: " + std::to_string(self.reward) + ", Status: " + status + ", Message: " + self.message;
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

    m.def("run_simulation", [](std::shared_ptr<Brain> brain, size_t numSteps, bool verbose = false) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        brain->initialize();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": " 
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
        
        return std::make_tuple(brain->getFiringNeuronCount(), 
                               brain->getTotalSpikeCount(),
                               brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("numSteps"), py::arg("verbose") = false,
       "Run a simulation for a given number of steps with statistics");
    
    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain, 
                                    std::shared_ptr<SimpleWorld> world, 
                                    std::shared_ptr<AgentBrain> agent,
                                    size_t numSteps, bool verbose = false) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Brain, world, and agent cannot be null");
        }
        
        agent->initialize(*world);
        
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": " 
                         << "Pos: (" << agent->getBrain()->getRegion(RegionId(1))->getName() << ") "
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << brain->getTotalSpikeCount() << " total spikes, "
                         << "Curiosity: " << agent->getCuriosityLevel() << ", "
                         << "Novelty: " << agent->getNoveltyLevel() << std::endl;
            }
        }
        
        return std::make_tuple(brain->getFiringNeuronCount(), 
                               brain->getTotalSpikeCount(),
                               brain->getAverageFiringRate(),
                               agent->getCuriosityLevel(),
                               agent->getNoveltyLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
       py::arg("numSteps"), py::arg("verbose") = false,
       "Run a complete agent simulation with world interaction");
    
    m.def("get_brain_stats", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) throw std::runtime_error("Brain cannot be null");
        
        nlohmann::json stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["pending_spike_events"] = brain->getPendingSpikeEventCount();
        
        return stats.dump(2);
    }, py::arg("brain"), "Get comprehensive brain statistics as JSON");
    
    m.def("create_experiment_config", [](const std::string& name,
                                         size_t neuronCount = 1000,
                                         size_t regionCount = 1,
                                         float connectionProbability = 0.1f) {
        auto config = std::make_shared<Config>();
        
        config->setInt("neuron_count", neuronCount);
        config->setInt("region_count", regionCount);
        config->setFloat("connection_probability", connectionProbability);
        
        // Set some defaults for a specific experiment
        config->setString("experiment.name", name);
        config->setString("experiment.type", "behavioral_learning");
        config->setBool("neuromodulation.reward.enable", true);
        config->setBool("neuromodulation.curiosity.enable", true);
        config->setBool("plasticity.stdp.enable", true);
        config->setBool("plasticity.hebbian.enable", true);
        config->setFloat("plasticity.stdp.learning_rate", 0.001f);
        
        return config;
    }, py::arg("name"), py::arg("neuronCount") = 1000,
       py::arg("regionCount") = 1, py::arg("connectionProbability") = 0.1f,
       "Create an experiment configuration with sensible defaults");
    
    m.def("benchmark_performance", [](std::shared_ptr<Brain> brain,
                                     size_t numSteps = 1000,
                                     bool warmup = true) {
        if (!brain) throw std::runtime_error("Brain cannot be null");
        
        if (warmup) {
            brain->initialize();
            for (size_t step = 0; step < numSteps / 10; ++step) {
                brain->step(step);
            }
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        brain->initialize();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        double stepsPerSecond = numSteps / (duration.count() / 1000.0);
        
        nlohmann::json result;
        result["steps"] = numSteps;
        result["time_ms"] = duration.count();
        result["steps_per_second"] = stepsPerSecond;
        result["neurons"] = brain->getTotalNeuronCount();
        result["synapses"] = brain->getTotalSynapseCount();
        
        return result.dump(2);
    }, py::arg("brain"), py::arg("numSteps") = 1000, py::arg("warmup") = true,
       "Benchmark brain performance and return metrics");
    
    m.def("print_config_summary", [](const Config& config) {
        std::cout << config.summary() << std::endl;
    }, py::arg("config"), "Print configuration summary to stdout");
    
    // Visualization helpers
    m.def("generate_histogram_data", [](const std::vector<float>& data,
                                        size_t bins = 10,
                                        float min = 0.0f,
                                        float max = 1.0f) {
        nlohmann::json histogram;
        
        float binWidth = (max - min) / bins;
        std::vector<size_t> binCounts(bins, 0);
        
        for (float value : data) {
            if (value < min) continue;
            if (value >= max) continue;
            
            size_t binIndex = static_cast<size_t>((value - min) / binWidth);
            if (binIndex >= bins) binIndex = bins - 1;
            binCounts[binIndex]++;
        }
        
        histogram["bins"] = bins;
        histogram["min"] = min;
        histogram["max"] = max;
        histogram["bin_width"] = binWidth;
        histogram["data"] = binCounts;
        
        return histogram.dump(2);
    }, py::arg("data"), py::arg("bins") = 10, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
       "Generate histogram data from numerical values");
    
    // Performance profiling utilities
    m.def("profile_simulation", [](std::shared_ptr<nlm::Brain> brain,
                                   size_t numSteps,
                                   size_t profileInterval = 100) {
        nlohmann::json profile;
        
        if (!brain) throw std::runtime_error("Brain cannot be null");
        
        brain->initialize();
        
        std::vector<std::chrono::high_resolution_clock::time_point> timestamps;
        std::vector<size_t> spikeCounts;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < numSteps; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            brain->step(step);
            auto stepEnd = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart);
            timestamps.push_back(stepStart);
            spikeCounts.push_back(brain->getTotalSpikeCount());
            
            if ((step + 1) % profileInterval == 0) {
                auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime);
                double stepsPerSecond = (step + 1) / (totalDuration.count() / 1000000.0);
                
                nlohmann::json point;
                point["step"] = step + 1;
                point["duration_us"] = duration.count();
                point["total_duration_us"] = totalDuration.count();
                point["steps_per_second"] = stepsPerSecond;
                point["total_spikes"] = brain->getTotalSpikeCount();
                point["firing_neurons"] = brain->getFiringNeuronCount();
                
                profile["profile_points"].push_back(point);
            }
        }
        
        auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime);
        double overallStepsPerSecond = numSteps / (totalDuration.count() / 1000000.0);
        
        profile["total_steps"] = numSteps;
        profile["total_duration_us"] = totalDuration.count();
        profile["overall_steps_per_second"] = overallStepsPerSecond;
        profile["average_step_duration_us"] = totalDuration.count() / numSteps;
        
        return profile.dump(2);
    }, py::arg("brain"), py::arg("numSteps"), py::arg("profileInterval") = 100,
       "Profile simulation performance and return timing data");
    
    // Neural network analysis utilities
    m.def("analyze_connectivity", [](const std::shared_ptr<nlm::Brain>& brain) {
        nlohmann::json connectivity;
        
        if (!brain) throw std::runtime_error("Brain cannot be null");
        
        size_t totalConnections = brain->getTotalSynapseCount();
        if (totalConnections == 0) {
            return "[]"; // Return empty JSON array as string
        }
        
        // Get all regions
        auto regions = brain->getRegions();
        
        // Analyze connectivity patterns
        nlohmann::json regionsAnalysis;
        for (const auto& region : regions) {
            nlohmann::json regionData;
            regionData["id"] = region->getId().index();
            regionData["name"] = region->getName();
            regionData["neuron_count"] = region->getTotalNeuronCount();
            regionData["synapse_count"] = region->getSynapseCount();
            regionData["average_weight"] = region->getAverageSynapticWeight();
            
            // Calculate E/I ratio for this region
            float totalExcitatory = 0.0f;
            float totalInhibitory = 0.0f;
            
            auto synapses = region->getSynapses();
            for (const auto* syn : synapses) {
                float weight = syn->getWeight();
                if (weight > 0) {
                    totalExcitatory += weight;
                } else {
                    totalInhibitory += std::abs(weight);
                }
            }
            
            regionData["e_i_ratio"] = (totalInhibitory > 0) ? (totalExcitatory / totalInhibitory) : (totalExcitatory > 0 ? std::numeric_limits<float>::infinity() : 0.0f);
            
            regionsAnalysis[region->getId().index()] = regionData;
        }
        
        connectivity["total_connections"] = totalConnections;
        connectivity["region_count"] = regions.size();
        connectivity["regions"] = regionsAnalysis;
        
        return connectivity.dump(2);
    }, py::arg("brain"), "Analyze neural connectivity patterns");
    
    // Time series analysis
    m.def("analyze_time_series", [](const std::vector<std::pair<size_t, float>>& timeSeries) {
        nlohmann::json analysis;
        
        if (timeSeries.empty()) {
            return "[]"; // Return empty JSON array as string
        }
        
        std::vector<size_t> steps;
        std::vector<float> values;
        
        for (const auto& point : timeSeries) {
            steps.push_back(point.first);
            values.push_back(point.second);
        }
        
        // Calculate rolling statistics
        size_t windowSize = std::min<size_t>(10, values.size());
        std::vector<float> rollingMeans;
        
        for (size_t i = 0; i <= values.size() - windowSize; ++i) {
            float sum = 0.0f;
            for (size_t j = 0; j < windowSize; ++j) {
                sum += values[i + j];
            }
            rollingMeans.push_back(sum / windowSize);
        }
        
        analysis["points"] = timeSeries.size();
        analysis["window_size"] = windowSize;
        analysis["rolling_means"] = rollingMeans;
        analysis["trends"] = "increasing";
        
        return analysis.dump(2);
    }, py::arg("timeSeries"), "Analyze time series data from simulations");
    
    // Experiment management utilities
    m.def("export_experiment_results", [](const std::string& filename,
                                          const std::vector<nlohmann::json>& results,
                                          const std::string& description = "") {
        nlohmann::json experiment;
        
        experiment["description"] = description;
        experiment["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        experiment["version"] = "6.0_final_integration";
        experiment["results"] = results;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << experiment.dump(2);
        file.close();
        
        return std::string("Results exported to " + filename);
    }, py::arg("filename"), py::arg("results"), py::arg("description") = "",
       "Export experiment results to JSON file");
    
    m.def("import_experiment_results", [](const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filename);
        }
        
        nlohmann::json experiment;
        file >> experiment;
        file.close();
        
        return experiment["results"].get<std::vector<nlohmann::json>>();
    }, py::arg("filename"), "Import experiment results from JSON file");
    
    // Checkpoint management utilities
    m.def("create_checkpoint_bundle", [](const std::string& directory,
                                        const std::shared_ptr<nlm::Brain>& brain,
                                        const std::string& metadata = "") {
        nlohmann::json bundle;
        
        bundle["directory"] = directory;
        bundle["metadata"] = metadata;
        bundle["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        
        if (brain) {
            bundle["brain_stats"]["neurons"] = brain->getTotalNeuronCount();
            bundle["brain_stats"]["synapses"] = brain->getTotalSynapseCount();
            bundle["brain_stats"]["total_spikes"] = brain->getTotalSpikeCount();
            bundle["brain_stats"]["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        }
        
        std::ofstream file(directory + "/checkpoint_bundle.json");
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create checkpoint bundle in directory: " + directory);
        }
        
        file << bundle.dump(2);
        file.close();
        
        return std::string("Checkpoint bundle created in " + directory);
    }, py::arg("directory"), py::arg("brain"), py::arg("metadata") = "",
       "Create a checkpoint bundle with metadata and stats");
    
    // Research configuration utilities
    m.def("create_advanced_config", [](const std::string& name,
                                       const nlohmann::json& customParams) {
        auto config = std::make_shared<nlm::Config>();
        
        // Set basic defaults
        config->setInt("neuron_count", 1000);
        config->setInt("region_count", 1);
        config->setFloat("connection_probability", 0.1f);
        config->setFloat("simulation_timestep", 0.001f);
        config->setString("experiment.name", name);
        config->setString("experiment.type", "advanced_research");
        
        // Apply custom parameters if provided
        if (!customParams.is_null()) {
            for (auto it = customParams.begin(); it != customParams.end(); ++it) {
                std::string key = it.key();
                if (it->is_number_integer()) {
                    config->setInt(key, it->get<int>());
                } else if (it->is_number_float()) {
                    config->setFloat(key, it->get<float>());
                } else if (it->is_string()) {
                    config->setString(key, it->get<std::string>());
                } else if (it->is_boolean()) {
                    config->setBool(key, it->get<bool>());
                }
            }
        }
        
        return config;
    }, py::arg("name"), py::arg("customParams") = nlohmann::json::object(),
       "Create an advanced configuration with custom parameters");
    
    m.def("create_research_suite", []() {
        nlohmann::json suite;
        
        // Add common research configurations
        suite["configs"] = nlohmann::json::array();
        
        // High plasticity config
        nlohmann::json highPlasticity;
        highPlasticity["name"] = "high_plasticity";
        highPlasticity["neuron_count"] = 2000;
        highPlasticity["connection_probability"] = 0.2f;
        highPlasticity["plasticity.stdp.learning_rate"] = 0.01f;
        highPlasticity["plasticity.hebbian.learning_rate"] = 0.005f;
        highPlasticity["development.stage"] = "critical_period";
        suite["configs"].push_back(highPlasticity);
        
        // Low connectivity config
        nlohmann::json lowConnectivity;
        lowConnectivity["name"] = "low_connectivity";
        lowConnectivity["neuron_count"] = 500;
        lowConnectivity["connection_probability"] = 0.01f;
        lowConnectivity["plasticity.stdp.learning_rate"] = 0.0005f;
        lowConnectivity["plasticity.structural.enable"] = false;
        suite["configs"].push_back(lowConnectivity);
        
        // Balanced config
        nlohmann::json balanced;
        balanced["name"] = "balanced_research";
        balanced["neuron_count"] = 1500;
        balanced["connection_probability"] = 0.15f;
        balanced["plasticity.stdp.learning_rate"] = 0.002f;
        balanced["plasticity.hebbian.learning_rate"] = 0.001f;
        balanced["plasticity.structural.enable"] = true;
        balanced["neuromodulation.curiosity.enable"] = true;
        balanced["neuromodulation.novelty.enable"] = true;
        suite["configs"].push_back(balanced);
        
        return suite.dump(2);
    }, "Create a suite of research configurations for experimentation");
    
    m.def("validate_config", [](const nlm::Config& config) {
        std::vector<std::string> errors;
        
        // Check for valid neuron count
        if (config.has("neuron_count")) {
            size_t neuronCount = config.get("neuron_count", 0);
            if (neuronCount == 0) {
                errors.push_back("neuron_count cannot be zero");
            }
        }
        
        // Check for valid connection probability
        if (config.has("connection_probability")) {
            float connProb = config.get("connection_probability", 0.0f);
            if (connProb < 0.0f || connProb > 1.0f) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        
        // Check for valid timestep
        if (config.has("simulation_timestep")) {
            float timestep = config.get("simulation_timestep", 0.001f);
            if (timestep <= 0.0f) {
                errors.push_back("simulation_timestep must be positive");
            }
        }
        
        nlohmann::json validation;
        validation["valid"] = errors.empty();
        validation["errors"] = errors;
        
        return validation.dump(2);
    }, py::arg("config"), "Validate configuration and return errors");
    
    // Build information
    m.def("get_version_info", []() {
        nlohmann::json info;
        info["name"] = "NLM (熙然)";
        info["version"] = "6.0_final_integration";
        info["phase"] = "Phase 6: Final Integration";
        info["description"] = "An Experimental Artificial Developmental Brain";
        info["python_bindings_version"] = "3.0_advanced";
        info["build_date"] = __DATE__;
        info["core_systems"] = {
            "memory", "neuromodulation", "prediction", "cognition",
            "development", "plasticity", "dynamics", "sensory",
            "motor", "environment", "visualization"
        };
        return info.dump(2);
    }, "Get NLM version and build information");
    
    // Initialize advanced module
    m.def("init_advanced", []() {
        return "NLM Advanced Python API initialized. Use pynlm_advanced for extended functionality.";
    }, "Initialize advanced NLM Python API");

    } // namespace nlm
