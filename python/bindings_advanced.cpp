#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <filesystem>
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
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Brain Extensions
PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        Advanced NLM (Neural Learning Machine) Python Extensions
        ------------------------------------------------------
        Enhanced Python API for advanced users and power users.
        Provides extended functionality, scripting capabilities, and
        advanced configuration options.
    )pbdoc";

    // === ADVANCED BRAIN CLASS ===
    py::class_<Brain, std::shared_ptr<Brain>>(m, "AdvancedBrain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize)
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"))
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"))
        .def("runEpisode", [](std::shared_ptr<Brain> brain, size_t steps,
                            const std::function<void(size_t, const std::shared_ptr<Brain>&)>& callback) {
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
                if (callback) callback(i, brain);
            }
        }, py::arg("steps"), py::arg("callback"),
             "Run an episode with progress callback")
        .def("runBatchSimulation", [](std::shared_ptr<Brain> brain, size_t steps,
                                     const std::vector<float>& inputs,
                                     const std::vector<float>& expected_outputs) {
            std::vector<float> results;
            results.reserve(steps);
            
            for (size_t i = 0; i < steps; ++i) {
                if (i < inputs.size()) {
                    brain->receiveSensoryInput(std::make_shared<Vision>(1, 1, 1, inputs[i]));
                }
                brain->step(i);
                auto action = brain->produceAction();
                float output = action ? action->getParameters().empty() ? 0.0f : action->getParameters()[0] : 0.0f;
                results.push_back(output);
                
                if (i < expected_outputs.size()) {
                    float error = output - expected_outputs[i];
                    brain->applyNeuromodulation(std::make_shared<Neuromodulator>(error, 0.0f, 0.0f, 0.0f, 0.0f));
                }
            }
            return results;
        }, py::arg("steps"), py::arg("inputs"), py::arg("expected_outputs"),
             "Run batch simulation and return outputs")
        .def("getActivityPattern", [](std::shared_ptr<Brain> brain, size_t numSamples = 100) {
            std::vector<float> pattern;
            pattern.reserve(numSamples);
            
            for (size_t i = 0; i < numSamples; ++i) {
                brain->step(i);
                float activity = brain->getFiringNeuronCount() * 1.0f / brain->getTotalNeuronCount();
                pattern.push_back(activity);
            }
            return pattern;
        }, py::arg("numSamples") = 100, "Get activity pattern over time")
        .def("saveSession", [](std::shared_ptr<Brain> brain, const std::string& prefix,
                               bool saveConfig = true, bool saveWeights = true,
                               bool saveState = true) {
            std::map<std::string, bool> saves;
            saves["config"] = saveConfig;
            saves["weights"] = saveWeights;
            saves["state"] = saveState;
            
            std::string prefixPath = prefix.empty() ? "nlm_session_" : prefix;
            
            if (saveConfig) {
                brain->getConfig()->saveToFile(prefixPath + "_config.json");
            }
            
            if (saveWeights || saveState) {
                brain->save(prefixPath + "_brain_state.bin");
            }
            
            return saves;
        }, py::arg("prefix") = "nlm_session", py::arg("saveConfig") = true,
             py::arg("saveWeights") = true, py::arg("saveState") = true,
             "Save brain session with various components")
        .def("loadSession", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                              bool loadConfig = true, bool loadState = true) {
            bool configLoaded = false;
            bool stateLoaded = false;
            
            if (loadConfig && std::filesystem::exists(filepath + "_config.json")) {
                configLoaded = brain->getConfig()->loadFromFile(filepath + "_config.json");
            }
            
            if (loadState && std::filesystem::exists(filepath + "_brain_state.bin")) {
                stateLoaded = brain->load(filepath + "_brain_state.bin");
            }
            
            return std::make_tuple(configLoaded, stateLoaded);
        }, py::arg("filepath"), py::arg("loadConfig") = true,
             py::arg("loadState") = true, "Load brain session")
        .def("exportNeuralData", [](std::shared_ptr<Brain> brain, const std::string& filename,
                                   const std::vector<RegionId>& regions,
                                   const std::vector<float>& timePoints) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filename);
            }
            
            file << "Time,Step,FiringRate,EI_Ratio,TotalNeurons,TotalSynapses\n";
            
            for (float timePoint : timePoints) {
                size_t step = static_cast<size_t>(timePoint * 1000.0f);  // Convert to step
                brain->step(step);
                
                float firingRate = brain->getAverageFiringRate();
                float eiRatio = brain->getExcitationInhibitionRatio();
                size_t totalNeurons = brain->getTotalNeuronCount();
                size_t totalSynapses = brain->getTotalSynapseCount();
                
                file << timePoint << "," << step << "," << firingRate << "," << eiRatio
                     << "," << totalNeurons << "," << totalSynapses << "\n";
            }
            
            file.close();
            return std::vector<std::string>{"data_exported", filename};
        }, py::arg("filename"), py::arg("regions") = std::vector<RegionId>(),
             py::arg("timePoints") = std::vector<float>(), "Export neural data to CSV");

    // === ADVANCED CONFIG CLASS ===
    py::class_<Config, std::shared_ptr<Config>>(m, "AdvancedConfig")
        .def(py::init<>())
        .def("createFromTemplate", [](const std::string& templateName) {
            auto config = std::make_shared<Config>();
            
            if (templateName == "minimal") {
                // Minimal configuration for fast testing
                config->set("brain.neuron_count", 100);
                config->set("brain.synapse_density", 0.05);
                config->set("plasticity.stdp.enable", true);
                config->set("plasticity.hebbian.enable", false);
                config->set("neuromod.dopamine.scale", 1.0f);
            } else if (templateName == "balanced") {
                // Balanced configuration for general use
                config->set("brain.neuron_count", 1000);
                config->set("brain.synapse_density", 0.1);
                config->set("plasticity.stdp.enable", true);
                config->set("plasticity.stdp.learning_rate", 0.001f);
                config->set("plasticity.hebbian.enable", true);
                config->set("plasticity.hebbian.learning_rate", 0.001f);
                config->set("neuromod.curiosity.enable", true);
                config->set("neuromod.novelty.enable", true);
                config->set("memory.enable", true);
                config->set("prediction.enable", true);
            } else if (templateName == "high_performance") {
                // High-performance configuration
                config->set("brain.neuron_count", 5000);
                config->set("brain.synapse_density", 0.2);
                config->set("plasticity.stdp.enable", true);
                config->set("plasticity.stdp.learning_rate", 0.002f);
                config->set("plasticity.hebbian.enable", true);
                config->set("plasticity.hebbian.learning_rate", 0.001f);
                config->set("neuromod.curiosity.enable", true);
                config->set("neuromod.novelty.enable", true);
                config->set("memory.enable", true);
                config->set("prediction.enable", true);
                config->set("plasticity.structural.enable", true);
                config->set("development.enable", true);
            }
            
            return config;
        }, py::arg("templateName"), "Create config from predefined template")
        .def("validateConfiguration", [](std::shared_ptr<Config> config) {
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
            
            // Check neuron count
            if (!config->has("brain.neuron_count")) {
                warnings.push_back("No neuron_count specified, using default");
            }
            
            // Check synapse density range
            if (config->has("brain.synapse_density")) {
                auto density = config->get<double>("brain.synapse_density");
                if (density < 0.0 || density > 1.0) {
                    errors.push_back("Synapse density must be between 0.0 and 1.0");
                }
            }
            
            // Check plasticity settings
            if (config->has("plasticity.stdp.enable") &&
                config->get<bool>("plasticity.stdp.enable")) {
                if (!config->has("plasticity.stdp.learning_rate")) {
                    warnings.push_back("STDP enabled but no learning rate specified");
                }
            }
            
            // Check memory constraints
            if (config->has("brain.neuron_count")) {
                auto neuronCount = config->get<int64_t>("brain.neuron_count");
                if (neuronCount > 100000) {
                    warnings.push_back("Large neuron count may impact performance");
                }
            }
            
            return std::make_tuple(errors, warnings);
        }, "Validate configuration and return errors/warnings")
        .def("generateRandomConfig", [](std::shared_ptr<Config> config, size_t minNeurons = 500,
                                        size_t maxNeurons = 5000) {
            auto rng = std::mt19937(std::random_device{}());
            std::uniform_real_distribution<float> densityDist(0.05f, 0.3f);
            std::uniform_real_distribution<float> stdDevDist(0.0f, 0.1f);
            
            config->set("brain.neuron_count", static_cast<int64_t>(minNeurons + 
                (rng() % static_cast<int>(maxNeurons - minNeurons + 1))));
            config->set("brain.synapse_density", densityDist(rng));
            config->set("brain.initial_weight_mean", stdDevDist(rng));
            config->set("brain.initial_weight_std", stdDevDist(rng));
            
            return config;
        }, py::arg("minNeurons") = 500, py::arg("maxNeurons") = 5000,
             "Generate random configuration within specified range");

    // === ADVANCED AGENT BRAIN CLASS ===
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AdvancedAgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("createLearningEnvironment", [](std::shared_ptr<AgentBrain> agent,
                                           std::shared_ptr<SimpleWorld> world,
                                           const std::string& taskType,
                                           const std::map<std::string, float>& parameters) {
            agent->initialize(world);
            
            // Configure based on task type
            if (taskType == "reinforcement_learning") {
                agent->enableRewardModulation(true);
                agent->enableStructuralPlasticity(true);
                agent->enableDevelopment(true);
                agent->enableCuriosity(true);
            } else if (taskType == "exploration") {
                agent->enableCuriosity(true);
                agent->enableNoveltyDetection(true);
                agent->enableDevelopment(true);
            } else if (taskType == "pattern_completion") {
                agent->enableRewardModulation(true);
                agent->enableStructuralPlasticity(true);
            }
            
            return true;
        }, py::arg("world"), py::arg("taskType"), py::arg("parameters"),
             "Create specialized learning environment")
        .def("trainAgent", [](std::shared_ptr<AgentBrain> agent,
                            std::shared_ptr<SimpleWorld> world,
                            size_t episodes,
                            const std::function<void(size_t, float, float)>& progressCallback) {
            std::vector<float> episodeRewards;
            std::vector<float> episodeSteps;
            
            for (size_t episode = 0; episode < episodes; ++episode) {
                world->reset();
                float totalReward = 0.0f;
                size_t stepsInEpisode = 0;
                
                for (size_t step = 0; step < 1000; ++step) {
                    world->update(0.1f);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    auto brain = agent->getBrain();
                    brain->step(step);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Calculate reward (simplified)
                    float reward = percept->getInternal().empty() ? 0.0f : percept->getInternal()[0];
                    totalReward += reward;
                    agent->applyRewardModulation(reward, 0.0f);
                    
                    if (progressCallback) {
                        progressCallback(episode, totalReward, stepsInEpisode);
                    }
                    
                    stepsInEpisode++;
                    if (world->getSimulationTime() > 10.0f) break; // Episode timeout
                }
                
                episodeRewards.push_back(totalReward);
                episodeSteps.push_back(static_cast<float>(stepsInEpisode));
            }
            
            return std::make_tuple(episodeRewards, episodeSteps);
        }, py::arg("episodes"), py::arg("progressCallback") = nullptr,
             "Train agent and return episode statistics")
        .def("evaluateAgent", [](std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world,
                               const std::vector<float>& testInputs) {
            std::vector<float> performanceMetrics;
            std::vector<float> completionRates;
            
            for (size_t i = 0; i < testInputs.size(); ++i) {
                world->reset();
                
                // Create test input percept
                auto vision = std::make_shared<Vision>(1, 1, 1, testInputs[i]);
                auto percept = std::make_shared<SensoryPercept>();
                percept->setVision(vision);
                
                agent->processSensoryInput(percept);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, 0.0f);
                
                // Calculate performance (simplified)
                float metric = static_cast<float>(i) / static_cast<float>(testInputs.size());
                float completion = std::min(1.0f, testInputs[i] * 0.5f);
                
                performanceMetrics.push_back(metric);
                completionRates.push_back(completion);
            }
            
            return std::make_tuple(performanceMetrics, completionRates);
        }, py::arg("testInputs"), "Evaluate agent performance on test inputs");

    // === PLACILITY RULE MANAGEMENT ===
    py::class_<STDP, std::shared_ptr<STDP>>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)")
        .def(py::init<>())
        .def("update", &STDP::update)
        .def("applyWeightChange", &STDP::applyWeightChange)
        .def("getName", &STDP::getName)
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"))
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"))
        .def("getLTPWeight", &STDP::getLTPWeight)
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"))
        .def("getLTDWeight", &STDP::getLTDWeight)
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"))
        .def("getTimeConstant", &STDP::getTimeConstant);

    py::class_<Hebbian, std::shared_ptr<Hebbian>>(m, "Hebbian", R"pbdoc(Hebbian learning rule)")
        .def(py::init<>())
        .def("update", &Hebbian::update)
        .def("applyWeightChange", &Hebbian::applyWeightChange)
        .def("getName", &Hebbian::getName)
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &Hebbian::getLearningRate)
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"))
        .def("getMaxWeight", &Hebbian::getMaxWeight);

    // === ADVANCED HELPERS ===
    m.def("generateNeuralNetworkTopology", [](size_t neuronCount,
                                            const std::string& topologyType,
                                            float connectionProbability) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        config->set("brain.connection_probability", connectionProbability);
        
        if (topologyType == "small_world") {
            config->set("brain.topology", "small_world");
        } else if (topologyType == "scale_free") {
            config->set("brain.topology", "scale_free");
        } else if (topologyType == "regular_grid") {
            config->set("brain.topology", "regular_grid");
        }
        
        return config;
    }, py::arg("neuronCount"), py::arg("topologyType"),
         py::arg("connectionProbability") = 0.1f,
         "Generate neural network topology configuration");

    m.def("createSyntheticDataset", [](size_t samples, size_t features,
                                      const std::string& noiseLevel) {
        std::vector<std::vector<float>> dataset;
        std::mt19937 rng(std::random_device{}());
        std::normal_distribution<float> dist(0.0f, 1.0f);
        
        for (size_t i = 0; i < samples; ++i) {
            std::vector<float> sample;
            for (size_t j = 0; j < features; ++j) {
                float value = dist(rng);
                // Add noise based on noise level
                if (noiseLevel == "high") {
                    value += dist(rng) * 0.5f;
                } else if (noiseLevel == "low") {
                    value += dist(rng) * 0.1f;
                }
                sample.push_back(value);
            }
            dataset.push_back(sample);
        }
        
        return dataset;
    }, py::arg("samples"), py::arg("features"),
         py::arg("noiseLevel") = "medium",
         "Create synthetic dataset for testing");

    // === PERFORMANCE MONITORING ===
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
        .def(py::init<>())
        .def("startMonitoring", &PerformanceMonitor::startMonitoring)
        .def("stopMonitoring", &PerformanceMonitor::stopMonitoring)
        .def("getMetrics", &PerformanceMonitor::getMetrics)
        .def("resetMetrics", &PerformanceMonitor::resetMetrics)
        .def("logTime", &PerformanceMonitor::logTime, py::arg("operation"),
              py::arg("duration"))
        .def("logMemory", &PerformanceMonitor::logMemory,
              py::arg("memoryUsage"));

    // === SCRIPTING INTERFACE ===
    m.def("createScriptedBrain", [](const std::string& script) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        
        // Simple script parsing (placeholder for advanced script execution)
        if (script.find("fast") != std::string::npos) {
            config->set("brain.neuron_count", 500);
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("script"), "Create brain from configuration script");

    // === UTILITY FUNCTIONS ===
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t steps,
                              const std::string& testType) {
        auto monitor = std::make_shared<PerformanceMonitor>();
        monitor->startMonitoring();
        
        if (testType == "step_time") {
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
            }
        } else if (testType == "spike_processing") {
            for (size_t i = 0; i < steps; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 10.0f);
                brain->step(i);
            }
        }
        
        monitor->stopMonitoring();
        auto metrics = monitor->getMetrics();
        
        return std::make_tuple(
            metrics.peakMemoryUsage,
            metrics.averageStepTime,
            metrics.spikesPerStep,
            metrics.cpuUsage
        );
    }, py::arg("brain"), py::arg("steps"), py::arg("testType") = "step_time",
         "Benchmark brain performance");

    // === ERROR HANDLING ===
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        }
    });

    // === VERSION AND METADATA ===
    m.attr("__version__") = "2.0.0-advanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Advanced Python extensions for NLM neural simulation";
}

// Forward declaration for PerformanceMonitor
class PerformanceMonitor {
private:
    bool monitoringActive_;
    float peakMemoryUsage_;
    float averageStepTime_;
    float spikesPerStep_;
    float cpuUsage_;
    std::unordered_map<std::string, std::vector<float>> timeLogs_;
    std::vector<float> memoryLogs_;

public:
    PerformanceMonitor() : monitoringActive_(false),
                          peakMemoryUsage_(0.0f), averageStepTime_(0.0f),
                          spikesPerStep_(0.0f), cpuUsage_(0.0f) {}

    void startMonitoring() {
        monitoringActive_ = true;
        timeLogs_.clear();
        memoryLogs_.clear();
    }

    void stopMonitoring() {
        monitoringActive_ = false;
    }

    std::tuple<float, float, float, float> getMetrics() const {
        return std::make_tuple(peakMemoryUsage_, averageStepTime_,
                              spikesPerStep_, cpuUsage_);
    }

    void resetMetrics() {
        peakMemoryUsage_ = 0.0f;
        averageStepTime_ = 0.0f;
        spikesPerStep_ = 0.0f;
        cpuUsage_ = 0.0f;
        timeLogs_.clear();
        memoryLogs_.clear();
    }

    void logTime(const std::string& operation, float duration) {
        timeLogs_[operation].push_back(duration);
        if (!timeLogs_[operation].empty()) {
            float sum = std::accumulate(timeLogs_[operation].begin(),
                                      timeLogs_[operation].end(), 0.0f);
            averageStepTime_ = sum / timeLogs_[operation].size();
        }
    }

    void logMemory(float memoryUsage) {
        memoryLogs_.push_back(memoryUsage);
        peakMemoryUsage_ = std::max(peakMemoryUsage_, memoryUsage);
    }
};

} // namespace nlm
