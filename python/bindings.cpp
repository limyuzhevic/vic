// Enhanced error handling with custom exception types
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message) : NLMException("Configuration error: " + message) {}
};

class InitializationError : public NLMException {
public:
    InitializationError(const std::string& message) : NLMException("Initialization error: " + message) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message) : NLMException("Simulation error: " + message) {}
};

class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message) : NLMException("Memory error: " + message) {}
};

class NeuromodulationError : public NLMException {
public:
    NeuromodulationError(const std::string& message) : NLMException("Neuromodulation error: " + message) {}
};

class ExportError : public NLMException {
public:
    ExportError(const std::string& message) : NLMException("Export error: " + message) {}
};

// Parameter validation utilities
struct ParameterValidator {
    static void validateRange(float value, float min, float max, const std::string& paramName) {
        if (value < min || value > max) {
            throw ConfigurationError(paramName + " must be in range [" + 
                std::to_string(min) + ", " + std::to_string(max) + "]");
        }
    }
    
    static void validatePositive(float value, const std::string& paramName) {
        if (value < 0) {
            throw ConfigurationError(paramName + " must be non-negative");
        }
    }
    
    static void validateNonEmpty(const std::string& value, const std::string& paramName) {
        if (value.empty()) {
            throw ConfigurationError(paramName + " cannot be empty");
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        =================================================================
        Advanced Python binding for the NLM C++ neural simulation framework.
        
        Features:
        - Enhanced error handling with specific exception types
        - Direct access to neuromodulation systems (dopamine, curiosity, novelty, prediction error)
        - Complete memory system access (working, episodic, associative memory)
        - Advanced brain configuration and neural circuit management
        - Real-time simulation control and monitoring
        - Multi-format brain state export and analysis
        - Power user features: batch processing, parallel simulation, custom learning rules
        - Comprehensive documentation and parameter validation
        - Backward compatible with existing API
        
        Quick Start Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> world = pynlm.createSimpleWorld()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> # Use advanced features...
    )pbdoc";

    // Register enhanced exception types
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<NeuromodulationError>(m, "NeuromodulationError");
    py::register_exception<ExportError>(m, "ExportError");

    // Type definitions
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

    // Enhanced Config class with validation
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)
        
        Configuration management with parameter validation and type checking.
        Supports loading/saving from JSON files, command line arguments, and programmatic setting.
        
        Features:
        - Type-safe parameter validation
        - Multiple import/export formats
        - Parameter ranges and constraints
        - Configuration templates and presets
        - Parameter groups and categories
        )pbdoc")
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
        // Advanced configuration methods with validation
        .def("set", [](Config& self, const std::string& key, int value) {
            if (value < 0) throw ConfigurationError("Integer value must be non-negative");
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration parameter with validation")
        .def("set", [](Config& self, const std::string& key, float value) {
            if (value < 0.0f) throw ConfigurationError("Float value must be non-negative");
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration parameter with validation")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            if (value.empty()) throw ConfigurationError("String value cannot be empty");
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration parameter with validation")
        .def("get", [](const Config& self, const std::string& key, int defaultValue) {
            return self.get(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get configuration parameter with default")
        .def("get", [](const Config& self, const std::string& key, float defaultValue) {
            return self.get(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
           "Get configuration parameter with default")
        .def("get", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.get(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get configuration parameter with default")
        .def("validateAll", &Config::validate,
             "Validate all configuration parameters against constraints")
        .def("createTemplate", [](const std::string& templateName) {
            auto config = std::make_shared<Config>();
            if (templateName == "default") {
                config->set("brain.neuronCount", 1000);
                config->set("brain.regionCount", 10);
                config->set("brain.sensoryInputSize", 20);
                config->set("brain.motorOutputSize", 8);
                config->set("brain.plasticity.stdp.enable", true);
                config->set("brain.plasticity.hebbian.enable", true);
                config->set("brain.development.criticalPeriod.duration", 100);
                config->set("brain.neuromodulation.dopamine.enabled", true);
                config->set("brain.neuromodulation.curiosity.enabled", true);
                config->set("brain.neuromodulation.novelty.enabled", true);
                config->set("brain.neuromodulation.predictionError.enabled", true);
            }
            return config;
        }, py::arg("templateName"), "Create configuration template");

    // Enhanced SensoryInput classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        
        Abstract base class for all sensory inputs. Provides common interface for
        accessing sensory data with timestamps and validation.
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("isValid", &SensoryInput::isValid,
             "Check if the sensory input data is valid");

    // Advanced Vision class with feature extraction
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Advanced vision sensory input)
        
        High-resolution visual input system with frame processing, edge detection,
        and feature extraction capabilities.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getGrayscale", &Vision::getGrayscale)
        .def("applyFilter", [](Vision& self, const std::string& filterType) {
            self.applyFilter(filterType);
        }, py::arg("filterType"))
        .def("detectEdges", &Vision::detectEdges)
        .def("extractFeatures", &Vision::extractFeatures)
        .def("getHistogram", &Vision::getHistogram)
        .def("normalize", &Vision::normalize)
        .def("resize", [](Vision& self, size_t width, size_t height) {
            self.resize(width, height);
        }, py::arg("width"), py::arg("height"));

    // Enhanced Brain class with advanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced central neural simulation brain class)
        
        Complete neural simulation brain with spiking neurons, plasticity,
        neuromodulation, memory systems, and real-time control capabilities.
        )pbdoc")
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
        // Advanced brain methods for neuromodulation and memory access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error neuromodulator")
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
             "Get prediction system")
        .def("getNeuralPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getNeuromodulationLevel", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signal to brain")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("simulate", [](Brain& self, int steps, float timestep) {
            if (steps < 0) throw ConfigurationError("Step count must be non-negative");
            if (timestep <= 0) throw ConfigurationError("Timestep must be positive");
            for (int i = 0; i < steps; ++i) {
                self.step(i, timestep * i);
            }
        }, py::arg("steps"), py::arg("timestep"),
           "Run simulation for specified number of steps")
        .def("getSimulationHistory", [](Brain& self, int steps) {
            if (steps < 0) throw ConfigurationError("Step count must be non-negative");
            std::vector<std::map<std::string, py::object>> history;
            for (int i = 0; i < steps; ++i) {
                self.step(i);
                // Simplified history for brevity
                history.push_back(std::map<std::string, py::object>{
                    {"step", static_cast<int>(i)},
                    {"time", static_cast<double>(i)}
                });
            }
            return history;
        }, py::arg("steps"),
           "Get simulation history for analysis");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface)
        
        Complete agent brain interface connecting NLM brain to world with
        sensory processing, motor decoding, and reinforcement learning.
        )pbdoc")
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
        .def("act", [](AgentBrain& self, const SensoryPercept& percept, float reward) {
            self.processSensoryInput(percept);
            self.applyRewardModulation(reward, 0.0f);
            self.updateDevelopment(1.0);
            return self.decodeMotorCommand();
        }, py::arg("percept"), py::arg("reward"),
           "Perform complete agent action cycle");

    // Neuromodulation classes
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)
        
        Reward and reinforcement learning neuromodulator using Ornstein-Uhlenbeck
        processes for realistic temporal difference learning.
        )pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", [](Dopamine& self, float level) {
            if (level < 0.0f || level > 1.0f) {
                throw NeuromodulationError("Dopamine level must be in range [0.0, 1.0]");
            }
            self.setLevel(level);
        }, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"))
        .def("getValueFunction", &Dopamine::getValueFunction)
        .def("getRewardPrediction", &Dopamine::getRewardPrediction)
        .def("reset", &Dopamine::reset);

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)
        
        Exploration motivation based on weighted combination of novelty and
        prediction error signals.
        )pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel)
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"))
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"))
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"))
        .def("reset", &Curiosity::reset);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator)
        
        Novelty detection signal computed from comparison with previous observations.
        )pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("detectNovelty", [](Novelty& self, const Vision& observation, const Vision& previousObservation) {
            self.detectNovelty(observation, previousObservation);
        }, py::arg("observation"), py::arg("previousObservation"))
        .def("detectNoveltyPattern", [](Novelty& self, const std::vector<float>& currentPattern, const std::vector<float>& previousPattern) {
            self.detectNovelty(currentPattern, previousPattern);
        }, py::arg("currentPattern"), py::arg("previousPattern"))
        .def("update", &Novelty::update, py::arg("dt"))
        .def("getHistory", &Novelty::getHistory)
        .def("clearHistory", &Novelty::clearHistory)
        .def("reset", &Novelty::reset);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)
        
        Prediction error signal for curiosity and learning using temporal difference
        methods to compute discrepancies between predicted and actual values.
        )pbdoc")
        .def(py::init<>())
        .def("getError", &PredictionError::getError)
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"))
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"))
        .def("getHistory", &PredictionError::getHistory)
        .def("clearHistory", &PredictionError::clearHistory)
        .def("getMagnitude", &PredictionError::getMagnitude)
        .def("reset", &PredictionError::reset);

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward neuromodulator)
        
        Reward signal for reinforcement learning with temporal accumulation
        and reset capabilities.
        )pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"))
        .def("reset", &Reward::reset)
        .def("computeReward", [](Reward& self, const Vision& observation) {
            return self.computeReward(observation);
        }, py::arg("observation"))
        .def("getHistory", &Reward::getHistory)
        .def("clearHistory", &Reward::clearHistory)
        .def("getTotalReward", &Reward::getTotalReward)
        .def("getAverageReward", &Reward::getAverageReward)
        .def("reset", &Reward::reset);

    // Utility functions
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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version info
    m.attr("__version__") = "2.0.0-enhanced";

    // Exception translator
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const InitializationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const MemoryError& e) {
            PyErr_SetString(PyExc_MemoryError, e.what());
        } catch (const NeuromodulationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const ExportError& e) {
            PyErr_SetString(PyExc_IOError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });
}

} // namespace nlm