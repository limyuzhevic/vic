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
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            // Add parameter validation
            if (filepath.empty()) {
                throw std::invalid_argument("File path cannot be empty");
            }
            // Add file extension validation
            if (filepath.size() < 4 || filepath.substr(filepath.size() - 4) != ".json" && 
                filepath.substr(filepath.size() - 4) != ".yaml" && 
                filepath.substr(filepath.size() - 4) != ".yml") {
                throw std::invalid_argument("File must have .json, .yaml, or .yml extension");
            }
            return self.loadFromFile(filepath);
        }, py::arg("filepath"),
             "Load configuration from a JSON or YAML file. Automatically detects file format based on extension.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            // Add validation for arguments
            if (argc < 0) {
                throw std::invalid_argument("Number of arguments cannot be negative");
            }
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments (C-style argv).")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            if (filepath.empty()) {
                throw std::invalid_argument("File path cannot be empty");
            }
            return self.saveToFile(filepath);
        }, py::arg("filepath"),
             "Save configuration to a JSON file. Automatically uses JSON format for all saves.")
        .def("has", [](Config& self, const std::string& key) {
            if (key.empty()) {
                throw std::invalid_argument("Key cannot be empty");
            }
            return self.has(key);
        }, py::arg("key"),
             "Check if a configuration key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a Python list.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                if (self.has(key)) {
                    // Try to get as string first, then as other types
                    auto str_val = self.get<std::string>(key);
                    if (str_val.has_value()) {
                        result[key] = str_val.value();
                    } else {
                        auto int_val = self.get<int64_t>(key);
                        if (int_val.has_value()) {
                            result[key] = int_val.value();
                        } else {
                            auto double_val = self.get<double>(key);
                            if (double_val.has_value()) {
                                result[key] = double_val.value();
                            } else {
                                auto bool_val = self.get<bool>(key);
                                if (bool_val.has_value()) {
                                    result[key] = bool_val.value();
                                }
                            }
                        }
                    }
                }
            }
            return result;
        }, "Convert configuration to Python dictionary.");
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def(py::init<>())
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a Python list")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality as a tuple (width, height, channels)")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp as a float")
        .def("setTimestamp", [](SensoryInput& self, float timestamp) {
            if (timestamp < 0.0f) {
                throw std::invalid_argument("Timestamp cannot be negative");
            }
            self.setTimestamp(timestamp);
        }, py::arg("timestamp"), "Set the timestamp with validation");


    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create a vision input with validation")
        .def("setData", [](Vision& self, const std::vector<float>& data, size_t expectedWidth = 0, size_t expectedHeight = 0) {
            if (data.empty()) {
                throw std::invalid_argument("Vision data cannot be empty");
            }
            if (expectedWidth > 0 && expectedHeight > 0) {
                if (data.size() != expectedWidth * expectedHeight * self.getChannels()) {
                    throw std::invalid_argument("Data size does not match expected dimensions");
                }
            }
            self.setData(data);
        }, py::arg("data"), py::arg("expectedWidth") = 0, py::arg("expectedHeight") = 0,
             "Set vision data with optional dimension validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("to_array", [](const Vision& self) {
            py::array_t<float> array(py::buffer_info(
                const_cast<float*>(self.getData().data()),
                sizeof(float),
                py::format_descriptor<float>::format(),
                1,
                {static_cast<ssize_t>(self.getWidth() * self.getHeight() * self.getChannels())},
                {sizeof(float)}
            ));
            return array;
        }, "Convert vision data to a NumPy array");


    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an audio input with validation")
        .def("setData", [](Audio& self, const std::vector<float>& data, size_t expectedSamples = 0) {
            if (data.empty()) {
                throw std::invalid_argument("Audio data cannot be empty");
            }
            if (expectedSamples > 0 && data.size() != expectedSamples) {
                throw std::invalid_argument("Data size does not match expected sample count");
            }
            self.setData(data);
        }, py::arg("data"), py::arg("expectedSamples") = 0,
             "Set audio data with optional validation")
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            if (sampleRate == 0) {
                throw std::invalid_argument("Sample rate cannot be zero");
            }
            self.setSampleRate(sampleRate);
        }, py::arg("sampleRate"), "Set sample rate with validation")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("to_array", [](const Audio& self) {
            py::array_t<float> array(py::buffer_info(
                const_cast<float*>(self.getData().data()),
                sizeof(float),
                py::format_descriptor<float>::format(),
                1,
                {static_cast<ssize_t>(self.getNumSamples())},
                {sizeof(float)}
            ));
            return array;
        }, "Convert audio data to a NumPy array");


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
        .def("configure", [](SimpleWorld& self, size_t width, size_t height, 
                           size_t visionWidth, size_t visionHeight) {
            if (width == 0 || height == 0) {
                throw std::invalid_argument("World dimensions must be positive");
            }
            if (visionWidth == 0 || visionHeight == 0) {
                throw std::invalid_argument("Vision dimensions must be positive");
            }
            if (visionWidth > width || visionHeight > height) {
                throw std::invalid_argument("Vision dimensions cannot exceed world dimensions");
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions with validation")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            if (x < 0.0f || x > self.getWidth()) {
                throw std::invalid_argument("X coordinate must be within world bounds");
            }
            if (y < 0.0f || y > self.getHeight()) {
                throw std::invalid_argument("Y coordinate must be within world bounds");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent start position with validation")
        .def("applyMotorCommand", [](SimpleWorld& self, const Action& action, float currentTime) {
            if (currentTime < 0.0f) {
                throw std::invalid_argument("Current time cannot be negative");
            }
            self.applyMotorCommand(action, currentTime);
        }, py::arg("action"), py::arg("currentTime"),
             "Apply motor command with timestamp validation")
        .def("getSensoryPercept", [](SimpleWorld& self) {
            // Return percept as a dictionary for easier Python access
            auto percept = self.getSensoryPercept();
            py::dict result;
            
            auto vision = percept.getVision();
            if (!vision.getData().empty()) {
                result["vision"] = py::array_t<float>(py::buffer_info(
                    const_cast<float*>(vision.getData().data()),
                    sizeof(float),
                    py::format_descriptor<float>::format(),
                    1,
                    {static_cast<ssize_t>(vision.getWidth() * vision.getHeight() * vision.getChannels())},
                    {sizeof(float)}
                ));
            }
            
            auto audio = percept.getAudio();
            if (!audio.getData().empty()) {
                result["audio"] = py::array_t<float>(py::buffer_info(
                    const_cast<float*>(audio.getData().data()),
                    sizeof(float),
                    py::format_descriptor<float>::format(),
                    1,
                    {static_cast<ssize_t>(audio.getNumSamples())},
                    {sizeof(float)}
                ));
            }
            
            auto internal = percept.getInternal();
            if (!internal.getData().empty()) {
                result["internal"] = internal.getData();
            }
            
            auto proprioception = percept.getProprioception();
            if (!proprioception.getData().empty()) {
                result["proprioception"] = proprioception.getData();
            }
            
            return result;
        }, "Get sensory percept as a Python dictionary with NumPy arrays")
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            if (obj.x < 0.0f || obj.x > self.getWidth()) {
                throw std::invalid_argument("Object X position out of bounds");
            }
            if (obj.y < 0.0f || obj.y > self.getHeight()) {
                throw std::invalid_argument("Object Y position out of bounds");
            }
            self.addObject(obj);
        }, py::arg("obj"), "Add world object with validation")
        .def("isValidPosition", [](SimpleWorld& self, float x, float y) {
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"),
             "Check if position is valid (includes bounds checking)")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state with timestep")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept reference")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body reference")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            if (e <= 0.0f) {
                throw std::invalid_argument("Max energy must be positive");
            }
            self.setMaxEnergy(e);
        }, py::arg("e"), "Set maximum energy with validation")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            if (r < 0.0f || r > 1.0f) {
                throw std::invalid_argument("Energy decay rate must be between 0 and 1");
            }
            self.setEnergyDecayRate(r);
        }, py::arg("r"), "Set energy decay rate with validation")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", [](SimpleWorld& self, uint64_t seed) {
            self.setRandomSeed(seed);
        }, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("to_dict", [](SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            result["maxEnergy"] = self.getMaxEnergy();
            result["energyDecayRate"] = self.getEnergyDecayRate();
            result["simulationTime"] = self.getSimulationTime();
            result["randomSeed"] = self.getRandomSeed();
            
            auto agent = self.getAgentBody();
            py::dict agent_dict;
            agent_dict["x"] = agent.x;
            agent_dict["y"] = agent.y;
            agent_dict["orientation"] = agent.orientation;
            agent_dict["velocityX"] = agent.velocityX;
            agent_dict["velocityY"] = agent.velocityY;
            agent_dict["angularVelocity"] = agent.angularVelocity;
            agent_dict["energy"] = agent.energy;
            agent_dict["health"] = agent.health;
            agent_dict["age"] = agent.age;
            agent_dict["isMoving"] = agent.isMoving;
            agent_dict["isTurning"] = agent.isTurning;
            agent_dict["lastActionTime"] = agent.lastActionTime;
            result["agent"] = agent_dict;
            
            return result;
        }, "Convert world state to Python dictionary");


    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            if (!self.initialize()) {
                throw std::runtime_error("Brain initialization failed");
            }
            return true;
        }, "Initialize the brain with validation")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", [](Brain& self, NeuronId neuron, float current) {
            if (current < -1000.0f || current > 1000.0f) { // Reasonable bounds for neural current
                throw std::invalid_argument("Current injection out of reasonable range (-1000 to 1000)");
            }
            self.injectCurrent(neuron, current);
        }, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron with validation")
        .def("injectCurrentToNeurons", [](Brain& self, NeuronType type, float current) {
            if (current < -1000.0f || current > 1000.0f) {
                throw std::invalid_argument("Current injection out of reasonable range (-1000 to 1000)");
            }
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type with validation")
        .def("produceAction", [](Brain& self) {
            auto action = self.produceAction();
            py::dict result;
            result["type"] = static_cast<int>(action.getType());
            result["parameters"] = action.getParameters();
            return result;
        }, "Produce motor action based on neural activity as dictionary")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", [](Brain& self, const std::string& name) {
            if (name.empty()) {
                throw std::invalid_argument("Region name cannot be empty");
            }
            return self.addRegion(name);
        }, py::arg("name") = "",
             "Add a new neural region with validation")
        .def("getRegion", [](Brain& self, RegionId id) {
            auto* region = self.getRegion(id);
            if (!region) {
                throw std::invalid_argument("Region not found");
            }
            return region;
        }, py::arg("id"),
             "Get a region by ID, throws exception if not found")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID (may return nullptr)")
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
        .def("setDevelopmentalStage", [](Brain& self, DevelopmentalStage stage) {
            self.setDevelopmentalStage(stage);
        }, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", [](Brain& self) {
            std::string status = self.summary();
            // Add Python-specific formatting
            std::cout << "=== NLM Brain Status (Python API) ===" << std::endl;
            std::cout << status << std::endl;
            return status;
        }, "Log brain status to stdout and return as string")
        .def("to_dict", [](Brain& self) {
            py::dict result;
            result["regionCount"] = self.getRegionCount();
            result["totalNeuronCount"] = self.getTotalNeuronCount();
            result["totalSynapseCount"] = self.getTotalSynapseCount();
            result["activeNeuronCount"] = self.getActiveNeuronCount();
            result["firingNeuronCount"] = self.getFiringNeuronCount();
            result["averageFiringRate"] = self.getAverageFiringRate();
            result["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
            result["totalSpikeCount"] = self.getTotalSpikeCount();
            result["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
            
            py::list region_ids;
            for (auto id : self.getRegionIds()) {
                region_ids.append(id.value);
            }
            result["regionIds"] = region_ids;
            
            return result;
        }, "Convert brain state to Python dictionary");


    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, const py::dict& percept_dict) {
            // Convert Python dictionary back to SensoryPercept
            auto percept = self.getSensoryPercept();  // Get existing percept to modify
            
            if (percept_dict.contains("vision")) {
                py::array vision_array = percept_dict["vision"].cast<py::array>();
                std::vector<float> vision_data(vision_array.size());
                std::memcpy(vision_data.data(), vision_array.data(), vision_data.size() * sizeof(float));
                auto vision = percept.getVision();
                vision.setData(vision_data);
                percept.setVision(vision);
            }
            
            if (percept_dict.contains("audio")) {
                py::array audio_array = percept_dict["audio"].cast<py::array>();
                std::vector<float> audio_data(audio_array.size());
                std::memcpy(audio_data.data(), audio_array.data(), audio_data.size() * sizeof(float));
                auto audio = percept.getAudio();
                audio.setData(audio_data);
                percept.setAudio(audio);
            }
            
            if (percept_dict.contains("internal")) {
                std::vector<float> internal_data = percept_dict["internal"].cast<std::vector<float>>();
                auto internal = percept.getInternal();
                internal.clearSignals();
                for (float val : internal_data) {
                    internal.addSignal(val);
                }
                percept.setInternal(internal);
            }
            
            if (percept_dict.contains("proprioception")) {
                std::vector<float> proprio_data = percept_dict["proprioception"].cast<std::vector<float>>();
                auto proprioception = percept.getProprioception();
                proprioception.clearSignals();
                for (float val : proprio_data) {
                    proprioception.addSignal(val);
                }
                percept.setProprioception(proprioception);
            }
            
            if (percept_dict.contains("timestamp")) {
                float timestamp = percept_dict["timestamp"].cast<float>();
                if (timestamp < 0.0f) {
                    throw std::invalid_argument("Timestamp cannot be negative");
                }
                percept.setTimestamp(timestamp);
            }
            
            self.processSensoryInput(percept);
        }, py::arg("percept_dict"),
             "Process sensory input from Python dictionary (supports vision, audio, internal, proprioception arrays)")
        .def("decodeMotorCommand", [](AgentBrain& self) {
            auto action = self.decodeMotorCommand();
            py::dict result;
            result["type"] = static_cast<int>(action.getType());
            result["parameters"] = action.getParameters();
            return result;
        }, "Decode brain motor activity into motor command dictionary")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            if (std::isnan(reward) || std::isinf(reward)) {
                throw std::invalid_argument("Reward value is invalid");
            }
            if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
                throw std::invalid_argument("Predicted reward value is invalid");
            }
            self.applyRewardModulation(reward, predictedReward);
        }, py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation with validation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
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
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("to_dict", [](AgentBrain& self) {
            py::dict result;
            result["brainStats"] = self.getBrain().summary();
            result["neuromodulationLevel"] = self.getNeuromodulationLevel();
            result["curiosityLevel"] = self.getCuriosityLevel();
            result["noveltyLevel"] = self.getNoveltyLevel();
            result["predictionError"] = self.getPredictionError();
            result["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
            result["rewardModulationEnabled"] = self.isRewardModulationEnabled();
            result["structuralPlasticityEnabled"] = self.isStructuralPlasticityEnabled();
            result["developmentEnabled"] = self.isDevelopmentEnabled();
            result["curiosityEnabled"] = self.isCuriosityEnabled();
            return result;
        }, "Convert agent brain state to Python dictionary");


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
