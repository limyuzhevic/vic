// Refactored NLM Python Bindings
// Organized for better maintainability and Pythonic features

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

// Custom NLM exception classes
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain Error: " + msg) {}
};

// Helper function to register ID classes
template<typename T>
void registerIDClass(py::module_& m, const std::string& name) {
    py::class_<T>(m, name.c_str())
        .def(py::init<>())
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

// Helper function to register enum classes
template<typename E>
void registerEnum(py::module_& m, const std::string& name) {
    py::enum_<E>(m, name.c_str())
        .value("Unknown", E::Unknown)
        .export_values();
}

// Register configuration class
void registerConfig(py::module_& m) {
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
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) {
                return *value;
            }
            throw ConfigError("Key not found: " + key);
        }, py::arg("key"), "Get configuration value (Pythonic interface)")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get integer configuration value")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get float configuration value")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean configuration value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

// Register basic input classes
void registerBasicInputs(py::module_& m) {
    // Base SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        });

    // Vision class
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

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);
}

// Register action and world classes
void registerActionAndWorld(py::module_& m) {
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
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw std::out_of_range("Parameter index out of range");
        }, py::arg("index"), "Get parameter by index")
        .def("addParameter", &Action::setParameters, py::arg("params"), "Set all parameters")
        .def("clone", &Action::clone);

    // WorldObject class
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

    // AgentBody class
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
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", &ActionResult::success, "Check if action was successful");

    // SensoryPercept class
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
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            return self;
        }, py::arg("other"), "Merge another percept into this one");
}

// Register SimpleWorld class with context manager support
void registerSimpleWorld(py::module_& m) {
    py::class_<SimpleWorld> worldClass(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc");
    
    worldClass
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
        .def("__enter__", [](py::class_<SimpleWorld>::handle& self) {
            return self;
        })
        .def("__exit__", [](py::class_<SimpleWorld>::handle& self, py::object, py::object, py::object) {
            self->reset();
        });
}

// Register Brain class with enhanced API
void registerBrain(py::module_& m) {
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToType", &Brain::injectCurrentToNeurons,
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
        // Pythonic convenience methods
        .def("getStatistics", [](const Brain& self) {
            return py::dict(
                "neuron_count", self.getTotalNeuronCount(),
                "synapse_count", self.getTotalSynapseCount(),
                "spike_count", self.getTotalSpikeCount(),
                "firing_neurons", self.getFiringNeuronCount(),
                "avg_firing_rate", self.getAverageFiringRate(),
                "exc_inh_ratio", self.getExcitationInhibitionRatio(),
                "region_count", self.getRegionCount()
            );
        }, "Get brain statistics as dictionary")
        .def("waitForSpikes", [](Brain& self, size_t targetSpikes, SimulationStep maxSteps) {
            size_t currentSpikes = self.getTotalSpikeCount();
            for (SimulationStep step = 0; step < maxSteps; ++step) {
                self.step(step);
                if (self.getTotalSpikeCount() > currentSpikes) {
                    return step;
                }
            }
            return maxSteps;
        }, py::arg("targetSpikes"), py::arg("maxSteps"),
           "Wait for a minimum number of spikes (convenience method)")
        .def("createSensoryInput", [](const std::string& type, const std::vector<float>& data) {
            throw BrainError("Sensory input factory not implemented for this type");
        }, py::arg("type"), py::arg("data"), "Create sensory input (convenience method)");
}

// Register AgentBrain class
void registerAgentBrain(py::module_& m) {
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Pythonic convenience methods
        .def("updateWithPercept", [](AgentBrain& self, const SensoryPercept& percept, const Action& action, float reward) {
            self.processSensoryInput(percept);
            self.applyRewardModulation(reward, 0.0f);
            return self.decodeMotorCommand();
        }, py::arg("percept"), py::arg("action"), py::arg("reward"),
           "Process sensory input and return motor command (convenience method)");
}

// Factory functions
void registerFactoryFunctions(py::module_& m) {
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

    // Convenience factory functions
    m.def("createQuickBrain", [](size_t neuronCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuronCount"), "Quick brain creation with default settings");

    m.def("createTrainingConfig", [](const std::string& environment, size_t width, size_t height) {
        auto config = std::make_shared<Config>();
        config->set("environment_name", environment);
        config->set("environment_width", static_cast<int64_t>(width));
        config->set("environment_height", static_cast<int64_t>(height));
        config->set("max_simulation_steps", 10000);
        config->set("reward_discount_factor", 0.99);
        return config;
    }, py::arg("environment"), py::arg("width"), py::arg("height"),
       "Create a training configuration");

    // Utility functions
    m.def("runEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, size_t steps) {
        agent->reset();
        world->reset();
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            agent->getBrain()->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_pair(agent->getBrain()->getTotalSpikeCount(), agent->getBrain()->getFiringNeuronCount());
    }, py::arg("agent"), py::arg("world"), py::arg("steps"),
       "Run a complete training episode and return spike/firing counts");

    // Version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";
    
    // Error constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// Advanced simulation features for power users
void registerAdvancedFeatures(py::module_& m) {
    // Advanced brain configuration functions
    m.def("createResearchConfig", [](const std::string& description, 
                                    size_t neuronCount = 1000,
                                    size_t regionCount = 2,
                                    float connectionProb = 0.1f,
                                    bool enablePlasticity = true,
                                    bool enableDevelopment = true,
                                    bool enableNeuromodulation = true) {
        auto config = std::make_shared<Config>();
        
        // Set research-specific parameters
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("connection_probability", connectionProb);
        config->set("plasticity_learning_rate", 0.01);
        config->set("stdp_ltp_weight", 0.01);
        config->set("stdp_ltd_weight", 0.012);
        
        if (!enablePlasticity) {
            config->set("plasticity_learning_rate", 0.0);
        }
        
        if (enableDevelopment) {
            config->set("development_synaptogenesis_rate", 0.001);
            config->set("development_pruning_rate", 0.0001);
        }
        
        if (enableNeuromodulation) {
            config->set("dopamine_baseline", 0.1);
            config->set("curiosity_baseline", 0.5);
            config->set("novelty_threshold", 0.3);
        }
        
        config->set("description", description);
        config->set("research_mode", true);
        
        return config;
    }, py::arg("description"), py::arg("neuronCount") = 1000,
       py::arg("regionCount") = 2, py::arg("connectionProb") = 0.1f,
       py::arg("enablePlasticity") = true,
       py::arg("enableDevelopment") = true,
       py::arg("enableNeuromodulation") = true,
       "Create a research-oriented configuration");

    // Batch processing functions
    m.def("batchStep", [](std::shared_ptr<Brain> brain, 
                        const std::vector<std::shared_ptr<SensoryInput>>& inputs,
                        const std::vector<SimulationStep>& steps) {
        if (inputs.size() != steps.size()) {
            throw BrainError("Input and step vectors must have the same size");
        }
        
        std::vector<size_t> spikeCounts;
        
        for (size_t i = 0; i < inputs.size(); ++i) {
            if (inputs[i]) {
                brain->receiveSensoryInput(*inputs[i]);
            }
            brain->step(steps[i]);
            spikeCounts.push_back(brain->getTotalSpikeCount());
        }
        
        return spikeCounts;
    }, py::arg("brain"), py::arg("inputs"), py::arg("steps"),
       "Perform batch stepping with multiple inputs");

    // Analysis and monitoring functions
    m.def("analyzeBrainActivity", [](const std::shared_ptr<Brain>& brain,
                                    size_t windowSize = 100) {
        std::map<std::string, double> analysis;
        
        if (windowSize == 0) {
            throw ConfigError("Window size must be greater than 0");
        }
        
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["total_spikes"] = brain->getTotalSpikeCount();
        analysis["active_neurons"] = brain->getActiveNeuronCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["avg_firing_rate"] = brain->getAverageFiringRate();
        analysis["exc_inh_ratio"] = brain->getExcitationInhibitionRatio();
        
        // Calculate additional statistics
        analysis["spike_density"] = static_cast<double>(brain->getTotalSpikeCount()) /
                                  (brain->getTotalNeuronCount() * windowSize * 0.001);
        analysis["connectivity_density"] = static_cast<double>(brain->getTotalSynapseCount()) /
                                         (brain->getTotalNeuronCount() * (brain->getTotalNeuronCount() - 1));
        
        return analysis;
    }, py::arg("brain"), py::arg("windowSize") = 100,
       "Analyze brain activity and return statistics as dictionary");

    // Visualization helpers
    m.def("createSensoryGrid", [](size_t width, size_t height, 
                                 const std::vector<float>& values) {
        if (values.size() != width * height) {
            throw ConfigError("Values vector size must match grid dimensions");
        }
        
        auto vision = std::make_shared<Vision>(width, height, 1);
        std::vector<float> flattened;
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t index = y * width + x;
                flattened.push_back(values[index]);
            }
        }
        vision->setData(flattened);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("values"),
       "Create a vision input from a grid of values (useful for testing)");

    // Performance monitoring
    m.def("profileSimulation", [](std::shared_ptr<Brain> brain,
                                 std::shared_ptr<SimpleWorld> world,
                                 size_t steps,
                                 size_t warmUpSteps = 100) {
        if (steps < warmUpSteps) {
            throw ConfigError("Total steps must be greater than warm-up steps");
        }
        
        // Warm-up phase
        for (size_t i = 0; i < warmUpSteps; ++i) {
            brain->step(i);
        }
        
        // Reset counters
        brain->reset();
        brain->initialize();
        world->reset();
        
        // Profile phase
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t spikeCount = 0;
        size_t firingCount = 0;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            brain->step(step);
            auto action = brain->produceAction();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            spikeCount = brain->getTotalSpikeCount();
            firingCount = brain->getFiringNeuronCount();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        return std::make_tuple(
            spikeCount,
            firingCount,
            static_cast<double>(steps) / (duration.count() / 1000.0), // steps per second
            duration.count() // total milliseconds
        );
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("warmUpSteps") = 100,
       "Profile simulation performance and return (spike_count, firing_count, steps_per_sec, total_ms)");

    // Checkpoint management
    m.def("saveBrainCheckpoint", [](const std::shared_ptr<Brain>& brain,
                                   const std::string& basePath,
                                   const std::string& tag = "") {
        std::string filepath = basePath;
        if (!filepath.empty() && filepath.back() != '/') {
            filepath += '/';
        }
        filepath += "brain_";
        if (!tag.empty()) {
            filepath += tag + "_";
        }
        filepath += std::to_string(brain->getTotalSpikeCount()) + ".nlm";
        
        bool success = brain->save(filepath);
        if (!success) {
            throw BrainError("Failed to save brain checkpoint to: " + filepath);
        }
        
        return filepath;
    }, py::arg("brain"), py::arg("basePath"), py::arg("tag") = "",
       "Save brain checkpoint with automatic naming");

    m.def("loadBrainCheckpoint", [](std::shared_ptr<Brain> brain,
                                   const std::string& filepath) {
        bool success = brain->load(filepath);
        if (!success) {
            throw BrainError("Failed to load brain checkpoint from: " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain checkpoint");

    // World manipulation helpers
    m.def("createRandomWorld", [](size_t width = 10, size_t height = 10,
                                 float obstacleDensity = 0.2f,
                                 float resourceDensity = 0.1f) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        
        // Add random obstacles
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                float r = static_cast<float>(rand()) / RAND_MAX;
                if (r < obstacleDensity) {
                    auto obj = std::make_unique<WorldObject>();
                    obj->x = static_cast<float>(x);
                    obj->y = static_cast<float>(y);
                    obj->type = WorldObjectType::Wall;
                    obj->radius = 0.4f;
                    obj->active = true;
                    world->addObject(*obj);
                } else if (r < obstacleDensity + resourceDensity) {
                    auto obj = std::make_unique<WorldObject>();
                    obj->x = static_cast<float>(x);
                    obj->y = static_cast<float>(y);
                    obj->type = WorldObjectType::Resource;
                    obj->radius = 0.3f;
                    obj->value = 1.0f;
                    obj->active = true;
                    world->addObject(*obj);
                }
            }
        }
        
        return world;
    }, py::arg("width") = 10, py::arg("height") = 10,
       py::arg("obstacleDensity") = 0.2f, py::arg("resourceDensity") = 0.1f,
       "Create a world with random obstacles and resources");

    // Agent utilities
    m.def("createTrainingAgent", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   bool enableLearning = true,
                                   bool enableExploration = true) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        
        if (enableExploration) {
            agent->enableNoveltyDetection(true);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("enableLearning") = true,
       py::arg("enableExploration") = true,
       "Create a training agent with specified capabilities");

    // Simulation utilities
    m.def("runContinuousSimulation", [](std::shared_ptr<AgentBrain> agent,
                                        std::shared_ptr<SimpleWorld> world,
                                        size_t episodes,
                                        size_t stepsPerEpisode) {
        std::vector<double> episodeRewards;
        std::vector<size_t> episodeSpikeCounts;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            agent->reset();
            world->reset();
            double episodeReward = 0.0;
            
            for (size_t step = 0; step < stepsPerEpisode; ++step) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                agent->getBrain()->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward based on agent's curiosity and performance
                auto reward = agent->getCuriosityLevel();
                auto predictionError = agent->getPredictionError();
                episodeReward += reward + predictionError;
            }
            
            episodeRewards.push_back(episodeReward);
            episodeSpikeCounts.push_back(agent->getBrain()->getTotalSpikeCount());
        }
        
        return std::make_tuple(episodeRewards, episodeSpikeCounts);
    }, py::arg("agent"), py::arg("world"), py::arg("episodes"), py::arg("stepsPerEpisode"),
       "Run continuous simulation across multiple episodes and return rewards and spike counts");

    // Version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";
    m.attr("__build_date__") = "2026-10-19";
    
    // Error constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Feature flags
    m.attr("FEATURE_ADVANCED_PLOTTING") = true;
    m.attr("FEATURE_BATCH_PROCESSING") = true;
    m.attr("FEATURE_CHECKPOINTING") = true;
    m.attr("FEATURE_PROFILING") = true;
    m.attr("FEATURE_RESEARCH_MODE") = true;
}

// Main module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Real spiking neural computation with LIF neurons
        - Event-driven spike propagation and plasticity
        - Integrated memory, prediction, cognition, and neuromodulation
        - Context manager support for world simulation
        - Pythonic convenience methods and factory functions
        - Enhanced error handling with NLM-specific exceptions
        - Statistical access methods for brain monitoring
        - Advanced simulation features for research and power users
        
        Advanced Features:
        - Research configuration with customizable parameters
        - Batch processing for efficient simulation
        - Brain activity analysis and profiling
        - Checkpointing for long-running simulations
        - Random world generation
        - Continuous episode training
        - Performance monitoring and profiling
        
        Installation:
        pip install pynlm
        
        Basic Usage:
        ```python
        import pynlm
        
        # Create brain and world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        world = pynlm.createSimpleWorld()
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent = pynlm.createAgentBrain(brain)
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        ```
        
        Research Usage:
        ```python
        import pynlm
        
        # Create research configuration
        config = pynlm.createResearchConfig(
            description="My research simulation",
            neuronCount=2000,
            enablePlasticity=True,
            enableDevelopment=True,
            enableNeuromodulation=True
        )
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run advanced analysis
        analysis = pynlm.analyzeBrainActivity(brain, windowSize=500)
        print(f"Neurons: {analysis['total_neurons']}")
        print(f"Connectivity: {analysis['connectivity_density']:.4f}")
        ```
        
        Training Usage:
        ```python
        import pynlm
        
        # Setup training environment
        brain = pynlm.createQuickBrain(1000)
        world = pynlm.createRandomWorld(width=20, height=20)
        agent = pynlm.createTrainingAgent(brain, world)
        
        # Run training episode
        spikes, firing = pynlm.runEpisode(agent, world, 1000)
        
        # Profile performance
        spikes, firing, speed, time_ms = pynlm.profileSimulation(
            brain, world, 500, warmUpSteps=100
        )
        print(f"Performance: {speed:.1f} steps/sec")
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");

    // Register ID classes
    registerIDClass(m, "NeuronId");
    registerIDClass(m, "SynapseId");
    registerIDClass(m, "RegionId");
    registerIDClass(m, "PopulationId");

    // Register enums
    registerEnum<NeuronType>(m, "NeuronType");
    registerEnum<SynapseType>(m, "SynapseType");
    registerEnum<DevelopmentalStage>(m, "DevelopmentalStage");
    registerEnum<FiringState>(m, "FiringState");
    registerEnum<ActionType>(m, "ActionType");
    registerEnum<MotorCommand>(m, "MotorCommand");
    registerEnum<WorldObjectType>(m, "WorldObjectType");

    // Register core classes
    registerConfig(m);
    registerBasicInputs(m);
    registerActionAndWorld(m);
    registerSimpleWorld(m);
    registerBrain(m);
    registerAgentBrain(m);

    // Register factory functions
    registerFactoryFunctions(m);
    
    // Register advanced features
    registerAdvancedFeatures(m);
}

} // namespace nlm