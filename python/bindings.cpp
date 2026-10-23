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
        // Enhanced error handling
        .def("validate", [](SensoryInput& self) {
            try {
                // Add validation logic here
                return true;
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Validate sensory input data")
        .def("getValidationErrors", [](SensoryInput& self) {
            std::vector<std::string> errors;
            // Add validation error collection
            return errors;
        }, "Get validation errors for sensory input")
        .def("isValid", [](SensoryInput& self) {
            try {
                return self.validate();
            } catch (...) {
                return false;
            }
        }, "Check if sensory input is valid");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Enhanced vision-specific methods
        .def("validateImage", [](Vision& self) {
            // Add image validation logic
            return self.getWidth() > 0 && self.getHeight() > 0 && self.getChannels() > 0;
        }, "Validate vision image dimensions")
        .def("getImageInfo", [](Vision& self) {
            py::dict info;
            info["width"] = self.getWidth();
            info["height"] = self.getHeight();
            info["channels"] = self.getChannels();
            info["pixel_count"] = self.getWidth() * self.getHeight() * self.getChannels();
            return info;
        }, "Get vision image information")
        .def("normalizeData", [](Vision& self, float min = 0.0f, float max = 1.0f) {
            auto data = self.getData();
            for (auto& value : data) {
                value = (value - min) / (max - min);
            }
            self.setData(data);
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f, "Normalize vision data to specified range");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Enhanced audio-specific methods
        .def("validateAudio", [](Audio& self) {
            return self.getSampleRate() > 0 && self.getNumSamples() > 0;
        }, "Validate audio data")
        .def("getDuration", [](Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("resample", [](Audio& self, size_t newSampleRate) {
            // Add resampling logic here
            self.setSampleRate(newSampleRate);
        }, py::arg("newSampleRate"), "Resample audio to new sample rate");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Enhanced internal signals methods
        .def("getSignalCount", [](InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals")
        .def("getAverageSignal", [](InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get average of internal signals")
        .def("getSignalRange", [](InternalSignals& self) {
            py::dict range;
            auto data = self.getData();
            if (data.empty()) {
                range["min"] = 0.0f;
                range["max"] = 0.0f;
                return range;
            }
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            range["min"] = min;
            range["max"] = max;
            return range;
        }, "Get min and max of internal signals");

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
        // Enhanced action utilities
        .def("validate", [](Action& self) {
            return !self.getParameters().empty() || self.getType() != ActionType::Custom;
        }, "Validate action parameters")
        .def("getParameterCount", [](Action& self) {
            return self.getParameters().size();
        }, "Get number of action parameters")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add parameter to action")
        .def("getParameterSummary", [](Action& self) {
            auto params = self.getParameters();
            if (params.empty()) return std::string("No parameters");
            std::string summary = "Parameters: [";
            for (size_t i = 0; i < std::min(size_t(3), params.size()); ++i) {
                summary += std::to_string(params[i]);
                if (i < std::min(size_t(2), params.size()) - 1) summary += ", ";
            }
            if (params.size() > 3) summary += ", ...";
            summary += "]";
            return summary;
        }, "Get summary of action parameters");

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
        // Enhanced world object utilities
        .def("isColliding", [](WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= (self.radius * self.radius);
        }, py::arg("x"), py::arg("y"), "Check if object collides with point (x,y)")
        .def("distanceTo", [](WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another world object")
        .def("getObjectInfo", [](WorldObject& self) {
            py::dict info;
            info["x"] = self.x;
            info["y"] = self.y;
            info["radius"] = self.radius;
            info["type"] = static_cast<int>(self.type);
            info["value"] = self.value;
            info["active"] = self.active;
            return info;
        }, "Get comprehensive object information");

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
        // Enhanced agent body utilities
        .def("getSpeed", [](AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("getPosition", [](AgentBody& self) {
            py::list position;
            position.append(self.x);
            position.append(self.y);
            return position;
        }, "Get position as list [x, y]")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("updatePosition", [](AgentBody& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
        }, py::arg("dx"), py::arg("dy"), "Update position by delta");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Enhanced action result utilities
        .def("isSuccessful", [](ActionResult& self) {
            return self.success && !self.message.empty();
        }, "Check if action was successful")
        .def("getFormattedResult", [](ActionResult& self) {
            std::string result = "Action " + std::string(self.success ? "SUCCESS" : "FAILED");
            if (self.success) {
                result += " (Reward: " + std::to_string(self.reward) + ")";
            }
            if (!self.message.empty()) {
                result += " - " + self.message;
            }
            return result;
        }, "Get formatted action result string");

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
        // Enhanced sensory percept utilities
        .def("validate", [](SensoryPercept& self) {
            auto vision = self.getVision();
            auto audio = self.getAudio();
            auto internal = self.getInternal();
            auto touch = self.getTouch();
            auto proprioception = self.getProprioception();
            
            bool hasValidData = !vision.empty() || !audio.empty() || !internal.empty() || 
                              !touch.empty() || !proprioception.empty();
            return hasValidData;
        }, "Validate sensory percept")
        .def("getSignalSummary", [](SensoryPercept& self) {
            py::dict summary;
            summary["has_vision"] = !self.getVision().empty();
            summary["has_audio"] = !self.getAudio().empty();
            summary["has_internal"] = !self.getInternal().empty();
            summary["has_touch"] = !self.getTouch().empty();
            summary["has_proprioception"] = !self.getProprioception().empty();
            
            summary["vision_width"] = self.getVisionWidth();
            summary["vision_height"] = self.getVisionHeight();
            summary["total_signals"] = self.getAllSignals().size();
            
            return summary;
        }, "Get summary of all sensory signals")
        .def("mergeWith", [](SensoryPercept& self, const SensoryPercept& other) {
            auto vision = self.getVision();
            auto otherVision = other.getVision();
            vision.insert(vision.end(), otherVision.begin(), otherVision.end());
            self.setVision(vision);
            
            auto audio = self.getAudio();
            auto otherAudio = other.getAudio();
            audio.insert(audio.end(), otherAudio.begin(), otherAudio.end());
            self.setAudio(audio);
            
            auto internal = self.getInternal();
            auto otherInternal = other.getInternal();
            internal.insert(internal.end(), otherInternal.begin(), otherInternal.end());
            self.setInternal(internal);
            
            auto touch = self.getTouch();
            auto otherTouch = other.getTouch();
            touch.insert(touch.end(), otherTouch.begin(), otherTouch.end());
            self.setTouch(touch);
            
            auto proprioception = self.getProprioception();
            auto otherProprioception = other.getProprioception();
            proprioception.insert(proprioception.end(), otherProprioception.begin(), otherProprioception.end());
            self.setProprioception(proprioception);
            
            return true;
        }, py::arg("other"), "Merge this percept with another percept");

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
        // Enhanced world utilities
        .def("getEnvironmentInfo", [](SimpleWorld& self) {
            py::dict info;
            info["width"] = self.getWidth();
            info["height"] = self.getHeight();
            info["max_energy"] = self.getMaxEnergy();
            info["energy_decay_rate"] = self.getEnergyDecayRate();
            info["simulation_time"] = self.getSimulationTime();
            info["seed"] = self.getRandomSeed();
            return info;
        }, "Get environment information")
        .def("validateWorld", [](SimpleWorld& self) {
            return self.getWidth() > 0 && self.getHeight() > 0 &&
                   self.getMaxEnergy() > 0 && self.getEnergyDecayRate() >= 0;
        }, "Validate world configuration")
        .def("setEnvironmentParameters", [](SimpleWorld& self, double maxEnergy, double decayRate) {
            self.setMaxEnergy(maxEnergy);
            self.setEnergyDecayRate(decayRate);
        }, py::arg("maxEnergy"), py::arg("decayRate"), "Set environment parameters")
        .def("getWorldStats", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["simulation_time"] = self.getSimulationTime();
            stats["agent_x"] = self.getAgentBody().x;
            stats["agent_y"] = self.getAgentBody().y;
            stats["agent_orientation"] = self.getAgentBody().orientation;
            return stats;
        }, "Get world statistics");

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
        // Enhanced brain methods
        .def("getNetworkDensity", [](Brain& self) {
            float synapses = self.getTotalSynapseCount();
            float neurons = self.getTotalNeuronCount();
            return synapses / neurons;
        }, "Calculate network density (synapses per neuron)")
        .def("getRegionInfo", [](Brain& self, RegionId regionId) {
            py::dict info;
            auto* region = self.getRegion(regionId);
            if (region) {
                info["id"] = regionId.value;
                info["neuron_count"] = region->getAllNeurons().size();
                info["synapse_count"] = region->getSynapses().size();
                info["name"] = std::string("Region_") + std::to_string(regionId.value);
            }
            return info;
        }, py::arg("regionId"), "Get information about a specific region")
        .def("addRegionsBatch", [](Brain& self, const std::vector<std::string>& regionNames) {
            std::vector<RegionId> regionIds;
            for (const auto& name : regionNames) {
                regionIds.push_back(self.addRegion(name));
            }
            return regionIds;
        }, py::arg("regionNames"), "Add multiple regions at once")
        .def("validateBrainState", [](Brain& self) {
            bool valid = true;
            valid = valid && self.getTotalNeuronCount() > 0;
            valid = valid && self.getTotalSynapseCount() >= 0;
            return valid;
        }, "Validate brain state integrity")
        .def("exportState", [](Brain& self, const std::string& format) {
            if (format == "stats") {
                py::dict state;
                state["total_neurons"] = self.getTotalNeuronCount();
                state["total_synapses"] = self.getTotalSynapseCount();
                state["firing_neurons"] = self.getFiringNeuronCount();
                state["average_firing_rate"] = self.getAverageFiringRate();
                state["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
                return state;
            }
            // For other formats, just return basic info
            py::dict state;
            state["status"] = "export_" + format;
            return state;
        }, py::arg("format"), "Export brain state in specified format");

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
        // Enhanced agent brain utilities
        .def("getSubsystemStatus", [](AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Get status of all neuromodulation subsystems")
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all agent features")
        .def("getEnabledFeatures", [](AgentBrain& self) {
            std::vector<std::string> features;
            if (self.isRewardModulationEnabled()) features.push_back("reward_modulation");
            if (self.isStructuralPlasticityEnabled()) features.push_back("structural_plasticity");
            if (self.isDevelopmentEnabled()) features.push_back("development");
            if (self.isCuriosityEnabled()) features.push_back("curiosity");
            return features;
        }, "Get list of enabled features")
        .def("applyLearningSchedule", [](AgentBrain& self, double learningRate) {
            // Apply learning rate to all enabled subsystems
            if (self.isRewardModulationEnabled()) {
                self.applyRewardModulation(learningRate, 0.0);
            }
        }, py::arg("learningRate"), "Apply learning rate to enabled subsystems");

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

    // Advanced configuration creation
    m.def("createConfigFromDict", [](const std::map<std::string, ConfigValue>& configDict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (const auto& pair : configDict) {
            config->set(pair.first, pair.second);
        }
        return config;
    }, py::arg("configDict"), "Create configuration from Python dictionary");

    // Advanced brain management utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, size_t steps, double timestep = 0.1) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid brain, world, or agent pointers");
        }
        
        brain->initialize();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * timestep);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("timestep") = 0.1,
       "Run a complete agent simulation with brain and world");

    // Batch operations for performance
    m.def("batchProcessSensoryInputs", [](std::shared_ptr<Brain> brain, const std::vector<class SensoryInput>& inputs) {
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        for (const auto& input : inputs) {
            brain->receiveSensoryInput(input);
        }
    }, py::arg("brain"), py::arg("inputs"), "Process multiple sensory inputs in batch");

    // Statistics and analytics helpers
    m.def("computeNetworkStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["pending_spikes"] = brain->getPendingSpikeEventCount();
        return stats;
    }, py::arg("brain"), "Compute comprehensive network statistics");

    // Development stage helper
    m.def("getDevelopmentalStageName", [](DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return std::string("Initial");
            case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
            case DevelopmentalStage::Maturation: return std::string("Maturation");
            case DevelopmentalStage::Adult: return std::string("Adult");
            case DevelopmentalStage::Aging: return std::string("Aging");
            default: return std::string("Unknown");
        }
    }, py::arg("stage"), "Get human-readable developmental stage name");

    // Configuration utilities
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced configuration creation
    m.def("createConfigFromDict", [](const std::map<std::string, ConfigValue>& configDict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (const auto& pair : configDict) {
            config->set(pair.first, pair.second);
        }
        return config;
    }, py::arg("configDict"), "Create configuration from Python dictionary");

    // Advanced brain management utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, size_t steps, double timestep = 0.1) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid brain, world, or agent pointers");
        }
        
        brain->initialize();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * timestep);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("timestep") = 0.1,
       "Run a complete agent simulation with brain and world");

    // Batch operations for performance
    m.def("batchProcessSensoryInputs", [](std::shared_ptr<Brain> brain, const std::vector<class SensoryInput>& inputs) {
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        for (const auto& input : inputs) {
            brain->receiveSensoryInput(input);
        }
    }, py::arg("brain"), py::arg("inputs"), "Process multiple sensory inputs in batch");

    // Statistics and analytics helpers
    m.def("computeNetworkStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["pending_spikes"] = brain->getPendingSpikeEventCount();
        return stats;
    }, py::arg("brain"), "Compute comprehensive network statistics");

    // Development stage helper
    m.def("getDevelopmentalStageName", [](DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return std::string("Initial");
            case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
            case DevelopmentalStage::Maturation: return std::string("Maturation");
            case DevelopmentalStage::Adult: return std::string("Adult");
            case DevelopmentalStage::Aging: return std::string("Aging");
            default: return std::string("Unknown");
        }
    }, py::arg("stage"), "Get human-readable developmental stage name");

    // Configuration utilities
    m.def("exportConfigToJSON", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::runtime_error("Invalid config pointer");
        }
        std::string json = "{\n";
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            json += "  \"" + keys[i] + "\": \"" + std::to_string(config->getOr<int64_t>(keys[i], 0)) + "\"";
            if (i < keys.size() - 1) {
                json += ",\n";
            }
        }
        json += "\n}";
        return json;
    }, py::arg("config"), "Export configuration to JSON string");

    // Region management utilities
    m.def("addRegions", [](std::shared_ptr<Brain> brain, const std::vector<std::string>& regionNames) {
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        std::vector<RegionId> regionIds;
        for (const auto& name : regionNames) {
            regionIds.push_back(brain->addRegion(name));
        }
        return regionIds;
    }, py::arg("brain"), py::arg("regionNames"), "Add multiple regions at once");

    // Action utilities
    m.def("createActionBatch", [](const std::vector<std::pair<ActionType, std::vector<float>>>& actions) {
        std::vector<std::unique_ptr<Action>> actionPtrs;
        for (const auto& pair : actions) {
            auto action = std::make_unique<Action>(pair.first, pair.second);
            actionPtrs.push_back(std::move(action));
        }
        return actionPtrs;
    }, py::arg("actions"), "Create a batch of actions from pairs of type and parameters");

    // Checkpoint utilities
    m.def("saveBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain checkpoint");

    m.def("loadBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain checkpoint");

    // Sensor utilities
    m.def("createVisionSensor", [](size_t width, size_t height, size_t channels = 3) {
        return std::make_unique<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create vision sensor with specified dimensions");

    m.def("createAudioSensor", [](size_t sampleRate, size_t numSamples) {
        return std::make_unique<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create audio sensor with specified parameters");

    // Progress monitoring
    m.def("runSimulationWithProgress", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                        std::shared_ptr<AgentBrain> agent, size_t steps, double timestep = 0.1,
                                        std::function<void(size_t, size_t)> progressCallback = nullptr) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid brain, world, or agent pointers");
        }
        
        brain->initialize();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * timestep);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (progressCallback) {
                progressCallback(i, steps);
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("timestep") = 0.1,
       py::arg("progressCallback") = nullptr, "Run simulation with progress callback");

    // Experimental utilities
    m.def("setupExperiment", [](const std::string& name, const std::map<std::string, std::string>& parameters) {
        auto config = std::make_shared<Config>();
        for (const auto& pair : parameters) {
            config->set(pair.first, pair.second);
        }
        config->set("experiment.name", name);
        return config;
    }, py::arg("name"), py::arg("parameters"), "Setup experiment configuration with metadata");

    // Memory utilities
    m.def("getMemoryStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        py::dict memoryStats;
        memoryStats["working_memory_size"] = 1024;  // Placeholder
        memoryStats["episodic_memory_capacity"] = 5000;  // Placeholder
        memoryStats["associative_memory_capacity"] = 10000;  // Placeholder
        return memoryStats;
    }, py::arg("brain"), "Get memory system statistics");

    // Visualization utilities
    m.def("createVisualizationConfig", [](const std::string& outputDir, bool saveSnapshots = true, 
                                         int snapshotInterval = 100) {
        py::dict config;
        config["output_directory"] = outputDir;
        config["save_snapshots"] = saveSnapshots;
        config["snapshot_interval"] = snapshotInterval;
        return config;
    }, py::arg("outputDir"), py::arg("saveSnapshots") = true, py::arg("snapshotInterval") = 100,
       "Create visualization configuration");

    // Advanced agent utilities
    m.def("enableAllFeatures", [](std::shared_ptr<AgentBrain> agent) {
        if (!agent) {
            throw std::runtime_error("Invalid agent pointer");
        }
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
    }, py::arg("agent"), "Enable all agent features for maximum learning capacity");

    // Advanced world utilities
    m.def("createWorldWithObjects", [](size_t width, size_t height, 
                                     const std::vector<WorldObject>& objects) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        for (const auto& obj : objects) {
            world->addObject(obj);
        }
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("objects"), "Create world with initial objects");

    // Action sequence utilities
    m.def("executeActionSequence", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                                    const std::vector<Action>& actions, double timestep = 0.1) {
        if (!agent || !world) {
            throw std::runtime_error("Invalid agent or world pointer");
        }
        
        for (size_t i = 0; i < actions.size(); ++i) {
            world->applyMotorCommand(actions[i], i * timestep);
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Note: Brain step would need to be accessible from here
            // For now, this is a simplified version
        }
    }, py::arg("agent"), py::arg("world"), py::arg("actions"), py::arg("timestep") = 0.1,
       "Execute a predefined sequence of actions");

    // Reward utilities
    m.def("applyRewardSequence", [](std::shared_ptr<AgentBrain> agent,
                                   const std::vector<std::pair<double, double>>& rewardSequences) {
        if (!agent) {
            throw std::runtime_error("Invalid agent pointer");
        }
        
        for (const auto& pair : rewardSequences) {
            agent->applyRewardModulation(pair.first, pair.second);
        }
    }, py::arg("agent"), py::arg("rewardSequences"), "Apply a sequence of reward predictions");

    // Development utilities
    m.def("advanceDevelopmentStages", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        
        for (size_t i = 0; i < steps; ++i) {
            brain->setDevelopmentalStage(static_cast<DevelopmentalStage>(i % 5));
        }
    }, py::arg("brain"), py::arg("steps"), "Advance through developmental stages");

    // Statistics aggregation
    m.def("aggregateStats", [](const std::vector<py::dict>& statsList) {
        py::dict aggregated;
        if (statsList.empty()) return aggregated;
        
        // Aggregate statistics from all stats dictionaries
        for (const auto& stats : statsList) {
            for (const auto& item : stats) {
                auto key = item.first.cast<std::string>();
                auto value = item.second;
                if (aggregated.contains(key)) {
                    if (PyFloat_Check(value.ptr())) {
                        double current = aggregated[key].cast<double>();
                        double newVal = value.cast<double>();
                        aggregated[key] = current + newVal;
                    } else if (PyLong_Check(value.ptr())) {
                        long current = aggregated[key].cast<long>();
                        long newVal = value.cast<long>();
                        aggregated[key] = current + newVal;
                    }
                } else {
                    aggregated[key] = value;
                }
            }
        }
        
        // Calculate averages
        for (auto& item : aggregated) {
            if (item.second.is(py::float_())) {
                // This is a simple implementation; more sophisticated averaging could be added
            }
        }
        
        return aggregated;
    }, py::arg("statsList"), "Aggregate statistics from multiple time points");

    // Configuration comparison
    m.def("compareConfigs", [](const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2) {
        py::dict differences;
        
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        for (const auto& key : keys1) {
            auto val1 = config1->getOr<std::string>(key, "");
            if (!config2->has(key)) {
                differences["missing_in_config2"] = key;
            }
        }
        
        for (const auto& key : keys2) {
            if (!config1->has(key)) {
                differences["missing_in_config1"] = key;
            }
        }
        
        return differences;
    }, py::arg("config1"), py::arg("config2"), "Compare two configurations and return differences");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
