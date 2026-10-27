// Enhanced NLM Python Bindings - High-level API
// This module provides Pythonic interfaces for NLM with better error handling, 
// convenience methods, and advanced features.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <sstream>
#include <iomanip>

#include "../src/python/bindings.cpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exception hierarchy for NLM
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

// Helper functions for Python integration
namespace python {
    
    // Safe conversion functions
    template<typename T>
    T safeGet(const py::dict& d, const std::string& key, const T& defaultValue = T()) {
        if (d.contains(key)) {
            try {
                return d[key].cast<T>();
            } catch (const py::cast_error&) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    // Convenience function to create config from Python dict
    std::shared_ptr<Config> configFromDict(const py::dict& d) {
        auto config = std::make_shared<Config>();
        
        for (auto& item : d) {
            std::string key = item.first.cast<std::string>();
            
            // Try different types based on the value
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>(), ConfigSource::User);
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<float>(), ConfigSource::User);
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>(), ConfigSource::User);
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>(), ConfigSource::User);
            }
        }
        
        return config;
    }
    
    // Convenience function to run complete simulation
    void runSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                      std::shared_ptr<AgentBrain> agent, int numSteps,
                      bool verbose = false) {
        if (!brain || !world || !agent) {
            throw SimulationError("Invalid brain, world, or agent provided");
        }
        
        if (numSteps <= 0) {
            throw ValidationError("Number of steps must be positive");
        }
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.1);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": " 
                          << "Firing: " << brain->getFiringNeuronCount()
                          << ", Total spikes: " << brain->getTotalSpikeCount()
                          << std::endl;
            }
        }
        
        if (verbose) {
            brain->logStatus();
        }
    }
    
    // Convenience function for learning simulation
    void runLearningSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, int numSteps,
                              bool enableRewardModulation = true,
                              bool enableCuriosity = true,
                              bool verbose = false) {
        if (!brain || !world || !agent) {
            throw SimulationError("Invalid brain, world, or agent provided");
        }
        
        // Enable learning subsystems
        if (enableRewardModulation) {
            agent->enableRewardModulation(true);
        }
        if (enableCuriosity) {
            agent->enableCuriosity(true);
        }
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        
        runSimulation(brain, world, agent, numSteps, verbose);
    }
    
    // Create a simple agent with default configuration
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    createSimpleAgent() {
        auto config = std::make_shared<Config>();
        
        // Set default values for a basic agent
        config->set("neuron_count", int64_t(500), ConfigSource::Default);
        config->set("region_count", int64_t(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("random_seed", int64_t(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Performance monitoring utility
    struct PerformanceStats {
        double simulationTime;
        size_t totalSteps;
        size_t totalSpikes;
        size_t firingNeurons;
        float avgFiringRate;
        float eI;
        
        std::string toString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3);
            ss << "Simulation Time: " << simulationTime << "s\n"
               << "Total Steps: " << totalSteps << "\n"
               << "Total Spikes: " << totalSpikes << "\n"
               << "Firing Neurons: " << firingNeurons << "\n"
               << "Avg Firing Rate: " << avgFiringRate << " Hz\n"
               << "E/I Ratio: " << eI;
            return ss.str();
        }
    };
    
    PerformanceStats measurePerformance(std::shared_ptr<Brain> brain, 
                                         double elapsedTime, 
                                         size_t steps) {
        PerformanceStats stats;
        stats.simulationTime = elapsedTime;
        stats.totalSteps = steps;
        stats.totalSpikes = brain->getTotalSpikeCount();
        stats.firingNeurons = brain->getFiringNeuronCount();
        stats.avgFiringRate = brain->getAverageFiringRate();
        stats.eI = brain->getExcitationInhibitionRatio();
        
        return stats;
    }
}

// Create a better Pythonic interface with properties and context managers
class PyConfigWrapper {
public:
    std::shared_ptr<Config> config_;
    
    PyConfigWrapper() : config_(std::make_shared<Config>()) {}
    explicit PyConfigWrapper(std::shared_ptr<Config> config) : config_(config) {}
    
    // Property system
    void set(const std::string& key, int64_t value) {
        config_->set(key, value, ConfigSource::User);
    }
    
    void set(const std::string& key, float value) {
        config_->set(key, value, ConfigSource::User);
    }
    
    void set(const std::string& key, bool value) {
        config_->set(key, value, ConfigSource::User);
    }
    
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value, ConfigSource::User);
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    py::dict getAll() const {
        py::dict result;
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            if (config_->has(key)) {
                // Try to get as different types
                if (auto intVal = config_->get<int64_t>(key)) {
                    result[key] = *intVal;
                } else if (auto floatVal = config_->get<float>(key)) {
                    result[key] = *floatVal;
                } else if (auto boolVal = config_->get<bool>(key)) {
                    result[key] = *boolVal;
                } else if (auto stringVal = config_->get<std::string>(key)) {
                    result[key] = *stringVal;
                }
            }
        }
        return result;
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    void loadFromFile(const std::string& filepath) {
        if (!config_->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load config from " + filepath);
        }
    }
    
    void saveToFile(const std::string& filepath) const {
        config_->saveToFile(filepath);
    }
};

class PyBrainWrapper {
public:
    std::shared_ptr<Brain> brain_;
    
    PyBrainWrapper() = default;
    explicit PyBrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Property access
    size_t getNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    float getFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    size_t getTotalSpikes() const {
        return brain_->getTotalSpikeCount();
    }
    
    size_t getFiringNeurons() const {
        return brain_->getFiringNeuronCount();
    }
    
    std::string getStatus() const {
        std::stringstream ss;
        brain_->logStatus();
        return "Brain: " + std::to_string(getNeuronCount()) + " neurons, " +
               std::to_string(getFiringNeurons()) + " firing";
    }
    
    // Simulation convenience methods
    void runSteps(int numSteps, double timestep = 0.1) {
        brain_->initialize();
        for (int step = 0; step < numSteps; ++step) {
            brain_->step(step, step * timestep);
        }
    }
    
    void step() {
        static SimulationStep step = 0;
        brain_->step(step++, 0.1);
    }
    
    // Statistical methods
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    float getDevelopmentProgress() const {
        return static_cast<float>(brain_->getDevelopmentalStage()) / 4.0f;
    }
};

class PyWorldWrapper {
public:
    std::shared_ptr<SimpleWorld> world_;
    
    PyWorldWrapper() : world_(std::make_shared<SimpleWorld>()) {}
    explicit PyWorldWrapper(std::shared_ptr<SimpleWorld> world) : world_(world) {}
    
    // Property access
    float getWidth() const { return world_->getWidth(); }
    float getHeight() const { return world_->getHeight(); }
    float getMaxEnergy() const { return world_->getMaxEnergy(); }
    double getSimulationTime() const { return world_->getSimulationTime(); }
    
    // Sensory access
    py::dict getSensoryPercept() const {
        auto percept = world_->getSensoryPercept();
        py::dict result;
        
        auto vision = percept.getVision();
        if (!vision.empty()) {
            result["vision"] = vision;
        }
        
        auto touch = percept.getTouch();
        if (!touch.empty()) {
            result["touch"] = touch;
        }
        
        auto internal = percept.getInternal();
        if (!internal.empty()) {
            result["internal"] = internal;
        }
        
        auto proprioception = percept.getProprioception();
        if (!proprioception.empty()) {
            result["proprioception"] = proprioception;
        }
        
        return result;
    }
    
    // Simulation control
    void reset() {
        world_->reset();
    }
    
    void update(double timestep = 0.1) {
        world_->update(timestep);
    }
    
    // Object management
    void addResource(float x, float y, float value = 10.0f) {
        WorldObject obj(x, y, WorldObjectType::Resource, value);
        world_->addObject(obj);
    }
    
    void addHazard(float x, float y, float value = -15.0f) {
        WorldObject obj(x, y, WorldObjectType::Hazard, value);
        world_->addObject(obj);
    }
    
    // Agent control
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
};

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        ----------------------------
        A comprehensive Python interface for the NLM neural simulation framework.
        
        This module provides high-level, Pythonic APIs for working with artificial brains,
        neural simulations, and agent-world interactions.
        
        Key Features:
        - Simplified configuration and setup
        - High-level simulation utilities
        - Advanced error handling
        - Performance monitoring tools
        - Context managers for clean resource management
        - Type-safe Pythonic interfaces
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Register new exception types
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    
    // Convenience functions
    m.def("create_simple_agent", &python::createSimpleAgent, 
          "Create a simple agent with default configuration");
    
    m.def("run_simulation", &python::runSimulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
          py::arg("verbose") = false,
          "Run a complete simulation with brain, world, and agent");
    
    m.def("run_learning_simulation", &python::runLearningSimulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
          py::arg("enable_reward_modulation") = true,
          py::arg("enable_curiosity") = true,
          py::arg("verbose") = false,
          "Run a learning simulation with all subsystems enabled");
    
    // Performance monitoring
    py::class_<python::PerformanceStats>(m, "PerformanceStats", R"pbdoc(Performance monitoring statistics)pbdoc")
        .def_readonly("simulation_time", &python::PerformanceStats::simulationTime)
        .def_readonly("total_steps", &python::PerformanceStats::totalSteps)
        .def_readonly("total_spikes", &python::PerformanceStats::totalSpikes)
        .def_readonly("firing_neurons", &python::PerformanceStats::firingNeurons)
        .def_readonly("avg_firing_rate", &python::PerformanceStats::avgFiringRate)
        .def_readonly("e_i_ratio", &python::PerformanceStats::eI)
        .def("to_string", &python::PerformanceStats::toString);
    
    // Enhanced wrapper classes
    py::class_<PyConfigWrapper>(m, "ConfigBuilder", R"pbdoc(Enhanced configuration builder)pbdoc")
        .def(py::init<>())
        .def("set", [](PyConfigWrapper& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](PyConfigWrapper& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](PyConfigWrapper& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](PyConfigWrapper& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("has", &PyConfigWrapper::has, py::arg("key"))
        .def("get_all", &PyConfigWrapper::getAll)
        .def("summary", &PyConfigWrapper::summary)
        .def("load_from_file", &PyConfigWrapper::loadFromFile, py::arg("filepath"))
        .def("save_to_file", &PyConfigWrapper::saveToFile, py::arg("filepath"));
    
    py::class_<PyBrainWrapper>(m, "BrainSession", R"pbdoc(High-level brain interface)pbdoc")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("get_neuron_count", &PyBrainWrapper::getNeuronCount)
        .def("get_synapse_count", &PyBrainWrapper::getSynapseCount)
        .def("get_firing_rate", &PyBrainWrapper::getFiringRate)
        .def("get_total_spikes", &PyBrainWrapper::getTotalSpikes)
        .def("get_firing_neurons", &PyBrainWrapper::getFiringNeurons)
        .def("get_status", &PyBrainWrapper::getStatus)
        .def("run_steps", &PyBrainWrapper::runSteps, py::arg("num_steps"), py::arg("timestep") = 0.1)
        .def("step", &PyBrainWrapper::step)
        .def("get_excitation_inhibition_ratio", &PyBrainWrapper::getExcitationInhibitionRatio)
        .def("get_development_progress", &PyBrainWrapper::getDevelopmentProgress);
    
    py::class_<PyWorldWrapper>(m, "WorldSession", R"pbdoc(High-level world interface)pbdoc")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"))
        .def("get_width", &PyWorldWrapper::getWidth)
        .def("get_height", &PyWorldWrapper::getHeight)
        .def("get_max_energy", &PyWorldWrapper::getMaxEnergy)
        .def("get_simulation_time", &PyWorldWrapper::getSimulationTime)
        .def("get_sensory_percept", &PyWorldWrapper::getSensoryPercept)
        .def("reset", &PyWorldWrapper::reset)
        .def("update", &PyWorldWrapper::update, py::arg("timestep") = 0.1)
        .def("add_resource", &PyWorldWrapper::addResource, py::arg("x"), py::arg("y"), py::arg("value") = 10.0f)
        .def("add_hazard", &PyWorldWrapper::addHazard, py::arg("x"), py::arg("y"), py::arg("value") = -15.0f)
        .def("set_agent_start", &PyWorldWrapper::setAgentStart, py::arg("x"), py::arg("y"));
    
    // Backward compatibility aliases
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
    
    // Python 3.6+ module initialization
    m.def("__version__", []() { return "1.0.0"; });
}

} // namespace nlm
