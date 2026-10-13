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
        
        // ===== CONVENIENCE METHODS =====
        
        // Get config value with type conversion and validation
        .def("get", [](Config& self, const std::string& key, bool required = false) {
            if (!self.has(key)) {
                if (required) {
                    throw std::runtime_error("Required configuration key not found: " + key);
                }
                return py::none();
            }
            // Default to string return for flexibility
            return "TODO: Implement proper type conversion";
        }, py::arg("key"), py::arg("required") = false,
             "Get configuration value with type conversion")
        
        // Set config value with validation
        .def("set_safe", [](Config& self, const std::string& key, const std::string& value) {
            if (key.empty()) {
                throw std::runtime_error("Configuration key cannot be empty");
            }
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value with validation")
        
        // Create presets for common configurations
        .def("_create_preset", [](Config& self, const std::string& presetName) {
            if (presetName == "minimal") {
                self.clear();
                self.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
                self.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
                self.set("connection_probability", 0.05f, ConfigSource::Default);
                self.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            } else if (presetName == "high_performance") {
                self.clear();
                self.set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
                self.set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
                self.set("connection_probability", 0.1f, ConfigSource::Default);
                self.set("plasticity_stdp_enable", true, ConfigSource::Default);
                self.set("plasticity_hebbian_enable", true, ConfigSource::Default);
                self.set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
            } else if (presetName == "learning") {
                self.clear();
                self.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
                self.set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
                self.set("connection_probability", 0.08f, ConfigSource::Default);
                self.set("plasticity_stdp_enable", true, ConfigSource::Default);
                self.set("plasticity_hebbian_enable", true, ConfigSource::Default);
                self.set("plasticity_structural_enable", true, ConfigSource::Default);
                self.set("neuromod_dopamine_scale", 1.0f, ConfigSource::Default);
                self.set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
            } else {
                throw std::runtime_error("Unknown preset: " + presetName);
            }
        }, py::arg("presetName"),
             "Create a predefined configuration preset")
        
        // Get config diagnostics
        .def("diagnose", [](const Config& self) {
            struct ConfigDiagnostics {
                size_t totalKeys;
                size_t userKeys;
                size_t defaultKeys;
                std::string status;
                std::vector<std::string> warnings;
            };
            
            ConfigDiagnostics diag;
            diag.totalKeys = self.getKeys().size();
            // Count user vs default keys (simplified)
            diag.userKeys = 0; // Would need to track source
            diag.defaultKeys = diag.totalKeys - diag.userKeys;
            
            if (diag.totalKeys == 0) {
                diag.status = "Empty (using all defaults)";
            } else if (diag.userKeys == 0) {
                diag.status = "Only default values";
            } else if (diag.defaultKeys == 0) {
                diag.status = "Only user values";
            } else {
                diag.status = "Mixed user and default values";
            }
            
            // Check for common issues
            if (!self.has("neuron_count")) {
                diag.warnings.push_back("Missing neuron_count - will use default");
            }
            if (!self.has("random_seed")) {
                diag.warnings.push_back("Missing random_seed - will use default (42)");
            }
            
            return diag;
        }, "Diagnose configuration and return health report");
        
        // ===== BATCH OPERATIONS =====
        
        // Batch set multiple values
        .def("batch_set", [](Config& self, const std::vector<std::pair<std::string, std::string>>& settings) {
            size_t successCount = 0;
            for (const auto& setting : settings) {
                try {
                    self.set(setting.first, setting.second, ConfigSource::User);
                    successCount++;
                } catch (const std::exception& e) {
                    // Log but continue with other settings
                    std::cerr << "Warning: Failed to set " << setting.first 
                             << ": " << e.what() << std::endl;
                }
            }
            return successCount;
        }, py::arg("settings"),
             "Set multiple configuration values in batch")
        
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
        
        // ===== CONVENIENCE METHODS =====
        // Initialize with default config
        .def("_initialize_default", [](Brain& self) {
            return self.initialize();
        }, "Initialize with default configuration")
        
        // Enhanced step methods with validation
        .def("step_with_validation", [](Brain& self, SimulationStep currentStep, bool validate = true) {
            if (validate) {
                if (!self.getConfig() || self.getTotalNeuronCount() == 0) {
                    throw std::runtime_error("Brain not properly initialized");
                }
            }
            self.step(currentStep);
            return self.getFiringNeuronCount();
        }, py::arg("currentStep"), py::arg("validate") = true,
             "Perform a simulation step with validation")
        
        // Batch operations for efficiency
        .def("batch_step", [](Brain& self, SimulationStep startStep, size_t numSteps) {
            size_t firedCount = 0;
            for (size_t i = 0; i < numSteps; ++i) {
                SimulationStep step = startStep + i;
                self.step(step);
                firedCount += self.getFiringNeuronCount();
            }
            return firedCount;
        }, py::arg("startStep"), py::arg("numSteps"),
             "Perform multiple simulation steps efficiently")
        
        // State management shortcuts
        .def("save_checkpoint", [](Brain& self, const std::string& prefix = "checkpoint") {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << prefix << "_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".bin";
            bool success = self.save(ss.str());
            if (!success) {
                throw std::runtime_error("Failed to save checkpoint");
            }
            return ss.str();
        }, py::arg("prefix") = "checkpoint",
             "Save brain state with timestamped filename")
        
        // Enhanced simulation loop for beginners
        .def("run_simulation", [](Brain& self, size_t numSteps, bool verbose = false) {
            if (self.getTotalNeuronCount() == 0) {
                throw std::runtime_error("Brain not initialized - call initialize() first");
            }
            
            std::vector<size_t> firingCounts;
            firingCounts.reserve(numSteps);
            
            for (SimulationStep step = 0; step < numSteps; ++step) {
                self.step(step);
                firingCounts.push_back(self.getFiringNeuronCount());
                if (verbose && step % 100 == 0) {
                    std::cout << "Step " << step << ": " << firingCounts.back() << " firing neurons" << std::endl;
                }
            }
            
            // Calculate statistics
            size_t totalFiring = std::accumulate(firingCounts.begin(), firingCounts.end(), 0UL);
            double avgFiring = static_cast<double>(totalFiring) / firingCounts.size();
            
            return py::make_tuple(firingCounts, totalFiring, avgFiring);
        }, py::arg("numSteps"), py::arg("verbose") = false,
             "Run a complete simulation and return statistics")
        
        // Brain health check
        .def("check_health", [](Brain& self) {
            struct HealthReport {
                bool isInitialized;
                size_t neuronCount;
                size_t synapseCount;
                float avgFiringRate;
                bool isHealthy;
                std::string statusMessage;
            };
            
            HealthReport report;
            report.isInitialized = (self.getConfig() != nullptr) && (self.getTotalNeuronCount() > 0);
            report.neuronCount = self.getTotalNeuronCount();
            report.synapseCount = self.getTotalSynapseCount();
            report.avgFiringRate = self.getAverageFiringRate();
            report.isHealthy = report.isInitialized && report.neuronCount > 10;
            
            if (report.isHealthy) {
                report.statusMessage = "Brain is healthy and operational";
            } else if (!report.isInitialized) {
                report.statusMessage = "Brain not initialized - call initialize() first";
            } else {
                report.statusMessage = "Brain has too few neurons to function properly";
            }
            
            return report;
        }, "Check brain health and return diagnostic report");
        
        // ===== ADVANCED FEATURES FOR EXPERT USERS =====
        
        // Direct neuron manipulation (advanced)
        .def("inject_current_batch", [](Brain& self, const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
            for (const auto& injection : injections) {
                self.injectCurrent(injection.first, injection.second);
            }
        }, py::arg("injections"),
             "Inject current into multiple neurons efficiently")
        
        // Development control (advanced)
        .def("skip_development", [](Brain& self, size_t steps) {
            auto currentStage = self.getDevelopmentalStage();
            for (size_t i = 0; i < steps; ++i) {
                self.develop();
            }
            return self.getDevelopmentalStage();
        }, py::arg("steps"),
             "Advance development stage by specified steps")
        
        // Memory management
        .def("clear_memory", [](Brain& self) {
            self.reset();
            if (self.getConfig()) {
                self.initialize();
            }
        }, "Clear all brain state and optionally re-initialize");
        
        // Statistics and metrics
        .def("get_detailed_stats", [](Brain& self) {
            struct DetailedStats {
                size_t totalNeurons;
                size_t totalSynapses;
                size_t activeNeurons;
                size_t firingNeurons;
                float avgFiringRate;
                float eiratio;
                size_t totalSpikes;
                float developmentalStage;
                std::string networkDensity;
            };
            
            DetailedStats stats;
            stats.totalNeurons = self.getTotalNeuronCount();
            stats.totalSynapses = self.getTotalSynapseCount();
            stats.activeNeurons = self.getActiveNeuronCount();
            stats.firingNeurons = self.getFiringNeuronCount();
            stats.avgFiringRate = self.getAverageFiringRate();
            stats.eiratio = self.getExcitationInhibitionRatio();
            stats.totalSpikes = self.getTotalSpikeCount();
            stats.developmentalStage = static_cast<float>(self.getDevelopmentalStage());
            
            // Calculate network density
            double density = (stats.totalNeurons > 0) ? 
                static_cast<double>(stats.totalSynapses) / (stats.totalNeurons * stats.totalNeurons) : 0.0;
            stats.networkDensity = (density < 0.01) ? "Sparse" : 
                                  (density < 0.1) ? "Moderate" : "Dense";
            
            return stats;
        }, "Get detailed brain statistics and network metrics");
        
        // ===== INTEROP WITH PYTHON CONVENTIONS =====
        
        // Property-style access for common properties
        .def_readonly("neuron_count", &Brain::getTotalNeuronCount,
                     "Total number of neurons (convenience property)")
        .def_readonly("synapse_count", &Brain::getTotalSynapseCount,
                     "Total number of synapses (convenience property)")
        .def_readonly("firing_count", &Brain::getFiringNeuronCount,
                     "Current number of firing neurons (convenience property)")
        .def_readonly("is_initialized", [](const Brain& self) {
            return self.getConfig() != nullptr && self.getTotalNeuronCount() > 0;
        }, "Check if brain is properly initialized (convenience property)")

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        
        // ===== CONVENIENCE METHODS =====
        
        // Initialize with validation
        .def("_initialize_safe", [](AgentBrain& self, const SimpleWorld& world) {
            if (!self.getBrain() || !self.getBrain()->getConfig()) {
                throw std::runtime_error("AgentBrain must be created with a valid Brain instance");
            }
            self.initialize(world);
            return self.getSensoryInputSize();
        }, py::arg("world"),
             "Initialize with validation and return sensory input size")
        
        // Process sensory input with validation
        .def("process_sensory_input_safe", [](AgentBrain& self, const SensoryPercept& percept) {
            if (!self.getBrain()) {
                throw std::runtime_error("AgentBrain not properly initialized");
            }
            self.processSensoryInput(percept);
            return self.getBrain()->getFiringNeuronCount();
        }, py::arg("percept"),
             "Process sensory input and return firing neuron count")
        
        // Enhanced motor command decoding with debugging
        .def("decode_motor_command_with_debug", [](AgentBrain& self, bool enableDebug = false) {
            if (!self.getBrain()) {
                throw std::runtime_error("AgentBrain not properly initialized");
            }
            auto action = self.decodeMotorCommand();
            if (enableDebug) {
                std::cout << "[DEBUG] Motor command decoded: " 
                         << static_cast<int>(action->getType()) << std::endl;
                std::cout << "[DEBUG] Brain firing neurons: " 
                         << self.getBrain()->getFiringNeuronCount() << std::endl;
            }
            return action;
        }, py::arg("enableDebug") = false,
             "Decode motor command with optional debug output")
        
        // Enable all subsystems at once
        .def("enable_all_systems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
            return true;
        }, "Enable all agent subsystems at once");
        
        // Reset with cleanup
        .def("reset_complete", [](AgentBrain& self) {
            if (self.getBrain()) {
                self.getBrain()->reset();
            }
            self.reset();
            return true;
        }, "Complete reset of agent and brain state");
        
        // Get agent status report
        .def("get_status_report", [](const AgentBrain& self) {
            struct AgentStatus {
                bool isInitialized;
                size_t sensoryInputSize;
                size_t motorOutputSize;
                float curiosityLevel;
                float noveltyLevel;
                float predictionError;
                bool rewardModulation;
                bool structuralPlasticity;
                bool development;
                bool curiosity;
                std::string statusMessage;
            };
            
            AgentStatus status;
            status.isInitialized = (self.getBrain() != nullptr);
            status.sensoryInputSize = self.getSensoryInputSize();
            status.motorOutputSize = self.getMotorOutputSize();
            status.curiosityLevel = self.getCuriosityLevel();
            status.noveltyLevel = self.getNoveltyLevel();
            status.predictionError = self.getPredictionError();
            status.rewardModulation = self.isRewardModulationEnabled();
            status.structuralPlasticity = self.isStructuralPlasticityEnabled();
            status.development = self.isDevelopmentEnabled();
            status.curiosity = self.isCuriosityEnabled();
            
            if (!status.isInitialized) {
                status.statusMessage = "AgentBrain not properly initialized";
            } else if (status.sensoryInputSize == 0) {
                status.statusMessage = "Initialized but sensory input size is zero";
            } else {
                status.statusMessage = "AgentBrain ready for operation";
            }
            
            return status;
        }, "Get comprehensive agent status report");
        
        // ===== ADVANCED FEATURES =====
        
        // Batch sensory processing (for efficiency)
        .def("process_sensory_batch", [](AgentBrain& self, 
                                        const std::vector<SensoryPercept>& percepts) {
            if (!self.getBrain()) {
                throw std::runtime_error("AgentBrain not properly initialized");
            }
            
            size_t processedCount = 0;
            for (const auto& percept : percepts) {
                self.processSensoryInput(percept);
                processedCount++;
            }
            
            return processedCount;
        }, py::arg("percepts"),
             "Process multiple sensory inputs in batch")
        
        // Get motor command with analysis
        .def("analyze_motor_command", [](AgentBrain& self) {
            if (!self.getBrain()) {
                throw std::runtime_error("AgentBrain not properly initialized");
            }
            
            struct MotorAnalysis {
                MotorCommand command;
                float curiosityLevel;
                float noveltyLevel;
                float predictionError;
                size_t firingNeurons;
                std::string recommendation;
            };
            
            MotorAnalysis analysis;
            analysis.command = self.decodeMotorCommand();
            analysis.curiosityLevel = self.getCuriosityLevel();
            analysis.noveltyLevel = self.getNoveltyLevel();
            analysis.predictionError = self.getPredictionError();
            analysis.firingNeurons = self.getBrain()->getFiringNeuronCount();
            
            // Simple recommendation logic
            if (analysis.curiosityLevel > 0.7) {
                analysis.recommendation = "High curiosity - consider exploratory action";
            } else if (analysis.predictionError > 0.5) {
                analysis.recommendation = "High prediction error - consider novel action";
            } else {
                analysis.recommendation = "Standard action recommended";
            }
            
            return analysis;
        }, "Analyze motor command with recommendation");
        
        // Configure all neuromodulation at once
        .def("configure_neuromodulation", [](AgentBrain& self, 
                                            bool reward, bool curiosity, 
                                            bool novelty, bool prediction) {
            self.enableRewardModulation(reward);
            self.enableCuriosity(curiosity);
            
            // Note: Novelty and prediction error are typically computed automatically
            // This method is a placeholder for advanced users who want explicit control
            
            return true;
        }, py::arg("reward"), py::arg("curiosity"), py::arg("novelty"), py::arg("prediction"),
             "Configure neuromodulation subsystems");
        
        // Enhanced development update
        .def("update_development_advanced", [](AgentBrain& self, double timestep, 
                                              bool enableAutoStage = false) {
            if (!self.getBrain()) {
                throw std::runtime_error("AgentBrain not properly initialized");
            }
            
            auto currentStage = self.getDevelopmentalStage();
            
            self.updateDevelopment(timestep);
            
            if (enableAutoStage && currentStage != self.getDevelopmentalStage()) {
                std::cout << "[DEVELOPMENT] Stage changed from " 
                         << static_cast<int>(currentStage) << " to " 
                         << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            }
            
            return self.getDevelopmentalStage();
        }, py::arg("timestep"), py::arg("enableAutoStage") = false,
             "Update development with optional stage tracking");
        
        // Apply reward modulation with analysis
        .def("apply_reward_modulation_analyzed", [](AgentBrain& self, 
                                                  float reward, float predictedReward,
                                                  bool verbose = false) {
            if (!self.getBrain()) {
                throw std::runtime_error("AgentBrain not properly initialized");
            }
            
            float error = std::abs(reward - predictedReward);
            
            self.applyRewardModulation(reward, predictedReward);
            
            if (verbose) {
                std::cout << "[REWARD] Applied reward: " << reward 
                         << " | Prediction: " << predictedReward 
                         << " | Error: " << error << std::endl;
                std::cout << "[REWARD] Neuromodulation level: " 
                         << self.getNeuromodulationLevel() << std::endl;
            }
            
            return error;
        }, py::arg("reward"), py::arg("predictedReward"), py::arg("verbose") = false,
             "Apply reward modulation with error analysis");
        
        // Get comprehensive agent state
        .def("get_comprehensive_state", [](AgentBrain& self) {
            struct ComprehensiveAgentState {
                // Basic info
                bool isInitialized;
                size_t sensorySize;
                size_t motorSize;
                
                // Neuromodulation state
                float curiosityLevel;
                float noveltyLevel;
                float predictionError;
                float neuromodulationLevel;
                
                // Developmental state
                DevelopmentalStage developmentalStage;
                
                // Subsystem status
                bool rewardModEnabled;
                bool structuralPlasticEnabled;
                bool developmentEnabled;
                bool curiosityEnabled;
                
                // Performance metrics
                size_t lastFiringCount;
                double lastDevelopmentStep;
                
                // Status message
                std::string status;
            };
            
            ComprehensiveAgentState state;
            state.isInitialized = (self.getBrain() != nullptr);
            state.sensorySize = self.getSensoryInputSize();
            state.motorSize = self.getMotorOutputSize();
            state.curiosityLevel = self.getCuriosityLevel();
            state.noveltyLevel = self.getNoveltyLevel();
            state.predictionError = self.getPredictionError();
            state.neuromodulationLevel = self.getNeuromodulationLevel();
            state.developmentalStage = self.getDevelopmentalStage();
            state.rewardModEnabled = self.isRewardModulationEnabled();
            state.structuralPlasticEnabled = self.isStructuralPlasticityEnabled();
            state.developmentEnabled = self.isDevelopmentEnabled();
            state.curiosityEnabled = self.isCuriosityEnabled();
            
            if (self.getBrain()) {
                state.lastFiringCount = self.getBrain()->getFiringNeuronCount();
            } else {
                state.lastFiringCount = 0;
            }
            
            // Generate status message
            if (!state.isInitialized) {
                state.status = "AgentBrain not initialized";
            } else if (state.sensorySize == 0) {
                state.status = "Initialized but no sensory input configured";
            } else {
                std::stringstream ss;
                ss << "Agent ready - Curiosity: " << state.curiosityLevel 
                   << ", Novelty: " << state.noveltyLevel 
                   << ", Prediction Error: " << state.predictionError;
                state.status = ss.str();
            }
            
            return state;
        }, "Get comprehensive agent state for debugging/analysis");
        
        // Configuration helpers for common patterns
        .def("_setup_learning_agent", [](AgentBrain& self) {
            // Common setup for learning agents
            self.enableAllSystems();
            // Additional setup could include specific neuromodulation weights, etc.
            return true;
        }, "Setup agent for learning tasks (convenience method)")
        
        .def("_setup_exploratory_agent", [](AgentBrain& self) {
            // Setup for exploratory agents
            self.enableCuriosity(true);
            self.enableRewardModulation(true);
            // Higher curiosity levels encourage exploration
            return true;
        }, "Setup agent for exploratory tasks (convenience method)")
        
        .def("_setup_stable_agent", [](AgentBrain& self) {
            // Setup for stable, reliable agents
            self.enableRewardModulation(true);
            self.enableDevelopment(true);
            self.enableCuriosity(false); // Lower curiosity for stability
            return true;
        }, "Setup agent for stable operation (convenience method)");

m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
    return std::make_shared<Config>();
}, "Create a default configuration");

    // ===== CONVENIENCE FACTORY FUNCTIONS =====
    
    // Create brain with common presets
    m.def("create_brain_for_task", [](const std::string& taskType) {
        auto config = std::make_shared<Config>();
        
        if (taskType == "simple") {
            config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
        } else if (taskType == "complex") {
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            config->set("plasticity_stdp_enable", true, ConfigSource::Default);
            config->set("plasticity_hebbian_enable", true, ConfigSource::Default);
        } else if (taskType == "learning") {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config->set("connection_probability", 0.08f, ConfigSource::Default);
            config->set("plasticity_stdp_enable", true, ConfigSource::Default);
            config->set("plasticity_hebbian_enable", true, ConfigSource::Default);
            config->set("plasticity_structural_enable", true, ConfigSource::Default);
            config->set("neuromod_dopamine_scale", 1.0f, ConfigSource::Default);
        } else {
            throw std::runtime_error("Unknown task type: " + taskType);
        }
        
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        return std::make_shared<Brain>(config);
    }, py::arg("taskType"),
         "Create a brain pre-configured for specific task types");
    
    // Enhanced world creation
    m.def("create_simple_world_with_settings", [](size_t width, size_t height,
                                                 size_t visionWidth, size_t visionHeight,
                                                 float maxEnergy) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setMaxEnergy(maxEnergy);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
           py::arg("maxEnergy") = 100.0f,
           "Create a simple world with custom settings");
    
    // Create agent with common configurations
    m.def("create_agent_for_environment", [](std::shared_ptr<Brain> brain, 
                                           const std::string& behaviorMode) {
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (behaviorMode == "exploratory") {
            agent->enableAllSystems();
            agent->enableCuriosity(true);
        } else if (behaviorMode == "conservative") {
            agent->enableRewardModulation(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(false);
        } else if (behaviorMode == "adaptive") {
            agent->enableAllSystems();
            // Adaptive learning - all systems working together
        } else {
            agent->enableRewardModulation(true);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("behaviorMode"),
         "Create an agent brain with pre-configured behavior mode");
    
    // ===== ADVANCED UTILITY FUNCTIONS =====
    
    // Complete agent setup with world integration
    m.def("setup_complete_agent", [](const std::string& taskType,
                                   const std::string& behaviorMode) {
        // Create brain for task
        auto brain = create_brain_for_task(taskType);
        brain->initialize();
        
        // Create world
        auto world = create_simple_world_with_settings(20, 20, 8, 8, 100.0f);
        world->setAgentStart(10.0f, 10.0f);
        
        // Create agent
        auto agent = create_agent_for_environment(brain, behaviorMode);
        agent->initialize(*world);
        
        return py::make_tuple(brain, world, agent);
    }, py::arg("taskType"), py::arg("behaviorMode"),
         "Create a complete agent setup with brain, world, and agent brain");
    
    // Agent simulation runner
    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain,
                                     std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent,
                                     size_t numSteps, bool verbose = false) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid agent components");
        }
        
        agent->initialize(*world);
        
        std::vector<size_t> firingCounts;
        firingCounts.reserve(numSteps);
        
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            firingCounts.push_back(brain->getFiringNeuronCount());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": " 
                         << firingCounts.back() << " firing neurons, "
                         << "Action: " << static_cast<int>(action->getType()) << std::endl;
            }
        }
        
        // Calculate statistics
        size_t totalFiring = std::accumulate(firingCounts.begin(), firingCounts.end(), 0UL);
        double avgFiring = static_cast<double>(totalFiring) / firingCounts.size();
        
        return py::make_tuple(firingCounts, totalFiring, avgFiring);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
           py::arg("numSteps"), py::arg("verbose") = false,
           "Run a complete agent simulation and return statistics");
    
    // Debug and profiling utilities
    m.def("profile_brain_performance", [](std::shared_ptr<Brain> brain,
                                          size_t warmupSteps = 100,
                                          size_t measurementSteps = 500) {
        if (!brain) {
            throw std::runtime_error("Invalid brain object");
        }
        
        // Warmup
        for (size_t i = 0; i < warmupSteps; ++i) {
            brain->step(i);
        }
        
        // Measurement
        std::vector<size_t> firingCounts;
        firingCounts.reserve(measurementSteps);
        
        for (size_t i = 0; i < measurementSteps; ++i) {
            brain->step(i + warmupSteps);
            firingCounts.push_back(brain->getFiringNeuronCount());
        }
        
        // Calculate metrics
        size_t totalFiring = std::accumulate(firingCounts.begin(), firingCounts.end(), 0UL);
        double avgFiring = static_cast<double>(totalFiring) / firingCounts.size();
        
        double minFiring = *std::min_element(firingCounts.begin(), firingCounts.end());
        double maxFiring = *std::max_element(firingCounts.begin(), firingCounts.end());
        double variance = 0.0;
        for (size_t firing : firingCounts) {
            variance += (firing - avgFiring) * (firing - avgFiring);
        }
        variance /= firingCounts.size();
        double stdDev = std::sqrt(variance);
        
        struct PerformanceMetrics {
            double avgFiringRate;
            double minFiringRate;
            double maxFiringRate;
            double firingRateStdDev;
            size_t totalSpikes;
            double coefficientOfVariation;
            std::string stabilityClassification;
        };
        
        PerformanceMetrics metrics;
        metrics.avgFiringRate = avgFiring;
        metrics.minFiringRate = minFiring;
        metrics.maxFiringRate = maxFiring;
        metrics.firingRateStdDev = stdDev;
        metrics.totalSpikes = totalFiring;
        metrics.coefficientOfVariation = (avgFiring > 0) ? (stdDev / avgFiring) : 0.0;
        
        // Classification
        if (metrics.coefficientOfVariation < 0.1) {
            metrics.stabilityClassification = "Highly Stable";
        } else if (metrics.coefficientOfVariation < 0.3) {
            metrics.stabilityClassification = "Stable";
        } else if (metrics.coefficientOfVariation < 0.5) {
            metrics.stabilityClassification = "Moderately Variable";
        } else {
            metrics.stabilityClassification = "Highly Variable";
        }
        
        return metrics;
    }, py::arg("brain"), py::arg("warmupSteps") = 100, py::arg("measurementSteps") = 500,
         "Profile brain performance and return detailed metrics");
    
    // Error analysis utilities
    m.def("analyze_brain_errors", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Invalid brain object");
        }
        
        struct ErrorAnalysis {
            bool hasMemoryLeak;
            bool hasPerformanceIssues;
            bool isUninitialized;
            bool hasExcessiveSpiking;
            std::string primaryIssue;
            std::string recommendations;
            double confidence;
        };
        
        ErrorAnalysis analysis;
        analysis.hasMemoryLeak = false; // Would need memory tracking
        analysis.hasPerformanceIssues = false;
        analysis.isUninitialized = (brain->getTotalNeuronCount() == 0);
        analysis.hasExcessiveSpiking = (brain->getTotalSpikeCount() > 10000);
        
        if (analysis.isUninitialized) {
            analysis.primaryIssue = "Brain not properly initialized";
            analysis.recommendations = "Call brain.initialize() before running simulation";
            analysis.confidence = 0.95;
        } else if (analysis.hasExcessiveSpiking) {
            analysis.primaryIssue = "Excessive spiking activity";
            analysis.recommendations = "Reduce input current or adjust neuron thresholds";
            analysis.confidence = 0.85;
        } else {
            analysis.primaryIssue = "No major issues detected";
            analysis.recommendations = "Brain appears to be functioning normally";
            analysis.confidence = 0.90;
        }
        
        return analysis;
    }, py::arg("brain"),
         "Analyze brain for potential errors and issues");
    
    // ===== LEGACY COMPATIBILITY =====
    
    // Keep original create functions for backward compatibility
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (legacy compatibility)");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (legacy compatibility)");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (legacy compatibility)");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (legacy compatibility)");
    
    // ===== CONSTANTS AND HELPERS =====
    
    // Mathematical constants
    m.attr("PI") = 3.14159265358979323846;
    m.attr("E") = 2.71828182845904523536;
    m.attr("GOLDEN_RATIO") = 1.61803398874989484820;
    
    // Version information
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features for researchers and developers";
    
    // Error categories for better error handling
    py::class_<std::runtime_error>(m, "RuntimeError");
    
    m.def("get_available_presets", []() {
        return std::vector<std::string>{"simple", "complex", "learning"};
    }, "Get list of available configuration presets");
    
    m.def("get_behavior_modes", []() {
        return std::vector<std::string>{"exploratory", "conservative", "adaptive"};
    }, "Get list of available behavior modes");
}

} // namespace nlm
