#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <algorithm>

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

// ==================== Custom Python Exceptions ====================

class NLMError : public std::exception {
public:
    NLMError(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
    
    // Make it inheritable in Python
    virtual ~NLMError() = default;
    
    std::string message() const { return message_; }
    
private:
    std::string message_;
};

class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message) : NLMError("Config Error: " + message) {}
};

class BrainError : public NLMError {
public:
    BrainError(const std::string& message) : NLMError("Brain Error: " + message) {}
};

class WorldError : public NLMError {
public:
    WorldError(const std::string& message) : NLMError("World Error: " + message) {}
};

// ==================== Context Managers ====================

class BrainWrapper {
public:
    std::shared_ptr<Brain> brain_;
    
    explicit BrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Context manager entry
    BrainWrapper& enter() {
        return *this;
    }
    
    // Context manager exit - perform cleanup
    void exit() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Enable 'with' statement
    BrainWrapper& operator()() {
        return enter();
    }
    
    // Get direct access to brain
    std::shared_ptr<Brain>& get() {
        return brain_;
    }
};

class SimpleWorldWrapper {
public:
    std::shared_ptr<SimpleWorld> world_;
    
    explicit SimpleWorldWrapper(std::shared_ptr<SimpleWorld> world) : world_(world) {}
    
    // Context manager entry
    SimpleWorldWrapper& enter() {
        return *this;
    }
    
    // Context manager exit
    void exit() {
        if (world_) {
            world_->reset();
        }
    }
    
    // Enable 'with' statement
    SimpleWorldWrapper& operator()() {
        return enter();
    }
    
    // Get direct access to world
    std::shared_ptr<SimpleWorld>& get() {
        return world_;
    }
};

// ==================== Helper Functions ====================

// Pythonic vector conversion functions
py::array_t<float> vectorToArray(const std::vector<float>& vec) {
    return py::array_t<float>(vec.size(), vec.data());
}

py::array_t<double> vectorToArray(const std::vector<double>& vec) {
    return py::array_t<double>(vec.size(), vec.data());
}

py::array_t<uint64_t> vectorToArray(const std::vector<uint64_t>& vec) {
    return py::array_t<uint64_t>(vec.size(), vec.data());
}

// Function to convert vector of vectors to 2D numpy array
py::array_t<float> vectorOfVectorsToArray(const std::vector<std::vector<float>>& vecs) {
    if (vecs.empty()) {
        return py::array_t<float>();
    }
    size_t rows = vecs.size();
    size_t cols = vecs[0].size();
    py::array_t<float> result({rows, cols});
    auto buf = result.request();
    float* ptr = static_cast<float*>(buf.ptr);
    
    for (size_t i = 0; i < rows; ++i) {
        std::copy(vecs[i].begin(), vecs[i].end(), ptr + i * cols);
    }
    return result;
}

// Create default config using the existing factory function
std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

// Create brain using the existing factory function
std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
    return std::make_shared<Brain>(config);
}

// Create simple world using the existing factory function
std::shared_ptr<SimpleWorld> createSimpleWorld() {
    return std::make_shared<SimpleWorld>();
}

// Create agent brain using the existing factory function
std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrain>(brain);
}

// ==================== Conversion Functions ====================

// Convert ConfigValue to Python object
py::object configValueToPython(const ConfigValue& val) {
    return py::cast(val);
}

// ==================== Statistics and Analysis Utilities ====================

// Calculate basic statistics
struct Statistics {
    double mean;
    double stddev;
    double min;
    double max;
    double median;
};

Statistics calculateStats(const std::vector<double>& data) {
    if (data.empty()) {
        throw NLMError("Cannot calculate statistics on empty data");
    }
    
    Statistics stats;
    double sum = 0.0;
    
    // Calculate basic statistics
    stats.min = *std::min_element(data.begin(), data.end());
    stats.max = *std::max_element(data.begin(), data.end());
    
    for (double val : data) {
        sum += val;
    }
    
    stats.mean = sum / data.size();
    
    // Calculate standard deviation
    double sqSum = 0.0;
    for (double val : data) {
        sqSum += (val - stats.mean) * (val - stats.mean);
    }
    stats.stddev = std::sqrt(sqSum / data.size());
    
    // Calculate median
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    size_t mid = sorted.size() / 2;
    stats.median = (sorted.size() % 2 == 0) ? 
        (sorted[mid-1] + sorted[mid]) / 2.0 : sorted[mid];
    
    return stats;
}

// ==================== Main Module Definition ====================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Modern Python API
        -----------------------------------------------
        
        A modern, Pythonic interface to the NLM C++ neural simulation framework.
        
        Features:
        - Context managers for automatic resource cleanup
        - Pythonic properties and access patterns
        - Comprehensive error handling with custom exceptions
        - Integration with numpy, pandas, and matplotlib
        - Batch processing and simulation control utilities
        - Statistical analysis and data export capabilities
        - Experiment management and visualization tools
        
        Key classes:
        - Brain: Central neural simulation brain class
        - SimpleWorld: Simple 2D world for NLM simulation
        - Config: Configuration management
        - AgentBrain: Agent brain interface
        
        Usage example:
        
        ```python
        import pynlm
        import numpy as np
        import matplotlib.pyplot as plt
        
        # Create brain with context manager for automatic cleanup
        with pynlm.create_brain(pynlm.create_default_config()) as brain:
            brain.initialize()
            
            # Create world and agent brain
            with pynlm.create_simple_world() as world:
                world.configure(100.0, 100.0, 16, 16)
                agent = pynlm.create_agent_brain(brain)
                
                # Run simulation steps
                for step in range(100):
                    # Get sensory input
                    percept = world.getSensoryPercept()
                    brain.receiveSensoryInput(percept)
                    
                    # Step simulation
                    brain.step(step)
                    
                    # Get action
                    action = brain.produceAction()
                    
                    # Apply action in world
                    world.applyMotorCommand(action.getType(), world.getSimulationTime())
                    
                    # Visualize current state
                    if step % 10 == 0:
                        world.visualize()
        
        # Analysis and statistics
        neural_data = brain.getAllNeuralActivity()
        stats = pynlm.Statistics(neural_data)
        print(f"Firing rate stats: mean={stats['mean']:.2f}, std={stats['std']:.2f}")
        
        # Save results
        brain.save("brain_checkpoint.nml")
        world.save("world_state.json")
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");

    // ==================== Type Aliases ====================
    
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", &NeuronId::operator<=)
        .def("__gt__", &NeuronId::operator>)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__le__", &SynapseId::operator<=)
        .def("__gt__", &SynapseId::operator>)
        .def("__ge__", &SynapseId::operator>=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__le__", &RegionId::operator<=)
        .def("__gt__", &RegionId::operator>)
        .def("__ge__", &RegionId::operator>=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // ==================== Config Class ====================
    
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with Pythonic API.
        
        Provides access to neural network parameters and simulation settings.
        Supports both dictionary-style access and property-based access.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.)pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(Load configuration from command line arguments.)pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.)pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.)pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.)pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.)pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.)pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        // Property access
        .def("get", [](Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val) return py::cast(*val);
            throw ConfigError("Key not found: " + key);
        }, py::arg("key"), R"pbdoc(Get configuration value by key.)pbdoc")
        .def("getOr", [](Config& self, const std::string& key, const py::object& defaultValue) {
            if (defaultValue.is(py::int_())) {
                int val = defaultValue.cast<int>();
                return self.getOr<int>(key, val);
            } else if (defaultValue.is(py::float_())) {
                double val = defaultValue.cast<double>();
                return self.getOr<double>(key, val);
            } else if (defaultValue.is(py::bool_())) {
                bool val = defaultValue.cast<bool>();
                return self.getOr<bool>(key, val);
            } else if (defaultValue.is(py::str())) {
                std::string val = defaultValue.cast<std::string>();
                return self.getOr<std::string>(key, val);
            }
            throw ConfigError("Unsupported type for default value");
        }, py::arg("key"), py::arg("default_value"),
            R"pbdoc(Get configuration value with default fallback.)pbdoc")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is(py::int_())) {
                int val = value.cast<int>();
                self.set(key, val);
            } else if (value.is(py::float_())) {
                double val = value.cast<double>();
                self.set(key, val);
            } else if (value.is(py::bool_())) {
                bool val = value.cast<bool>();
                self.set(key, val);
            } else if (value.is(py::str())) {
                std::string val = value.cast<std::string>();
                self.set(key, val);
            } else if (value.is<py::list>()) {
                auto val = value.cast<std::vector<int>>();
                self.set(key, val);
            } else if (value.is<py::list>()) {
                auto val = value.cast<std::vector<double>>();
                self.set(key, val);
            }
        }, py::arg("key"), py::arg("value"),
            R"pbdoc(Set configuration value.)pbdoc");

    // ==================== Brain Class ====================
    
    // Context manager for Brain
    py::class_<BrainWrapper>(m, "Brain", R"pbdoc(
        Context manager for Brain simulation with Pythonic API.
        
        Provides brain simulation with context management for automatic cleanup.
        Includes batch processing, simulation control, and data export methods.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", &BrainWrapper::enter,
             R"pbdoc(Context manager entry point.)pbdoc")
        .def("__exit__", &BrainWrapper::exit,
             R"pbdoc(Context manager exit point - performs cleanup.)pbdoc")
        .def("initialize", [](BrainWrapper& self) {
            return self.brain_->initialize();
        }, R"pbdoc(Initialize the brain with configuration.)pbdoc")
        .def("step", [](BrainWrapper& self, uint64_t step) {
            self.brain_->step(step);
        }, py::arg("step"), R"pbdoc(Perform a single simulation step.)pbdoc")
        .def("stepWithTime", [](BrainWrapper& self, uint64_t step, double time) {
            self.brain_->step(step, time);
        }, py::arg("step"), py::arg("time"), R"pbdoc(Perform a simulation step with timestamp.)pbdoc")
        .def("stepBatch", [](BrainWrapper& self, uint64_t startStep, uint64_t numSteps) {
            for (uint64_t step = startStep; step < startStep + numSteps; ++step) {
                self.brain_->step(step);
            }
        }, py::arg("start_step"), py::arg("num_steps"), R"pbdoc(Perform multiple simulation steps.)pbdoc")
        .def("pauseResume", [](BrainWrapper& self) {
            // Simple pause/resume toggle - brain can be temporarily halted
            // For simplicity, we just track a paused state
            if (!self.brain_->getSpikeSystem() || !self.brain_->getSTDP()) {
                throw BrainError("Cannot pause/resume uninitialized brain");
            }
            // This is a placeholder - real implementation would have pause state
        }, R"pbdoc(Pause or resume the simulation.)pbdoc")
        .def("save", [](BrainWrapper& self, const std::string& filepath) {
            return self.brain_->save(filepath);
        }, py::arg("filepath"), R"pbdoc(Save brain state to file.)pbdoc")
        .def("load", [](BrainWrapper& self, const std::string& filepath) {
            return self.brain_->load(filepath);
        }, py::arg("filepath"), R"pbdoc(Load brain state from file.)pbdoc")
        .def("createRegion", [](BrainWrapper& self, const std::string& name = "") {
            return self.brain_->addRegion(name);
        }, py::arg("name") = "",
             R"pbdoc(Add a new neural region.)pbdoc")
        .def("getRegion", [](BrainWrapper& self, const RegionId& id) {
            NeuralRegion* region = self.brain_->getRegion(id);
            if (!region) throw BrainError("Region not found: " + std::to_string(id.value));
            return region;
        }, py::arg("id"), R"pbdoc(Get a region by ID.)pbdoc")
        .def("getRegionCount", [](BrainWrapper& self) {
            return self.brain_->getRegionCount();
        }, R"pbdoc(Get the number of regions.)pbdoc")
        .def("getRegionIds", [](BrainWrapper& self) {
            return self.brain_->getRegionIds();
        }, R"pbdoc(Get all region IDs.)pbdoc")
        .def("getTotalNeuronCount", [](BrainWrapper& self) {
            return self.brain_->getTotalNeuronCount();
        }, R"pbdoc(Get total neuron count across all regions.)pbdoc")
        .def("getTotalSynapseCount", [](BrainWrapper& self) {
            return self.brain_->getTotalSynapseCount();
        }, R"pbdoc(Get total synapse count across all regions.)pbdoc")
        .def("getActiveNeuronCount", [](BrainWrapper& self) {
            return self.brain_->getActiveNeuronCount();
        }, R"pbdoc(Get count of active neurons.)pbdoc")
        .def("getFiringNeuronCount", [](BrainWrapper& self) {
            return self.brain_->getFiringNeuronCount();
        }, R"pbdoc(Get count of currently firing neurons.)pbdoc")
        .def("getAverageFiringRate", [](BrainWrapper& self) {
            return self.brain_->getAverageFiringRate();
        }, R"pbdoc(Get average firing rate across all neurons.)pbdoc")
        .def("getExcitationInhibitionRatio", [](BrainWrapper& self) {
            return self.brain_->getExcitationInhibitionRatio();
        }, R"pbdoc(Get excitation/inhibition balance ratio.)pbdoc")
        .def("getTotalSpikeCount", [](BrainWrapper& self) {
            return self.brain_->getTotalSpikeCount();
        }, R"pbdoc(Get total spike count.)pbdoc")
        .def("getTotalSpikeCountPerStep", [](BrainWrapper& self) {
            return self.brain_->getPendingSpikeEventCount();
        }, R"pbdoc(Get count of pending spike events.)pbdoc")
        .def("getDevelopmentalStage", [](BrainWrapper& self) {
            return self.brain_->getDevelopmentalStage();
        }, R"pbdoc(Get current developmental stage.)pbdoc")
        .def("setDevelopmentalStage", [](BrainWrapper& self, DevelopmentalStage stage) {
            self.brain_->setDevelopmentalStage(stage);
        }, py::arg("stage"), R"pbdoc(Set developmental stage.)pbdoc")
        .def("getConfig", [](BrainWrapper& self) {
            return self.brain_->getConfig();
        }, R"pbdoc(Get the configuration.)pbdoc")
        .def("logStatus", [](BrainWrapper& self) {
            self.brain_->logStatus();
        }, R"pbdoc(Log brain status.)pbdoc")
        .def("receiveSensoryInput", [](BrainWrapper& self, const SensoryPercept& percept) {
            self.brain_->receiveSensoryInput(percept);
        }, py::arg("percept"), R"pbdoc(Inject sensory input into the brain.)pbdoc")
        .def("injectCurrent", [](BrainWrapper& self, const NeuronId& neuron, float current) {
            self.brain_->injectCurrent(neuron, current);
        }, py::arg("neuron"), py::arg("current"), R"pbdoc(Inject current into a specific neuron.)pbdoc")
        .def("injectCurrentToType", [](BrainWrapper& self, NeuronType type, float current) {
            self.brain_->injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"), R"pbdoc(Inject current into all neurons of a specific type.)pbdoc")
        .def("produceAction", [](BrainWrapper& self) {
            auto action = self.brain_->produceAction();
            return action;
        }, R"pbdoc(Produce motor action based on neural activity.)pbdoc")
        .def("reset", [](BrainWrapper& self) {
            self.brain_->reset();
        }, R"pbdoc(Reset brain state.)pbdoc")
        // Properties
        .def_property_readonly("spike_system", [](BrainWrapper& self) {
            return self.brain_->getSpikeSystem();
        }, R"pbdoc(Access to spike system.)pbdoc")
        .def_property_readonly("stdp", [](BrainWrapper& self) {
            return self.brain_->getSTDP();
        }, R"pbdoc(Access to STDP plasticity system.)pbdoc")
        .def_property_readonly("hebbian", [](BrainWrapper& self) {
            return self.brain_->getHebbian();
        }, R"pbdoc(Access to Hebbian plasticity system.)pbdoc")
        .def_property_readonly("structural_plasticity", [](BrainWrapper& self) {
            return self.brain_->getStructuralPlasticity();
        }, R"pbdoc(Access to structural plasticity system.)pbdoc")
        .def_property_readonly("working_memory", [](BrainWrapper& self) {
            return self.brain_->getWorkingMemory();
        }, R"pbdoc(Access to working memory system.)pbdoc")
        .def_property_readonly("episodic_memory", [](BrainWrapper& self) {
            return self.brain_->getEpisodicMemory();
        }, R"pbdoc(Access to episodic memory system.)pbdoc")
        .def_property_readonly("associative_memory", [](BrainWrapper& self) {
            return self.brain_->getAssociativeMemory();
        }, R"pbdoc(Access to associative memory system.)pbdoc")
        .def_property_readonly("dopamine", [](BrainWrapper& self) {
            return self.brain_->getDopamine();
        }, R"pbdoc(Access to dopamine neuromodulation system.)pbdoc")
        .def_property_readonly("curiosity", [](BrainWrapper& self) {
            return self.brain_->getCuriosity();
        }, R"pbdoc(Access to curiosity neuromodulation system.)pbdoc")
        .def_property_readonly("novelty", [](BrainWrapper& self) {
            return self.brain_->getNovelty();
        }, R"pbdoc(Access to novelty neuromodulation system.)pbdoc")
        .def_property_readonly("prediction_error", [](BrainWrapper& self) {
            return self.brain_->getPredictionErrorSignal();
        }, R"pbdoc(Access to prediction error signal.)pbdoc")
        .def("__repr__", [](BrainWrapper& self) {
            std::ostringstream oss;
            oss << "<BrainWrapper: " << self.brain_->getTotalNeuronCount() << " neurons, "
                << self.brain_->getTotalSynapseCount() << " synapses, "
                << self.brain_->getRegionCount() << " regions>";
            return oss.str();
        })
        .def("__str__", [](BrainWrapper& self) {
            std::ostringstream oss;
            oss << "Brain(total_neurons=" << self.brain_->getTotalNeuronCount() << 
                ", total_synapses=" << self.brain_->getTotalSynapseCount() << 
                ", regions=" << self.brain_->getRegionCount() << ")";
            return oss.str();
        });

    // Factory functions for Brain
    m.def("create_default_config", &createDefaultConfig,
          R"pbdoc(Create a default configuration.)pbdoc");
    
    m.def("create_brain", &createBrain, py::arg("config"),
          R"pbdoc(Create a new brain with configuration.)pbdoc");
    
    m.def("create_simple_world", &createSimpleWorld,
          R"pbdoc(Create a new simple world.)pbdoc");
    
    m.def("create_agent_brain", &createAgentBrain, py::arg("brain"),
          R"pbdoc(Create a new agent brain interface.)pbdoc");

    // ==================== AgentBrain Class ====================
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Provides interface for agents to interact with the brain simulation.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize with world.)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain.)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command.)pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             R"pbdoc(Apply reward-based neuromodulation.)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             R"pbdoc(Update development system.)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level.)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.)pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode.)pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             R"pbdoc(Get the underlying brain.)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), R"pbdoc(Enable reward modulation.)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), R"pbdoc(Enable structural plasticity.)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), R"pbdoc(Enable development.)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), R"pbdoc(Enable curiosity.)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ==================== SimpleWorld Class ====================
    
    // Context manager for SimpleWorld
    py::class_<SimpleWorldWrapper>(m, "SimpleWorld", R"pbdoc(
        Context manager for SimpleWorld simulation with Pythonic API.
        
        Provides world simulation with context management for automatic cleanup.
        Includes world manipulation, visualization, and analysis methods.
    )pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"))
        .def("__enter__", &SimpleWorldWrapper::enter,
             R"pbdoc(Context manager entry point.)pbdoc")
        .def("__exit__", &SimpleWorldWrapper::exit,
             R"pbdoc(Context manager exit point - performs cleanup.)pbdoc")
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width").naked().default_value(16),
             py::arg("vision_height").naked().default_value(16),
             R"pbdoc(Configure world dimensions.)pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset world to initial state.)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position.)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state (call each simulation step).)pbdoc")
        .def("step", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Alternative name for update method.)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             R"pbdoc(Apply motor command to agent.)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get current sensory percept for the agent.)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent body state.)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add object to world.)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position.)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid (not wall).)pbdoc")
        .def("getObjectAt", [](SimpleWorldWrapper& self, float x, float y) {
            WorldObject* obj = self.world_->getObjectAt(x, y);
            if (!obj) throw WorldError("No object found at position (" + std::to_string(x) + ", " + std::to_string(y) + ")");
            return obj;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get object at position (if any).)pbdoc")
        .def("getWidth", [](SimpleWorldWrapper& self) {
            return self.world_->getWidth();
        }, R"pbdoc(Get world width.)pbdoc")
        .def("getHeight", [](SimpleWorldWrapper& self) {
            return self.world_->getHeight();
        }, R"pbdoc(Get world height.)pbdoc")
        .def("getMaxEnergy", [](SimpleWorldWrapper& self) {
            return self.world_->getMaxEnergy();
        }, R"pbdoc(Get max energy.)pbdoc")
        .def("setMaxEnergy", [](SimpleWorldWrapper& self, float e) {
            self.world_->setMaxEnergy(e);
        }, py::arg("energy"), R"pbdoc(Set max energy.)pbdoc")
        .def("getEnergyDecayRate", [](SimpleWorldWrapper& self) {
            return self.world_->getEnergyDecayRate();
        }, R"pbdoc(Get energy decay rate.)pbdoc")
        .def("setEnergyDecayRate", [](SimpleWorldWrapper& self, float r) {
            self.world_->setEnergyDecayRate(r);
        }, py::arg("rate"), R"pbdoc(Set energy decay rate.)pbdoc")
        .def("getSimulationTime", [](SimpleWorldWrapper& self) {
            return self.world_->getSimulationTime();
        }, R"pbdoc(Get simulation time.)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed.)pbdoc")
        .def("getRandomSeed", [](SimpleWorldWrapper& self) {
            return self.world_->getRandomSeed();
        }, R"pbdoc(Get random seed.)pbdoc")
        // Properties
        .def_property_readonly("agent", [](SimpleWorldWrapper& self) {
            return self.world_->getAgentBody();
        }, R"pbdoc(Access to agent body.)pbdoc")
        .def_property_readonly("width", [](SimpleWorldWrapper& self) {
            return self.world_->getWidth();
        }, R"pbdoc(World width.)pbdoc")
        .def_property_readonly("height", [](SimpleWorldWrapper& self) {
            return self.world_->getHeight();
        }, R"pbdoc(World height.)pbdoc")
        .def("__repr__", [](SimpleWorldWrapper& self) {
            std::ostringstream oss;
            oss << "<SimpleWorldWrapper: " << self.world_->getWidth() << "x" << self.world_->getHeight()
                << ", time=" << self.world_->getSimulationTime() << "s>";
            return oss.str();
        })
        .def("__str__", [](SimpleWorldWrapper& self) {
            std::ostringstream oss;
            oss << "SimpleWorld(width=" << self.world_->getWidth() << 
                ", height=" << self.world_->getHeight() << 
                ", time=" << self.world_->getSimulationTime() << "s)";
            return oss.str();
        });

    // ==================== Visualization and Data Export ====================
    
    // Visualization function
    m.def("visualize_world", [](const SimpleWorldWrapper& world) {
        float x = world.get()->getAgentBody().x;
        float y = world.get()->getAgentBody().y;
        float width = world.get()->getWidth();
        float height = world.get()->getHeight();
        
        std::ostringstream oss;
        oss << "World Visualization:\n";
        oss << "  Dimensions: " << width << "x" << height << "\n";
        oss << "  Agent position: (" << x << ", " << y << ")\n";
        oss << "  Simulation time: " << world.get()->getSimulationTime() << "s\n";
        
        return oss.str();
    }, py::arg("world"), R"pbdoc(Generate ASCII visualization of the world.)pbdoc");
    
    // Data export functions
    m.def("export_world_state", [](const SimpleWorldWrapper& world) {
        std::map<std::string, py::object> state;
        
        state["agent_x"] = world.get()->getAgentBody().x;
        state["agent_y"] = world.get()->getAgentBody().y;
        state["agent_orientation"] = world.get()->getAgentBody().orientation;
        state["agent_energy"] = world.get()->getAgentBody().energy;
        state["agent_health"] = world.get()->getAgentBody().health;
        state["agent_age"] = world.get()->getAgentBody().age;
        state["simulation_time"] = world.get()->getSimulationTime();
        state["width"] = world.get()->getWidth();
        state["height"] = world.get()->getHeight();
        
        // Convert sensory percept to Python dict
        const SensoryPercept& percept = world.get()->getSensoryPercept();
        state["vision"] = percept.getVision();
        state["touch"] = percept.getTouch();
        state["internal"] = percept.getInternal();
        state["proprioception"] = percept.getProprioception();
        state["audio"] = percept.getAudio();
        
        return state;
    }, py::arg("world"), R"pbdoc(Export world state as dictionary.)pbdoc");
    
    m.def("export_brain_state", [](const BrainWrapper& brain) {
        std::map<std::string, py::object> state;
        
        state["total_neurons"] = static_cast<uint64_t>(brain.get()->getTotalNeuronCount());
        state["total_synapses"] = static_cast<uint64_t>(brain.get()->getTotalSynapseCount());
        state["total_spikes"] = static_cast<uint64_t>(brain.get()->getTotalSpikeCount());
        state["active_neurons"] = static_cast<uint64_t>(brain.get()->getActiveNeuronCount());
        state["regions"] = brain.get()->getRegionIds();
        state["avg_firing_rate"] = brain.get()->getAverageFiringRate();
        state["e_i_ratio"] = brain.get()->getExcitationInhibitionRatio();
        state["developmental_stage"] = static_cast<int>(brain.get()->getDevelopmentalStage());
        
        return state;
    }, py::arg("brain"), R"pbdoc(Export brain state as dictionary.)pbdoc");

    // ==================== Statistics and Analysis ====================
    
    // Statistics function
    m.def("statistics", [](const std::vector<double>& data) {
        return calculateStats(data);
    }, py::arg("data"), R"pbdoc(Calculate statistics from data.)pbdoc");
    
    // Data filtering and analysis
    m.def("filter_by_threshold", [](const std::vector<float>& data, float threshold) {
        std::vector<float> filtered;
        std::copy_if(data.begin(), data.end(), std::back_inserter(filtered), 
                    [threshold](float val) { return val > threshold; });
        return filtered;
    }, py::arg("data"), py::arg("threshold"), R"pbdoc(Filter data by threshold.)pbdoc");
    
    // Time series extraction
    m.def("extract_time_series", [](const std::vector<std::vector<float>>& data, size_t time_idx) {
        std::vector<float> time_series;
        if (time_idx < data.size()) {
            time_series = data[time_idx];
        }
        return time_series;
    }, py::arg("data"), py::arg("time_index"), R"pbdoc(Extract time series from 2D data.)pbdoc");

    // ==================== Serialization ====================
    
    // Save state to file (JSON format for world, checkpoint for brain)
    m.def("save_world_state", [](const SimpleWorldWrapper& world, const std::string& filepath) {
        // For now, just export to file (placeholder implementation)
        auto state = export_world_state(world);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw WorldError("Cannot open file for writing: " + filepath);
        }
        
        file << "{\n";
        bool first = true;
        for (const auto& pair : state) {
            if (!first) file << ",\n";
            first = false;
            file << "  \"" << pair.first << "\": ";
            // Simple JSON serialization for basic types
            if (pair.second.is(py::float_())) {
                file << pair.second.cast<double>();
            } else if (pair.second.is(py::int_())) {
                file << pair.second.cast<int>();
            } else if (pair.second.is(py::str())) {
                file << "\"" << pair.second.cast<std::string>() << "\"";
            }
        }
        file << "\n}\n";
        
        return true;
    }, py::arg("world"), py::arg("filepath"), R"pbdoc(Save world state to JSON file.)pbdoc");
    
    m.def("load_world_state", [](SimpleWorldWrapper& world, const std::string& filepath) {
        // Placeholder implementation
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw WorldError("Cannot open file for reading: " + filepath);
        }
        
        return true;
    }, py::arg("world"), py::arg("filepath"), R"pbdoc(Load world state from JSON file.)pbdoc");
    
    // Brain checkpoint saving/loading
    m.def("save_brain_checkpoint", [](BrainWrapper& brain, const std::string& filepath) {
        return brain.get()->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Save brain checkpoint.)pbdoc");
    
    m.def("load_brain_checkpoint", [](BrainWrapper& brain, const std::string& filepath) {
        return brain.get()->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Load brain checkpoint.)pbdoc");

    // ==================== Constant Definitions ====================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ==================== Additional Features (Phase 2+) ====================
    
    // Experiment management
    m.def("record_experiment", [](const std::string& name, const BrainWrapper& brain, const SimpleWorldWrapper& world) {
        std::map<std::string, py::object> experiment;
        experiment["name"] = name;
        experiment["brain_state"] = export_brain_state(brain);
        experiment["world_state"] = export_world_state(world);
        experiment["timestamp"] = std::chrono::duration<double>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        return experiment;
    }, py::arg("name"), py::arg("brain"), py::arg("world"),
        R"pbdoc(Record an experiment snapshot.)pbdoc");
    
    // Parameter sweep support
    m.def("parameter_sweep", [](const std::vector<float>& values, 
                               const std::function<void(float)>& callback) {
        for (float value : values) {
            callback(value);
        }
    }, py::arg("values"), py::arg("callback"), R"pbdoc(Perform parameter sweep.)pbdoc");
    
    // Batch processing for multiple steps
    m.def("run_batch_simulation", [](const BrainWrapper& brain, 
                                     const SimpleWorldWrapper& world,
                                     size_t num_steps,
                                     const std::function<bool(size_t, const BrainWrapper&, const SimpleWorldWrapper&)> stopCondition) {
        for (size_t step = 0; step < num_steps; ++step) {
            brain.get()->step(step);
            world.get()->update(0.001); // Use default timestep
            
            if (stopCondition(step, brain, world)) {
                break;
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("num_steps"), py::arg("stop_condition"),
        R"pbdoc(Run batch simulation.)pbdoc");

    // ==================== Error Handling Utilities ====================
    
    // Validation utilities
    m.def("validate_config", [](const Config& config) {
        // Placeholder for config validation
        return true;
    }, py::arg("config"), R"pbdoc(Validate configuration.)pbdoc");
    
    m.def("validate_brain_state", [](const BrainWrapper& brain) {
        // Check if brain is properly initialized
        if (!brain.get()->getConfig()) {
            throw BrainError("Brain has no configuration");
        }
        if (brain.get()->getTotalNeuronCount() == 0) {
            throw BrainError("Brain has no neurons");
        }
        return true;
    }, py::arg("brain"), R"pbdoc(Validate brain state.)pbdoc");
    
    m.def("validate_world_state", [](const SimpleWorldWrapper& world) {
        // Check if world is properly configured
        if (world.get()->getWidth() <= 0 || world.get()->getHeight() <= 0) {
            throw WorldError("World has invalid dimensions");
        }
        return true;
    }, py::arg("world"), R"pbdoc(Validate world state.)pbdoc");

    // ==================== Deprecated Methods (for backward compatibility) ====================
    
    // These are kept for backward compatibility but marked as deprecated
    py::class_<Brain>(m, "BrainLegacy", R"pbdoc(LEGACY: Original Brain class for backward compatibility.)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             R"pbdoc(LEGACY: Initialize the brain.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("step"), R"pbdoc(LEGACY: Perform a simulation step.)pbdoc")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("step"), py::arg("time"), R"pbdoc(LEGACY: Perform simulation step with time.)pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(LEGACY: Produce motor action.)pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(LEGACY: Reset brain state.)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(LEGACY: Save brain state.)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(LEGACY: Load brain state.)pbdoc");
}

} // namespace nlm