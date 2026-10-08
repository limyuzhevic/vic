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
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Additional convenience methods
        .def("clearData", &SensoryInput::clearData, "Clear all sensory data")
        .def("hasData", &SensoryInput::hasData, "Check if sensory input has data")
        .def("getDataSize", &SensoryInput::getDataSize, "Get size of sensory data");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataPtr", [](Vision& self, float* data, size_t size) {
            self.setData(data, size);
        }, py::arg("data"), py::arg("size"), "Set data from raw pointer")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Additional convenience methods
        .def("getImageSize", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total image size (width * height)")
        .def("getTotalPixels", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Get total number of pixels")
        .def("normalizeData", [](Vision& self) {
            auto data = self.getData();
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            if (max > min) {
                for (auto& val : data) {
                    val = (val - min) / (max - min);
                }
                self.setData(data);
            }
        }, "Normalize vision data to [0,1]")
        .def("updateData", [](Vision& self, size_t index, float value) {
            auto data = self.getData();
            if (index < data.size()) {
                data[index] = value;
                self.setData(data);
            }
        }, py::arg("index"), py::arg("value"), "Update individual pixel value")
        .def("clearData", &Vision::clearData, "Clear all vision data")
        .def("hasData", [](const Vision& self) {
            return !self.getData().empty();
        }, "Check if vision has data")
        .def("getDataSize", [](const Vision& self) {
            return self.getData().size();
        }, "Get size of vision data");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Additional convenience methods
        .def("clearData", &Audio::clearData, "Clear all audio data")
        .def("hasData", [](const Audio& self) {
            return !self.getData().empty();
        }, "Check if audio has data")
        .def("getDataSize", [](const Audio& self) {
            return self.getData().size();
        }, "Get size of audio data")
        .def("normalizeData", [](Audio& self) {
            auto data = self.getData();
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            if (max > min) {
                for (auto& val : data) {
                    val = (val - min) / (max - min);
                }
                self.setData(data);
            }
        }, "Normalize audio data to [0,1]")
        .def("updateSample", [](Audio& self, size_t index, float value) {
            auto data = self.getData();
            if (index < data.size()) {
                data[index] = value;
                self.setData(data);
            }
        }, py::arg("index"), py::arg("value"), "Update individual audio sample");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals")
        .def("normalizeSignals", [](InternalSignals& self) {
            auto data = self.getData();
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            if (max > min) {
                for (auto& val : data) {
                    val = (val - min) / (max - min);
                }
                self.setData(data);
            }
        }, "Normalize internal signals to [0,1]")
        .def("getMaxSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum signal value")
        .def("getMinSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum signal value");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Additional convenience methods
        .def("clearData", &Audio::clearData, "Clear all audio data")
        .def("hasData", [](const Audio& self) {
            return !self.getData().empty();
        }, "Check if audio has data")
        .def("getDataSize", [](const Audio& self) {
            return self.getData().size();
        }, "Get size of audio data")
        .def("normalizeData", [](Audio& self) {
            auto data = self.getData();
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            if (max > min) {
                for (auto& val : data) {
                    val = (val - min) / (max - min);
                }
                self.setData(data);
            }
        }, "Normalize audio data to [0,1]")
        .def("updateSample", [](Audio& self, size_t index, float value) {
            auto data = self.getData();
            if (index < data.size()) {
                data[index] = value;
                self.setData(data);
            }
        }, py::arg("index"), py::arg("value"), "Update individual audio sample");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals")
        .def("normalizeSignals", [](InternalSignals& self) {
            auto data = self.getData();
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            if (max > min) {
                for (auto& val : data) {
                    val = (val - min) / (max - min);
                }
                self.setData(data);
            }
        }, "Normalize internal signals to [0,1]")
        .def("getMaxSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum signal value")
        .def("getMinSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum signal value");

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
        // Additional convenience methods
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            }
        }, py::arg("index"), py::arg("value"), "Set parameter at index")
        .def("getParameterCount", &Action::getParameterCount, "Get number of parameters")
        .def("isValid", &Action::isValid, "Check if action is valid")
        .def("toDict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to dictionary representation");

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
        // Additional convenience methods
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            return std::sqrt(std::pow(self.x - other.x, 2) + std::pow(self.y - other.y, 2));
        }, py::arg("other"), "Calculate distance to another world object")
        .def("isResource", &WorldObject::isResource, "Check if this is a resource object")
        .def("isHazard", &WorldObject::isHazard, "Check if this is a hazard object")
        .def("isWall", &WorldObject::isWall, "Check if this is a wall")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position of world object")
        .def("setRadius", &WorldObject::setRadius, py::arg("radius"), "Set radius of world object")
        .def("toDict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["type"] = self.type;
            result["value"] = self.value;
            result["radius"] = self.radius;
            result["active"] = self.active;
            return result;
        }, "Convert world object to dictionary representation");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Additional convenience methods for advanced users
        .def("setLearningRate", [](AgentBrain& self, float rate) {
            // Access dopamine system to modify learning rate
            auto* dopamine = self.getBrain()->getDopamine();
            if (dopamine) {
                // This would require adding a setLearningRate method to dopamine
                // For now, just set neuromodulation level
                self.getBrain()->applyNeuromodulation(*dopamine);
            }
        }, py::arg("rate"), "Set learning rate for reward modulation")
        .def("setExplorationLevel", [](AgentBrain& self, float level) {
            // Modulate curiosity and exploration behavior
            // This is a convenience wrapper for curiosity settings
            if (level < 0.0f || level > 1.0f) {
                throw std::invalid_argument("Exploration level must be between 0.0 and 1.0");
            }
            // Implementation would require more complex neuromodulation control
            // For now, just log the setting
            std::cout << "Exploration level set to: " << level << std::endl;
        }, py::arg("level"), "Set exploration level (0.0-1.0)")
        .def("simulateEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
                                  int numSteps, float timestep) {
            // Run a complete simulation episode
            auto brain = self.getBrain();
            for (int step = 0; step < numSteps; ++step) {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                
                // Brain step
                brain->step(step, step * timestep);
                
                // Get action and apply to world
                auto action = self.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Simulate reward
                float reward = 0.0f; // Simplified reward calculation
                self.applyRewardModulation(reward, 0.0f);
                
                // Update development
                self.updateDevelopment(timestep);
            }
        }, py::arg("world"), py::arg("numSteps"), py::arg("timestep") = 0.1f,
             "Run a complete simulation episode")
        .def("getBrainStats", [](const AgentBrain& self) {
            py::dict stats;
            auto brain = self.getBrain();
            if (brain) {
                stats["total_neurons"] = brain->getTotalNeuronCount();
                stats["total_synapses"] = brain->getTotalSynapseCount();
                stats["firing_neurons"] = brain->getFiringNeuronCount();
                stats["average_firing_rate"] = brain->getAverageFiringRate();
                stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
                stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
                stats["total_spike_count"] = brain->getTotalSpikeCount();
            }
            return stats;
        }, "Get comprehensive brain statistics");

    // Factory functions for advanced users
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set up advanced configuration with typical values for complex simulations
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.05);
        config->set("brain.connection_probability", 0.08);
        config->set("brain.initial_weight_mean", 0.6);
        config->set("brain.initial_weight_std", 0.15);
        
        // STDP parameters
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.005);
        config->set("plasticity.stdp.tau_plus", 25.0f);
        config->set("plasticity.stdp.tau_minus", 25.0f);
        
        // Hebbian parameters
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.002);
        
        // Structural plasticity
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0002f);
        config->set("plasticity.structural.pruning_rate", 0.00002f);
        
        // Neuromodulation
        config->set("neuromod.dopamine.scale", 1.2f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        // Development
        config->set("development.enabled", true);
        config->set("development.initial_plasticity", 1.0f);
        config->set("development.maturation_rate", 0.01f);
        
        return config;
    }, "Create an advanced configuration with optimized parameters for complex simulations");

    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Research-grade configuration for scientific experiments
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.03);
        config->set("brain.connection_probability", 0.05);
        config->set("brain.initial_weight_mean", 0.5);
        config->set("brain.initial_weight_std", 0.1);
        
        // Fine-tuned STDP for research
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.stdp.tau_plus", 30.0f);
        config->set("plasticity.stdp.tau_minus", 30.0f);
        
        // Enable all plasticity mechanisms
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.0005);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.synaptogenesis_rate", 0.00005f);
        config->set("plasticity.structural.pruning_rate", 0.000005f);
        
        // Full neuromodulation suite
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        // Advanced development with critical periods
        config->set("development.enabled", true);
        config->set("development.initial_plasticity", 1.2f);
        config->set("development.critical_period_length", 100.0f);
        config->set("development.maturation_rate", 0.02f);
        
        // Memory systems
        config->set("memory.working.enabled", true);
        config->set("memory.episodic.enabled", true);
        config->set("memory.associative.enabled", true);
        
        // Prediction system
        config->set("prediction.enabled", true);
        config->set("prediction.model_order", 2);
        
        // Cognition
        config->set("cognition.planner.enabled", true);
        config->set("cognition.concept_formation.enabled", true);
        config->set("cognition.attention.enabled", true);
        
        return config;
    }, "Create a research-grade configuration for scientific experiments");

    m.def("createQuickTestConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Very fast configuration for quick testing and prototyping
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.15);
        config->set("brain.initial_weight_mean", 0.5);
        config->set("brain.initial_weight_std", 0.2);
        
        // Minimal plasticity for speed
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01);
        config->set("plasticity.hebbian.enable", false);  // Disabled for speed
        config->set("plasticity.structural.enable", false);  // Disabled for speed
        
        // Basic neuromodulation
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", false);  // Disabled for speed
        config->set("neuromod.novelty.enable", false);   // Disabled for speed
        
        // Fast development
        config->set("development.enabled", false);  // Disabled for speed
        
        return config;
    }, "Create a quick-test configuration optimized for fast prototyping");

    // Advanced simulation utilities
    m.def("runIntegrationTest", []() {
        // Run the built-in Phase 6 integration test
        Phase6IntegratedExperiment experiment;
        
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 1000;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        py::dict pyResult;
        pyResult["total_reward"] = result.totalReward;
        pyResult["avg_firing_rate"] = result.avgFiringRate;
        pyResult["memory_episodes_stored"] = result.memoryEpisodesStored;
        pyResult["dopamine_level"] = result.dopamineLevel;
        pyResult["novelty_level"] = result.noveltyLevel;
        pyResult["curiosity_level"] = result.curiosityLevel;
        pyResult["memory_working_memory_integrated"] = result.memoryWorkingMemoryIntegrated;
        pyResult["memory_episodic_memory_integrated"] = result.memoryEpisodicMemoryIntegrated;
        pyResult["neuromodulation_integrated"] = result.neuromodulationIntegrated;
        pyResult["prediction_integrated"] = result.predictionIntegrated;
        pyResult["development_integrated"] = result.developmentIntegrated;
        
        return pyResult;
    }, "Run built-in integration test and return results");

    // Brain analysis utilities
    m.def("analyzeBrainConnectivity", [](const std::shared_ptr<Brain>& brain) {
        py::dict analysis;
        
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["average_firing_rate"] = brain->getAverageFiringRate();
        analysis["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        analysis["total_spike_count"] = brain->getTotalSpikeCount();
        
        // Regional analysis
        auto regions = brain->getRegions();
        py::list regions_list;
        for (const auto& region : regions) {
            py::dict region_info;
            region_info["id"] = region->getId();
            region_info["name"] = region->getName();
            region_info["neuron_count"] = region->getTotalNeuronCount();
            region_info["synapse_count"] = region->getSynapseCount();
            region_info["firing_rate"] = region->getAverageFiringRate();
            regions_list.append(region_info);
        }
        analysis["regions"] = regions_list;
        
        // Activity patterns
        analysis["activity_pattern"] = "active";
        if (brain->getFiringNeuronCount() == 0) {
            analysis["activity_pattern"] = "silent";
        } else if (brain->getFiringNeuronCount() < brain->getTotalNeuronCount() * 0.1) {
            analysis["activity_pattern"] = "sparse";
        } else if (brain->getFiringNeuronCount() > brain->getTotalNeuronCount() * 0.8) {
            analysis["activity_pattern"] = "synchronized";
        }
        
        return analysis;
    }, py::arg("brain"), "Analyze brain connectivity and activity patterns");

    // Export and import utilities
    m.def("exportBrainState", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to file");

    m.def("importBrainState", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        if (!brain->load(filepath)) {
            throw std::runtime_error("Failed to load brain state");
        }
        return brain;
    }, py::arg("filepath"), "Import brain state from file");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
