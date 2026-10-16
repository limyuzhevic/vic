#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <fstream>
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

// Base EventHandler class (abstract)
class EventHandler {
public:
    virtual ~EventHandler() = default;
    virtual void onStep(SimulationStep step) {}
    virtual void onNeuronSpike(NeuronId neuron, RegionId region) {}
    virtual void onPlasticityUpdate(NeuronId pre, NeuronId post, float weightChange) {}
    virtual void onMemoryUpdate(const std::string& memoryType, const std::string& content) {}
    virtual void onActionProduced(const Action& action) {}
    virtual void onAgentUpdate(const std::string& agentState) {}
    virtual std::string getName() const { return "EventHandler"; }
};

// Base SimulationMonitor class
class SimulationMonitor {
public:
    virtual ~SimulationMonitor() = default;
    virtual void addEventHandler(std::shared_ptr<EventHandler> handler) {}
    virtual void removeEventHandler(std::shared_ptr<EventHandler> handler) {}
    virtual bool isMonitoring() const { return false; }
    virtual void startMonitoring() {}
    virtual void stopMonitoring() {}
    virtual size_t getEventCount() const { return 0; }
    virtual void clearEvents() {}
    virtual double getElapsedTime() const { return 0.0; }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Advanced features include:
        - Callback/event systems for simulation monitoring
        - Performance monitoring and profiling APIs
        - Visualization tools for brain activity
        - Convenience functions for common scenarios
        - Advanced configuration options
        - Serialization/deserialization utilities
        - Safety and validation utilities
        - Utility functions for analysis and statistics
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Performance monitoring and profiling
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance monitoring and profiling tool)pbdoc")
        .def(py::init<>())
        .def("start", &PerformanceProfiler::start, "Start profiling")
        .def("stop", &PerformanceProfiler::stop, "Stop profiling")
        .def("is_running", &PerformanceProfiler::isRunning, "Check if profiler is running")
        .def("get_duration", &PerformanceProfiler::getDuration, "Get elapsed time in seconds")
        .def("get_cpu_time", &PerformanceProfiler::getCpuTime, "Get CPU time in seconds")
        .def("get_memory_usage", &PerformanceProfiler::getMemoryUsage, "Get current memory usage in MB")
        .def("get_samples_per_second", &PerformanceProfiler::getSamplesPerSecond, "Get average samples per second")
        .def("reset", &PerformanceProfiler::reset, "Reset profiler statistics")
        .def("record_sample", &PerformanceProfiler::recordSample, py::arg("count"), "Record sample count for average calculation")
        .def("get_statistics", &PerformanceProfiler::getStatistics, py::return_value_policy::copy, "Get profiling statistics")
        .def("export_profile", &PerformanceProfiler::exportProfile, py::arg("filepath"), "Export profile data to file");

    // Benchmarking utilities
    py::class_<Benchmark>(m, "Benchmark", R"pbdoc(Benchmarking utilities for performance comparison)pbdoc")
        .def(py::init<>())
        .def("run_benchmark", &Benchmark::runBenchmark, py::arg("test_name"), py::arg("test_func"), "Run a benchmark test")
        .def("compare_benchmarks", &Benchmark::compareBenchmarks, py::arg("benchmarks"), "Compare multiple benchmarks")
        .def("get_results", &Benchmark::getResults, py::return_value_policy::copy, "Get benchmark results")
        .def("clear_results", &Benchmark::clearResults, "Clear benchmark results")
        .def("get_best_benchmark", &Benchmark::getBestBenchmark, py::return_value_policy::copy, "Get best performing benchmark");

    // Visualization tools for brain activity
    py::class_<BrainVisualizer>(m, "BrainVisualizer", R"pbdoc(Brain activity visualization tools)pbdoc")
        .def(py::init<>())
        .def("initialize", &BrainVisualizer::initialize, py::arg("width") = 800, py::arg("height") = 600,
             "Initialize the visualizer with specified dimensions")
        .def("visualize_network", &BrainVisualizer::visualizeNetwork, py::arg("brain"), py::arg("region_id") = INVALID_REGION_ID,
             "Visualize brain network structure")
        .def("visualize_activity", &BrainVisualizer::visualizeActivity, py::arg("brain"), py::arg("time_window") = 1.0,
             "Visualize recent neural activity")
        .def("visualize_neurons", &BrainVisualizer::visualizeNeurons, py::arg("brain"), py::arg("neuron_type") = NeuronType::Excitatory,
             "Visualize specific neuron population")
        .def("visualize_synapses", &BrainVisualizer::visualizeSynapses, py::arg("brain"), py::arg("synapse_type") = SynapseType::Excitatory,
             "Visualize synaptic connections")
        .def("visualize_spikes", &BrainVisualizer::visualizeSpikes, py::arg("spikes"), py::arg("brain"),
             "Visualize spike raster plot")
        .def("visualize_weights", &BrainVisualizer::visualizeWeights, py::arg("brain"), py::arg("region_id") = INVALID_REGION_ID,
             "Visualize weight matrix")
        .def("visualize_development", &BrainVisualizer::visualizeDevelopment, py::arg("brain"),
             "Visualize developmental stage")
        .def("visualize_memory", &BrainVisualizer::visualizeMemory, py::arg("brain"), py::arg("memory_type"),
             "Visualize memory systems")
        .def("save_visualization", &BrainVisualizer::saveVisualization, py::arg("filepath"),
             "Save current visualization to file")
        .def("close", &BrainVisualizer::close, "Close the visualizer");

    // Network visualization utilities
    py::class_<NetworkVisualizer>(m, "NetworkVisualizer", R"pbdoc(Network topology visualization)pbdoc")
        .def(py::init<>())
        .def("plot_connectivity_matrix", &NetworkVisualizer::plotConnectivityMatrix, py::arg("brain"),
             "Plot connectivity matrix heatmap")
        .def("plot_adjacency_list", &NetworkVisualizer::plotAdjacencyList, py::arg("brain"), py::arg("region_id"),
             "Plot adjacency list for a region")
        .def("plot_degree_distribution", &NetworkVisualizer::plotDegreeDistribution, py::arg("brain"),
             "Plot degree distribution histogram")
        .def("plot_region_connections", &NetworkVisualizer::plotRegionConnections, py::arg("brain"),
             "Plot connections between regions")
        .def("plot_neuron_distribution", &NetworkVisualizer::plotNeuronDistribution, py::arg("brain"),
             "Plot neuron type distribution");

    // Activity visualization utilities
    py::class_<ActivityVisualizer>(m, "ActivityVisualizer", R"pbdoc(Neural activity time series visualization)pbdoc")
        .def(py::init<>())
        .def("plot_spike_raster", &ActivityVisualizer::plotSpikeRaster, py::arg("spikes"), py::arg("brain"),
             "Plot spike raster plot")
        .def("plot_firing_rate", &ActivityVisualizer::plotFiringRate, py::arg("brain"), py::arg("neuron_type"), py::arg("window") = 10.0,
             "Plot firing rate time series")
        .def("plot_voltage_traces", &ActivityVisualizer::plotVoltageTraces, py::arg("brain"), py::arg("neuron_ids"),
             "Plot membrane potential traces")
        .def("plot_correlation_matrix", &ActivityVisualizer::plotCorrelationMatrix, py::arg("brain"), py::arg("window") = 5.0,
             "Plot correlation matrix of activity")
        .def("plot_brain_graph", &ActivityVisualizer::plotBrainGraph, py::arg("brain"), py::arg("threshold"),
             "Plot brain activity graph");
        .def(py::init<>())
        .def("on_step", &EventHandler::onStep, py::arg("step"),
             "Called at the beginning of each simulation step")
        .def("on_neuron_spike", &EventHandler::onNeuronSpike, py::arg("neuron_id"), py::arg("region_id"),
             "Called when a neuron fires a spike")
        .def("on_plasticity_update", &EventHandler::onPlasticityUpdate, py::arg("pre_neuron"), py::arg("post_neuron"), py::arg("weight_change"),
             "Called when plasticity occurs")
        .def("on_memory_update", &EventHandler::onMemoryUpdate, py::arg("memory_type"), py::arg("content"),
             "Called when memory is updated")
        .def("on_action_produced", &EventHandler::onActionProduced, py::arg("action"),
             "Called when an action is produced")
        .def("on_agent_update", &EventHandler::onAgentUpdate, py::arg("agent_state"),
             "Called when agent state changes");

    py::class_<SimulationMonitor>(m, "SimulationMonitor", R"pbdoc(Simulation event monitoring and callback system)pbdoc")
        .def(py::init<>())
        .def("add_event_handler", &SimulationMonitor::addEventHandler, py::arg("handler"),
             "Add an event handler for simulation monitoring")
        .def("remove_event_handler", &SimulationMonitor::removeEventHandler, py::arg("handler"),
             "Remove an event handler")
        .def("is_monitoring", &SimulationMonitor::isMonitoring,
             "Check if monitoring is active")
        .def("start_monitoring", &SimulationMonitor::startMonitoring,
             "Start simulation monitoring")
        .def("stop_monitoring", &SimulationMonitor::stopMonitoring,
             "Stop simulation monitoring")
        .def("get_event_count", &SimulationMonitor::getEventCount,
             "Get total number of events recorded")
        .def("clear_events", &SimulationMonitor::clearEvents,
             "Clear all recorded events");
    
    // Python-friendly callback type
    m.def("set_simulation_callback", &setSimulationCallback, py::arg("callback"),
          "Set a simple callback function for simulation steps (Python callback)");
    m.def("set_step_callback", &setStepCallback, py::arg("callback"),
          "Set a step callback function (Python callback)");

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

    // Register custom Python exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<InvalidConfigException>(m, "InvalidConfigException");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<FileNotFoundException>(m, "FileNotFoundException");
    py::register_exception<InvalidParameterException>(m, "InvalidParameterException");
    
    // Add advanced configuration options to Config class
    .def("setInt", &Config::set, py::arg("key"), py::arg("value"),
         "Set integer configuration value")
    .def("setDouble", &Config::set, py::arg("key"), py::arg("value"),
         "Set double configuration value")
    .def("setBool", &Config::set, py::arg("key"), py::arg("value"),
         "Set boolean configuration value")
    .def("setString", &Config::set, py::arg("key"), py::arg("value"),
         "Set string configuration value")
    .def("setVectorInt", [](Config& self, const std::string& key, const std::vector<int>& value) {
         self.set(key, value);
     }, py::arg("key"), py::arg("value"),
         "Set integer vector configuration value")
    .def("setVectorDouble", [](Config& self, const std::string& key, const std::vector<double>& value) {
         self.set(key, value);
     }, py::arg("key"), py::arg("value"),
         "Set double vector configuration value")
    .def("setVectorString", [](Config& self, const std::string& key, const std::vector<std::string>& value) {
         self.set(key, value);
     }, py::arg("key"), py::arg("value"),
         "Set string vector configuration value")
    .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
         auto value = self.get<int>(key);
         return value ? *value : defaultValue;
     }, py::arg("key"), py::arg("defaultValue"),
         "Get integer configuration value with default")
    .def("getDouble", [](const Config& self, const std::string& key, double defaultValue) {
         auto value = self.get<double>(key);
         return value ? *value : defaultValue;
     }, py::arg("key"), py::arg("defaultValue"),
         "Get double configuration value with default")
    .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
         auto value = self.get<bool>(key);
         return value ? *value : defaultValue;
     }, py::arg("key"), py::arg("defaultValue"),
         "Get boolean configuration value with default")
    .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
         auto value = self.get<std::string>(key);
         return value ? *value : defaultValue;
     }, py::arg("key"), py::arg("defaultValue"),
         "Get string configuration value with default")
    .def("getVectorInt", [](const Config& self, const std::string& key, const std::vector<int>& defaultValue) {
         auto value = self.get<std::vector<int>>(key);
         return value ? *value : defaultValue;
     }, py::arg("key"), py::arg("defaultValue"),
         "Get integer vector configuration value with default")
    .def("getVectorDouble", [](const Config& self, const std::string& key, const std::vector<double>& defaultValue) {
         auto value = self.get<std::vector<double>>(key);
         return value ? *value : defaultValue;
     }, py::arg("key"), py::arg("defaultValue"),
         "Get double vector configuration value with default")
    .def("getVectorString", [](const Config& self, const std::string& key, const std::vector<std::string>& defaultValue) {
         auto value = self.get<std::vector<std::string>>(key);
         return value ? *value : defaultValue;
     }, py::arg("key"), py::arg("defaultValue"),
         "Get string vector configuration value with default")
    .def("loadFromFile", [](Config& self, const std::string& filepath) {
         // Enhanced file loading with validation and better error reporting
         std::string validatedPath = validateFilePath(filepath, "loadFromFile");
         
         if (!std::filesystem::exists(validatedPath)) {
             throw FileNotFoundException(validatedPath);
         }
         
         if (!std::filesystem::is_regular_file(validatedPath)) {
             throw InvalidConfigException("Path is not a regular file: " + validatedPath);
         }
         
         // Check file readability
         std::ifstream testFile(validatedPath);
         if (!testFile.good()) {
             throw InvalidConfigException("Cannot read file (permissions or corrupted): " + validatedPath);
         }
         
         bool result = self.loadFromFile(validatedPath);
         if (!result) {
             throw InvalidConfigException("Failed to parse JSON configuration: " + validatedPath);
         }
         
         return result;
     }, py::arg("filepath"),
         "Load configuration from a JSON file with validation")
    .def("saveToFile", [](Config& self, const std::string& filepath) {
         // Enhanced file saving with validation and better error reporting
         std::string validatedPath = validateFilePath(filepath, "saveToFile");
         
         // Check if parent directory exists
         std::filesystem::path fsPath(validatedPath);
         if (!fsPath.has_parent_path() || !std::filesystem::exists(fsPath.parent_path())) {
             throw InvalidParameterException("filepath", "Parent directory does not exist: " + validatedPath);
         }
         
         // Check if we can write to the directory
         std::ofstream testFile(fsPath.parent_path() / "test_write.tmp");
         if (!testFile.good()) {
             throw InvalidParameterException("filepath", "Cannot write to directory: " + fsPath.parent_path().string());
         }
         testFile.close();
         std::filesystem::remove(fsPath.parent_path() / "test_write.tmp");
         
         bool result = self.saveToFile(validatedPath);
         if (!result) {
             throw InvalidConfigException("Failed to save configuration to file: " + validatedPath);
         }
         
         return result;
     }, py::arg("filepath"),
         "Save configuration to a JSON file with validation");

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
             py::arg("channels") = 3,
             "Initialize vision input with specified dimensions. Validates parameters to ensure positive values.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            // Validate data size before processing
            if (data.empty()) {
                throw InvalidParameterException("data", "Data vector cannot be empty");
            }
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expectedSize) {
                throw InvalidParameterException("data", 
                    "Data size mismatch: expected " + std::to_string(expectedSize) + 
                    " elements, got " + std::to_string(data.size()) + "");
            }
            self.setData(data);
        }, py::arg("data"),
         "Set vision data with validation. Automatically validates that data size matches allocated buffer.")
        .def("getWidth", &Vision::getWidth, "Get vision image width")
        .def("getHeight", &Vision::getHeight, "Get vision image height")
        .def("getChannels", &Vision::getChannels, "Get vision image channels (typically 3 for RGB)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize audio input with sample rate and number of samples. Validates parameters to ensure positive values.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            // Validate data size before processing
            if (data.empty()) {
                throw InvalidParameterException("data", "Data vector cannot be empty");
            }
            if (data.size() != static_cast<size_t>(self.getNumSamples())) {
                throw InvalidParameterException("data", 
                    "Audio data size mismatch: expected " + std::to_string(self.getNumSamples()) + 
                    " samples, got " + std::to_string(data.size()) + "");
            }
            self.setData(data);
        }, py::arg("data"),
         "Set audio data with validation. Automatically validates that data size matches allocated buffer.")
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            validatePositive("sampleRate", static_cast<double>(sampleRate), 1.0);
            self.setSampleRate(sampleRate);
        }, py::arg("sampleRate"),
         "Set audio sample rate. Validates that sample rate is positive.")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

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
        .def("configure", [](SimpleWorld& self, float width, float height, 
                           size_t visionWidth, size_t visionHeight) {
            // Validate configuration parameters
            if (width <= 0.0f) {
                throw InvalidParameterException("width", "Width must be positive: " + std::to_string(width));
            }
            if (height <= 0.0f) {
                throw InvalidParameterException("height", "Height must be positive: " + std::to_string(height));
            }
            if (visionWidth == 0) {
                throw InvalidParameterException("visionWidth", "Vision width must be positive: " + std::to_string(visionWidth));
            }
            if (visionHeight == 0) {
                throw InvalidParameterException("visionHeight", "Vision height must be positive: " + std::to_string(visionHeight));
            }
            if (visionWidth > static_cast<size_t>(width)) {
                throw InvalidParameterException("visionWidth", 
                    "Vision width cannot exceed world width: " + std::to_string(visionWidth) + " > " + std::to_string(width));
            }
            if (visionHeight > static_cast<size_t>(height)) {
                throw InvalidParameterException("visionHeight", 
                    "Vision height cannot exceed world height: " + std::to_string(visionHeight) + " > " + std::to_string(height));
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions with validation. Validates that dimensions are positive and vision fits within world bounds.")
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            if (!self.isValidPosition(x, y)) {
                throw InvalidParameterException("position", 
                    "Invalid starting position (" + std::to_string(x) + ", " + std::to_string(y) + ")");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"),
             "Set agent starting position with validation. Validates that position is within bounds and not a wall.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent with validation")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject,
             py::arg("obj"),
             "Add object to world with validation")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy,
             py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate,
             py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed,
             py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed for reproducibility");

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

    // Enhanced Brain class with advanced system access
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration. Validates config pointer.")
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
             "Inject sensory input into the brain with validation")
        .def("injectCurrent", [](Brain& self, NeuronId neuron, MembranePotential current) {
            if (current == 0.0f) {
                throw InvalidParameterException("current", "Current cannot be zero");
            }
            self.injectCurrent(neuron, current);
        }, py::arg("neuron"), py::arg("current"),
         "Inject current into a specific neuron with validation")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            std::string validatedPath = validateFilePath(filepath, "save");
            bool result = self.save(validatedPath);
            if (!result) {
                throw SimulationError("Failed to save brain state to file: " + validatedPath);
            }
            return result;
        }, py::arg("filepath"),
         "Save brain state to file with validation")
        .def("load", [](Brain& self, const std::string& filepath) {
            std::string validatedPath = validateFilePath(filepath, "load");
            if (!std::filesystem::exists(validatedPath)) {
                throw FileNotFoundException(validatedPath);
            }
            bool result = self.load(validatedPath);
            if (!result) {
                throw SimulationError("Failed to load brain state from file: " + validatedPath);
            }
            return result;
        }, py::arg("filepath"),
         "Load brain state from file with validation")
        .def("addRegion", &Brain::addRegion,
             py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion,
             py::arg("id"),
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // Advanced system access methods
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory - transient active information")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory - experience storage")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory - pattern associations")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system - sensory prediction and error computation")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner - action planning")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation - abstract pattern discovery")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection - focus control")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system - developmental processes")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine - reward prediction error signal")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity - exploration motivation")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty - novelty detection")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal - predictive coding error")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator for stochastic processes")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system - neural dynamics and spike propagation")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity rule - spike-timing dependent plasticity")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity rule - correlation-based learning")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity - connectivity changes");

    // AgentBrain class (enhanced with validation)
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with validation")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world with validation")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain with validation")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation with validation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system with validation")
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
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

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

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

// Performance monitoring and profiling implementations
namespace nlm {

class PerformanceProfilerImpl : public PerformanceProfiler {
public:
    PerformanceProfilerImpl() : running_(false), startTime_(0), cpuTime_(0), samples_(0), memoryUsage_(0.0) {}
    
    void start() override {
        running_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void stop() override {
        if (running_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            duration_ = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_).count() / 1e6;
            running_ = false;
        }
    }
    
    bool isRunning() const override { return running_; }
    
    double getDuration() const override { return duration_; }
    
    double getCpuTime() const override { return cpuTime_; }
    
    double getMemoryUsage() const override { return memoryUsage_; }
    
    double getSamplesPerSecond() const override {
        if (duration_ > 0) {
            return samples_ / duration_;
        }
        return 0.0;
    }
    
    void reset() override {
        running_ = false;
        startTime_ = std::chrono::high_resolution_clock::now();
        duration_ = 0.0;
        cpuTime_ = 0.0;
        samples_ = 0;
        memoryUsage_ = 0.0;
    }
    
    void recordSample(size_t count) override {
        samples_ += count;
    }
    
    std::string getStatistics() const override {
        std::stringstream ss;
        ss << "PerformanceProfiler Statistics:" << std::endl;
        ss << "  Duration: " << duration_ << " seconds" << std::endl;
        ss << "  Samples: " << samples_ << std::endl;
        ss << "  Samples/sec: " << getSamplesPerSecond() << std::endl;
        ss << "  CPU Time: " << cpuTime_ << " seconds" << std::endl;
        ss << "  Memory Usage: " << memoryUsage_ << " MB" << std::endl;
        ss << "  Running: " << (running_ ? "Yes" : "No") << std::endl;
        return ss.str();
    }
    
    bool exportProfile(const std::string& filepath) override {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Performance Profile Export" << std::endl;
        file << "=====================" << std::endl;
        file << "Timestamp: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
        file << "Duration (s): " << duration_ << std::endl;
        file << "Samples: " << samples_ << std::endl;
        file << "Samples/sec: " << getSamplesPerSecond() << std::endl;
        file << "CPU Time (s): " << cpuTime_ << std::endl;
        file << "Memory Usage (MB): " << memoryUsage_ << std::endl;
        file << "Running: " << (running_ ? "true" : "false") << std::endl;
        
        file.close();
        return true;
    }
    
private:
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    double duration_;
    double cpuTime_;
    size_t samples_;
    double memoryUsage_;
};

class BenchmarkImpl : public Benchmark {
public:
    BenchmarkImpl() = default;
    
    struct BenchmarkResult {
        std::string name;
        double time;
        size_t iterations;
        double opsPerSecond;
        std::string details;
    };
    
    void runBenchmark(const std::string& testName, const std::function<void()>& testFunc) override {
        Timer timer;
        timer.start();
        testFunc();
        double elapsed = timer.getDuration();
        
        BenchmarkResult result;
        result.name = testName;
        result.time = elapsed;
        result.iterations = 1; // Simple benchmark
        result.opsPerSecond = 1.0 / elapsed;
        result.details = "Benchmark completed in " + std::to_string(elapsed) + " seconds";
        
        results_.push_back(result);
        
        // Add to global profiler
        if (profiler_) {
            profiler_->recordSample(1);
        }
    }
    
    void compareBenchmarks(const std::vector<BenchmarkImpl*>& benchmarks) override {
        std::vector<BenchmarkResult> allResults;
        
        for (auto* bench : benchmarks) {
            for (const auto& result : bench->results_) {
                allResults.push_back(result);
            }
        }
        
        results_.insert(results_.end(), allResults.begin(), allResults.end());
    }
    
    std::vector<BenchmarkResult> getResults() const override {
        return results_;
    }
    
    void clearResults() override {
        results_.clear();
    }
    
    BenchmarkResult getBestBenchmark() const override {
        if (results_.empty()) {
            return BenchmarkResult{"None", 0.0, 0, 0.0, "No benchmarks run"};
        }
        
        auto best = std::min_element(results_.begin(), results_.end(),
            [](const BenchmarkResult& a, const BenchmarkResult& b) { return a.time < b.time; });
        
        return *best;
    }
    
    void setProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
        profiler_ = profiler;
    }
    
private:
    std::vector<BenchmarkResult> results_;
    std::shared_ptr<PerformanceProfiler> profiler_;
};

class TimerImpl : public Timer {
public:
    TimerImpl() : running_(false), startTime_(0) {}
    
    void start() override {
        running_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void stop() override {
        if (running_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            duration_ = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_).count() / 1e6;
            running_ = false;
        }
    }
    
    void reset() override {
        running_ = false;
        startTime_ = std::chrono::high_resolution_clock::now();
        duration_ = 0.0;
    }
    
    bool isRunning() const override { return running_; }
    
    double getDuration() const override { return duration_; }
    
    double getDurationMs() const override { return duration_ * 1000.0; }
    
    double getDurationUs() const override { return duration_ * 1000000.0; }
    
    double peek() const override {
        if (!running_) return duration_;
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_).count() / 1e6 + duration_;
    }
    
    double peekMs() const override { return peek() * 1000.0; }
    
    double peekUs() const override { return peek() * 1000000.0; }
    
private:
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    double duration_;
};

} // namespace nlm

// Global instances
static std::shared_ptr<PerformanceProfiler> globalProfiler_ = std::make_shared<PerformanceProfilerImpl>();
static std::shared_ptr<Timer> globalTimer_ = std::make_shared<TimerImpl>();

PerformanceProfilerImpl::PerformanceProfilerImpl() : running_(false), startTime_(0), cpuTime_(0), samples_(0), memoryUsage_(0.0) {}
std::function<void(SimulationStep)> EventHandlerImpl::stepCallback_;
std::function<void(const std::string&)> EventHandlerImpl::eventCallback_;

std::shared_ptr<PerformanceProfiler> getGlobalProfiler() {
    return globalProfiler_;
}

std::shared_ptr<Timer> getGlobalTimer() {
    return globalTimer_;
}

void startProfiler() {
    globalProfiler_->start();
}

void stopProfiler() {
    globalProfiler_->stop();
}

void startTimer() {
    globalTimer_->start();
}

void stopTimer() {
    globalTimer_->stop();
}

} // namespace nlm
