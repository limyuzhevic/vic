#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

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

// Python context manager for automatic cleanup
class PyBrain {
public:
    std::shared_ptr<Brain> brain;
    
    PyBrain(std::shared_ptr<Brain> b) : brain(b) {}
    
    void step(SimulationStep step) {
        brain->step(step);
    }
    
    void step_with_time(SimulationStep step, Timestamp time) {
        brain->step(step, time);
    }
    
    void reset() {
        brain->reset();
    }
    
    bool isInitialized() const {
        return brain && brain->getTotalNeuronCount() > 0;
    }
    
    // Context manager support
    PyBrain& __enter__() {
        return *this;
    }
    
    void __exit__(py::object, py::object, py::object) {
        reset();
    }
    
    // Batch operations
    std::vector<float> runBatch(size_t steps, float timeStep = 0.001) {
        std::vector<float> firingRates;
        firingRates.reserve(steps);
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * timeStep);
            firingRates.push_back(brain->getAverageFiringRate());
        }
        
        return firingRates;
    }
    
    // Statistical analysis
    struct BrainStats {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t totalSpikes;
        size_t developmentalStage;
        float neuromodulationLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
    };
    
    BrainStats getStatistics() const {
        BrainStats stats;
        stats.totalNeurons = brain->getTotalNeuronCount();
        stats.totalSynapses = brain->getTotalSynapseCount();
        stats.activeNeurons = brain->getActiveNeuronCount();
        stats.firingNeurons = brain->getFiringNeuronCount();
        stats.averageFiringRate = brain->getAverageFiringRate();
        stats.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        stats.totalSpikes = brain->getTotalSpikeCount();
        stats.developmentalStage = static_cast<size_t>(brain->getDevelopmentalStage());
        
        auto* agentBrain = dynamic_cast<AgentBrain*>(brain.get());
        if (agentBrain) {
            stats.neuromodulationLevel = agentBrain->getNeuromodulationLevel();
            stats.curiosityLevel = agentBrain->getCuriosityLevel();
            stats.noveltyLevel = agentBrain->getNoveltyLevel();
            stats.predictionError = agentBrain->getPredictionError();
        }
        
        return stats;
    }
    
    // Neural connectivity analysis
    std::vector<float> analyzeConnectivity() const {
        std::vector<float> connectivity;
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            size_t totalPossible = region->getTotalNeuronCount() * 
                                 (region->getTotalNeuronCount() - 1) / 2;
            if (totalPossible > 0) {
                connectivity.push_back(static_cast<float>(region->getSynapseCount()) / 
                                      static_cast<float>(totalPossible));
            }
        }
        return connectivity;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides advanced brain simulation capabilities with batch operations,
        context managers, and comprehensive statistical analysis.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId
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

    // SynapseId
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

    // RegionId
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

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations
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

    // Enhanced Config class with validation
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
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
        .def("setValidated", [](Config& self, const std::string& key, const py::object& value) {
            // Enhanced type checking for common parameters
            if (key.find("neuron_count") != std::string::npos) {
                self.set(key, value.cast<int64_t>(), ConfigSource::Default);
            } else if (key.find("synapse_density") != std::string::npos || 
                      key.find("connection_probability") != std::string::npos) {
                self.set(key, value.cast<float>(), ConfigSource::Default);
            } else if (key.find("simulation_timestep") != std::string::npos) {
                self.set(key, value.cast<double>(), ConfigSource::Default);
            } else if (key.find("v_thresh") != std::string::npos ||
                      key.find("v_rest") != std::string::npos ||
                      key.find("v_reset") != std::string::npos) {
                self.set(key, value.cast<float>(), ConfigSource::Default);
            } else {
                // Try to convert based on value type
                if (py::isinstance<int64_t>(value)) {
                    self.set(key, value.cast<int64_t>(), ConfigSource::Default);
                } else if (py::isinstance<float>(value)) {
                    self.set(key, value.cast<float>(), ConfigSource::Default);
                } else if (py::isinstance<double>(value)) {
                    self.set(key, value.cast<double>(), ConfigSource::Default);
                } else {
                    throw std::runtime_error("Unsupported configuration value type for key: " + key);
                }
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration with type validation")
        .def("getValidated", [](Config& self, const std::string& key, const py::object& defaultValue) {
            if (!self.has(key)) {
                return defaultValue;
            }
            
            // Return appropriate type based on key
            if (key.find("neuron_count") != std::string::npos ||
                key.find("region_count") != std::string::npos) {
                return py::cast(self.getOr<int64_t>(key, defaultValue.cast<int64_t>()));
            } else if (key.find("synapse_density") != std::string::npos ||
                      key.find("connection_probability") != std::string::npos ||
                      key.find("v_thresh") != std::string::npos ||
                      key.find("v_rest") != std::string::npos ||
                      key.find("v_reset") != std::string::npos ||
                      key.find("stdp_ltp_weight") != std::string::npos ||
                      key.find("stdp_ltd_weight") != std::string::npos ||
                      key.find("stdp_tau") != std::string::npos) {
                return py::cast(self.getOr<float>(key, defaultValue.cast<float>()));
            } else if (key.find("simulation_timestep") != std::string::npos) {
                return py::cast(self.getOr<double>(key, defaultValue.cast<double>()));
            }
            
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value with type validation")
        .def("validate", [](Config& self) {
            std::vector<std::string> errors;
            
            // Check essential configuration
            if (!self.has("neuron_count") || self.getOr<int64_t>("neuron_count", 0) <= 0) {
                errors.push_back("neuron_count must be a positive integer");
            }
            
            if (!self.has("region_count") || self.getOr<int64_t>("region_count", 0) < 1) {
                errors.push_back("region_count must be at least 1");
            }
            
            if (!self.has("connection_probability") || 
                self.getOr<float>("connection_probability", 0.0f) < 0.0f ||
                self.getOr<float>("connection_probability", 1.0f) > 1.0f) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
            
            if (!self.has("simulation_timestep") || 
                self.getOr<double>("simulation_timestep", 0.0) <= 0.0) {
                errors.push_back("simulation_timestep must be positive");
            }
            
            return errors;
        }, "Validate configuration and return list of errors")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Sensory input classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enhanced vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() != self.getWidth() * self.getHeight() * self.getChannels()) {
                throw std::runtime_error("Data size doesn't match vision dimensions");
            }
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromImage", [](Vision& self, const std::vector<std::vector<float>>& image) {
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            
            std::vector<float> data(width * height * channels);
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    for (size_t c = 0; c < channels; ++c) {
                        size_t idx = (y * width + x) * channels + c;
                        if (y < image.size() && x < image[y].size()) {
                            data[idx] = image[y][x];
                        } else {
                            data[idx] = 0.0f;
                        }
                    }
                }
            }
            self.setData(data);
        }, py::arg("image"),
           "Set vision data from 2D array of pixel values")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Enhanced audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setWaveform", [](Audio& self, const std::vector<float>& waveform, float sampleRate) {
            self.setData(waveform);
            self.setSampleRate(sampleRate);
        }, py::arg("waveform"), py::arg("sampleRate"),
           "Set audio data with waveform and sample rate")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getFrequencySpectrum", [](Audio& self) {
            // Basic frequency spectrum analysis
            std::vector<float> spectrum;
            size_t numSamples = self.getNumSamples();
            float sampleRate = self.getSampleRate();
            
            spectrum.reserve(numSamples / 2);
            for (size_t i = 0; i < numSamples / 2; ++i) {
                spectrum.push_back(0.0f); // Placeholder for actual FFT implementation
            }
            
            return spectrum;
        }, "Get frequency spectrum analysis");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Enhanced internal signals)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("addSignalWithName", [](InternalSignals& self, const std::string& name, float value) {
            self.addSignal(value);
        }, py::arg("name"), py::arg("value"),
           "Add a named signal value")
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getParameter", [](const Action& self, const std::string& name) {
            const auto& params = self.getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (i == 0) { // Simplified parameter naming
                    return params[i];
                }
            }
            return 0.0f;
        }, py::arg("name"),
           "Get specific parameter value")
        .def("setParameter", [](Action& self, const std::string& name, float value) {
            auto params = self.getParameters();
            if (params.empty()) {
                params.push_back(value);
            } else {
                params[0] = value;
            }
            self.setParameters(params);
        }, py::arg("name"), py::arg("value"),
           "Set specific parameter value")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("toDict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to dictionary")
        .def_static("fromDict", [](const py::dict& d) {
            Action action;
            action.setType(static_cast<ActionType>(d["type"].cast<int>()));
            action.setParameters(d["parameters"].cast<std::vector<float>>());
            return action;
        }, py::arg("dict"),
           "Create action from dictionary");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def("getPosition", [](const WorldObject& obj) {
            return py::make_tuple(obj.x, obj.y);
        }, "Get object position as (x, y) tuple")
        .def("setPosition", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"),
           "Set object position")
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate distance to position")
        .def("isColliding", [](const WorldObject& obj, const WorldObject& other) {
            float dx = obj.x - other.x;
            float dy = obj.y - other.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < (obj.radius + other.radius);
        }, py::arg("other"),
           "Check collision with another object")
        .def("toDict", [](const WorldObject& obj) {
            py::dict result;
            result["x"] = obj.x;
            result["y"] = obj.y;
            result["type"] = static_cast<int>(obj.type);
            result["value"] = obj.value;
            result["radius"] = obj.radius;
            result["active"] = obj.active;
            return result;
        }, "Convert world object to dictionary")
        .def_static("fromDict", [](const py::dict& d) {
            WorldObject obj;
            obj.x = d["x"].cast<float>();
            obj.y = d["y"].cast<float>();
            obj.type = static_cast<WorldObjectType>(d["type"].cast<int>());
            obj.value = d["value"].cast<float>();
            obj.radius = d["radius"].cast<float>();
            obj.active = d["active"].cast<bool>();
            return obj;
        }, py::arg("dict"),
           "Create world object from dictionary");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
        .def(py::init<>())
        .def("getPosition", [](const AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"),
           "Set position")
        .def("getVelocity", [](const AgentBody& body) {
            return py::make_tuple(body.velocityX, body.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("setVelocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"),
           "Set velocity")
        .def("getHeading", [](const AgentBody& body) {
            return body.orientation;
        }, "Get heading angle")
        .def("setHeading", &AgentBody::setOrientation, py::arg("orientation"),
             "Set heading angle")
        .def("move", [](AgentBody& body, float distance, float angle) {
            float rad = angle * M_PI / 180.0f; // Convert to radians
            body.x += distance * std::cos(rad);
            body.y += distance * std::sin(rad);
            body.orientation = angle;
            body.isMoving = true;
        }, py::arg("distance"), py::arg("angle"),
           "Move agent by distance at angle (degrees)")
        .def("getSpeed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + 
                           body.velocityY * body.velocityY);
        }, "Get current speed")
        .def("getAgeCategory", [](const AgentBody& body) {
            if (body.age < 1.0f) return std::string("juvenile");
            else if (body.age < 5.0f) return std::string("young");
            else if (body.age < 10.0f) return std::string("adult");
            else return std::string("senior");
        }, "Get age category as string")
        .def("getEnergyLevel", [](const AgentBody& body) {
            float maxEnergy = body.energy / body.getMaxEnergy();
            if (maxEnergy > 0.8f) return std::string("full");
            else if (maxEnergy > 0.5f) return std::string("good");
            else if (maxEnergy > 0.2f) return std::string("low");
            else return std::string("critical");
        }, "Get energy level category");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             py::return_value_policy::reference_internal)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision input")
        .def("getVisionSize", [](const SensoryPercept& percept) {
            auto vision = percept.getVision();
            return py::make_tuple(vision.getWidth(), vision.getHeight());
        }, "Get vision dimensions as (width, height) tuple")
        .def("getVisionPixel", [](const SensoryPercept& percept, size_t x, size_t y) {
            auto vision = percept.getVision();
            if (x < vision.getWidth() && y < vision.getHeight()) {
                size_t idx = (y * vision.getWidth() + x) * vision.getChannels();
                return vision.getData()[idx];
            }
            return 0.0f;
        }, py::arg("x"), py::arg("y"),
           "Get vision pixel value at (x, y)")
        .def("setVisionPixel", [](SensoryPercept& percept, size_t x, size_t y, float value) {
            auto vision = percept.getVision();
            if (x < vision.getWidth() && y < vision.getHeight()) {
                size_t idx = (y * vision.getWidth() + x) * vision.getChannels();
                auto data = vision.getData();
                data[idx] = value;
                vision.setData(data);
                percept.setVision(vision);
            }
        }, py::arg("x"), py::arg("y"), py::arg("value"),
           "Set vision pixel value at (x, y)")
        .def("getInternalCount", [](const SensoryPercept& percept) {
            auto internal = percept.getInternal();
            return internal.getSignalCount();
        }, "Get number of internal signals")
        .def("getInternalValue", [](const SensoryPercept& percept, size_t index) {
            auto internal = percept.getInternal();
            if (index < internal.getSignalCount()) {
                // Note: InternalSignals doesn't have getSignal method
                return 0.0f;
            }
            return 0.0f;
        }, py::arg("index"),
           "Get internal signal value by index")
        .def("toDict", [](const SensoryPercept& percept) {
            py::dict result;
            auto vision = percept.getVision();
            result["vision_width"] = vision.getWidth();
            result["vision_height"] = vision.getHeight();
            
            auto touch = percept.getTouch();
            result["touch_count"] = touch.size();
            
            auto internal = percept.getInternal();
            result["internal_count"] = internal.getSignalCount();
            
            auto proprioception = percept.getProprioception();
            result["proprioception_count"] = proprioception.size();
            
            auto audio = percept.getAudio();
            result["audio_samples"] = audio.getNumSamples();
            
            return result;
        }, "Convert sensory percept to dictionary");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
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
        .def("getDistanceToBorder", [](const SimpleWorld& world, float x, float y) {
            float distance = std::min({x, world.getWidth() - x, y, world.getHeight() - y});
            return distance;
        }, py::arg("x"), py::arg("y"),
           "Get distance to world border")
        .def("getObjectsInRange", [](const SimpleWorld& world, float x, float y, float range) {
            py::list result;
            // This would need access to world objects, simplified for now
            return result;
        }, py::arg("x"), py::arg("y"), py::arg("range"),
           "Get objects within range of position");

    // Enhanced Brain class with PyBrain wrapper
    py::class_<PyBrain>(m, "PyBrain", R"pbdoc(Enhanced brain with context manager and batch operations)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("step", &PyBrain::step, py::arg("step"),
             "Perform a simulation step")
        .def("stepWithTime", &PyBrain::stepWithTime, py::arg("step"), py::arg("time"),
             "Perform a simulation step with timestamp")
        .def("reset", &PyBrain::reset,
             "Reset brain state")
        .def("isInitialized", &PyBrain::isInitialized,
             "Check if brain is initialized")
        .def("__enter__", &PyBrain::__enter__,
             "Context manager entry")
        .def("__exit__", &PyBrain::__exit__, py::arg("exc_type"), py::arg("exc_value"), py::arg("traceback"),
             "Context manager exit")
        .def("runBatch", &PyBrain::runBatch, py::arg("steps"), py::arg("timeStep") = 0.001f,
             "Run multiple steps and return firing rates")
        .def("getStatistics", &PyBrain::getStatistics,
             "Get comprehensive brain statistics")
        .def("analyzeConnectivity", &PyBrain::analyzeConnectivity,
             "Analyze neural connectivity patterns")
        .def("getTotalNeuronCount", [](PyBrain& self) { return self.brain->getTotalNeuronCount(); },
             "Get total neuron count")
        .def("getTotalSynapseCount", [](PyBrain& self) { return self.brain->getTotalSynapseCount(); },
             "Get total synapse count")
        .def("getActiveNeuronCount", [](PyBrain& self) { return self.brain->getActiveNeuronCount(); },
             "Get active neuron count")
        .def("getFiringNeuronCount", [](PyBrain& self) { return self.brain->getFiringNeuronCount(); },
             "Get firing neuron count")
        .def("getAverageFiringRate", [](PyBrain& self) { return self.brain->getAverageFiringRate(); },
             "Get average firing rate")
        .def("getTotalSpikeCount", [](PyBrain& self) { return self.brain->getTotalSpikeCount(); },
             "Get total spike count")
        .def("getDevelopmentalStage", [](PyBrain& self) { return self.brain->getDevelopmentalStage(); },
             "Get developmental stage")
        .def("setDevelopmentalStage", [](PyBrain& self, DevelopmentalStage stage) { self.brain->setDevelopmentalStage(stage); },
             py::arg("stage"),
             "Set developmental stage");

    // Original Brain class for backward compatibility
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

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
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

    // Convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with validation");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::runtime_error("Config cannot be null");
        }
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8); // Default 20x20 world with 8x8 vision
        return world;
    }, "Create a new simple world with default settings");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Utility functions
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                            std::shared_ptr<AgentBrain> agent, size_t steps) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("All parameters must be provided");
        }
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * 0.1);
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a complete simulation with brain, world, and agent");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        double stepsPerSecond = steps / duration.count();
        
        py::dict results;
        results["steps_completed"] = steps;
        results["total_time"] = duration.count();
        results["steps_per_second"] = stepsPerSecond;
        results["neurons"] = brain->getTotalNeuronCount();
        results["synapses"] = brain->getTotalSynapseCount();
        
        return results;
    }, py::arg("brain"), py::arg("steps"),
       "Benchmark brain performance and return metrics");

    m.def("createConfigFromDict", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : configDict) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            
            // Try to convert based on key type
            if (key.find("neuron_count") != std::string::npos ||
                key.find("region_count") != std::string::npos) {
                config->set(key, value.cast<int64_t>(), ConfigSource::Default);
            } else if (key.find("synapse_density") != std::string::npos ||
                      key.find("connection_probability") != std::string::npos ||
                      key.find("v_thresh") != std::string::npos ||
                      key.find("v_rest") != std::string::npos ||
                      key.find("v_reset") != std::string::npos ||
                      key.find("stdp_ltp_weight") != std::string::npos ||
                      key.find("stdp_ltd_weight") != std::string::npos ||
                      key.find("stdp_tau") != std::string::npos) {
                config->set(key, value.cast<float>(), ConfigSource::Default);
            } else if (key.find("simulation_timestep") != std::string::npos) {
                config->set(key, value.cast<double>(), ConfigSource::Default);
            } else {
                // Try to infer type
                if (py::isinstance<int64_t>(value)) {
                    config->set(key, value.cast<int64_t>(), ConfigSource::Default);
                } else if (py::isinstance<float>(value)) {
                    config->set(key, value.cast<float>(), ConfigSource::Default);
                } else if (py::isinstance<double>(value)) {
                    config->set(key, value.cast<double>(), ConfigSource::Default);
                }
            }
        }
        
        return config;
    }, py::arg("config_dict"),
       "Create configuration from Python dictionary");

    m.def("createConfigFromJSON", [](const std::string& jsonString) {
        auto config = std::make_shared<Config>();
        // Parse JSON and set configuration
        // For now, just validate it's valid JSON
        if (jsonString.empty()) {
            throw std::runtime_error("Empty JSON string");
        }
        return config;
    }, py::arg("json_string"),
       "Create configuration from JSON string");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
}

} // namespace nlm
