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

    // Enhanced Configuration methods
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class)pbdoc")
        .def(py::init<>())
        .def("loadFromJSON", &Config::loadFromJSON, py::arg("jsonString"), py::arg("validate") = false,
             "Load configuration from a JSON string")
        .def("saveToJSON", [](Config& self, std::string& jsonString) {
            // Use existing saveToFile for now - saveToJSON needs implementation
            std::string temp = "/tmp/temp_config.json";
            self.saveToFile(temp);
            std::ifstream file(temp);
            if (file.is_open()) {
                jsonString.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            }
        }, py::arg("jsonString"), "Save configuration to JSON string")
        .def("validateConfig", [](Config& self, std::string& errorMessage) {
            return self.validate(errorMessage);
        }, py::arg("errorMessage"), "Validate configuration")
        .def("setWithSource", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set configuration value with source tracking")
        .def("getValidationReport", &Config::getValidationReport,
             "Get detailed validation report");

    // Advanced STDP
    py::class_<AdvancedSTDP, std::shared_ptr<AdvancedSTDP>>(m, "AdvancedSTDP", R"pbdoc(Advanced STDP with triplet and reward modulation)pbdoc")
        .def("enableTripletSTDP", &AdvancedSTDP::enableTripletSTDP, py::arg("enable"), "Enable/disable triplet STDP")
        .def("isTripletSTDP", &AdvancedSTDP::isTripletSTDP, "Check if triplet STDP is enabled")
        .def("enableRewardModulatedSTDP", &AdvancedSTDP::enableRewardModulatedSTDP, py::arg("enable"), "Enable/disable reward-modulated STDP")
        .def("isRewardModulatedSTDP", &AdvancedSTDP::isRewardModulatedSTDP, "Check if reward-modulated STDP is enabled")
        .def("setTripletWeights", &AdvancedSTDP::setTripletWeights, py::arg("A1"), py::arg("A2"), py::arg("B1"), py::arg("B2"), "Set triplet STDP weights")
        .def("setRewardScale", &AdvancedSTDP::setRewardScale, py::arg("scale"), "Set reward modulation scale");

    // Visualization Interface
    py::class_<VisualizationInterface, std::shared_ptr<VisualizationInterface>>(m, "VisualizationInterface", R"pbdoc(Advanced visualization and analysis interface)pbdoc")
        .def("initialize", &VisualizationInterface::initialize, "Initialize visualization")
        .def("update", &VisualizationInterface::update, "Update visualization data")
        .def("render", &VisualizationInterface::render, "Render current state")
        .def("close", &VisualizationInterface::close, "Close visualization")
        .def("isActive", &VisualizationInterface::isActive, "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"), "Set update rate (Hz)")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate, "Get update rate (Hz)")
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork, py::arg("brain"), "Create network visualization")
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity, py::arg("brain"), "Create activity visualization")
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, py::arg("spikes"), "Create spike raster visualization")
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights, py::arg("brain"), "Create weight matrix visualization")
        .def("exportVisualization", &VisualizationInterface::exportVisualization, py::arg("filepath"), py::arg("format") = "png", "Export visualization to file")
        .def("getStatistics", &VisualizationInterface::getStatistics, "Get visualization statistics")
        .def("saveState", &VisualizationInterface::saveState, py::arg("filepath"), "Save visualization state")
        .def("loadState", &VisualizationInterface::loadState, py::arg("filepath"), "Load visualization state");

    // Profiler
    py::class_<Profiler, std::shared_ptr<Profiler>>(m, "Profiler", R"pbdoc(Performance profiler for timing operations)pbdoc")
        .def("start", &Profiler::start, py::arg("name"), "Start profiling a named operation")
        .def("stop", &Profiler::stop, py::arg("name"), "Stop profiling and return duration")
        .def("getTotalDuration", &Profiler::getTotalDuration, py::arg("name"), "Get total duration for operation")
        .def("getAverageDuration", &Profiler::getAverageDuration, py::arg("name"), "Get average duration for operation")
        .def("getMeasurementCount", &Profiler::getMeasurementCount, py::arg("name"), "Get number of measurements")
        .def("getOperationNames", &Profiler::getOperationNames, "Get all profiled operation names")
        .def("clear", &Profiler::clear, "Clear all recorded data")
        .def("getTotalTime", &Profiler::getTotalTime, "Get total profiling time")
        .def("getReport", &Profiler::getReport, "Get detailed profiling report")
        .def("setVerbose", &Profiler::setVerbose, py::arg("enable"), "Enable/disable verbose output")
        .def("setThreshold", &Profiler::setThreshold, py::arg("threshold"), "Set minimum threshold for reporting (seconds)")
        .def("profile", py::overload_cast<const std::string&, std::function<double(void)>>(&Profiler::profile),
             py::arg("name"), py::arg("func"), "Profile a function using RAII");

    // Error Analyzer
    py::class_<ErrorAnalyzer, std::shared_ptr<ErrorAnalyzer>>(m, "ErrorAnalyzer", R"pbdoc(Systematic error analysis and reporting)pbdoc")
        .def("recordError", &ErrorAnalyzer::recordError, py::arg("errorType"), py::arg("errorMessage"), py::arg("context") = "", py::arg("stackTrace") = "",
             "Record an error occurrence")
        .def("getErrorCount", &ErrorAnalyzer::getErrorCount, py::arg("errorType"), "Get error count by type")
        .def("getErrorTypes", &ErrorAnalyzer::getErrorTypes, "Get all error types")
        .def("analyzePatterns", &ErrorAnalyzer::analyzePatterns, "Analyze error patterns and return recommendations")
        .def("getRecentErrors", &ErrorAnalyzer::getRecentErrors, py::arg("limit") = 10, "Get recent errors")
        .def("clear", &ErrorAnalyzer::clear, "Clear all recorded errors")
        .def("getReport", &ErrorAnalyzer::getReport, "Get error analysis report")
        .def("setAutoReport", &ErrorAnalyzer::setAutoReport, py::arg("enable"), "Enable/disable automatic error reporting")
        .def("setErrorThreshold", &ErrorAnalyzer::setErrorThreshold, py::arg("threshold"), "Set error threshold for reporting")
        .def("shouldReportError", &ErrorAnalyzer::shouldReportError, py::arg("errorType"), "Check if error should be reported")
        .def("exportErrors", &ErrorAnalyzer::exportErrors, py::arg("filepath"), "Export errors to file")
        .def("importErrors", &ErrorAnalyzer::importErrors, py::arg("filepath"), "Import errors from file");

    // Batch Processor
    py::class_<BatchProcessor, std::shared_ptr<BatchProcessor>>(m, "BatchProcessor", R"pbdoc(Batch processing and automation)pbdoc")
        .def("addConfiguration", &BatchProcessor::addConfiguration, py::arg("config"), py::arg("name") = "",
             "Add a configuration to the batch")
        .def("getConfigurations", &BatchProcessor::getConfigurations, "Get all configurations")
        .def("clearConfigurations", &BatchProcessor::clearConfigurations, "Clear all configurations")
        .def("runSimulationBatch", &BatchProcessor::runSimulationBatch, py::arg("environmentTemplate"), py::arg("maxStepsPerSimulation"), py::arg("progressCallback") = nullptr,
             "Run multiple simulations with different parameters")
        .def("runHyperparameterSearch", &BatchProcessor::runHyperparameterSearch, py::arg("hyperparameterNames"), py::arg("hyperparameterValues"), py::arg("environmentTemplate"), py::arg("maxSteps"), py::arg("progressCallback") = nullptr,
             "Run automated hyperparameter optimization")
        .def("runCrossValidation", &BatchProcessor::runCrossValidation, py::arg("configs"), py::arg("environmentTemplate"), py::arg("maxSteps"), py::arg("trainTestRatio") = 0.8, py::arg("progressCallback") = nullptr,
             "Run model validation across different conditions")
        .def("saveSimulationState", &BatchProcessor::saveResults, py::arg("filepath"), "Save batch results to file")
        .def("loadSimulationState", &BatchProcessor::loadResults, py::arg("filepath"), "Load batch results from file")
        .def("getStatistics", &BatchProcessor::getStatistics, "Get batch processing statistics")
        .def("setVerbose", &BatchProcessor::setVerbose, py::arg("enable"), "Enable/disable verbose output")
        .def("setMaxConcurrent", &BatchProcessor::setMaxConcurrent, py::arg("maxConcurrent"), "Set maximum concurrent simulations");

    // Network Analysis namespace
    m.def("analyzeConnectivity", &NetworkAnalysis::analyzeConnectivity, py::arg("brain"), py::arg("report"),
          "Analyze connectivity patterns in a brain")
        .def("computeGraphMetrics", &NetworkAnalysis::computeGraphMetrics, py::arg("brain"), py::arg("metrics"),
             "Compute graph metrics for neural connectivity")
        .def("identifyCommunities", &NetworkAnalysis::identifyCommunities, py::arg("brain"), py::arg("communities"),
             "Identify communities/modules in the network")
        .def("computeCentrality", &NetworkAnalysis::computeCentrality, py::arg("brain"), py::arg("centrality"),
             "Compute centrality measures")
        .def("detectHubs", &NetworkAnalysis::detectHubs, py::arg("brain"), py::arg("hubs"), py::arg("threshold") = 0.8f,
             "Detect network hubs");

    // Embedding namespace
    m.def("createEmbedding", &Embedding::createEmbedding, py::arg("brain"), py::arg("dimensions"), py::arg("embedding"),
          "Create embedding for neural activity patterns")
        .def("createSpikeEmbedding", &Embedding::createSpikeEmbedding, py::arg("spikeTrains"), py::arg("dimensions"), py::arg("embedding"),
             "Create embedding from spike trains")
        .def("computeSimilarity", &Embedding::computeSimilarity, py::arg("embedding1"), py::arg("embedding2"), py::arg("similarity"),
             "Compute similarity between embeddings")
        .def("clusterEmbeddings", &Embedding::clusterEmbeddings, py::arg("embeddings"), py::arg("clusters"),
             "Cluster embeddings");

    // Advanced Brain methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with advanced features)pbdoc")
        .def("enableRewardModulatedSTDP", &Brain::enableRewardModulatedSTDP, py::arg("enable"),
             "Enable/disable reward-modulated STDP")
        .def("enableTripletSTDP", &Brain::enableTripletSTDP, py::arg("enable"),
             "Enable/disable triplet-based STDP")
        .def("configureHebbianMode", &Brain::configureHebbianMode, py::arg("mode"),
             "Configure Hebbian learning modes")
        .def("setCuriosityParameters", &Brain::setCuriosityParameters, py::arg("noveltyWeight"), py::arg("predictionErrorWeight"),
             "Set curiosity-driven exploration parameters")
        .def("createVisualizationPipeline", &Brain::createVisualizationPipeline, "Create multi-modal visualization pipeline")
        .def("analyzeConnectivity", &Brain::analyzeConnectivity, "Analyze network connectivity")
        .def("computeGraphMetrics", &Brain::computeGraphMetrics, "Compute network graph metrics")
        .def("createEmbedding", &Brain::createEmbedding, py::arg("dimensions"), py::arg("embedding"), "Create embedding for dimensionality reduction")
        .def("loadFromJSON", &Brain::loadFromJSON, py::arg("jsonString"), py::arg("validate") = false,
             "Load configuration from JSON string")
        .def("saveToJSON", &Brain::saveToJSON, py::arg("jsonString"), "Save configuration to JSON string")
        .def("validateConfig", &Brain::validateConfig, py::arg("errorMessage"), "Validate configuration")
        .def("createCustomAgent", &Brain::createCustomAgent, py::arg("neuronTypes"), py::arg("connections"), py::arg("customConfig"),
             "Create agent with custom architecture")
        .def("enableMultiAgentSystem", &Brain::enableMultiAgentSystem, py::arg("agentCount"), py::arg("templateConfig"),
             "Create agent society")
        .def("createHierarchicalAgent", &Brain::createHierarchicalAgent, py::arg("layerNeuronCounts"), py::arg("layerTypes"), py::arg("hierarchicalConfig"),
             "Create layered agent structure")
        .def("setVerboseLogging", &Brain::setVerboseLogging, py::arg("enable"),
             "Enable/disable detailed logging")
        .def("createProfiler", &Brain::createProfiler, py::arg("name") = "default", "Create performance profiler")
        .def("createErrorAnalyzer", &Brain::createErrorAnalyzer, "Create systematic error analyzer")
        .def("logDetailedStatus", &Brain::logDetailedStatus, "Log detailed brain status")
        .def("getSystemHealthReport", &Brain::getSystemHealthReport, "Get system health report")
        .def("getPerformanceMetrics", &Brain::getPerformanceMetrics, "Get performance metrics")
        .def("exportModel", &Brain::exportModel, py::arg("filepath"), py::arg("format") = "binary",
             "Export model to file")
        .def("importModel", &Brain::importModel, py::arg("filepath"), py::arg("format") = "binary",
             "Import model from file")
        .def("getAdvancedSTDP", &Brain::getAdvancedSTDP, "Get advanced STDP instance")
        .def("getVisualizationInterface", &Brain::getVisualizationInterface, "Get visualization interface")
        .def("getSystemProfiler", &Brain::getSystemProfiler, "Get system profiler")
        .def("getErrorAnalyzer", &Brain::getErrorAnalyzer, "Get error analyzer");

    // Enhanced AgentBrain methods
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface with advanced capabilities)pbdoc")
        .def("setCuriosityParameters", &AgentBrain::setCuriosityParameters, py::arg("noveltyWeight"), py::arg("predictionErrorWeight"),
             "Set curiosity-driven exploration parameters")
        .def("createCustomAgent", &AgentBrain::createCustomAgent, py::arg("neuronTypes"), py::arg("connections"), py::arg("customConfig"),
             "Create agent with custom architecture")
        .def("enableMultiAgentSystem", &AgentBrain::enableMultiAgentSystem, py::arg("agentCount"), py::arg("templateConfig"),
             "Create agent society")
        .def("createHierarchicalAgent", &AgentBrain::createHierarchicalAgent, py::arg("layerNeuronCounts"), py::arg("layerTypes"), py::arg("hierarchicalConfig"),
             "Create layered agent structure");

    // Factory functions for advanced features
    m.def("createBatchProcessor", []() -> std::shared_ptr<BatchProcessor> {
        return std::make_shared<BatchProcessor>();
    }, "Create batch processor for simulation automation")
        .def("createVisualizationPipeline", []() -> std::shared_ptr<VisualizationPipeline> {
        return std::make_shared<VisualizationPipeline>();
    }, "Create multi-modal visualization pipeline")
        .def("createProfiler", [](const std::string& name) -> std::shared_ptr<Profiler> {
        auto profiler = std::make_shared<Profiler>();
        profiler->setVerbose(true);
        return profiler;
    }, py::arg("name") = "default", "Create performance profiler")
        .def("createErrorAnalyzer", []() -> std::shared_ptr<ErrorAnalyzer> {
        return std::make_shared<ErrorAnalyzer>();
    }, "Create systematic error analyzer");

    // Constants
    m.attr("ConfigSource") = py::module::import("config").attr("ConfigSource");

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
