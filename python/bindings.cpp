#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/performance/OptimizedBrain.hpp"

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
        .def("loadFromJsonString", &Config::loadFromJsonString, py::arg("jsonString"),
             "Load configuration from JSON string")
        .def("loadFromYamlString", &Config::loadFromYamlString, py::arg("yamlString"),
             "Load configuration from YAML string")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), py::arg("prettyPrint") = true,
             "Save configuration to a JSON file")
        .def("saveToLegacyFile", &Config::saveToLegacyFile, py::arg("filepath"),
             "Save configuration to key=value format (backward compatibility)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("hasWithSection", &Config::has, py::arg("section"), py::arg("key"),
             "Check if a configuration key exists with section")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::get),
             py::arg("section"), py::arg("key"),
             "Get value from configuration with section")
        .def("get", py::overload_cast<const std::string&>(&Config::get),
             py::arg("key"),
             "Get value from configuration")
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr),
             py::arg("section"), py::arg("key"), py::arg("defaultValue"),
             "Get value from configuration with section, providing default")
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get value from configuration, providing default")
        .def("set", py::overload_cast<const std::string&, const ConfigValue&, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value with source")
        .def("setWithSection", py::overload_cast<const std::string&, const std::string&, const ConfigValue&, ConfigSource>(&Config::set),
             py::arg("section"), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value with section and source")
        .def("setString", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration string value with source")
        .def("setInt", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration integer value with source")
        .def("setDouble", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration double value with source")
        .def("setBool", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration boolean value with source")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("removeWithSection", &Config::remove, py::arg("section"), py::arg("key"),
             "Remove configuration key with section")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getKeySectionPairs", &Config::getKeySectionPairs,
             "Get all configuration keys with section pairs")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("getStats", &Config::getStats,
             "Get configuration statistics")
        .def("validate", &Config::validate,
             "Validate configuration")
        .def("getValidationErrors", &Config::getValidationErrors,
             "Get validation errors")
        .def("setValidationEnabled", &Config::setValidationEnabled, py::arg("enabled"),
             "Enable or disable validation")
        .def("loadInheritance", &Config::loadInheritance, py::arg("baseConfigFile"),
             "Load configuration inheritance from base file")
        .def("mergeConfig", &Config::mergeConfig, py::arg("otherConfig"),
             "Merge another configuration into this one")
        .def("addSchemaEntry", &Config::addSchemaEntry, py::arg("schema"),
             "Add configuration schema entry")
        .def("clearSchemas", &Config::clearSchemas,
             "Clear all configuration schemas")
        .def("toJson", &Config::toJson,
             "Convert configuration to JSON")
        .def("fromJson", &Config::fromJson, py::arg("jsonData"),
             "Load configuration from JSON object")
        .def("isJsonFile", &Config::isJsonFile, py::arg("filepath"),
             "Check if file is JSON format")
        .def("isYamlFile", &Config::isYamlFile, py::arg("filepath"),
             "Check if file is YAML format")
        .def("isLegacyFile", &Config::isLegacyFile, py::arg("filepath"),
             "Check if file is legacy key=value format")
        .def("parseSectionKey", &Config::parseSectionKey, py::arg("fullKey"),
             "Parse section:key from full key string")
        .def("createSectionKey", &Config::createSectionKey, py::arg("section"), py::arg("key"),
             "Create full key string from section and key")
        .def("trim", &Config::trim, py::arg("str"),
             "Trim whitespace from string")
        .def("toLower", &Config::toLower, py::arg("str"),
             "Convert string to lowercase")
        .def("toUpper", &Config::toUpper, py::arg("str"),
             "Convert string to uppercase")
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
        .def("getChannelData", [](const SensoryInput& self, int channel) {
            return self.getData(); // Simplified - would need channel access
        }, py::arg("channel"), "Get data from specific channel")
        .def("getNormalizedData", [](const SensoryInput& self) {
            auto data = self.getData();
            float maxVal = *std::max_element(data.begin(), data.end());
            float minVal = *std::min_element(data.begin(), data.end());
            float range = maxVal - minVal;
            if (range > 0) {
                for (auto& val : data) val = (val - minVal) / range;
            }
            return data;
        }, "Get normalized data (0-1 range)")
        .def("getStandardizedData", [](const SensoryInput& self) {
            auto data = self.getData();
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            double mean = sum / data.size();
            double sqSum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
            double stddev = std::sqrt(sqSum / data.size() - mean * mean);
            if (stddev > 0) {
                for (auto& val : data) val = (val - mean) / stddev;
            }
            return data;
        }, "Get standardized data (mean 0, stddev 1)")
        .def("getDataStats", [](const SensoryInput& self) {
            auto data = self.getData();
            if (data.empty()) return py::make_tuple(0.0f, 0.0f, 0.0f, 0.0f);
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            float sqSum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0f);
            float variance = sqSum / data.size() - mean * mean;
            float stddev = std::sqrt(std::max(0.0f, variance));
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            return py::make_tuple(min, max, mean, stddev);
        }, "Get data statistics (min, max, mean, stddev)")
        .def("addNoise", [](SensoryInput& self, float noiseLevel) {
            auto data = self.getData();
            for (auto& val : data) {
                val += (rand() % 2000 - 1000) / 1000.0f * noiseLevel;
            }
            return data;
        }, py::arg("noiseLevel"), "Add Gaussian noise to sensory data")
        .def("applyFilter", [](SensoryInput& self, float lowPass = 0.1f, float highPass = 0.9f) {
            auto data = self.getData();
            if (data.size() < 2) return data;
            
            // Simple filtering
            std::vector<float> filtered;
            filtered.reserve(data.size());
            filtered.push_back(data[0]);
            for (size_t i = 1; i < data.size(); ++i) {
                filtered.push_back(lowPass * filtered.back() + (1 - lowPass) * data[i]);
            }
            
            // High pass
            float alpha = highPass;
            for (size_t i = 1; i < filtered.size(); ++i) {
                filtered[i] = alpha * (filtered[i] - filtered[i-1]) + filtered[i-1];
            }
            
            return filtered;
        }, py::arg("lowPass") = 0.1f, py::arg("highPass") = 0.9f,
           "Apply simple low-pass and high-pass filtering to sensory data")
        .def("compressData", [](SensoryInput& self, float threshold) {
            auto data = self.getData();
            float maxVal = *std::max_element(data.begin(), data.end());
            float minVal = *std::min_element(data.begin(), data.end());
            float range = maxVal - minVal;
            
            std::vector<float> compressed;
            for (float val : data) {
                if (range > 0) {
                    float norm = (val - minVal) / range;
                    if (norm < threshold) norm = 0.0f;
                    else if (norm > (1.0f - threshold)) norm = 1.0f;
                    else norm = (norm - threshold) / (1.0f - 2.0f * threshold);
                }
                compressed.push_back(norm);
            }
            return compressed;
        }, py::arg("threshold") = 0.1f,
           "Apply data compression/thresholding to sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", [](const Vision& self) {
            return self.getData();
        })
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getPixel", [](const Vision& self, size_t x, size_t y, size_t c = 0) {
            size_t index = ((y * self.getWidth() + x) * self.getChannels()) + c;
            if (index < self.getData().size()) {
                return self.getData()[index];
            }
            return 0.0f;
        }, py::arg("x"), py::arg("y"), py::arg("c") = 0,
           "Get pixel value at coordinates (x, y) in channel c")
        .def("setPixel", [](Vision& self, size_t x, size_t y, size_t c, float value) {
            size_t index = ((y * self.getWidth() + x) * self.getChannels()) + c;
            if (index < self.getData().size()) {
                auto data = self.getData();
                data[index] = value;
                self.setData(data);
            }
        }, py::arg("x"), py::arg("y"), py::arg("c") = 0, py::arg("value"),
           "Set pixel value at coordinates (x, y) in channel c")
        .def("getGrayscale", [](const Vision& self) {
            auto data = self.getData();
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            
            if (channels >= 3) {
                std::vector<float> grayscale;
                grayscale.reserve(width * height);
                for (size_t y = 0; y < height; ++y) {
                    for (size_t x = 0; x < width; ++x) {
                        size_t idx = ((y * width + x) * channels);
                        float gray = 0.299f * data[idx] + 0.587f * data[idx + 1] + 0.114f * data[idx + 2];
                        grayscale.push_back(gray);
                    }
                }
                return grayscale;
            }
            return data;
        }, "Convert vision data to grayscale")
        .def("getEdgeFeatures", [](const Vision& self, float threshold = 0.5f) {
            auto data = self.getData();
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            
            if (data.size() < width * height) return std::vector<float>();
            
            std::vector<float> edges;
            edges.reserve(width * height);
            
            for (size_t y = 1; y < height - 1; ++y) {
                for (size_t x = 1; x < width - 1; ++x) {
                    size_t idx = y * width + x;
                    float center = data[idx];
                    float left = data[idx - 1];
                    float right = data[idx + 1];
                    float up = data[idx - width];
                    float down = data[idx + width];
                    
                    float gradient = std::abs(left - right) + std::abs(up - down);
                    edges.push_back(gradient > threshold ? 1.0f : 0.0f);
                }
            }
            return edges;
        }, py::arg("threshold") = 0.5f,
           "Detect edge features in vision data")
        .def("getColorHistogram", [](const Vision& self) {
            auto data = self.getData();
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            
            if (channels == 0) return std::vector<size_t>();
            
            const size_t bins = 8;
            std::vector<size_t> histogram(bins, 0);
            
            for (size_t c = 0; c < channels; ++c) {
                float minVal = *std::min_element(data.begin(), data.end());
                float maxVal = *std::max_element(data.begin(), data.end());
                float range = maxVal - minVal;
                if (range > 0) {
                    for (size_t i = 0; i < data.size(); i += channels) {
                        if (i + c < data.size()) {
                            size_t bin = static_cast<size_t>((data[i + c] - minVal) / range * bins);
                            if (bin >= bins) bin = bins - 1;
                            histogram[bin]++;
                        }
                    }
                }
            }
            return histogram;
        }, "Get color histogram features from vision data")
        .def("normalizePixels", [](Vision& self) {
            auto data = self.getData();
            float minVal = *std::min_element(data.begin(), data.end());
            float maxVal = *std::max_element(data.begin(), data.end());
            float range = maxVal - minVal;
            
            if (range > 0) {
                for (auto& val : data) val = (val - minVal) / range;
                self.setData(data);
            }
            return data;
        }, "Normalize pixel values to [0, 1] range")
        .def("applyColorEnhance", [](Vision& self, float contrast = 1.0f, float brightness = 0.0f) {
            auto data = self.getData();
            size_t channels = self.getChannels();
            
            for (size_t i = 0; i < data.size(); i += channels) {
                for (size_t c = 0; c < channels && i + c < data.size(); ++c) {
                    float val = data[i + c];
                    val = contrast * (val - 0.5f) + 0.5f + brightness;
                    data[i + c] = std::max(0.0f, std::min(1.0f, val));
                }
            }
            self.setData(data);
            return data;
        }, py::arg("contrast") = 1.0f, py::arg("brightness") = 0.0f,
           "Apply contrast and brightness enhancement to vision data");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", [](const Audio& self) {
            return self.getData();
        })
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getFrequencySpectrum", [](const Audio& self) {
            auto data = self.getData();
            size_t n = data.size();
            
            if (n < 2) return std::vector<float>();
            
            // Simple FFT approximation using DFT
            std::vector<float> spectrum;
            size_t spectrumSize = std::min(n / 2, static_cast<size_t>(1024));
            spectrum.reserve(spectrumSize);
            
            float sampleRate = self.getSampleRate();
            float binWidth = sampleRate / n;
            
            for (size_t k = 0; k < spectrumSize; ++k) {
                float real = 0.0f, imag = 0.0f;
                for (size_t i = 0; i < n; ++i) {
                    float angle = -2.0f * M_PI * k * i / n;
                    real += data[i] * std::cos(angle);
                    imag -= data[i] * std::sin(angle);
                }
                float magnitude = std::sqrt(real * real + imag * imag) / n;
                spectrum.push_back(magnitude);
            }
            
            return spectrum;
        }, "Get approximate frequency spectrum of audio data")
        .def("getZeroCrossingRate", [](const Audio& self) {
            auto data = self.getData();
            size_t n = data.size();
            if (n < 2) return 0.0f;
            
            int zeroCrossings = 0;
            for (size_t i = 1; i < n; ++i) {
                if ((data[i] >= 0.0f && data[i-1] < 0.0f) ||
                    (data[i] < 0.0f && data[i-1] >= 0.0f)) {
                    zeroCrossings++;
                }
            }
            
            return static_cast<float>(zeroCrossings) / (n - 1);
        }, "Calculate zero-crossing rate (measure of voice activity)")
        .def("getEnvelope", [](Audio& self) {
            auto data = self.getData();
            size_t n = data.size();
            if (n == 0) return std::vector<float>();
            
            std::vector<float> envelope;
            envelope.reserve(n);
            envelope.push_back(std::abs(data[0]));
            
            for (size_t i = 1; i < n; ++i) {
                float current = std::abs(data[i]);
                float previous = envelope.back();
                envelope.push_back(0.95f * previous + 0.05f * current);
            }
            
            return envelope;
        }, "Get amplitude envelope of audio signal")
        .def("normalizeAudio", [](Audio& self) {
            auto data = self.getData();
            float maxVal = *std::max_element(data.begin(), data.end());
            float minVal = *std::min_element(data.begin(), data.end());
            float range = maxVal - minVal;
            
            if (range > 0) {
                for (auto& val : data) val = (val - minVal) / range;
                self.setData(data);
            }
            return data;
        }, "Normalize audio signal to [-1, 1] range")
        .def("applyGain", [](Audio& self, float gain) {
            auto data = self.getData();
            for (auto& val : data) val *= gain;
            self.setData(data);
            return data;
        }, py::arg("gain"), "Apply gain (amplification) to audio signal")
        .def("applyFilter", [](Audio& self, float cutoffFreq, float sampleRate) {
            // Simple first-order low-pass filter
            auto data = self.getData();
            std::vector<float> filtered;
            filtered.reserve(data.size());
            
            if (data.empty()) return filtered;
            filtered.push_back(data[0]);
            
            float alpha = 2.0f * M_PI * cutoffFreq / sampleRate;
            for (size_t i = 1; i < data.size(); ++i) {
                filtered.push_back(alpha * data[i] + (1.0f - alpha) * filtered.back());
            }
            
            return filtered;
        }, py::arg("cutoffFreq"), py::arg("sampleRate"),
           "Apply simple low-pass filter to audio signal")
        .def("getRMS", [](const Audio& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            
            float sqSum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0f);
            float variance = sqSum / data.size() - mean * mean;
            
            return std::sqrt(std::max(0.0f, variance));
        }, "Calculate root mean square (RMS) amplitude of audio signal")
        .def("getStereoInfo", [](const Audio& self) {
            auto data = self.getData();
            size_t n = data.size();
            
            if (n < 2) return py::make_tuple(0.0f, 0.0f, 0.0f);
            
            float left = 0.0f, right = 0.0f, duration = 0.0f;
            size_t leftCount = 0, rightCount = 0;
            
            for (size_t i = 0; i < n; i += 2) {
                if (i < n) { left += std::abs(data[i]); leftCount++; }
                if (i + 1 < n) { right += std::abs(data[i + 1]); rightCount++; }
            }
            
            if (leftCount > 0) left /= leftCount;
            if (rightCount > 0) right /= rightCount;
            
            float sampleRate = self.getSampleRate();
            duration = n / (sampleRate * 2.0f);
            
            return py::make_tuple(left, right, duration);
        }, "Get stereo audio information (left, right levels, duration)");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", [](const InternalSignals& self) {
            return self.getData();
        }, "Get all internal signals")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            auto data = self.getData();
            if (index < data.size()) return data[index];
            return 0.0f;
        }, py::arg("index"), "Get specific internal signal value")
        .def("setSignal", [](InternalSignals& self, size_t index, float value) {
            auto data = self.getData();
            if (index < data.size()) {
                data[index] = value;
                self.setData(data);
            }
        }, py::arg("index"), py::arg("value"), "Set specific internal signal value")
        .def("getSignalStats", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return py::make_tuple(0.0f, 0.0f, 0.0f, 0.0f);
            
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            
            float sqSum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0f);
            float variance = sqSum / data.size() - mean * mean;
            float stddev = std::sqrt(std::max(0.0f, variance));
            
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            
            return py::make_tuple(min, max, mean, stddev);
        }, "Get statistics for internal signals (min, max, mean, stddev)")
        .def("normalizeSignals", [](InternalSignals& self) {
            auto data = self.getData();
            float maxVal = *std::max_element(data.begin(), data.end());
            float minVal = *std::min_element(data.begin(), data.end());
            float range = maxVal - minVal;
            
            if (range > 0) {
                for (auto& val : data) val = (val - minVal) / range;
                self.setData(data);
            }
            return data;
        }, "Normalize internal signals to [0, 1] range")
        .def("addGaussianNoise", [](InternalSignals& self, float noiseLevel) {
            auto data = self.getData();
            std::mt19937 rng(std::random_device{}());
            std::normal_distribution<float> dist(0.0f, noiseLevel);
            
            for (auto& val : data) {
                val += dist(rng);
            }
            self.setData(data);
            return data;
        }, py::arg("noiseLevel"), "Add Gaussian noise to internal signals")
        .def("smoothSignals", [](InternalSignals& self, float smoothingFactor) {
            auto data = self.getData();
            std::vector<float> smoothed;
            smoothed.reserve(data.size());
            
            if (!data.empty()) {
                smoothed.push_back(data[0]);
                for (size_t i = 1; i < data.size(); ++i) {
                    smoothed.push_back(smoothingFactor * data[i] + (1.0f - smoothingFactor) * smoothed.back());
                }
                self.setData(smoothed);
            }
            
            return smoothed;
        }, py::arg("smoothingFactor") = 0.5f,
           "Apply exponential smoothing to internal signals");

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
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) return params[index];
            return 0.0f;
        }, py::arg("index"), "Get specific parameter value")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            }
        }, py::arg("index"), py::arg("value"), "Set specific parameter value")
        .def("getActionDuration", [](const Action& self) {
            return self.getParameters().size() * 0.1f; // Default 0.1s per parameter
        }, "Get estimated action duration")
        .def("isComplex", [](const Action& self) {
            return self.getParameters().size() > 3;
        }, "Check if action is complex (many parameters)")
        .def("getActionComplexity", [](const Action& self) {
            return static_cast<float>(self.getParameters().size()) / 10.0f; // Normalize to [0, 1]
        }, "Get action complexity score (0-1)")
        .def("toJson", [](const Action& self) {
            auto params = self.getParameters();
            auto type = self.getType();
            py::dict json;
            json["type"] = static_cast<int>(type);
            json["name"] = self.getName();
            py::list paramList;
            for (float p : params) paramList.append(p);
            json["parameters"] = paramList;
            return json;
        }, "Convert action to JSON representation")
        .def("fromJson", [](Action& self, py::dict json) {
            if (json.contains("type")) {
                ActionType type = static_cast<ActionType>(json["type"].cast<int>());
                self.setType(type);
            }
            if (json.contains("parameters")) {
                py::list paramList = json["parameters"].cast<py::list>();
                std::vector<float> params;
                for (auto item : paramList) {
                    params.push_back(item.cast<float>());
                }
                self.setParameters(params);
            }
        }, py::arg("json"), "Load action from JSON representation")
        .def("applyToAgentBody", [](Action& self, AgentBody& body) {
            ActionType type = self.getType();
            switch (type) {
                case ActionType::MoveForward:
                    body.x += 1.0f;
                    break;
                case ActionType::MoveBackward:
                    body.x -= 1.0f;
                    break;
                case ActionType::TurnLeft:
                    body.orientation += M_PI / 4.0f;
                    break;
                case ActionType::TurnRight:
                    body.orientation -= M_PI / 4.0f;
                    break;
                case ActionType::Look:
                    body.orientation += M_PI / 8.0f;
                    break;
                default:
                    // Other actions don't affect position directly
                    break;
            }
        }, py::arg("body"), "Apply action to agent body state");

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
        .def("getPosition", [](const WorldObject& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get object position (x, y)")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another world object")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float distance = self.distanceTo(other);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), "Check if object is colliding with another")
        .def("isPointInside", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return dx * dx + dy * dy <= self.radius * self.radius;
        }, py::arg("x"), py::arg("y"), "Check if point is inside object")
        .def("getArea", [](const WorldObject& self) {
            return M_PI * self.radius * self.radius;
        }, "Get object area")
        .def("toString", [](const WorldObject& self) {
            std::string typeStr;
            switch (self.type) {
                case WorldObjectType::Empty: typeStr = "Empty"; break;
                case WorldObjectType::Resource: typeStr = "Resource"; break;
                case WorldObjectType::Hazard: typeStr = "Hazard"; break;
                case WorldObjectType::Wall: typeStr = "Wall"; break;
                case WorldObjectType::Marker: typeStr = "Marker"; break;
                default: typeStr = "Unknown";
            }
            return "WorldObject(x=" + std::to_string(self.x) + ", y=" + std::to_string(self.y) +
                   ", radius=" + std::to_string(self.radius) + ", type=" + typeStr +
                   ", value=" + std::to_string(self.value) + ", active=" +
                   std::string(self.active ? "true" : "false") + ")";
        }, "Get string representation of world object");

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
            return py::make_tuple(self.x, self.y);
        }, "Get agent position (x, y)")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get agent velocity (vx, vy)")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"), "Set agent velocity")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get agent speed")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get agent direction (radians)")
        .def("moveTo", [](AgentBody& self, float targetX, float targetY, float speed, float dt) {
            float dx = targetX - self.x;
            float dy = targetY - self.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance > 0) {
                float moveX = (dx / distance) * speed * dt;
                float moveY = (dy / distance) * speed * dt;
                self.x += moveX;
                self.y += moveY;
                self.velocityX = moveX / dt;
                self.velocityY = moveY / dt;
                self.isMoving = true;
            }
        }, py::arg("targetX"), py::arg("targetY"), py::arg("speed"), py::arg("dt"),
           "Move agent to target position")
        .def("rotate", [](AgentBody& self, float angle, float angularSpeed, float dt) {
            float rotation = angle;
            if (angularSpeed > 0) {
                rotation = std::min(angle, angularSpeed * dt);
            } else if (angularSpeed < 0) {
                rotation = std::max(angle, angularSpeed * dt);
            }
            self.orientation += rotation;
            self.angularVelocity = rotation / dt;
            self.isTurning = (rotation != 0.0f);
        }, py::arg("angle"), py::arg("angularSpeed"), py::arg("dt"),
           "Rotate agent by angle")
        .def("updatePhysics", [](AgentBody& self, float dt, float friction = 0.95f) {
            self.velocityX *= friction;
            self.velocityY *= friction;
            self.angularVelocity *= friction;
            
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            self.orientation += self.angularVelocity * dt;
            
            self.isMoving = (self.velocityX != 0.0f || self.velocityY != 0.0f);
            self.isTurning = (self.angularVelocity != 0.0f);
        }, py::arg("dt"), py::arg("friction") = 0.95f,
           "Update agent physics with friction")
        .def("getStats", [](const AgentBody& self) {
            return py::make_tuple(
                self.x, self.y,
                std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY),
                self.orientation,
                self.energy,
                self.health,
                self.age,
                self.isMoving,
                self.isTurning,
                self.lastActionTime
            );
        }, "Get comprehensive agent body statistics")
        .def("toJson", [](const AgentBody& self) {
            py::dict json;
            json["x"] = self.x;
            json["y"] = self.y;
            json["orientation"] = self.orientation;
            json["velocityX"] = self.velocityX;
            json["velocityY"] = self.velocityY;
            json["angularVelocity"] = self.angularVelocity;
            json["energy"] = self.energy;
            json["health"] = self.health;
            json["age"] = self.age;
            json["isMoving"] = self.isMoving;
            json["isTurning"] = self.isTurning;
            json["lastActionTime"] = self.lastActionTime;
            return json;
        }, "Convert agent body to JSON representation")
        .def("fromJson", [](AgentBody& self, py::dict json) {
            if (json.contains("x")) self.x = json["x"].cast<float>();
            if (json.contains("y")) self.y = json["y"].cast<float>();
            if (json.contains("orientation")) self.orientation = json["orientation"].cast<float>();
            if (json.contains("velocityX")) self.velocityX = json["velocityX"].cast<float>();
            if (json.contains("velocityY")) self.velocityY = json["velocityY"].cast<float>();
            if (json.contains("angularVelocity")) self.angularVelocity = json["angularVelocity"].cast<float>();
            if (json.contains("energy")) self.energy = json["energy"].cast<float>();
            if (json.contains("health")) self.health = json["health"].cast<float>();
            if (json.contains("age")) self.age = json["age"].cast<float>();
            if (json.contains("isMoving")) self.isMoving = json["isMoving"].cast<bool>();
            if (json.contains("isTurning")) self.isTurning = json["isTurning"].cast<bool>();
            if (json.contains("lastActionTime")) self.lastActionTime = json["lastActionTime"].cast<float>();
        }, py::arg("json"), "Load agent body from JSON representation");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("getFormattedMessage", [](const ActionResult& self) {
            std::string successStr = self.success ? "SUCCESS" : "FAILED";
            return successStr + ": " + self.message + " (reward: " + std::to_string(self.reward) + ")";
        }, "Get formatted action result message")
        .def("isPositiveOutcome", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if action had positive outcome")
        .def("getPerformanceScore", [](const ActionResult& self) {
            float baseScore = self.success ? 1.0f : 0.0f;
            return baseScore + (self.reward > 0.0f ? self.reward * 0.5f : 0.0f);
        }, "Get performance score (0-2 range)")
        .def("toJson", [](const ActionResult& self) {
            py::dict json;
            json["reward"] = self.reward;
            json["success"] = self.success;
            json["message"] = self.message;
            return json;
        }, "Convert action result to JSON representation")
        .def("fromJson", [](ActionResult& self, py::dict json) {
            if (json.contains("reward")) self.reward = json["reward"].cast<float>();
            if (json.contains("success")) self.success = json["success"].cast<bool>();
            if (json.contains("message")) self.message = json["message"].cast<std::string>();
        }, py::arg("json"), "Load action result from JSON representation");

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
        .def("getPerceptStats", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            if (signals.empty()) return py::make_tuple(0.0f, 0.0f, 0.0f, 0.0f);
            
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            float mean = sum / signals.size();
            
            float sqSum = std::inner_product(signals.begin(), signals.end(), signals.begin(), 0.0f);
            float variance = sqSum / signals.size() - mean * mean;
            float stddev = std::sqrt(std::max(0.0f, variance));
            
            float min = *std::min_element(signals.begin(), signals.end());
            float max = *std::max_element(signals.begin(), signals.end());
            
            return py::make_tuple(min, max, mean, stddev);
        }, "Get percept statistics (min, max, mean, stddev)")
        .def("getDominantSignal", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            if (signals.empty()) return 0.0f;
            
            float maxVal = *std::max_element(signals.begin(), signals.end());
            size_t maxIndex = std::distance(signals.begin(), std::max_element(signals.begin(), signals.end()));
            
            std::string signalType;
            if (maxIndex < 10) signalType = "Vision";
            else if (maxIndex < 20) signalType = "Touch";
            else if (maxIndex < 30) signalType = "Internal";
            else if (maxIndex < 40) signalType = "Proprioception";
            else signalType = "Audio";
            
            return py::make_tuple(maxVal, signalType);
        }, "Get dominant signal value and type")
        .def("normalizePercept", [](SensoryPercept& self) {
            auto signals = self.getAllSignals();
            float maxVal = *std::max_element(signals.begin(), signals.end());
            float minVal = *std::min_element(signals.begin(), signals.end());
            float range = maxVal - minVal;
            
            if (range > 0) {
                std::vector<float> normalized;
                normalized.reserve(signals.size());
                for (float val : signals) normalized.push_back((val - minVal) / range);
                self.setInternal(normalized);
            }
            
            return normalized;
        }, "Normalize all signals in percept to [0, 1] range")
        .def("addNoise", [](SensoryPercept& self, float noiseLevel) {
            auto signals = self.getAllSignals();
            std::mt19937 rng(std::random_device{}());
            std::normal_distribution<float> dist(0.0f, noiseLevel);
            
            for (auto& val : signals) {
                val += dist(rng);
            }
            self.setInternal(signals);
            return signals;
        }, py::arg("noiseLevel"), "Add Gaussian noise to all signals")
        .def("getPerceptSummary", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            size_t totalSignals = signals.size();
            if (totalSignals == 0) return std::string("Empty percept");
            
            float totalActivity = std::accumulate(signals.begin(), signals.end(), 0.0f);
            float avgActivity = totalActivity / totalSignals;
            
            return "Percept with " + std::to_string(totalSignals) + " signals, " +
                   "avg activity: " + std::to_string(avgActivity);
        }, "Get human-readable summary of percept")
        .def("toJson", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            py::dict json;
            py::list signalList;
            for (float s : signals) signalList.append(s);
            json["signals"] = signalList;
            json["timestamp"] = self.getTimestamp();
            return json;
        }, "Convert percept to JSON representation")
        .def("fromJson", [](SensoryPercept& self, py::dict json) {
            if (json.contains("signals")) {
                py::list signalList = json["signals"].cast<py::list>();
                std::vector<float> signals;
                for (auto item : signalList) {
                    signals.push_back(item.cast<float>());
                }
                self.setInternal(signals);
            }
            if (json.contains("timestamp")) {
                self.setTimestamp(json["timestamp"].cast<float>());
            }
        }, py::arg("json"), "Load percept from JSON representation");

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
        
        // Advanced SimpleWorld methods
        .def("getAgentPosition", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return py::make_tuple(body.x, body.y);
        }, "Get agent position (x, y)")
        .def("getAgentOrientation", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.orientation;
        }, "Get agent orientation (radians)")
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("moveAgent", [](SimpleWorld& self, float dx, float dy) {
            auto body = self.getAgentBody();
            self.setAgentStart(body.x + dx, body.y + dy);
        }, py::arg("dx"), py::arg("dy"), "Move agent by delta")
        .def("rotateAgent", [](SimpleWorld& self, float angle) {
            auto body = self.getAgentBody();
            body.orientation += angle;
        }, py::arg("angle"), "Rotate agent by angle")
        .def("getWorldObjectAt", [](const SimpleWorld& self, float x, float y) {
            auto objects = self.getObjects(); // If this method exists
            for (const auto& obj : objects) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (dx * dx + dy * dy <= obj.radius * obj.radius) {
                    return obj;
                }
            }
            return WorldObject(); // Return empty object if not found
        }, py::arg("x"), py::arg("y"), "Get world object at position (will fail if method doesn't exist)")
        .def("getVisibleObjects", [](const SimpleWorld& self, float agentX, float agentY, float radius) {
            auto objects = self.getObjects(); // If this method exists
            std::vector<WorldObject> visible;
            for (const auto& obj : objects) {
                float dx = obj.x - agentX;
                float dy = obj.y - agentY;
                float distance = std::sqrt(dx * dx + dy * dy);
                if (distance <= radius && distance <= obj.radius) {
                    visible.push_back(obj);
                }
            }
            return visible;
        }, py::arg("agentX"), py::arg("agentY"), py::arg("radius"),
           "Get visible world objects within radius (will fail if method doesn't exist)")
        .def("getEnvironmentStats", [](const SimpleWorld& self) {
            return py::make_tuple(
                self.getWidth(),
                self.getHeight(),
                self.getMaxEnergy(),
                self.getEnergyDecayRate(),
                self.getSimulationTime(),
                self.getRandomSeed()
            );
        }, "Get world environment statistics")
        .def("spawnResource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject resource;
            resource.x = x;
            resource.y = y;
            resource.type = WorldObjectType::Resource;
            resource.value = value;
            resource.radius = 0.3f;
            resource.active = true;
            self.addObject(resource);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
           "Spawn resource at position (will fail if method doesn't exist)")
        .def("spawnHazard", [](SimpleWorld& self, float x, float y, float dangerLevel = 1.0f) {
            WorldObject hazard;
            hazard.x = x;
            hazard.y = y;
            hazard.type = WorldObjectType::Hazard;
            hazard.value = dangerLevel;
            hazard.radius = 0.4f;
            hazard.active = true;
            self.addObject(hazard);
        }, py::arg("x"), py::arg("y"), py::arg("dangerLevel") = 1.0f,
           "Spawn hazard at position (will fail if method doesn't exist)")
        .def("getWorldSize", [](const SimpleWorld& self) {
            return py::make_tuple(self.getWidth(), self.getHeight());
        }, "Get world dimensions (width, height)")
        .def("getWorldArea", [](const SimpleWorld& self) {
            return self.getWidth() * self.getHeight();
        }, "Get world area in square units")
        .def("setWorldBoundaries", [](SimpleWorld& self, float boundaryRadius) {
            // Implement boundary checking logic
        }, py::arg("boundaryRadius"), "Set world boundary radius (will fail if method doesn't exist)")
        .def("checkBoundaries", [](SimpleWorld& self, float& x, float& y) {
            // Implement boundary checking logic
        }, py::arg("x"), py::arg("y"), "Check and correct for world boundaries (will fail if method doesn't exist)");

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
        
        // Enhanced Brain methods
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system for neural dynamics")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get spike-timing dependent plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system for sensory prediction")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner for action planning")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system for neural maturation")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulation system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulation system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error neuromodulation system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"), "Apply neuromodulatory signal")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        
        // Advanced Brain management methods
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add inter-region connection")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"), "Remove inter-region connection")
        .def("getAllNeurons", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions (alias for getTotalNeuronCount)")
        .def("getAllRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions (convenience method)")
        .def("getStatistics", [](const Brain& self) {
            return py::make_tuple(
                self.getTotalNeuronCount(),
                self.getTotalSynapseCount(),
                self.getActiveNeuronCount(),
                self.getFiringNeuronCount(),
                self.getAverageFiringRate(),
                self.getExcitationInhibitionRatio(),
                self.getTotalSpikeCount(),
                self.getPendingSpikeEventCount()
            );
        }, "Get comprehensive brain statistics")
        .def("getSystemInfo", [](const Brain& self) {
            return py::make_tuple(
                self.getDevelopmentalStage(),
                self.getConfig() != nullptr,
                self.getSpikeSystem() != nullptr,
                self.getSTDP() != nullptr,
                self.getWorkingMemory() != nullptr,
                self.getEpisodicMemory() != nullptr,
                self.getAssociativeMemory() != nullptr,
                self.getPredictionSystem() != nullptr,
                self.getPlanner() != nullptr,
                self.getConceptFormation() != nullptr,
                self.getAttention() != nullptr,
                self.getDevelopmentSystem() != nullptr,
                self.getDopamine() != nullptr,
                self.getCuriosity() != nullptr,
                self.getNovelty() != nullptr,
                self.getPredictionErrorSignal() != nullptr
            );
        }, "Get system information and active components");

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
             py::arg("enable"), "Enable or disable development system")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity system")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity system is enabled")
        
        // Advanced AgentBrain methods
        .def("decodeMotorCommandWithExploration", 
             &AgentBrain::selectWithCuriosity,
             py::arg("defaultCmd"), "Decode motor command with curiosity-based exploration")
        .def("processSensoryInputWithContext",
             [](AgentBrain& self, const SensoryPercept& percept, bool useWorkingMemory = true) {
                 self.processSensoryInput(percept);
             }, py::arg("percept"), py::arg("useWorkingMemory") = true,
             "Process sensory input with working memory context")
        .def("applyNeuromodulationSignals",
             [](AgentBrain& self, float reward, float novelty, float predictionError, float curiosity) {
                 self.applyRewardModulation(reward, curiosity);
                 // Novelty and prediction error effects are internal to the brain
             }, py::arg("reward"), py::arg("novelty"), py::arg("predictionError"), py::arg("curiosity"),
             "Apply comprehensive neuromodulation signals")
        .def("updateDevelopmentWithStage",
             [](AgentBrain& self, double timestep, DevelopmentalStage stage) {
                 self.setDevelopmentalStage(stage);
                 self.updateDevelopment(timestep);
             }, py::arg("timestep"), py::arg("stage"), "Update development with specific stage")
        .def("getBrainStats",
             [](const AgentBrain& self) {
                 auto* brain = self.getBrain();
                 return py::make_tuple(
                     brain->getTotalNeuronCount(),
                     brain->getTotalSynapseCount(),
                     brain->getActiveNeuronCount(),
                     brain->getFiringNeuronCount(),
                     brain->getAverageFiringRate()
                 );
             }, "Get comprehensive brain statistics")
        .def("getNeuromodulationState",
             [](const AgentBrain& self) {
                 return py::make_tuple(
                     self.getNeuromodulationLevel(),
                     self.getCuriosityLevel(),
                     self.getNoveltyLevel(),
                     self.getPredictionError()
                 );
             }, "Get current neuromodulation state")
        .def("getDevelopmentState",
             [](const AgentBrain& self) {
                 return py::make_tuple(
                     self.getDevelopmentalStage(),
                     self.isDevelopmentEnabled(),
                     self.isRewardModulationEnabled(),
                     self.isStructuralPlasticityEnabled(),
                     self.isCuriosityEnabled()
                 );
             }, "Get current development and plasticity state");

    // Memory system classes
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory system for transient information storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information pattern in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store activation to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve, "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate, "Get memory decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set memory decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation (for rehearsal)")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is part of winning population")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Episodic memory item representing a stored experience)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("sensoryState", &EpisodicMemoryItem::sensoryState)
        .def_readwrite("positionX", &EpisodicMemoryItem::positionX)
        .def_readwrite("positionY", &EpisodicMemoryItem::positionY)
        .def_readwrite("orientation", &EpisodicMemoryItem::orientation)
        .def_readwrite("action", &EpisodicMemoryItem::action)
        .def_readwrite("reward", &EpisodicMemoryItem::reward)
        .def_readwrite("energy", &EpisodicMemoryItem::energy)
        .def_readwrite("novelty", &EpisodicMemoryItem::novelty)
        .def_readwrite("resultingSensoryState", &EpisodicMemoryItem::resultingSensoryState)
        .def_readwrite("resultingReward", &EpisodicMemoryItem::resultingReward)
        .def_readwrite("activeNeurons", &EpisodicMemoryItem::activeNeurons)
        .def_readwrite("neuronActivations", &EpisodicMemoryItem::neuronActivations)
        .def_readwrite("age", &EpisodicMemoryItem::age)
        .def("getDuration", [](const EpisodicMemoryItem& self) {
            return self.age;
        }, "Get memory age/duration")
        .def("getPosition", [](const EpisodicMemoryItem& self) {
            return py::make_tuple(self.positionX, self.positionY);
        }, "Get position (x, y)")
        .def("getOutcomeValue", [](const EpisodicMemoryItem& self) {
            return self.reward + self.energy + self.novelty;
        }, "Get overall outcome value (reward + energy + novelty)")
        .def("isPositive", [](const EpisodicMemoryItem& self) {
            return self.reward > 0.0f && self.resultingReward > self.reward;
        }, "Check if this was a positive experience")
        .def("isRewardIncreased", [](const EpisodicMemoryItem& self) {
            return self.resultingReward > self.reward;
        }, "Check if reward increased after this action")
        .def("toString", [](const EpisodicMemoryItem& self) {
            return "EpisodicMemoryItem(timestamp=" + std::to_string(self.timestamp) +
                   ", position=(" + std::to_string(self.positionX) + ", " + std::to_string(self.positionY) +
                   "), reward=" + std::to_string(self.reward) + ", action=" +
                   std::to_string(static_cast<int>(self.action)) + ")";
        }, "Get string representation");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory system for experience storage and retrieval)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new experience episode")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve episodes similar to query pattern")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             "Retrieve episodes following specific action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay an episode - reactivates neural patterns")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"),
             "Update episode relevance based on recent use")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"),
             "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"),
             "Clear old episodes based on relevance threshold")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
             "Enable or disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"),
             "Get episodes selected for replay (relevance and recency)")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"),
             "Replay multiple episodes (for sleep-like consolidation)")
        .def("getMemoryStats", [](const NeuralEpisodicMemory& self) {
            return py::make_tuple(
                self.getEpisodeCount(),
                self.getAverageReward(),
                self.isReplayEnabled()
            );
        }, "Get memory system statistics");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory system for pattern relationships)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between two neural patterns")
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience,
             py::arg("episode"), "Create association based on experience")
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve patterns associated with query pattern")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"),
             "Get association strength between two patterns")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association based on outcome")
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2,
             "Spread activation from cue pattern")
        .def("clear", &NeuralAssociativeMemory::clear, "Clear associations")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of associations")
        .def("getNetworkDensity", [](const NeuralAssociativeMemory& self) {
            float count = static_cast<float>(self.getAssociationCount());
            // Would need total patterns for density calculation
            return count; // Simplified
        }, "Get network association density");

    // Neuromodulation classes
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get neuromodulatory effect on plasticity")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward event")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"), "Signal reward prediction error");

    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Attention and memory consolidation)pbdoc")
        .def(py::init<>());

    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Arousal and vigilance)pbdoc")
        .def(py::init<>());

    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Mood, impulsivity, and social behavior)pbdoc")
        .def(py::init<>());

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"))
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set weight for novelty in curiosity calculation")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set weight for prediction error in curiosity calculation")
        .def("reset", &Curiosity::reset, "Reset curiosity state");

    // Development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(System for neural development and maturation)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance to next developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get stage name")
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("currentStep"),
             "Update development (simple version)")
        .def("updateWithParams", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age (in simulation time)")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age");

    // Experiment system
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Experiment data recording and analysis)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("name"))
        .def("getName", &Experiment::getName, "Get experiment name")
        .def("setName", &Experiment::setName, py::arg("name"), "Set experiment name")
        .def("getSeed", &Experiment::getSeed, "Get random seed")
        .def("setSeed", &Experiment::setSeed, py::arg("seed"), "Set random seed")
        .def("setConfigValue", &Experiment::setConfigValue, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("getConfigValue", &Experiment::getConfigValue, py::arg("key"),
             "Get configuration value")
        .def("recordMetric", py::overload_cast<const std::string&, double>(&Experiment::recordMetric),
             py::arg("name"), py::arg("value"), "Record float metric")
        .def("recordMetric", py::overload_cast<const std::string&, float>(&Experiment::recordMetric),
             py::arg("name"), py::arg("value"), "Record float metric")
        .def("recordMetric", py::overload_cast<const std::string&, int>(&Experiment::recordMetric),
             py::arg("name"), py::arg("value"), "Record integer metric")
        .def("getMetrics", &Experiment::getMetrics, "Get all metrics")
        .def("getMetric", &Experiment::getMetric, py::arg("name"), "Get metric value")
        .def("hasMetric", &Experiment::hasMetric, py::arg("name"), "Check if metric exists")
        .def("recordStep", &Experiment::recordStep,
             py::arg("step"), py::arg("neuronCount"), py::arg("synapseCount"), py::arg("firingCount"), py::arg("avgFiringRate"),
             "Record step data")
        .def("getStepData", &Experiment::getStepData, "Get step data")
        .def("getStartStep", &Experiment::getStartStep, "Get start step")
        .def("setStartStep", &Experiment::setStartStep, py::arg("step"), "Set start step")
        .def("getEndStep", &Experiment::getEndStep, "Get end step")
        .def("setEndStep", &Experiment::setEndStep, py::arg("step"), "Set end step")
        .def("addNote", &Experiment::addNote, py::arg("note"), "Add note")
        .def("getNotes", &Experiment::getNotes, "Get notes");

    // Performance and monitoring
    py::class_<PerformanceStats>(m, "PerformanceStats", R"pbdoc(Performance monitoring statistics)pbdoc")
        .def(py::init<>())
        .def_readwrite("neuronCount", &PerformanceStats::neuronCount)
        .def_readwrite("synapseCount", &PerformanceStats::synapseCount)
        .def_readwrite("activeNeuronCount", &PerformanceStats::activeNeuronCount)
        .def_readwrite("firingNeuronCount", &PerformanceStats::firingNeuronCount)
        .def_readwrite("averageFiringRate", &PerformanceStats::averageFiringRate)
        .def_readwrite("excitationInhibitionRatio", &PerformanceStats::excitationInhibitionRatio)
        .def_readwrite("totalSpikeCount", &PerformanceStats::totalSpikeCount)
        .def_readwrite("pendingSpikeEventCount", &PerformanceStats::pendingSpikeEventCount)
        .def_readwrite("memoryUsage", &PerformanceStats::memoryUsage)
        .def_readwrite("cpuUsage", &PerformanceStats::cpuUsage)
        .def_readwrite("simulationTime", &PerformanceStats::simulationTime)
        .def("getSummary", [](const PerformanceStats& stats) {
            std::stringstream ss;
            ss << "Performance: " << stats.neuronCount << " neurons, "
               << stats.synapseCount << " synapses, "
               << stats.activeNeuronCount << " active, "
               << stats.firingNeuronCount << " firing, "
               << stats.averageFiringRate << " Hz avg rate, "
               << stats.totalSpikeCount << " total spikes";
            return ss.str();
        }, "Get performance summary string");

} // namespace nlm
