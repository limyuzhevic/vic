#include <sstream>
#include <iomanip>

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

    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, "Neuron ID value")
        .def("index", &NeuronId::index, "Get index of neuron ID")
        .def("__eq__", &NeuronId::operator==, "Compare neuron IDs for equality")
        .def("__ne__", &NeuronId::operator!=, "Compare neuron IDs for inequality")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("to_string", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "Convert to string representation")
        .def("is_valid", &NeuronId::index, "Check if the ID is valid (non-zero)")
        .def_static("from_value", [](uint64_t value) { return NeuronId(value); },
            py::arg("value"), "Create a NeuronId from a value");

    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value, "Synapse ID value")
        .def("index", &SynapseId::index, "Get index of synapse ID")
        .def("__eq__", &SynapseId::operator==, "Compare synapse IDs for equality")
        .def("__ne__", &SynapseId::operator!=, "Compare synapse IDs for inequality")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("to_string", [](const SynapseId& id) {
            return std::to_string(id.value);
        }, "Convert to string representation")
        .def("is_valid", &SynapseId::index, "Check if the ID is valid (non-zero)")
        .def_static("from_value", [](uint64_t value) { return SynapseId(value); },
            py::arg("value"), "Create a SynapseId from a value");

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
        })
        .def("to_string", [](const RegionId& id) {
            return std::to_string(id.value);
        }, "Convert to string representation")
        .def("is_valid", &RegionId::index, "Check if the ID is valid (non-zero)")
        .def_static("from_value", [](uint64_t value) { return RegionId(value); },
            py::arg("value"), "Create a RegionId from a value");

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
        .def("get", &Config::get, py::arg("key"),
             "Get a configuration value by key")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration key-value pair")
        .def("setIfNotExists", [](Config& self, const std::string& key, const std::string& value) {
            if (!self.has(key)) {
                self.set(key, value);
            }
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value only if the key doesn't exist")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration into this one")
        .def("validate", &Config::validate,
             "Validate configuration and return error messages if any")
        .def("toDict", &Config::toDict,
             "Convert configuration to a Python dictionary")
        .def("fromDict", [](Config& self, const py::dict& d) {
            nlohmann::json j;
            for (auto it : d) {
                j[it.first.cast<std::string>()] = it.second;
            }
            self.merge(j);
        }, py::arg("dict"),
           "Load configuration from a Python dictionary")
        .def("copy", [](const Config& self) {
            return std::make_shared<Config>(self);
        }, "Return a copy of this configuration");

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
        .def("getPixelCount", &Vision::getWidth, "Get total number of pixels")
        .def("getAspectRatio", [](const Vision& self) {
            return static_cast<float>(self.getWidth()) / self.getHeight();
        }, "Get aspect ratio (width/height)")
        .def("isGrayscale", [](const Vision& self) {
            return self.getChannels() == 1;
        }, "Check if image is grayscale")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            // Implementation would need to be added to Vision class
            throw std::runtime_error("Vision.resize() not yet implemented");
        }, py::arg("width"), py::arg("height"),
           "Resize the image to specified dimensions")
        .def("normalize", [](Vision& self, float min, float max) {
            // Implementation would need to be added to Vision class
            throw std::runtime_error("Vision.normalize() not yet implemented");
        }, py::arg("min"), py::arg("max"),
           "Normalize pixel values to specified range [min, max]")
        .def("flipHorizontal", [](Vision& self) {
            // Implementation would need to be added to Vision class
            throw std::runtime_error("Vision.flipHorizontal() not yet implemented");
        }, "Flip image horizontally");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("getAmplitude", [](const Audio& self) {
            if (self.getNumSamples() == 0) return 0.0f;
            float maxVal = 0.0f;
            const auto& data = self.getData();
            for (float sample : data) {
                maxVal = std::max(maxVal, std::abs(sample));
            }
            return maxVal;
        }, "Get maximum amplitude")
        .def("normalize", [](Audio& self, float targetAmplitude) {
            auto& data = self.getData();
            if (data.empty()) return;
            float currentAmp = self.getAmplitude();
            if (currentAmp > 0.0f) {
                float scale = targetAmplitude / currentAmp;
                for (auto& sample : data) {
                    sample *= scale;
                }
            }
        }, py::arg("targetAmplitude"),
           "Normalize audio to specified target amplitude")
        .def("filterLowPass", [](Audio& self, float cutoffFreq, float sampleRate) {
            // Implementation would need to be added to Audio class
            throw std::runtime_error("Audio.filterLowPass() not yet implemented");
        }, py::arg("cutoffFreq"), py::arg("sampleRate"),
           "Apply low-pass filter");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getSignals)
        .def("getSignalCount", &InternalSignals::getSignals,
             "Get the number of signals (alias for getSignals)")
        .def("getMean", [](const InternalSignals& self) {
            const auto& signals = self.getSignals();
            if (signals.empty()) return 0.0f;
            float sum = 0.0f;
            for (float sig : signals) sum += sig;
            return sum / signals.size();
        }, "Get mean value of signals")
        .def("getStdDev", [](const InternalSignals& self) {
            const auto& signals = self.getSignals();
            if (signals.size() <= 1) return 0.0f;
            float mean = self.getMean();
            float sumSq = 0.0f;
            for (float sig : signals) {
                float diff = sig - mean;
                sumSq += diff * diff;
            }
            return std::sqrt(sumSq / (signals.size() - 1));
        }, "Get standard deviation of signals")
        .def("getMin", [](const InternalSignals& self) {
            const auto& signals = self.getSignals();
            if (signals.empty()) return 0.0f;
            return *std::min_element(signals.begin(), signals.end());
        }, "Get minimum value")
        .def("getMax", [](const InternalSignals& self) {
            const auto& signals = self.getSignals();
            if (signals.empty()) return 0.0f;
            return *std::max_element(signals.begin(), signals.end());
        }, "Get maximum value");

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
        .def("isValid", [](const Action& self) {
            return self.getType() != ActionType::Custom || !self.getParameters().empty();
        }, "Check if action is valid")
        .def("getParameterCount", &Action::getParameters,
             "Get number of parameters (alias for getParameters)")
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            return params[index];
        }, py::arg("index"),
           "Get parameter value by index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            params[index] = value;
        }, py::arg("index"), py::arg("value"),
           "Set parameter value by index")
        .def("addParameter", &Action::getParameters, // Note: this might need a different implementation
             "Add a new parameter (alias for getParameters)")
        .def("to_dict", [](const Action& self) {
            py::dict d;
            d["type"] = self.getType();
            d["parameters"] = self.getParameters();
            d["name"] = self.getName();
            return d;
        }, "Convert action to Python dictionary")
        .def("from_dict", [](Action& self, const py::dict& d) {
            self.setType(d["type"].cast<ActionType>());
            self.setParameters(d["parameters"].cast<std::vector<float>>());
        }, py::arg("dict"),
           "Load action from Python dictionary");

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
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"),
           "Calculate Euclidean distance to another object")
        .def("isActive", &WorldObject::active,
             "Check if object is active (alias for active property)")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float distance = self.distanceTo(other);
            return distance < (self.radius + other.radius);
        }, py::arg("other"),
           "Check if colliding with another object")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
           "Set object position")
        .def("to_string", [](const WorldObject& self) {
            std::ostringstream oss;
            oss << "WorldObject(x=" << self.x << ", y=" << self.y
                << ", type=" << static_cast<int>(self.type)
                << ", radius=" << self.radius << ", active=" << self.active << ")";
            return oss.str();
        }, "Return string representation");

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
        .def("distanceTo", [](const AgentBody& self, const AgentBody& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"),
           "Calculate distance to another agent body")
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
           "Set position")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getAngularVelocity", &AgentBody::angularVelocity)
        .def("isActive", [](const AgentBody& self) {
            return self.energy > 0.0f && self.health > 0.0f && self.age < 1000;
        }, "Check if agent body is active")
        .def("to_string", [](const AgentBody& self) {
            std::ostringstream oss;
            oss << "AgentBody(x=" << self.x << ", y=" << self.y
                << ", orientation=" << self.orientation
                << ", energy=" << self.energy
                << ", health=" << self.health << ", age=" << self.age << ")";
            return oss.str();
        }, "Return string representation");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", &ActionResult::success,
             "Check if action was successful (alias for success property)")
        .def("to_string", [](const ActionResult& self) {
            return "ActionResult(reward=" + std::to_string(self.reward)
                   + ", success=" + (self.success ? "true" : "false")
                   + ", message=\"" + self.message + "\")";
        }, "Return string representation")
        .def("getFormattedReward", [](const ActionResult& self) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << self.reward;
            return oss.str();
        }, "Get formatted reward string with 3 decimal places");

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
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVisionWidth().empty() && !self.getVisionHeight().empty();
        }, "Check if vision data is present")
        .def("hasTouch", [](const SensoryPercept& self) {
            const auto& touch = self.getTouch();
            return !touch.empty();
        }, "Check if touch data is present")
        .def("hasInternal", [](const SensoryPercept& self) {
            const auto& internal = self.getInternal();
            return !internal.empty();
        }, "Check if internal signals are present")
        .def("hasAudio", [](const SensoryPercept& self) {
            const auto& audio = self.getAudio();
            return !audio.empty();
        }, "Check if audio data is present")
        .def("isValid", [](const SensoryPercept& self) {
            return self.getTimestamp() > 0 && (self.hasVision() || self.hasTouch() || self.hasInternal() || self.hasAudio());
        }, "Validate sensory percept and return True if valid")
        .def("to_string", [](const SensoryPercept& self) {
            std::ostringstream oss;
            oss << "SensoryPercept(vision=" << self.getVisionWidth() << "x" << self.getVisionHeight()
                << ", touch=" << self.getTouch().size()
                << ", internal=" << self.getInternal().size()
                << ", audio=" << self.getAudio().size()
                << ", timestamp=" << self.getTimestamp() << ")";
            return oss.str();
        }, "Return string representation");

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
             "Log brain status")
        .def("getStatistics", &Brain::getStatistics,
             "Get comprehensive brain statistics as dictionary")
        .def("validateState", &Brain::validateState,
             "Validate brain state and return error messages if any")
        .def("getNetworkDensity", &Brain::getNetworkDensity,
             "Calculate network density (synapses per neuron)")
        .def("getExcitatoryInhibitoryBalance", &Brain::getExcitationInhibitionRatio,
             "Get excitatory/inhibitory balance (alias for getExcitationInhibitionRatio)")
        .def("createRegion", [](Brain& self, const std::string& name) {
            return self.addRegion(name);
        }, py::arg("name"),
           "Create a new region and return its ID (alias for addRegion)")
        .def("removeRegion", [](Brain& self, const RegionId& id) {
            self.removeRegion(id);
        }, py::arg("id"),
           "Remove a region by ID")
        .def("getRegionByName", &Brain::getRegionByName, py::arg("name"),
             py::return_value_policy::reference_internal,
             "Get region by name")
        .def("getActiveNeurons", &Brain::getActiveNeurons,
             "Get list of active neuron IDs")
        .def("getFiringNeurons", &Brain::getFiringNeurons,
             "Get list of firing neuron IDs")
        .def("getSpikeHistory", &Brain::getSpikeHistory,
             "Get spike history over time")
        .def("resetStatistics", &Brain::resetStatistics,
             "Reset brain statistics counters");

    py::class_<AgentBrain>(m, "AgentBrain")
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
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("setAllFeaturesEnabled", [](AgentBrain& self, bool enable) {
            self.enableRewardModulation(enable);
            self.enableStructuralPlasticity(enable);
            self.enableDevelopment(enable);
            self.enableCuriosity(enable);
        }, py::arg("enable"),
           "Enable or disable all features (reward modulation, structural plasticity, development, curiosity)")
        .def("getAllFeaturesEnabled", &AgentBrain::isRewardModulationEnabled,
             "Get if all features are enabled (alias for isRewardModulationEnabled)")
        .def("processEpisode", [](AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
            for (const auto& percept : percepts) {
                self.processSensoryInput(percept);
            }
            return self.decodeMotorCommand();
        }, py::arg("percepts"),
           "Process a sequence of sensory percepts and return motor commands for the episode")
        .def("runEpisode", [](AgentBrain& self, const std::shared_ptr<SimpleWorld>& world,
                           const std::vector<SensoryPercept>& percepts,
                           int maxSteps = 100) {
            self.initialize(world);
            std::vector<Action> actions;
            for (int step = 0; step < maxSteps; ++step) {
                Action action = self.decodeMotorCommand();
                actions.push_back(action);
                // Apply action to world would require additional world interface
            }
            return actions;
        }, py::arg("world"), py::arg("percepts"), py::arg("maxSteps") = 100,
           "Run an entire episode with given world and initial percepts, returning list of actions")
        .def("to_string", [](const AgentBrain& self) {
            std::ostringstream oss;
            oss << "AgentBrain(";
            oss << "reward_modulation=" << (self.isRewardModulationEnabled() ? "enabled" : "disabled");
            oss << ", structural_plasticity=" << (self.isStructuralPlasticityEnabled() ? "enabled" : "disabled");
            oss << ", development=" << (self.isDevelopmentEnabled() ? "enabled" : "disabled");
            oss << ", curiosity=" << (self.isCuriosityEnabled() ? "enabled" : "disabled");
            oss << ")";
            return oss.str();
        }, "Return string representation");

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
