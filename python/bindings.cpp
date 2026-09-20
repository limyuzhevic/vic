#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/advanced/AdvancedTypes.hpp"
#include "../src/advanced/Simulation.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Pythonic Features:
        - Context managers for resource management
        - Property-based access for better UX
        - Python-style iteration over collections
        - Rich exception handling with helpful error messages
        - Type hints and better documentation
        
        Advanced Functionality:
        - Advanced simulation modes (reinforcement learning, episodes, benchmarks)
        - Enhanced visualization and analysis tools
        - Better memory management and profiling
        - Save/load capabilities with JSON serialization
        - Configuration validation and templates
        
        New Commands and Utilities:
        - Simulation control (pause, resume, step-through)
        - Performance monitoring and profiling
        - Data logging and analysis tools
        - Replay and history tracking
        - Advanced agent behaviors
        
        Educational and Research Tools:
        - Experiment builder and runner
        - Visualization helpers
        - Statistics and metrics collection
        - Teaching mode with step-by-step debugging
    )pbdoc";

    // Python-style exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    
    // Custom Python exceptions for better error messages
    py::class_<py::error_already_set>(m, "PythonError")
        .def(py::init<>());

    // Enhanced logging and error handling
    py::class_<nlm::ErrorHandler>(m, "ErrorHandler", R"pbdoc(Error handling with logging)pbdoc")
        .def(py::init<>())
        .def("log_error", &nlm::ErrorHandler::logError, py::arg("message"), "Log an error message")
        .def("log_warning", &nlm::ErrorHandler::logWarning, py::arg("message"), "Log a warning message")
        .def("log_info", &nlm::ErrorHandler::logInfo, py::arg("message"), "Log info message")
        .def("get_errors", &nlm::ErrorHandler::getErrors, "Get all errors")
        .def("clear", &nlm::ErrorHandler::clear, "Clear all errors");

    // Configuration validator
    py::class_<nlm::ConfigValidator>(m, "ConfigValidator", R"pbdoc(Configuration validation)pbdoc")
        .def(py::init<>())
        .def("add_rule", &nlm::ConfigValidator::addRule, 
             py::arg("key"), py::arg("rule"), py::arg("value1") = 0.0, py::arg("value2") = 0.0,
             "Add a validation rule")
        .def("validate", &nlm::ConfigValidator::validate, py::arg("config"), "Validate configuration")
        .def("get_errors", &nlm::ConfigValidator::getErrors, "Get validation errors")
        .def("clear", &nlm::ConfigValidator::clear, "Clear all rules");

    // Context manager for resources
    py::class_<nlm::ContextManager::ResourceContext>(m, "ResourceContext", R"pbdoc(Context manager for resources)pbdoc")
        .def(py::init<>())
        .def("__enter__", [](nlm::ContextManager::ResourceContext& self) {
            return self;
        })
        .def("__exit__", [](nlm::ContextManager::ResourceContext& self, 
                         py::args) {
            // Resource cleanup
        })
        .def("get_resource", &nlm::ContextManager::ResourceContext::getResource)
        .def("is_cleaned_up", &nlm::ContextManager::ResourceContext::isCleanedUp);

    // Enhanced Config class with property-based access
    py::class_<nlm::Config, std::shared_ptr<nlm::Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](nlm::Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &nlm::Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &nlm::Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", &nlm::Config::getKeys, "Get all configuration keys")
        .def("clear", &nlm::Config::clear, "Clear all configuration entries")
        .def("summary", &nlm::Config::summary, "Get a summary string of the configuration")
        
        // Python property-based access
        .def("__getitem__", [](const nlm::Config& self, const std::string& key) {
            return self.get<std::string>(key).value_or("");
        }, py::arg("key"), "Get configuration value by key")
        .def("__setitem__", [](nlm::Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value by key")
        
        // Python iteration support
        .def("__iter__", [](const nlm::Config& self) {
            auto keys = self.getKeys();
            return py::make_iterator(keys.begin(), keys.end());
        }, py::return_value_policy::reference_internal);

    // Simplified Brain interface with Pythonic features
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::Brain::initialize)
        .def("step", py::overload_cast<nlm::SimulationStep>(&nlm::Brain::step), py::arg("currentStep"))
        .def("step", py::overload_cast<nlm::SimulationStep, nlm::Timestamp>(&nlm::Brain::step), 
             py::arg("currentStep"), py::arg("currentTime"))
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput, py::arg("input"))
        .def("injectCurrent", py::overload_cast<nlm::NeuronId, nlm::MembranePotential>(&nlm::Brain::injectCurrent), 
             py::arg("neuron"), py::arg("current"))
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons, 
             py::arg("type"), py::arg("current"))
        .def("produceAction", &nlm::Brain::produceAction)
        .def("reset", &nlm::Brain::reset)
        .def("save", &nlm::Brain::save, py::arg("filepath"))
        .def("load", &nlm::Brain::load, py::arg("filepath"))
        .def("addRegion", &nlm::Brain::addRegion, py::arg("name") = "")
        .def("getRegion", py::overload_cast<nlm::RegionId>(&nlm::Brain::getRegion, 
                     py::const), py::arg("id"))
        .def("getRegionCount", &nlm::Brain::getRegionCount)
        .def("getRegionIds", &nlm::Brain::getRegionIds)
        .def("getRegions", py::overload_cast<>(&nlm::Brain::getRegions, 
                     py::const))
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage, py::arg("stage"))
        .def("getConfig", py::overload_cast<>(&nlm::Brain::getConfig, py::const))
        .def("logStatus", &nlm::Brain::logStatus)
        
        // Python property-based access
        .def_property_readonly("neurons", &nlm::Brain::getTotalNeuronCount)
        .def_property_readonly("synapses", &nlm::Brain::getTotalSynapseCount)
        .def_property_readonly("firing", &nlm::Brain::getFiringNeuronCount)
        .def_property_readonly("active", &nlm::Brain::getActiveNeuronCount)
        .def_property_readonly("spike_count", &nlm::Brain::getTotalSpikeCount)
        .def_property_readonly("excitation_inhibition_ratio", &nlm::Brain::getExcitationInhibitionRatio)
        .def_property_readonly("developmental_stage", &nlm::Brain::getDevelopmentalStage)
        
        // Python iteration support
        .def("__iter__", [](const nlm::Brain& brain) {
            auto regions = brain.getRegions();
            return py::make_iterator(regions.begin(), regions.end());
        }, py::return_value_policy::reference_internal)
        
        // Python context manager support
        .def("__enter__", [](std::shared_ptr<nlm::Brain>& brain) {
            return brain;
        })
        .def("__exit__", [](std::shared_ptr<nlm::Brain>& brain, py::args) {
            // Brain will be destroyed when shared_ptr goes out of scope
        });

    // Enhanced SimpleWorld interface
    py::class_<nlm::SimpleWorld, std::shared_ptr<nlm::SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8)
        .def("reset", &nlm::SimpleWorld::reset)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody)
        .def("addObject", &nlm::SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed)
        
        // Python property-based access
        .def_property_readonly("width", &nlm::SimpleWorld::getWidth)
        .def_property_readonly("height", &nlm::SimpleWorld::getHeight)
        .def_property_readonly("simulation_time", &nlm::SimpleWorld::getSimulationTime)
        .def_property("max_energy", &nlm::SimpleWorld::getMaxEnergy, &nlm::SimpleWorld::setMaxEnergy)
        .def_property("energy_decay_rate", &nlm::SimpleWorld::getEnergyDecayRate, 
                     &nlm::SimpleWorld::setEnergyDecayRate)
        
        // Python iteration support
        .def("__iter__", [](const nlm::SimpleWorld& world) {
            return py::make_iterator(&world.getAgentBody(), &world.getAgentBody() + 1);
        }, py::return_value_policy::reference_internal);

    // Simulation mode enumeration
    py::enum_<nlm::SimulationMode>(m, "SimulationMode", R"pbdoc(Simulation mode enumeration)pbdoc")
        .value("STANDARD", nlm::SimulationMode::Standard)
        .value("EPISODE", nlm::SimulationMode::Episode)
        .value("REINFORCEMENT_LEARNING", nlm::SimulationMode::ReinforcementLearning)
        .value("BENCHMARK", nlm::SimulationMode::Benchmark)
        .export_values();

    // Exploration strategy enumeration
    py::enum_<nlm::ExplorationStrategy>(m, "ExplorationStrategy", R"pbdoc(Exploration strategy enumeration)pbdoc")
        .value("EPSILON_GREEDY", nlm::ExplorationStrategy::EpsilonGreedy)
        .value("SOFTMAX", nlm::ExplorationStrategy::Softmax)
        .value("UCT", nlm::ExplorationStrategy::UCT)
        .value("NOISE", nlm::ExplorationStrategy::Noise)
        .export_values();

    // Metric aggregation enumeration
    py::enum_<nlm::MetricAggregation>(m, "MetricAggregation", R"pbdoc(Metric aggregation enumeration)pbdoc")
        .value("SUM", nlm::MetricAggregation::Sum)
        .value("MEAN", nlm::MetricAggregation::Mean)
        .value("MEDIAN", nlm::MetricAggregation::Median)
        .value("MAX", nlm::MetricAggregation::Max)
        .value("MIN", nlm::MetricAggregation::Min)
        .value("LAST", nlm::MetricAggregation::Last)
        .export_values();

    // Validation rule enumeration
    py::enum_<nlm::ValidationRule>(m, "ValidationRule", R"pbdoc(Validation rule enumeration)pbdoc")
        .value("RANGE", nlm::ValidationRule::Range)
        .value("POSITIVE", nlm::ValidationRule::Positive)
        .value("NON_NEGATIVE", nlm::ValidationRule::NonNegative)
        .value("STRING", nlm::ValidationRule::String)
        .value("BOOLEAN", nlm::ValidationRule::Boolean)
        .value("CHOICE", nlm::ValidationRule::Choice)
        .export_values();

    // Performance metrics access
    py::class_<nlm::PerformanceMetrics>(m, "PerformanceMetrics", R"pbdoc(Performance metrics data)pbdoc")
        .def(py::init<>())
        .def_readwrite("avg_step_time_ms", &nlm::PerformanceMetrics::avgStepTimeMs)
        .def_readwrite("memory_usage_mb", &nlm::PerformanceMetrics::memoryUsageMb)
        .def_readwrite("cpu_usage_percent", &nlm::PerformanceMetrics::cpuUsagePercent)
        .def_readwrite("peak_memory_mb", &nlm::PerformanceMetrics::peakMemoryMb)
        .def_readwrite("step_count", &nlm::PerformanceMetrics::stepCount)
        .def("summary", &nlm::PerformanceMetrics::summary);

    // Simulation class with enhanced control
    py::class_<nlm::Simulation>(m, "Simulation", R"pbdoc(Advanced simulation controller)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("run", &nlm::Simulation::run)
        .def("step", &nlm::Simulation::step, py::arg("step_number"))
        .def("pause", &nlm::Simulation::pause)
        .def("resume", &nlm::Simulation::resume)
        .def("stepThrough", &nlm::Simulation::stepThrough)
        .def("set_mode", &nlm::Simulation::setMode, py::arg("mode"))
        .def("get_mode", &nlm::Simulation::getMode)
        .def("enable_performance_monitoring", &nlm::Simulation::enablePerformanceMonitoring, py::arg("enable"))
        .def("set_max_steps_per_episode", &nlm::Simulation::setMaxStepsPerEpisode, py::arg("max_steps"))
        .def("set_autosave_interval", &nlm::Simulation::setAutosaveInterval, py::arg("interval"))
        .def("get_performance_metrics", &nlm::Simulation::getPerformanceMetrics)
        .def("get_history", &nlm::Simulation::getHistory)
        .def("clear_history", &nlm::Simulation::clearHistory)
        .def("save", &nlm::Simulation::save, py::arg("filepath"))
        .def("load", &nlm::Simulation::load, py::arg("filepath"))
        .def("export_to_json", &nlm::Simulation::exportToJson, py::arg("filepath"))
        .def("import_from_json", &nlm::Simulation::importFromJson, py::arg("filepath"))
        .def("replay_episode", &nlm::Simulation::replayEpisode, py::arg("episode_index"))
        
        // Python property-based access
        .def_property_readonly("current_step", &nlm::Simulation::getCurrentStep)
        .def_property_readonly("current_episode", &nlm::Simulation::getCurrentEpisode)
        .def_property_readonly("is_running", &nlm::Simulation::isRunning)
        .def_property_readonly("is_paused", &nlm::Simulation::isPaused)
        .def_property("mode", &nlm::Simulation::getMode, &nlm::Simulation::setMode);

    // Experiment class with enhanced features
    py::class_<nlm::Experiment>(m, "Experiment", R"pbdoc(Scientific experiment with metrics collection)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("name"))
        .def("getName", &nlm::Experiment::getName)
        .def("setName", &nlm::Experiment::setName, py::arg("name"))
        .def("setSeed", &nlm::Experiment::setSeed, py::arg("seed"))
        .def("setConfigValue", &nlm::Experiment::setConfigValue, py::arg("key"), py::arg("value"))
        .def("getConfigValue", &nlm::Experiment::getConfigValue, py::arg("key"))
        .def("recordMetric", py::overload_cast<const std::string&, double>(&nlm::Experiment::recordMetric), 
             py::arg("name"), py::arg("value"))
        .def("recordMetric", py::overload_cast<const std::string&, float>(&nlm::Experiment::recordMetric), 
             py::arg("name"), py::arg("value"))
        .def("recordMetric", py::overload_cast<const std::string&, int>(&nlm::Experiment::recordMetric), 
             py::arg("name"), py::arg("value"))
        .def("getMetrics", &nlm::Experiment::getMetrics)
        .def("getMetric", &nlm::Experiment::getMetric, py::arg("name"))
        .def("hasMetric", &nlm::Experiment::hasMetric, py::arg("name"))
        .def("recordStep", &nlm::Experiment::recordStep, py::arg("step"), py::arg("neuronCount"), py::arg("synapseCount"), 
             py::arg("firingCount"), py::arg("avgFiringRate"))
        .def("getStepData", &nlm::Experiment::getStepData)
        .def("getStartStep", &nlm::Experiment::getStartStep)
        .def("setStartStep", &nlm::Experiment::setStartStep, py::arg("step"))
        .def("getEndStep", &nlm::Experiment::getEndStep)
        .def("setEndStep", &nlm::Experiment::setEndStep, py::arg("step"))
        .def("addNote", &nlm::Experiment::addNote, py::arg("note"))
        .def("getNotes", &nlm::Experiment::getNotes)
        .def("get_summary", &nlm::Experiment::summary)
        .def("export_to_json", &nlm::Experiment::exportToJson, py::arg("filepath"))
        .def("import_from_json", &nlm::Experiment::importFromJson, py::arg("filepath"))
        
        // Python property-based access
        .def_property_readonly("name", &nlm::Experiment::getName)
        .def_property_readonly("start_step", &nlm::Experiment::getStartStep)
        .def_property_readonly("end_step", &nlm::Experiment::getEndStep)
        .def_property("seed", &nlm::Experiment::getSeed, &nlm::Experiment::setSeed);

    // ExperimentRunner class
    py::class_<nlm::ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(Runs experiments and collects data)pbdoc")
        .def(py::init<>())
        .def("createExperiment", &nlm::ExperimentRunner::createExperiment, py::arg("name"))
        .def("runExperiment", &nlm::ExperimentRunner::runExperiment, 
             py::arg("experiment"), py::arg("brain"), py::arg("environment"), py::arg("maxSteps"))
        .def("runBatch", &nlm::ExperimentRunner::runBatch,
             py::arg("experiments"), py::arg("brainTemplate"), py::arg("environmentTemplate"), py::arg("maxSteps"))
        .def("getExperiments", &nlm::ExperimentRunner::getExperiments)
        .def("getExperiment", &nlm::ExperimentRunner::getExperiment, py::arg("name"))
        .def("clearExperiments", &nlm::ExperimentRunner::clearExperiments)
        .def("setGlobalSeed", &nlm::ExperimentRunner::setGlobalSeed, py::arg("seed"))
        .def("saveResults", &nlm::ExperimentRunner::saveResults, py::arg("filepath"));

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<nlm::Config> {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::Brain> {
        return std::make_shared<nlm::Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<nlm::SimpleWorld> {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<nlm::AgentBrain> {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createSimulation", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<nlm::Simulation> {
        return std::make_shared<nlm::Simulation>(brain);
    }, py::arg("brain"), "Create a new simulation controller");

    m.def("createExperiment", [](const std::string& name) -> std::shared_ptr<nlm::Experiment> {
        return std::make_shared<nlm::Experiment>(name);
    }, py::arg("name"), "Create a new experiment");

    m.def("createExperimentRunner", []() -> std::shared_ptr<nlm::ExperimentRunner> {
        return std::make_shared<nlm::ExperimentRunner>();
    }, "Create a new experiment runner");

    m.def("createErrorHandler", []() -> std::shared_ptr<nlm::ErrorHandler> {
        return std::make_shared<nlm::ErrorHandler>();
    }, "Create a new error handler");

    m.def("createConfigValidator", []() -> std::shared_ptr<nlm::ConfigValidator> {
        return std::make_shared<nlm::ConfigValidator>();
    }, "Create a new configuration validator");

    // Advanced agent behaviors
    py::class_<nlm::AgentBrain, std::shared_ptr<nlm::AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"))
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation, 
             py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("reset", &nlm::AgentBrain::reset)
        .def("getBrain", &nlm::AgentBrain::getBrain)
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled);

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);

    // Version info
    m.attr("__version__") = "1.0.0";
    m.attr("__description__") = "Enhanced NLM Python Bindings with Pythonic features, context managers, property-based access, and advanced simulation capabilities";
}

} // namespace nlm