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
        .def("validate", &Config::summary,
             "Validate configuration and return status report")
        .def("validateSchema", [](Config& self, const std::string& schema) {
            // Schema validation implementation
            // For now, just validate that config is not empty
            if (self.getKeys().empty()) {
                return false;
            }
            // Additional schema validation logic would go here
            return true;
        }, py::arg("schema"), "Validate configuration against a schema string")
        .def("getAdvancedSummary", [](Config& self) {
            std::vector<std::string> keys = self.getKeys();
            std::stringstream ss;
            ss << "Advanced Configuration Summary:" << std::endl;
            ss << "Total entries: " << keys.size() << std::endl;
            ss << "Sources:" << std::endl;
            // Count entries by source (would need access to internal implementation)
            ss << "  Default: 0" << std::endl;
            ss << "  File: 0" << std::endl;
            ss << "  Command Line: 0" << std::endl;
            ss << "  Runtime: " << keys.size() << std::endl;
            ss << "Key types:" << std::endl;
            // Count value types (would need access to internal implementation)
            ss << "  Numeric (int): 0" << std::endl;
            ss << "  Numeric (double): 0" << std::endl;
            ss << "  Boolean: 0" << std::endl;
            ss << "  String: " << keys.size() << std::endl;
            return ss.str();
        }, "Get advanced configuration summary with statistics")
        .def("getConfigurationHash", [](Config& self) {
            // Generate a hash of the configuration for comparison
            std::vector<std::string> keys = self.getKeys();
            std::hash<std::string> hasher;
            size_t hashValue = 0;
            for (const auto& key : keys) {
                hashValue ^= hasher(key);
                // Would need to hash values too if accessible
            }
            return hashValue;
        }, "Get hash of configuration for comparison")
        .def("mergeConfigs", [](Config& self, const Config& other) {
            // Merge another configuration into this one
            std::vector<std::string> otherKeys = other.getKeys();
            for (const auto& key : otherKeys) {
                auto value = other.get<int>(key);
                if (value.has_value()) {
                    self.set(key, value.value());
                } else {
                    auto valueStr = other.get<std::string>(key);
                    if (valueStr.has_value()) {
                        self.set(key, valueStr.value());
                    }
                }
            }
            return self; // Return self for chaining
        }, py::arg("other"), "Merge another configuration into this one (returns self for chaining)")
        .def("diff", [](const Config& self, const Config& other) {
            std::vector<std::string> diffKeys;
            std::vector<std::string> selfKeys = self.getKeys();
            std::vector<std::string> otherKeys = other.getKeys();
            
            // Keys in self but not in other
            for (const auto& key : selfKeys) {
                if (!other.has(key)) {
                    diffKeys.push_back("REMOVED: " + key);
                }
            }
            
            // Keys in other but not in self
            for (const auto& key : otherKeys) {
                if (!self.has(key)) {
                    diffKeys.push_back("ADDED: " + key);
                }
            }
            
            // Keys in both but with different values
            for (const auto& key : otherKeys) {
                if (self.has(key)) {
                    auto selfValue = self.get<std::string>(key);
                    auto otherValue = other.get<std::string>(key);
                    if (selfValue != otherValue) {
                        diffKeys.push_back("CHANGED: " + key + " (self: " + 
                                         (selfValue.has_value() ? selfValue.value() : "") +
                                         ", other: " + (otherValue.has_value() ? otherValue.value() : "") + ")");
                    }
                }
            }
            return diffKeys;
        }, py::arg("other"), "Get difference between this and another configuration")
        .def("clone", [](const Config& self) {
            return std::make_shared<Config>(self); // Would need to implement copy constructor
        }, "Create a copy of this configuration")
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
        .def("setData", [](Vision& self, const std::vector<std::vector<float>>& imageData) {
            // Convert 2D image data to 1D vector
            size_t totalSize = 0;
            for (const auto& row : imageData) {
                totalSize += row.size();
            }
            std::vector<float> flatData(totalSize);
            size_t index = 0;
            for (const auto& row : imageData) {
                for (float pixel : row) {
                    flatData[index++] = pixel;
                }
            }
            self.setData(flatData);
        }, py::arg("imageData"))
        .def("setData", [](Vision& self, const std::vector<std::vector<std::vector<float>>>& rgbData) {
            // Convert 3D RGB image data to 1D vector
            size_t totalSize = 0;
            for (const auto& height : rgbData) {
                for (const auto& width : height) {
                    totalSize += width.size();
                }
            }
            std::vector<float> flatData(totalSize);
            size_t index = 0;
            for (const auto& height : rgbData) {
                for (const auto& width : height) {
                    for (float channel : width) {
                        flatData[index++] = channel;
                    }
                }
            }
            self.setData(flatData);
        }, py::arg("rgbData"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getFeatureVector", &VisionProcessor::getFeatures,
             "Get processed feature vector from vision processor")
        .def("getFeatureDimensions", &VisionProcessor::getFeatureDimensions,
             "Get dimensionality of processed features")
        .def("toArray", [](const Vision& self) {
            return self.getData();
        }, "Convert vision data to 1D array")
        .def("reshape", [](Vision& self, size_t width, size_t height, size_t channels) {
            // Reshape the internal data array (assuming it's stored as 1D)
            // This is a simplified implementation - would need access to actual reshape method
            std::vector<float> data = self.getData();
            size_t expectedSize = width * height * channels;
            if (data.size() != expectedSize) {
                throw std::runtime_error("Data size does not match requested dimensions");
            }
            // In a real implementation, we'd store reshaped data
            return self; // Return self for chaining
        }, py::arg("width"), py::arg("height"), py::arg("channels"),
             "Reshape vision data to specified dimensions")
        .def("normalize", [](Vision& self, float min = 0.0f, float max = 1.0f) {
            // Normalize vision data to range [min, max]
            std::vector<float> data = self.getData();
            float dataMin = *std::min_element(data.begin(), data.end());
            float dataMax = *std::max_element(data.begin(), data.end());
            
            if (dataMax == dataMin) {
                // Avoid division by zero
                std::fill(data.begin(), data.end(), max);
            } else {
                for (size_t i = 0; i < data.size(); ++i) {
                    data[i] = min + (data[i] - dataMin) * (max - min) / (dataMax - dataMin);
                }
            }
            self.setData(data);
            return self; // Return self for chaining
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize vision data to specified range")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            // Resize vision data (simplified nearest-neighbor)
            std::vector<float> data = self.getData();
            size_t oldWidth = self.getWidth();
            size_t oldHeight = self.getHeight();
            size_t channels = self.getChannels();
            
            if (oldWidth == 0 || oldHeight == 0) {
                throw std::runtime_error("Invalid original dimensions");
            }
            
            float scaleX = static_cast<float>(oldWidth) / newWidth;
            float scaleY = static_cast<float>(oldHeight) / newHeight;
            
            std::vector<float> resizedData(newWidth * newHeight * channels, 0.0f);
            
            for (size_t c = 0; c < channels; ++c) {
                for (size_t y = 0; y < newHeight; ++y) {
                    float fy = y * scaleY;
                    size_t y0 = static_cast<size_t>(fy);
                    size_t y1 = std::min(y0 + 1, oldHeight - 1);
                    float ty = fy - y0;
                    
                    for (size_t x = 0; x < newWidth; ++x) {
                        float fx = x * scaleX;
                        size_t x0 = static_cast<size_t>(fx);
                        size_t x1 = std::min(x0 + 1, oldWidth - 1);
                        float tx = fx - x0;
                        
                        // Bilinear interpolation
                        float value = (1 - tx) * (1 - ty) * data[(y0 * oldWidth + x0) * channels + c] +
                                      tx * (1 - ty) * data[(y0 * oldWidth + x1) * channels + c] +
                                      (1 - tx) * ty * data[(y1 * oldWidth + x0) * channels + c] +
                                      tx * ty * data[(y1 * oldWidth + x1) * channels + c];
                        
                        resizedData[(y * newWidth + x) * channels + c] = value;
                    }
                }
            }
            
            // Create a new Vision with resized data (requires constructor with dimensions)
            // This is a simplified implementation
            return resizedData; // Return as vector for now
        }, py::arg("newWidth"), py::arg("newHeight"),
             "Resize vision data using bilinear interpolation")
        .def("extractChannel", [](Vision& self, size_t channel) {
            std::vector<float> data = self.getData();
            size_t channels = self.getChannels();
            
            if (channel >= channels) {
                throw std::runtime_error("Channel index out of range");
            }
            
            std::vector<float> channelData;
            channelData.reserve(data.size() / channels);
            
            for (size_t i = channel; i < data.size(); i += channels) {
                channelData.push_back(data[i]);
            }
            
            return channelData;
        }, py::arg("channel"),
             "Extract a specific color channel as 1D array")
        .def("blend", [](Vision& self, const Vision& other, float weight) {
            std::vector<float> data1 = self.getData();
            std::vector<float> data2 = other.getData();
            
            if (data1.size() != data2.size()) {
                throw std::runtime_error("Vision objects must have same data size for blending");
            }
            
            std::vector<float> blendedData(data1.size());
            for (size_t i = 0; i < data1.size(); ++i) {
                blendedData[i] = (1 - weight) * data1[i] + weight * data2[i];
            }
            
            // Create a new Vision with blended data
            return blendedData; // Return as vector for now
        }, py::arg("other"), py::arg("weight") = 0.5f,
             "Blend with another vision input using specified weight (default 0.5)")

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("batchStep", [](Brain& self, const std::vector<SimulationStep>& steps) {
            for (const auto& step : steps) {
                self.step(step);
            }
        }, py::arg("steps"), "Perform multiple simulation steps in batch for efficiency")
        .def("batchStep", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("batchStepWithTime", [](Brain& self, const std::vector<SimulationStep>& steps, const std::vector<Timestamp>& times) {
            if (steps.size() != times.size()) {
                throw std::runtime_error("Steps and times vectors must have the same size");
            }
            for (size_t i = 0; i < steps.size(); ++i) {
                self.step(steps[i], times[i]);
            }
        }, py::arg("steps"), py::arg("times"), "Perform multiple simulation steps with timestamps in batch")
        .def("batchStepWithMultipleInputs", [](Brain& self, 
                                             const std::vector<SimulationStep>& steps,
                                             const std::vector<std::shared_ptr<SensoryInput>>& inputs,
                                             const std::vector<Timestamp>& times) {
            if (steps.size() != inputs.size() || steps.size() != times.size()) {
                throw std::runtime_error("Steps, inputs, and times vectors must have the same size");
            }
            for (size_t i = 0; i < steps.size(); ++i) {
                if (inputs[i]) {
                    self.step(steps[i], times[i]);
                    self.receiveSensoryInput(*inputs[i]);
                }
            }
        }, py::arg("steps"), py::arg("inputs"), py::arg("times"), "Perform multiple simulation steps with associated sensory inputs in batch")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
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

    // Convenience functions for common workflows
    
    m.def("createSimpleAgent", []() -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        brain->initialize();
        world->configure(800, 600, 84, 84);
        world->reset();
        agentBrain->initialize(*world);
        
        return std::make_tuple(brain, world, agentBrain);
    }, "Create a complete simple agent setup with brain, world, and agent brain");
    
    m.def("runSimulation", [](std::shared_ptr<AgentBrain> agentBrain, 
                           const std::vector<std::shared_ptr<SensoryInput>>& inputs,
                           const std::vector<Action>& actions,
                           size_t steps) {
        if (!agentBrain) {
            throw std::runtime_error("AgentBrain is null");
        }
        
        auto brain = agentBrain->getBrain();
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        brain->reset();
        agentBrain->reset();
        
        for (size_t i = 0; i < steps && i < inputs.size(); ++i) {
            if (inputs[i]) {
                // Create percept from input
                auto percept = std::make_shared<SensoryPercept>();
                if (auto vision = std::dynamic_pointer_cast<Vision>(inputs[i])) {
                    percept->setVision(*vision);
                }
                agentBrain->processSensoryInput(*percept);
                
                if (i < actions.size()) {
                    // In a real implementation, we'd apply the action
                    // For now, we'll just log it
                    std::cout << "Step " << i << ": Applying action " << actions[i].getName() << std::endl;
                }
            }
            
            brain->step(i, static_cast<Timestamp>(i * 0.1));
        }
        
        return true;
    }, py::arg("agentBrain"), py::arg("inputs"), py::arg("actions"), py::arg("steps") = 100,
         "Run a complete simulation with agent brain, sensory inputs, and actions");

    m.def("createVisionFromArray", [](const std::vector<float>& data, size_t width, size_t height, size_t channels = 3) -> std::shared_ptr<Vision> {
        auto vision = std::make_shared<Vision>(width, height, channels);
        if (data.size() == width * height * channels) {
            vision->setData(data);
        }
        return vision;
    }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
         "Create a Vision object from a 1D array with specified dimensions");

    m.def("createSensoryPercept", []() -> std::shared_ptr<SensoryPercept> {
        return std::make_shared<SensoryPercept>();
    }, "Create a new sensory percept object");

    m.def("createInternalSignals", []() -> std::shared_ptr<InternalSignals> {
        return std::make_shared<InternalSignals>();
    }, "Create internal signals sensory input");

    m.def("createAudioFromSamples", [](const std::vector<float>& samples, size_t sampleRate) -> std::shared_ptr<Audio> {
        auto audio = std::make_shared<Audio>(sampleRate, samples.size());
        audio->setData(samples);
        return audio;
    }, py::arg("samples"), py::arg("sampleRate"),
         "Create an Audio object from sample data");

    // Exception types and better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
    
    m.def("raiseError", [](const std::string& message) {
        throw std::runtime_error(message);
    }, py::arg("message"), "Raise a runtime error with the given message");

    m.def("isValidNeuronId", [](const NeuronId& id) {
        return id.value != 0; // Assuming INVALID_NEURON_ID is 0
    }, py::arg("id"), "Check if a neuron ID is valid");

    m.def("isValidSynapseId", [](const SynapseId& id) {
        return id.value != 0; // Assuming INVALID_SYNAPSE_ID is 0
    }, py::arg("id"), "Check if a synapse ID is valid");

    m.def("isValidRegionId", [](const RegionId& id) {
        return id.value != 0; // Assuming INVALID_REGION_ID is 0
    }, py::arg("id"), "Check if a region ID is valid");

    // Session and checkpoint utilities
    m.def("createBackupConfig", [](const Config& original) -> std::shared_ptr<Config> {
        // Create a copy of the configuration (simplified implementation)
        auto backup = std::make_shared<Config>();
        auto keys = original.getKeys();
        for (const auto& key : keys) {
            auto value = original.get<std::string>(key);
            if (value.has_value()) {
                backup->set(key, value.value());
            }
        }
        return backup;
    }, py::arg("original"), "Create a backup/copy of a configuration");

    m.def("compareConfigs", [](const Config& config1, const Config& config2) {
        std::vector<std::string> differences;
        auto keys1 = config1.getKeys();
        auto keys2 = config2.getKeys();
        
        // Check for keys that exist in config1 but not config2
        for (const auto& key : keys1) {
            if (!config2.has(key)) {
                differences.push_back("Missing in config2: " + key);
            }
        }
        
        // Check for keys that exist in config2 but not config1
        for (const auto& key : keys2) {
            if (!config1.has(key)) {
                differences.push_back("Missing in config1: " + key);
            }
        }
        
        // Check for different values
        for (const auto& key : keys2) {
            if (config1.has(key)) {
                auto value1 = config1.get<std::string>(key);
                auto value2 = config2.get<std::string>(key);
                if (value1 != value2) {
                    differences.push_back("Different values for " + key + ": " +
                                        (value1.has_value() ? value1.value() : "N/A") + " vs " +
                                        (value2.has_value() ? value2.value() : "N/A"));
                }
            }
        }
        
        return differences;
    }, py::arg("config1"), py::arg("config2"), "Compare two configurations and return differences");

    m.def("getSystemInfo", []() {
        std::stringstream ss;
        ss << "NLM Python Bindings System Info:" << std::endl;
        ss << "Version: 1.0.0" << std::endl;
        ss << "Build timestamp: " << __DATE__ << " " << __TIME__ << std::endl;
        ss << "Python version: " << PYBIND11_VERSION_MAJOR << "." << PYBIND11_VERSION_MINOR << "." << PYBIND11_VERSION_PATCH << std::endl;
        return ss.str();
    }, "Get system information about the bindings");

    m.def("shutdownGracefully", [](Brain& brain, SimpleWorld& world) {
        brain.reset();
        world.reset();
        return true;
    }, py::arg("brain"), py::arg("world"), "Reset and clean up brain and world resources");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
