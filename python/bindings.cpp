#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>
#include <stdexcept>
#include <iostream>
#include <sstream>

// Forward declarations for forward referencing
namespace nlm {
    class CheckpointSystem;
}

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/performance/Performance.hpp"
#include "../src/performance/CheckpointSystem.hpp"

// Custom exception types
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMValidationError : public std::runtime_error {
public:
    explicit NLMValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigurationError : public std::runtime_error {
public:
    explicit NLMConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMPermissionError : public std::runtime_error {
public:
    explicit NLMPermissionError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMSerializationError : public std::runtime_error {
public:
    explicit NLMSerializationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Context manager wrapper for automatic cleanup
class BrainContextManager {
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::Config> config_;
    bool initialized_;

public:
    explicit BrainContextManager(std::shared_ptr<nlm::Config> config = nullptr)
        : config_(config), initialized_(false) {
        if (!config_) {
            config_ = std::make_shared<nlm::Config>();
        }
    }

    ~BrainContextManager() {
        if (initialized_) {
            try {
                reset();
            } catch (...) {
                // Ignore errors in destructor
            }
        }
    }

    std::shared_ptr<nlm::Brain> createBrain() {
        brain_ = std::make_shared<nlm::Brain>(config_);
        return brain_;
    }

    bool initialize(double timestep = 0.01) {
        if (!brain_) {
            throw NLMConfigurationError("Brain not created. Call createBrain() first.");
        }
        
        // Initialize brain with default parameters
        bool success = brain_->initialize();
        if (!success) {
            throw NLMRuntimeError("Failed to initialize brain.");
        }
        
        initialized_ = true;
        return true;
    }

    void reset() {
        if (brain_) {
            brain_->reset();
            initialized_ = false;
            brain_.reset();
        }
    }

    std::shared_ptr<nlm::Brain> getBrain() const {
        return brain_;
    }

    bool isInitialized() const {
        return initialized_;
    }

    // Python context manager support
    void __enter__() {
        if (brain_ && !initialized_) {
            initialize();
        }
    }

    void __exit__(int type, const char* value, const char* traceback) {
        reset();
    }
};

// Batch processing utilities
class BatchProcessor {
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    std::vector<std::shared_ptr<nlm::SensoryInput>> sensoryInputs_;
    std::vector<std::shared_ptr<nlm::ActionResult>> results_;

public:
    BatchProcessor(std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::SimpleWorld> world)
        : brain_(brain), world_(world) {}

    void addSensoryInput(std::shared_ptr<nlm::SensoryInput> input) {
        sensoryInputs_.push_back(input);
    }

    void processBatch(double timestep = 0.01) {
        results_.clear();
        
        if (!world_) {
            throw NLMConfigurationError("World not available for batch processing.");
        }
        
        for (auto& input : sensoryInputs_) {
            // Process each input through the brain
            brain_->receiveSensoryInput(*input);
            auto action = brain_->produceAction();
            
            // Apply action in world
            if (action) {
                world_->applyMotorCommand(
                    static_cast<nlm::MotorCommand>(action->getType()),
                    0.0 // currentTime
                );
                
                // Get result
                auto percept = world_->getSensoryPercept();
                auto agentBody = world_->getAgentBody();
                
                // Create result (simplified)
                auto result = std::make_shared<nlm::ActionResult>(
                    0.0, true, "Processed"
                );
                results_.push_back(result);
            }
        }
    }

    std::vector<std::shared_ptr<nlm::ActionResult>> getResults() const {
        return results_;
    }

    size_t getProcessedCount() const {
        return results_.size();
    }

    void clear() {
        sensoryInputs_.clear();
        results_.clear();
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
private:
    struct Metrics {
        double fps = 0.0;
        double timestep = 0.0;
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        double memoryUsage = 0.0;
        double cpuUsage = 0.0;
        double neuronActivity = 0.0;
        double synapticActivity = 0.0;
        uint64_t simulationTime = 0;
    };

    Metrics currentMetrics_;
    std::chrono::high_resolution_clock::time_point lastUpdateTime_;
    size_t updateCount_ = 0;

public:
    PerformanceMonitor() {
        lastUpdateTime_ = std::chrono::high_resolution_clock::now();
    }

    void updateMetrics(const nlm::Brain& brain, const nlm::SimpleWorld& world,
                      uint64_t currentStep, double currentTime) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastUpdateTime_);
        
        updateCount_++;
        
        if (duration.count() > 0.0) {
            currentMetrics_.fps = 1.0 / duration.count();
        }
        
        currentMetrics_.timestep = duration.count();
        currentMetrics_.totalSteps = currentStep;
        currentMetrics_.totalSpikes = brain.getTotalSpikeCount();
        currentMetrics_.neuronActivity = static_cast<double>(brain.getActiveNeuronCount()) / 
                                         std::max(1.0, static_cast<double>(brain.getTotalNeuronCount()));
        currentMetrics_.synapticActivity = static_cast<double>(brain.getTotalSynapseCount()) / 
                                           std::max(1.0, static_cast<double>(brain.getActiveNeuronCount()));
        currentMetrics_.simulationTime = currentTime * 1e6; // Convert to microseconds
        
        lastUpdateTime_ = now;
    }

    double getFPS() const { return currentMetrics_.fps; }
    double getTimestep() const { return currentMetrics_.timestep; }
    size_t getTotalSteps() const { return currentMetrics_.totalSteps; }
    size_t getTotalSpikes() const { return currentMetrics_.totalSpikes; }
    double getNeuronActivity() const { return currentMetrics_.neuronActivity; }
    double getSynapticActivity() const { return currentMetrics_.synapticActivity; }
    uint64_t getSimulationTime() const { return currentMetrics_.simulationTime; }

    std::string getReport() const {
        std::stringstream ss;
        ss << "Performance Report:\n";
        ss << "  FPS: " << currentMetrics_.fps << "\n";
        ss << "  Timestep: " << currentMetrics_.timestep << "\n";
        ss << "  Total Steps: " << currentMetrics_.totalSteps << "\n";
        ss << "  Total Spikes: " << currentMetrics_.totalSpikes << "\n";
        ss << "  Neuron Activity: " << (currentMetrics_.neuronActivity * 100) << "%\n";
        ss << "  Synaptic Activity: " << (currentMetrics_.synapticActivity * 100) << "%\n";
        ss << "  Simulation Time: " << currentMetrics_.simulationTime << " μs\n";
        return ss.str();
    }
};

// Checkpoint utilities
class CheckpointManager {
private:
    std::shared_ptr<nlm::CheckpointManager> manager_;

public:
    CheckpointManager() {
        manager_ = std::make_shared<nlm::CheckpointManager>();
    }

    void configure(const std::string& checkpointDir, uint64_t saveIntervalSteps = 10000,
                   size_t maxCheckpoints = 10, bool compress = true) {
        manager_->configure(checkpointDir, saveIntervalSteps, maxCheckpoints, compress);
    }

    bool save(const std::string& name = "") {
        return manager_->saveImmediately(name);
    }

    bool load(const std::string& name) {
        return manager_->load(name);
    }

    std::vector<std::string> listCheckpoints() const {
        std::vector<std::string> names;
        auto infos = manager_->listCheckpoints();
        for (const auto& info : infos) {
            names.push_back(info.name);
        }
        return names;
    }

    void prune(size_t keepCount) {
        manager_->pruneOldCheckpoints(keepCount);
    }

    std::string getLastCheckpointPath() const {
        return manager_->getLastCheckpointPath();
    }
};

// Serialization utilities
namespace serialization {
    template<typename T>
    class Serializer {
    public:
        static std::vector<uint8_t> toBinary(const T& obj) {
            // Simplified serialization
            std::vector<uint8_t> buffer;
            buffer.reserve(sizeof(T));
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&obj);
            buffer.insert(buffer.end(), ptr, ptr + sizeof(T));
            return buffer;
        }

        static T fromBinary(const std::vector<uint8_t>& buffer) {
            if (buffer.size() < sizeof(T)) {
                throw NLMSerializationError("Buffer too small for deserialization");
            }
            T obj;
            std::copy(buffer.begin(), buffer.begin() + sizeof(T), 
                     reinterpret_cast<uint8_t*>(&obj));
            return obj;
        }

        static std::string toJSON(const T& obj) {
            // Simplified JSON serialization
            std::stringstream ss;
            ss << "{\"data\":\"" << std::to_string(reinterpret_cast<uintptr_t>(&obj)) 
               << "\"}";
            return ss.str();
        }

        static T fromJSON(const std::string& json) {
            // Simplified JSON deserialization
            throw NLMSerializationError("JSON deserialization not implemented for this type");
        }
    };

    // Specialized serializers for common types
    template<typename T>
    class SmartPointerSerializer {
    public:
        static std::vector<uint8_t> toBinary(const std::shared_ptr<T>& obj) {
            if (!obj) return std::vector<uint8_t>();
            return Serializer<T>::toBinary(*obj);
        }

        static std::shared_ptr<T> fromBinary(const std::vector<uint8_t>& buffer) {
            if (buffer.empty()) return nullptr;
            return std::make_shared<T>(Serializer<T>::fromBinary(buffer));
        }
    };
}

// Configuration utilities
namespace config {
    class ConfigBuilder {
    public:
        static std::shared_ptr<nlm::Config> createDefaultConfig() {
            auto config = std::make_shared<nlm::Config>();
            
            // Set default values
            config->set("brain.simType", std::string("spiking"));
            config->set("brain.maxNeurons", 10000);
            config->set("brain.maxSynapses", 100000);
            config->set("brain.dt", 0.01);
            config->set("brain.tau", 10.0);
            config->set("brain.v_thresh", -50.0);
            config->set("brain.v_reset", -65.0);
            config->set("brain.v_rest", -65.0);
            
            // Sensory defaults
            config->set("sensory.vision.width", 64);
            config->set("sensory.vision.height", 64);
            config->set("sensory.vision.channels", 3);
            
            // Motor defaults
            config->set("motor.maxForce", 10.0);
            config->set("motor.maxTorque", 5.0);
            
            // Development defaults
            config->set("development.enabled", true);
            config->set("development.stageDuration", 1000);
            
            return config;
        }

        static std::shared_ptr<nlm::Config> loadFromFile(const std::string& filepath) {
            auto config = std::make_shared<nlm::Config>();
            if (!config->loadFromFile(filepath)) {
                throw NLMRuntimeError("Failed to load configuration from file: " + filepath);
            }
            return config;
        }

        static std::shared_ptr<nlm::Config> loadFromString(const std::string& json) {
            auto config = std::make_shared<nlm::Config>();
            
            // Write JSON to temporary file
            const char* tmpFilename = "temp_config.json";
            std::ofstream file(tmpFilename);
            if (!file) {
                throw NLMRuntimeError("Failed to create temporary file for JSON parsing");
            }
            file << json;
            file.close();
            
            if (!config->loadFromFile(tmpFilename)) {
                std::remove(tmpFilename);
                throw NLMRuntimeError("Failed to parse JSON configuration");
            }
            std::remove(tmpFilename);
            return config;
        }
    };
}

// Workflow convenience functions
namespace workflows {
    class AgentTrainingWorkflow {
    private:
        std::shared_ptr<nlm::Brain> brain_;
        std::shared_ptr<nlm::SimpleWorld> world_;
        std::shared_ptr<nlm::AgentBrain> agentBrain_;
        PerformanceMonitor monitor_;

    public:
        AgentTrainingWorkflow() {
            world_ = std::make_shared<nlm::SimpleWorld>();
            auto config = config::ConfigBuilder::createDefaultConfig();
            brain_ = std::make_shared<nlm::Brain>(config);
            agentBrain_ = std::make_shared<nlm::AgentBrain>(brain_);
            
            world_->configure(100, 100, 64, 64);
            brain_->initialize();
        }

        void trainEpisode(int steps) {
            world_->reset();
            agentBrain_->initialize(*world_);
            
            for (int step = 0; step < steps; ++step) {
                // Get sensory percept
                auto percept = world_->getSensoryPercept();
                
                // Process through agent brain
                agentBrain_->processSensoryInput(percept);
                
                // Get action
                auto action = brain_->produceAction();
                if (action) {
                    world_->applyMotorCommand(
                        static_cast<nlm::MotorCommand>(action->getType()),
                        step * 0.01
                    );
                }
                
                // Update world
                world_->update(0.01);
                
                // Update monitoring
                monitor_.updateMetrics(*brain_, *world_, step, step * 0.01);
            }
        }

        double getTrainingEfficiency() const {
            return monitor_.getFPS() * monitor_.getNeuronActivity();
        }

        std::string getSummary() const {
            std::stringstream ss;
            ss << "Training Summary:\n";
            ss << "  Efficiency: " << getTrainingEfficiency() << "\n";
            ss << "  Total Steps: " << monitor_.getTotalSteps() << "\n";
            ss << "  Total Spikes: " << monitor_.getTotalSpikes() << "\n";
            ss << "  Average FPS: " << monitor_.getFPS() << "\n";
            return ss.str();
        }
    };

    class SimulationControlFlow {
    public:
        static void runStandardSimulation(std::shared_ptr<nlm::Brain> brain,
                                         std::shared_ptr<nlm::SimpleWorld> world,
                                         uint64_t steps,
                                         const std::string& outputDir = "") {
            if (!brain) {
                throw NLMConfigurationError("Brain is null.");
            }
            
            if (!world) {
                throw NLMConfigurationError("World is null.");
            }
            
            brain->initialize();
            world->reset();
            
            PerformanceMonitor monitor;
            
            for (uint64_t step = 0; step < steps; ++step) {
                // Get sensory input from world
                auto percept = world->getSensoryPercept();
                
                // Process through brain
                brain->receiveSensoryInput(*percept);
                
                // Generate action
                auto action = brain->produceAction();
                if (action) {
                    world->applyMotorCommand(
                        static_cast<nlm::MotorCommand>(action->getType()),
                        step * 0.01
                    );
                }
                
                // Update simulation
                world->update(0.01);
                
                // Update metrics
                monitor.updateMetrics(*brain, *world, step, step * 0.01);
                
                // Save checkpoint if output directory specified
                if (!outputDir.empty() && step % 1000 == 0) {
                    std::stringstream filename;
                    filename << outputDir << "/checkpoint_step_" << step << ".bin";
                    brain->save(filename.str());
                }
            }
        }

        static void runAgentWorldSimulation(std::shared_ptr<nlm::AgentBrain> agentBrain,
                                           std::shared_ptr<nlm::SimpleWorld> world,
                                           uint64_t episodes,
                                           uint64_t stepsPerEpisode) {
            if (!agentBrain) {
                throw NLMConfigurationError("AgentBrain is null.");
            }
            
            if (!world) {
                throw NLMConfigurationError("World is null.");
            }
            
            for (uint64_t episode = 0; episode < episodes; ++episode) {
                world->reset();
                agentBrain->initialize(*world);
                
                for (uint64_t step = 0; step < stepsPerEpisode; ++step) {
                    auto percept = world->getSensoryPercept();
                    agentBrain->processSensoryInput(percept);
                    
                    auto command = agentBrain->decodeMotorCommand();
                    if (command) {
                        world->applyMotorCommand(
                            static_cast<nlm::MotorCommand>(*command),
                            step * 0.01
                        );
                    }
                    
                    world->update(0.01);
                }
            }
        }
    };
}

// Visualization utilities
namespace visualization {
    class VisualizationHelper {
    private:
        std::shared_ptr<nlm::VisualizationInterface> viz_;

    public:
        VisualizationHelper() {
            viz_ = std::make_shared<nlm::VisualizationInterface>();
            viz_->initialize();
        }

        ~VisualizationHelper() {
            if (viz_->isActive()) {
                viz_->close();
            }
        }

        void visualizeBrain(const nlm::Brain& brain, bool network = true,
                          bool activity = true, bool spikes = true) {
            if (!viz_->isActive()) {
                return;
            }
            
            if (network) {
                viz_->visualizeNetwork(brain);
            }
            if (activity) {
                viz_->visualizeActivity(brain);
            }
            if (spikes) {
                // Would need to collect spikes from brain
                // viz_->visualizeSpikes(brain.getRecentSpikes());
            }
        }

        void visualizeWorld(const nlm::SimpleWorld& world, bool objects = true,
                          bool agent = true, bool sensor = true) {
            if (!viz_->isActive()) {
                return;
            }
            
            // Visualization logic would go here
            // viz_->visualizeWorldState(world, objects, agent, sensor);
        }

        void render() {
            if (viz_->isActive()) {
                viz_->render();
            }
        }

        void update() {
            if (viz_->isActive()) {
                viz_->update();
            }
        }

        bool isActive() const {
            return viz_->isActive();
        }

        void setUpdateRate(double hz) {
            if (viz_->isActive()) {
                viz_->setUpdateRate(hz);
            }
        }
    };
}

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
