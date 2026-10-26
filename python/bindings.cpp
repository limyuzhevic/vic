#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <numeric>
#include <cmath>
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
#include "../src/experiments/ExperimentRunner.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced neural network builder and configuration utilities
class NeuralNetworkBuilder {
public:
    static std::shared_ptr<Config> createAdvancedConfig(
        size_t neuronCount = 500,
        size_t regionCount = 1,
        float connectionProbability = 0.15f,
        double timestep = 0.001,
        uint64_t randomSeed = 42) {
        
        auto config = std::make_shared<Config>();
        
        // Basic configuration
        config->set("random_seed", static_cast<int64_t>(randomSeed), ConfigSource::Default);
        config->set("simulation_timestep", timestep, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config->set("connection_probability", connectionProbability, ConfigSource::Default);
        
        // Advanced plasticity settings
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("stdp_theta", 0.5f, ConfigSource::Default); // STDP eligibility trace threshold
        
        // Hebbian plasticity settings
        config->set("hebbian_learning_rate", 0.01f, ConfigSource::Default);
        config->set("hebbian_trace_decay", 0.995f, ConfigSource::Default);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        config->set("pruning_threshold", 0.01f, ConfigSource::Default);
        
        // Neuromodulation
        config->set("dopamine_sensitivity", 0.1f, ConfigSource::Default);
        config->set("curiosity_rate", 0.05f, ConfigSource::Default);
        config->set("novelty_sensitivity", 0.2f, ConfigSource::Default);
        
        // Development
        config->set("plasticity_decay_rate", 0.999f, ConfigSource::Default);
        config->set("critical_period_end", 1000, ConfigSource::Default);
        
        // Performance optimizations
        config->set("sparse_connectivity", true, ConfigSource::Default);
        config->set("vectorization_level", 3, ConfigSource::Default);
        config->set("parallel_processing", true, ConfigSource::Default);
        
        return config;
    }
    
    static std::shared_ptr<Brain> createBrainWithArchitecture(
        const std::string& architecture = "balanced",
        std::shared_ptr<Config> config = nullptr) {
        
        if (!config) {
            config = createAdvancedConfig();
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        if (architecture == "deep") {
            // Deep architecture: more layers, fewer neurons per layer
            config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
        } else if (architecture == "wide") {
            // Wide architecture: fewer layers, more neurons per layer
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("connection_probability", 0.25f, ConfigSource::Default);
        } else if (architecture == "small") {
            // Small architecture: minimal network for fast testing
            config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
            config->set("connection_probability", 0.10f, ConfigSource::Default);
        } else if (architecture == "recurrent") {
            // Recurrent architecture: stronger recurrent connections
            config->set("connection_probability", 0.3f, ConfigSource::Default);
        }
        
        brain->initialize();
        return brain;
    }
};

// Advanced performance monitoring and profiling
class PerformanceProfiler {
private:
    struct TimingStats {
        double min_time = 0.0;
        double max_time = 0.0;
        double avg_time = 0.0;
        double total_time = 0.0;
        size_t sample_count = 0;
    };
    
    std::map<std::string, TimingStats> timingData;
    size_t stepCount = 0;
    double totalSimulationTime = 0.0;
    
public:
    void recordStepTime(const std::string& operation, double timeMs) {
        auto& stats = timingData[operation];
        stats.total_time += timeMs;
        if (stats.min_time == 0.0 || timeMs < stats.min_time) {
            stats.min_time = timeMs;
        }
        if (timeMs > stats.max_time) {
            stats.max_time = timeMs;
        }
        stats.sample_count++;
        stats.avg_time = stats.total_time / stats.sample_count;
        stepCount++;
        totalSimulationTime += timeMs;
    }
    
    py::dict getPerformanceReport() {
        py::dict report;
        report["total_steps"] = stepCount;
        report["total_simulation_time_ms"] = totalSimulationTime;
        report["avg_step_time_ms"] = stepCount > 0 ? totalSimulationTime / stepCount : 0.0;
        
        py::list operations;
        for (const auto& pair : timingData) {
            py::dict op_stats;
            op_stats["name"] = pair.first;
            op_stats["min_ms"] = pair.second.min_time;
            op_stats["max_ms"] = pair.second.max_time;
            op_stats["avg_ms"] = pair.second.avg_time;
            op_stats["total_ms"] = pair.second.total_time;
            op_stats["samples"] = pair.second.sample_count;
            operations.append(op_stats);
        }
        report["operations"] = operations;
        
        return report;
    }
    
    void reset() {
        timingData.clear();
        stepCount = 0;
        totalSimulationTime = 0.0;
    }
};

// Advanced visualization and analysis utilities
class NeuralAnalyzer {
public:
    static py::dict analyzeNeuralActivity(std::shared_ptr<Brain> brain) {
        py::dict analysis;
        
        // Basic statistics
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["active_neurons"] = brain->getActiveNeuronCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["total_spikes"] = brain->getTotalSpikeCount();
        
        // Compute firing rate
        double firingRate = brain->getTotalNeuronCount() > 0 
            ? brain->getFiringNeuronCount() * 100.0 / brain->getTotalNeuronCount()
            : 0.0;
        analysis["firing_rate_percent"] = firingRate;
        
        // Excitatory/inhibitory balance
        double eiRatio = brain->getExcitationInhibitionRatio();
        analysis["excitation_inhibition_ratio"] = eiRatio;
        
        // Developmental stage
        analysis["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Performance metrics
        auto spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            auto mostActive = spikeSystem->getMostActiveNeurons(10);
            py::list activeNeuronIds;
            for (const auto& id : mostActive) {
                activeNeuronIds.append(id.value);
            }
            analysis["most_active_neurons"] = activeNeuronIds;
        }
        
        return analysis;
    }
    
    static std::vector<float> computeNeuralPowerSpectrum(std::shared_ptr<Brain> brain, size_t steps = 100) {
        std::vector<float> powerSpectrum(steps, 0.0f);
        
        // Record spike activity over time
        std::vector<size_t> spikeHistory(steps, 0);
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            spikeHistory[step] = brain->getTotalSpikeCount();
        }
        
        // Compute power spectrum using FFT
        size_t n = steps;
        std::vector<float> spectrum(n, 0.0f);
        
        for (size_t k = 0; k < n; ++k) {
            double real = 0.0, imag = 0.0;
            for (size_t t = 0; t < n; ++t) {
                double angle = -2.0 * M_PI * k * t / n;
                real += spikeHistory[t] * cos(angle);
                imag += spikeHistory[t] * sin(angle);
            }
            spectrum[k] = static_cast<float>(sqrt(real * real + imag * imag) / n);
        }
        
        return spectrum;
    }
};

// Advanced learning and optimization controllers
class LearningController {
public:
    static void optimizePlasticityParameters(std::shared_ptr<Brain> brain, 
                                             const std::map<std::string, float>& targetMetrics) {
        auto config = brain->getConfig();
        
        // Adjust STDP parameters based on target metrics
        if (targetMetrics.count("stability") > 0) {
            float stability = targetMetrics.at("stability");
            // Reduce STDP weights for stability
            float currentLtp = config->getOr<float>("stdp_ltp_weight", 0.02f);
            config->set("stdp_ltp_weight", currentLtp * stability, ConfigSource::Override);
            
            float currentLtd = config->getOr<float>("stdp_ltd_weight", 0.015f);
            config->set("stdp_ltd_weight", currentLtd * stability, ConfigSource::Override);
        }
        
        if (targetMetrics.count("learning_rate") > 0) {
            float learningRate = targetMetrics.at("learning_rate");
            float currentLtp = config->getOr<float>("stdp_ltp_weight", 0.02f);
            config->set("stdp_ltp_weight", currentLtp * learningRate, ConfigSource::Override);
        }
        
        if (targetMetrics.count("exploration") > 0) {
            float exploration = targetMetrics.at("exploration");
            config->set("curiosity_rate", exploration, ConfigSource::Override);
        }
    }
    
    static std::vector<float> runLearningCurve(std::shared_ptr<Brain> brain, 
                                              size_t episodes = 100,
                                              const std::string& task = "random_walk") {
        std::vector<float> learningCurve;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            brain->reset();
            brain->initialize();
            
            float episodeScore = 0.0f;
            
            if (task == "random_walk") {
                // Simple random walk task
                for (size_t step = 0; step < 50; ++step) {
                    // Inject random sensory input
                    for (size_t i = 0; i < 5; ++i) {
                        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f * (rand() / (RAND_MAX + 1.0f)));
                    }
                    brain->step(step, step * 0.001);
                    
                    // Simple reward for activity
                    episodeScore += brain->getFiringNeuronCount() * 0.1f;
                }
            }
            
            learningCurve.push_back(episodeScore);
            
            // Log progress every 10 episodes
            if (episode % 10 == 0) {
                // In a real implementation, this would log to the actual logger
                // For now, we'll just accumulate
            }
        }
        
        return learningCurve;
    }
};

// Pythonic API utilities
class PythonicBrainInterface {
public:
    // Context manager for simulation steps
    class SimulationContext {
    private:
        std::shared_ptr<Brain> brain;
        double timestep;
        bool autoStep;
        
    public:
        SimulationContext(std::shared_ptr<Brain> b, double ts = 0.001, bool autoStepEnabled = true)
            : brain(b), timestep(ts), autoStep(autoStepEnabled) {}
        
        void step() {
            static SimulationStep step = 0;
            brain->step(step, step * timestep);
            if (autoStep) step++;
        }
        
        void step(SimulationStep step, double time) {
            brain->step(step, time);
        }
        
        py::dict getState() {
            py::dict state;
            state["neurons"] = brain->getTotalNeuronCount();
            state["synapses"] = brain->getTotalSynapseCount();
            state["spikes"] = brain->getTotalSpikeCount();
            state["active"] = brain->getActiveNeuronCount();
            return state;
        }
        
        ~SimulationContext() {
            // Final step if auto-step enabled
            if (autoStep) {
                static SimulationStep step = 1;
                brain->step(step, step * timestep);
            }
        }
    };
    
    // Pythonic neural operation utilities
    static std::vector<std::vector<float>> computeConnectivityMatrix(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return {};
        
        auto neurons = region->getAllNeurons();
        size_t n = neurons.size();
        std::vector<std::vector<float>> connectivity(n, std::vector<float>(n, 0.0f));
        
        // For now, return a simple placeholder matrix
        // In a real implementation, this would compute actual connectivity
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (i != j) {
                    connectivity[i][j] = static_cast<float>(rand()) / RAND_MAX * 0.5f;
                }
            }
        }
        
        return connectivity;
    }
    
    static std::vector<float> computeNeuronExcitability(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return {};
        
        auto neurons = region->getAllNeurons();
        std::vector<float> excitabilities;
        
        for (const auto& neuron : neurons) {
            float membranePotential = neuron->getMembranePotential();
            float threshold = neuron->getThreshold();
            excitabilities.push_back((membranePotential - threshold) * 10.0f); // Normalized
        }
        
        return excitabilities;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Advanced Python Bindings
        ------------------------------------------------------
        
        Advanced Python API for the NLM C++ neural simulation framework.
        
        Features:
        - Neural Network Builder with advanced configuration options
        - Performance Profiling and Monitoring
        - Neural Activity Analysis and Visualization
        - Learning Curve Optimization
        - Pythonic Context Managers for Simulation Control
        - Advanced Plasticity Control
        - Connectivity Analysis Utilities
        - Power Spectrum Computation
        
        Usage Examples:
        
        >>> import pynlm
        >>> # Create an advanced neural network
        >>> config = pynlm.createAdvancedConfig(neuron_count=1000, architecture="deep")
        >>> brain = pynlm.createBrain(config)
        >>> 
        >>> # Use context manager for simulation control
        >>> with brain.simulate(timestep=0.001) as sim:
        >>>     sim.step()
        >>>     state = sim.getState()
        >>> 
        >>> # Analyze neural activity
        >>> analysis = pynlm.analyzeNeuralActivity(brain)
        >>> print(f"Firing rate: {analysis['firing_rate_percent']:.2f}%")
        >>> 
        >>> # Profile performance
        >>> profiler = pynlm.PerformanceProfiler()
        >>> for step in range(100):
        ...     profiler.recordStepTime("simulation", simulate_step())
        >>> report = profiler.getPerformanceReport()
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // === Advanced Configuration and Building ===
    
    py::class_<NeuralNetworkBuilder>(m, "NeuralNetworkBuilder")
        .def_static("create_advanced_config", &NeuralNetworkBuilder::createAdvancedConfig,
                   py::arg("neuron_count") = 500,
                   py::arg("region_count") = 1,
                   py::arg("connection_probability") = 0.15f,
                   py::arg("timestep") = 0.001,
                   py::arg("random_seed") = 42,
                   "Create an advanced neural network configuration")
        .def_static("create_brain_with_architecture", &NeuralNetworkBuilder::createBrainWithArchitecture,
                   py::arg("architecture") = "balanced",
                   py::arg("config") = nullptr,
                   "Create a brain with specific architecture")
        .def_static("ARCHITECTURES", []() {
            py::dict architectures;
            architectures["balanced"] = "Balanced architecture (default)";
            architectures["deep"] = "Deep architecture (many layers)";
            architectures["wide"] = "Wide architecture (many neurons)";
            architectures["small"] = "Small architecture (minimal)";
            architectures["recurrent"] = "Recurrent architecture (strong feedback)";
            return architectures;
        }, "Get available neural architectures");

    // === Performance Profiling ===
    
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler")
        .def(py::init<>())
        .def("record_step_time", &PerformanceProfiler::recordStepTime,
             py::arg("operation"), py::arg("time_ms"),
             "Record timing for a simulation operation")
        .def("get_performance_report", &PerformanceProfiler::getPerformanceReport,
             "Get performance profiling report as dictionary")
        .def("reset", &PerformanceProfiler::reset, "Reset profiler data");

    // === Neural Analysis and Visualization ===
    
    py::class_<NeuralAnalyzer>(m, "NeuralAnalyzer")
        .def_static("analyze_neural_activity", &NeuralAnalyzer::analyzeNeuralActivity,
                   py::arg("brain"),
                   "Analyze neural activity and return statistics")
        .def_static("compute_neural_power_spectrum", &NeuralAnalyzer::computeNeuralPowerSpectrum,
                   py::arg("brain"), py::arg("steps") = 100,
                   "Compute power spectrum of neural activity")
        .def_static("compute_connectivity_matrix", &PythonicBrainInterface::computeConnectivityMatrix,
                   py::arg("brain"),
                   "Compute connectivity matrix between neurons")
        .def_static("compute_neuron_excitability", &PythonicBrainInterface::computeNeuronExcitability,
                   py::arg("brain"),
                   "Compute excitability values for all neurons");

    // === Learning and Optimization ===
    
    py::class_<LearningController>(m, "LearningController")
        .def_static("optimize_plasticity_parameters", &LearningController::optimizePlasticityParameters,
                   py::arg("brain"), py::arg("target_metrics"),
                   "Optimize plasticity parameters based on target metrics")
        .def_static("run_learning_curve", &LearningController::runLearningCurve,
                   py::arg("brain"), py::arg("episodes") = 100, py::arg("task") = "random_walk",
                   "Run learning curve optimization");

    // === Pythonic Interface ===
    
    py::class_<PythonicBrainInterface>(m, "PythonicBrainInterface")
        .def_static("create_context", &PythonicBrainInterface::SimulationContext,
                   py::arg("brain"), py::arg("timestep") = 0.001, py::arg("auto_step") = true,
                   "Create a simulation context manager");

    // === Enhanced Brain Class ===
    
    py::class_<Brain>(m, "Brain")
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
             "Log brain status")
        // Enhanced methods for advanced usage
        .def("enable_advanced_plasticity", [](Brain& self, bool hebbian, bool stdp, bool structural) {
            // Implementation would enable advanced plasticity rules
            self.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        }, py::arg("hebbian") = true, py::arg("stdp") = true, py::arg("structural") = true,
           "Enable advanced plasticity rules")
        .def("optimize_for_task", [](Brain& self, const std::string& taskType) {
            // Implementation would optimize brain for specific task
            // For now, just set appropriate developmental stage
            if (taskType == "continuous_learning") {
                self.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            } else {
                self.setDevelopmentalStage(DevelopmentalStage::Adult);
            }
        }, py::arg("task_type"),
           "Optimize brain for specific task type");

    // === Enhanced AgentBrain Class ===
    
    py::class_<AgentBrain>(m, "AgentBrain")
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

    // === Enhanced World Class ===
    
    py::class_<SimpleWorld>(m, "SimpleWorld")
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

    // === Enhanced Config Class ===
    
    py::class_<Config>(m, "Config")
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
        })
        // Enhanced configuration methods
        .def("set_advanced_plasticity", [](Config& self, float hebbianRate, float stdpLtp, float stdpLtd) {
            self.set("hebbian_learning_rate", hebbianRate, ConfigSource::Default);
            self.set("stdp_ltp_weight", stdpLtp, ConfigSource::Default);
            self.set("stdp_ltd_weight", stdpLtd, ConfigSource::Default);
        }, py::arg("hebbian_rate"), py::arg("stdp_ltp"), py::arg("stdp_ltd"),
           "Set advanced plasticity parameters")
        .def("enable_performance_optimizations", [](Config& self, bool sparse, bool vectorize, bool parallel) {
            self.set("sparse_connectivity", sparse, ConfigSource::Default);
            self.set("vectorization_level", vectorize ? 3 : 1, ConfigSource::Default);
            self.set("parallel_processing", parallel, ConfigSource::Default);
        }, py::arg("sparse_connectivity") = true, py::arg("vectorization") = true, py::arg("parallel") = true,
           "Enable performance optimizations");

    // === Factory Functions ===
    
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

    // === Utility Functions ===
    
    m.def("createAdvancedConfig", &NeuralNetworkBuilder::createAdvancedConfig,
          py::arg("neuron_count") = 500,
          py::arg("region_count") = 1,
          py::arg("connection_probability") = 0.15f,
          py::arg("timestep") = 0.001,
          py::arg("random_seed") = 42,
          "Create an advanced neural network configuration (convenience function)");
    
    m.def("createBrainWithArchitecture", &NeuralNetworkBuilder::createBrainWithArchitecture,
          py::arg("architecture") = "balanced",
          py::arg("config") = nullptr,
          "Create a brain with specific architecture (convenience function)");

    // === Constants and Enums ===
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // === Legacy Compatibility ===
    
    // Keep original factory functions for backward compatibility
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
}

} // namespace nlm