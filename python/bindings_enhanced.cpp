#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <stdexcept>

// Include all core headers
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/motor/Action.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/environment/Environment.hpp"

namespace py = pybind11;
namespace nlm {

// Python exception types
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigurationError : public std::runtime_error {
public:
    explicit NLMConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMSimulationError : public std::runtime_error {
public:
    explicit NLMSimulationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Context manager for automatic resource cleanup
class BrainManager {
public:
    explicit BrainManager(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (!brain_) {
            throw NLMRuntimeError("BrainManager: brain pointer is null");
        }
    }

    ~BrainManager() {
        try {
            if (brain_) {
                brain_->reset();
            }
        } catch (...) {
            // Silently ignore errors during destruction
        }
    }

    // Prevent copying
    BrainManager(const BrainManager&) = delete;
    BrainManager& operator=(const BrainManager&) = delete;

    // Allow moving
    BrainManager(BrainManager&& other) noexcept : brain_(std::move(other.brain_)) {}
    BrainManager& operator=(BrainManager&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
        }
        return *this;
    }

    std::shared_ptr<Brain> getBrain() { return brain_; }
    const std::shared_ptr<Brain> getBrain() const { return brain_; }

private:
    std::shared_ptr<Brain> brain_;
};

class WorldManager {
public:
    explicit WorldManager(std::shared_ptr<SimpleWorld> world) : world_(world) {
        if (!world_) {
            throw NLMRuntimeError("WorldManager: world pointer is null");
        }
    }

    ~WorldManager() {
        try {
            if (world_) {
                world_->reset();
            }
        } catch (...) {
            // Silently ignore errors during destruction
        }
    }

    // Prevent copying
    WorldManager(const WorldManager&) = delete;
    WorldManager& operator=(const WorldManager&) = delete;

    // Allow moving
    WorldManager(WorldManager&& other) noexcept : world_(std::move(other.world_)) {}
    WorldManager& operator=(WorldManager&& other) noexcept {
        if (this != &other) {
            world_ = std::move(other.world_);
        }
        return *this;
    }

    std::shared_ptr<SimpleWorld> getWorld() { return world_; }
    const std::shared_ptr<SimpleWorld> getWorld() const { return world_; }

private:
    std::shared_ptr<SimpleWorld> world_;
};

class AgentBrainManager {
public:
    explicit AgentBrainManager(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {
        if (!agentBrain_) {
            throw NLMRuntimeError("AgentBrainManager: agentBrain pointer is null");
        }
    }

    ~AgentBrainManager() {
        try {
            if (agentBrain_) {
                agentBrain_->reset();
            }
        } catch (...) {
            // Silently ignore errors during destruction
        }
    }

    // Prevent copying
    AgentBrainManager(const AgentBrainManager&) = delete;
    AgentBrainManager& operator=(const AgentBrainManager&) = delete;

    // Allow moving
    AgentBrainManager(AgentBrainManager&& other) noexcept : agentBrain_(std::move(other.agentBrain_)) {}
    AgentBrainManager& operator=(AgentBrainManager&& other) noexcept {
        if (this != &other) {
            agentBrain_ = std::move(other.agentBrain_);
        }
        return *this;
    }

    std::shared_ptr<AgentBrain> getAgentBrain() { return agentBrain_; }
    const std::shared_ptr<AgentBrain> getAgentBrain() const { return agentBrain_; }

private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

// Statistics and analysis utilities
namespace py_utils {
    // Compute mean and standard deviation of a vector
    std::pair<double, double> computeMeanStdDev(const std::vector<float>& data) {
        if (data.empty()) {
            return {0.0, 0.0};
        }
        double sum = 0.0;
        for (float val : data) sum += val;
        double mean = sum / data.size();
        
        double sqSum = 0.0;
        for (float val : data) sqSum += (val - mean) * (val - mean);
        double stddev = std::sqrt(sqSum / data.size());
        
        return {mean, stddev};
    }

    // Normalize vector to [0, 1] range
    std::vector<float> normalizeVector(const std::vector<float>& data, float min, float max) {
        std::vector<float> result;
        result.reserve(data.size());
        float range = max - min;
        if (range <= 0.0f) {
            for (float val : data) result.push_back(0.5f);
        } else {
            for (float val : data) {
                result.push_back((val - min) / range);
            }
        }
        return result;
    }

    // Convert timestamp to human-readable string
    std::string timestampToString(double timestamp) {
        auto seconds = static_cast<long>(timestamp);
        auto millis = static_cast<long>((timestamp - seconds) * 1000.0);
        
        std::stringstream ss;
        ss << seconds << "." << std::setw(3) << std::setfill('0') << millis << "s";
        return ss.str();
    }

    // Format neuron activity statistics
    std::string formatNeuronStats(size_t total, size_t active, size_t firing, float rate) {
        std::stringstream ss;
        ss << "Neuron Statistics:\n";
        ss << "  Total: " << total << "\n";
        ss << "  Active: " << active << " (" 
           << (total > 0 ? (100.0 * active / total) : 0.0) << "%)\n";
        ss << "  Firing: " << firing << " (" 
           << (total > 0 ? (100.0 * firing / total) : 0.0) << "%)\n";
        ss << "  Average Firing Rate: " << rate << " Hz\n";
        return ss.str();
    }

    // Format world statistics
    std::string formatWorldStats(const SimpleWorld& world) {
        std::stringstream ss;
        ss << "World Statistics:\n";
        ss << "  Dimensions: " << world.getWidth() << " x " << world.getHeight() << "\n";
        ss << "  Simulation Time: " << world.getSimulationTime() << "s\n";
        ss << "  Max Energy: " << world.getMaxEnergy() << "\n";
        ss << "  Energy Decay Rate: " << world.getEnergyDecayRate() << "\n";
        ss << "  Random Seed: " << world.getRandomSeed() << "\n";
        return ss.str();
    }
}

// Serialization utilities
namespace serialization_utils {
    // Save brain to file
    bool saveBrainToFile(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        if (!brain) {
            throw NLMRuntimeError("saveBrainToFile: brain pointer is null");
        }
        return brain->save(filepath);
    }

    // Load brain from file
    bool loadBrainFromFile(std::shared_ptr<Brain>& brain, const std::string& filepath) {
        if (!brain) {
            brain = std::make_shared<Brain>(std::make_shared<Config>());
        }
        return brain->load(filepath);
    }

    // Save config to JSON
    bool saveConfigToJSON(const std::shared_ptr<Config>& config, const std::string& filepath) {
        if (!config) {
            throw NLMRuntimeError("saveConfigToJSON: config pointer is null");
        }
        return config->saveToFile(filepath);
    }

    // Load config from JSON
    bool loadConfigFromJSON(std::shared_ptr<Config>& config, const std::string& filepath) {
        if (!config) {
            config = std::make_shared<Config>();
        }
        return config->loadFromFile(filepath);
    }
}

// Debug and development utilities
namespace debug_utils {
    // Print brain state with color coding
    void printBrainState(const Brain& brain) {
        auto console = py::module::import("sys").attr("stdout");
        py::print("=== BRAIN STATE ===");
        py::print("Total Neurons:", brain.getTotalNeuronCount());
        py::print("Total Synapses:", brain.getTotalSynapseCount());
        py::print("Active Neurons:", brain.getActiveNeuronCount());
        py::print("Firing Neurons:", brain.getFiringNeuronCount());
        py::print("Total Spike Count:", brain.getTotalSpikeCount());
        py::print("Pending Spike Events:", brain.getPendingSpikeEventCount());
        py::print("Developmental Stage:", static_cast<int>(brain.getDevelopmentalStage()));
        py::print("Average Firing Rate:", brain.getAverageFiringRate(), "Hz");
        py::print("Excitatory/Inhibitory Ratio:", brain.getExcitationInhibitionRatio());
    }

    // Print detailed neuron statistics
    void printNeuronStatistics(const Brain& brain) {
        std::stringstream ss;
        ss << "=== NEURON STATISTICS ===\n";
        
        auto regions = brain.getRegions();
        for (const auto& region : regions) {
            ss << "Region: " << region->getName() << " (ID: " << region->getId().index() << ")\n";
            ss << "  Populations: " << region->getPopulationCount() << "\n";
            ss << "  Neurons: " << region->getTotalNeuronCount() << "\n";
            ss << "  Active: " << region->getActiveNeuronCount() << "\n";
            ss << "  Firing: " << region->getFiringNeuronCount() << "\n";
            ss << "  Avg Firing Rate: " << region->getAverageFiringRate() << " Hz\n";
        }
        
        py::print(ss.str());
    }

    // Profile simulation step timing
    class SimulationProfiler {
    public:
        SimulationProfiler() : total_time_(0.0), step_count_(0) {}
        
        void start() {
            start_time_ = std::chrono::high_resolution_clock::now();
        }
        
        void stop() {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
            total_time_ += duration.count() / 1000.0;  // Convert to milliseconds
            step_count_++;
        }
        
        double getAverageTime() const {
            return step_count_ > 0 ? total_time_ / step_count_ : 0.0;
        }
        
        std::string getReport() const {
            std::stringstream ss;
            ss << "=== SIMULATION PROFILING ===\n";
            ss << "Total Steps: " << step_count_ << "\n";
            ss << "Average Time per Step: " << getAverageTime() << " ms\n";
            ss << "Total Time: " << total_time_ << " ms\n";
            if (step_count_ > 0) {
                ss << "Steps per Second: " << (step_count_ * 1000.0 / total_time_) << "\n";
            }
            return ss.str();
        }
        
    private:
        std::chrono::high_resolution_clock::time_point start_time_;
        double total_time_;
        size_t step_count_;
    };

    // Write debug information to file
    void writeDebugLog(const std::string& filename, const std::string& message) {
        std::ofstream file(filename, std::ios::app);
        if (file.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            file << "[" << std::ctime(&time) << "] " << message << std::endl;
        }
    }
}

// Experiment management utilities
namespace experiment_utils {
    // Run a simple benchmark
    struct BenchmarkResult {
        std::string name;
        double time_seconds;
        size_t steps_completed;
        float memory_usage_mb;
        std::map<std::string, double> metrics;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "=== BENCHMARK: " << name << " ===\n";
            ss << "Time: " << time_seconds << " seconds\n";
            ss << "Steps: " << steps_completed << "\n";
            ss << "Memory: " << memory_usage_mb << " MB\n";
            for (const auto& metric : metrics) {
                ss << metric.first << ": " << metric.second << "\n";
            }
            return ss.str();
        }
    };

    // Run a simulation for a specified duration
    BenchmarkResult runSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                double duration_seconds, size_t target_fps = 60) {
        BenchmarkResult result;
        result.name = "Simulation Benchmark";
        
        if (!brain || !world) {
            throw NLMSimulationError("runSimulation: brain or world is null");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        size_t steps_completed = 0;
        double timestep = 1.0 / target_fps;
        
        double elapsed = 0.0;
        while (elapsed < duration_seconds) {
            auto step_start = std::chrono::high_resolution_clock::now();
            
            brain->step(static_cast<SimulationStep>(steps_completed), static_cast<Timestamp>(elapsed));
            world->update(timestep);
            
            auto step_end = std::chrono::high_resolution_clock::now();
            auto step_duration = std::chrono::duration_cast<std::chrono::microseconds>(step_end - step_start);
            
            elapsed += timestep;
            steps_completed++;
            
            // Simulate frame rate limiting
            auto frame_time = std::chrono::microseconds(static_cast<long>(timestep * 1000000));
            auto sleep_time = frame_time - (step_end - step_start);
            if (sleep_time > std::chrono::microseconds(0)) {
                std::this_thread::sleep_for(sleep_time);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        result.time_seconds = total_duration.count() / 1000.0;
        result.steps_completed = steps_completed;
        result.memory_usage_mb = 0.0;  // Placeholder - would need system-specific measurement
        
        return result;
    }
}

// Main bindings setup
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Pythonic API with context managers for automatic resource cleanup
        - Comprehensive error handling with custom Python exceptions
        - Statistical analysis utilities for neural data
        - Debugging and development tools
        - Serialization utilities for brain state management
        - Experiment management and benchmarking
        - Property accessors and improved method naming
        - Type safety and better documentation
        
        Classes:
        - Brain: Central neural simulation brain class
        - SimpleWorld: 2D world for NLM simulation
        - AgentBrain: Agent brain interface connecting NLM brain to world
        - Config: Configuration class for NLM system
        - Neuron: Single neuron implementation
        - NeuralRegion: Brain region containing populations
        - Sensory systems: Vision, Audio, InternalSignals
        - Plasticity systems: STDP, Hebbian, StructuralPlasticity
        - Neuromodulation systems: Reward, Curiosity, Novelty
        - Memory systems: WorkingMemory, EpisodicMemory
        - Development system for brain maturation
        - Action: Motor action representation
        - AgentBody: Agent physical state
        - SensoryPercept: Agent's sensory perception
        
        Utilities:
        - Statistical analysis functions
        - Serialization/deserialization
        - Debugging and profiling tools
        - Experiment management
        - Error handling classes
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<NLMRuntimeError>(m, "NLMRuntimeError");
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError");
    py::register_exception<NLMSimulationError>(m, "NLMSimulationError");

    // Register ID types
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

    // Register enum types
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
        .value("LookRight", MotorCommand::TurnRight)
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

    // Enhanced Config class with improved API
    py::class_<Config>(m, "Config", R"pbdoc(Comprehensive configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        // Enhanced getOr method with better Pythonic API
        .def("getOr", static_cast<std::optional<ConfigValue>(Config::*)(const std::string&) const>(
            &Config::get), py::arg("key"), "Get configuration value or None if not found")
        .def("getOr", static_cast<T(Config::*)(const std::string&, const T&) const>(
            &Config::getOr), py::arg("key"), py::arg("defaultValue"), 
            "Get configuration value with default fallback")
        // Pythonic property accessors
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Context manager support
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config&, const py::object&, const py::object&, const py::object&) {
            // Nothing to do for config cleanup
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with Pythonic properties
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input - enhanced with Pythonic properties)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Pythonic properties
        .def_property("width", &Vision::getWidth, nullptr, "Vision width (Python property)")
        .def_property("height", &Vision::getHeight, nullptr, "Vision height (Python property)")
        .def_property("channels", &Vision::getChannels, nullptr, "Vision channels (Python property)")
        .def_property("shape", [](Vision& self) { return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()}; },
                     nullptr, "Vision shape as [width, height, channels]")
        // Context manager
        .def("__enter__", [](Vision& self) { return &self; })
        .def("__exit__", [](Vision&, const py::object&, const py::object&, const py::object&) {
            // Nothing to do for vision cleanup
        });

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            return self.getSignal(index);
        }, py::arg("index"), "Get signal at index")
        .def("setSignal", [](InternalSignals& self, size_t index, float value) {
            self.setSignal(index, value);
        }, py::arg("index"), py::arg("value"), "Set signal at index");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Pythonic properties
        .def_property("type", &Action::getType, &Action::setType, "Action type (Python property)")
        .def_property("parameters", &Action::getParameters, &Action::setParameters, "Action parameters (Python property)")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // Enhanced WorldObject class
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
        .def_readwrite("active", &WorldObject::active)
        // Pythonic properties
        .def_property("position", [](WorldObject& self) { return std::pair<float, float>(self.x, self.y); },
                     [](WorldObject& self, const std::pair<float, float>& pos) { self.x = pos.first; self.y = pos.second; },
                     "Object position (Python property as x,y tuple)")
        .def_property("is_resource", &WorldObject::getTypeAsResource, nullptr, "Check if object is a resource")
        .def_property("is_hazard", &WorldObject::getTypeAsHazard, nullptr, "Check if object is a hazard")
        .def("getTypeName", [](const WorldObject& self) {
            switch (self.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get human-readable type name");

    // Enhanced AgentBody class
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
        .def("reset", &AgentBody::reset)
        // Pythonic properties
        .def_property("position", [](AgentBody& self) { return std::pair<float, float>(self.x, self.y); },
                     [](AgentBody& self, const std::pair<float, float>& pos) { self.x = pos.first; self.y = pos.second; },
                     "Agent position (Python property as x,y tuple)")
        .def_property("velocity", [](AgentBody& self) { return std::pair<float, float>(self.velocityX, self.velocityY); },
                     [](AgentBody& self, const std::pair<float, float>& vel) { self.velocityX = vel.first; self.velocityY = vel.second; },
                     "Agent velocity (Python property as vx,vy tuple)")
        .def_property("speed", [](AgentBody& self) { return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY); },
                     nullptr, "Agent speed (calculated Python property)")
        .def("isAlive", [](const AgentBody& self) { return self.health > 0.0f; }, "Check if agent is alive");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositiveReward", [](const ActionResult& self) { return self.reward > 0.0f; }, "Check if reward is positive")
        .def("isNegativeReward", [](const ActionResult& self) { return self.reward < 0.0f; }, "Check if reward is negative")
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult: reward=" << result.reward << ", success=" << (result.success ? "true" : "false")
               << ", message=\"" << result.message << "\">";
            return ss.str();
        });

    // Enhanced SensoryPercept class with Pythonic properties
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Pythonic properties for convenience
        .def_property("vision", &SensoryPercept::getVision, &SensoryPercept::setVision, "Vision data (Python property)")
        .def_property("touch", &SensoryPercept::getTouch, &SensoryPercept::setTouch, "Touch data (Python property)")
        .def_property("internal", &SensoryPercept::getInternal, &SensoryPercept::setInternal, "Internal signals (Python property)")
        .def_property("proprioception", &SensoryPercept::getProprioception, &SensoryPercept::setProprioception, "Proprioception data (Python property)")
        .def_property("audio", &SensoryPercept::getAudio, &SensoryPercept::setAudio, "Audio data (Python property)")
        .def_property("vision_shape", [](SensoryPercept& self) { return std::vector<size_t>{self.getVisionWidth(), self.getVisionHeight()}; },
                     nullptr, "Vision shape [width, height] (Python property)")
        .def("__repr__", [](const SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept: vision=" << percept.getVision().size() << " elements, touch=" << percept.getTouch().size()
               << ", internal=" << percept.getInternal().size() << ", timestamp=" << percept.getTimestamp() << ">";
            return ss.str();
        });

    // Enhanced SimpleWorld class with Pythonic API
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getVisionWidth", &SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &SimpleWorld::getVisionHeight)
        .def("getTouchData", [](const SimpleWorld& world) {
            return world.getSensoryPercept().getTouch();
        })
        .def("getInternalData", [](const SimpleWorld& world) {
            return world.getSensoryPercept().getInternal();
        })
        // Pythonic properties
        .def_property("vision_width", &SimpleWorld::getVisionWidth, nullptr, "Vision width (Python property)")
        .def_property("vision_height", &SimpleWorld::getVisionHeight, nullptr, "Vision height (Python property)")
        .def_property("agent", &SimpleWorld::getAgentBody, nullptr, "Agent body (Python property)")
        .def_property("percept", &SimpleWorld::getSensoryPercept, nullptr, "Current sensory percept (Python property)")
        .def("getTotalObjects", [](const SimpleWorld& world) {
            return world.getObjects().size();
        }, "Get total number of objects in world")
        // Context manager
        .def("__enter__", [](SimpleWorld& self) {
            return std::unique_ptr<WorldManager>(new WorldManager(&self));
        })
        .def("__exit__", [](WorldManager& manager, const py::object&, const py::object&, const py::object&) {
            // Manager will handle cleanup automatically
        });

    // Enhanced Brain class with comprehensive Python API
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
        // Enhanced plasticity system access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system (neural dynamics)")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get spike-timing dependent plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian learning system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        // Memory systems
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
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        // Development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        // Neuromodulation systems
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
             "Get prediction error signal")
        // Other systems
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update all plasticity rules")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signal")
        // Context manager
        .def("__enter__", [](Brain& self) {
            return std::unique_ptr<BrainManager>(new BrainManager(&self));
        })
        .def("__exit__", [](BrainManager& manager, const py::object&, const py::object&, const py::object&) {
            // Manager will handle cleanup automatically
        });

    // Enhanced AgentBrain class with Pythonic API
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
        // Configuration methods
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
        // Internal state access (for debugging)
        .def("getMotorNeurons", &AgentBrain::getMotorNeurons,
             "Get motor neuron groups")
        .def("getSensoryNeurons", &AgentBrain::getSensoryNeurons,
             "Get sensory neuron groups")
        .def("decodeFromMotorNeurons", &AgentBrain::decodeFromMotorNeurons,
             "Internal motor decoding")
        .def("selectWithCuriosity", &AgentBrain::selectWithCuriosity,
             py::arg("defaultCmd"),
             "Internal curiosity-based command selection")
        // Context manager
        .def("__enter__", [](AgentBrain& self) {
            return std::unique_ptr<AgentBrainManager>(new AgentBrainManager(&self));
        })
        .def("__exit__", [](AgentBrainManager& manager, const py::object&, const py::object&, const py::object&) {
            // Manager will handle cleanup automatically
        });

    // Register auxiliary classes (Neurons, Synapses, Regions, etc.)
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron implementation)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId)
        .def("getType", &Neuron::getType)
        .def("setType", &Neuron::setType, py::arg("type"))
        .def("getState", &Neuron::getState, py::return_value_policy::reference_internal)
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"))
        .def("getThreshold", &Neuron::getThreshold)
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"))
        .def("isFiring", &Neuron::isFiring)
        .def("isRefractory", &Neuron::isRefractory)
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"))
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"))
        .def("decrementRefractory", &Neuron::decrementRefractory)
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"))
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"))
        .def("getLeakConductance", &Neuron::getLeakConductance)
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"))
        .def("getRestingPotential", &Neuron::getRestingPotential)
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"))
        .def("checkThreshold", &Neuron::checkThreshold)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"))
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"))
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"))
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"))
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"))
        .def("getTotalCurrent", &Neuron::getTotalCurrent)
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent)
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"))
        .def("getSpikeHistory", &Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory)
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses)
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses)
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, py::return_value_policy::reference_internal)
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"))
        .def("getRegionId", &Neuron::getRegionId)
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"))
        .def("getPopulationId", &Neuron::getPopulationId)
        .def("step", &Neuron::step, py::arg("currentTime"))
        .def("reset", &Neuron::reset)
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"))
        .def("__repr__", [](const Neuron& neuron) {
            std::stringstream ss;
            ss << "<Neuron: id=" << neuron.getId().index() 
               << ", type=" << static_cast<int>(neuron.getType())
               << ", potential=" << neuron.getMembranePotential()
               << ", firing=" << (neuron.isFiring() ? "true" : "false") << ">";
            return ss.str();
        });

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)pbdoc")
        .def(py::init<>(), py::init<SynapseId, NeuronId, NeuronId, SynapseType, SynapticWeight>())
        .def("getId", &Synapse::getId)
        .def("getSource", &Synapse::getSource)
        .def("getDestination", &Synapse::getDestination)
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", &Synapse::setWeight, py::arg("weight"))
        .def("getDelay", &Synapse::getDelay)
        .def("getType", &Synapse::getType)
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, py::return_value_policy::reference_internal)
        .def("updateWeight", &Synapse::updateWeight, py::arg("delta"))
        .def("isPotentiation", &Synapse::isPotentiation)
        .def("isDepression", &Synapse::isDepression)
        .def("isModulatory", &Synapse::isModulatory)
        .def("isExcitatory", &Synapse::isExcitatory)
        .def("isInhibitory", &Synapse::isInhibitory)
        .def("getLastPreSpikeTime", &Synapse::getLastPreSpikeTime)
        .def("getLastPostSpikeTime", &Synapse::getLastPostSpikeTime)
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"))
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"))
        .def("getSTDPWeightChange", &Synapse::getSTDPWeightChange, py::arg("preSpikeTime"), py::arg("postSpikeTime"))
        .def("__repr__", [](const Synapse& synapse) {
            std::stringstream ss;
            ss << "<Synapse: id=" << synapse.getId().index()
               << ", src=" << synapse.getSource().index()
               << ", dst=" << synapse.getDestination().index()
               << ", weight=" << synapse.getWeight()
               << ", type=" << static_cast<int>(synapse.getType()) << ">";
            return ss.str();
        });

    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing neural populations)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal)
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"),
             py::return_value_policy::reference_internal)
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getPopulations", &NeuralRegion::getPopulations,
             py::return_value_policy::reference_internal)
        .def("getAllPopulations", &NeuralRegion::getAllPopulations)
        .def("addSynapse", &NeuralRegion::addSynapse,
             py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"),
             py::return_value_policy::reference_internal)
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getSynapses", &NeuralRegion::getSynapses,
             py::return_value_policy::reference_internal)
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"))
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"))
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount)
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount)
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate)
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight)
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity)
        .def("step", &NeuralRegion::step, py::arg("currentTime"))
        .def("reset", &NeuralRegion::reset)
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"))
        .def("getAllNeurons", &NeuralRegion::getAllNeurons)
        .def("getAllNeuronsConst", &NeuralRegion::getAllNeurons)
        .def("__repr__", [](const NeuralRegion& region) {
            std::stringstream ss;
            ss << "<NeuralRegion: id=" << region.getId().index() << ", name=\"" << region.getName() << "\">";
            return ss.str();
        });

    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Population of identical neurons)pbdoc")
        .def(py::init<PopulationId, size_t, NeuronType>(), py::arg("id"), py::arg("size"), py::arg("type"))
        .def("getId", &NeuralPopulation::getId)
        .def("getSize", &NeuralPopulation::getSize)
        .def("getType", &NeuralPopulation::getType)
        .def("getNeurons", &NeuralPopulation::getNeurons,
             py::return_value_policy::reference_internal)
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"),
             py::return_value_policy::reference_internal)
        .def("getActiveCount", &NeuralPopulation::getActiveCount)
        .def("getFiringCount", &NeuralPopulation::getFiringCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getTotalExcitatoryConnections", &NeuralPopulation::getTotalExcitatoryConnections)
        .def("getTotalInhibitoryConnections", &NeuralPopulation::getTotalInhibitoryConnections)
        .def("setAllNeuronsType", &NeuralPopulation::setAllNeuronsType, py::arg("type"))
        .def("step", &NeuralPopulation::step, py::arg("currentTime"))
        .def("reset", &NeuralPopulation::reset)
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"));

    // Enhanced plasticity system classes
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("update", &STDP::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &STDP::applyWeightChange, py::arg("synapse"), py::arg("delta"))
        .def("getName", &STDP::getName)
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"))
        .def("getLTPWeight", &STDP::getLTPWeight)
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"))
        .def("getLTDWeight", &STDP::getLTDWeight)
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"))
        .def("getTimeConstant", &STDP::getTimeConstant)
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"));

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("update", &Hebbian::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &Hebbian::applyWeightChange, py::arg("synapse"), py::arg("delta"))
        .def("getName", &Hebbian::getName)
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &Hebbian::getLearningRate)
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"))
        .def("getMaxWeight", &Hebbian::getMaxWeight);

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity system)pbdoc")
        .def(py::init<>())
        .def("createSynapse", &StructuralPlasticity::createSynapse,
             py::arg("brain"), py::arg("source"), py::arg("destination"), py::arg("weight"))
        .def("removeSynapse", &StructuralPlasticity::removeSynapse,
             py::arg("brain"), py::arg("synapse"))
        .def("createNeuron", &StructuralPlasticity::createNeuron,
             py::arg("brain"), py::arg("type"))
        .def("removeNeuron", &StructuralPlasticity::removeNeuron,
             py::arg("brain"), py::arg("neuron"))
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate)
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"))
        .def("getPruningRate", &StructuralPlasticity::getPruningRate)
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"))
        .def("update", &StructuralPlasticity::update, py::arg("brain"), py::arg("rng"));

    // Neuromodulation system classes
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Neuromodulator signal)pbdoc")
        .def(py::init<>(), py::init<double, double>())
        .def("getValue", &Neuromodulator::getValue)
        .def("getTimeConstant", &Neuromodulator::getTimeConstant)
        .def("setValue", &Neuromodulator::setValue, py::arg("value"))
        .def("setTimeConstant", &Neuromodulator::setTimeConstant, py::arg("tau"))
        .def("getName", &Neuromodulator::getName);

    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(Reward neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("getName", &Reward::getName);

    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getValue", &Curiosity::getValue)
        .def("setValue", &Curiosity::setValue, py::arg("value"))
        .def("getName", &Curiosity::getName);

    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getValue", &Novelty::getValue)
        .def("setValue", &Novelty::setValue, py::arg("value"))
        .def("getName", &Novelty::getName);

    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getValue", &PredictionError::getValue)
        .def("setValue", &PredictionError::setValue, py::arg("value"))
        .def("getName", &PredictionError::getName);

    // Memory system classes
    py::class_<NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def(py::init<>())
        .def("store", &NeuralWorkingMemory::store, py::arg("key"), py::arg("value"))
        .def("retrieve", &NeuralWorkingMemory::retrieve, py::arg("key"))
        .def("has", &NeuralWorkingMemory::has, py::arg("key"))
        .def("remove", &NeuralWorkingMemory::remove, py::arg("key"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getSize", &NeuralWorkingMemory::getSize)
        .def("getAllKeys", &NeuralWorkingMemory::getAllKeys)
        .def("update", &NeuralWorkingMemory::update, py::arg("timestep"));

    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def(py::init<>())
        .def("storeExperience", &NeuralEpisodicMemory::storeExperience,
             py::arg("timestamp"), py::arg("sensory"), py::arg("action"), py::arg("reward"))
        .def("retrieveSimilarExperience", &NeuralEpisodicMemory::retrieveSimilarExperience,
             py::arg("pattern"), py::arg("threshold") = 0.5)
        .def("getMemoryCount", &NeuralEpisodicMemory::getMemoryCount)
        .def("getLastExperience", &NeuralEpisodicMemory::getLastExperience)
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("prune", &NeuralEpisodicMemory::prune, py::arg("maxSize"));

    // Spike system class
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike event processing system)pbdoc")
        .def(py::init<>())
        .def("queueSpike", &SpikeSystem::queueSpike, py::arg("event"))
        .def("queueDelayedSpike", &SpikeSystem::queueDelayedSpike, py::arg("event"))
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("currentStep"))
        .def("processDelayedSpikes", &SpikeSystem::processDelayedSpikes,
             py::arg("currentStep"), py::arg("currentTime"))
        .def("registerHandler", &SpikeSystem::registerHandler, py::arg("handler"))
        .def("registerDelayedHandler", &SpikeSystem::registerDelayedHandler, py::arg("handler"))
        .def("getSpikeHistory", &SpikeSystem::getSpikeHistory)
        .def("clearHistory", &SpikeSystem::clearHistory)
        .def("getSpikeCount", &SpikeSystem::getSpikeCount)
        .def("getPendingSpikeCount", &SpikeSystem::getPendingSpikeCount)
        .def("getPendingDelayedCount", &SpikeSystem::getPendingDelayedCount)
        .def("getAverageSpikeRate", &SpikeSystem::getAverageSpikeRate)
        .def("getMostActiveNeurons", &SpikeSystem::getMostActiveNeurons, py::arg("count"))
        .def("reset", &SpikeSystem::reset);

    // Development system classes
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Brain development system)pbdoc")
        .def(py::init<>())
        .def("getCurrentStage", &DevelopmentSystem::getCurrentStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("getAge", &DevelopmentSystem::getAge)
        .def("update", &DevelopmentSystem::update, py::arg("timestep"))
        .def("isStageComplete", &DevelopmentSystem::isStageComplete, py::arg("stage"))
        .def("getSynaptogenesis", &DevelopmentSystem::getSynaptogenesis)
        .def("getPruning", &DevelopmentSystem::getPruning)
        .def("getMaturation", &DevelopmentSystem::getMaturation);

    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis subsystem)pbdoc")
        .def(py::init<>())
        .def("createSynapse", &Synaptogenesis::createSynapse,
             py::arg("brain"), py::arg("source"), py::arg("destination"),
             py::arg("weight") = 0.0f)
        .def("getCreationRate", &Synaptogenesis::getCreationRate)
        .def("setCreationRate", &Synaptogenesis::setCreationRate, py::arg("rate"));

    py::class_<Pruning>(m, "Pruning", R"pbdoc(Pruning subsystem)pbdoc")
        .def(py::init<>())
        .def("removeSynapse", &Pruning::removeSynapse,
             py::arg("brain"), py::arg("synapse"))
        .def("removeNeuron", &Pruning::removeNeuron,
             py::arg("brain"), py::arg("neuron"))
        .def("getPruningRate", &Pruning::getPruningRate)
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"));

    py::class_<Maturation>(m, "Maturation", R"pbdoc(Maturation subsystem)pbdoc")
        .def(py::init<>())
        .def("updateNeuronParameters", &Maturation::updateNeuronParameters,
             py::arg("neuron"), py::arg("timestep"))
        .def("getMaturationRate", &Maturation::getMaturationRate)
        .def("setMaturationRate", &Maturation::setMaturationRate, py::arg("rate"));

    // Prediction system classes
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("predict", &PredictionSystem::predict, py::arg("sensory"))
        .def("computeError", &PredictionSystem::computeError, py::arg("prediction"), py::arg("actual"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("reset", &PredictionSystem::reset);

    // Cognition system classes
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("planAction", &NeuralPlanner::planAction, py::arg("sensory"), py::arg("available_actions"))
        .def("getPlanConfidence", &NeuralPlanner::getPlanConfidence)
        .def("reset", &NeuralPlanner::reset);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation for pattern discovery)pbdoc")
        .def(py::init<>())
        .def("learnConcept", &ConceptFormation::learnConcept, py::arg("pattern"), py::arg("label"))
        .def("recognizeConcept", &ConceptFormation::recognizeConcept, py::arg("pattern"))
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("reset", &ConceptFormation::reset);

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection for focus)pbdoc")
        .def(py::init<>())
        .def("selectFocus", &AttentionalSelection::selectFocus, py::arg("sensory"), py::arg("available_targets"))
        .def("getAttentionLevel", &AttentionalSelection::getAttentionLevel)
        .def("reset", &AttentionalSelection::reset);

    // World and environment classes
    py::class_<Environment>(m, "Environment", R"pbdoc(Environment interface)pbdoc")
        .def(py::init<>())
        .def("reset", &Environment::reset)
        .def("step", &Environment::step, py::arg("world"), py::arg("action"))
        .def("getObservation", &Environment::getObservation, py::arg("world"))
        .def("isTerminal", &Environment::isTerminal)
        .def("getReward", &Environment::getReward, py::arg("world"), py::arg("action"), py::arg("nextWorld"))
        .def("getDescription", &Environment::getDescription);

    // Debug and analysis utilities
    m.def("compute_statistics", &py_utils::computeMeanStdDev,
         py::arg("data"),
         "Compute mean and standard deviation of data vector");

    m.def("normalize_vector", &py_utils::normalizeVector,
         py::arg("data"), py::arg("min") = 0.0f, py::arg("max") = 1.0f,
         "Normalize vector to [0, 1] range");

    m.def("timestamp_to_string", &py_utils::timestampToString,
         py::arg("timestamp"),
         "Convert timestamp to human-readable string");

    m.def("format_neuron_stats", &py_utils::formatNeuronStats,
         py::arg("total"), py::arg("active"), py::arg("firing"), py::arg("rate"),
         "Format neuron statistics as string");

    m.def("format_world_stats", &py_utils::formatWorldStats,
         py::arg("world"),
         "Format world statistics as string");

    // Debug utilities
    m.def("print_brain_state", &debug_utils::printBrainState,
         py::arg("brain"),
         "Print brain state to console");

    m.def("print_neuron_statistics", &debug_utils::printNeuronStatistics,
         py::arg("brain"),
         "Print detailed neuron statistics");

    m.def("write_debug_log", &debug_utils::writeDebugLog,
         py::arg("filename"), py::arg("message"),
         "Write debug message to log file");

    // Serialization utilities
    m.def("save_brain", &serialization_utils::saveBrainToFile,
         py::arg("brain"), py::arg("filepath"),
         "Save brain state to file");

    m.def("load_brain", &serialization_utils::loadBrainFromFile,
         py::arg("brain"), py::arg("filepath"),
         "Load brain state from file");

    m.def("save_config", &serialization_utils::saveConfigToJSON,
         py::arg("config"), py::arg("filepath"),
         "Save configuration to JSON file");

    m.def("load_config", &serialization_utils::loadConfigFromJSON,
         py::arg("config"), py::arg("filepath"),
         "Load configuration from JSON file");

    // Experiment utilities
    py::class_<experiment_utils::BenchmarkResult>(m, "BenchmarkResult")
        .def_readonly("name", &experiment_utils::BenchmarkResult::name)
        .def_readonly("time_seconds", &experiment_utils::BenchmarkResult::time_seconds)
        .def_readonly("steps_completed", &experiment_utils::BenchmarkResult::steps_completed)
        .def_readonly("memory_usage_mb", &experiment_utils::BenchmarkResult::memory_usage_mb)
        .def_readonly("metrics", &experiment_utils::BenchmarkResult::metrics)
        .def("toString", &experiment_utils::BenchmarkResult::toString);

    m.def("run_simulation", &experiment_utils::runSimulation,
         py::arg("brain"), py::arg("world"), py::arg("duration_seconds"),
         py::arg("target_fps") = 60,
         "Run simulation for specified duration and return benchmark results");

    // Factory functions
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
    m.attr("__description__") = "Enhanced NLM Python bindings with comprehensive API improvements";
}

} // namespace nlm