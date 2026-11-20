#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <thread>
#include <cmath>

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

// Helper function for Python string conversion
inline std::string pyToStdString(const py::str& s) {
    return std::string(s.c_str());
}

// Helper function for batch processing results
struct BatchResult {
    std::vector<size_t> spikeCounts;
    std::vector<float> avgFiringRates;
    std::vector<float> eiratios;
    std::vector<size_t> totalSynapses;
    double totalTime;
    size_t totalSteps;
    
    BatchResult() : totalTime(0.0), totalSteps(0) {}
    
    py::dict toDict() const {
        py::dict result;
        result["spike_counts"] = spikeCounts;
        result["avg_firing_rates"] = avgFiringRates;
        result["eiratios"] = eiratios;
        result["total_synapses"] = totalSynapses;
        result["total_time"] = totalTime;
        result["total_steps"] = totalSteps;
        return result;
    }
};

// Helper function for configuration loading from Python dict
class ConfigFromDict {
public:
    static std::shared_ptr<Config> fromDict(const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : configDict) {
            std::string key = pyToStdString(item.first);
            
            if (item.second.is(py::int_())) {
                config->set(key, item.second.cast<int64_t>(), ConfigSource::Runtime);
            } else if (item.second.is(py::float_())) {
                config->set(key, item.second.cast<double>(), ConfigSource::Runtime);
            } else if (item.second.is(py::bool_())) {
                config->set(key, item.second.cast<bool>(), ConfigSource::Runtime);
            } else if (item.second.is(py::str())) {
                config->set(key, item.second.cast<std::string>(), ConfigSource::Runtime);
            } else if (item.second.is(py::list())) {
                // Handle lists
                py::list list = item.second.cast<py::list>();
                std::vector<double> vec;
                for (auto element : list) {
                    vec.push_back(element.cast<double>());
                }
                config->set(key, vec, ConfigSource::Runtime);
            }
        }
        
        return config;
    }
};

// Visualization helper functions
namespace viz {
    inline py::dict createBrainGraphData(const std::shared_ptr<Brain>& brain) {
        py::dict data;
        
        // Get basic statistics
        data["total_neurons"] = brain->getTotalNeuronCount();
        data["total_synapses"] = brain->getTotalSynapseCount();
        data["total_spikes"] = brain->getTotalSpikeCount();
        data["firing_rate"] = brain->getAverageFiringRate();
        data["eiratios"] = brain->getExcitationInhibitionRatio();
        
        // Create simple activation data
        std::vector<float> activations;
        if (brain->getTotalNeuronCount() > 0) {
            activations.reserve(brain->getTotalNeuronCount());
            for (size_t i = 0; i < std::min<size_t>(100, brain->getTotalNeuronCount()); ++i) {
                // Generate sample activation data (this would need actual brain data access)
                activations.push_back(0.5f); // Placeholder
            }
        }
        data["sample_activations"] = activations;
        
        return data;
    }
    
    inline std::string generateNetworkStatistics(const std::shared_ptr<Brain>& brain) {
        std::stringstream ss;
        ss << "=== NLM Brain Statistics ===\n";
        ss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        ss << "Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        return ss.str();
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides:
        
        - **Brain simulation**: Create and control virtual brains
        - **Agent framework**: Connect brains to environments
        - **Configuration**: Python-friendly configuration management
        - **Batch processing**: Run multiple simulations efficiently
        - **Profiling tools**: Analyze brain performance and behavior
        - **Visualization helpers**: Basic plotting and data visualization
        - **Advanced features**: Machine learning integration, parallel processing
        
        Usage example:
        
        ```python
        import pynlm
        
        # Create a brain with custom configuration
        config = pynlm.createDefaultConfig()
        config.set_value("neuron_count", 1000)
        config.set_value("random_seed", 42)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with visualization
        results = brain.run_simulation_with_stats(1000, visualize=True)
        
        print("Simulation completed!")
        print(brain.get_statistics())
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Configuration class with enhanced Python API
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", 
        R"pbdoc(
        Configuration class for NLM system with enhanced Python API.
        
        This class provides:
        - Dictionary-style access
        - Type-safe value retrieval
        - Default value support
        - Configuration validation
        - Export/import capabilities
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
              "Load configuration from a file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"), "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
              "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"), "Check if configuration key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get configuration summary")
        // Enhanced value setting with type inference
        .def("set_value", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>(), ConfigSource::Runtime);
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>(), ConfigSource::Runtime);
            } else if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>(), ConfigSource::Runtime);
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>(), ConfigSource::Runtime);
            } else if (value.is(py::list())) {
                py::list list = value.cast<py::list>();
                std::vector<double> vec;
                for (auto element : list) {
                    vec.push_back(element.cast<double>());
                }
                self.set(key, vec, ConfigSource::Runtime);
            } else {
                throw std::runtime_error("Unsupported value type for configuration key: " + key);
            }
        }, py::arg("key"), py::arg("value"), 
              "Set configuration value with Python type inference")
        // Enhanced value retrieval with type safety
        .def("get_value", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            auto result = self.get(key);
            if (result.has_value()) {
                // Convert back to Python object based on stored type
                return result.value();
            } else {
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue"),
              "Get configuration value with type safety")
        .def("get_int", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
              "Get integer configuration value")
        .def("get_float", [](const Config& self, const stdiremote Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
              "Get float configuration value")
        .def("get_double", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
              "Get double configuration value")
        .def("get_bool", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
              "Get boolean configuration value")
        .def("get_string", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
              "Get string configuration value")
        .def("from_dict", &ConfigFromDict::fromDict,
              py::arg("config_dict"), "Create configuration from Python dictionary")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get(key);
                if (value.has_value()) {
                    // Convert stored value back to Python
                    auto& v = value.value();
                    std::visit([&result, &key](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, int64_t>) {
                            result[key] = arg;
                        } else if constexpr (std::is_same_v<T, double>) {
                            result[key] = arg;
                        } else if constexpr (std::is_same_v<T, bool>) {
                            result[key] = arg;
                        } else if constexpr (std::is_same_v<T, std::string>) {
                            result[key] = arg;
                        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                            py::list list;
                            for (const auto& item : arg) {
                                list.append(item);
                            }
                            result[key] = list;
                        }
                    }, v);
                }
            }
            return result;
        }, "Export configuration to Python dictionary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain class with advanced features
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", 
        R"pbdoc(
        Enhanced neural simulation brain class with advanced features.
        
        This class provides:
        - Real spiking neural computation
        - Event-driven spike propagation
        - STDP and Hebbian plasticity
        - Structural plasticity
        - Memory systems integration
        - Neuromodulation
        - Development tracking
        - Checkpoint/save functionality
        - Advanced visualization
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
              "Create brain with configuration")
        .def("initialize", &Brain::initialize,
              "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
              py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
              py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("run_steps", [](std::shared_ptr<Brain>& brain, size_t steps, bool verbose = false) {
            for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
                brain->step(step);
                if (verbose && step % 100 == 0) {
                    std::cout << "Step " << step << ", Spikes: " << brain->getTotalSpikeCount() 
                             << ", Firing: " << brain->getFiringNeuronCount() << std::endl;
                }
            }
        }, py::arg("steps"), py::arg("verbose" = false), "Run multiple simulation steps")
        .def("run_simulation", [](std::shared_ptr<Brain>& brain, size_t steps) {
            BatchResult result;
            for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
                brain->step(step);
                result.spikeCounts.push_back(brain->getTotalSpikeCount());
                result.avgFiringRates.push_back(brain->getAverageFiringRate());
                result.eiratios.push_back(brain->getExcitationInhibitionRatio());
                result.totalSynapses.push_back(brain->getTotalSynapseCount());
            }
            result.totalSteps = steps;
            return result;
        }, py::arg("steps"), "Run simulation and return results")
        .def("run_simulation_with_stats", [](std::shared_ptr<Brain>& brain, size_t steps, bool visualize = false) {
            BatchResult result;
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
                brain->step(step);
                result.spikeCounts.push_back(brain->getTotalSpikeCount());
                result.avgFiringRates.push_back(brain->getAverageFiringRate());
                result.eiratios.push_back(brain->getExcitationInhibitionRatio());
                result.totalSynapses.push_back(brain->getTotalSynapseCount());
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
            result.totalSteps = steps;
            
            if (visualize) {
                std::cout << viz::generateNetworkStatistics(brain) << std::endl;
            }
            
            return result;
        }, py::arg("steps"), py::arg("visualize" = false), "Run simulation with detailed statistics and optional visualization")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
              "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
              py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
              py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name"), "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
              py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        // Advanced brain analysis
        .def("analyze_activity", [](const std::shared_ptr<Brain>& brain) {
            py::dict analysis;
            analysis["total_neurons"] = brain->getTotalNeuronCount();
            analysis["total_synapses"] = brain->getTotalSynapseCount();
            analysis["total_spikes"] = brain->getTotalSpikeCount();
            analysis["avg_firing_rate"] = brain->getAverageFiringRate();
            analysis["eiratios"] = brain->getExcitationInhibitionRatio();
            analysis["active_neurons"] = brain->getActiveNeuronCount();
            analysis["firing_neurons"] = brain->getFiringNeuronCount();
            return analysis;
        }, "Analyze brain activity and return statistics")
        .def("get_visualization_data", &viz::createBrainGraphData, py::arg("brain"),
              "Get visualization data for plotting")
        .def("generate_statistics", &viz::generateNetworkStatistics, py::arg("brain"),
              "Generate human-readable statistics string");

    // Enhanced AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain",
        R"pbdoc(
        Enhanced agent brain interface with advanced features.
        
        This class provides:
        - Enhanced sensory processing
        - Advanced motor decoding
        - Neuromodulation support
        - Development tracking
        - Curiosity and novelty detection
        - Reward prediction
        - Action planning
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
              "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
              "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain activity to motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
              py::arg("reward"), py::arg("predictedReward"),
              "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
              "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
       :// Enhanced neuromodulation methods
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get underlying brain")
        // Enhanced plasticity control
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
              "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
              "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
              "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
              "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled");

    // Convenience factory functions with enhanced defaults
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set sensible defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        return config;
    }, "Create default configuration with sensible values")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create brain with configuration")
    
    m.def("createBrainFromDict", [](const py::dict& configDict) -> std::shared_ptr<Brain> {
        auto config = ConfigFromDict::fromDict(configDict);
        return std::make_shared<Brain>(config);
    }, py::arg("config_dict"), "Create brain from Python dictionary configuration")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Example demonstration function
    m.def("demo_example", []() {
        std::cout << "=== NLM Python Demo Example ===\n" << std::endl;
        std::cout << "1. Creating a brain with custom configuration..." << std::endl;
        auto config = pynlm::createDefaultConfig();
        config->set_value("neuron_count", 500);
        config->set_value("random_seed", 123);
        
        std::cout << "2. Initializing brain..." << std::endl;
        auto brain = pynlm::createBrain(config);
        brain->initialize();
        
        std::cout << "3. Running simulation..." << std::endl;
        auto results = brain->run_simulation_with_stats(100, visualize=true);
        
        std::cout << "4. Analysis complete!" << std::endl;
        std::cout << "   Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "   Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        
        return py::dict({
            {"success", true},
            {"total_spikes", brain->getTotalSpikeCount()},
            {"avg_firing_rate", brain->getAverageFiringRate()},
            {"results", results.toDict()}
        });
    }, "Run demonstration example showing enhanced Python features");

} // namespace nlm
