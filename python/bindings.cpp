#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <iomanip>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for helper classes
class PyVisionWrapper;
class PySensoryInputWrapper;

// Enhanced Python exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError(msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError(msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError(msg) {}
};

// Python Context Manager for Brain Simulation
class BrainSimulationContext {
private:
    std::shared_ptr<Brain> brain_;
    double timestep_;
    bool isInitialized_;
public:
    explicit BrainSimulationContext(std::shared_ptr<Brain> brain, double timestep = 0.1)
        : brain_(brain), timestep_(timestep), isInitialized_(false) {
        if (!brain_) {
            throw ValidationError("Brain instance cannot be null");
        }
    }

    ~BrainSimulationContext() {
        if (isInitialized_) {
            // Auto-save simulation state on destruction
            try {
                std::string filename = "nlm_simulation_checkpoint_" + 
                    std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".bin";
                brain_->save(filename);
            } catch (const std::exception&) {
                // Ignore save errors in destructor
            }
        }
    }

    BrainSimulationContext& initialize() {
        if (!brain_->initialize()) {
            throw SimulationError("Failed to initialize brain in context manager");
        }
        isInitialized_ = true;
        return *this;
    }

    BrainSimulationContext& runSteps(size_t numSteps) {
        if (!isInitialized_) {
            throw SimulationError("Brain must be initialized before running steps");
        }
        for (size_t i = 0; i < numSteps; ++i) {
            brain_->step(i, brain_->getConfig()->get<double>("simulation.time.step_size", 0.1));
        }
        return *this;
    }

    BrainSimulationContext& saveState(const std::string& filename) {
        if (!brain_->save(filename)) {
            throw SimulationError("Failed to save simulation state to: " + filename);
        }
        return *this;
    }

    std::shared_ptr<Brain> getBrain() const { return brain_; }
    bool isInitialized() const { return isInitialized_; }
};

// Python wrapper for enhanced Vision functionality
class PyVisionWrapper {
private:
    std::shared_ptr<Vision> vision_;
public:
    PyVisionWrapper() : vision_(std::make_shared<Vision>()) {}
    PyVisionWrapper(size_t width, size_t height, size_t channels = 3) 
        : vision_(std::make_shared<Vision>(width, height, channels)) {}

    // RGB convenience constructor
    PyVisionWrapper(size_t width, size_t height) 
        : vision_(std::make_shared<Vision>(width, height, 3)) {}

    void setRGBData(size_t x, size_t y, float r, float g, float b) {
        if (!vision_) throw ValidationError("Vision object not initialized");
        size_t idx = ((y * vision_->getWidth()) + x) * 3;
        auto& data = vision_->getData();
        if (idx + 2 >= data.size()) {
            throw ValidationError("RGB data index out of bounds");
        }
        data[idx] = r;
        data[idx + 1] = g;
        data[idx + 2] = b;
    }

    void setGrayscaleData(size_t x, size_t y, float intensity) {
        if (!vision_) throw ValidationError("Vision object not initialized");
        size_t idx = y * vision_->getWidth() + x;
        auto& data = vision_->getData();
        if (idx >= data.size()) {
            throw ValidationError("Grayscale data index out of bounds");
        }
        data[idx] = intensity;
    }

    float getRed(size_t x, size_t y) const {
        if (!vision_) throw ValidationError("Vision object not initialized");
        size_t idx = ((y * vision_->getWidth()) + x) * 3;
        auto& data = vision_->getData();
        if (idx + 2 >= data.size()) {
            throw ValidationError("Red channel access out of bounds");
        }
        return data[idx];
    }

    float getGreen(size_t x, size_t y) const {
        if (!vision_) throw ValidationError("Vision object not initialized");
        size_t idx = ((y * vision_->getWidth()) + x) * 3;
        auto& data = vision_->getData();
        if (idx + 1 >= data.size()) {
            throw ValidationError("Green channel access out of bounds");
        }
        return data[idx + 1];
    }

    float getBlue(size_t x, size_t y) const {
        if (!vision_) throw ValidationError("Vision object not initialized");
        size_t idx = ((y * vision_->getWidth()) + x) * 3;
        auto& data = vision_->getData();
        if (idx >= data.size()) {
            throw ValidationError("Blue channel access out of bounds");
        }
        return data[idx + 2];
    }

    float getIntensity(size_t x, size_t y) const {
        if (!vision_) throw ValidationError("Vision object not initialized");
        size_t idx = y * vision_->getWidth() + x;
        auto& data = vision_->getData();
        if (idx >= data.size()) {
            throw ValidationError("Intensity access out of bounds");
        }
        return data[idx];
    }

    std::shared_ptr<Vision> getVision() const { return vision_; }

    // Iterator support for data access
    class iterator {
    private:
        std::shared_ptr<Vision> vision_;
        size_t index_;
        bool is_rgba_;
    public:
        iterator(std::shared_ptr<Vision> vision, size_t index, bool is_rgba)
            : vision_(vision), index_(index), is_rgba_(is_rgba) {}

        float& operator*() {
            auto& data = vision_->getData();
            return data[index_];
        }

        iterator& operator++() {
            if (is_rgba_) {
                index_ += 3;  // Skip RGB channels
            } else {
                index_++;
            }
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return index_ != other.index_;
        }
    };

    iterator begin(bool grayscale = true) {
        return iterator(vision_, grayscale ? 0 : 0, !grayscale);
    }

    iterator end(bool grayscale = true) {
        size_t dataSize = vision_->getData().size();
        if (grayscale) {
            return iterator(vision_, dataSize, false);
        } else {
            // For RGBA data, end at last channel
            size_t rgbaSize = dataSize % 3 == 0 ? dataSize : dataSize - (dataSize % 3);
            return iterator(vision_, rgbaSize, !grayscale);
        }
    }
};

// Enhanced Configuration Builder
class ConfigurationBuilder {
private:
    std::shared_ptr<Config> config_;
public:
    ConfigurationBuilder() : config_(std::make_shared<Config>()) {}

    ConfigurationBuilder& addBrainParameters(size_t neuronCount = 1000, 
                                            float synapseDensity = 0.1,
                                            float vThresh = -50.0f,
                                            float vRest = -70.0f) {
        config_->set("brain.neuron_count", neuronCount);
        config_->set("brain.synapse_density", synapseDensity);
        config_->set("brain.v_thresh", vThresh);
        config_->set("brain.v_rest", vRest);
        return *this;
    }

    ConfigurationBuilder& addPlasticityParameters(bool enableSTDP = true,
                                                  float stdpLearningRate = 0.001,
                                                  bool enableHebbian = true,
                                                  bool enableStructural = true) {
        config_->set("plasticity.stdp.enable", enableSTDP);
        config_->set("plasticity.stdp.learning_rate", stdpLearningRate);
        config_->set("plasticity.hebbian.enable", enableHebbian);
        config_->set("plasticity.structural.enable", enableStructural);
        return *this;
    }

    ConfigurationBuilder& addNeuromodulationParameters(bool enableCuriosity = true,
                                                         bool enableNovelty = true,
                                                         float dopamineScale = 1.0f) {
        config_->set("neuromod.curiosity.enable", enableCuriosity);
        config_->set("neuromod.novelty.enable", enableNovelty);
        config_->set("neuromod.dopamine.scale", dopamineScale);
        return *this;
    }

    ConfigurationBuilder& addDevelopmentParameters(bool enableDevelopment = true,
                                                   float criticalPeriodStart = 10.0f,
                                                   float criticalPeriodEnd = 100.0f) {
        config_->set("development.enabled", enableDevelopment);
        config_->set("development.critical_period_start", criticalPeriodStart);
        config_->set("development.critical_period_end", criticalPeriodEnd);
        return *this;
    }

    ConfigurationBuilder& addSimulationParameters(double timestep = 0.1,
                                                  size_t maxSteps = 1000) {
        config_->set("simulation.time.step_size", timestep);
        config_->set("simulation.max_steps", maxSteps);
        return *this;
    }

    ConfigurationBuilder& setRandomSeed(uint64_t seed) {
        config_->set("random_seed", seed);
        return *this;
    }

    std::shared_ptr<Config> build() {
        return config_;
    }

    // Convenience methods for common configurations
    ConfigurationBuilder& createMinimalBrain() {
        return addBrainParameters(100, 0.05)
               .addPlasticityParameters(true, 0.001, false, false)
               .addNeuromodulationParameters(false, false)
               .addDevelopmentParameters(false);
    }

    ConfigurationBuilder& createAdvancedBrain() {
        return addBrainParameters(1000, 0.1)
               .addPlasticityParameters(true, 0.001, true, true)
               .addNeuromodulationParameters(true, true, 1.0f)
               .addDevelopmentParameters(true, 10.0f, 100.0f);
    }

    ConfigurationBuilder& createExplorationBrain() {
        return addBrainParameters(500, 0.05)
               .addPlasticityParameters(true, 0.0005, true, true)
               .addNeuromodulationParameters(true, true, 0.5f)
               .addDevelopmentParameters(true, 5.0f, 50.0f);
    }
};

// Enhanced Visualization Helper
class VisualizationHelper {
public:
    static std::string formatBrainStats(const std::shared_ptr<Brain>& brain) {
        if (!brain) return "Invalid brain pointer";
        
        std::stringstream ss;
        ss << "=== NLM Brain Statistics ===\n";
        ss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        ss << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        
        auto config = brain->getConfig();
        if (config->has("brain.neuron_count")) {
            ss << "Config Neuron Count: " << config->get<size_t>("brain.neuron_count") << "\n";
        }
        
        return ss.str();
    }

    static std::string generateHexColor(float value, float min = 0.0f, float max = 1.0f) {
        float normalized = (value - min) / (max - min);
        normalized = std::max(0.0f, std::min(1.0f, normalized));
        
        int r = static_cast<int>(255 * (1 - normalized));
        int g = static_cast<int>(255 * normalized);
        int b = 128;
        
        std::stringstream ss;
        ss << "#" << std::hex << std::uppercase << std::setfill('0')
           << std::setw(2) << r << std::setw(2) << g << std::setw(2) << b;
        return ss.str();
    }

    static std::vector<std::string> generateProgressReport(const std::shared_ptr<Brain>& brain,
                                                           size_t step,
                                                           double time) {
        std::vector<std::string> report;
        report.push_back("=== Simulation Progress ===");
        report.push_back("Step: " + std::to_string(step));
        report.push_back("Time: " + std::to_string(time) + " s");
        report.push_back("Neurons Firing: " + std::to_string(brain->getFiringNeuronCount()));
        report.push_back("Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        report.push_back("Development Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
        
        auto config = brain->getConfig();
        if (config->has("simulation.max_steps")) {
            size_t maxSteps = config->get<size_t>("simulation.max_steps");
            float progress = static_cast<float>(step) / maxSteps * 100.0f;
            report.push_back("Progress: " + std::to_string(progress) + "%");
        }
        
        return report;
    }
};

// Enhanced Learning Algorithm Demonstrator
class LearningAlgorithmDemo {
public:
    static void demonstrateSTDP(const std::shared_ptr<Brain>& brain, size_t steps = 100) {
        if (!brain) throw ValidationError("Brain pointer is null");
        
        // Configure for STDP demonstration
        auto config = brain->getConfig();
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.hebbian.enable", false);
        config->set("plasticity.structural.enable", false);
        
        brain->initialize();
        
        // Record initial state
        size_t initialSpikes = brain->getTotalSpikeCount();
        float initialFiringRate = brain->getAverageFiringRate();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, 0.1);
            
            // Inject periodic bursts to stimulate learning
            if (i % 20 == 0) {
                for (size_t j = 0; j < brain->getTotalNeuronCount() / 10; ++j) {
                    brain->injectCurrentToNeurons(NeuronType::Excitatory, 10.0f);
                }
            }
        }
        
        // Demonstrate learning results
        size_t finalSpikes = brain->getTotalSpikeCount();
        float finalFiringRate = brain->getAverageFiringRate();
        
        std::cout << "=== STDP Learning Demonstration ===\n";
        std::cout << "Initial spikes: " << initialSpikes << "\n";
        std::cout << "Final spikes: " << finalSpikes << "\n";
        std::cout << "Spike increase: " << (finalSpikes - initialSpikes) << "\n";
        std::cout << "Initial firing rate: " << initialFiringRate << " Hz\n";
        std::cout << "Final firing rate: " << finalFiringRate << " Hz\n";
    }

    static void demonstrateHebbianLearning(const std::shared_ptr<Brain>& brain, size_t steps = 100) {
        if (!brain) throw ValidationError("Brain pointer is null");
        
        // Configure for Hebbian demonstration
        auto config = brain->getConfig();
        config->set("plasticity.stdp.enable", false);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", false);
        
        brain->initialize();
        
        std::cout << "=== Hebbian Learning Demonstration ===\n";
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, 0.1);
            
            // Synchronize two neurons to promote Hebbian strengthening
            if (i % 10 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 5.0f);
            }
        }
        
        std::cout << "Hebbian learning completed with " << brain->getTotalSpikeCount() << " total spikes\n";
    }

    static void demonstrateCuriosityDrivenLearning(const std::shared_ptr<Brain>& brain,
                                                   const std::shared_ptr<SimpleWorld>& world,
                                                   size_t steps = 100) {
        if (!brain) throw ValidationError("Brain pointer is null");
        if (!world) throw ValidationError("World pointer is null");
        
        // Configure for curiosity-driven learning
        auto config = brain->getConfig();
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        std::cout << "=== Curiosity-Driven Learning Demonstration ===\n";
        
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, 0.1);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f;
            auto internal = percept.getInternal();
            if (!internal.empty()) {
                reward = internal[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
            
            if (i % 20 == 0) {
                std::cout << "Step " << i << ": Curiosity=" << agent->getCuriosityLevel()
                         << ", Novelty=" << agent->getNoveltyLevel()
                         << ", Reward=" << reward << "\n";
            }
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        Advanced Python bindings for the NLM neural simulation framework.
        
        Features:
        - Enhanced error handling with custom Python exceptions
        - Context managers for resource management
        - Iterator support for vision data
        - Configuration builder for easy setup
        - Visualization helpers for statistics
        - Learning algorithm demonstrations
        - Type safety and validation
        - Backward compatibility maintained
    )pbdoc";

    // Register enhanced exception types
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);

    // NeuronType and SynapseType (unchanged from original)
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

    // Enhanced NeuronId with additional Python features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron with Python enhancements)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration system for NLM with Pythonic API)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::get), py::arg("key"), py::arg("required") = false,
             "Get configuration value with optional validation")
        .def("get", py::overload_cast<const std::string&, const T&>(&Config::get), py::arg("key"), py::arg("default_value"),
             "Get configuration value with default fallback")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](const Config& self) {
            // Basic validation - check for required keys
            auto keys = self.getKeys();
            std::vector<std::string> missing;
            for (const auto& key : {"brain.neuron_count", "brain.v_thresh", "brain.v_rest"}) {
                if (!self.has(key)) {
                    missing.push_back(key);
                }
            }
            if (!missing.empty()) {
                throw ConfigurationError("Missing required configuration keys: " + 
                    []() {
                        std::stringstream ss;
                        for (size_t i = 0; i < missing.size(); ++i) {
                            if (i > 0) ss << ", ";
                            ss << missing[i];
                        }
                        return ss.str();
                    }());
            }
        }, "Validate configuration and throw ConfigurationError if invalid")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__getitem__", [](Config& self, const std::string& key) {
            // Allow Python-style config['key'] access
            if (!self.has(key)) {
                throw ConfigurationError("Configuration key not found: " + key);
            }
            return self.get<std::string>(key);  // This will be specialized for different types
        })
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        });

    // Enhanced Brain class with context manager support
    py::class_<BrainSimulationContext>(m, "BrainSimulationContext", 
        R"pbdoc(Context manager for brain simulation with automatic resource management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, double>(), py::arg("brain"), py::arg("timestep") = 0.1)
        .def("initialize", &BrainSimulationContext::initialize,
             "Initialize the brain in this context")
        .def("runSteps", &BrainSimulationContext::runSteps, py::arg("numSteps"),
             "Run multiple simulation steps")
        .def("saveState", &BrainSimulationContext::saveState, py::arg("filename"),
             "Save simulation state to file")
        .def("getBrain", &BrainSimulationContext::getBrain,
             "Get the brain instance")
        .def("isInitialized", &BrainSimulationContext::isInitialized,
             "Check if the context is initialized");

    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced neural simulation brain with advanced Python interface)pbdoc")
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
        .def("__enter__", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<BrainSimulationContext>(brain);
        })
        .def("__exit__", [](std::shared_ptr<Brain> brain, const py::args&) {
            // Cleanup handled by context manager
        });

    // Enhanced AgentBrain with better type safety
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with type safety)pbdoc")
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
        // Subsystem control
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        // Subsystem status checks
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Enhanced World with Python features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple world with Pythonic API)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Advanced factory functions and utilities
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

    // Advanced utility functions
    m.def("createConfigurationBuilder", []() {
        return std::make_shared<ConfigurationBuilder>();
    }, "Create a configuration builder for easy setup");

    m.def("visualizeBrainStats", &VisualizationHelper::formatBrainStats,
         py::arg("brain"),
         "Generate formatted brain statistics string");

    m.def("generateProgressReport", &VisualizationHelper::generateProgressReport,
         py::arg("brain"), py::arg("step"), py::arg("time"),
         "Generate simulation progress report");

    m.def("demonstrateSTDP", &LearningAlgorithmDemo::demonstrateSTDP,
         py::arg("brain"), py::arg("steps") = 100,
         "Demonstrate STDP learning algorithm");

    m.def("demonstrateHebbianLearning", &LearningAlgorithmDemo::demonstrateHebbianLearning,
         py::arg("brain"), py::arg("steps") = 100,
         "Demonstrate Hebbian learning algorithm");

    m.def("demonstrateCuriosityDrivenLearning", &LearningAlgorithmDemo::demonstrateCuriosityDrivenLearning,
         py::arg("brain"), py::arg("world"), py::arg("steps") = 100,
         "Demonstrate curiosity-driven learning algorithm");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error types for Python users
    py::class_<NLMError>(m, "NLMError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<ConfigurationError>(m, "ConfigurationError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<SimulationError>(m, "SimulationError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<ValidationError>(m, "ValidationError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"));

    // Version and metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM with advanced features";
}

} // namespace nlm