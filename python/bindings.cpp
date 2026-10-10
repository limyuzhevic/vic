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
        
        // Advanced batch operations
        .def("setBatch", [](Config& self, const py::dict& values) {
            for (auto item : values) {
                std::string key = py::cast<std::string>(item.first);
                py::object value_obj = item.second;
                
                // Determine type and call appropriate setter
                if (value_obj.is_none()) {
                    self.remove(key);
                } else if (value_obj.is_true()) {
                    self.set(key, true, ConfigSource::Runtime);
                } else if (value_obj.is_false()) {
                    self.set(key, false, ConfigSource::Runtime);
                } else if (py::isinstance<py::int_>(value_obj)) {
                    int val = py::cast<int>(value_obj);
                    self.set(key, val, ConfigSource::Runtime);
                } else if (py::isinstance<py::float_>(value_obj)) {
                    double val = py::cast<double>(value_obj);
                    self.set(key, val, ConfigSource::Runtime);
                } else {
                    std::string val = py::cast<std::string>(value_obj);
                    self.set(key, val, ConfigSource::Runtime);
                }
            }
        }, py::arg("values"), "Set multiple configuration values from a dictionary")
        
        .def("getAll", [](Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                // Try different types
                auto int_val = self.get<int>(key);
                if (int_val.has_value()) {
                    result[py::cast(key)] = py::cast(int_val.value());
                    continue;
                }
                auto double_val = self.get<double>(key);
                if (double_val.has_value()) {
                    result[py::cast(key)] = py::cast(double_val.value());
                    continue;
                }
                auto bool_val = self.get<bool>(key);
                if (bool_val.has_value()) {
                    result[py::cast(key)] = py::cast(bool_val.value());
                    continue;
                }
                auto string_val = self.get<std::string>(key);
                if (string_val.has_value()) {
                    result[py::cast(key)] = py::cast(string_val.value());
                    continue;
                }
                // If we can't determine type, use string representation
                result[py::cast(key)] = "UNKNOWN_TYPE";
            }
            return result;
        }, "Get all configuration values as a dictionary")
        
        .def("validateKeys", [](Config& self, const py::list& required_keys) {
            std::vector<std::string> missing_keys;
            for (auto item : required_keys) {
                std::string key = py::cast<std::string>(item);
                if (!self.has(key)) {
                    missing_keys.push_back(key);
                }
            }
            if (missing_keys.empty()) {
                return py::dict();
            }
            py::dict result;
            result["success"] = false;
            result["missing_keys"] = missing_keys;
            return result;
        }, py::arg("required_keys"), "Validate that all required keys exist, returns dict with success and missing keys")
        
        .def("getAsDict", [](Config& self, const py::list& keys) {
            py::dict result;
            for (auto item : keys) {
                std::string key = py::cast<std::string>(item);
                
                // Try different types
                auto int_val = self.get<int>(key);
                if (int_val.has_value()) {
                    result[py::cast(key)] = py::cast(int_val.value());
                    continue;
                }
                auto double_val = self.get<double>(key);
                if (double_val.has_value()) {
                    result[py::cast(key)] = py::cast(double_val.value());
                    continue;
                }
                auto bool_val = self.get<bool>(key);
                if (bool_val.has_value()) {
                    result[py::cast(key)] = py::cast(bool_val.value());
                    continue;
                }
                auto string_val = self.get<std::string>(key);
                if (string_val.has_value()) {
                    result[py::cast(key)] = py::cast(string_val.value());
                    continue;
                }
            }
            return result;
        }, py::arg("keys"), "Get specified keys as a dictionary with automatic type conversion")
        
        .def("setSource", [](Config& self, const py::dict& values, ConfigSource source) {
            for (auto item : values) {
                std::string key = py::cast<std::string>(item.first);
                py::object value_obj = item.second;
                
                if (value_obj.is_none()) {
                    self.remove(key);
                } else if (value_obj.is_true()) {
                    self.set(key, true, source);
                } else if (value_obj.is_false()) {
                    self.set(key, false, source);
                } else if (py::isinstance<py::int_>(value_obj)) {
                    int val = py::cast<int>(value_obj);
                    self.set(key, val, source);
                } else if (py::isinstance<py::float_>(value_obj)) {
                    double val = py::cast<double>(value_obj);
                    self.set(key, val, source);
                } else {
                    std::string val = py::cast<std::string>(value_obj);
                    self.set(key, val, source);
                }
            }
        }, py::arg("values"), py::arg("source"), "Set multiple configuration values with a specific source")
        
        .def("compareWithDefaults", [](Config& self, const Config& defaults) {
            py::dict changes;
            for (const auto& key : self.getKeys()) {
                auto self_val = self.get<std::string>(key);
                auto default_val = defaults.get<std::string>(key);
                
                if (self_val.has_value() && default_val.has_value()) {
                    if (self_val.value() != default_val.value()) {
                        changes[py::cast(key)] = py::make_tuple(
                            py::cast(self_val.value()),
                            py::cast(default_val.value())
                        );
                    }
                }
            }
            return changes;
        }, py::arg("defaults"), "Compare current config with default config, returns dict of changed values")
        
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

        .def("enablePerformanceMonitoring", [](Brain& self, bool enable) {
            // Set a performance monitoring flag in the brain config
            if (auto config = self.getConfig()) {
                config->set("performance_monitoring", enable, ConfigSource::Runtime);
            }
        }, py::arg("enable"), "Enable or disable performance monitoring")
        
        .def("enableDebugLogging", [](Brain& self, bool enable, std::string level) {
            if (auto config = self.getConfig()) {
                config->set("debug_level", level, ConfigSource::Runtime);
            }
        }, py::arg("enable"), py::arg("level"), "Enable debug logging with specified level")
        
        .def("benchmarkStep", [](Brain& self, SimulationStep steps, bool reset) {
            if (reset) {
                self.reset();
                self.initialize();
            }
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (SimulationStep step = 0; step < steps; ++step) {
                self.step(step);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            
            py::dict results;
            results["steps"] = steps;
            results["duration_us"] = duration.count();
            results["avg_time_per_step_us"] = duration.count() / steps;
            results["spikes_per_step"] = static_cast<double>(self.getTotalSpikeCount()) / steps;
            results["neurons_firing"] = self.getFiringNeuronCount();
            results["firing_rate_hz"] = (duration.count() / 1000000.0) > 0 ? 
                static_cast<double>(self.getTotalSpikeCount()) / (duration.count() / 1000000.0) : 0.0;
            
            return results;
        }, py::arg("steps"), py::arg("reset") = true, "Benchmark brain performance over a number of steps")
        
        .def("getPerformanceStats", [](Brain& self) {
            py::dict stats;
            stats["total_neuron_count"] = self.getTotalNeuronCount();
            stats["total_synapse_count"] = self.getTotalSynapseCount();
            stats["total_spike_count"] = self.getTotalSpikeCount();
            stats["firing_neuron_count"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["pending_spike_events"] = self.getPendingSpikeEventCount();
            stats["region_count"] = self.getRegionCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            
            if (auto config = self.getConfig()) {
                py::dict config_stats;
                config_stats["keys"] = config->getKeys();
                stats["config"] = config_stats;
            }
            
            return stats;
        }, "Get comprehensive performance and system statistics")
        
        .def("validateBrainState", [](Brain& self) {
            py::dict validation_results;
            validation_results["valid"] = true;
            validation_results["errors"] = py::list();
            validation_results["warnings"] = py::list();
            
            // Check basic consistency
            if (self.getTotalNeuronCount() == 0) {
                validation_results["valid"] = false;
                validation_results["errors"].attr("append")("Brain has no neurons");
            }
            
            if (self.getTotalSynapseCount() < 0) {
                validation_results["warnings"].attr("append")("Synapse count is negative");
            }
            
            if (self.getAverageFiringRate() > 1000.0f) {
                validation_results["warnings"].attr("append")("Very high firing rate detected");
            }
            
            // Check region consistency
            if (self.getRegionCount() == 0) {
                validation_results["warnings"].attr("append")("No regions found");
            }
            
            return validation_results;
        }, "Validate brain state consistency and return validation results")
        
    // Checkpoint and restart capabilities
    .def("createCheckpoint", [](Brain& self, const std::string& label = "") {
        std::string filepath = "checkpoint_" + label + "_" + 
                               std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + 
                               ".bin";
        bool success = self.save(filepath);
        if (success) {
            py::dict result;
            result["success"] = true;
            result["filepath"] = filepath;
            result["label"] = label;
            return result;
        } else {
            py::dict result;
            result["success"] = false;
            result["filepath"] = filepath;
            result["label"] = label;
            return result;
        }
    }, py::arg("label") = "", "Create a checkpoint with optional label, returns dict with success and filepath")
    
    .def("restoreCheckpoint", [](Brain& self, const std::string& filepath, bool resetFirst = true) {
        if (resetFirst) {
            self.reset();
            self.initialize();
        }
        bool success = self.load(filepath);
        py::dict result;
        result["success"] = success;
        result["filepath"] = filepath;
        result["was_reset"] = resetFirst;
        return result;
    }, py::arg("filepath"), py::arg("resetFirst") = true, "Restore a checkpoint, optionally resetting first")
    
    .def("listCheckpoints", [](const std::string& directory = "./checkpoints") {
        std::vector<std::string> checkpoints;
        
        try {
            if (std::filesystem::exists(directory)) {
                for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                        checkpoints.push_back(entry.path().string());
                    }
                }
            }
        } catch (const std::exception& e) {
            // Return empty list if directory doesn't exist or can't be read
        }
        
        return checkpoints;
    }, py::arg("directory") = "./checkpoints", "List available checkpoint files in directory")
    
    .def("checkpointInfo", [](Brain& self, const std::string& filepath) {
        // Simple checkpoint info extraction (placeholder for more detailed info)
        py::dict info;
        
        std::ifstream file(filepath, std::ios::binary);
        if (file.is_open()) {
            // Basic file info
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            
            info["filepath"] = filepath;
            info["file_size_bytes"] = static_cast<size_t>(fileSize);
            info["readable"] = true;
            
            // Try to load and get brain info
            Brain tempBrain(self.getConfig());
            if (tempBrain.load(filepath)) {
                info["brain_size"] = tempBrain.getTotalNeuronCount();
                info["synapses"] = tempBrain.getTotalSynapseCount();
                info["regions"] = tempBrain.getRegionCount();
            } else {
                info["brain_size"] = 0;
                info["synapses"] = 0;
                info["regions"] = 0;
            }
        } else {
            info["filepath"] = filepath;
            info["file_size_bytes"] = 0;
            info["readable"] = false;
            info["brain_size"] = 0;
            info["synapses"] = 0;
            info["regions"] = 0;
        }
        
        return info;
    }, py::arg("filepath"), "Get information about a checkpoint file")
    
    .def("compareCheckpoints", [](const std::string& filepath1, const std::string& filepath2) {
        py::dict comparison;
        
        // Basic file comparison
        comparison["file1_exists"] = std::filesystem::exists(filepath1);
        comparison["file2_exists"] = std::filesystem::exists(filepath2);
        
        if (!comparison["file1_exists"].cast<bool>() || !comparison["file2_exists"].cast<bool>()) {
            comparison["comparable"] = false;
            comparison["error"] = "One or both files do not exist";
            return comparison;
        }
        
        // Compare file sizes
        auto size1 = std::filesystem::file_size(filepath1);
        auto size2 = std::filesystem::file_size(filepath2);
        comparison["file1_size"] = static_cast<size_t>(size1);
        comparison["file2_size"] = static_cast<size_t>(size2);
        comparison["size_difference"] = std::abs(size1 - size2);
        
        comparison["comparable"] = true;
        comparison["recommendation"] = "Load one checkpoint and compare with the other";
        
        return comparison;
    }, py::arg("filepath1"), py::arg("filepath2"), "Compare two checkpoint files")
    
    .def("cleanupOldCheckpoints", [](const std::string& directory, size_t maxFiles = 10) {
        py::dict result;
        
        if (!std::filesystem::exists(directory)) {
            result["success"] = false;
            result["message"] = "Directory does not exist: " + directory;
            return result;
        }
        
        std::vector<std::filesystem::path> checkpoint_files;
        std::vector<std::filesystem::file_time_type> file_times;
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                checkpoint_files.push_back(entry.path());
                file_times.push_back(entry.last_write_time());
            }
        }
        
        if (checkpoint_files.size() <= maxFiles) {
            result["success"] = true;
            result["message"] = "No cleanup needed, " + std::to_string(checkpoint_files.size()) + " files <= " + std::to_string(maxFiles);
            return result;
        }
        
        // Sort by modification time (oldest first)
        std::vector<size_t> indices(checkpoint_files.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(), [&](size_t i, size_t j) {
            return file_times[i] < file_times[j];
        });
        
        size_t to_delete = checkpoint_files.size() - maxFiles;
        size_t deleted = 0;
        
        for (size_t i = 0; i < to_delete && i < indices.size(); ++i) {
            try {
                std::filesystem::remove(checkpoint_files[indices[i]]);
                deleted++;
            } catch (const std::exception& e) {
                // Continue with other files
            }
        }
        
        result["success"] = true;
        result["message"] = "Cleaned up " + std::to_string(deleted) + " old checkpoints, keeping " + 
                           std::to_string(checkpoint_files.size() - to_delete) + " most recent";
        result["deleted"] = static_cast<size_t>(deleted);
        result["remaining"] = static_cast<size_t>(checkpoint_files.size() - to_delete);
        
        return result;
    }, py::arg("directory") = "./checkpoints", py::arg("maxFiles") = 10, "Clean up old checkpoint files, keeping only the most recent ones")
    
    .def("autoCheckpoint", [](Brain& self, const std::string& directory, size_t interval) {
        py::dict result;
        
        // This is a placeholder for automatic checkpointing logic
        // In a real implementation, this would:
        // 1. Check if interval has elapsed since last checkpoint
        // 2. If so, create a checkpoint in the specified directory
        // 3. Track checkpoint metadata
        
        if (!std::filesystem::exists(directory)) {
            std::filesystem::create_directories(directory);
        }
        
        // For now, just create a simple checkpoint
        std::string filename = "auto_checkpoint_" + 
                               std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + 
                               ".bin";
        std::string filepath = directory + "/" + filename;
        
        bool success = self.save(filepath);
        
        result["success"] = success;
        result["filepath"] = filepath;
        result["interval"] = interval;
        result["note"] = "Note: This is a simple implementation. Full auto-checkpointing requires additional state management.";
        
        return result;
    }, py::arg("directory") = "./checkpoints", py::arg("interval") = 1000, "Create automatic checkpoint at specified step intervals")

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
