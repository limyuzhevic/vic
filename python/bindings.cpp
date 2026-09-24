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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
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
        .def("clone", &SensoryInput::clone, "Create a copy of this sensory input")
        .def("_get_type", &SensoryInput::getType, py::return_value_policy::copy)
        .def("_get_data", &SensoryInput::getData, py::return_value_policy::copy)
        .def("_get_dimensions", &SensoryInput::getDimensions)
        .def("_set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"));

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set image data from vector")
        .def("setData", [](Vision& self, const py::array_t<float>& data) {
            if (!data) {
                throw py::value_error("Numpy array cannot be null");
            }
            auto buf = data.request();
            if (buf.size == 0) {
                throw py::value_error("Numpy array cannot be empty");
            }
            std::vector<float> temp_data(static_cast<float*>(buf.ptr), static_cast<float*>(buf.ptr) + buf.size);
            self.setData(temp_data);
        }, py::arg("data"), "Set image data from numpy array")
        .def("validateData", [](const Vision& self) {
            const auto& data = self.getData();
            if (data.empty()) {
                throw py::value_error("Vision data is empty");
            }
            if (data.size() != self.getWidth() * self.getHeight() * self.getChannels()) {
                throw py::value_error("Vision data size does not match dimensions");
            }
            return true;
        }, "Validate vision data dimensions")
        .def("normalize", [](Vision& self, float min = 0.0f, float max = 1.0f) {
            auto& data = self.getData();
            if (data.empty()) {
                return;
            }
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val - min_val < 1e-6f) {
                return; // All values are the same
            }
            for (auto& val : data) {
                val = min + (val - min_val) * (max - min) / (max_val - min_val);
            }
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f, "Normalize vision data to range")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getShape", &Vision::getDimensions)
        .def("reshape", [](Vision& self, size_t width, size_t height, size_t channels) {
            if (width == 0 || height == 0 || channels == 0) {
                throw py::value_error("Dimensions cannot be zero");
            }
            // Note: This is a placeholder implementation
            return self;
        }, py::arg("width"), py::arg("height"), py::arg("channels"), "Create new Vision with different shape")
        .def("resize", [](Vision& self, size_t width, size_t height) {
            if (width == 0 || height == 0) {
                throw py::value_error("Dimensions cannot be zero");
            }
            // Note: This is a placeholder implementation
            return self;
        }, py::arg("width"), py::arg("height"), "Resize image")
        .def("getPixel", [](const Vision& self, size_t x, size_t y, size_t c = 0) {
            if (x >= self.getWidth() || y >= self.getHeight() || c >= self.getChannels()) {
                throw py::index_error("Pixel coordinates out of bounds");
            }
            size_t index = ((y * self.getWidth()) + x) * self.getChannels() + c;
            return self.getData()[index];
        }, py::arg("x"), py::arg("y"), py::arg("c") = 0, "Get pixel value at position (x, y) channel c")
        .def("setPixel", [](Vision& self, size_t x, size_t y, size_t c, float value) {
            if (x >= self.getWidth() || y >= self.getHeight() || c >= self.getChannels()) {
                throw py::index_error("Pixel coordinates out of bounds");
            }
            size_t index = ((y * self.getWidth()) + x) * self.getChannels() + c;
            self.getData()[index] = value;
        }, py::arg("x"), py::arg("y"), py::arg("c"), py::arg("value"), "Set pixel value at position (x, y) channel c")
        .def("getDataAsArray", [](const Vision& self) {
            return py::array_t<float>(self.getData().size(), self.getData().data());
        }, "Get vision data as numpy array (read-only)")
        .def("copyData", [](const Vision& self) {
            return self.getData();
        }, "Copy vision data as Python list");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data from vector")
        .def("setData", [](Audio& self, const py::array_t<float>& data) {
            if (!data) {
                throw py::value_error("Numpy array cannot be null");
            }
            auto buf = data.request();
            if (buf.size == 0) {
                throw py::value_error("Numpy array cannot be empty");
            }
            std::vector<float> temp_data(static_cast<float*>(buf.ptr), static_cast<float*>(buf.ptr) + buf.size);
            self.setData(temp_data);
        }, py::arg("data"), "Set audio data from numpy array")
        .def("validateData", [](const Audio& self) {
            const auto& data = self.getData();
            if (data.empty()) {
                throw py::value_error("Audio data is empty");
            }
            if (data.size() != self.getNumSamples()) {
                throw py::value_error("Audio data size does not match sample count");
            }
            return true;
        }, "Validate audio data dimensions")
        .def("normalize", [](Audio& self, float min = 0.0f, float max = 1.0f) {
            auto& data = self.getData();
            if (data.empty()) {
                return;
            }
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val - min_val < 1e-6f) {
                return; // All values are the same
            }
            for (auto& val : data) {
                val = min + (val - min_val) * (max - min) / (max_val - min_val);
            }
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f, "Normalize audio data to range")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", &Audio::getNumSamples)
        .def("getFrequency", [](const Audio& self) {
            // Get first non-zero frequency component (placeholder)
            if (self.getData().empty()) {
                return 0.0f;
            }
            float max_val = *std::max_element(self.getData().begin(), self.getData().end());
            return max_val;
        })
        .def("getRMS", [](const Audio& self) {
            const auto& data = self.getData();
            if (data.empty()) {
                return 0.0f;
            }
            float sum_squares = 0.0f;
            for (float val : data) {
                sum_squares += val * val;
            }
            return std::sqrt(sum_squares / data.size());
        }, "Get root mean square (RMS) of audio signal")
        .def("getPeak", [](const Audio& self) {
            const auto& data = self.getData();
            if (data.empty()) {
                return 0.0f;
            }
            return *std::max_element(data.begin(), data.end());
        }, "Get peak amplitude of audio signal")
        .def("getEnvelope", [](const Audio& self) {
            const auto& data = self.getData();
            if (data.empty()) {
                return std::vector<float>();
            }
            std::vector<float> envelope(data.size());
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val < 1e-6f) {
                return envelope;
            }
            for (size_t i = 0; i < data.size(); ++i) {
                envelope[i] = std::abs(data[i]) / max_val;
            }
            return envelope;
        }, "Get audio envelope (normalized amplitude)")
        .def("getDataAsArray", [](const Audio& self) {
            return py::array_t<float>(self.getData().size(), self.getData().data());
        }, "Get audio data as numpy array (read-only)")
        .def("copyData", [](const Audio& self) {
            return self.getData();
        }, "Copy audio data as Python list")
        .def("getDataSlice", [](const Audio& self, size_t start, size_t end) {
            if (start > end || end > self.getData().size()) {
                throw py::index_error("Slice indices out of bounds");
            }
            return std::vector<float>(self.getData().begin() + start, self.getData().begin() + end);
        }, py::arg("start"), py::arg("end"), "Get audio data slice as vector")
        .def("getSampleAt", [](const Audio& self, size_t index) {
            if (index >= self.getData().size()) {
                throw py::index_error("Sample index out of bounds");
            }
            return self.getData()[index];
        }, py::arg("index"), "Get audio sample at index")
        .def("setSampleAt", [](Audio& self, size_t index, float value) {
            if (index >= self.getData().size()) {
                throw py::index_error("Sample index out of bounds");
            }
            self.getData()[index] = value;
        }, py::arg("index"), py::arg("value"), "Set audio sample at index");

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("getSignal", [](const SensoryPercept& self, const std::string& type) {
            if (type == "vision") return self.getVision();
            else if (type == "touch") return self.getTouch();
            else if (type == "internal") return self.getInternal();
            else if (type == "proprioception") return self.getProprioception();
            else if (type == "audio") return self.getAudio();
            else throw py::value_error("Unknown signal type: " + type);
        }, py::arg("type"), "Get signal data by type (vision, touch, internal, proprioception, audio)")
        .def("setSignal", [](SensoryPercept& self, const std::string& type, const py::array_t<float>& data) {
            auto buf = data.request();
            std::vector<float> temp_data(static_cast<float*>(buf.ptr), static_cast<float*>(buf.ptr) + buf.size);
            if (type == "vision") self.setVision(Vision(temp_data));
            else if (type == "touch") self.setTouch(Vision(temp_data));
            else if (type == "internal") self.setInternal(InternalSignals(temp_data));
            else if (type == "proprioception") self.setProprioception(Vision(temp_data));
            else if (type == "audio") self.setAudio(Audio(temp_data));
            else throw py::value_error("Unknown signal type: " + type);
        }, py::arg("type"), py::arg("data"), "Set signal data by type")
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (!vision.getData().empty()) {
                result["vision"] = vision.getData();
                result["vision_width"] = vision.getWidth();
                result["vision_height"] = vision.getHeight();
                result["vision_channels"] = vision.getChannels();
            }
            auto touch = self.getTouch();
            if (!touch.getData().empty()) {
                result["touch"] = touch.getData();
            }
            auto internal = self.getInternal();
            if (!internal.getData().empty()) {
                result["internal"] = internal.getData();
            }
            auto proprioception = self.getProprioception();
            if (!proprioception.getData().empty()) {
                result["proprioception"] = proprioception.getData();
            }
            auto audio = self.getAudio();
            if (!audio.getData().empty()) {
                result["audio"] = audio.getData();
                result["sample_rate"] = audio.getSampleRate();
                result["num_samples"] = audio.getNumSamples();
            }
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert percept to Python dictionary")
        .def("from_dict", [](SensoryPercept& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::cast<std::string>(item.first);
                if (key == "vision") {
                    auto vision_data = py::cast<std::vector<float>>(item.second);
                    self.setVision(Vision(vision_data));
                }
                else if (key == "touch") {
                    auto touch_data = py::cast<std::vector<float>>(item.second);
                    self.setTouch(Vision(touch_data));
                }
                else if (key == "internal") {
                    auto internal_data = py::cast<std::vector<float>>(item.second);
                    self.setInternal(InternalSignals(internal_data));
                }
                else if (key == "proprioception") {
                    auto proprioception_data = py::cast<std::vector<float>>(item.second);
                    self.setProprioception(Vision(proprioception_data));
                }
                else if (key == "audio") {
                    auto audio_data = py::cast<std::vector<float>>(item.second);
                    self.setAudio(Audio(audio_data));
                }
                else if (key == "sample_rate") {
                    auto sample_rate = py::cast<size_t>(item.second);
                    // Note: Audio sample rate setter is accessed via Audio class
                }
                else if (key == "num_samples") {
                    auto num_samples = py::cast<size_t>(item.second);
                    // Note: Audio num_samples is accessed via Audio class
                }
                else if (key == "timestamp") {
                    auto timestamp = py::cast<double>(item.second);
                    self.setTimestamp(timestamp);
                }
            }
        }, py::arg("data"), "Update percept from Python dictionary");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            if (width == 0 || height == 0) {
                throw py::value_error("World dimensions cannot be zero");
            }
            if (visionWidth > width || visionHeight > height) {
                throw py::value_error("Vision dimensions cannot exceed world dimensions");
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", [](SimpleWorld& self, double x, double y) {
            if (!self.isValidPosition(x, y)) {
                throw py::value_error("Position is not valid for removal");
            }
            self.removeObject(x, y);
        }, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getVisionWidth", []() -> size_t { return 0; }, "Get vision width (placeholder)")
        .def("getVisionHeight", []() -> size_t { return 0; }, "Get vision height (placeholder)")
        .def("getSize", []() -> std::pair<size_t, size_t> { return {0, 0}; }, "Get world size")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("getDeltaTime", []() -> double { return 0.1; }, "Get time step (default)")
        .def("setDeltaTime", [](SimpleWorld& self, double dt) {
            if (dt <= 0.0) {
                throw py::value_error("Time step must be positive");
            }
            if (dt > 1.0) {
                throw py::value_error("Time step cannot exceed 1.0");
            }
            // Note: SimpleWorld doesn't have setDeltaTime, would need modification
            return dt;
        }, py::arg("dt"), "Set time step (placeholder)")
        .def("createDefaultObject", [](SimpleWorld& self, double x, double y, const std::string& type) {
            if (!self.isValidPosition(x, y)) {
                throw py::value_error("Invalid position for creating object");
            }
            WorldObject obj(x, y, WorldObjectType::Empty, 0.0f, 0.5f);
            if (type == "resource") obj.type = WorldObjectType::Resource;
            else if (type == "hazard") obj.type = WorldObjectType::Hazard;
            else if (type == "wall") obj.type = WorldObjectType::Wall;
            else if (type == "marker") obj.type = WorldObjectType::Marker;
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("type"), "Create and add default world object")
        .def("getObjects", [](SimpleWorld& self) {
            // Return empty vector for now - would need to modify SimpleWorld class
            return std::vector<WorldObject>();
        }, "Get all world objects (placeholder)")
        .def("getObjectAt", [](SimpleWorld& self, double x, double y) {
            if (!self.isValidPosition(x, y)) {
                throw py::value_error("Invalid position for getting object");
            }
            // Return empty object for now - would need to modify SimpleWorld class
            return WorldObject();
        }, py::arg("x"), py::arg("y"), "Get object at position (placeholder)")
        .def("getCollision", [](SimpleWorld& self, double x, double y, double radius) {
            if (radius <= 0.0) {
                throw py::value_error("Radius must be positive");
            }
            return false;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check collision (placeholder)")
        .def("getNearestResource", [](SimpleWorld& self, double x, double y) {
            if (!self.isValidPosition(x, y)) {
                throw py::value_error("Invalid position for finding nearest resource");
            }
            return WorldObject();
        }, py::arg("x"), py::arg("y"), "Find nearest resource (placeholder)")
        .def("getNearestHazard", [](SimpleWorld& self, double x, double y) {
            if (!self.isValidPosition(x, y)) {
                throw py::value_error("Invalid position for finding nearest hazard");
            }
            return WorldObject();
        }, py::arg("x"), py::arg("y"), "Find nearest hazard (placeholder)")
        .def("teleportAgent", [](SimpleWorld& self, double x, double y) {
            if (!self.isValidPosition(x, y)) {
                throw py::value_error("Cannot teleport agent to invalid position");
            }
            // Note: SimpleWorld doesn't have teleport method, would need modification
            return std::make_pair(x, y);
        }, py::arg("x"), py::arg("y"), "Teleport agent to position (placeholder)")
        .def("getDistance", [](const SimpleWorld& self, double x1, double y1, double x2, double y2) {
            double dx = x2 - x1;
            double dy = y2 - y1;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"), "Calculate Euclidean distance between positions");

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
             "Produce motor/action output based on neural activity")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // Python-specific methods
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             "Alias for getTotalNeuronCount")
        .def("getSynapseCount", &Brain::getTotalSynapseCount,
             "Alias for getTotalSynapseCount")
        .def("getActiveNeurons", &Brain::getActiveNeuronCount,
             "Alias for getActiveNeuronCount")
        .def("getFiringRate", &Brain::getAverageFiringRate,
             "Alias for getAverageFiringRate")
        .def("getStats", [](const Brain& brain) {
            py::dict stats;
            stats["neurons"] = brain.getTotalNeuronCount();
            stats["synapses"] = brain.getTotalSynapseCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["firing_rate"] = brain.getAverageFiringRate();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            stats["pending_spikes"] = brain.getPendingSpikeEventCount();
            stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            return stats;
        }, "Get brain statistics as dictionary")
        .def("getStatus", [](const Brain& brain) {
            std::stringstream ss;
            brain.logStatus();
            return "Brain status logged to output";
        }, "Get brain status and log to output")
        .def("getHealth", [](const Brain& brain) {
            float e_i_ratio = brain.getExcitationInhibitionRatio();
            size_t firing = brain.getFiringNeuronCount();
            size_t total = brain.getTotalNeuronCount();
            float firing_ratio = total > 0 ? static_cast<float>(firing) / total : 0.0f;
            float health_score = std::min(1.0f, (1.0f - std::abs(e_i_ratio - 1.0f)) * firing_ratio);
            return health_score;
        }, "Get brain health score (0.0 to 1.0)")
        .def("isHealthy", [](const Brain& brain) {
            return brain.getHealth() > 0.5f;
        }, "Check if brain is healthy")
        .def("getDevelopmentInfo", [](const Brain& brain) {
            py::dict dev_info;
            dev_info["stage"] = static_cast<int>(brain.getDevelopmentalStage());
            dev_info["stage_name"] = "unknown"; // Would need mapping
            dev_info["plastic"] = brain.getExcitationInhibitionRatio() > 0.5f;
            return dev_info;
        }, "Get developmental information")
        .def("simulate", [](Brain& brain, const py::array_t<float>& sensory_data, int steps) {
            // Simulate with given sensory input
            auto vision = Vision(sensory_data);
            brain.receiveSensoryInput(vision);
            for (int i = 0; i < steps; i++) {
                brain.step(i);
            }
            return brain.produceAction();
        }, py::arg("sensory_data"), py::arg("steps"), "Simulate brain with sensory input")
        .def("simulateEpisode", [](Brain& brain, int steps, double dt) {
            // Run a complete simulation episode
            for (int i = 0; i < steps; i++) {
                brain.step(i, i * dt);
            }
            return brain.produceAction();
        }, py::arg("steps"), py::arg("dt"), "Run simulation episode")
        .def("createTestData", [](Brain& brain) {
            py::dict test_data;
            auto vision = Vision(10, 10, 3);
            std::vector<float> test_image(300, 0.5f);
            vision.setData(test_image);
            test_data["vision"] = vision;
            test_data["vision_width"] = vision.getWidth();
            test_data["vision_height"] = vision.getHeight();
            test_data["vision_channels"] = vision.getChannels();
            return test_data;
        }, "Create test sensory data for simulation");

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
        // Python-specific methods
        .def("getSubsystems", [](const AgentBrain& self) {
            py::dict subsystems;
            subsystems["reward_modulation"] = self.isRewardModulationEnabled();
            subsystems["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            subsystems["development"] = self.isDevelopmentEnabled();
            subsystems["curiosity"] = self.isCuriosityEnabled();
            return subsystems;
        }, "Get current subsystem status")
        .def("enableAll", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all subsystems")
        .def("disableAll", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all subsystems")
        .def("getSystemStats", [](const AgentBrain& self) {
            py::dict stats;
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["has_reward_modulation"] = self.isRewardModulationEnabled();
            stats["has_structural_plasticity"] = self.isStructuralPlasticityEnabled();
            stats["has_development"] = self.isDevelopmentEnabled();
            stats["has_curiosity"] = self.isCuriosityEnabled();
            return stats;
        }, "Get agent brain system statistics")
        .def("getLearningState", [](const AgentBrain& self) {
            py::dict learning_state;
            learning_state["neuromodulation"] = self.getNeuromodulationLevel();
            learning_state["curiosity"] = self.getCuriosityLevel();
            learning_state["novelty"] = self.getNoveltyLevel();
            learning_state["prediction_error"] = self.getPredictionError();
            learning_state["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return learning_state;
        }, "Get current learning state")
        .def("updateAll", [](AgentBrain& self, double timestep) {
            self.updateDevelopment(timestep);
            // Note: Neuromodulation updates are called automatically during brain steps
            return self.getLearningState();
        }, py::arg("timestep"), "Update all agent systems")
        .def("processInputAndUpdate", [](AgentBrain& self, const SensoryPercept& percept, double timestep) {
            self.processSensoryInput(percept);
            return self.updateDevelopment(timestep);
        }, py::arg("percept"), py::arg("timestep"), "Process sensory input and update development")
        .def("getAction", [](AgentBrain& self, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
            return self.decodeMotorCommand();
        }, py::arg("percept"), "Get action from percept (full episode)")
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, int steps, double dt) {
            world.reset();
            for (int i = 0; i < steps; i++) {
                world.update(dt);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                self.updateDevelopment(dt);
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                // Apply reward if available
                if (percept.getInternal().getData().size() > 0) {
                    float reward = percept.getInternal().getData()[0];
                    self.applyRewardModulation(reward, 0.0f);
                }
            }
            return self.getLearningState();
        }, py::arg("world"), py::arg("steps"), py::arg("dt"), "Run complete episode simulation")
        .def("createEpisodeData", [](AgentBrain& self) {
            py::dict episode_data;
            episode_data["system_stats"] = self.getSystemStats();
            episode_data["learning_state"] = self.getLearningState();
            episode_data["subsystems"] = self.getSubsystems();
            return episode_data;
        }, "Create episode data summary");

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
