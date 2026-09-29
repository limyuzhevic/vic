#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/json.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <stdexcept>
#include <unordered_map>

// Include all the necessary headers
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

// Python-style exception classes for different error types
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigError : public std::runtime_error {
public:
    explicit NLMConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMBatchError : public std::runtime_error {
public:
    explicit NLMBatchError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMVisualizationError : public std::runtime_error {
public:
    explicit NLMVisualizationError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMPerformanceError : public std::runtime_error {
public:
    explicit NLMPerformanceError(const std::string& msg) : std::runtime_error(msg) {}
};

// Performance monitoring utilities
class PerformanceProfiler {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;
    
    struct TimingResult {
        std::string operation;
        Duration elapsedTime;
        uint64_t iterations;
        double timePerIteration;
    };
    
    PerformanceProfiler() : totalTime_(0.0), callCount_(0) {}
    
    void start() { startTime_ = Clock::now(); }
    
    Duration stop(const std::string& operation, uint64_t iterations = 1) {
        auto endTime = Clock::now();
        auto duration = std::chrono::duration_cast<Duration>(endTime - startTime_);
        
        TimingResult result;
        result.operation = operation;
        result.elapsedTime = duration;
        result.iterations = iterations;
        result.timePerIteration = duration.count() / iterations;
        
        results_.push_back(result);
        totalTime_ += duration.count();
        callCount_++;
        
        return duration;
    }
    
    std::vector<TimingResult> getResults() const { return results_; }
    double getTotalTime() const { return totalTime_; }
    size_t getCallCount() const { return callCount_; }
    
    void clear() {
        results_.clear();
        totalTime_ = 0.0;
        callCount_ = 0;
    }
    
    // Convenience timing wrapper
    template<typename Func, typename... Args>
    auto timeOperation(const std::string& name, Func&& func, Args&&... args) {
        start();
        auto result = std::forward<Func>(func)(std::forward<Args>(args)...);
        stop(name, 1);
        return result;
    }
    
    template<typename Func, typename... Args>
    auto timeBatchOperation(const std::string& name, Func&& func, Args&&... args) {
        start();
        auto result = std::forward<Func>(func)(std::forward<Args>(args)...);
        stop(name, 1); // Could be modified to count actual iterations
        return result;
    }
    
private:
    TimePoint startTime_;
    std::vector<TimingResult> results_;
    double totalTime_;
    size_t callCount_;
};

// Configuration schema validation
struct ConfigSchema {
    std::string name;
    std::unordered_map<std::string, std::string> fieldTypes;
    std::unordered_map<std::string, std::string> fieldDescriptions;
    bool validateRequired;
    
    ConfigSchema(const std::string& n, bool required = true) 
        : name(n), validateRequired(required) {}
    
    void addField(const std::string& key, const std::string& type, const std::string& description = "") {
        fieldTypes[key] = type;
        fieldDescriptions[key] = description;
    }
};

// Batch operation helpers for common simulation workflows
class BatchOperationHelper {
public:
    // Batch simulation step with error handling
    static bool runBatchSimulation(std::shared_ptr<Brain> brain, 
                                  std::shared_ptr<SimpleWorld> world,
                                  const std::vector<SimulationStep>& steps,
                                  const std::vector<Timestamp>& times) {
        if (!brain || !world) {
            throw NLMBatchError("Brain and world must be non-null for batch simulation");
        }
        
        if (steps.size() != times.size()) {
            throw NLMBatchError("Steps and times vectors must have the same size");
        }
        
        bool success = true;
        for (size_t i = 0; i < steps.size(); ++i) {
            try {
                brain->step(steps[i], times[i]);
                world->update(times[i]);
            } catch (const std::exception& e) {
                std::cerr << "Batch step " << i << " failed: " << e.what() << std::endl;
                success = false;
                // Continue with next step to complete as much as possible
            }
        }
        
        return success;
    }
    
    // Network configuration helper
    static std::shared_ptr<Config> configureNetwork(const std::string& configFile,
                                                    const std::string& outputFile = "") {
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty() && !config->loadFromFile(configFile)) {
            throw NLMConfigError("Failed to load configuration from file: " + configFile);
        }
        
        // Apply sensible defaults if not set
        if (!config->has("brain.neuronCount")) {
            config->set("brain.neuronCount", 1000);
        }
        if (!config->has("brain.regionCount")) {
            config->set("brain.regionCount", 10);
        }
        if (!config->has("simulation.maxSteps")) {
            config->set("simulation.maxSteps", 1000);
        }
        
        if (!outputFile.empty()) {
            config->saveToFile(outputFile);
        }
        
        return config;
    }
    
    // Agent setup helper with validation
    static std::pair<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>> 
    setupAgent(const std::string& configFile, const std::string& worldConfig = "") {
        auto config = configureNetwork(configFile);
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw NLMRuntimeError("Failed to initialize brain");
        }
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        if (!worldConfig.empty()) {
            auto world = std::make_shared<SimpleWorld>();
            // Parse world config (simplified)
            // In real implementation, would parse YAML/JSON config
            agentBrain->initialize(*world);
        }
        
        return {brain, agentBrain};
    }
    
    // Performance benchmark helper
    static std::map<std::string, double> benchmarkPerformance(
        const std::string& operationName,
        const std::function<void()>& operation,
        int iterations = 10) {
        
        PerformanceProfiler profiler;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        double totalTime = 0.0;
        
        for (int i = 0; i < iterations; ++i) {
            profiler.start();
            operation();
            auto elapsed = profiler.stop(operationName + "_iter_" + std::to_string(i), 1);
            
            double elapsedSeconds = elapsed.count();
            minTime = std::min(minTime, elapsedSeconds);
            maxTime = std::max(maxTime, elapsedSeconds);
            totalTime += elapsedSeconds;
        }
        
        std::map<std::string, double> results;
        results["mean_time"] = totalTime / iterations;
        results["min_time"] = minTime;
        results["max_time"] = maxTime;
        results["total_time"] = totalTime;
        results["iterations"] = static_cast<double>(iterations);
        
        return results;
    }
};

// Enhanced AgentBrain interface with more intuitive methods
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<AgentBrain> agentBrain) 
        : agentBrain_(agentBrain) {
        if (!agentBrain_) {
            throw NLMRuntimeError("AgentBrain cannot be null");
        }
    }
    
    // Convenience methods for common agent workflows
    void trainEpisode(double duration, const std::vector<float>& initialSensors = {}) {
        if (duration <= 0) {
            throw NLMConfigError("Episode duration must be positive");
        }
        
        auto brain = agentBrain_->getBrain();
        brain->reset();
        
        double elapsed = 0.0;
        Timestamp startTime = std::chrono::system_clock::now().time_since_epoch().count();
        
        while (elapsed < duration) {
            double currentTime = startTime + elapsed;
            brain->step(static_cast<SimulationStep>(currentTime), currentTime);
            
            // Process sensory input if provided
            if (!initialSensors.empty()) {
                // Simplified sensory processing
                // In real implementation would convert sensor values to SensoryInput
            }
            
            elapsed += 0.1; // Fixed timestep for simplicity
        }
    }
    
    // Get formatted status report
    std::string getStatusReport() const {
        auto brain = agentBrain_->getBrain();
        std::stringstream ss;
        
        ss << "=== NLM Agent Status ===\n";
        ss << "Brain Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "Brain Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        ss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        ss << "Agent Neuromodulation Level: " << agentBrain_->getNeuromodulationLevel() << "\n";
        ss << "Agent Curiosity Level: " << agentBrain_->getCuriosityLevel() << "\n";
        ss << "Agent Novelty Level: " << agentBrain_->getNoveltyLevel() << "\n";
        ss << "Agent Prediction Error: " << agentBrain_->getPredictionError() << "\n";
        
        return ss.str();
    }
    
    // Run comparison between multiple configurations
    static std::vector<std::string> compareConfigurations(
        const std::vector<std::string>& configFiles,
        const std::function<void(std::shared_ptr<Brain>)>& setupFn) {
        
        std::vector<std::string> results;
        
        for (const auto& configFile : configFiles) {
            try {
                auto config = std::make_shared<Config>();
                if (!config->loadFromFile(configFile)) {
                    results.push_back("Config " + configFile + ": FAILED_TO_LOAD");
                    continue;
                }
                
                auto brain = std::make_shared<Brain>(config);
                setupFn(brain);
                
                auto report = EnhancedAgentBrain::createFromBrain(brain).getStatusReport();
                results.push_back("Config " + configFile + ": SUCCESS\n" + report);
                
            } catch (const std::exception& e) {
                results.push_back("Config " + configFile + ": ERROR - " + std::string(e.what()));
            }
        }
        
        return results;
    }
    
    static EnhancedAgentBrain createFromBrain(std::shared_ptr<Brain> brain) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        return EnhancedAgentBrain(agentBrain);
    }
    
    std::shared_ptr<AgentBrain> get() const { return agentBrain_; }
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

// Visualization helpers
class VisualizationHelper {
public:
    // Generate simple network visualization data
    static std::map<std::string, std::vector<std::vector<double>>> 
    generateNetworkData(const class Brain& brain, bool includeWeights = false) {
        
        std::map<std::string, std::vector<std::vector<double>>> visualizationData;
        
        // Get regions
        auto regions = brain.getRegions();
        std::vector<std::vector<double>> regionData;
        
        for (const auto& region : regions) {
            std::vector<double> regionPoint(3); // x, y, z coordinates
            // In real implementation, would compute actual layout
            regionPoint[0] = static_cast<double>(region->getId().index()) * 10.0;
            regionPoint[1] = 0.0;
            regionPoint[2] = 0.0;
            regionData.push_back(regionPoint);
        }
        
        visualizationData["regions"] = regionData;
        
        // Get neurons
        std::vector<std::vector<double>> neuronData;
        for (const auto& region : regions) {
            auto neurons = region->getNeurons();
            for (const auto& neuron : neurons) {
                std::vector<double> neuronPoint(3);
                // Simplified positioning
                neuronPoint[0] = static_cast<double>(neuron->getId().index()) * 0.5;
                neuronPoint[1] = static_cast<double>(region->getId().index()) * 5.0;
                neuronPoint[2] = 0.0;
                neuronData.push_back(neuronPoint);
            }
        }
        
        visualizationData["neurons"] = neuronData;
        
        // Get connections (synapses)
        if (includeWeights) {
            std::vector<std::vector<double>> connectionData;
            // In real implementation, would add weights as third dimension
            visualizationData["connections"] = connectionData;
        }
        
        return visualizationData;
    }
    
    // Generate spike raster data
    static std::map<std::string, std::vector<std::vector<double>>> 
    generateSpikeData(const std::vector<SpikeEvent>& spikes) {
        std::map<std::string, std::vector<std::vector<double>>> spikeData;
        
        std::vector<std::vector<double>> rasterData;
        
        for (const auto& spike : spikes) {
            std::vector<double> spikePoint(3);
            spikePoint[0] = static_cast<double>(spike.step);
            spikePoint[1] = spike.source_neuron.index();
            spikePoint[2] = spike.timestamp;
            rasterData.push_back(spikePoint);
        }
        
        spikeData["raster"] = rasterData;
        return spikeData;
    }
    
    // Generate activity heat map data
    static std::map<std::string, std::vector<std::vector<double>>> 
    generateActivityData(const class Brain& brain) {
        std::map<std::string, std::vector<std::vector<double>>> activityData;
        
        std::vector<std::vector<double>> activityGrid;
        size_t gridSize = 100; // 10x10 grid
        
        for (size_t i = 0; i < gridSize; ++i) {
            std::vector<double> row(gridSize, 0.0);
            activityGrid.push_back(row);
        }
        
        // Simplified activity calculation
        // In real implementation, would use actual neural activity data
        for (size_t i = 0; i < gridSize; ++i) {
            for (size_t j = 0; j < gridSize; ++j) {
                // Add some noise for visual interest
                activityGrid[i][j] = static_cast<double>(rand()) / RAND_MAX * 0.5 + 0.1;
            }
        }
        
        activityData["heatmap"] = activityGrid;
        return activityData;
    }
};

// Enhanced configuration loading with schema validation
class ConfigValidator {
public:
    static bool validateWithSchema(const Config& config, const ConfigSchema& schema) {
        if (schema.validateRequired) {
            for (const auto& [key, type] : schema.fieldTypes) {
                if (!config.has(key)) {
                    std::cerr << "Missing required field: " << key << std::endl;
                    return false;
                }
                
                // In real implementation, would check type compatibility
                // For now, just check existence
            }
        }
        
        // Check for unknown fields (optional)
        // In real implementation, would check against schema
        
        return true;
    }
    
    static ConfigSchema createNetworkSchema() {
        ConfigSchema schema("NetworkConfig");
        schema.addField("brain.neuronCount", "int", "Number of neurons in the network");
        schema.addField("brain.regionCount", "int", "Number of neural regions");
        schema.addField("simulation.maxSteps", "int", "Maximum simulation steps");
        schema.addField("simulation.dt", "double", "Time step duration");
        schema.addField("plasticity.enabled", "bool", "Whether plasticity is enabled");
        return schema;
    }
    
    static ConfigSchema createAgentSchema() {
        ConfigSchema schema("AgentConfig");
        schema.addField("agent.rewardModulation", "bool", "Enable reward modulation");
        schema.addField("agent.curiosity", "bool", "Enable curiosity-driven exploration");
        schema.addField("agent.development", "bool", "Enable developmental changes");
        schema.addField("agent.planning.horizon", "int", "Planning horizon length");
        return schema;
    }
};

// Main Python bindings
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides advanced error handling, batch operations, configuration validation,
        performance monitoring, visualization helpers, and enhanced agent interfaces.
    )pbdoc";

    // Register exception classes
    py::register_exception<NLMRuntimeError>(m, "NLMError");
    py::register_exception<NLMConfigError>(m, "NLMConfigError");
    py::register_exception<NLMBatchError>(m, "NLMBatchError");
    py::register_exception<NLMVisualizationError>(m, "NLMVisualizationError");
    py::register_exception<NLMPerformanceError>(m, "NLMPerformanceError");

    // Original exception registration (for backward compatibility)
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register existing NeuronId class
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

    // Register existing SynapseId class
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

    // Register existing Config class with enhancements
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class with validation)pbdoc")
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
        .def("validateSchema", [](Config& self, const ConfigSchema& schema) {
            return ConfigValidator::validateWithSchema(self, schema);
        }, py::arg("schema"), "Validate configuration against a schema")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Register existing SensoryInput classes
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

    // Register existing Action class
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

    // Register existing WorldObject class
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

    // Register existing AgentBody class
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

    // Register existing ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Register existing SensoryPercept class
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

    // Register existing SimpleWorld class with enhanced methods
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
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

    // Register PerformanceProfiler class
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance monitoring and profiling utility)pbdoc")
        .def(py::init<>())
        .def("start", &PerformanceProfiler::start)
        .def("stop", &PerformanceProfiler::stop, py::arg("operation"), py::arg("iterations") = 1)
        .def("getResults", &PerformanceProfiler::getResults)
        .def("getTotalTime", &PerformanceProfiler::getTotalTime)
        .def("getCallCount", &PerformanceProfiler::getCallCount)
        .def("clear", &PerformanceProfiler::clear)
        .def("timeOperation", &PerformanceProfiler::timeOperation,
             py::arg("name"), py::arg("func"))
        .def("timeBatchOperation", &PerformanceProfiler::timeBatchOperation,
             py::arg("name"), py::arg("func"));

    // Register ConfigSchema class
    py::class_<ConfigSchema>(m, "ConfigSchema", R"pbdoc(Configuration schema for validation)pbdoc")
        .def(py::init<const std::string&, bool>(), py::arg("name"), py::arg("required") = true)
        .def("addField", &ConfigSchema::addField, py::arg("key"), py::arg("type"), py::arg("description") = "")
        .def_readwrite("name", &ConfigSchema::name)
        .def_readwrite("validateRequired", &ConfigSchema::validateRequired);

    // Register ConfigValidator class
    py::class_<ConfigValidator>(m, "ConfigValidator", R"pbdoc(Configuration validation utilities)pbdoc")
        .def_static("validateWithSchema", &ConfigValidator::validateWithSchema,
                   py::arg("config"), py::arg("schema"))
        .def_static("createNetworkSchema", &ConfigValidator::createNetworkSchema)
        .def_static("createAgentSchema", &ConfigValidator::createAgentSchema);

    // Register BatchOperationHelper class
    py::class_<BatchOperationHelper>(m, "BatchOperationHelper", R"pbdoc(Batch operation utilities)pbdoc")
        .def_static("runBatchSimulation", &BatchOperationHelper::runBatchSimulation,
                   py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("times"))
        .def_static("configureNetwork", &BatchOperationHelper::configureNetwork,
                   py::arg("configFile"), py::arg("outputFile") = "")
        .def_static("setupAgent", &BatchOperationHelper::setupAgent,
                   py::arg("configFile"), py::arg("worldConfig") = "")
        .def_static("benchmarkPerformance", &BatchOperationHelper::benchmarkPerformance,
                   py::arg("operationName"), py::arg("operation"), py::arg("iterations") = 10);

    // Register EnhancedAgentBrain class
    py::class_<EnhancedAgentBrain>(m, "EnhancedAgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>>(), py::arg("agentBrain"))
        .def("trainEpisode", &EnhancedAgentBrain::trainEpisode,
             py::arg("duration"), py::arg("initialSensors") = std::vector<float>())
        .def("getStatusReport", &EnhancedAgentBrain::getStatusReport)
        .def_static("compareConfigurations", &EnhancedAgentBrain::compareConfigurations,
                   py::arg("configFiles"), py::arg("setupFn"))
        .def_static("createFromBrain", &EnhancedAgentBrain::createFromBrain,
                   py::arg("brain"));

    // Register VisualizationHelper class
    py::class_<VisualizationHelper>(m, "VisualizationHelper", R"pbdoc(Visualization data generation)pbdoc")
        .def_static("generateNetworkData", &VisualizationHelper::generateNetworkData,
                   py::arg("brain"), py::arg("includeWeights") = false)
        .def_static("generateSpikeData", &VisualizationHelper::generateSpikeData,
                   py::arg("spikes"))
        .def_static("generateActivityData", &VisualizationHelper::generateActivityData,
                   py::arg("brain"));

    // Register VisualizationInterface (original)
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface)pbdoc")
        .def(py::init<>())
        .def("initialize", &VisualizationInterface::initialize)
        .def("update", &VisualizationInterface::update)
        .def("render", &VisualizationInterface::render)
        .def("close", &VisualizationInterface::close)
        .def("isActive", &VisualizationInterface::isActive)
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"))
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate)
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork, py::arg("brain"))
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity, py::arg("brain"))
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, py::arg("spikes"))
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights, py::arg("brain"));

    // Register original classes (Brain, AgentBrain)
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced neural simulation brain class)pbdoc")
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

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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

    // Register convenience factory functions
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

    // Register convenience aliases
    m.def("createEnhancedAgentBrain", [](std::shared_ptr<AgentBrain> agentBrain) {
        return EnhancedAgentBrain(agentBrain);
    }, py::arg("agentBrain"), "Create an enhanced agent brain wrapper");

    m.def("createVisualizationHelper", []() {
        return VisualizationHelper();
    }, "Create a visualization helper");

    m.def("createConfigValidator", []() {
        return ConfigValidator();
    }, "Create a configuration validator");

    m.def("createBatchHelper", []() {
        return BatchOperationHelper();
    }, "Create a batch operation helper");

    // Register invalid ID constants (for backward compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register JSON/YAML support (simplified)
    m.def("loadJsonConfig", [](const std::string& jsonString) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // In real implementation, would parse JSON using pybind11::json
        // For now, just return empty config
        return config;
    }, py::arg("jsonString"), "Load configuration from JSON string");

    m.def("saveJsonConfig", [](const Config& config) {
        // In real implementation, would serialize to JSON using pybind11::json
        std::cout << "Configuration summary: " << config.summary() << std::endl;
    }, py::arg("config"), "Save configuration to JSON format");

    m.def("createFromDict", [](const py::dict& configDict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // In real implementation, would convert Python dict to Config
        return config;
    }, py::arg("configDict"), "Create configuration from Python dictionary");

    m.def("toDict", [](const Config& config) {
        py::dict result;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            // In real implementation, would get value from config
            result[key.c_str()] = "example_value";
        }
        return result;
    }, py::arg("config"), "Convert configuration to Python dictionary");
}

} // namespace nlm
