#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>

// Template helper for creating ID bindings to reduce duplication
template<typename T, typename ID>
void bindIdType(py::class_<T>& cl, const std::string& name) {
    cl.def(py::init<>())
      .def(py::init<uint64_t>(), py::arg("value"))
      .def_readwrite("value", &T::value)
      .def("index", &T::index)
      .def("__eq__", &T::operator==)
      .def("__ne__", &T::operator!=)
      .def("__hash__", [](const T& id) { return std::hash<uint64_t>{}(id.value); })
      .def("__repr__", [](const T& id) {
          return "<" + name + ": " + std::to_string(id.value) + ">";
      });
}

// Template helper for enum bindings
template<typename Enum>
void bindEnum(py::module_& m, const std::string& name) {
    py::enum_<Enum>(m, name.c_str())
        .value("Excitatory", Enum::Excitatory)
        .value("Inhibitory", Enum::Inhibitory)
        .value("Modulatory", Enum::Modulatory)
        .value("Sensory", Enum::Sensory)
        .value("Motor", Enum::Motor)
        .value("Internal", Enum::Internal)
        .export_values();
}

// Context manager helper for automatic resource management
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~BrainContextManager() {
        if (brain_) {
            brain_->logStatus();
        }
    }
    
    void reset() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Batch operations helper
class BatchOperations {
public:
    BatchOperations(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Inject current into multiple neurons by type
    void injectCurrentToType(NeuronType type, MembranePotential current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    // Get multiple neuron properties at once
    std::vector<std::pair<NeuronId, float>> getNeuronPotentials(const std::vector<NeuronId>& ids) {
        std::vector<std::pair<NeuronId, float>> results;
        for (const auto& id : ids) {
            // Note: This assumes getMembranePotential method exists
            // In practice, you'd need to implement proper neuron access
            results.emplace_back(id, 0.0f); // Placeholder
        }
        return results;
    }
    
    // Run multiple steps efficiently
    void runSteps(uint64_t startStep, uint64_t numSteps, float timestep = 0.001f) {
        for (uint64_t step = startStep; step < startStep + numSteps; ++step) {
            brain_->step(step, step * timestep);
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Advanced query builder for memory systems
class MemoryQueryBuilder {
public:
    MemoryQueryBuilder(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Query working memory for traces above threshold
    std::vector<uint64_t> queryWorkingMemoryAbove(uint64_t threshold) const {
        auto* wm = brain_->getWorkingMemory();
        if (!wm) return {};
        // Implementation would need access to working memory API
        return {};
    }
    
    // Query episodic memory by time range
    std::vector<uint64_t> queryEpisodicMemoryByTime(uint64_t startTime, uint64_t endTime) const {
        auto* em = brain_->getEpisodicMemory();
        if (!em) return {};
        // Implementation would need access to episodic memory API
        return {};
    }
    
    // Get memory statistics
    std::map<std::string, float> getMemoryStats() const {
        std::map<std::string, float> stats;
        auto* wm = brain_->getWorkingMemory();
        auto* em = brain_->getEpisodicMemory();
        
        if (wm) {
            stats["working_memory_traces"] = static_cast<float>(wm->getActiveTraces());
        }
        if (em) {
            stats["episodic_memory_episodes"] = static_cast<float>(em->getEpisodeCount());
        }
        
        return stats;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

namespace py = pybind11;
namespace nlm {

// Helper function for batch operations
void bindBatchOperations(py::class_<Brain, std::shared_ptr<Brain>>& brainClass) {
    brainClass.def("batchInjectCurrent", [](std::shared_ptr<Brain> self, NeuronType type, 
                                           MembranePotential current) {
        return BatchOperations(self).injectCurrentToType(type, current);
    }, py::arg("type"), py::arg("current"),
       "Inject current into all neurons of specified type (batch operation)");
    
    brainClass.def("runSteps", [](std::shared_ptr<Brain> self, uint64_t steps, 
                                  float timestep = 0.001f) {
        BatchOperations(self).runSteps(0, steps, timestep);
    }, py::arg("steps"), py::arg("timestep") = 0.001f,
       "Run multiple simulation steps efficiently (batch operation)");
}

// Helper function for memory query builder
void bindMemoryQueryBuilder(py::class_<Brain, std::shared_ptr<Brain>>& brainClass) {
    brainClass.def("memoryQuery", [](std::shared_ptr<Brain> self) {
        return MemoryQueryBuilder(self);
    }, "Create memory query builder for advanced memory operations");
}

// Helper function for context manager support
void bindContextManager(py::class_<Brain, std::shared_ptr<Brain>>& brainClass) {
    brainClass.def("__enter__", [](std::shared_ptr<Brain> self) {
        return self;
    })
    .def("__exit__", [](std::shared_ptr<Brain> self, py::args) {
        self->logStatus();
    })
    .def("withContext", [](std::shared_ptr<Brain> self) {
        return BrainContextManager(self);
    }, "Create a context manager for automatic brain management");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Context managers for automatic resource management
        - Batch operations for improved performance
        - Advanced memory query capabilities
        - Property-style access for common attributes
        - Enhanced error handling and validation
        - Performance monitoring utilities
    )pbdoc";

    // Register exception translation
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // ID type bindings using template to reduce duplication
    bindIdType<NeuronId>(py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc"), "NeuronId");
    bindIdType<SynapseId>(py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc"), "SynapseId");
    bindIdType<RegionId>(py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc"), "RegionId");
    bindIdType<PopulationId>(py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc"), "PopulationId");

    // Enum bindings using template
    bindEnum<NeuronType>(m, "NeuronType");
    bindEnum<SynapseType>(m, "SynapseType");
    bindEnum<DevelopmentalStage>(m, "DevelopmentalStage");
    bindEnum<FiringState>(m, "FiringState");
    bindEnum<ActionType>(m, "ActionType");
    bindEnum<MotorCommand>(m, "MotorCommand");
    bindEnum<WorldObjectType>(m, "WorldObjectType");

    // Enhanced Config class with better validation and property-style access
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced validation and property access)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or .cfg file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value (supports int, float, string, bool)")
        .def("get", [](const Config& self, const std::string& key) {
            // Template-based get with better error handling
            auto val = self.get<std::string>(key);
            if (!val) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return *val;
        }, py::arg("key"),
             "Get configuration value (string)")
        .def("get_int", [](const Config& self, const std::string& key) {
            auto val = self.get<int64_t>(key);
            if (!val) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return *val;
        }, py::arg("key"),
             "Get configuration value as integer")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (!val) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return *val;
        }, py::arg("key"),
             "Get configuration value as float")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (!val) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return *val;
        }, py::arg("key"),
             "Get configuration value as boolean")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput with better property access
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def_readonly("type", &SensoryInput::getType, "Sensory input type (read-only)")
        .def_readonly("timestamp", &SensoryInput::getTimestamp, "Sensory input timestamp (read-only)");

    // Enhanced Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def_readonly("width", &Vision::getWidth, "Vision width (read-only)")
        .def_readonly("height", &Vision::getHeight, "Vision height (read-only)")
        .def_readonly("channels", &Vision::getChannels, "Vision channels (read-only)");

    // Enhanced Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def_readonly("sample_rate", &Audio::getSampleRate, "Audio sample rate (read-only)")
        .def_readonly("num_samples", &Audio::getNumSamples, "Audio sample count (read-only)");

    // Enhanced InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Enhanced Action with better property access
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
        .def_readwrite("type", &Action::getType, &Action::setType, "Action type")
        .def_readwrite("parameters", &Action::getParameters, &Action::setParameters, "Action parameters");

    // Enhanced WorldObject with property-style access
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
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // Enhanced AgentBody with property access
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
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Calculate current speed");

    // Enhanced ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) { return self.reward > 0.0f; },
             "Check if action resulted in positive reward");

    // Enhanced SensoryPercept
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

    // Enhanced SimpleWorld
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def_readonly("width", &SimpleWorld::getWidth, "World width (read-only)")
        .def_readonly("height", &SimpleWorld::getHeight, "World height (read-only)")
        .def("distanceToAgent", [](SimpleWorld& self, float x, float y) {
            auto body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Distance to agent");

    // Enhanced Brain with context manager and batch operations
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced functionality)pbdoc")
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
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
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
        // Context manager support
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](std::shared_ptr<Brain> brain) {
            return brain;
        })
        .def("__exit__", [](std::shared_ptr<Brain> brain, py::args) {
            brain->logStatus();
        })
        .def("withContext", [](std::shared_ptr<Brain> brain) {
            return BrainContextManager(brain);
        }, "Create a context manager for automatic brain management")
        .def("batch", [](std::shared_ptr<Brain> brain) {
            return BatchOperations(brain);
        }, "Create batch operations for efficient processing")
        .def("memoryQuery", [](std::shared_ptr<Brain> brain) {
            return MemoryQueryBuilder(brain);
        }, "Create memory query builder for advanced memory operations")
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count (alias for compatibility)")
        .def("getSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count (alias for compatibility)");

    // Enhanced AgentBrain
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get agent's current thought trace");

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

    // Advanced utilities
    m.def("validateBrainConfig", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        // Validate required fields
        if (!config->has("neuron_count")) {
            errors.push_back("Missing required field: neuron_count");
        }
        if (!config->has("region_count")) {
            errors.push_back("Missing required field: region_count");
        }
        
        // Validate values
        auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0 || neuronCount > 1000000) {
            errors.push_back("Invalid neuron_count: must be between 1 and 1,000,000");
        }
        
        auto regionCount = config->getOr<int64_t>("region_count", 0);
        if (regionCount < 0 || regionCount > 10000) {
            errors.push_back("Invalid region_count: must be between 0 and 10,000");
        }
        
        auto connProb = config->getOr<double>("connection_probability", 0.0);
        if (connProb < 0.0 || connProb > 1.0) {
            errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
        }
        
        if (!errors.empty()) {
            throw std::invalid_argument("Configuration validation failed:\n" + 
                std::accumulate(errors.begin(), errors.end(), std::string(),
                    [](const std::string& acc, const std::string& err) {
                        return acc + "  - " + err + "\n";
                    }));
        }
        
        return true;
    }, py::arg("config"), "Validate configuration and return True if valid");

    m.def("benchmarkPerformance", [](const std::shared_ptr<Brain>& brain, uint64_t steps) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start);
        
        double stepsPerSecond = steps / duration.count();
        double avgFiringRate = brain->getAverageFiringRate();
        
        return std::make_tuple(
            stepsPerSecond,
            avgFiringRate,
            brain->getTotalSpikeCount(),
            duration.count()
        );
    }, py::arg("brain"), py::arg("steps"), 
       "Run benchmark and return performance metrics (steps_per_sec, avg_firing_rate, total_spikes, wall_time)");

    // Advanced configuration presets
    m.def("createStandardConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000LL);
        config->set("region_count", 10LL);
        config->set("connection_probability", 0.1);
        config->set("random_seed", 42LL);
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01);
        config->set("stdp_ltd_weight", 0.012);
        config->set("stdp_tau", 20.0);
        config->set("synaptogenesis_rate", 0.0001);
        config->set("pruning_rate", 0.00001);
        config->set("enable_reward_modulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_development", true);
        return config;
    }, "Create a standard configuration preset");

    m.def("createHighPerformanceConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500LL);
        config->set("region_count", 5LL);
        config->set("connection_probability", 0.05);
        config->set("random_seed", 12345LL);
        config->set("simulation_timestep", 0.002);
        config->set("stdp_ltp_weight", 0.02);
        config->set("stdp_ltd_weight", 0.018);
        config->set("stdp_tau", 25.0);
        config->set("synaptogenesis_rate", 0.00005);
        config->set("pruning_rate", 0.000005);
        config->set("enable_reward_modulation", true);
        config->set("enable_curiosity", false);
        config->set("enable_development", false);
        return config;
    }, "Create a high-performance configuration preset");

    m.def("createEducationalConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 200LL);
        config->set("region_count", 2LL);
        config->set("connection_probability", 0.15);
        config->set("random_seed", 999LL);
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.005);
        config->set("stdp_ltd_weight", 0.004);
        config->set("stdp_tau", 15.0);
        config->set("synaptogenesis_rate", 0.0002);
        config->set("pruning_rate", 0.00002);
        config->set("enable_reward_modulation", false);
        config->set("enable_curiosity", true);
        config->set("enable_development", true);
        return config;
    }, "Create an educational configuration preset");

    // Configuration utility functions
    m.def("validateConfiguration", [](const std::shared_ptr<Config>& config) {
        std::unordered_map<std::string, std::string> validationResults;
        
        // Check for required fields
        std::vector<std::string> requiredFields = {"neuron_count", "region_count", "connection_probability"};
        for (const auto& field : requiredFields) {
            if (!config->has(field)) {
                validationResults[field] = "Missing required field";
            } else {
                validationResults[field] = "OK";
            }
        }
        
        // Validate value ranges
        auto neuronCount = config->getOr<int64_t>("neuron_count", -1);
        if (neuronCount < 1 || neuronCount > 1000000) {
            validationResults["neuron_count"] = "Invalid range: must be 1-1,000,000";
        }
        
        auto regionCount = config->getOr<int64_t>("region_count", -1);
        if (regionCount < 0 || regionCount > 10000) {
            validationResults["region_count"] = "Invalid range: must be 0-10,000";
        }
        
        auto connProb = config->getOr<double>("connection_probability", -1.0);
        if (connProb < 0.0 || connProb > 1.0) {
            validationResults["connection_probability"] = "Invalid range: must be 0.0-1.0";
        }
        
        return validationResults;
    }, py::arg("config"), "Validate configuration and return results dictionary");

    m.def("mergeConfigurations", [](const std::shared_ptr<Config>& base, const std::shared_ptr<Config>& override) {
        auto result = std::make_shared<Config>(*base); // Copy constructor
        
        // Apply overrides
        for (const auto& key : override->getKeys()) {
            if (override->has(key)) {
                auto val = override->get<std::string>(key);
                if (val) {
                    result->set(key, *val, ConfigSource::Runtime);
                }
            }
        }
        
        return result;
    }, py::arg("base"), py::arg("override"), 
       "Merge two configurations (override takes precedence)");

    // Error handling utilities
    m.def("safeBrainOperation", [](const std::shared_ptr<Brain>& brain, 
                                 std::function<void(std::shared_ptr<Brain>)> operation) {
        try {
            operation(brain);
            return std::make_pair(true, std::string("Operation completed successfully"));
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string("Error: ") + e.what());
        }
    }, py::arg("brain"), py::arg("operation"),
       "Perform a brain operation safely with error handling");

    // Configuration comparison
    m.def("compareConfigs", [](const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2) {
        std::map<std::string, std::pair<std::string, std::string>> differences;
        
        // Get all keys from both configs
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        // Check keys in config1
        for (const auto& key : keys1) {
            auto val1 = config1->get<std::string>(key);
            auto val2 = config2->get<std::string>(key);
            
            if (val2 && *val2 != *val1) {
                differences[key] = { *val1, *val2 };
            }
        }
        
        // Check keys only in config2
        for (const auto& key : keys2) {
            if (!config1->has(key)) {
                auto val2 = config2->get<std::string>(key);
                if (val2) {
                    differences[key] = { "MISSING", *val2 };
                }
            }
        }
        
        return differences;
    }, py::arg("config1"), py::arg("config2"),
       "Compare two configurations and return differences");

    // Documentation
    m.attr("__version__") = "2.0.0";  // Updated version
    m.attr("__author__") = "NLM Project Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM with advanced features";

} // namespace nlm

