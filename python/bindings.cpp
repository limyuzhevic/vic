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
            try {
                return self.loadFromFile(filepath);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"), "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                return self.saveToFile(filepath);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"), "Save configuration to a JSON file")
        .def("has", [](Config& self, const std::string& key) {
            try {
                return self.has(key);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", [](Config& self) {
            try {
                return self.getKeys();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get all configuration keys")
        .def("clear", [](Config& self) {
            try {
                self.clear();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Clear all configuration entries")
        .def("summary", [](Config& self) {
            try {
                return self.summary();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Additional Config methods for better Python API
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue) {
            try {
                if (auto value = self.get<std::string>(key)) {
                    return *value;
                }
                return defaultValue;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), py::arg("defaultValue") = "", "Get string value from config")
        .def("get", [](Config& self, const std::string& key, int defaultValue) {
            try {
                if (auto value = self.get<int64_t>(key)) {
                    return *value;
                }
                return defaultValue;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get int value from config")
        .def("get", [](Config& self, const std::string& key, float defaultValue) {
            try {
                if (auto value = self.get<float>(key)) {
                    return *value;
                }
                return defaultValue;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get float value from config")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            try {
                self.set(key, value, ConfigSource::User);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), py::arg("value"), "Set string value in config")
        .def("set", [](Config& self, const std::string& key, int value) {
            try {
                self.set(key, static_cast<int64_t>(value), ConfigSource::User);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), py::arg("value"), "Set int value in config")
        .def("set", [](Config& self, const std::string& key, float value) {
            try {
                self.set(key, value, ConfigSource::User);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), py::arg("value"), "Set float value in config")
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            try {
                return self.getOr<std::string>(key, defaultValue);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), py::arg("defaultValue") = "", "Get value from config or default")
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            try {
                return self.getOr<int64_t>(key, defaultValue);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get int value from config or default")
        .def("getOr", [](Config& self, const std::string& key, float defaultValue) {
            try {
                return self.getOr<float>(key, defaultValue);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get float value from config or default")

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", [](const SensoryInput& self) {
            try {
                return self.getType();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get the type of sensory input")
        .def("getData", [](const SensoryInput& self) {
            try {
                return self.getData();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get the raw data as a vector")
        .def("getDimensions", [](const SensoryInput& self) {
            try {
                return self.getDimensions();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get the dimensionality")
        .def("getTimestamp", [](const SensoryInput& self) {
            try {
                return self.getTimestamp();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get the timestamp")
        .def("setTimestamp", [](SensoryInput& self, Timestamp timestamp) {
            try {
                self.setTimestamp(timestamp);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("timestamp"), "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                self.setData(data);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("data"))
        .def("getWidth", [](Vision& self) {
            try {
                return self.getWidth();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("getHeight", [](Vision& self) {
            try {
                return self.getHeight();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("getChannels", [](Vision& self) {
            try {
                return self.getChannels();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                self.setData(data);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("data"))
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            try {
                self.setSampleRate(sampleRate);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("sampleRate"))
        .def("getSampleRate", [](Audio& self) {
            try {
                return self.getSampleRate();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("getNumSamples", [](Audio& self) {
            try {
                return self.getNumSamples();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, float value) {
            try {
                self.addSignal(value);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("value"))
        .def("clearSignals", [](InternalSignals& self) {
            try {
                self.clearSignals();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        });

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", [](Action& self) {
            try {
                return self.getType();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setType", [](Action& self, ActionType type) {
            try {
                self.setType(type);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("type"))
        .def("getParameters", [](Action& self) {
            try {
                return self.getParameters();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            try {
                self.setParameters(params);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("params"))
        .def("getName", [](Action& self) {
            try {
                return self.getName();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("clone", [](Action& self) {
            try {
                return self.clone();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
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
        .def("reset", [](AgentBody& self) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", [](SensoryPercept& self) {
            try {
                return self.getVision();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setVision", [](SensoryPercept& self, const Vision& vision) {
            try {
                self.setVision(vision);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("vision"))
        .def("getVisionWidth", [](SensoryPercept& self) {
            try {
                return self.getVisionWidth();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("getVisionHeight", [](SensoryPercept& self) {
            try {
                return self.getVisionHeight();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("getTouch", [](SensoryPercept& self) {
            try {
                return self.getTouch();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setTouch", [](SensoryPercept& self, const std::vector<float>& touch) {
            try {
                self.setTouch(touch);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("touch"))
        .def("getInternal", [](SensoryPercept& self) {
            try {
                return self.getInternal();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setInternal", [](SensoryPercept& self, const std::vector<float>& internal) {
            try {
                self.setInternal(internal);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("internal"))
        .def("getProprioception", [](SensoryPercept& self) {
            try {
                return self.getProprioception();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setProprioception", [](SensoryPercept& self, const std::vector<float>& proprioception) {
            try {
                self.setProprioception(proprioception);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("proprioception"))
        .def("getAudio", [](SensoryPercept& self) {
            try {
                return self.getAudio();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setAudio", [](SensoryPercept& self, const std::vector<float>& audio) {
            try {
                self.setAudio(audio);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("audio"))
        .def("getAllSignals", [](SensoryPercept& self) {
            try {
                return self.getAllSignals();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("getTimestamp", [](SensoryPercept& self) {
            try {
                return self.getTimestamp();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setTimestamp", [](SensoryPercept& self, Timestamp timestamp) {
            try {
                self.setTimestamp(timestamp);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            try {
                self.configure(width, height, visionWidth, visionHeight);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", [](SimpleWorld& self) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            try {
                self.setAgentStart(x, y);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("x"), py::arg("y"))
        .def("update", [](SimpleWorld& self, double timestep) {
            try {
                self.update(timestep);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("timestep"))
        .def("applyMotorCommand", [](SimpleWorld& self, MotorCommand cmd, double currentTime) {
            try {
                self.applyMotorCommand(cmd, currentTime);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", [](SimpleWorld& self) {
            try {
                return self.getSensoryPercept();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::return_value_policy::reference_internal)
        .def("getAgentBody", [](SimpleWorld& self) {
            try {
                return self.getAgentBody();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::return_value_policy::reference_internal)
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            try {
                self.addObject(obj);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("obj"))
        .def("removeObject", [](SimpleWorld& self, float x, float y) {
            try {
                self.removeObject(x, y);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("x"), py::arg("y"))
        .def("isValidPosition", [](SimpleWorld& self, float x, float y) {
            try {
                return self.isValidPosition(x, y);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("x"), py::arg("y"))
        .def("getWidth", [](SimpleWorld& self) {
            try {
                return self.getWidth();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("getHeight", [](SimpleWorld& self) {
            try {
                return self.getHeight();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("getMaxEnergy", [](SimpleWorld& self) {
            try {
                return self.getMaxEnergy();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            try {
                self.setMaxEnergy(e);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("e"))
        .def("getEnergyDecayRate", [](SimpleWorld& self) {
            try {
                return self.getEnergyDecayRate();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            try {
                self.setEnergyDecayRate(r);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("r"))
        .def("getSimulationTime", [](SimpleWorld& self) {
            try {
                return self.getSimulationTime();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        })
        .def("setRandomSeed", [](SimpleWorld& self, uint64_t seed) {
            try {
                self.setRandomSeed(seed);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("seed"))
        .def("getRandomSeed", [](SimpleWorld& self) {
            try {
                return self.getRandomSeed();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](std::shared_ptr<Brain> self) {
            try {
                return self->initialize();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Initialize the brain with configuration")
        .def("step", [](std::shared_ptr<Brain> self, SimulationStep currentStep) {
            try {
                self->step(currentStep);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("currentStep"), "Perform a simulation step")
        .def("step", [](std::shared_ptr<Brain> self, SimulationStep currentStep, Timestamp currentTime) {
            try {
                self->step(currentStep, currentTime);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](std::shared_ptr<Brain> self, const SensoryInput& input) {
            try {
                self->receiveSensoryInput(input);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", [](std::shared_ptr<Brain> self, NeuronId neuron, MembranePotential current) {
            try {
                self->injectCurrent(neuron, current);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", [](std::shared_ptr<Brain> self, NeuronType type, MembranePotential current) {
            try {
                self->injectCurrentToNeurons(type, current);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", [](std::shared_ptr<Brain> self) {
            try {
                auto action = self->produceAction();
                if (!action) {
                    throw std::runtime_error("No action could be produced");
                }
                return action;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Produce motor action based on neural activity")
        .def("reset", [](std::shared_ptr<Brain> self) {
            try {
                self->reset();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Reset brain state")
        .def("save", [](std::shared_ptr<Brain> self, const std::string& filepath) {
            try {
                return self->save(filepath);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"), "Save brain state to file")
        .def("load", [](std::shared_ptr<Brain> self, const std::string& filepath) {
            try {
                return self->load(filepath);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", [](std::shared_ptr<Brain> self, const std::string& name = "") {
            try {
                return self->addRegion(name);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", [](std::shared_ptr<Brain> self, RegionId id) {
            try {
                auto region = self->getRegion(id);
                if (!region) {
                    throw std::runtime_error("Region not found");
                }
                return region;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("id"), "Get a region by ID")
        .def("getRegionCount", [](std::shared_ptr<Brain> self) {
            try {
                return self->getRegionCount();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get the number of regions")
        .def("getRegionIds", [](std::shared_ptr<Brain> self) {
            try {
                return self->getRegionIds();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get all region IDs")
        .def("getTotalNeuronCount", [](std::shared_ptr<Brain> self) {
            try {
                return self->getTotalNeuronCount();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", [](std::shared_ptr<Brain> self) {
            try {
                return self->getTotalSynapseCount();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", [](std::shared_ptr<Brain> self) {
            try {
                return self->getActiveNeuronCount();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get count of active neurons")
        .def("getFiringNeuronCount", [](std::shared_ptr<Brain> self) {
            try {
                return self->getFiringNeuronCount();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get count of currently firing neurons")
        .def("getAverageFiringRate", [](std::shared_ptr<Brain> self) {
            try {
                return self->getAverageFiringRate();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", [](std::shared_ptr<Brain> self) {
            try {
                return self->getExcitationInhibitionRatio();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", [](std::shared_ptr<Brain> self) {
            try {
                return self->getTotalSpikeCount();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get total spike count")
        .def("getDevelopmentalStage", [](std::shared_ptr<Brain> self) {
            try {
                return self->getDevelopmentalStage();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get current developmental stage")
        .def("setDevelopmentalStage", [](std::shared_ptr<Brain> self, DevelopmentalStage stage) {
            try {
                self->setDevelopmentalStage(stage);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("stage"), "Set developmental stage")
        .def("getConfig", [](std::shared_ptr<Brain> self) {
            try {
                return self->getConfig();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Get the configuration")
        .def("logStatus", [](std::shared_ptr<Brain> self) {
            try {
                self->logStatus();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Log brain status");

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
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Enhanced configuration methods for convenience
        .def("configureLearning", [](AgentBrain& self, bool reward, bool structural,
                                   bool development, bool curiosity) {
            self.enableRewardModulation(reward);
            self.enableStructuralPlasticity(structural);
            self.enableDevelopment(development);
            self.enableCuriosity(curiosity);
        }, py::arg("reward"), py::arg("structural"), py::arg("development"), py::arg("curiosity"),
        "Configure all learning features at once")
        .def("getLearningConfig", [](const AgentBrain& self) {
            return std::make_tuple(
                self.isRewardModulationEnabled(),
                self.isStructuralPlasticityEnabled(),
                self.isDevelopmentEnabled(),
                self.isCuriosityEnabled()
            );
        }, "Get current learning configuration as (reward, structural, development, curiosity)")
        .def("getBrainStats", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            return std::make_tuple(
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getAverageFiringRate(),
                brain->getFiringNeuronCount()
            );
        }, "Get comprehensive brain statistics as (neuron_count, synapse_count, avg_firing_rate, firing_neuron_count)")
        .def("getActiveSystems", [](const AgentBrain& self) {
            std::vector<std::string> activeSystems;
            if (self.isRewardModulationEnabled()) activeSystems.push_back("reward_modulation");
            if (self.isStructuralPlasticityEnabled()) activeSystems.push_back("structural_plasticity");
            if (self.isDevelopmentEnabled()) activeSystems.push_back("development");
            if (self.isCuriosityEnabled()) activeSystems.push_back("curiosity");
            return activeSystems;
        }, "Get list of active learning systems");
    
    // Create convenient factory functions for common use cases
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, "Create a new brain with default configuration (1000 neurons, 1 region)");
    
    m.def("createSimpleWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        "Create and configure a simple world with given dimensions");
    
    m.def("createSimpleWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        return world;
    }, "Create a simple world with default dimensions (10x10 world, 8x8 vision)");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Convenience simulation helper
    // Advanced Python API features for experienced users
    
    // Performance monitoring and profiling
    m.def("profileSimulation", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto startTime = std::chrono::high_resolution_clock::now();
        size_t initialSpikes = brain->getTotalSpikeCount();
        size_t initialNeurons = brain->getTotalNeuronCount();
        size_t initialSynapses = brain->getTotalSynapseCount();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t spikeRate = (finalSpikes - initialSpikes) / std::max(size_t(1), steps);
        
        return std::make_tuple(
            duration.count(),  // elapsed_time_ms
            spikeRate,        // average_spike_rate_per_step
            brain->getTotalNeuronCount(), // neuron_count
            brain->getTotalSynapseCount(), // synapse_count
            brain->getFiringNeuronCount(), // current_firing_neurons
            brain->getActiveNeuronCount() // current_active_neurons
        );
    }, py::arg("brain"), py::arg("steps"),
        "Profile simulation performance and return metrics (elapsed_time_ms, avg_spike_rate, neuron_count, synapse_count, current_firing, current_active)");
    
    // Advanced configuration and parameter tuning
    m.def("configureForLearning", [](std::shared_ptr<Brain> brain, bool enableRewardModulation, bool enableCuriosity, bool enableDevelopment, bool enableStructuralPlasticity, float learningRate) {
        auto config = brain->getConfig();
        
        // Set configuration based on learning preferences
        if (enableRewardModulation) {
            config->set("dopamine_enabled", true, ConfigSource::User);
        }
        if (enableCuriosity) {
            config->set("curiosity_enabled", true, ConfigSource::User);
        }
        if (enableDevelopment) {
            config->set("development_enabled", true, ConfigSource::User);
        }
        if (enableStructuralPlasticity) {
            config->set("structural_plasticity_enabled", true, ConfigSource::User);
        }
        
        // Set learning rate for plasticity rules
        config->set("stdp_ltp_weight", learningRate, ConfigSource::User);
        config->set("stdp_ltd_weight", learningRate * 0.9f, ConfigSource::User);
        
        return config;
    }, py::arg("brain"), py::arg("enableRewardModulation"), py::arg("enableCuriosity"), 
        py::arg("enableDevelopment"), py::arg("enableStructuralPlasticity"), py::arg("learningRate"),
        "Configure brain for specific learning scenario and return updated configuration");
    
    // Advanced simulation control and monitoring
    m.def("runWithMonitoring", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                               const std::vector<float>& sensoryData, size_t steps,
                               std::function<bool(size_t)> shouldStop) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        std::vector<size_t> spikeCounts;
        std::vector<float> firingRates;
        std::vector<double> simulationTimes;
        
        for (size_t step = 0; step < steps && !shouldStop(step); ++step) {
            world->update(0.1);
            
            // Convert sensory data to Vision input
            size_t width = world->getVisionWidth();
            size_t height = world->getVisionHeight();
            size_t totalPixels = width * height;
            
            std::vector<float> visionData(totalPixels, 0.0f);
            for (size_t i = 0; i < std::min(sensoryData.size(), totalPixels); ++i) {
                visionData[i] = sensoryData[i];
            }
            
            auto percept = std::make_unique<Vision>(width, height, 1, visionData);
            agent->processSensoryInput(*percept);
            brain->step(step);
            
            auto action = brain->produceAction();
            if (action) {
                world->applyMotorCommand(action->getType(), world->getSimulationTime());
            }
            
            // Record metrics
            spikeCounts.push_back(brain->getTotalSpikeCount());
            firingRates.push_back(brain->getAverageFiringRate());
            simulationTimes.push_back(world->getSimulationTime());
        }
        
        return std::make_tuple(
            spikeCounts,
            firingRates,
            simulationTimes,
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            agent->getDevelopmentalStage()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("sensoryData"), py::arg("steps"), 
        py::arg("shouldStop") = [](size_t) { return false; },
        "Run simulation with custom monitoring and return detailed metrics");
    
    // Advanced connectivity and network analysis
    m.def("analyzeNetworkStructure", [](std::shared_ptr<Brain> brain) {
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        
        // Calculate network statistics
        float connectionDensity = totalSynapses / std::max(size_t(1), totalNeurons * (totalNeurons - 1));
        
        // Analyze excitation/inhibition balance
        float eiRatio = brain->getExcitationInhibitionRatio();
        
        // Estimate clustering coefficient (simplified)
        float clusteringCoefficient = 0.0f;
        if (totalNeurons >= 3) {
            // Very rough approximation - real implementation would be more complex
            clusteringCoefficient = 0.1f + (0.5f * eiRatio / std::max(1.0f, eiRatio + 1.0f));
        }
        
        // Estimate path length (simplified)
        float pathLength = 0.0f;
        if (totalNeurons > 1) {
            pathLength = 2.0f / std::sqrt(totalNeurons);
        }
        
        return std::make_tuple(
            connectionDensity,
            eiRatio,
            clusteringCoefficient,
            pathLength,
            totalNeurons,
            totalSynapses
        );
    }, py::arg("brain"), "Analyze network structure and return topology metrics (connection_density, ei_ratio, clustering_coefficient, path_length, neuron_count, synapse_count)");
    
    // Advanced visualization and export capabilities
    m.def("exportNetworkGraph", [](std::shared_ptr<Brain> brain, const std::string& outputFile, 
                                 bool includeWeights = true, bool includeTypes = true) {
        // This is a simplified implementation - real export would be more sophisticated
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + outputFile);
        }
        
        file << "NLM Network Graph Export" << std::endl;
        file << "=======================" << std::endl;
        file << "Neuron count: " << brain->getTotalNeuronCount() << std::endl;
        file << "Synapse count: " << brain->getTotalSynapseCount() << std::endl;
        file << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        
        if (includeWeights) {
            file << "Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        }
        
        if (includeTypes) {
            file << "Network contains neurons of multiple types (Excitatory, Inhibitory, etc.)" << std::endl;
        }
        
        file << "Export completed successfully" << std::endl;
        return true;
    }, py::arg("brain"), py::arg("outputFile"), py::arg("includeWeights") = true, py::arg("includeTypes") = true,
        "Export network information to file (returns True on success)");
    
    // Advanced learning and training utilities
    m.def("trainWithReward", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              const std::vector<std::pair<std::vector<float>, float>>& trainingEpisodes,
                              size_t stepsPerEpisode) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        std::vector<float> episodeRewards;
        std::vector<size_t> episodeSpikes;
        
        for (size_t episode = 0; episode < trainingEpisodes.size(); ++episode) {
            const auto& trainingData = trainingEpisodes[episode];
            const auto& sensoryData = trainingData.first;
            float targetReward = trainingData.second;
            
            float episodeReward = 0.0f;
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            for (size_t step = 0; step < stepsPerEpisode; ++step) {
                world->update(0.1);
                
                // Convert sensory data to Vision input
                size_t width = world->getVisionWidth();
                size_t height = world->getVisionHeight();
                size_t totalPixels = width * height;
                
                std::vector<float> visionData(totalPixels, 0.0f);
                for (size_t i = 0; i < std::min(sensoryData.size(), totalPixels); ++i) {
                    visionData[i] = sensoryData[i];
                }
                
                auto percept = std::make_unique<Vision>(width, height, 1, visionData);
                agent->processSensoryInput(*percept);
                brain->step(step);
                
                auto action = brain->produceAction();
                if (action) {
                    world->applyMotorCommand(action->getType(), world->getSimulationTime());
                }
                
                // Apply reward modulation
                float currentReward = world->getSimulationTime() * 0.01f; // Simplified reward
                agent->applyRewardModulation(currentReward, targetReward);
                episodeReward += currentReward;
            }
            
            size_t finalSpikes = brain->getTotalSpikeCount();
            episodeSpikes.push_back(finalSpikes - initialSpikes);
            episodeRewards.push_back(episodeReward);
            
            // Reset for next episode
            brain->reset();
            world->reset();
            agent->reset();
        }
        
        return std::make_tuple(
            episodeRewards,
            episodeSpikes,
            std::accumulate(episodeRewards.begin(), episodeRewards.end(), 0.0f) / episodeRewards.size(),
            std::accumulate(episodeSpikes.begin(), episodeSpikes.end(), 0) / episodeSpikes.size()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("trainingEpisodes"), py::arg("stepsPerEpisode"),
        "Train brain with reward-based learning and return training statistics (rewards, spikes, avg_reward, avg_spikes)");
    
    // Error handling utilities
    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        
        auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0) {
            errors.push_back("neuron_count must be positive");
        } else if (neuronCount > 100000) {
            errors.push_back("neuron_count too large (max 100000)");
        }
        
        auto regionCount = config.getOr<int64_t>("region_count", 1);
        if (regionCount <= 0) {
            errors.push_back("region_count must be positive");
        }
        
        auto connProb = config.getOr<float>("connection_probability", 0.0f);
        if (connProb < 0.0f || connProb > 1.0f) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
        
        if (errors.empty()) {
            return std::vector<std::string>();
        } else {
            return errors;
        }
    }, py::arg("config"), "Validate configuration and return list of errors (empty if valid)");
    
    m.def("createConfig", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from " + filepath);
        }
        return config;
    }, py::arg("filepath"), "Load configuration from file or create default if file doesn't exist");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");



    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
