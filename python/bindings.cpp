#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <chrono>

// Custom exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class InitializationError : public NLMError {
public:
    explicit InitializationError(const std::string& msg) : NLMError("Initialization failed: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class ResourceError : public NLMError {
public:
    explicit ResourceError(const std::string& msg) : NLMError("Resource error: " + msg) {}
};

namespace py = pybind11;
namespace nlm {

// Context manager for automatic brain cleanup
class BrainContext {
public:
    BrainContext(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~BrainContext() {
        if (brain_) {
            brain_->reset();
        }
    }
    std::shared_ptr<Brain> get() const { return brain_; }
    void reset() {
        if (brain_) {
            brain_->reset();
            brain_.reset();
        }
    }
private:
    std::shared_ptr<Brain> brain_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        An enhanced Python interface for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action,
        plus convenience methods, context managers, and helper functions.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<ResourceError>(m, "ResourceError");

    // Helper functions for validation and conversions
    m.def("validate_brain", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw ResourceError("Brain pointer is null");
        }
        if (!brain->getConfig()) {
            throw ConfigurationError("Brain has no configuration");
        }
        return true;
    }, "Validate a brain object", py::arg("brain"));

    m.def("create_simulation_config", [](int neuron_count = 1000, float connection_probability = 0.1,
                                         const std::string& mode = "default") {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.connection_probability", connection_probability);
        config->set("simulation.mode", mode);
        return config;
    }, py::arg("neuron_count") = 1000,
       py::arg("connection_probability") = 0.1,
       py::arg("mode") = "default",
       "Create a pre-configured simulation configuration");

    // Enhanced Config class with convenience methods
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Improved configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        .def("getInt", [](const Config& self, const std::string& key, int default_value) {
            return self.has(key) ? self.getInt(key) : default_value;
        }, py::arg("key"), py::arg("default_value"),
            "Get integer value, returns default if key doesn't exist")
        .def("getFloat", [](const Config& self, const std::string& key, float default_value) {
            return self.has(key) ? self.getFloat(key) : default_value;
        }, py::arg("key"), py::arg("default_value"),
            "Get float value, returns default if key doesn't exist")
        .def("getString", [](const Config& self, const std::string& key, const std::string& default_value) {
            return self.has(key) ? self.getString(key) : default_value;
        }, py::arg("key"), py::arg("default_value"),
            "Get string value, returns default if key doesn't exist")
        .def("setInt", &Config::setInt, py::arg("key"), py::arg("value"),
             "Set integer value")
        .def("setFloat", &Config::setFloat, py::arg("key"), py::arg("value"),
             "Set float value")
        .def("setString", &Config::setString, py::arg("key"), py::arg("value"),
             "Set string value")
        .def("setBool", &Config::setBool, py::arg("key"), py::arg("value"),
             "Set boolean value")
        .def("getIntDefault", &Config::getInt, py::arg("key"), py::arg("default_value"),
             "Get integer value with default (alias for getInt)")
        .def("getFloatDefault", &Config::getFloat, py::arg("key"), py::arg("default_value"),
             "Get float value with default (alias for getFloat)")
        .def("getStringDefault", &Config::getString, py::arg("key"), py::arg("default_value"),
             "Get string value with default (alias for getString)")
        .def("getBool", &Config::getBool, py::arg("key"), py::arg("default_value") = false,
             "Get boolean value, returns default if key doesn't exist")
        .def("validate", [](const Config& self) {
            bool valid = true;
            if (self.has("brain.neuron_count")) {
                int count = self.getInt("brain.neuron_count");
                if (count <= 0 || count > 1000000) {
                    valid = false;
                }
            }
            if (self.has("brain.connection_probability")) {
                float prob = self.getFloat("brain.connection_probability");
                if (prob < 0.0f || prob > 1.0f) {
                    valid = false;
                }
            }
            return valid;
        }, "Validate configuration parameters")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain class with context manager and convenience methods
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for automatic brain cleanup)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("get", &BrainContext::get)
        .def("reset", &BrainContext::reset);

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("stepWithTime", [](Brain& self, float time) {
            return self.step(static_cast<SimulationStep>(0), static_cast<Timestamp>(time * 1000.0f));
        }, py::arg("time"), "Perform a simulation step with time in seconds")
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
        .def("isInitialized", [](const Brain& self) {
            return self.getConfig() != nullptr;
        }, "Check if brain is initialized")
        .def("getStats", [](const Brain& self) {
            return py::make_dict(
                "neuron_count", self.getTotalNeuronCount(),
                "synapse_count", self.getTotalSynapseCount(),
                "active_neurons", self.getActiveNeuronCount(),
                "firing_neurons", self.getFiringNeuronCount(),
                "firing_rate", self.getAverageFiringRate(),
                "spike_count", self.getTotalSpikeCount(),
                "e_i_ratio", self.getExcitationInhibitionRatio(),
                "development_stage", static_cast<int>(self.getDevelopmentalStage())
            );
        }, "Get brain statistics as a dictionary")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getFiringNeuronCount()) + 
                   " firing>";
        });

    // Simulation utilities and helper functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, int num_steps, float timestep = 0.1f,
                               bool log_stats = false) {
        if (!brain) {
            throw ResourceError("Brain pointer is null");
        }
        brain->initialize();
        std::vector<float> firing_rates;
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step, step * timestep * 1000.0f);
            if (log_stats && step % 100 == 0) {
                firing_rates.push_back(brain->getAverageFiringRate());
            }
        }
        return firing_rates;
    }, py::arg("brain"), py::arg("num_steps"), py::arg("timestep") = 0.1f,
       py::arg("log_stats") = false,
       "Run a simulation and return firing rates");

    m.def("create_brain_context", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<BrainContext>(brain);
    }, py::arg("brain"),
         "Create a context manager for automatic brain cleanup");

    // Enhanced AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
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
        .def("getAllStats", [](const AgentBrain& self) {
            return py::make_dict(
                "curiosity_level", self.getCuriosityLevel(),
                "novelty_level", self.getNoveltyLevel(),
                "prediction_error", self.getPredictionError(),
                "neuromodulation_level", self.getNeuromodulationLevel(),
                "development_stage", static_cast<int>(self.getDevelopmentalStage()),
                "reward_modulation_enabled", self.isRewardModulationEnabled(),
                "structural_plasticity_enabled", self.isStructuralPlasticityEnabled(),
                "development_enabled", self.isDevelopmentEnabled(),
                "curiosity_enabled", self.isCuriosityEnabled()
            );
        }, "Get all agent brain statistics");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("updateWithTime", [](SimpleWorld& self, float time) {
            return self.update(time);
        }, py::arg("time"), "Update world with time in seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("getAllStats", [](const SimpleWorld& self) {
            return py::make_dict(
                "width", self.getWidth(),
                "height", self.getHeight(),
                "simulation_time", self.getSimulationTime(),
                "max_energy", self.getMaxEnergy(),
                "energy_decay_rate", self.getEnergyDecayRate(),
                "random_seed", self.getRandomSeed()
            );
        }, "Get world statistics");

    // Create convenience functions for common use cases
    m.def("create_agent_simulation", [](int steps = 1000, bool enable_learning = true,
                                        bool enable_development = true) {
        auto config = pynlm::createDefaultConfig();
        config->set("brain.neuron_count", 500);
        config->set("brain.connection_probability", 0.05);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        if (enable_development) {
            agent->enableDevelopment(true);
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("steps") = 1000,
       py::arg("enable_learning") = true,
       py::arg("enable_development") = true,
       "Create a complete agent simulation setup");

    m.def("run_agent_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent, int steps) {
        if (!brain || !world || !agent) {
            throw ResourceError("One or more components is null");
        }
        
        agent->initialize(*world);
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * 0.1f * 1000.0f);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (i % 100 == 0) {
                auto stats = agent->getAllStats();
                std::cout << "Step " << i << ": "
                         << "Neurons firing: " << brain->getFiringNeuronCount()
                         << ", Curiosity: " << stats["curiosity_level"].cast<float>()
                         << ", Novelty: " << stats["novelty_level"].cast<float>() << std::endl;
            }
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a complete agent episode");

    // Original factory functions (maintained for backward compatibility)
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

    // Version information
    m.attr("__version__") = "0.1.0";  // Should match pyproject.toml

    // Registration for Python attributes
    m.attr("__all__") = py::list()
        .attr("append")("Brain")
        .attr("append")("Config")
        .attr("append")("AgentBrain")
        .attr("append")("SimpleWorld")
        .attr("append")("Vision")
        .attr("append")("Audio")
        .attr("append")("InternalSignals")
        .attr("append")("Action")
        .attr("append")("WorldObject")
        .attr("append")("AgentBody")
        .attr("append")("ActionResult")
        .attr("append")("SensoryPercept")
        .attr("append")("createDefaultConfig")
        .attr("append")("createBrain")
        .attr("append")("createSimpleWorld")
        .attr("append")("createAgentBrain")
        .attr("append")("INVALID_NEURON_ID")
        .attr("append")("INVALID_SYNAPSE_ID")
        .attr("append")("INVALID_REGION_ID")
        .attr("append")("INVALID_POPULATION_ID");

    // Register exceptions for Python
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<ResourceError>(m, "ResourceError");
}

} // namespace nlm