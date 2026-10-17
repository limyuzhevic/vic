#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for enhanced features
class ConfigTemplate;
class ParameterSweep;
class PerformanceMonitor;
class StatisticalAnalyzer;
class MemoryManager;
class SimulationController;
class VisualizationManager;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive classes for brain simulation, configuration management,
        agent control, world environments, and advanced neural computation features.
        
        Key features:
        - Enhanced configuration with templates and validation
        - Advanced brain manipulation with neural populations
        - Rich agent system with curiosity-driven exploration
        - Multiple world environments (SimpleWorld, DynamicWorld, etc.)
        - Visualization and debugging tools
        - Performance monitoring and statistical analysis
        - Context managers for automatic resource cleanup
        - Parameter sweeps and experiment automation
        - Reinforcement learning integration
        - Memory management utilities
        - Advanced plasticity control
        
        Usage examples:
        ```python
        import pynlm
        
        # Create brain with enhanced configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        brain = pynlm.createBrain(config)
        
        # Run simulation with monitoring
        with pynlm.SimulationContext(config) as sim:
            brain.initialize()
            for step in range(1000):
                brain.step(step)
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Enhanced Configuration Templates
    py::class_<ConfigTemplate>(m, "ConfigTemplate", R"pbdoc(Neural network configuration template)pbdoc")
        .def(py::init<>())
        .def("createConfig", &ConfigTemplate::createConfig,
             "Create configuration from template")
        .def("validate", &ConfigTemplate::validate,
             "Validate template against configuration")
        .def("getTemplateParameters", &ConfigTemplate::getTemplateParameters,
             "Get template parameters")
        .def("addParameter", &ConfigTemplate::addParameter,
             py::arg("name"), py::arg("value"), py::arg("description"),
             "Add parameter to template")
        .def("setParameterRange", &ConfigTemplate::setParameterRange,
             py::arg("name"), py::arg("min"), py::arg("max"),
             "Set parameter range for sweep");

    // Configuration Validation System
    py::class_<ConfigValidationResult>(m, "ConfigValidationResult", R"pbdoc(Configuration validation result)pbdoc")
        .def_readwrite("is_valid", &ConfigValidationResult::isValid)
        .def_readwrite("errors", &ConfigValidationResult::errors)
        .def_readwrite("warnings", &ConfigValidationResult::warnings)
        .def("getSuggestions", &ConfigValidationResult::getSuggestions);

    // Parameter Sweep System
    py::class_<ParameterSweep>(m, "ParameterSweep", R"pbdoc(Parameter sweep for experiments)pbdoc")
        .def(py::init<>())
        .def("addParameter", &ParameterSweep::addParameter,
             py::arg("name"), py::arg("values"),
             "Add parameter with possible values")
        .def("addContinuousParameter", &ParameterSweep::addContinuousParameter,
             py::arg("name"), py::arg("min"), py::arg("max"), py::arg("steps"),
             "Add continuous parameter for sweep")
        .def("setObjectiveFunction", &ParameterSweep::setObjectiveFunction,
             py::arg("func"),
             "Set objective function for optimization")
        .def("runSweep", &ParameterSweep::runSweep,
             py::arg("steps_per_parameter"), py::arg("parallel"),
             "Run parameter sweep")
        .def("getResults", &ParameterSweep::getResults,
             "Get sweep results")
        .def("getBestParameters", &ParameterSweep::getBestParameters,
             "Get best parameters found")
        .def("saveResults", &ParameterSweep::saveResults,
             py::arg("filename"),
             "Save sweep results to file");

    // Neural Population Management
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population with shared properties)pbdoc")
        .def(py::init<>())
        .def("setType", &NeuralPopulation::setType,
             py::arg("type"), "Set neuron type")
        .def("getType", &NeuralPopulation::getType,
             "Get neuron type")
        .def("setCount", &NeuralPopulation::setCount,
             py::arg("count"), "Set neuron count")
        .def("getCount", &NeuralPopulation::getCount,
             "Get neuron count")
        .def("addSynapticConnection", &NeuralPopulation::addSynapticConnection,
             py::arg("target"), py::arg("weight"),
             "Add synaptic connection to target population")
        .def("getConnections", &NeuralPopulation::getConnections,
             "Get outgoing connections")
        .def("calculateStats", &NeuralPopulation::calculateStats,
             "Calculate population statistics")
        .def("__repr__", &NeuralPopulation::toString,
             "String representation");

    // Enhanced Plasticity Control
    py::class_<PlasticityController>(m, "PlasticityController", R"pbdoc(Advanced plasticity control system)pbdoc")
        .def(py::init<>())
        .def("enableRule", &PlasticityController::enableRule,
             py::arg("rule_type"), py::arg("enabled"),
             "Enable/disable plasticity rule")
        .def("configureRule", &PlasticityController::configureRule,
             py::arg("rule_type"), py::arg("params"),
             "Configure plasticity rule parameters")
        .def("getRuleState", &PlasticityController::getRuleState,
             py::arg("rule_type"),
             "Get plasticity rule state")
        .def("addCustomRule", &PlasticityController::addCustomRule,
             py::arg("name"), py::arg("condition"), py::arg("update"),
             "Add custom plasticity rule")
        .def("getCurrentState", &PlasticityController::getCurrentState,
             "Get current plasticity state");

    // Enhanced Agent Behaviors
    py::class_<AgentBehaviorManager>(m, "AgentBehaviorManager", R"pbdoc(Advanced agent behavior management)pbdoc")
        .def(py::init<>())
        .def("enableBehavior", &AgentBehaviorManager::enableBehavior,
             py::arg("behavior_type"), py::arg("params"),
             "Enable agent behavior with parameters")
        .def("disableBehavior", &AgentBehaviorManager::disableBehavior,
             py::arg("behavior_type"),
             "Disable agent behavior")
        .def("configureBehavior", &AgentBehaviorManager::configureBehavior,
             py::arg("behavior_type"), py::arg("config"),
             "Configure agent behavior configuration")
        .def("getBehaviorState", &AgentBehaviorManager::getBehaviorState,
             py::arg("behavior_type"),
             "Get agent behavior state")
        .def("updateBehaviors", &AgentBehaviorManager::updateBehaviors,
             py::arg("timestep"),
             "Update all agent behaviors")
        .def("getBehaviorStats", &AgentBehaviorManager::getBehaviorStats,
             "Get agent behavior statistics");

    // Advanced World Environments
    py::class_<DynamicWorld>(m, "DynamicWorld", R"pbdoc(Dynamic world with moving objects and environmental factors)pbdoc")
        .def(py::init<>())
        .def("configure", &DynamicWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure dynamic world")
        .def("addMovingObject", &DynamicWorld::addMovingObject,
             py::arg("x"), py::arg("y"), py::arg("type"),
             py::arg("velocity_x") = 0.0f, py::arg("velocity_y") = 0.0f,
             "Add moving object to world")
        .def("addDynamicObject", &DynamicWorld::addDynamicObject,
             py::arg("x"), py::arg("y"), py::arg("type"),
             py::arg("radius") = 0.5f, py::arg("dynamic") = true,
             "Add dynamic object to world")
        .def("updateObject", &DynamicWorld::updateObject,
             py::arg("index"), py::arg("x"), py::arg("y"),
             "Update object position")
        .def("setEnvironmentalFactor", &DynamicWorld::setEnvironmentalFactor,
             py::arg("factor"), py::arg("value"),
             "Set environmental factor")
        .def("getEnvironmentalFactor", &DynamicWorld::getEnvironmentalFactor,
             py::arg("factor"),
             "Get environmental factor value")
        .def("getWorldStats", &DynamicWorld::getWorldStats,
             "Get world statistics");

    // Visualization Manager
    py::class_<VisualizationManager>(m, "VisualizationManager", R"pbdoc(Advanced visualization and debugging tools)pbdoc")
        .def(py::init<>())
        .def("enableVisualization", &VisualizationManager::enableVisualization,
             py::arg("enabled"), "Enable/disable visualization")
        .def("setVisualizationOutput", &VisualizationManager::setVisualizationOutput,
             py::arg("output_type"), "Set visualization output type")
        .def("captureFrame", &VisualizationManager::captureFrame,
             py::arg("filename") = "",
             "Capture visualization frame")
        .def("createAnimation", &VisualizationManager::createAnimation,
             py::arg("filename"), py::arg("fps"), py::arg("duration"),
             "Create animation from frames")
        .def("generateReport", &VisualizationManager::generateReport,
             "Generate visualization report")
        .def("getVisualizationData", &VisualizationManager::getVisualizationData,
             "Get current visualization data")
        .def("exportData", &VisualizationManager::exportData,
             py::arg("filename"), "Export visualization data");

    // Performance Monitoring
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring and analysis)pbdoc")
        .def(py::init<>())
        .def("startMonitoring", &PerformanceMonitor::startMonitoring,
             "Start performance monitoring")
        .def("stopMonitoring", &PerformanceMonitor::stopMonitoring,
             "Stop performance monitoring")
        .def("recordStep", &PerformanceMonitor::recordStep,
             py::arg("step"),
             "Record step performance")
        .def("getReport", &PerformanceMonitor::getReport,
             "Get performance report")
        .def("getBottlenecks", &PerformanceMonitor::getBottlenecks,
             "Get performance bottlenecks")
        .def("getRecommendations", &PerformanceMonitor::getRecommendations,
             "Get optimization recommendations")
        .def("configure", &PerformanceMonitor::configure,
             py::arg("config"),
             "Configure performance monitoring");

    // Statistical Analysis
    py::class_<StatisticalAnalyzer>(m, "StatisticalAnalyzer", R"pbdoc(Statistical analysis of simulation data)pbdoc")
        .def(py::init<>())
        .def("analyzeDataFrame", &StatisticalAnalyzer::analyzeDataFrame,
             py::arg("data"),
             "Analyze statistical data")
        .def("detectOutliers", &StatisticalAnalyzer::detectOutliers,
             py::arg("data"), py::arg("method") = "zscore",
             "Detect outliers in data")
        .def("calculateCorrelation", &StatisticalAnalyzer::calculateCorrelation,
             py::arg("data1"), py::arg("data2"),
             "Calculate correlation between datasets")
        .def("performHypothesisTest", &StatisticalAnalyzer::performHypothesisTest,
             py::arg("data1"), py::arg("data2"), py::arg("test_type") = "t_test",
             "Perform hypothesis test")
        .def("getNormalityTest", &StatisticalAnalyzer::getNormalityTest,
             py::arg("data"),
             "Perform normality test");

    // Memory Manager
    py::class_<MemoryManager>(m, "MemoryManager", R"pbdoc(Memory management and optimization)pbdoc")
        .def(py::init<>())
        .def("getMemoryUsage", &MemoryManager::getMemoryUsage,
             "Get current memory usage")
        .def("getMemoryStats", &MemoryManager::getMemoryStats,
             "Get memory statistics")
        .def("optimizeMemory", &MemoryManager::optimizeMemory,
             "Optimize memory usage")
        .def("getOptimizationSuggestions", &MemoryManager::getOptimizationSuggestions,
             "Get memory optimization suggestions")
        .def("trackAllocation", &MemoryManager::trackAllocation,
             py::arg("size"), py::arg("type"),
             "Track memory allocation")
        .def("trackDeallocation", &MemoryManager::trackDeallocation,
             py::arg("size"), py::arg("type"),
             "Track memory deallocation");

    // Simulation Controller
    py::class_<SimulationController>(m, "SimulationController", R"pbdoc(Advanced simulation control and automation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimulationController::configure,
             py::arg("config"),
             "Configure simulation controller")
        .def("runSimulation", &SimulationController::runSimulation,
             py::arg("brain"), py::arg("world"), py::arg("steps"),
             py::arg("on_step_callback") = nullptr,
             py::arg("checkpoint_interval") = 100,
             "Run controlled simulation")
        .def("runEpisode", &SimulationController::runEpisode,
             py::arg("agent"), py::arg("num_steps"),
             py::arg("reward_threshold") = 0.0,
             "Run agent episode")
        .def("pauseSimulation", &SimulationController::pauseSimulation,
             "Pause simulation")
        .def("resumeSimulation", &SimulationController::resumeSimulation,
             "Resume simulation")
        .def("stopSimulation", &SimulationController::stopSimulation,
             "Stop simulation")
        .def("getStatistics", &SimulationController::getStatistics,
             "Get simulation statistics")
        .def("saveSimulation", &SimulationController::saveSimulation,
             py::arg("filename"),
             "Save simulation state")
        .def("loadSimulation", &SimulationController::loadSimulation,
             py::arg("filename"),
             "Load simulation state");

    // Resource Management Utilities
    py::class_<ResourceManager>(m, "ResourceManager", R"pbdoc(Resource management and cleanup utilities)pbdoc")
        .def(py::init<>())
        .def("registerResource", &ResourceManager::registerResource,
             py::arg("name"), py::arg("deleter"),
             "Register resource with cleanup function")
        .def("cleanupResources", &ResourceManager::cleanupResources,
             "Clean up all resources")
        .def("getResourceStats", &ResourceManager::getResourceStats,
             "Get resource usage statistics")
        .def("trackUsage", &ResourceManager::trackUsage,
             py::arg("resource_name"), py::arg("size"),
             "Track resource usage");

    // Context Manager for automatic resource cleanup
    py::class_<SimulationContext>(m, "SimulationContext", R"pbdoc(Context manager for simulation resource cleanup)pbdoc")
        .def(py::init<>())
        .def("__enter__", &SimulationContext::enter)
        .def("__exit__", &SimulationContext::exit)
        .def("addResource", &SimulationContext::addResource,
             py::arg("resource"),
             "Add resource to context")
        .def("cleanup", &SimulationContext::cleanup,
             "Cleanup all resources");

    // Batch Simulation Context
    py::class_<BatchSimulationContext>(m, "BatchSimulationContext", R"pbdoc(Context manager for batch simulation processing)pbdoc")
        .def(py::init<>())
        .def("__enter__", &BatchSimulationContext::enter)
        .def("__exit__", &BatchSimulationContext::exit)
        .def("recordProgress", &BatchSimulationContext::recordProgress,
             py::arg("current"), py::arg("total"),
             "Record batch progress")
        .def("getProgress", &BatchSimulationContext::getProgress,
             "Get current progress");

    // Utility Functions for Enhanced API
    m.def("createConfigTemplate", &createConfigTemplate,
         py::arg("name"),
         "Create configuration template");

    m.def("createParameterSweep", &createParameterSweep,
         "Create parameter sweep controller");

    m.def("createPerformanceMonitor", &createPerformanceMonitor,
         "Create performance monitor");

    m.def("createStatisticalAnalyzer", &createStatisticalAnalyzer,
         "Create statistical analyzer");

    m.def("createMemoryManager", &createMemoryManager,
         "Create memory manager");

    m.def("createSimulationController", &createSimulationController,
         "Create simulation controller");

    m.def("createResourceManager", &createResourceManager,
         "Create resource manager");

    m.def("createVisualizationManager", &createVisualizationManager,
         "Create visualization manager");

    m.def("createAgentBehaviorManager", &createAgentBehaviorManager,
         "Create agent behavior manager");

    m.def("createPlasticityController", &createPlasticityController,
         "Create plasticity controller");

    m.def("createDynamicWorld", &createDynamicWorld,
         "Create dynamic world environment");

    // Enhanced Error Types
    py::register_exception<InvalidConfigurationError>(m, "InvalidConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<VisualizationError>(m, "VisualizationError");
    py::register_exception<PerformanceError>(m, "PerformanceError");

    // Additional utility functions
    m.def("run_parameter_sweep", &runParameterSweep,
         py::arg("param_space"), py::arg("objective_function"),
         py::arg("max_evaluations") = 100, py::arg("parallel") = False,
         "Run parameter sweep optimization");

    m.def("analyze_simulation_data", &analyzeSimulationData,
         py::arg("data"), py::arg("analysis_type") = "comprehensive",
         "Analyze simulation data with statistical tools");

    m.def("generate_performance_report", &generatePerformanceReport,
         py::arg("monitor"),
         "Generate performance report from monitor data");

    m.def("validate_configuration", &validateConfiguration,
         py::arg("config"), py::arg("strict") = True,
         "Validate configuration with optional strict checking");

    m.def("get_api_info", &getApiInfo,
         "Get API version and feature information");

    m.def("get_system_info", &getSystemInfo,
         "Get system information and capabilities");

    // Version and metadata
    m.attr("__version__") = "2.0.0";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features";
    m.attr("__api_version__") = "2.0";

    // Example function for demonstration
    m.def("create_example_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000, nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, "Create example brain for demonstration");

    // Example agent function
    m.def("create_example_agent", []() {
        auto config = std::make_shared<Config>();
        config->set("agent.reward_shaping", 0.1, nlm::ConfigSource::Default);
        config->set("agent.curiosity.enabled", true, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        return agent;
    }, "Create example agent for demonstration");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
