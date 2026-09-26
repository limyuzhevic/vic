#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryPercept.hpp"
#include "../src/core/Constants.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced configuration helper class
class AdvancedConfig {
public:
    std::shared_ptr<Config> config;
    
    AdvancedConfig() : config(std::make_shared<Config>()) {}
    
    AdvancedConfig& setBrainParameters(
        int64_t neuronCount = 1000,
        int64_t regionCount = 1,
        float connectionProbability = 0.15f,
        float synapseDensity = 0.1f
    ) {
        config->set("brain.neuron_count", neuronCount, ConfigSource::Default);
        config->set("brain.region_count", regionCount, ConfigSource::Default);
        config->set("brain.connection_probability", connectionProbability, ConfigSource::Default);
        config->set("brain.synapse_density", synapseDensity, ConfigSource::Default);
        return *this;
    }
    
    AdvancedConfig& setPlasticityParameters(
        bool enableSTDP = true,
        float stdpLTP = 0.02f,
        float stdpLTD = 0.015f,
        float stdpTau = 20.0f,
        bool enableHebbian = true,
        float hebbianRate = 0.001f
    ) {
        config->set("plasticity.stdp.enable", enableSTDP, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", stdpLTP, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", stdpTau, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", stdpTau, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", enableHebbian, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", hebbianRate, ConfigSource::Default);
        return *this;
    }
    
    AdvancedConfig& setDevelopmentParameters(
        bool enableDevelopment = true,
        DevelopmentalStage initialStage = DevelopmentalStage::Initial
    ) {
        config->set("development.enabled", enableDevelopment, ConfigSource::Default);
        config->set("development.initial_stage", static_cast<int64_t>(initialStage), ConfigSource::Default);
        return *this;
    }
    
    AdvancedConfig& setNeuromodulationParameters(
        bool enableCuriosity = true,
        float curiosityScale = 0.1f,
        bool enableNovelty = true,
        float noveltyThreshold = 0.5f
    ) {
        config->set("neuromod.curiosity.enable", enableCuriosity, ConfigSource::Default);
        config->set("neuromod.curiosity.scale", curiosityScale, ConfigSource::Default);
        config->set("neuromod.novelty.enable", enableNovelty, ConfigSource::Default);
        config->set("neuromod.novelty.threshold", noveltyThreshold, ConfigSource::Default);
        return *this;
    }
    
    std::shared_ptr<Config> getConfig() const { return config; }
};

// Performance profiling class
class PerformanceProfiler {
public:
    struct Metrics {
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        size_t activeNeurons = 0;
        size_t firingNeurons = 0;
        float averageFiringRate = 0.0f;
        float excitationInhibitionRatio = 0.0f;
        double elapsedTime = 0.0;
        std::vector<double> stepTimes;
        
        Metrics() {}
        
        void recordStep(double stepTime) {
            totalSteps++;
            stepTimes.push_back(stepTime);
            if (stepTimes.size() > 100) {
                stepTimes.erase(stepTimes.begin());
            }
        }
        
        double getAverageStepTime() const {
            if (stepTimes.empty()) return 0.0;
            double sum = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0);
            return sum / stepTimes.size();
        }
        
        double getStepsPerSecond() const {
            if (elapsedTime <= 0.0) return 0.0;
            return totalSteps / elapsedTime;
        }
    };
    
    std::shared_ptr<Metrics> metrics;
    std::shared_ptr<Brain> brain;
    bool profilingEnabled = true;
    
    PerformanceProfiler(std::shared_ptr<Brain> b) : brain(b) {
        metrics = std::make_shared<Metrics>();
    }
    
    void start() {
        metrics->totalSteps = 0;
        metrics->totalSpikes = 0;
        metrics->stepTimes.clear();
    }
    
    void recordStep() {
        if (!profilingEnabled) return;
        
        // Get current time
        auto now = std::chrono::high_resolution_clock::now();
        if (metrics->totalSteps == 0) {
            metrics->startTime = now;
        } else {
            auto duration = std::chrono::duration<double>(now - metrics->startTime);
            metrics->recordStep(duration.count());
            metrics->elapsedTime = duration.count();
        }
        
        // Update neural metrics
        metrics->totalSpikes = brain->getTotalSpikeCount();
        metrics->activeNeurons = brain->getActiveNeuronCount();
        metrics->firingNeurons = brain->getFiringNeuronCount();
        metrics->averageFiringRate = brain->getAverageFiringRate();
        metrics->excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSteps() const { return metrics->totalSteps; }
    size_t getTotalSpikes() const { return metrics->totalSpikes; }
    double getElapsedTime() const { return metrics->elapsedTime; }
    double getAverageStepTime() const { return metrics->getAverageStepTime(); }
    double getStepsPerSecond() const { return metrics->getStepsPerSecond(); }
    
    std::string getMetricsSummary() const {
        std::stringstream ss;
        ss << "Performance Metrics:" << std::endl;
        ss << "  Total Steps: " << metrics->totalSteps << std::endl;
        ss << "  Total Spikes: " << metrics->totalSpikes << std::endl;
        ss << "  Active Neurons: " << metrics->activeNeurons << std::endl;
        ss << "  Firing Neurons: " << metrics->firingNeurons << std::endl;
        ss << "  Average Firing Rate: " << metrics->averageFiringRate << " Hz" << std::endl;
        ss << "  E/I Ratio: " << metrics->excitationInhibitionRatio << std::endl;
        ss << "  Elapsed Time: " << metrics->elapsedTime << " s" << std::endl;
        ss << "  Steps per Second: " << getStepsPerSecond() << std::endl;
        ss << "  Average Step Time: " << getAverageStepTime() * 1000 << " ms" << std::endl;
        return ss.str();
    }
};

// Convenience functions
void addAdvancedBindings(py::module& m) {
    // AdvancedConfig class
    py::class_<AdvancedConfig>(m, "AdvancedConfig")
        .def(py::init<>())
        .def("setBrainParameters", &AdvancedConfig::setBrainParameters,
             py::arg("neuronCount") = 1000,
             py::arg("regionCount") = 1,
             py::arg("connectionProbability") = 0.15f,
             py::arg("synapseDensity") = 0.1f)
        .def("setPlasticityParameters", &AdvancedConfig::setPlasticityParameters,
             py::arg("enableSTDP") = true,
             py::arg("stdpLTP") = 0.02f,
             py::arg("stdpLTD") = 0.015f,
             py::arg("stdpTau") = 20.0f,
             py::arg("enableHebbian") = true,
             py::arg("hebbianRate") = 0.001f)
        .def("setDevelopmentParameters", &AdvancedConfig::setDevelopmentParameters,
             py::arg("enableDevelopment") = true,
             py::arg("initialStage") = DevelopmentalStage::Initial)
        .def("setNeuromodulationParameters", &AdvancedConfig::setNeuromodulationParameters,
             py::arg("enableCuriosity") = true,
             py::arg("curiosityScale") = 0.1f,
             py::arg("enableNovelty") = true,
             py::arg("noveltyThreshold") = 0.5f)
        .def("getConfig", &AdvancedConfig::getConfig);

    // PerformanceProfiler class
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("start", &PerformanceProfiler::start)
        .def("recordStep", &PerformanceProfiler::recordStep)
        .def("getTotalSteps", &PerformanceProfiler::getTotalSteps)
        .def("getTotalSpikes", &PerformanceProfiler::getTotalSpikes)
        .def("getElapsedTime", &PerformanceProfiler::getElapsedTime)
        .def("getAverageStepTime", &PerformanceProfiler::getAverageStepTime)
        .def("getStepsPerSecond", &PerformanceProfiler::getStepsPerSecond)
        .def("getMetricsSummary", &PerformanceProfiler::getMetricsSummary);

    // Convenience functions
    m.def("createAdvancedConfig", []() {
        return std::make_shared<AdvancedConfig>();
    }, "Create an advanced configuration helper");

    m.def("createPerformanceProfiler", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<PerformanceProfiler>(brain);
    }, py::arg("brain"), "Create a performance profiler for monitoring brain activity");
}
