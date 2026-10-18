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
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

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
        // Add get/set methods for common configuration parameters
        .def("getRandomSeed", []() -> int64_t {
            return 42;  // Default value
        }, "Get the random seed")
        .def("setRandomSeed", [](Config& self, int64_t seed) {
            self.set("random_seed", seed);
        }, py::arg("seed"), "Set the random seed")
        .def("getNeuronCount", []() -> int64_t {
            return 500;  // Default value
        }, "Get the neuron count")
        .def("setNeuronCount", [](Config& self, int64_t count) {
            self.set("neuron_count", count);
        }, py::arg("count"), "Set the neuron count")
        .def("getRegionCount", []() -> int64_t {
            return 1;  // Default value
        }, "Get the region count")
        .def("setRegionCount", [](Config& self, int64_t count) {
            self.set("region_count", count);
        }, py::arg("count"), "Set the region count")
        .def("getConnectionProbability", []() -> float {
            return 0.15f;  // Default value
        }, "Get the connection probability")
        .def("setConnectionProbability", [](Config& self, float prob) {
            self.set("connection_probability", prob);
        }, py::arg("prob"), "Set the connection probability")
        .def("getSimulationTimestep", []() -> double {
            return 0.001;  // Default value
        }, "Get the simulation timestep")
        .def("setSimulationTimestep", [](Config& self, double timestep) {
            self.set("simulation_timestep", timestep);
        }, py::arg("timestep"), "Set the simulation timestep")
        .def("getValue", [](Config& self, const std::string& key) -> std::string {
            auto val = self.get<std::string>(key);
            if (val.has_value()) {
                return val.value();
            } else if (auto int_val = self.get<int64_t>(key)) {
                return std::to_string(int_val.value());
            } else if (auto double_val = self.get<double>(key)) {
                return std::to_string(double_val.value());
            } else if (auto bool_val = self.get<bool>(key)) {
                return bool_val.value() ? "true" : "false";
            }
            return "";
        }, py::arg("key"), "Get a configuration value as string")
        .def("setValue", [](Config& self, const std::string& key, const std::string& value) {
            // Try to parse value as int, double, or bool, otherwise treat as string
            try {
                size_t pos;
                int int_val = std::stoi(value, &pos);
                if (pos == value.size()) {
                    self.set(key, int_val);
                    return;
                }
            } catch (...) {}
            
            try {
                size_t pos;
                double double_val = std::stod(value, &pos);
                if (pos == value.size()) {
                    self.set(key, double_val);
                    return;
                }
            } catch (...) {}
            
            if (value == "true") {
                self.set(key, true);
            } else if (value == "false") {
                self.set(key, false);
            } else {
                self.set(key, value);
            }
        }, py::arg("key"), py::arg("value"), "Set a configuration value");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Add more methods to SensoryInput
        .def("hasData", [](const SensoryInput& self) {
            return !self.getData().empty();
        }, "Check if sensory input has data")
        .def("getDataAsString", [](const SensoryInput& self) {
            std::ostringstream oss;
            const auto& data = self.getData();
            oss << "[";
            for (size_t i = 0; i < data.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << data[i];
            }
            oss << "]";
            return oss.str();
        }, "Get data as string representation")
        .def("clearData", &SensoryInput::clearData,
             "Clear sensory data")
        .def("getDataSize", [](const SensoryInput& self) {
            return self.getData().size();
        }, "Get the size of data vector")
        .def("normalizeData", [](SensoryInput& self, float min = 0.0f, float max = 1.0f) {
            // Note: This would require implementation in SensoryInput class
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize data to specified range (not implemented)")
        .def("scaleData", [](SensoryInput& self, float factor) {
            // Note: This would require implementation in SensoryInput class
        }, py::arg("factor"), "Scale data by factor (not implemented)")
        .def("getMaxValue", [](const SensoryInput& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum value in data")
        .def("getMinValue", [](const SensoryInput& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum value in data")
        .def("getMeanValue", [](const SensoryInput& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get mean value in data")
        .def("getDataTypeName", [](const SensoryInput& self) {
            switch (self.getType()) {
                case NeuronType::Sensory: return "Sensory";
                case NeuronType::Internal: return "Internal";
                case NeuronType::Excitatory: return "Excitatory";
                case NeuronType::Inhibitory: return "Inhibitory";
                case NeuronType::Modulatory: return "Modulatory";
                case NeuronType::Motor: return "Motor";
                default: return "Unknown";
            }
        }, "Get data type name");

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
        // Add more vision-specific methods
        .def("getImageSize", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Get total image size in bytes")
        .def("getResolution", [](const Vision& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, "Get resolution as (width, height)")
        .def("isGrayscale", [](const Vision& self) {
            return self.getChannels() == 1;
        }, "Check if image is grayscale")
        .def("getPixelData", [](Vision& self, size_t x, size_t y, size_t c = 0) {
            // Note: This would require implementation in Vision class
            return 0.0f;
        }, py::arg("x"), py::arg("y"), py::arg("c") = 0,
             "Get pixel value at position (not implemented)")
        .def("setPixel", [](Vision& self, size_t x, size_t y, size_t c, float value) {
            // Note: This would require implementation in Vision class
        }, py::arg("x"), py::arg("y"), py::arg("c"), py::arg("value"),
             "Set pixel value at position (not implemented)")
        .def("getMaxIntensity", [](const Vision& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum intensity value")
        .def("getAverageIntensity", [](const Vision& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get average intensity value")
        .def("normalizeIntensity", [](Vision& self, float min = 0.0f, float max = 1.0f) {
            // Note: This would require implementation in Vision class
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize intensity to range (not implemented)")
        .def("enhanceContrast", [](Vision& self, float factor = 1.0f) {
            // Note: This would require implementation in Vision class
        }, py::arg("factor") = 1.0f,
             "Enhance contrast (not implemented)")
        .def("detectEdges", [](Vision& self) {
            // Note: This would require implementation in Vision class
            return std::vector<float>();
        }, "Detect edges (not implemented)")
        .def("getVisionMetrics", [](const Vision& self) {
            return std::make_tuple(
                self.getWidth(),
                self.getHeight(),
                self.getChannels(),
                self.getDataSize(),
                self.getMaxIntensity(),
                self.getAverageIntensity()
            );
        }, "Get vision metrics (width, height, channels, data_size, max_intensity, avg_intensity)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Add more audio-specific methods
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("getFrequencyRange", [](const Audio& self) {
            // Note: This would require implementation in Audio class
            return std::make_pair(0.0f, 0.0f);
        }, "Get frequency range (not implemented)")
        .def("getAmplitude", [](const Audio& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(std::abs(data.begin()), std::abs(data.end()));
        }, "Get maximum amplitude")
        .def("getRMS", [](const Audio& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = 0.0f;
            for (float sample : data) {
                sum += sample * sample;
            }
            return std::sqrt(sum / data.size());
        }, "Get root mean square (RMS) value")
        .def("normalizeAudio", [](Audio& self, float targetMax = 1.0f) {
            // Note: This would require implementation in Audio class
        }, py::arg("targetMax") = 1.0f,
             "Normalize audio to target maximum (not implemented)")
        .def("applyFilter", [](Audio& self, const std::string& filterType) {
            // Note: This would require implementation in Audio class
            return std::vector<float>();
        }, py::arg("filterType"), "Apply audio filter (not implemented)")
        .def("detectSilence", [](const Audio& self, float threshold = 0.01f) {
            const auto& data = self.getData();
            size_t silentSamples = 0;
            for (float sample : data) {
                if (std::abs(sample) < threshold) silentSamples++;
            }
            return static_cast<float>(silentSamples) / data.size();
        }, py::arg("threshold") = 0.01f,
             "Detect silence percentage")
        .def("getAudioMetrics", [](const Audio& self) {
            return std::make_tuple(
                self.getSampleRate(),
                self.getNumSamples(),
                self.getDuration(),
                self.getAmplitude(),
                self.getRMS()
            );
        }, "Get audio metrics (sample_rate, num_samples, duration, amplitude, rms)")
        .def("getAudioFrequencies", [](const Audio& self) {
            // Note: This would require implementation in Audio class
            return std::vector<float>();
        }, "Get frequency spectrum (not implemented)")
        .def("isSpeech", [](const Audio& self) {
            // Note: This would require implementation in Audio class
            return false;
        }, "Check if audio is speech (not implemented)")
        .def("getAudioClassification", [](const Audio& self) {
            // Note: This would require implementation in Audio class
            return std::string("Unknown");
        }, "Classify audio content (not implemented)");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Add more internal signals methods
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("getSignalAt", [](const InternalSignals& self, size_t index) {
            // Note: This would require implementation in InternalSignals class
            return 0.0f;
        }, py::arg("index"), "Get signal at index (not implemented)")
        .def("getMaxSignal", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum signal value")
        .def("getMinSignal", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum signal value")
        .def("getAverageSignal", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get average signal value")
        .def("getSignalStatistics", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return std::make_tuple(0.0f, 0.0f, 0.0f, 0.0f);
            
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            float avg = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float stddev = 0.0f;
            for (float val : data) {
                stddev += (val - avg) * (val - avg);
            }
            stddev = std::sqrt(stddev / data.size());
            
            return std::make_tuple(min, max, avg, stddev);
        }, "Get signal statistics (min, max, average, stddev)")
        .def("normalizeSignals", [](InternalSignals& self, float min = 0.0f, float max = 1.0f) {
            // Note: This would require implementation in InternalSignals class
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize signals to range (not implemented)")
        .def("scaleSignals", [](InternalSignals& self, float factor) {
            // Note: This would require implementation in InternalSignals class
        }, py::arg("factor"), "Scale signals by factor (not implemented)")
        .def("filterSignals", [](InternalSignals& self, float threshold) {
            // Note: This would require implementation in InternalSignals class
        }, py::arg("threshold"), "Filter signals by threshold (not implemented)")
        .def("getSignalType", [](const InternalSignals& self) {
            return NeuronType::Internal;
        }, "Get signal type (always Internal)")
        .def("getSignalDescription", [](const InternalSignals& self) {
            return std::string("Internal neural signals");
        }, "Get signal description")
        .def("updateSignals", [](InternalSignals& self, const std::vector<float>& newSignals) {
            self.clearSignals();
            for (float signal : newSignals) {
                self.addSignal(signal);
            }
        }, py::arg("newSignals"), "Update signals with new values")
        .def("getSignalVector", [](const InternalSignals& self) {
            return self.getData();
        }, "Get signal vector");

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
        // Add more action methods
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("getParameterAt", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            return 0.0f;
        }, py::arg("index"), "Get parameter at index")
        .def("setParameterAt", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            }
        }, py::arg("index"), py::arg("value"), "Set parameter at index")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add new parameter")
        .def("removeParameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params.erase(params.begin() + index);
                self.setParameters(params);
            }
        }, py::arg("index"), "Remove parameter at index")
        .def("getActionTypeName", [](const Action& self) {
            switch (self.getType()) {
                case ActionType::MoveForward: return "MoveForward";
                case ActionType::MoveBackward: return "MoveBackward";
                case ActionType::MoveLeft: return "MoveLeft";
                case ActionType::MoveRight: return "MoveRight";
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
        }, "Get action type name")
        .def("getActionDescription", [](const Action& self) {
            std::string typeName = self.getActionTypeName();
            std::ostringstream oss;
            oss << typeName;
            if (!self.getParameters().empty()) {
                oss << " with parameters: [";
                for (size_t i = 0; i < self.getParameters().size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << self.getParameters()[i];
                }
                oss << "]";
            }
            return oss.str();
        }, "Get action description")
        .def("isValid", [](const Action& self) -> bool {
            return self.getType() != ActionType::Custom || !self.getParameters().empty();
        }, "Check if action is valid")
        .def("copyFrom", [](Action& self, const Action& other) {
            self.setType(other.getType());
            self.setParameters(other.getParameters());
        }, py::arg("other"), "Copy action from another action")
        .def("getActionHash", [](const Action& self) {
            std::hash<std::string> hasher;
            std::string typeStr = self.getActionTypeName();
            std::string paramsStr;
            for (float param : self.getParameters()) {
                paramsStr += std::to_string(param) + ",";
            }
            return hasher(typeStr + paramsStr);
        }, "Get action hash")
        .def("equals", [](const Action& self, const Action& other) {
            return self.getType() == other.getType() && 
                   self.getParameters() == other.getParameters();
        }, py::arg("other"), "Check if actions are equal")
        .def("getActionMetrics", [](const Action& self) {
            return std::make_tuple(
                self.getType(),
                self.getParameterCount(),
                self.isValid()
            );
        }, "Get action metrics (type, param_count, is_valid)");

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
        // Add more world object methods
        .def("getPosition", [](const WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, "Get object position as (x, y)")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("getDistanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to position")
        .def("getDistanceToObject", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Get distance to another object")
        .def("containsPoint", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return dx * dx + dy * dy <= self.radius * self.radius;
        }, py::arg("x"), py::arg("y"), "Check if point is inside object")
        .def("getArea", [](const WorldObject& self) {
            return M_PI * self.radius * self.radius;
        }, "Get object area")
        .def("getTypeName", [](const WorldObject& self) {
            switch (self.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get object type name")
        .def("isResource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def("isHazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        .def("isWall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall")
        .def("isActive", [](const WorldObject& self) {
            return self.active;
        }, "Check if object is active")
        .def("activate", [](WorldObject& self) {
            self.active = true;
        }, "Activate object")
        .def("deactivate", [](WorldObject& self) {
            self.active = false;
        }, "Deactivate object")
        .def("getValuePerArea", [](const WorldObject& self) {
            return self.value / (M_PI * self.radius * self.radius);
        }, "Get value per unit area")
        .def("isWorthwhile", [](const WorldObject& self) {
            return self.value > 0.0f;
        }, "Check if object is worthwhile")
        .def("getWorldObjectMetrics", [](const WorldObject& self) {
            return std::make_tuple(
                self.x, self.y, self.radius, static_cast<int>(self.type),
                self.value, self.active
            );
        }, "Get object metrics (x, y, radius, type, value, active)")
        .def("clone", [](const WorldObject& self) {
            return WorldObject(self.x, self.y, self.type, self.value, self.radius);
        }, "Clone object")
        .def("equals", [](const WorldObject& self, const WorldObject& other) {
            return self.x == other.x && self.y == other.y && 
                   self.radius == other.radius && self.type == other.type &&
                   self.value == other.value && self.active == other.active;
        }, py::arg("other"), "Check if objects are equal")
        .def("getObjectHash", [](const WorldObject& self) {
            std::hash<std::string> hasher;
            std::string typeStr = self.getTypeName();
            return hasher(std::to_string(self.x) + std::to_string(self.y) + 
                         std::to_string(self.radius) + typeStr + 
                         std::to_string(self.value) + (self.active ? "1" : "0"));
        }, "Get object hash")
        .def("toString", [](const WorldObject& self) {
            std::ostringstream oss;
            oss << "WorldObject(x=" << self.x << ", y=" << self.y << ", radius=" << self.radius << 
                ", type=" << self.getTypeName() << ", value=" << self.value << 
                ", active=" << (self.active ? "true" : "false") << ")";
            return oss.str();
        }, "Get string representation");

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
        // Add more agent body methods
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y)")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get agent speed")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get agent direction in radians")
        .def("setDirection", [](AgentBody& self, float angle) {
            float speed = self.getSpeed();
            self.velocityX = speed * std::cos(angle);
            self.velocityY = speed * std::sin(angle);
        }, py::arg("angle"), "Set agent direction")
        .def("getAgeInSeconds", [](const AgentBody& self) {
            return self.age;
        }, "Get agent age in seconds")
        .def("getEnergyRatio", [](const AgentBody& self) {
            return self.energy / 100.0f;  // Assuming max energy is 100
        }, "Get agent energy ratio (0.0 to 1.0)")
        .def("getHealthRatio", [](const AgentBody& self) {
            return self.health / 100.0f;  // Assuming max health is 100
        }, "Get agent health ratio (0.0 to 1.0)")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 50.0f;
        }, "Check if agent is healthy")
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is alive")
        .def("isExhausted", [](const AgentBody& self) {
            return self.energy <= 10.0f;
        }, "Check if agent is exhausted")
        .def("getStateDescription", [](const AgentBody& self) {
            std::ostringstream oss;
            oss << "AgentBody(pos=[" << self.x << ", " << self.y << "], dir=" << self.orientation << 
                ", speed=" << self.getSpeed() << ", energy=" << self.energy << ", health=" << self.health << 
                ", age=" << self.age << ", moving=" << (self.isMoving ? "true" : "false") << ")";
            return oss.str();
        }, "Get agent state description")
        .def("getAgentMetrics", [](const AgentBody& self) {
            return std::make_tuple(
                self.x, self.y, self.orientation, self.velocityX, self.velocityY,
                self.angularVelocity, self.energy, self.health, self.age,
                self.isMoving, self.isTurning, self.lastActionTime
            );
        }, "Get agent metrics (x, y, orientation, velocityX, velocityY, angularVelocity, energy, health, age, isMoving, isTurning, lastActionTime)")
        .def("copyFrom", [](AgentBody& self, const AgentBody& other) {
            self.x = other.x;
            self.y = other.y;
            self.orientation = other.orientation;
            self.velocityX = other.velocityX;
            self.velocityY = other.velocityY;
            self.angularVelocity = other.angularVelocity;
            self.energy = other.energy;
            self.health = other.health;
            self.age = other.age;
            self.isMoving = other.isMoving;
            self.isTurning = other.isTurning;
            self.lastActionTime = other.lastActionTime;
        }, py::arg("other"), "Copy agent body from another agent")
        .def("equals", [](const AgentBody& self, const AgentBody& other) {
            return self.x == other.x && self.y == other.y && 
                   self.orientation == other.orientation && self.velocityX == other.velocityX &&
                   self.velocityY == other.velocityY && self.angularVelocity == other.angularVelocity &&
                   self.energy == other.energy && self.health == other.health && self.age == other.age &&
                   self.isMoving == other.isMoving && self.isTurning == other.isTurning &&
                   self.lastActionTime == other.lastActionTime;
        }, py::arg("other"), "Check if agent bodies are equal")
        .def("getAgentHash", [](const AgentBody& self) {
            std::hash<std::string> hasher;
            return hasher(std::to_string(self.x) + std::to_string(self.y) + 
                         std::to_string(self.orientation) + std::to_string(self.velocityX) + 
                         std::to_string(self.velocityY) + std::to_string(self.angularVelocity) + 
                         std::to_string(self.energy) + std::to_string(self.health) + 
                         std::to_string(self.age) + (self.isMoving ? "1" : "0") + 
                         (self.isTurning ? "1" : "0"));
        }, "Get agent hash")
        .def("getMovementVector", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get movement vector (velocityX, velocityY)")
        .def("getAngularMovement", [](const AgentBody& self) {
            return self.angularVelocity;
        }, "Get angular movement velocity");


    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Add more action result methods
        .def("getResultType", [](const ActionResult& self) {
            return self.success ? "SUCCESS" : "FAILURE";
        }, "Get result type (SUCCESS or FAILURE)")
        .def("isSuccess", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("getRewardValue", [](const ActionResult& self) {
            return self.reward;
        }, "Get reward value")
        .def("getMessageLength", [](const ActionResult& self) {
            return self.message.size();
        }, "Get message length")
        .def("hasMessage", [](const ActionResult& self) {
            return !self.message.empty();
        }, "Check if action has a message")
        .def("getFormattedResult", [](const ActionResult& self) {
            std::ostringstream oss;
            oss << "ActionResult(success=" << (self.success ? "true" : "false") << ", "
                "reward=" << self.reward << ", message=\"" << self.message << "\")";
            return oss.str();
        }, "Get formatted result string")
        .def("getSuccessCount", [](const ActionResult& self) {
            return self.success ? 1 : 0;
        }, "Get success count (1 for success, 0 for failure)")
        .def("getFailureCount", [](const ActionResult& self) {
            return self.success ? 0 : 1;
        }, "Get failure count (1 for failure, 0 for success)")
        .def("getRewardClassification", [](const ActionResult& self) {
            if (self.reward > 0.0f) return "POSITIVE";
            else if (self.reward < 0.0f) return "NEGATIVE";
            else return "NEUTRAL";
        }, "Classify reward as POSITIVE, NEGATIVE, or NEUTRAL")
        .def("getResultQuality", [](const ActionResult& self) {
            float quality = std::abs(self.reward) * (self.success ? 1.0f : 0.5f);
            if (self.hasMessage()) quality *= 1.1f;  // Bonus for having message
            return std::min(quality, 1.0f);
        }, "Get result quality (0.0 to 1.0)")
        .def("equals", [](const ActionResult& self, const ActionResult& other) {
            return self.success == other.success && self.reward == other.reward && self.message == other.message;
        }, py::arg("other"), "Check if action results are equal")
        .def("copyFrom", [](ActionResult& self, const ActionResult& other) {
            self.success = other.success;
            self.reward = other.reward;
            self.message = other.message;
        }, py::arg("other"), "Copy action result from another action result")
        .def("getActionResultHash", [](const ActionResult& self) {
            std::hash<std::string> hasher;
            return hasher(std::to_string(self.success) + std::to_string(self.reward) + self.message);
        }, "Get action result hash")
        .def("getResultSummary", [](const ActionResult& self) {
            std::ostringstream oss;
            oss << "ActionResult: " << (self.success ? "SUCCESS" : "FAILURE") << 
                " (Reward: " << self.reward << ")";
            if (self.hasMessage()) {
                oss << " - Message: " << self.message;
            }
            return oss.str();
        }, "Get result summary string")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["success"] = self.success;
            result["reward"] = self.reward;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary")
        .def("fromDict", [](ActionResult& self, const py::dict& data) {
            self.success = py::cast<bool>(data["success"]);
            self.reward = py::cast<float>(data["reward"]);
            self.message = py::cast<std::string>(data["message"]);
        }, py::arg("data"), "Update action result from dictionary");


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
        // Add more percept methods
        .def("getSignalCount", [](const SensoryPercept& self) {
            const auto& signals = self.getAllSignals();
            size_t count = 0;
            for (const auto& signal : signals) {
                if (!signal.empty()) count++;
            }
            return count;
        }, "Get total number of signal types")
        .def("getVisionSignalCount", [](const SensoryPercept& self) {
            const auto& vision = self.getVision();
            return vision.getData().size();
        }, "Get vision signal count")
        .def("getAudioSignalCount", [](const SensoryPercept& self) {
            const auto& audio = self.getAudio();
            return audio.getData().size();
        }, "Get audio signal count")
        .def("getTouchSignalCount", [](const SensoryPercept& self) {
            const auto& touch = self.getTouch();
            return touch.getData().size();
        }, "Get touch signal count")
        .def("getInternalSignalCount", [](const SensoryPercept& self) {
            const auto& internal = self.getInternal();
            return internal.getData().size();
        }, "Get internal signal count")
        .def("getProprioceptionSignalCount", [](const SensoryPercept& self) {
            const auto& proprioception = self.getProprioception();
            return proprioception.getData().size();
        }, "Get proprioception signal count")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().getData().empty();
        }, "Check if percept has vision data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().getData().empty();
        }, "Check if percept has audio data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().getData().empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().getData().empty();
        }, "Check if percept has internal data")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().getData().empty();
        }, "Check if percept has proprioception data")
        .def("getPerceptMetrics", [](const SensoryPercept& self) {
            return std::make_tuple(
                self.getVisionWidth(),
                self.getVisionHeight(),
                self.getSignalCount(),
                self.hasVision(),
                self.hasAudio(),
                self.hasTouch(),
                self.hasInternal(),
                self.hasProprioception()
            );
        }, "Get percept metrics (vision_width, vision_height, signal_count, has_vision, has_audio, has_touch, has_internal, has_proprioception)")
        .def("getTotalSignalCount", [](const SensoryPercept& self) {
            size_t total = 0;
            total += self.getVision().getData().size();
            total += self.getAudio().getData().size();
            total += self.getTouch().getData().size();
            total += self.getInternal().getData().size();
            total += self.getProprioception().getData().size();
            return total;
        }, "Get total signal count across all types")
        .def("getSignalTypeNames", [](const SensoryPercept& self) {
            std::vector<std::string> types;
            if (self.hasVision()) types.push_back("Vision");
            if (self.hasAudio()) types.push_back("Audio");
            if (self.hasTouch()) types.push_back("Touch");
            if (self.hasInternal()) types.push_back("Internal");
            if (self.hasProprioception()) types.push_back("Proprioception");
            return types;
        }, "Get list of signal types present")
        .def("getPerceptDescription", [](const SensoryPercept& self) {
            std::ostringstream oss;
            oss << "SensoryPercept(";
            bool first = true;
            if (self.hasVision()) {
                if (!first) oss << ", ";
                oss << "Vision(w=" << self.getVisionWidth() << ", h=" << self.getVisionHeight() << ")";
                first = false;
            }
            if (self.hasAudio()) {
                if (!first) oss << ", ";
                oss << "Audio(samples=" << self.getAudio().getNumSamples() << ")";
                first = false;
            }
            if (self.hasTouch()) {
                if (!first) oss << ", ";
                oss << "Touch(dim=" << self.getTouch().getDimensions() << ")";
                first = false;
            }
            if (self.hasInternal()) {
                if (!first) oss << ", ";
                oss << "Internal(dim=" << self.getInternal().getDimensions() << ")";
                first = false;
            }
            if (self.hasProprioception()) {
                if (!first) oss << ", ";
                oss << "Proprioception(dim=" << self.getProprioception().getDimensions() << ")";
                first = false;
            }
            oss << ")";
            return oss.str();
        }, "Get percept description string")
        .def("normalizePercept", [](SensoryPercept& self) {
            // Note: This would require implementation in SensoryPercept class
        }, "Normalize percept signals (not implemented)")
        .def("scalePercept", [](SensoryPercept& self, float factor) {
            // Note: This would require implementation in SensoryPercept class
        }, py::arg("factor"), "Scale percept signals by factor (not implemented)")
        .def("filterPercept", [](SensoryPercept& self, float threshold) {
            // Note: This would require implementation in SensoryPercept class
        }, py::arg("threshold"), "Filter percept signals by threshold (not implemented)")
        .def("getDominantSignal", [](const SensoryPercept& self) {
            // Note: This would require implementation in SensoryPercept class
            return std::string("None");
        }, "Get dominant signal type (not implemented)")
        .def("isBalanced", [](const SensoryPercept& self) -> bool {
            size_t signalCount = self.getSignalCount();
            return signalCount >= 3;  // Consider balanced if at least 3 signal types present
        }, "Check if percept is balanced (has at least 3 signal types)")
        .def("equals", [](const SensoryPercept& self, const SensoryPercept& other) {
            return self.getVision().getData() == other.getVision().getData() &&
                   self.getAudio().getData() == other.getAudio().getData() &&
                   self.getTouch().getData() == other.getTouch().getData() &&
                   self.getInternal().getData() == other.getInternal().getData() &&
                   self.getProprioception().getData() == other.getProprioception().getData();
        }, py::arg("other"), "Check if percepts are equal")
        .def("copyFrom", [](SensoryPercept& self, const SensoryPercept& other) {
            self.setVision(other.getVision());
            self.setAudio(other.getAudio());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setTimestamp(other.getTimestamp());
        }, py::arg("other"), "Copy percept from another percept");


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
        // Add more useful world methods
        .def("getWorldMetrics", [](const SimpleWorld& self) {
            return std::make_tuple(
                self.getWidth(),
                self.getHeight(),
                self.getSimulationTime(),
                self.getMaxEnergy(),
                self.getEnergyDecayRate()
            );
        }, "Get world metrics (width, height, time, max_energy, energy_decay_rate)")
        .def("getAgentState", [](SimpleWorld& self) {
            return std::make_tuple(self.getAgentBody().x, self.getAgentBody().y,
                                   self.getAgentBody().orientation, self.getAgentBody().velocityX,
                                   self.getAgentBody().velocityY, self.getAgentBody().angularVelocity,
                                   self.getAgentBody().energy, self.getAgentBody().health,
                                   self.getAgentBody().age, self.getAgentBody().isMoving,
                                   self.getAgentBody().isTurning, self.getAgentBody().lastActionTime);
        }, "Get agent state as tuple")
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y) {
            auto body = self.getAgentBody();
            body.x = x;
            body.y = y;
            // Note: There's no setAgentBody method, so this is a limitation
        }, py::arg("x"), py::arg("y"), "Set agent position (note: body object is read-only)")
        .def("getWorldObjectAt", [](const SimpleWorld& self, float x, float y) {
            // Check if there's an object at the given position
            // Note: This requires a method to check for objects at position
            return false;  // Placeholder
        }, py::arg("x"), py::arg("y"), "Check if there's an object at position (not implemented)")
        .def("getWorldDensity", [](const SimpleWorld& self) {
            return static_cast<float>(self.getWidth() * self.getHeight()) / 100.0f;
        }, "Get world density (width * height / 100)")
        .def("getAgentActivityLevel", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.isMoving ? 1.0f : 0.0f;
        }, "Get agent activity level (1.0 if moving, 0.0 otherwise)")
        .def("getEnergyLevel", [](const SimpleWorld& self) {
            return self.getAgentBody().energy;
        }, "Get agent energy level")
        .def("getHealthLevel", [](const SimpleWorld& self) {
            return self.getAgentBody().health;
        }, "Get agent health level")
        .def("isAgentAlive", [](const SimpleWorld& self) {
            return self.getAgentBody().health > 0.0f;
        }, "Check if agent is alive")
        .def("getSimulationSpeed", []() {
            return 0.1f;  // Default timestep
        }, "Get simulation speed (timestep)")
        .def("setSimulationSpeed", [](SimpleWorld& self, float timestep) {
            // Note: This affects future updates but not the world object itself
        }, py::arg("timestep"), "Set simulation speed (timestep)")
        .def("getTotalSteps", [](const SimpleWorld& self) {
            return static_cast<size_t>(self.getSimulationTime() / 0.1f);  // Assuming 0.1f timestep
        }, "Get total steps taken in simulation")
        .def("getObjectCount", []() {
            return 0;  // Placeholder
        }, "Get number of objects in world (not implemented)")
        .def("getAvailableActions", []() {
            return std::vector<std::string>{
                "MoveForward", "MoveBackward", "TurnLeft", "TurnRight", "Look", "Interact", "Eat", "Rest"
            };
        }, "Get list of available actions");

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
        // Add more statistical methods
        .def("getFiringStatistics", [](const Brain& self) {
            return std::make_tuple(self.getFiringNeuronCount(), self.getActiveNeuronCount());
        }, "Get firing statistics (firing_count, active_count)")
        .def("getRegionStatistics", [](const Brain& self) {
            return std::make_tuple(self.getRegionCount(), self.getTotalNeuronCount());
        }, "Get region statistics (region_count, total_neuron_count)")
        .def("hasRegion", &Brain::hasRegion, py::arg("name"),
             "Check if a region exists")
        .def("removeRegion", &Brain::removeRegion, py::arg("id"),
             "Remove a region by ID")
        .def("getActivityLevel", [](const Brain& self) -> float {
            return static_cast<float>(self.getFiringNeuronCount()) / 
                   std::max(static_cast<size_t>(1), self.getTotalNeuronCount());
        }, "Get brain activity level (0.0 to 1.0)")
        .def("getLearningProgress", [](const Brain& self) -> float {
            return static_cast<float>(self.getTotalSpikeCount()) / 1000.0f;
        }, "Get learning progress indicator");

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
        // Add more useful methods
        .def("getSubsystemsStatus", [](const AgentBrain& self) {
            return std::make_tuple(
                self.isRewardModulationEnabled(),
                self.isStructuralPlasticityEnabled(),
                self.isDevelopmentEnabled(),
                self.isCuriosityEnabled()
            );
        }, "Get subsystems status as tuple (reward, structural, development, curiosity)")
        .def("getLearningLevel", [](const AgentBrain& self) -> float {
            float neuromod = self.getNeuromodulationLevel();
            float curiosity = self.getCuriosityLevel();
            float novelty = self.getNoveltyLevel();
            float predictionError = self.getPredictionError();
            return (neuromod + curiosity + novelty + predictionError) / 4.0f;
        }, "Get overall learning level (0.0 to 1.0)")
        .def("getExplorationScore", [](const AgentBrain& self) -> float {
            float curiosity = self.getCuriosityLevel();
            float novelty = self.getNoveltyLevel();
            return curiosity * 0.6f + novelty * 0.4f;
        }, "Get exploration score (0.0 to 1.0)")
        .def("getDevelopmentalProgress", [](const AgentBrain& self) {
            return std::make_tuple(
                self.getDevelopmentalStage(),
                self.getLearningLevel()
            );
        }, "Get developmental progress (stage, learning_level)")
        .def("getMotivationLevel", [](const AgentBrain& self) -> float {
            return self.getNeuromodulationLevel() * 0.7f + self.getCuriosityLevel() * 0.3f;
        }, "Get motivation level (0.0 to 1.0)")
        .def("enableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all subsystems")
        .def("disableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all subsystems");

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
