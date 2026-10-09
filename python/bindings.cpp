#include "../src/core/Comparison/ConfigurationComparator.hpp"
#include "../src/core/Merging/ConfigurationMerger.hpp"
#include "../src/core/Inheritance/ConfigurationInheritanceManager.hpp"
#include "../src/core/Analysis/ConfigurationComplexityAnalyzer.hpp"
#include "../src/core/Analysis/ConfigurationDependencyAnalyzer.hpp"
#include "../src/core/Analysis/ConfigurationSensitivityAnalyzer.hpp"
#include "../src/core/Analysis/ConfigurationImpactAnalyzer.hpp"
#include "../src/core/Analysis/ConfigurationOptimizationAnalyzer.hpp"
#include "../src/core/Export/ConfigurationExporter.hpp"
#include "../src/core/Import/ConfigurationImporter.hpp"
#include "../src/core/Versioning/ConfigurationVersionManager.hpp"
#include "../src/core/Templating/ConfigurationTemplateManager.hpp"
#include "../src/core/Sharing/ConfigurationSharingManager.hpp"
#include "../src/core/Marketplace/ConfigurationMarketplace.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, Action,
        BatchProcessing, ParallelExecution, PerformanceMonitoring, and AdvancedExperiment utilities.
    )pbdoc";

    // Register custom NLM exceptions
    py::register_exception<nlm::NLMError>(m, "NLMError");
    py::register_exception<nlm::ConfigError>(m, "ConfigError");
    py::register_exception<nlm::ConfigValidationError>(m, "ConfigValidationError");
    py::register_exception<nlm::ConfigTypeError>(m, "ConfigTypeError");
    py::register_exception<nlm::ConfigFileError>(m, "ConfigFileError");
    py::register_exception<nlm::BrainError>(m, "BrainError");
    py::register_exception<nlm::BrainValidationError>(m, "BrainValidationError");
    py::register_exception<nlm::BrainStateError>(m, "BrainStateError");
    py::register_exception<nlm::NeuralNetworkError>(m, "NeuralNetworkError");
    py::register_exception<nlm::SimulationError>(m, "SimulationError");
    py::register_exception<nlm::SimulationParameterError>(m, "SimulationParameterError");
    py::register_exception<nlm::SimulationStepError>(m, "SimulationStepError");
    py::register_exception<nlm::WorldError>(m, "WorldError");
    py::register_exception<nlm::WorldValidationError>(m, "WorldValidationError");
    py::register_exception<nlm::MemoryError>(m, "MemoryError");
    py::register_exception<nlm::MemoryCapacityError>(m, "MemoryCapacityError");
    py::register_exception<nlm::AgentError>(m, "AgentError");
    py::register_exception<nlm::AgentValidationError>(m, "AgentValidationError");
    py::register_exception<nlm::PerformanceError>(m, "PerformanceError");
    py::register_exception<nlm::MemoryLeakError>(m, "MemoryLeakError");
    py::register_exception<nlm::PerformanceBottleneckError>(m, "PerformanceBottleneckError");

    // Register standard exception for backward compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    // Configuration class
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
        
        // Configuration utilities
        .def("configure_for_task", &Config::configure_for_task,
             py::arg("taskType"), py::arg("parameters") = std::vector<std::string>(),
             R"pbdoc(High-level configuration for common tasks.
            
            Args:
                taskType (str): Type of task - 'basic', 'advanced', 'research', 'production'
                parameters (list[str]): Additional configuration parameters
                
            Returns:
                bool: True if configuration succeeded
                
            Raises:
                ValueError: If taskType is invalid
            )pbdoc")
        .def("setup_development", &Config::setup_development,
             py::arg("stage") = "critical_period",
             R"pbdoc(Configure brain for developmental studies.
            
            Args:
                stage (str): Developmental stage - 'initial', 'critical_period', 
                           'maturation', 'adult'
                
            Returns:
                bool: True if setup succeeded
                
            Raises:
                ValueError: If stage is invalid
            )pbdoc")
        .def("setup_learning", &Config::setup_learning,
             py::arg("mode") = "supervised",
             py::arg("learningRate") = 0.01f,
             R"pbdoc(Configure brain for learning experiments.
            
            Args:
                mode (str): Learning mode - 'supervised', 'reinforcement', 'unsupervised',
                           'self_supervised'
                learningRate (float): Learning rate (default: 0.01)
                
            Returns:
                bool: True if setup succeeded
                
            Raises:
                ValueError: If mode is invalid or learningRate is negative
            )pbdoc");

    // Sensory Input classes
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

    // Action class
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

    // World Object class
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

    // Agent Body class
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

    // Action Result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Sensory Percept class
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

    // Simple World class
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
        
        // World configuration utilities
        .def("create_default", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, "Create a default world instance")
        .def("create_empty", [](size_t width, size_t height) -> std::shared_ptr<SimpleWorld> {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, 64, 64);
            return world;
        }, py::arg("width"), py::arg("height"), "Create an empty world with specified dimensions")
        .def("create_with_objects", [](size_t width, size_t height, 
                                       const std::vector<WorldObject>& objects) -> std::shared_ptr<SimpleWorld> {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, 64, 64);
            for (const auto& obj : objects) {
                world->addObject(obj);
            }
            return world;
        }, py::arg("width"), py::arg("height"), py::arg("objects"), "Create a world with initial objects");

    // Brain class
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
        
        // Simulation management methods
        .def("run_simulation", &Brain::run_simulation,
             py::arg("timesteps"), py::arg("world") = nullptr,
             R"pbdoc(Complete simulation run with optional world integration. 
            
            Args:
                timesteps (int): Number of simulation steps to run
                world (SimpleWorld, optional): World instance for agent interaction
                
            Returns:
                dict: Simulation summary including stats, actions, and timing
            
            Raises:
                RuntimeError: If simulation fails to initialize or run
            )pbdoc")
        .def("run_multiple_episodes", &Brain::run_multiple_episodes,
             py::arg("episodes"), py::arg("stepsPerEpisode"), py::arg("world") = nullptr,
             py::arg("resetBetweenEpisodes" ) = true,
             R"pbdoc(Run multiple simulation episodes with state management.
            
            Args:
                episodes (int): Number of episodes to run
                stepsPerEpisode (int): Steps per episode
                world (SimpleWorld, optional): World instance
                resetBetweenEpisodes (bool): Reset brain state between episodes
                
            Returns:
                list[dict]: List of episode summaries
            )pbdoc")
        .def("run_parameter_sweep", &Brain::run_parameter_sweep,
             py::arg("paramSets"), py::arg("timesteps"), py::arg("world") = nullptr,
             R"pbdoc(Run parameter sweeps for experimental parameter exploration.
            
            Args:
                paramSets (list[dict]): List of parameter dictionaries to sweep
                timesteps (int): Number of timesteps per parameter set
                world (SimpleWorld, optional): World instance
                
            Returns:
                dict: Parameter sweep results including performance metrics
            )pbdoc")
        
        // Data utilities
        .def("get_brain_summary", &Brain::get_brain_summary,
             R"pbdoc(Get comprehensive brain statistics and state summary.
            
            Returns:
                dict: Brain summary including neuron counts, firing rates,
                     connectivity metrics, and developmental status
            )pbdoc")
        .def("export_network", &Brain::export_network,
             py::arg("format") = "csv",
             R"pbdoc(Export network connectivity to specified format.
            
            Args:
                format (str): Export format - 'csv', 'json', or 'graphml'
                
            Returns:
                str: Network connectivity data in requested format
            )pbdoc")
        .def("get_spiking_patterns", &Brain::get_spiking_patterns,
             py::arg("windowSize" ) = 100, py::arg("minSpikeCount" ) = 5,
             R"pbdoc(Extract spiking patterns from simulation history.
            
            Args:
                windowSize (int): Time window size for pattern analysis
                minSpikeCount (int): Minimum spikes required for pattern detection
                
            Returns:
                list[dict]: Detected spiking patterns with timing and statistics
            )pbdoc")
        
        // Validation helpers
        .def("validate_brain", &Brain::validate_brain,
             R"pbdoc(Validate brain configuration and state.
             
             Returns:
                 dict: Validation results including errors, warnings, and status
                 
             Raises:
                 RuntimeError: If brain configuration is invalid
             )pbdoc")
        .def("check_memory_integrity", &Brain::check_memory_integrity,
             R"pbdoc(Check memory systems integrity and consistency.
             
             Returns:
                 dict: Memory system integrity status and any detected issues
             )pbdoc")
        .def("validate_agent", &Brain::validate_agent,
             py::arg("world") = nullptr,
             R"pbdoc(Validate agent configuration and settings.
             
             Args:
                 world (SimpleWorld, optional): World instance for validation
                 
             Returns:
                 dict: Agent validation results including configuration and runtime checks
                 
             Raises:
                 RuntimeError: If agent configuration is invalid
             )pbdoc");

        // Neural activity visualization
        .def("visualize_spiking_patterns", &Brain::get_spiking_patterns,
             py::arg("window_size") = 100, py::arg("min_spike_count") = 5,
             R"pbdoc(Visualize neural spiking patterns.
             
             Args:
                 window_size (int): Time window size for pattern analysis
                 min_spike_count (int): Minimum spikes required for pattern detection
                 
             Returns:
                 list[dict]: Detected spiking patterns with timing and statistics
                 
             Raises:
                 RuntimeError: If visualization fails
             )pbdoc")
        .def("plot_firing_rates", &Brain::get_average_firing_rate,
             R"pbdoc(Plot neural firing rates over time.
             
             Returns:
                 dict: Firing rate statistics and visualization data
                 
             Raises:
                 RuntimeError: If plotting fails
             )pbdoc")
        .def("visualize_activity_heatmap", &Brain::get_brain_summary,
             py::arg("region_focus") = std::vector<RegionId>(),
             R"pbdoc(Create heatmap visualization of neural activity.
             
             Args:
                 region_focus (list[RegionId]): Specific regions to focus on
                 
             Returns:
                 dict: Heatmap data and visualization configuration
                 
             Raises:
                 RuntimeError: If heatmap generation fails
             )pbdoc")
        .def("visualize_time_series", &Brain::get_brain_summary,
             py::arg("metric") = "spiking_rate",
             py::arg("time_window") = "all",
             R"pbdoc(Generate time series visualization of neural activity.
             
             Args:
                 metric (str): Neural activity metric to visualize
                 time_window (str): Time window for visualization
                 
             Returns:
                 dict: Time series data and visualization configuration
                 
             Raises:
                 RuntimeError: If time series generation fails
             )pbdoc");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        // AgentBrain utilities
        .def("create_default_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a default agent brain interface")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        
        // AgentBrain configuration helpers
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        
        // AgentBrain configuration helpers
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        
        // AgentBrain simulation helpers
        .def("run_episode", &AgentBrain::run_episode,
             py::arg("world"), py::arg("steps"), py::arg("record" ) = false,
             R"pbdoc(Run a single episode with agent brain and world interaction.
             
             Args:
                 world (SimpleWorld): World instance for the episode
                 steps (int): Number of simulation steps for the episode
                 record (bool): Whether to record episode data
                 
             Returns:
                 dict: Episode results including actions, rewards, and observations
             )pbdoc")
        .def("run_agent_simulation", [](std::shared_ptr<AgentBrain> agentBrain, 
                                        std::shared_ptr<SimpleWorld> world,
                                        int episodes, int stepsPerEpisode,
                                        bool recordEpisodes = false) {
            // Create a wrapper that uses the AgentBrain's internal brain
            auto brain = agentBrain->getBrain();
            std::vector<SimulationStep> steps;
            
            for (int episode = 0; episode < episodes; ++episode) {
                agentBrain->reset();
                
                for (int step = 0; step < stepsPerEpisode; ++step) {
                    Timestamp currentTime = static_cast<Timestamp>(step);
                    SimulationStep currentStep;
                    currentStep.time = currentTime;
                    
                    // Get sensory input
                    auto percept = world->getSensoryPercept();
                    brain->receiveSensoryInput(percept);
                    
                    // Process and produce action
                    auto action = brain->produceAction();
                    world->applyMotorCommand(action, currentTime);
                    
                    // Update world
                    world->update(currentTime);
                    
                    if (recordEpisodes) {
                        steps.push_back(currentStep);
                    }
                }
            }
            
            return std::vector<SimulationStep>();
        }, py::arg("agentBrain"), py::arg("world"), py::arg("episodes"), 
             py::arg("stepsPerEpisode") = 100, py::arg("recordEpisodes") = false,
             R"pbdoc(Run a complete agent simulation with brain and world interaction.
             
             Args:
                 agentBrain (AgentBrain): The agent brain to control
                 world (SimpleWorld): The world for simulation
                 episodes (int): Number of episodes to run
                 stepsPerEpisode (int): Steps per episode
                 recordEpisodes (bool): Whether to record all steps
                 
             Returns:
                 list[dict]: List of recorded simulation steps (if recordEpisodes is True)
             )pbdoc");

    // Validation and error handling helpers
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        return nlm::validateConfig(*config);
    }, py::arg("config"),
          "Validate configuration and return result");
    
    m.def("validate_brain", [](std::shared_ptr<Brain> brain, const std::string& operation = "") {
        return nlm::validateBrain(*brain, operation);
    }, py::arg("brain"), py::arg("operation") = "",
          "Validate brain state and return result");
    
    m.def("validate_world", [](std::shared_ptr<SimpleWorld> world) {
        return nlm::validateWorld(*world);
    }, py::arg("world"),
          "Validate world state and return result");
    
    m.def("validate_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, Timestamp currentTime) {
        return nlm::validateSimulation(*brain, *world, currentTime);
    }, py::arg("brain"), py::arg("world"), py::arg("current_time"),
          "Validate simulation state and return result");
    
    m.def("safe_reset", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        std::string brainMsg, worldMsg;
        bool brainSuccess = nlm::ErrorRecovery::safeResetBrain(*brain, brainMsg);
        bool worldSuccess = nlm::ErrorRecovery::safeResetWorld(*world, worldMsg);
        return std::make_tuple(brainSuccess, brainMsg, worldSuccess, worldMsg);
    }, py::arg("brain"), py::arg("world"),
          "Safely reset brain and world, return success and messages");
    
    m.def("get_error_report", [](const std::exception& error) {
        return nlm::DebugDiagnostics::generateErrorReport(error);
    }, py::arg("error"),
          "Generate detailed error report from exception");
    
    m.def("generate_validation_summary", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        return nlm::DebugDiagnostics::generateValidationSummary(*brain, *config, *world);
    }, py::arg("brain"), py::arg("config"), py::arg("world"),
          "Generate comprehensive validation summary");
    
    m.def("check_common_issues", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        std::vector<std::string> issues;
        nlm::DebugDiagnostics::checkCommonIssues(*brain, *config, *world, issues);
        return issues;
    }, py::arg("brain"), py::arg("config"), py::arg("world"),
          "Check for common issues and return list of problems");
    
    m.def("monitor_system_health", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        std::unordered_map<std::string, double> metrics;
        nlm::DebugDiagnostics::monitorSystemHealth(*brain, *world, metrics);
        return metrics;
    }, py::arg("brain"), py::arg("world"),
          "Monitor system health and return metrics");
    
    m.def("detect_memory_leak", [](std::shared_ptr<Brain> brain, size_t sampleSize, std::string& leakType, size_t& detectedBytes) {
        return nlm::DebugDiagnostics::detectMemoryLeak(*brain, sampleSize, leakType, detectedBytes);
    }, py::arg("brain"), py::arg("sample_size"), py::arg("leak_type"), py::arg("detected_bytes"),
          "Detect memory leaks in brain and return results");
    
    m.def("detect_performance_bottleneck", [](std::shared_ptr<Brain> brain, const std::string& bottleneckType, double& thresholdExceeded, double& thresholdLimit) {
        return nlm::DebugDiagnostics::detectPerformanceBottleneck(*brain, bottleneckType, thresholdExceeded, thresholdLimit);
    }, py::arg("brain"), py::arg("bottleneck_type"), py::arg("threshold_exceeded"), py::arg("threshold_limit"),
          "Detect performance bottlenecks in brain and return results");

    // ===============================================
    // COMPREHENSIVE LOGGING AND DEBUGGING UTILITIES
    // ===============================================

    // Advanced Logger class
    py::class_<nlm::AdvancedLogger>(m, "AdvancedLogger", R"pbdoc(Advanced logging system with comprehensive debugging capabilities)pbdoc")
        .def(py::init<>())
        .def("add_sink", &nlm::AdvancedLogger::addSink,
             py::arg("sink"), "Add a logging sink")
        .def("remove_sink", &nlm::AdvancedLogger::removeSink,
             py::arg("sink"), "Remove a logging sink")
        .def("clear_sinks", &nlm::AdvancedLogger::clearSinks,
             "Remove all logging sinks")
        .def("set_min_level", &nlm::AdvancedLogger::setMinLevel,
             py::arg("level"), "Set minimum log level")
        .def("set_default_category", &nlm::AdvancedLogger::setDefaultCategory,
             py::arg("category"), "Set default log category")
        .def("set_enable_thread_id", &nlm::AdvancedLogger::setEnableThreadId,
             py::arg("enabled"), "Enable/disable thread ID logging")
        .def("set_enable_thread_name", &nlm::AdvancedLogger::setEnableThreadName,
             py::arg("enabled"), "Enable/disable thread name logging")
        .def("set_enable_performance_logging", &nlm::AdvancedLogger::setEnablePerformanceLogging,
             py::arg("enabled"), "Enable/disable performance logging")
        .def("set_enable_metadata_logging", &nlm::AdvancedLogger::setEnableMetadataLogging,
             py::arg("enabled"), "Enable/disable metadata logging")
        .def("log", &nlm::AdvancedLogger::log,
             py::arg("level"), py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
             py::arg("function") = "", py::arg("category") = nlm::LogCategory::Custom,
             py::arg("metadata") = std::unordered_map<std::string, std::string>(),
             "Log a message with specified parameters")
        .def("trace", &nlm::AdvancedLogger::trace,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
             py::arg("function") = "", py::arg("category") = nlm::LogCategory::Custom,
             "Log a trace message")
        .def("debug", &nlm::AdvancedLogger::debug,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
             py::arg("function") = "", py::arg("category") = nlm::LogCategory::Custom,
             "Log a debug message")
        .def("info", &nlm::AdvancedLogger::info,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
             py::arg("function") = "", py::arg("category") = nlm::LogCategory::Custom,
             "Log an info message")
        .def("warning", &nlm::AdvancedLogger::warning,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
             py::arg("function") = "", py::arg("category") = nlm::LogCategory::Custom,
             "Log a warning message")
        .def("error", &nlm::AdvancedLogger::error,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
             py::arg("function") = "", py::arg("category") = nlm::LogCategory::Custom,
             "Log an error message")
        .def("critical", &nlm::AdvancedLogger::critical,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0,
             py::arg("function") = "", py::arg("category") = nlm::LogCategory::Custom,
             "Log a critical message")
        .def("log_performance", &nlm::AdvancedLogger::logPerformance,
             py::arg("operation"), py::arg("processing_time"),
             py::arg("metrics") = std::unordered_map<std::string, double>(),
             "Log performance metrics for an operation")
        .def("log_neural_event", &nlm::AdvancedLogger::logNeuralEvent,
             py::arg("event_type"), py::arg("neuron_id"), py::arg("region_id"),
             py::arg("value") = 0.0, py::arg("metadata") = std::unordered_map<std::string, std::string>(),
             "Log a neural network event")
        .def("log_simulation_step", &nlm::AdvancedLogger::logSimulationStep,
             py::arg("step"), py::arg("time"),
             py::arg("metrics") = std::unordered_map<std::string, double>(),
             "Log a simulation step event")
        .def("log_memory_event", &nlm::AdvancedLogger::logMemoryEvent,
             py::arg("event_type"), py::arg("bytes"), py::arg("context") = "",
             "Log a memory usage event")
        .def("log_health_check", &nlm::AdvancedLogger::logHealthCheck,
             py::arg("check_name"), py::arg("passed"), py::arg("details") = "",
             "Log a health check result")
        .def("is_enabled", &nlm::AdvancedLogger::isEnabled,
             py::arg("level"), "Check if a log level is enabled")
        .def("get_global", &nlm::AdvancedLogger::getGlobal,
             "Get the global logger instance")
        .def("set_global", &nlm::AdvancedLogger::setGlobal,
             py::arg("logger"), "Set the global logger instance")
        .def("export_logs", &nlm::AdvancedLogger::exportLogs,
             py::arg("output_path"), py::arg("format") = nlm::LogFormat::JSON,
             py::arg("filter") = nullptr,
             "Export logs to file")
        .def("correlate_logs", &nlm::AdvancedLogger::correlateLogs,
             py::arg("entries"), py::arg("correlation_type"),
             py::arg("options") = std::unordered_map<std::string, std::string>(),
             "Correlate log entries based on specified correlation type")
        .def("troubleshoot_from_logs", &nlm::AdvancedLogger::troubleshootFromLogs,
             py::arg("entries"), py::arg("problem_description") = "",
             "Perform log-based troubleshooting");

    // Console logger sink
    py::class_<nlm::ConsoleLoggerSink, nlm::ILoggerSink>(m, "ConsoleLoggerSink", R"pbdoc(Console logging sink with advanced formatting)pbdoc")
        .def(py::init<>())
        .def(py::init<nlm::LogLevel>(), py::arg("min_level"))
        .def("set_min_level", &nlm::ConsoleLoggerSink::setMinLevel,
             py::arg("level"), "Set minimum log level")
        .def("set_use_colors", &nlm::ConsoleLoggerSink::setUseColors,
             py::arg("use_colors"), "Enable/disable colored output")
        .def("set_use_timestamps", &nlm::ConsoleLoggerSink::setUseTimestamps,
             py::arg("use_timestamps"), "Enable/disable timestamps")
        .def("set_timestamp_format", &nlm::ConsoleLoggerSink::setTimestampFormat,
             py::arg("format"), "Set timestamp format")
        .def("set_thread_info_enabled", &nlm::ConsoleLoggerSink::setThreadInfoEnabled,
             py::arg("enabled"), "Enable/disable thread information")
        .def("set_performance_highlighting", &nlm::ConsoleLoggerSink::setPerformanceHighlighting,
             py::arg("enabled"), "Enable/disable performance highlighting")
        .def("set_enable_category_prefix", &nlm::ConsoleLoggerSink::setEnableCategoryPrefix,
             py::arg("enabled"), "Enable/disable category prefix")
        .def("set_log_format", &nlm::ConsoleLoggerSink::setLogFormat,
             py::arg("format"), "Set log format")
        .def("set_enable_json_prettify", &nlm::ConsoleLoggerSink::setEnableJSONPrettify,
             py::arg("enabled"), "Enable/disable JSON prettification")
        .def("set_include_processing_time", &nlm::ConsoleLoggerSink::setIncludeProcessingTime,
             py::arg("include"), "Include/exclude processing time in output")
        .def("set_include_metrics", &nlm::ConsoleLoggerSink::setIncludeMetrics,
             py::arg("include"), "Include/exclude metrics in output");

    // File logger sink
    py::class_<nlm::FileLoggerSink, nlm::ILoggerSink>(m, "FileLoggerSink", R"pbdoc(File logging sink with rotation support)pbdoc")
        .def(py::init<std::string, std::string>(), py::arg("base_path"), py::arg("filename_prefix") = "nlm",
             "Create a file logger sink")
        .def("set_file_rotation_enabled", &nlm::FileLoggerSink::setFileRotationEnabled,
             py::arg("enabled"), "Enable/disable file rotation")
        .def("set_max_file_size", &nlm::FileLoggerSink::setMaxFileSize,
             py::arg("max_size"), "Set maximum file size before rotation")
        .def("set_max_backup_files", &nlm::FileLoggerSink::setMaxBackupFiles,
             py::arg("max_backups"), "Set maximum number of backup files")
        .def("set_rotation_interval", &nlm::FileLoggerSink::setRotationInterval,
             py::arg("interval"), "Set rotation interval")
        .def("set_compression_enabled", &nlm::FileLoggerSink::setCompressionEnabled,
             py::arg("enabled"), "Enable/disable compression")
        .def("set_indexing_enabled", &nlm::FileLoggerSink::setIndexingEnabled,
             py::arg("enabled"), "Enable/disable search indexing")
        .def("set_buffer_size", &nlm::FileLoggerSink::setBufferSize,
             py::arg("buffer_size"), "Set buffer size for async writes")
        .def("set_write_async", &nlm::FileLoggerSink::setWriteAsync,
             py::arg("async"), "Enable/disable asynchronous writing");

    // Remote logger sink
    py::class_<nlm::RemoteLoggerSink, nlm::ILoggerSink>(m, "RemoteLoggerSink", R"pbdoc(Remote logging sink for distributed logging)pbdoc")
        .def(py::init<std::string, std::string>(), py::arg("endpoint"), py::arg("protocol") = "udp",
             "Create a remote logger sink")
        .def("set_reconnect_enabled", &nlm::RemoteLoggerSink::setReconnectEnabled,
             py::arg("enabled"), "Enable/disable automatic reconnection")
        .def("set_max_reconnect_attempts", &nlm::RemoteLoggerSink::setMaxReconnectAttempts,
             py::arg("attempts"), "Set maximum reconnection attempts")
        .def("set_batch_size", &nlm::RemoteLoggerSink::setBatchSize,
             py::arg("size"), "Set batch size for sending")
        .def("set_send_interval", &nlm::RemoteLoggerSink::setSendInterval,
             py::arg("interval"), "Set send interval in seconds");

    // Log aggregator
    py::class_<nlm::LogAggregator>(m, "LogAggregator", R"pbdoc(Log aggregation for batch processing)pbdoc")
        .def(py::init<>())
        .def("process_entry", &nlm::LogAggregator::processEntry,
             py::arg("entry"), "Process a log entry")
        .def("set_aggregation_strategy", &nlm::LogAggregator::setAggregationStrategy,
             py::arg("strategy"), "Set aggregation strategy")
        .def("set_aggregation_window", &nlm::LogAggregator::setAggregationWindow,
             py::arg("window_seconds"), "Set aggregation window in seconds")
        .def("set_max_batch_size", &nlm::LogAggregator::setMaxBatchSize,
             py::arg("size"), "Set maximum batch size")
        .def("set_enable_metrics_aggregation", &nlm::LogAggregator::setEnableMetricsAggregation,
             py::arg("enabled"), "Enable/disable metrics aggregation")
        .def("flush_aggregates", &nlm::LogAggregator::flushAggregates,
             "Flush all aggregated entries")
        .def("clear_aggregates", &nlm::LogAggregator::clearAggregates,
             "Clear all aggregated entries")
        .def("get_entry_count", &nlm::LogAggregator::getEntryCount,
             "Get number of aggregated entries")
        .def("get_processing_rate", &nlm::LogAggregator::getProcessingRate,
             "Get processing rate (entries per second)")
        .def("set_aggregation_complete_callback", &nlm::LogAggregator::setAggregationCompleteCallback,
             py::arg("callback"), "Set callback for aggregation completion");

    // Log filter
    py::class_<nlm::LogFilter>(m, "LogFilter", R"pbdoc(Log filtering for selective logging)pbdoc")
        .def(py::init<>())
        .def("add_level_filter", &nlm::LogFilter::addLevelFilter,
             py::arg("min_level"), py::arg("max_level"), "Add a level filter")
        .def("add_category_filter", &nlm::LogFilter::addCategoryFilter,
             py::arg("category"), "Add a category filter")
        .def("add_keyword_filter", &nlm::LogFilter::addKeywordFilter,
             py::arg("keyword"), "Add a keyword filter")
        .def("add_metadata_filter", &nlm::LogFilter::addMetadataFilter,
             py::arg("key"), py::arg("value"), "Add a metadata filter")
        .def("add_performance_threshold", &nlm::LogFilter::addPerformanceThreshold,
             py::arg("min_time"), py::arg("max_time"), "Add a performance threshold filter")
        .def("add_metric_threshold", &nlm::LogFilter::addMetricThreshold,
             py::arg("metric_name"), py::arg("min_value"), py::arg("max_value"), "Add a metric threshold filter")
        .def("add_simulation_step_filter", &nlm::LogFilter::addSimulationStepFilter,
             py::arg("min_step"), py::arg("max_step"), "Add a simulation step filter")
        .def("add_agent_filter", &nlm::LogFilter::addAgentFilter,
             py::arg("agent_id"), "Add an agent ID filter")
        .def("add_region_filter", &nlm::LogFilter::addRegionFilter,
             py::arg("region_id"), "Add a region ID filter")
        .def("add_time_filter", &nlm::LogFilter::addTimeFilter,
             py::arg("start_time"), py::arg("end_time"), "Add a time filter")
        .def("clear_filters", &nlm::LogFilter::clearFilters,
             "Clear all filters")
        .def("set_case_insensitive", &nlm::LogFilter::setCaseInsensitive,
             py::arg("case_insensitive"), "Set case-insensitive keyword matching")
        .def("set_regex_matching", &nlm::LogFilter::setRegexMatching,
             py::arg("enabled"), "Enable/disable regex matching");

    // Enhanced DebugDiagnostics
    m.def("generate_debug_report", &nlm::DebugDiagnostics::generateDebugReport,
          py::arg("brain"), py::arg("config"), py::arg("world"),
          py::arg("report_type") = "comprehensive",
          "Generate a comprehensive debug report");
    
    m.def("visualize_neural_connections", &nlm::DebugDiagnostics::visualizeNeuralConnections,
          py::arg("brain"), py::arg("format") = "graphviz",
          "Visualize neural network connections");
    
    m.def("validate_neural_connectivity", &nlm::DebugDiagnostics::validateNeuralConnectivity,
          py::arg("brain"), py::arg("issues"),
          "Validate neural network connectivity and return issues");

    // ===============================================
    // COMPREHENSIVE VISUALIZATION AND EXPORT CAPABILITIES
    // ===============================================

    // Neural activity visualization utilities
    m.def("visualize_spiking_patterns", [](std::shared_ptr<Brain> brain, int window_size = 100, int min_spike_count = 5) {
        return nlm::DebugDiagnostics::visualizeSpikingPatterns(*brain, window_size, min_spike_count);
    }, py::arg("brain"), py::arg("window_size") = 100, py::arg("min_spike_count") = 5,
       "Visualize neural spiking patterns with timing and statistics");

    m.def("plot_firing_rates", [](std::shared_ptr<Brain> brain, int time_window = 0) {
        return nlm::DebugDiagnostics::plotFiringRates(*brain, time_window);
    }, py::arg("brain"), py::arg("time_window") = 0,
       "Plot neural firing rates over time with comprehensive statistics");

    m.def("visualize_activity_heatmap", [](std::shared_ptr<Brain> brain, const std::vector<RegionId>& region_focus = {}) {
        return nlm::DebugDiagnostics::visualizeActivityHeatmap(*brain, region_focus);
    }, py::arg("brain"), py::arg("region_focus") = std::vector<RegionId>(),
       "Create heatmap visualization of neural activity across brain regions");

    m.def("visualize_time_series", [](std::shared_ptr<Brain> brain, const std::string& metric = "spiking_rate", 
                                  const std::string& time_window = "all") {
        return nlm::DebugDiagnostics::visualizeTimeSeries(*brain, metric, time_window);
    }, py::arg("brain"), py::arg("metric") = "spiking_rate", py::arg("time_window") = "all",
       "Generate time series visualization of neural activity metrics");

    m.def("visualize_network_topology", [](std::shared_ptr<Brain> brain, const std::string& layout = "spring") {
        return nlm::DebugDiagnostics::visualizeNetworkTopology(*brain, layout);
    }, py::arg("brain"), py::arg("layout") = "spring",
       "Visualize neural network topology with connection graphs and brain regions");

    m.def("visualize_memory_systems", [](std::shared_ptr<Brain> brain, const std::string& memory_type = "all") {
        return nlm::DebugDiagnostics::visualizeMemorySystems(*brain, memory_type);
    }, py::arg("brain"), py::arg("memory_type") = "all",
       "Visualize memory system activity (working memory, episodic memory)");

    m.def("visualize_development_trajectory", [](std::shared_ptr<Brain> brain, int window = 10) {
        return nlm::DebugDiagnostics::visualizeDevelopmentTrajectory(*brain, window);
    }, py::arg("brain"), py::arg("window") = 10,
       "Visualize developmental trajectory across stages");

    m.def("visualize_neuromodulation", [](std::shared_ptr<Brain> brain, const std::string& neuromodulator = "all") {
        return nlm::DebugDiagnostics::visualizeNeuromodulation(*brain, neuromodulator);
    }, py::arg("brain"), py::arg("neuromodulator") = "all",
       "Visualize neuromodulation levels (dopamine, curiosity, novelty)");

    // Advanced analysis visualizations
    m.def("plot_network_analysis", [](std::shared_ptr<Brain> brain, const std::string& analysis_type = "community") {
        return nlm::DebugDiagnostics::plotNetworkAnalysis(*brain, analysis_type);
    }, py::arg("brain"), py::arg("analysis_type") = "community",
       "Generate network analysis visualization with community detection");

    m.def("plot_connectivity_correlation", [](std::shared_ptr<Brain> brain, const std::string& correlation_type = "pearson") {
        return nlm::DebugDiagnostics::plotConnectivityCorrelation(*brain, correlation_type);
    }, py::arg("brain"), py::arg("correlation_type") = "pearson",
       "Generate correlation visualization for neural connectivity patterns");

    m.def("plot_statistical_analysis", [](std::shared_ptr<Brain> brain, const std::string& analysis_type = "basic") {
        return nlm::DebugDiagnostics::plotStatisticalAnalysis(*brain, analysis_type);
    }, py::arg("brain"), py::arg("analysis_type") = "basic",
       "Generate statistical visualization for experimental results");

    // Neural data export capabilities
    m.def("export_neural_data", [](std::shared_ptr<Brain> brain, const std::string& output_format = "csv", 
                                 const std::string& filename = "") {
        return nlm::DebugDiagnostics::exportNeuralData(*brain, output_format, filename);
    }, py::arg("brain"), py::arg("output_format") = "csv", py::arg("filename") = "",
       "Export neural data (spiking data, connectivity, activity patterns)");

    m.def("export_configuration", [](std::shared_ptr<Brain> brain, const std::string& config_format = "json") {
        return nlm::DebugDiagnostics::exportConfiguration(*brain, config_format);
    }, py::arg("brain"), py::arg("config_format") = "json",
       "Export brain configuration and experiment settings");

    m.def("export_results", [](std::shared_ptr<Brain> brain, const std::string& result_format = "json") {
        return nlm::DebugDiagnostics::exportResults(*brain, result_format);
    }, py::arg("brain"), py::arg("result_format") = "json",
       "Export simulation results and performance metrics");

    m.def("export_visualization", [](std::shared_ptr<Brain> brain, const std::string& viz_format = "png",
                                   const std::string& filename = "") {
        return nlm::DebugDiagnostics::exportVisualization(*brain, viz_format, filename);
    }, py::arg("brain"), py::arg("viz_format") = "png", py::arg("filename") = "",
       "Export visualizations (plots, charts, graphs) in various formats");

    // Interactive visualization capabilities
    m.def("generate_interactive_plot", [](std::shared_ptr<Brain> brain, const std::string& plot_type = "interactive") {
        return nlm::DebugDiagnostics::generateInteractivePlot(*brain, plot_type);
    }, py::arg("brain"), py::arg("plot_type") = "interactive",
       "Generate web-based interactive visualization using WebAssembly");

    m.def("generate_jupyter_notebook", [](std::shared_ptr<Brain> brain) {
        return nlm::DebugDiagnostics::generateJupyterNotebook(*brain);
    }, py::arg("brain"),
       "Generate Jupyter notebook integration with live plotting capabilities");

    m.def("create_3d_visualization", [](std::shared_ptr<Brain> brain, const std::string& projection = " isometric") {
        return nlm::DebugDiagnostics::create3DVisualization(*brain, projection);
    }, py::arg("brain"), py::arg("projection") = "isometric",
       "Create 3D neural network visualization with multiple coordinate systems");

    m.def("create_animated_simulation", [](std::shared_ptr<Brain> brain, int fps = 30, const std::string& output_format = "gif") {
        return nlm::DebugDiagnostics::createAnimatedSimulation(*brain, fps, output_format);
    }, py::arg("brain"), py::arg("fps") = 30, py::arg("output_format") = "gif",
       "Create animated simulation with frame-by-frame capture");

    // Export format support
    m.def("export_to_formats", [](std::shared_ptr<Brain> brain, const std::vector<std::string>& formats) {
        return nlm::DebugDiagnostics::exportToMultipleFormats(*brain, formats);
    }, py::arg("brain"), py::arg("formats") = std::vector<std::string>(),
       "Export to multiple formats: image (PNG, JPEG, SVG, PDF), text (CSV, JSON, TXT), statistical (Excel, RData), web (HTML, JavaScript), archive (ZIP, TAR.GZ)");

    m.def("export_to_single_format", [](std::shared_ptr<Brain> brain, const std::string& format, const std::string& filename = "") {
        return nlm::DebugDiagnostics::exportToSingleFormat(*brain, format, filename);
    }, py::arg("brain"), py::arg("format"), py::arg("filename") = "",
       "Export to a single format with filename support");

    m.def("export_to_archive", [](std::shared_ptr<Brain> brain, const std::string& archive_format = "zip", const std::string& filename = "") {
        return nlm::DebugDiagnostics::exportToArchive(*brain, archive_format, filename);
    }, py::arg("brain"), py::arg("archive_format") = "zip", py::arg("filename") = "",
       "Export to archive formats (ZIP, TAR.GZ) for research paper submissions");

    // Advanced visualization utilities
    m.def("create_publication_quality_plot", [](std::shared_ptr<Brain> brain, const std::string& plot_type = "publication", const std::string& style = "scientific") {
        return nlm::DebugDiagnostics::createPublicationQualityPlot(*brain, plot_type, style);
    }, py::arg("brain"), py::arg("plot_type") = "publication", py::arg("style") = "scientific",
       "Create publication-quality visualization with custom theming and annotation capabilities");

    m.def("create_custom_visualization", [](std::shared_ptr<Brain> brain, const std::string& customization_config = "") {
        return nlm::DebugDiagnostics::createCustomVisualization(*brain, customization_config);
    }, py::arg("brain"), py::arg("customization_config") = "",
       "Create custom visualization with theming and annotation capabilities");

    m.def("generate_research_report", [](std::shared_ptr<Brain> brain, const std::string& report_type = "comprehensive") {
        return nlm::DebugDiagnostics::generateResearchReport(*brain, report_type);
    }, py::arg("brain"), py::arg("report_type") = "comprehensive",
       "Generate automated research report with integrated visualizations and export templates");

    m.def("create_research_templates", [](const std::string& template_type = "default") {
        return nlm::DebugDiagnostics::createResearchTemplates(template_type);
    }, py::arg("template_type") = "default",
       "Get export templates for common research formats and publication workflows");

    m.def("create_analysis_dashboard", [](std::shared_ptr<Brain> brain, const std::string& dashboard_type = "comprehensive") {
        return nlm::DebugDiagnostics::createAnalysisDashboard(*brain, dashboard_type);
    }, py::arg("brain"), py::arg("dashboard_type") = "comprehensive",
       "Create interactive analysis dashboard with real-time visualization for ongoing simulations");

    m.def("export_multiple_formats", [](std::shared_ptr<Brain> brain, const std::vector<std::string>& formats, 
                                      const std::string& base_filename = "nlm_output") {
        return nlm::DebugDiagnostics::exportMultipleFormats(*brain, formats, base_filename);
    }, py::arg("brain"), py::arg("formats"), py::arg("base_filename") = "nlm_output",
       "Export results in multiple formats simultaneously for research reproducibility");

    m.def("visualize_with_annotations", [](std::shared_ptr<Brain> brain, const std::string& annotation_config = "") {
        return nlm::DebugDiagnostics::visualizeWithAnnotations(*brain, annotation_config);
    }, py::arg("brain"), py::arg("annotation_config") = "",
       "Create visualizations with annotation and labeling capabilities for research analysis");

    m.def("integrate_with_analysis_workflows", [](std::shared_ptr<Brain> brain, const std::string& workflow_type = "standard") {
        return nlm::DebugDiagnostics::integrateWithAnalysisWorkflows(*brain, workflow_type);
    }, py::arg("brain"), py::arg("workflow_type") = "standard",
       "Integrate visualizations with common data analysis workflows and pipelines");

    m.def("create_coordinate_system_visualization", [](std::shared_ptr<Brain> brain, const std::string& coordinate_system = "standard") {
        return nlm::DebugDiagnostics::createCoordinateSystemVisualization(*brain, coordinate_system);
    }, py::arg("brain"), py::arg("coordinate_system") = "standard",
       "Create visualization supporting multiple coordinate systems for neural data");

    m.def("create_real_time_visualization", [](std::shared_ptr<Brain> brain, double update_interval = 1.0) {
        return nlm::DebugDiagnostics::createRealTimeVisualization(*brain, update_interval);
    }, py::arg("brain"), py::arg("update_interval") = 1.0,
       "Create real-time visualization for ongoing simulations with continuous updates");
    
    m.def("inspect_brain_state", &nlm::DebugDiagnostics::inspectBrainState,
          py::arg("brain"), py::arg("focus_areas") = std::vector<std::string>(),
          "Inspect brain state for debugging");
    
    m.def("inspect_agent_state", &nlm::DebugDiagnostics::inspectAgentState,
          py::arg("agent"), py::arg("world"),
          "Inspect agent state for debugging");
    
    m.def("inspect_world_state", &nlm::DebugDiagnostics::inspectWorldState,
          py::arg("world"), "Inspect world state for debugging");
    
    m.def("analyze_memory_usage", &nlm::DebugDiagnostics::analyzeMemoryUsage,
          py::arg("brain"), "Analyze memory usage in brain");
    
    m.def("detect_memory_leaks_detailed", &nlm::DebugDiagnostics::detectMemoryLeaksDetailed,
          py::arg("brain"), py::arg("leak_details"),
          "Detect memory leaks with detailed information");
    
    m.def("get_memory_allocation_history", &nlm::DebugDiagnostics::getMemoryAllocationHistory,
          py::arg("brain"), "Get memory allocation history");
    
    m.def("profile_neural_computation", &nlm::DebugDiagnostics::profileNeuralComputation,
          py::arg("brain"), py::arg("region_id") = nlm::INVALID_REGION_ID,
          "Profile neural computation performance");
    
    m.def("analyze_firing_patterns", &nlm::DebugDiagnostics::analyzeFiringPatterns,
          py::arg("brain"), py::arg("start"), py::arg("end"),
          "Analyze neural firing patterns");
    
    m.def("profile_memory_access_patterns", &nlm::DebugDiagnostics::profileMemoryAccessPatterns,
          py::arg("brain"), "Profile memory access patterns");
    
    m.def("generate_detailed_error_trace", &nlm::DebugDiagnostics::generateDetailedErrorTrace,
          py::arg("error"), "Generate detailed error trace");
    
    m.def("trace_error_propagation", &nlm::DebugDiagnostics::traceErrorPropagation,
          py::arg("brain"), py::arg("error"), "Trace error propagation through system");
    
    m.def("get_real_time_metrics", &nlm::DebugDiagnostics::getRealTimeMetrics,
          py::arg("brain"), py::arg("world"), py::arg("metric_types") = std::vector<std::string>(),
          "Get real-time system metrics");
    
    m.def("monitor_resource_usage", &nlm::DebugDiagnostics::monitorResourceUsage,
          py::arg("brain"), py::arg("resource_usage"),
          "Monitor resource usage of brain");
    
    m.def("monitor_simulation_progress", &nlm::DebugDiagnostics::monitorSimulationProgress,
          py::arg("brain"), py::arg("world"), py::arg("current_step"),
          "Monitor simulation progress");
    
    m.def("perform_health_check", &nlm::DebugDiagnostics::performHealthCheck,
          py::arg("brain"), py::arg("config"), py::arg("world"), py::arg("health_issues"),
          "Perform health check on brain, config, and world");
    
    m.def("generate_health_report", &nlm::DebugDiagnostics::generateHealthReport,
          py::arg("brain"), py::arg("config"), py::arg("world"),
          "Generate a comprehensive health report");
    
    m.def("analyze_neural_activity_patterns", &nlm::DebugDiagnostics::analyzeNeuralActivityPatterns,
          py::arg("brain"), "Analyze neural activity patterns");
    
    m.def("analyze_memory_usage_patterns", &nlm::DebugDiagnostics::analyzeMemoryUsagePatterns,
          py::arg("brain"), "Analyze memory usage patterns");
    
    m.def("analyze_learning_trajectories", &nlm::DebugDiagnostics::analyzeLearningTrajectories,
          py::arg("brain"), py::arg("learning_metrics"),
          "Analyze learning trajectories");
    
    m.def("get_optimization_recommendations", &nlm::DebugDiagnostics::getOptimizationRecommendations,
          py::arg("brain"), py::arg("config"), py::arg("world"),
          "Get optimization recommendations for brain performance");
    
    m.def("enable_logging", &nlm::DebugDiagnostics::enableLogging,
          py::arg("log_level") = "debug", py::arg("log_format") = "json",
          py::arg("log_output") = "file",
          "Enable comprehensive logging system");
    
    m.def("analyze_log_files", &nlm::DebugDiagnostics::analyzeLogFiles,
          py::arg("log_directory"), py::arg("analysis_type") = "trends",
          "Analyze log files for patterns and trends");
    
    m.def("correlate_system_logs", &nlm::DebugDiagnostics::correlateSystemLogs,
          py::arg("brain"), py::arg("world"), py::arg("log_sources"),
          "Correlate system logs with brain and world state");
    
    m.def("activate_debug_mode", &nlm::DebugDiagnostics::activateDebugMode,
          py::arg("debug_level") = "full", "Activate debug mode");
    
    m.def("deactivate_debug_mode", &nlm::DebugDiagnostics::deactivateDebugMode,
          "Deactivate debug mode");
    
    m.def("is_debug_mode_active", &nlm::DebugDiagnostics::isDebugModeActive,
          "Check if debug mode is active");
    
    m.def("debug_simulation_step", &nlm::DebugDiagnostics::debugSimulationStep,
          py::arg("brain"), py::arg("world"), py::arg("step"),
          py::arg("debug_details") = std::vector<std::string>(),
          "Debug a specific simulation step");
    
    m.def("trace_neural_event", &nlm::DebugDiagnostics::traceNeuralEvent,
          py::arg("brain"), py::arg("neuron_id"), py::arg("timestamp"),
          "Trace a specific neural event");
    
    m.def("debug_configuration", &nlm::DebugDiagnostics::debugConfiguration,
          py::arg("config"), py::arg("config_keys") = std::vector<std::string>(),
          "Debug configuration settings");
    
    m.def("analyze_performance_bottlenecks", &nlm::DebugDiagnostics::analyzePerformanceBottlenecks,
          py::arg("brain"), py::arg("bottleneck_types"),
          "Analyze performance bottlenecks");
    
    m.def("detect_memory_leaks_comprehensive", &nlm::DebugDiagnostics::detectMemoryLeaksComprehensive,
          py::arg("brain"), py::arg("leak_analysis"),
          "Perform comprehensive memory leak detection");
    
    m.def("debug_error_recovery", &nlm::DebugDiagnostics::debugErrorRecovery,
          py::arg("brain"), py::arg("error_context"),
          "Debug error recovery processes");
    
    m.def("generate_development_report", &nlm::DebugDiagnostics::generateDevelopmentReport,
          py::arg("brain"), py::arg("config"), py::arg("world"),
          "Generate a development report");
    
    m.def("generate_visualization_data", &nlm::DebugDiagnostics::generateVisualizationData,
          py::arg("brain"), py::arg("visualization_type") = "network",
          "Generate visualization data for brain");
    
    m.def("export_debug_data", &nlm::DebugDiagnostics::exportDebugData,
          py::arg("brain"), py::arg("config"), py::arg("world"),
          py::arg("output_path"),
          "Export debug data to files");

    // Factory functions for new logging utilities
    m.def("create_console_logger_sink", []() { return std::make_shared<nlm::ConsoleLoggerSink>(); },
          "Create a new console logger sink");
    
    m.def("create_file_logger_sink", [](const std::string& basePath, const std::string& filenamePrefix) {
        return std::make_shared<nlm::FileLoggerSink>(basePath, filenamePrefix);
    }, py::arg("base_path"), py::arg("filename_prefix") = "nlm",
          "Create a new file logger sink");
    
    m.def("create_remote_logger_sink", [](const std::string& endpoint, const std::string& protocol) {
        return std::make_shared<nlm::RemoteLoggerSink>(endpoint, protocol);
    }, py::arg("endpoint"), py::arg("protocol") = "udp",
          "Create a new remote logger sink");
    
    m.def("create_log_aggregator", []() { return std::make_shared<nlm::LogAggregator>(); },
          "Create a new log aggregator");
    
    m.def("create_log_filter", []() { return std::make_shared<nlm::LogFilter>(); },
          "Create a new log filter");

    // Type information
    m.attr("LOG_TRACE") = nlm::LogLevel::Trace;
    m.attr("LOG_DEBUG") = nlm::LogLevel::Debug;
    m.attr("LOG_INFO") = nlm::LogLevel::Info;
    m.attr("LOG_WARNING") = nlm::LogLevel::Warning;
    m.attr("LOG_ERROR") = nlm::LogLevel::Error;
    m.attr("LOG_CRITICAL") = nlm::LogLevel::Critical;
    m.attr("LOG_OFF") = nlm::LogLevel::Off;
    
    m.attr("LOG_CATEGORY_SYSTEM") = nlm::LogCategory::System;
    m.attr("LOG_CATEGORY_NEURAL") = nlm::LogCategory::Neural;
    m.attr("LOG_CATEGORY_SIMULATION") = nlm::LogCategory::Simulation;
    m.attr("LOG_CATEGORY_PERFORMANCE") = nlm::LogCategory::Performance;
    m.attr("LOG_CATEGORY_MEMORY") = nlm::LogCategory::Memory;
    m.attr("LOG_CATEGORY_CONFIGURATION") = nlm::LogCategory::Configuration;
    m.attr("LOG_CATEGORY_VALIDATION") = nlm::LogCategory::Validation;
    m.attr("LOG_CATEGORY_WORLD") = nlm::LogCategory::World;
    m.attr("LOG_CATEGORY_AGENT") = nlm::LogCategory::Agent;
    m.attr("LOG_CATEGORY_DEVELOPMENT") = nlm::LogCategory::Development;
    
    m.attr("LOG_FORMAT_PLAINTEXT") = nlm::LogFormat::PlainText;
    m.attr("LOG_FORMAT_JSON") = nlm::LogFormat::JSON;
    m.attr("LOG_FORMAT_CSV") = nlm::LogFormat::CSV;
    m.attr("LOG_FORMAT_XML") = nlm::LogFormat::XML;
    m.attr("LOG_FORMAT_HTML") = nlm::LogFormat::HTML;
    
    m.attr("AGGREGATION_NONE") = nlm::AggregationStrategy::None;
    m.attr("AGGREGATION_PERIODIC") = nlm::AggregationStrategy::Periodic;
    m.attr("AGGREGATION_SIZE_BASED") = nlm::AggregationStrategy::SizeBased;
    m.attr("AGGREGATION_TIME_BASED") = nlm::AggregationStrategy::TimeBased;
    m.attr("AGGREGATION_EVENT_BASED") = nlm::AggregationStrategy::EventBased;
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

    // Type information
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===============================================
    // BATCH PROCESSING UTILITIES
    // ===============================================

    // Batch Progress
    py::class_<BatchProgress>(m, "BatchProgress")
        .def(py::init<>())
        .def(py::init<size_t>())
        .def_readwrite("current_task", &BatchProgress::currentTask)
        .def_readwrite("total_tasks", &BatchProgress::totalTasks)
        .def_readwrite("completed_tasks", &BatchProgress::completedTasks)
        .def_readwrite("failed_tasks", &BatchProgress::failedTasks)
        .def("get_progress", &BatchProgress::getProgress)
        .def("get_status_string", &BatchProgress::getStatusString);

    // Batch Simulation Result
    py::class_<BatchSimulationResult>(m, "BatchSimulationResult")
        .def(py::init<>())
        .def_readwrite("batch_index", &BatchSimulationResult::batchIndex)
        .def_readwrite("config_name", &BatchSimulationResult::configName)
        .def_readwrite("execution_time", &BatchSimulationResult::executionTime)
        .def_readwrite("status", &BatchSimulationResult::status)
        .def_readwrite("error_message", &BatchSimulationResult::errorMessage)
        .def_readwrite("metrics", &BatchSimulationResult::metrics);

    // Batch Configuration Generator
    py::class_<BatchConfigGenerator>(m, "BatchConfigGenerator")
        .def_static("generate_parameter_grid", &BatchConfigGenerator::generateParameterGrid,
                   py::arg("sweeps"), "Generate parameter grid from multiple parameter configurations")
        .def_static("generate_cross_validation_folds", &BatchConfigGenerator::generateCrossValidationFolds,
                   py::arg("total_samples"), py::arg("num_folds"), py::arg("shuffle") = true,
                   "Generate cross-validation folds")
        .def_static("create_batch_configurations", &BatchConfigGenerator::createBatchConfigurations,
                   py::arg("parameter_grid"), "Create batch configurations from parameter grid");

    // Batch Simulation Runner
    py::class_<BatchSimulationRunner>(m, "BatchSimulationRunner")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<Environment>, BatchProgress&>())
        .def("run_single_simulation", &BatchSimulationRunner::runSingleSimulation,
             py::arg("batch_index"), py::arg("config"), py::arg("timesteps") = 100,
             "Run batch simulation with single configuration")
        .def("run_parallel_batch", &BatchSimulationRunner::runParallelBatch,
             py::arg("configs"), py::arg("timesteps") = 100, py::arg("max_threads") = 0,
             "Run batch of simulations in parallel")
        .def("run_sequential_batch", &BatchSimulationRunner::runSequentialBatch,
             py::arg("configs"), py::arg("timesteps") = 100,
             "Run batch of simulations sequentially")
        .def("get_batch_performance_metrics", &BatchSimulationRunner::getBatchPerformanceMetrics,
             py::arg("results"), "Get performance metrics for a batch");

    // Cross-Validation Utilities
    py::class_<CrossValidationUtilities>(m, "CrossValidationUtilities")
        .def_static("cross_validate", &CrossValidationUtilities::crossValidate,
                   py::arg("brain"), py::arg("environment"), py::arg("folds"),
                   py::arg("timesteps_per_fold") = 50,
                   "Perform k-fold cross-validation on brain simulations")
        .def_static("stratified_folds", &CrossValidationUtilities::stratifiedFolds,
                   py::arg("total_samples"), py::arg("num_folds"), py::arg("labels"),
                   "Generate stratified folds for balanced sampling");

    // ===============================================
    // PARALLEL EXECUTION CAPABILITIES
    // ===============================================

    // Shared Memory Pool
    py::class_<SharedMemoryPool>(m, "SharedMemoryPool")
        .def(py::init<size_t>())
        .def("allocate", &SharedMemoryPool::allocate,
             py::arg("size"), py::arg("owner"),
             "Allocate memory block for thread")
        .def("deallocate", &SharedMemoryPool::deallocate,
             py::arg("block"), py::arg("owner"),
             "Release memory block")
        .def("get_total_memory", &SharedMemoryPool::getTotalMemory,
             "Get total available memory")
        .def("get_free_memory", &SharedMemoryPool::getFreeMemory,
             "Get free memory");

    // Thread-Safe Config Manager
    py::class_<ThreadSafeConfigManager>(m, "ThreadSafeConfigManager")
        .def(py::init<std::shared_ptr<Config>>())
        .def("get_thread_config", &ThreadSafeConfigManager::getThreadConfig,
             py::arg("thread_id"), "Get thread-local configuration")
        .def("update_global_config", &ThreadSafeConfigManager::updateGlobalConfig,
             py::arg("updates"), "Update configuration for all threads")
        .def("get_global_config", &ThreadSafeConfigManager::getGlobalConfig,
             "Get global configuration")
        .def("reset_config", &ThreadSafeConfigManager::resetConfig,
             "Reset configuration");

    // Parallel Simulation Runner
    py::class_<ParallelSimulationRunner>(m, "ParallelSimulationRunner")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<Environment>, std::shared_ptr<SharedMemoryPool>>())
        .def("run_parallel_sweep", &ParallelSimulationRunner::runParallelSweep,
             py::arg("parameter_sets"), py::arg("param_key"), py::arg("timesteps") = 100,
             py::arg("max_threads") = 0, py::arg("use_shared_memory") = false,
             "Run parallel simulation with parameter sweep")
        .def("run_parallel_batch", &ParallelSimulationRunner::runParallelBatch,
             py::arg("configs"), py::arg("timesteps") = 100, py::arg("max_threads") = 0,
             py::arg("use_shared_memory") = false,
             "Run parallel batch simulations")
        .def("set_config_manager", &ParallelSimulationRunner::setConfigManager,
             py::arg("config_manager"), "Set thread-safe config manager")
        .def("get_config_manager", &ParallelSimulationRunner::getConfigManager,
             "Get current thread-safe config manager");

    // Parallel Parameter Sweep
    py::class_<ParallelParameterSweep>(m, "ParallelParameterSweep")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<Environment>, size_t>())
        .def("sweep_parameters", &ParallelParameterSweep::sweepParameters,
             py::arg("parameter_sets"), py::arg("parameter_names"), py::arg("timesteps") = 100,
             py::arg("output_metric") = "performance",
             "Perform parallel parameter sweep with dynamic work distribution")
        .def("get_completed_jobs", &ParallelParameterSweep::getCompletedJobs,
             "Get number of completed jobs")
        .def("get_total_jobs", &ParallelParameterSweep::getTotalJobs,
             "Get total number of jobs");

    // ===============================================
    // PERFORMANCE MONITORING UTILITIES
    // ===============================================

    // Performance Monitor
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
        .def(py::init<>())
        .def("start_task", &PerformanceMonitor::startTask,
             py::arg("task_name"), "Start monitoring a task")
        .def("end_task", &PerformanceMonitor::endTask,
             py::arg("task_name"), "End monitoring a task")
        .def("get_batch_metrics", &PerformanceMonitor::getBatchMetrics,
             py::arg("results"), "Get performance metrics for batch")
        .def("get_overall_metrics", &PerformanceMonitor::getOverallMetrics,
             "Get overall performance summary")
        .def("generate_optimization_recommendations", &PerformanceMonitor::generateOptimizationRecommendations,
             "Generate optimization recommendations")
        .def("reset", &PerformanceMonitor::reset, "Reset all metrics");

    // Resource Monitor
    py::class_<ResourceMonitor>(m, "ResourceMonitor")
        .def(py::init<>())
        .def("start_monitoring", &ResourceMonitor::startMonitoring,
             "Start resource monitoring")
        .def("stop_monitoring", &ResourceMonitor::stopMonitoring,
             "Stop resource monitoring")
        .def("get_current_resource_usage", &ResourceMonitor::getCurrentResourceUsage,
             "Get current resource usage")
        .def("get_resource_history", &ResourceMonitor::getResourceHistory,
             py::arg("max_history") = 1000, "Get historical resource usage")
        .def("is_overloaded", &ResourceMonitor::isOverloaded,
             "Check if resources are overloaded");

    // Execution Tracker
    py::class_<ExecutionTracker>(m, "ExecutionTracker")
        .def(py::init<>())
        .def("record_event", &ExecutionTracker::recordEvent,
             py::arg("event_type"), py::arg("description"),
             py::arg("thread_id") = ThreadPool::getCurrentThreadId(),
             py::arg("metadata") = std::map<std::string, std::string>(),
             "Record an execution event")
        .def("start_timing", &ExecutionTracker::startTiming,
             py::arg("section_name"), py::arg("thread_id") = ThreadPool::getCurrentThreadId(),
             "Start timing a section")
        .def("end_timing", &ExecutionTracker::endTiming,
             py::arg("section_name"), py::arg("thread_id") = ThreadPool::getCurrentThreadId(),
             "End timing a section")
        .def("get_execution_summary", &ExecutionTracker::getExecutionSummary,
             py::arg("results"), "Get execution summary for batch")
        .def("generate_optimization_recommendations", &ExecutionTracker::generateOptimizationRecommendations,
             "Generate optimization recommendations based on timing data")
        .def("clear_event_history", &ExecutionTracker::clearEventHistory,
             "Clear event history");

    // Memory Monitor
    py::class_<MemoryMonitor>(m, "MemoryMonitor")
        .def(py::init<>())
        .def("get_current_memory_stats", &MemoryMonitor::getCurrentMemoryStats,
             "Get current memory statistics")
        .def("get_memory_history", &MemoryMonitor::getMemoryHistory,
             py::arg("max_history") = 1000, "Get historical memory usage")
        .def("detect_memory_leak", &MemoryMonitor::detectMemoryLeak,
             py::arg("sample_size") = 10, "Check for memory leaks")
        .def("get_memory_usage_trend", &MemoryMonitor::getMemoryUsageTrend,
             py::arg("window_size") = 10, "Get memory usage trend");

    // ===============================================
    // ADVANCED EXPERIMENT UTILITIES
    // ===============================================

    // Experiment Result Aggregator
    py::class_<ExperimentResultAggregator>(m, "ExperimentResultAggregator")
        .def_static("aggregate_batch_results", &ExperimentResultAggregator::aggregateBatchResults,
                   py::arg("results"), "Aggregate batch simulation results")
        .def_static("perform_statistical_analysis", &ExperimentResultAggregator::performStatisticalAnalysis,
                   py::arg("data"), py::arg("analysis_type") = "basic",
                   "Perform statistical analysis on results")
        .def_static("generate_result_summary", &ExperimentResultAggregator::generateResultSummary,
                   py::arg("aggregated_results"), "Generate result summary report");

    // Result Comparator
    py::class_<ResultComparator>(m, "ResultComparator")
        .def_static("compare_experiments", &ResultComparator::compareExperiments,
                   py::arg("baseline"), py::arg("current"),
                   "Compare two experiment results")
        .def_static("generate_visualization_data", &ResultComparator::generateVisualizationData,
                   py::arg("comparison"), py::arg("chart_type") = "bar",
                   "Generate comparison visualization data");

    // Factory functions for new utilities
    m.def("createBatchProgress", []() { return BatchProgress(); },
          "Create a new batch progress tracker");

    m.def("createPerformanceMonitor", []() { return PerformanceMonitor(); },
          "Create a new performance monitor");

    m.def("createResourceMonitor", []() { return ResourceMonitor(); },
          "Create a new resource monitor");

    m.def("createExecutionTracker", []() { return ExecutionTracker(); },
          "Create a new execution tracker");

    m.def("createMemoryMonitor", []() { return MemoryMonitor(); },
          "Create a new memory monitor");

    // Type information
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

} // namespace nlm

} // namespace nlm

} // namespace nlm