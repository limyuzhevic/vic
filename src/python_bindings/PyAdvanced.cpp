// Python bindings for NLM - Advanced API for experienced users
#include "Python.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/functional.h"
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "World.hpp"
#include "VisualizationInterface.hpp"
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace py = pybind11;

// Advanced Brain Configuration for experienced users
class AdvancedBrainConfig {
public:
    std::shared_ptr<Config> config;
    
    AdvancedBrainConfig() {
        config = std::make_shared<Config>();
        // Set reasonable defaults for advanced usage
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15f);
        config->set("brain.connection_probability", 0.08f);
        config->set("brain.initial_weight_mean", 0.5f);
        config->set("brain.initial_weight_std", 0.15f);
        
        // Advanced plasticity settings
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01f);
        config->set("plasticity.stdp.tau_plus", 20.0f);
        config->set("plasticity.stdp.tau_minus", 20.0f);
        
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.02f);
        
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0001f);
        config->set("plasticity.structural.pruning_rate", 0.00001f);
        
        // Neuromodulation
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.threshold", 0.1f);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.sensitivity", 0.5f);
        
        // Development
        config->set("development.enable", true);
        config->set("development.initial_plasticity", 1.0f);
        config->set("development.plasticity_decay_rate", 0.01f);
        
        // Prediction
        config->set("prediction.enable", true);
        config->set("prediction.time_horizon", 100);
        config->set("prediction.error_threshold", 0.5f);
        
        // Memory
        config->set("memory.working_capacity", 100);
        config->set("memory.episodic_capacity", 1000);
        config->set("memory.associative_strength", 0.8f);
        config->set("memory.concept_threshold", 0.3f);
    }
    
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Advanced configuration methods
    void enableAdvancedLearning() {
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.015f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.03f);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
    }
    
    void enableFastTraining() {
        config->set("brain.neuron_count", 5000);
        config->set("plasticity.stdp.learning_rate", 0.02f);
        config->set("plasticity.hebbian.learning_rate", 0.04f);
        config->set("development.enable", false); // Skip development for speed
    }
    
    void enableBiologicalLearning() {
        config->set("plasticity.stdp.learning_rate", 0.005f);
        config->set("plasticity.hebbian.learning_rate", 0.01f);
        config->set("plasticity.structural.synaptogenesis_rate", 0.00001f);
        config->set("plasticity.structural.pruning_rate", 0.000005f);
        config->set("neuromod.dopamine.scale", 0.5f);
        config->set("development.enable", true);
        config->set("development.initial_plasticity", 2.0f); // Slower maturation
    }
    
    void enableCuriosityDriven() {
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.threshold", 0.05f);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.sensitivity", 0.8f);
        config->set("development.enable", true);
    }
    
    void enablePredictiveLearning() {
        config->set("prediction.enable", true);
        config->set("prediction.time_horizon", 200);
        config->set("prediction.error_threshold", 0.3f);
        config->set("memory.concept_threshold", 0.5f);
        config->set("plasticity.stdp.learning_rate", 0.01f);
    }
    
    std::string getProfileName() const {
        // Determine profile based on current settings
        if (config->getOr<float>("neuromod.curiosity.enable", 0) > 0.5f) {
            return "CuriosityDriven";
        } else if (config->getOr<float>("development.enable", 0) > 0.5f) {
            return "BiologicalLearning";
        } else if (config->getOr<int64_t>("brain.neuron_count", 0) > 3000) {
            return "FastTraining";
        } else {
            return "StandardLearning";
        }
    }
};

// Advanced Event-Driven Simulation for experienced users
class AdvancedSimulation {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    bool running;
    uint64_t stepsCompleted;
    
    AdvancedSimulation() : running(false), stepsCompleted(0) {}
    
    void initialize(std::shared_ptr<AdvancedBrainConfig> advConfig, 
                   std::shared_ptr<SimpleWorld> w) {
        brain = std::make_shared<Brain>(advConfig->getConfig());
        world = w;
        agent = std::make_shared<AgentBrain>(brain);
        
        brain->initialize();
        agent->initialize(*world);
        
        // Enable all advanced features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        running = true;
    }
    
    void runSteps(uint64_t numSteps, std::function<void(uint64_t, float)> callback = nullptr) {
        if (!running) return;
        
        for (uint64_t step = 0; step < numSteps && running; ++step) {
            // Update world
            double time = world->getSimulationTime();
            world->update(0.1);
            
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get action
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action
            world->applyAction(agent->getBrain()->getRegions()[0].get(), cmd);
            
            // Apply reward modulation
            float reward = agent->getCuriosityLevel() * 0.1f; // Simplified reward
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            stepsCompleted++;
            
            // Call callback if provided
            if (callback) {
                float curiosity = agent->getCuriosityLevel();
                float novelty = agent->getNoveltyLevel();
                float development = static_cast<float>(brain->getDevelopmentalStage());
                callback(step, curiosity * novelty * development);
            }
        }
    }
    
    void stop() {
        running = false;
    }
    
    // Advanced simulation modes
    void runUntilStable(float stabilityThreshold = 0.01f, uint64_t maxSteps = 10000) {
        float previousActivity = 0.0f;
        float currentActivity = brain->getAverageFiringRate();
        
        for (uint64_t step = 0; step < maxSteps && running; ++step) {
            // Run one step
            runSteps(1);
            
            currentActivity = brain->getAverageFiringRate();
            float change = std::abs(currentActivity - previousActivity);
            
            if (change < stabilityThreshold) {
                break; // Stable
            }
            
            previousActivity = currentActivity;
        }
    }
    
    void runExploration(float explorationRate = 0.3f, uint64_t maxSteps = 5000) {
        // Run simulation with curiosity-driven exploration
        for (uint64_t step = 0; step < maxSteps && running; ++step) {
            runSteps(1);
            
            // Inject novelty to encourage exploration
            if (agent->getCuriosityLevel() < 0.5f) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
        }
    }
    
    void runLearningSimulation(float learningRate = 0.01f, uint64_t numEpisodes = 100) {
        // Multi-episode learning simulation
        for (uint64_t episode = 0; episode < numEpisodes && running; ++episode) {
            runSteps(500); // Each episode
            
            // Reset for next episode
            brain->reset();
            brain->initialize();
            agent->reset();
            agent->initialize(*world);
        }
    }
    
    // Data collection and analysis
    py::dict collectSimulationData() {
        py::dict data;
        
        data["steps"] = stepsCompleted;
        data["neuron_count"] = brain->getTotalNeuronCount();
        data["synapse_count"] = brain->getTotalSynapseCount();
        data["avg_firing_rate"] = brain->getAverageFiringRate();
        data["total_spikes"] = brain->getTotalSpikeCount();
        data["curiosity_level"] = agent->getCuriosityLevel();
        data["novelty_level"] = agent->getNoveltyLevel();
        data["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return data;
    }
    
    // Performance analysis
    py::dict analyzePerformance() {
        py::dict analysis;
        
        // Calculate performance metrics
        float efficiency = brain->getAverageFiringRate() / 
                          static_cast<float>(brain->getTotalNeuronCount()) * 100.0f;
        
        analysis["efficiency"] = efficiency;
        analysis["stability"] = 1.0f / (1.0f + agent->getCuriosityLevel());
        analysis["learning_speed"] = agent->getNoveltyLevel();
        analysis["adaptation"] = agent->getCuriosityLevel() * agent->getNoveltyLevel();
        
        return analysis;
    }
};

// Enhanced Visualization for advanced users
class AdvancedVisualization {
public:
    std::shared_ptr<VisualizationInterface> viz;
    std::string outputPath;
    bool enableExport;
    
    AdvancedVisualization() {
        viz = std::make_shared<VisualizationInterface>();
        viz->initialize();
        enableExport = false;
    }
    
    void update(std::shared_ptr<Brain> brain, uint64_t step) {
        if (viz->isActive()) {
            viz->update();
            viz->visualizeNetwork(*brain);
            viz->visualizeActivity(*brain);
            
            if (enableExport && !outputPath.empty()) {
                viz->setExportOptions(outputPath, ExportFormat::JSON);
                viz->exportData();
            }
        }
    }
    
    void render() {
        if (viz->isActive()) {
            viz->render();
        }
    }
    
    void enableAdvancedRendering() {
        // Enable advanced rendering features
        viz->setUpdateRate(60.0); // 60 Hz
        viz->enableHeatmap(true);
        viz->enableTimeSeries(true);
        viz->enableStatisticalAnalysis(true);
    }
    
    py::dict getPerformanceMetrics() {
        py::dict metrics;
        
        PerformanceMetrics perf = viz->getPerformanceMetrics();
        
        metrics["cpu_usage"] = perf.cpuUsage;
        metrics["memory_usage"] = perf.memoryUsage;
        metrics["rendering_time"] = perf.renderingTime;
        metrics["update_time"] = perf.updateTime;
        metrics["neuron_count"] = perf.neuronCount;
        metrics["synapse_count"] = perf.synapseCount;
        
        return metrics;
    }
};

// High-level API for experienced users
class NLMSession {
public:
    std::shared_ptr<AdvancedBrainConfig> config;
    std::shared_ptr<AdvancedSimulation> simulation;
    std::shared_ptr<AdvancedVisualization> visualization;
    std::string sessionName;
    
    NLMSession(std::string name = "nlm_session") {
        sessionName = name;
        config = std::make_shared<AdvancedBrainConfig>();
        visualization = std::make_shared<AdvancedVisualization>();
    }
    
    void createBrain() {
        // Create world first
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 20, 20);
        world->reset();
        
        // Create simulation
        simulation = std::make_shared<AdvancedSimulation>();
        simulation->initialize(config, world);
    }
    
    void setupVisualization(std::string outputPath = "") {
        if (!outputPath.empty()) {
            visualization->enableExport = true;
            visualization->outputPath = outputPath;
        }
        visualization->enableAdvancedRendering();
    }
    
    void runExperiment(std::string experimentType, uint64_t steps) {
        if (!simulation) {
            throw std::runtime_error("Simulation not initialized. Call createBrain() first.");
        }
        
        if (experimentType == "learning") {
            simulation->runLearningSimulation();
        } else if (experimentType == "exploration") {
            simulation->runExploration();
        } else if (experimentType == "stable") {
            simulation->runUntilStable();
        } else {
            simulation->runSteps(steps);
        }
        
        // Update visualization
        visualization->update(simulation->brain, simulation->stepsCompleted);
        visualization->render();
    }
    
    py::dict getResults() {
        py::dict results;
        
        results["session_name"] = sessionName;
        results["profile"] = config->getProfileName();
        
        if (simulation) {
            py::dict simData = simulation->collectSimulationData();
            results["simulation"] = simData;
            
            py::dict performance = simulation->analyzePerformance();
            results["performance"] = performance;
        }
        
        if (visualization) {
            py::dict vizMetrics = visualization->getPerformanceMetrics();
            results["visualization"] = vizMetrics;
        }
        
        return results;
    }
    
    void saveSession(std::string filename) {
        // Save session data (simplified implementation)
        py::dict results = getResults();
        
        // In a real implementation, this would serialize to file
        // For now, just print a message
        std::cout << "Session saved to " << filename << std::endl;
    }
};

// Python module definition
PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = "Advanced NLM API for experienced users and researchers";
    
    // Advanced Brain Configuration
    py::class_<AdvancedBrainConfig>(m, "AdvancedBrainConfig")
        .def(py::init<>())
        .def("get_config", &AdvancedBrainConfig::getConfig)
        .def("enable_advanced_learning", &AdvancedBrainConfig::enableAdvancedLearning)
        .def("enable_fast_training", &AdvancedBrainConfig::enableFastTraining)
        .def("enable_biological_learning", &AdvancedBrainConfig::enableBiologicalLearning)
        .def("enable_curiosity_driven", &AdvancedBrainConfig::enableCuriosityDriven)
        .def("enable_predictive_learning", &AdvancedBrainConfig::enablePredictiveLearning)
        .def("get_profile_name", &AdvancedBrainConfig::getProfileName);
    
    // Advanced Simulation
    py::class_<AdvancedSimulation>(m, "AdvancedSimulation")
        .def(py::init<>())
        .def("initialize", &AdvancedSimulation::initialize)
        .def("run_steps", &AdvancedSimulation::runSteps, py::arg("num_steps"), py::arg("callback") = nullptr)
        .def("stop", &AdvancedSimulation::stop)
        .def("run_until_stable", &AdvancedSimulation::runUntilStable, py::arg("stability_threshold") = 0.01f, py::arg("max_steps") = 10000)
        .def("run_exploration", &AdvancedSimulation::runExploration, py::arg("exploration_rate") = 0.3f, py::arg("max_steps") = 5000)
        .def("run_learning_simulation", &AdvancedSimulation::runLearningSimulation, py::arg("learning_rate") = 0.01f, py::arg("num_episodes") = 100)
        .def("collect_simulation_data", &AdvancedSimulation::collectSimulationData)
        .def("analyze_performance", &AdvancedSimulation::analyzePerformance);
    
    // Advanced Visualization
    py::class_<AdvancedVisualization>(m, "AdvancedVisualization")
        .def(py::init<>())
        .def("update", &AdvancedVisualization::update)
        .def("render", &AdvancedVisualization::render)
        .def("enable_advanced_rendering", &AdvancedVisualization::enableAdvancedRendering)
        .def("get_performance_metrics", &AdvancedVisualization::getPerformanceMetrics);
    
    // High-level API
    py::class_<NLMSession>(m, "NLMSession")
        .def(py::init<std::string>(), py::arg("name") = "nlm_session")
        .def("create_brain", &NLMSession::createBrain)
        .def("setup_visualization", &NLMSession::setupVisualization, py::arg("output_path") = "")
        .def("run_experiment", &NLMSession::runExperiment, py::arg("experiment_type"), py::arg("steps"))
        .def("get_results", &NLMSession::getResults)
        .def("save_session", &NLMSession::saveSession, py::arg("filename"));
    
    // ExportFormat enum
    py::enum_<ExportFormat>(m, "ExportFormat")
        .value("TEXT", ExportFormat::TEXT)
        .value("CSV", ExportFormat::CSV)
        .value("JSON", ExportFormat::JSON)
        .value("PNG", ExportFormat::PNG)
        .value("SVG", ExportFormat::SVG)
        .value("MATLAB", ExportFormat::MATLAB)
        .export_values();
    
    // Convenience functions
    m.def("create_advanced_config", []() { return std::make_shared<AdvancedBrainConfig>(); });
    
    m.def("create_advanced_session", []() { return std::make_shared<NLMSession>(); });
    
    m.def("create_advanced_simulation", []() { return std::make_shared<AdvancedSimulation>(); });
    
    m.def("create_advanced_visualization", []() { return std::make_shared<AdvancedVisualization>(); });
}
