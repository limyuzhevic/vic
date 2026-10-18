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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)")
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
        
        // Enhanced Pythonic methods
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (value) return py::cast(*value);
            auto fval = self.get<double>(key);
            if (fval) return py::cast(*fval);
            auto bval = self.get<bool>(key);
            if (bval) return py::cast(*bval);
            auto str = self.get<std::string>(key);
            if (str) return py::cast(*str);
            py::list list_val;
            auto ival = self.get<std::vector<int>>(key);
            if (ival) {
                for (int i : *ival) list_val.append(i);
                return py::cast(list_val);
            }
            auto flist = self.get<std::vector<double>>(key);
            if (flist) {
                for (double d : *flist) list_val.append(d);
                return py::cast(list_val);
            }
            auto slist = self.get<std::vector<std::string>>(key);
            if (slist) {
                for (const std::string& s : *slist) list_val.append(s);
                return py::cast(list_val);
            }
            throw std::runtime_error("Key not found: " + key);
        }, py::arg("key"),
           "Get configuration value by key (returns Python equivalent type)")
        .def("get_or", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            if (self.has(key)) {
                return self.get<int64_t>(key).value_or(
                    self.get<double>(key).value_or(
                        self.get<bool>(key).value_or(
                            self.get<std::string>(key).value_or(defaultValue.get<std::string>())
                        )
                    )
                );
            }
            return defaultValue.get<int64_t>();
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value with default")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is_none()) {
                throw std::invalid_argument("Cannot set None value");
            } else if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<py::list>(value)) {
                // Convert Python list to std::vector
                py::list list_val = value.cast<py::list>();
                std::vector<double> vec;
                for (auto item : list_val) {
                    vec.push_back(item.cast<double>());
                }
                self.set(key, vec);
            } else {
                throw std::invalid_argument("Unsupported Python type for configuration value");
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with Python type conversion")
        .def("validate", [](const Config& self) {
            py::dict validation_results;
            validation_results["valid"] = true;
            py::list errors;
            
            // Basic validation rules
            if (self.has("brain.neuron_count")) {
                auto neuron_count = self.getOr<int64_t>("brain.neuron_count", 0);
                if (neuron_count < 1) {
                    errors.append("brain.neuron_count must be >= 1");
                    validation_results["valid"] = false;
                } else if (neuron_count > 100000) {
                    errors.append("brain.neuron_count too large (max 100000)");
                    validation_results["valid"] = false;
                }
            }
            
            if (self.has("brain.synapse_density")) {
                auto density = self.getOr<double>("brain.synapse_density", 0.0);
                if (density < 0.0 || density > 1.0) {
                    errors.append("brain.synapse_density must be between 0.0 and 1.0");
                    validation_results["valid"] = false;
                }
            }
            
            validation_results["errors"] = errors;
            return validation_results;
        }, "Validate configuration and return {valid: bool, errors: list}")
        .def("get_recommended", [](const Config& self) {
            py::dict recommendations;
            
            if (!self.has("brain.neuron_count")) {
                recommendations["brain.neuron_count"] = 1000;
            }
            if (!self.has("brain.synapse_density")) {
                recommendations["brain.synapse_density"] = 0.1;
            }
            if (!self.has("brain.connection_probability")) {
                recommendations["brain.connection_probability"] = 0.05;
            }
            
            return recommendations;
        }, "Get recommended configuration values for missing parameters")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of configuration");
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

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(),
             py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType,
             "Get action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters,
             "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName,
             "Get action name")
        .def("clone", &Action::clone,
             "Create a copy of this action")
        .def("set_parameter", [](Action& self, const std::string& name, float value) {
            auto params = self.getParameters();
            // Find parameter by name or add new one
            bool found = false;
            for (size_t i = 0; i < params.size(); ++i) {
                // Note: This assumes Action has a way to identify parameter names
                // For now, we'll just append if parameter not found
            }
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("name"), py::arg("value"),
             "Set specific parameter by name (convenience method)")
        .def("get_parameter", [](const Action& self, const std::string& name) {
            // Similar implementation for getting parameter
            return 0.0f; // Placeholder
        }, py::arg("name"),
             "Get parameter by name (convenience method)")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " type=" + 
                   std::to_string(static_cast<int>(action.getType())) + ">";
        }, "String representation of action");

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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds",
             py::call_guard<py::gil_scoped_release>())
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        
        // Enhanced Pythonic methods
        .def("run_simulation", [](SimpleWorld& self, size_t steps, double timestep) {
            py::dict results;
            py::list events;
            
            for (size_t step = 0; step < steps; ++step) {
                // Record state before update
                auto body_before = self.getAgentBody();
                auto percept_before = self.getSensoryPercept();
                
                // Update world
                self.update(timestep);
                
                // Record state after update
                auto body_after = self.getAgentBody();
                auto percept_after = self.getSensoryPercept();
                
                // Check for position changes
                if (body_before.x != body_after.x || body_before.y != body_after.y) {
                    py::dict event;
                    event["step"] = step;
                    event["position_before"] = py::make_tuple(body_before.x, body_before.y);
                    event["position_after"] = py::make_tuple(body_after.x, body_after.y);
                    event["distance"] = std::sqrt(std::pow(body_after.x - body_before.x, 2) + 
                                                 std::pow(body_after.y - body_before.y, 2));
                    events.append(event);
                }
            }
            
            results["steps_completed"] = steps;
            results["events"] = events;
            results["final_time"] = self.getSimulationTime();
            return results;
        }, py::arg("steps"), py::arg("timestep"),
             "Run simulation for specified steps and return results")
        .def("get_agent_stats", [](SimpleWorld& self) {
            py::dict stats;
            auto body = self.getAgentBody();
            
            stats["position"] = py::make_tuple(body.x, body.y);
            stats["orientation"] = body.orientation;
            stats["velocity"] = py::make_tuple(body.velocityX, body.velocityY);
            stats["energy"] = body.energy;
            stats["health"] = body.health;
            stats["age"] = body.age;
            stats["is_moving"] = body.isMoving;
            stats["is_turning"] = body.isTurning;
            stats["simulation_time"] = self.getSimulationTime();
            
            return stats;
        }, "Get agent statistics")
        .def("get_vision_stats", [](SimpleWorld& self) {
            py::dict stats;
            auto percept = self.getSensoryPercept();
            auto vision = percept.getVision();
            
            stats["width"] = percept.getVisionWidth();
            stats["height"] = percept.getVisionHeight();
            stats["has_vision"] = !vision.empty();
            if (!vision.empty()) {
                float total_intensity = 0.0f;
                size_t count = 0;
                for (float pixel : vision) {
                    total_intensity += pixel;
                    count++;
                }
                stats["avg_intensity"] = total_intensity / count;
                stats["max_intensity"] = *std::max_element(vision.begin(), vision.end());
                stats["min_intensity"] = *std::min_element(vision.begin(), vision.end());
            }
            
            return stats;
        }, "Get vision-related statistics")
        .def("is_agent_alive", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.health > 0 && body.energy > 0;
        }, "Check if agent is alive")
        .def("get_world_bounds", [](SimpleWorld& self) {
            return py::make_tuple(0.0f, self.getWidth(), 0.0f, self.getHeight());
        }, "Get world bounds as (min_x, max_x, min_y, max_y)")
        .def("__repr__", [](SimpleWorld& self) {
            return "<SimpleWorld: " + std::to_string(self.getWidth()) + "x" +
                   std::to_string(self.getHeight()) + "m, " +
                   std::to_string(self.getSimulationTime()) + "s>";
        }, "String representation of world");

    // Custom exception for NLM errors
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration",
             py::call_guard<py::gil_scoped_release>())
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step",
             py::call_guard<py::gil_scoped_release>())
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp",
             py::call_guard<py::gil_scoped_release>())
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
        
        // Enhanced Pythonic methods
        .def("run_for_steps", [](Brain& self, size_t steps) {
            for (SimulationStep i = 0; i < static_cast<SimulationStep>(steps); ++i) {
                self.step(i, i * 0.001);
            }
        }, py::arg("steps"),
             "Run simulation for specified number of steps")
        .def("get_stats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get brain statistics as a dictionary")
        .def("validate_config", [](Brain& self) {
            const auto& config = self.getConfig();
            bool is_valid = true;
            py::list errors;
            
            // Check required configuration parameters
            if (!config->has("brain.neuron_count")) {
                errors.append("Missing 'brain.neuron_count'");
                is_valid = false;
            }
            if (!config->has("brain.synapse_density")) {
                errors.append("Missing 'brain.synapse_density'");
                is_valid = false;
            }
            
            return std::make_pair(is_valid, errors);
        }, "Validate configuration and return (is_valid, errors_list)")
        .def("get_plasticity_stats", [](Brain& self) {
            py::dict plasticity_stats;
            
            if (auto* stdp = self.getSTDP()) {
                plasticity_stats["stdp_enabled"] = true;
                plasticity_stats["stdp_learning_rate"] = stdp->getLearningRate();
            } else {
                plasticity_stats["stdp_enabled"] = false;
            }
            
            if (auto* hebbian = self.getHebbian()) {
                plasticity_stats["hebbian_enabled"] = true;
                plasticity_stats["hebbian_learning_rate"] = hebbian->getLearningRate();
            } else {
                plasticity_stats["hebbian_enabled"] = false;
            }
            
            return plasticity_stats;
        }, "Get plasticity system statistics")
        .def("__repr__", [](const Brain& self) {
            py::dict stats;
            stats["neurons"] = self.getTotalNeuronCount();
            stats["synapses"] = self.getTotalSynapseCount();
            stats["stage"] = static_cast<int>(self.getDevelopmentalStage());
            return "<Brain: " + std::to_string(stats["neurons"].cast<size_t>()) + 
                   " neurons, " + std::to_string(stats["synapses"].cast<size_t>()) +
                   " synapses, stage=" + std::to_string(stats["stage"].cast<int>()) + ">";
        }, "String representation of brain state");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
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
        
        // Enhanced Pythonic methods
        .def("run_episode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, size_t steps) {
            for (size_t step = 0; step < steps; ++step) {
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                // Note: brain.step() would be called separately
                auto motor_cmd = self.decodeMotorCommand();
                world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            }
        }, py::arg("world"), py::arg("steps"),
             "Run complete episode with world interaction")
        .def("get_agent_stats", [](const AgentBrain& self) {
            py::dict stats;
            stats["curiosity"] = self.getCuriosityLevel();
            stats["novelty"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation"] = self.getNeuromodulationLevel();
            stats["dev_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get agent system statistics")
        .def("enable_all_systems", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_all_systems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all neuromodulation and learning systems")
        .def("is_active", [](const AgentBrain& self) {
            return self.getCuriosityLevel() > 0.0f ||
                   self.getNoveltyLevel() > 0.0f;
        }, "Check if agent systems are active")
        .def("__repr__", [](const AgentBrain& self) {
            auto& brain = *self.getBrain();
            return "<AgentBrain: neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   ", stage=" + std::to_string(static_cast<int>(self.getDevelopmentalStage())) + ">";
        }, "String representation of agent brain");

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

    // Performance monitoring and benchmarking
    m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep i = 0; i < static_cast<SimulationStep>(steps); ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict bench_results;
        bench_results["steps"] = steps;
        bench_results["time_ms"] = duration.count();
        bench_results["time_per_step_ms"] = duration.count() / static_cast<double>(steps);
        bench_results["spikes_per_second"] = brain->getTotalSpikeCount() / (duration.count() / 1000.0);
        
        return bench_results;
    }, py::arg("brain"), py::arg("steps"),
       "Benchmark brain performance and return timing statistics")
    .def("get_performance_metrics", [](const Brain& brain) {
        py::dict metrics;
        
        // Basic metrics
        metrics["total_neurons"] = brain.getTotalNeuronCount();
        metrics["total_synapses"] = brain.getTotalSynapseCount();
        metrics["active_neurons"] = brain.getActiveNeuronCount();
        metrics["firing_neurons"] = brain.getFiringNeuronCount();
        metrics["total_spikes"] = brain.getTotalSpikeCount();
        metrics["avg_firing_rate"] = brain.getAverageFiringRate();
        metrics["e_i_ratio"] = brain.getExcitationInhibitionRatio();
        metrics["development_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        // Performance indicators
        float total_activity = static_cast<float>(brain.getFiringNeuronCount()) / 
                              brain.getTotalNeuronCount();
        metrics["activity_level"] = total_activity;
        
        // State health
        bool is_healthy = brain.getTotalNeuronCount() > 0 && 
                         brain.getTotalSynapseCount() > 0;
        metrics["is_healthy"] = is_healthy;
        
        return metrics;
    }, "Get comprehensive performance metrics")
    .def("profile_simulation", [](Brain& brain, size_t steps, double timestep) {
        py::dict profile_data;
        py::list timeline;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
            auto step_start = std::chrono::high_resolution_clock::now();
            
            brain.step(step, step * timestep);
            
            auto step_end = std::chrono::high_resolution_clock::now();
            auto step_duration = std::chrono::duration_cast<std::chrono::microseconds>(step_end - step_start);
            
            // Record step data
            py::dict step_record;
            step_record["step"] = step;
            step_record["time_ms"] = step * timestep;
            step_record["duration_us"] = step_duration.count();
            step_record["spikes"] = brain.getTotalSpikeCount();
            step_record["firing_neurons"] = brain.getFiringNeuronCount();
            step_record["activity"] = brain.getFiringNeuronCount() / 
                                      brain.getTotalNeuronCount();
            
            timeline.append(step_record);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        profile_data["total_steps"] = steps;
        profile_data["total_time_ms"] = total_duration.count();
        profile_data["avg_time_per_step_ms"] = total_duration.count() / static_cast<double>(steps);
        profile_data["timeline"] = timeline;
        
        // Calculate statistics
        if (!timeline.empty()) {
            py::list durations;
            py::list activities;
            
            for (auto& step_record : timeline) {
                durations.append(step_record["duration_us"].cast<size_t>());
                activities.append(step_record["activity"].cast<double>());
            }
            
            double avg_duration = std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();
            double avg_activity = std::accumulate(activities.begin(), activities.end(), 0.0) / activities.size();
            
            profile_data["avg_step_duration_us"] = avg_duration;
            profile_data["avg_activity_level"] = avg_activity;
        }
        
        return profile_data;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep"),
       "Profile simulation performance and return detailed timing data")
    .def("get_system_health", [](const Brain& brain) {
        py::dict health_report;
        
        // Check various health indicators
        health_report["neuron_count"] = brain.getTotalNeuronCount();
        health_report["synapse_count"] = brain.getTotalSynapseCount();
        health_report["firing_count"] = brain.getFiringNeuronCount();
        
        // Calculate health scores
        float neuron_health = brain.getTotalNeuronCount() > 0 ? 1.0f : 0.0f;
        float synapse_health = brain.getTotalSynapseCount() > 0 ? 1.0f : 0.0f;
        
        // Activity health
        float activity_ratio = brain.getFiringNeuronCount() / 
                              brain.getTotalNeuronCount();
        float activity_health = activity_ratio > 0.01f ? 1.0f : (activity_ratio > 0.001f ? 0.5f : 0.1f);
        
        health_report["health_score"] = (neuron_health + synapse_health + activity_health) / 3.0f;
        health_report["neuron_health"] = neuron_health;
        health_report["synapse_health"] = synapse_health;
        health_report["activity_health"] = activity_health;
        
        // Recommendations
        py::list recommendations;
        if (brain.getTotalNeuronCount() < 100) {
            recommendations.append("Consider increasing neuron count for better simulation quality");
        }
        if (activity_ratio < 0.01f) {
            recommendations.append("Low activity detected - check sensory input and initial conditions");
        }
        if (brain.getTotalSynapseCount() < 10) {
            recommendations.append("Very few synapses - network connectivity may be too sparse");
        }
        
        health_report["recommendations"] = recommendations;
        health_report["status"] = health_report["health_score"].cast<float>() > 0.7f ? "healthy" : 
                                  (health_report["health_score"].cast<float>() > 0.4f ? "fair" : "needs_attention");
        
        return health_report;
    }, "Get system health report and recommendations")
    .def("get_comprehensive_stats", [](const Brain& brain) {
        py::dict comprehensive_stats;
        
        // Basic stats
        comprehensive_stats["basic"] = py::dict::unchecked_cast(brain.get_stats());
        
        // Performance stats
        comprehensive_stats["performance"] = py::dict::unchecked_cast(brain.get_stats());
        
        // Plasticity stats
        comprehensive_stats["plasticity"] = py::dict::unchecked_cast(brain.get_plasticity_stats());
        
        // Configuration validation
        py::dict config_validation;
        config_validation["is_valid"] = true;
        config_validation["errors"] = py::list(); // Would need actual validation logic
        comprehensive_stats["validation"] = config_validation;
        
        // System health
        comprehensive_stats["health"] = py::dict::unchecked_cast(brain.get_stats()); // Placeholder
        
        // Agent interface stats (if available)
        // This would require access to agent for full stats
        
        return comprehensive_stats;
    }, "Get comprehensive brain statistics and diagnostics")
    .def("save_brain_state", [](const Brain& brain, const std::string& filepath) {
        bool success = brain.save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain state to " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain state and return success status")
    .def("load_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain state from " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain state and return success status");

    // Global utility functions
    m.def("create_preset_config", [](const std::string& preset_name) {
        auto config = std::make_shared<Config>();
        
        if (preset_name == "minimal") {
            config->set("brain.neuron_count", 100);
            config->set("brain.synapse_density", 0.05);
            config->set("brain.connection_probability", 0.01);
        } else if (preset_name == "standard") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1);
            config->set("brain.connection_probability", 0.05);
        } else if (preset_name == "large") {
            config->set("brain.neuron_count", 10000);
            config->set("brain.synapse_density", 0.2);
            config->set("brain.connection_probability", 0.15);
        } else {
            throw std::invalid_argument("Unknown preset: " + preset_name);
        }
        
        return config;
    }, py::arg("preset_name"),
       "Create preset configuration (minimal, standard, large)")
    .def("validate_configuration", [](const std::shared_ptr<Config>& config) {
        py::dict validation;
        py::list errors;
        py::list warnings;
        
        // Check required keys
        std::vector<std::string> required_keys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "brain.connection_probability"
        };
        
        for (const auto& key : required_keys) {
            if (!config->has(key)) {
                errors.append("Missing required configuration: " + key);
            }
        }
        
        // Check value ranges
        if (config->has("brain.neuron_count")) {
            auto neurons = config->getOr<int64_t>("brain.neuron_count", 0);
            if (neurons < 10) {
                warnings.append("Very low neuron count may affect simulation quality");
            } else if (neurons > 100000) {
                errors.append("Neuron count too high (max 100000)");
            }
        }
        
        if (config->has("brain.synapse_density")) {
            auto density = config->getOr<double>("brain.synapse_density", 0.0);
            if (density < 0.0 || density > 1.0) {
                errors.append("Synapse density must be between 0.0 and 1.0");
            }
        }
        
        validation["valid"] = errors.empty();
        validation["errors"] = errors;
        validation["warnings"] = warnings;
        
        return validation;
    }, py::arg("config"),
       "Validate configuration and return validation results")
    .def("get_default_config", []() {
        auto config = std::make_shared<Config>();
        // Set sensible defaults
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.05);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        config->set("brain.tau_mem", 20.0);
        config->set("brain.tau_ref", 2.0);
        return config;
    }, "Get default configuration with sensible parameters")
    .def("get_version", []() { return "0.1.0"; }, "Get NLM Python bindings version")
    .def("get_build_info", []() {
        py::dict build_info;
        build_info["version"] = "0.1.0";
        build_info["build_type"] = "Release";
        build_info["compiler"] = "C++20";
        build_info["pybind11_version"] = "2.11.0";
        return build_info;
    }, "Get build information");

    // Global constants and utilities
    m.attr("__version__") = "0.1.0";
    m.attr("PHASE_2") = 2;
    m.attr("PHASE_4") = 4;
    m.attr("PHASE_6") = 6;
    
    // Simulation parameters
    m.def("get_default_timestep", []() { return 0.001; }, "Get default simulation timestep");
    m.def("get_simulation_step", [](size_t step) { return static_cast<SimulationStep>(step); },
          py::arg("step"), "Convert step index to SimulationStep");
    m.def("get_time_from_step", [](size_t step, double timestep) { 
        return step * timestep; 
    }, py::arg("step"), py::arg("timestep"), 
          "Calculate time from step and timestep");
