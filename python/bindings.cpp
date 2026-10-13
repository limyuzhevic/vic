// Advanced NLM Python Bindings
// Provides enhanced Python APIs for the Neural Learning Machine
// Includes high-level abstractions, convenience methods, and advanced features

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"

namespace py = pybind11;
namespace nlm {
namespace advanced {

// High-level brain abstraction
class PythonBrain {
public:
    std::shared_ptr<Brain> brain;
    
    PythonBrain(std::shared_ptr<Brain> b) : brain(b) {}
    
    // Configuration convenience methods
    void configure(int neuronCount, int regionCount, float connectionProb = 0.1, uint64_t seed = 42) {
        auto config = brain->getConfig();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProb);
        config->set("random_seed", seed);
        brain->setConfig(config);
    }
    
    // Simulation convenience methods
    void runSimulation(int steps, double timestep = 0.001, bool verbose = false) {
        brain->logStatus();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << 
                         << " | Spikes: " << brain->getTotalSpikeCount() << 
                         << " | Neurons active: " << brain->getActiveNeuronCount() << std::endl;
            }
        }
        
        brain->logStatus();
    }
    
    // Sensory input convenience methods
    void injectSensoryVector(const std::vector<float>& data, NeuronType type = NeuronType::Sensory) {
        SensoryInput input;
        input.setData(data);
        brain->receiveSensoryInput(input);
    }
    
    void injectSensoryImage(const std::vector<std::vector<float>>& image, 
                          size_t channels = 1) {
        // Create vision input from image data
        Vision vision(image.size(), image[0].size(), channels);
        // Simplified image processing for demonstration
        brain->receiveSensoryInput(vision);
    }
    
    // Action convenience methods
    Action createAction(MotorCommand cmd, const std::vector<float>& parameters = {}) {
        Action action(cmd, parameters);
        return action;
    }
    
    // Learning convenience methods
    float getLearningProgress() const {
        // Calculate learning progress based on spike statistics
        auto spikeCount = brain->getTotalSpikeCount();
        auto firingRate = brain->getAverageFiringRate();
        return std::min(1.0f, firingRate / 10.0f); // Normalize to 0-1
    }
    
    // State management
    void saveState(const std::string& filepath) {
        brain->save(filepath);
    }
    
    bool loadState(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Utility methods
    std::string getStateSummary() const {
        std::stringstream ss;
        ss << "Brain State Summary:" << std::endl;
        ss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        return ss.str();
    }
    
    // Comparison operators
    bool operator==(const PythonBrain& other) const {
        return brain->getTotalNeuronCount() == other.brain->getTotalNeuronCount() &&
               brain->getTotalSynapseCount() == other.brain->getTotalSynapseCount();
    }
    
    bool operator!=(const PythonBrain& other) const {
        return !(*this == other);
    }
};

// Advanced configuration builder
class ConfigBuilder {
public:
    std::shared_ptr<Config> config;
    
    ConfigBuilder() : config(std::make_shared<Config>()) {}
    
    // Brain configuration
    ConfigBuilder& brain(int neuronCount, int regionCount = 1, float connectionProb = 0.1) {
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProb);
        return *this;
    }
    
    ConfigBuilder& randomSeed(uint64_t seed) {
        config->set("random_seed", seed);
        return *this;
    }
    
    ConfigBuilder& simulation(double timestep = 0.001) {
        config->set("simulation_timestep", timestep);
        return *this;
    }
    
    // Plasticity configuration
    ConfigBuilder& plasticity(float stdpLtp = 0.02f, float stdpLtd = 0.015f, 
                              float hebbian = 0.01f, float synaptogenesis = 0.0001f,
                              float pruning = 0.00001f) {
        config->set("stdp_ltp_weight", stdpLtp);
        config->set("stdp_ltd_weight", stdpLtd);
        config->set("hebbian_weight", hebbian);
        config->set("synaptogenesis_rate", synaptogenesis);
        config->set("pruning_rate", pruning);
        return *this;
    }
    
    // Neuromodulation configuration
    ConfigBuilder& neuromodulation(bool dopamine = true, bool curiosity = true,
                                  bool novelty = true, bool predictionError = true) {
        config->set("dopamine_enabled", dopamine);
        config->set("curiosity_enabled", curiosity);
        config->set("novelty_enabled", novelty);
        config->set("prediction_error_enabled", predictionError);
        return *this;
    }
    
    // Memory configuration
    ConfigBuilder& memory(bool working = true, bool episodic = true,
                         bool semantic = true, bool procedural = true,
                         bool associative = true) {
        config->set("working_memory_enabled", working);
        config->set("episodic_memory_enabled", episodic);
        config->set("semantic_memory_enabled", semantic);
        config->set("procedural_memory_enabled", procedural);
        config->set("associative_memory_enabled", associative);
        return *this;
    }
    
    // Environment configuration
    ConfigBuilder& environment(int width = 20, int height = 20,
                               int visionWidth = 16, int visionHeight = 16) {
        config->set("environment_width", width);
        config->set("environment_height", height);
        config->set("vision_width", visionWidth);
        config->set("vision_height", visionHeight);
        return *this;
    }
    
    // Cognitive configuration
    ConfigBuilder& cognition(bool prediction = true, bool attention = true,
                            bool planning = true, bool selfModel = true,
                            bool socialLearning = true) {
        config->set("prediction_enabled", prediction);
        config->set("attention_enabled", attention);
        config->set("planning_enabled", planning);
        config->set("self_model_enabled", selfModel);
        config->set("social_learning_enabled", socialLearning);
        return *this;
    }
    
    // Development configuration
    ConfigBuilder& development(bool enable = true, int stages = 5) {
        config->set("development_enabled", enable);
        config->set("developmental_stages", stages);
        return *this;
    }
    
    // Build and return the configuration
    std::shared_ptr<Config> build() {
        return config;
    }
    
    // Build and create a brain
    std::shared_ptr<PythonBrain> buildBrain() {
        auto brain = std::make_shared<PythonBrain>(
            std::make_shared<Brain>(config)
        );
        return brain;
    }
};

// Simulation runner for complex experiments
class ExperimentRunner {
public:
    std::vector<std::shared_ptr<PythonBrain>> brains;
    std::shared_ptr<SimpleWorld> world;
    
    ExperimentRunner() {
        world = std::make_shared<SimpleWorld>();
    }
    
    // Add a brain to the experiment
    void addBrain(std::shared_ptr<PythonBrain> brain) {
        brains.push_back(brain);
    }
    
    // Run a multi-brain experiment
    void runMultiBrainExperiment(int steps, bool verbose = false) {
        std::cout << "Starting multi-brain experiment with " << brains.size() << " brains" << std::endl;
        
        for (int step = 0; step < steps; ++step) {
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << std::endl;
            }
            
            // Process each brain
            for (auto& brain : brains) {
                // Simulate brain step
                brain->brain->step(step, step * 0.01);
                
                // Simple agent behavior
                auto percept = world->getSensoryPercept();
                brain->brain->receiveSensoryInput(percept);
                
                auto action = brain->createAction(MotorCommand::Wait);
                // Apply action to world (simplified)
            }
            
            // Update world
            world->update(0.01);
        }
        
        std::cout << "Multi-brain experiment completed" << std::endl;
    }
    
    // Run learning experiment comparing different configurations
    void runLearningComparison(const std::vector<std::shared_ptr<Config>>& configs, 
                              int steps, const std::string& outputFile) {
        std::cout << "Running learning comparison with " << configs.size() << " configurations" << std::endl;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            std::cout << "Testing configuration " << i + 1 << std::endl;
            
            auto brain = std::make_shared<Brain>(configs[i]);
            auto pythonBrain = std::make_shared<PythonBrain>(brain);
            
            // Run learning simulation
            pythonBrain->runSimulation(steps, 0.001, false);
            
            // Record results
            double learningProgress = pythonBrain->getLearningProgress();
            std::cout << "  Learning progress: " << learningProgress << std::endl;
        }
        
        std::cout << "Learning comparison completed" << std::endl;
    }
    
    // Run agent interaction experiment
    void runAgentInteractionExperiment(int numAgents, int steps) {
        std::cout << "Running agent interaction experiment with " << numAgents << " agents" << std::endl;
        
        // Create multiple agents
        std::vector<std::shared_ptr<AgentBrain>> agents;
        for (int i = 0; i < numAgents; ++i) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 200);
            config->set("region_count", 1);
            config->set("random_seed", 42 + i);
            
            auto brain = std::make_shared<Brain>(config);
            auto agent = std::make_shared<AgentBrain>(brain);
            
            world->configure(20, 20, 16, 16);
            agent->initialize(*world);
            
            agents.push_back(agent);
        }
        
        // Run interaction
        for (int step = 0; step < steps; ++step) {
            // Each agent processes its own percept and takes action
            for (auto& agent : agents) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                auto action = agent->decodeMotorCommand();
                auto result = world->applyMotorCommand(action, world->getSimulationTime());
                
                agent->applyRewardModulation(result.reward, 0.0f);
                agent->updateDevelopment(0.01);
            }
            
            world->update(0.01);
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << std::endl;
            }
        }
        
        std::cout << "Agent interaction experiment completed" << std::endl;
    }
};

// Data recording utilities
class DataRecorder {
public:
    struct TrialData {
        int trialId;
        double startTime;
        double endTime;
        size_t steps;
        std::vector<float> metrics;
        std::vector<std::vector<float>> neuralActivity;
        std::string outcome;
    };
    
    std::vector<TrialData> recordedTrials;
    
    // Record a trial
    void recordTrial(const TrialData& trial) {
        recordedTrials.push_back(trial);
    }
    
    // Export data to CSV
    void exportToCSV(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "trial_id,start_time,end_time,steps,metrics,neural_activity,outcome\n";
        
        for (const auto& trial : recordedTrials) {
            file << trial.trialId << "," << trial.startTime << "," << trial.endTime << "," 
                 << trial.steps << ",";
            
            // Write metrics as comma-separated values
            for (size_t i = 0; i < trial.metrics.size(); ++i) {
                file << trial.metrics[i];
                if (i < trial.metrics.size() - 1) file << ";";
            }
            
            file << ",";
            
            // Write neural activity (simplified)
            file << trial.neuralActivity.size() << "x" << 
                 (trial.neuralActivity.empty() ? 0 : trial.neuralActivity[0].size()) << ";";
            
            file << trial.outcome << "\n";
        }
        
        file.close();
    }
    
    // Export data to JSON
    void exportToJSON(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        json data;
        
        for (size_t i = 0; i < recordedTrials.size(); ++i) {
            data[i]["trial_id"] = recordedTrials[i].trialId;
            data[i]["start_time"] = recordedTrials[i].startTime;
            data[i]["end_time"] = recordedTrials[i].endTime;
            data[i]["steps"] = recordedTrials[i].steps;
            data[i]["metrics"] = recordedTrials[i].metrics;
            data[i]["outcome"] = recordedTrials[i].outcome;
        }
        
        file << data.dump(2);
        file.close();
    }
    
    // Generate summary report
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== NLM Data Summary ===" << std::endl;
        ss << "Total trials recorded: " << recordedTrials.size() << std::endl;
        
        if (recordedTrials.empty()) return ss.str();
        
        double totalTime = 0.0;
        size_t totalSteps = 0;
        
        for (const auto& trial : recordedTrials) {
            totalTime += (trial.endTime - trial.startTime);
            totalSteps += trial.steps;
        }
        
        ss << "Average trial time: " << (totalTime / recordedTrials.size()) << " seconds" << std::endl;
        ss << "Average steps per trial: " << (totalSteps / recordedTrials.size()) << std::endl;
        
        return ss.str();
    }
};

// Callback system for real-time processing
class CallbackSystem {
public:
    using DataCallback = std::function<void(const std::vector<float>&)'>;
    using EventCallback = std::function<void(const std::string& event, const std::map<std::string, std::string>& data)>;
    
    std::vector<DataCallback> dataCallbacks;
    std::vector<EventCallback> eventCallbacks;
    
    // Add data callback
    void addDataCallback(const DataCallback& callback) {
        dataCallbacks.push_back(callback);
    }
    
    // Add event callback
    void addEventCallback(const EventCallback& callback) {
        eventCallbacks.push_back(callback);
    }
    
    // Trigger all data callbacks
    void triggerDataCallbacks(const std::vector<float>& data) {
        for (const auto& callback : dataCallbacks) {
            callback(data);
        }
    }
    
    // Trigger all event callbacks
    void triggerEventCallbacks(const std::string& event, const std::map<std::string, std::string>& data) {
        for (const auto& callback : eventCallbacks) {
            callback(event, data);
        }
    }
    
    // Process data through callbacks
    std::vector<float> processData(const std::vector<float>& input) {
        std::vector<float> processed = input;
        
        for (const auto& callback : dataCallbacks) {
            callback(processed);
        }
        
        return processed;
    }
};

// Visualization utilities
namespace visualization {
    
    // Create simple text-based visualization
    std::string createTextVisualization(const Brain& brain, int width = 50, int height = 20) {
        std::stringstream ss;
        ss << "=== Brain Activity Visualization ===" << std::endl;
        ss << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
        ss << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
        ss << "Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
        
        // Create a simple bar chart
        int activeCount = brain.getActiveNeuronCount();
        int totalCount = brain.getTotalNeuronCount();
        float ratio = static_cast<float>(activeCount) / totalCount;
        
        ss << std::endl;
        ss << "Activity Level:" << std::endl;
        ss << "[";
        int barWidth = static_cast<int>(width * ratio);
        for (int i = 0; i < width; ++i) {
            if (i < barWidth) {
                ss << "#";
            } else {
                ss << "-";
            }
        }
        ss << "] " << std::fixed << std::setprecision(2) << ratio * 100 << "%" << std::endl;
        
        return ss.str();
    }
    
    // Create simple JSON visualization data
    json createJSONVisualization(const Brain& brain) {
        json viz;
        
        viz["total_neurons"] = brain.getTotalNeuronCount();
        viz["active_neurons"] = brain.getActiveNeuronCount();
        viz["firing_neurons"] = brain.getFiringNeuronCount();
        viz["average_firing_rate"] = brain.getAverageFiringRate();
        viz["total_spikes"] = brain.getTotalSpikeCount();
        viz["excitatory_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        viz["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        // Add region information
        auto regionIds = brain.getRegionIds();
        for (size_t i = 0; i < regionIds.size(); ++i) {
            auto region = brain.getRegion(regionIds[i]);
            if (region) {
                json regionData;
                regionData["id"] = regionIds[i].value();
                regionData["neuron_count"] = region->getNeuronCount();
                regionData["synapse_count"] = region->getSynapseCount();
                viz["regions"][i] = regionData;
            }
        }
        
        return viz;
    }
    
} // namespace visualization

} // namespace advanced
} // namespace nlm

// Python module bindings for advanced functionality
PYBIND11_MODULE(advanced_nlm, m) {
    m.doc() = "Advanced NLM Python Bindings - High-level abstractions and convenience methods for NLM simulations";
    
    // Bind PythonBrain class
    py::class_<nlm::advanced::PythonBrain>(m, "PythonBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("configure", &nlm::advanced::PythonBrain::configure,
             py::arg("neuronCount"), py::arg("regionCount") = 1,
             py::arg("connectionProb") = 0.1, py::arg("seed") = 42)
        .def("runSimulation", &nlm::advanced::PythonBrain::runSimulation,
             py::arg("steps"), py::arg("timestep") = 0.001, py::arg("verbose") = false)
        .def("injectSensoryVector", &nlm::advanced::PythonBrain::injectSensoryVector,
             py::arg("data"), py::arg("type") = NeuronType::Sensory)
        .def("injectSensoryImage", &nlm::advanced::PythonBrain::injectSensoryImage,
             py::arg("image"), py::arg("channels") = 1)
        .def("createAction", &nlm::advanced::PythonBrain::createAction,
             py::arg("cmd"), py::arg("parameters") = std::vector<float>())
        .def("getLearningProgress", &nlm::advanced::PythonBrain::getLearningProgress)
        .def("saveState", &nlm::advanced::PythonBrain::saveState, py::arg("filepath"))
        .def("loadState", &nlm::advanced::PythonBrain::loadState, py::arg("filepath"))
        .def("getStateSummary", &nlm::advanced::PythonBrain::getStateSummary)
        .def("__eq__", &nlm::advanced::PythonBrain::operator==)
        .def("__ne__", &nlm::advanced::PythonBrain::operator!=);
    
    // Bind ConfigBuilder class
    py::class_<nlm::advanced::ConfigBuilder>(m, "ConfigBuilder")
        .def(py::init<>())
        .def("brain", &nlm::advanced::ConfigBuilder::brain,
             py::arg("neuronCount"), py::arg("regionCount") = 1,
             py::arg("connectionProb") = 0.1)
        .def("randomSeed", &nlm::advanced::ConfigBuilder::randomSeed, py::arg("seed"))
        .def("simulation", &nlm::advanced::ConfigBuilder::simulation, py::arg("timestep") = 0.001)
        .def("plasticity", &nlm::advanced::ConfigBuilder::plasticity,
             py::arg("stdpLtp") = 0.02f, py::arg("stdpLtd") = 0.015f,
             py::arg("hebbian") = 0.01f, py::arg("synaptogenesis") = 0.0001f,
             py::arg("pruning") = 0.00001f)
        .def("neuromodulation", &nlm::advanced::ConfigBuilder::neuromodulation,
             py::arg("dopamine") = true, py::arg("curiosity") = true,
             py::arg("novelty") = true, py::arg("predictionError") = true)
        .def("memory", &nlm::advanced::ConfigBuilder::memory,
             py::arg("working") = true, py::arg("episodic") = true,
             py::arg("semantic") = true, py::arg("procedural") = true,
             py::arg("associative") = true)
        .def("environment", &nlm::advanced::ConfigBuilder::environment,
             py::arg("width") = 20, py::arg("height") = 20,
             py::arg("visionWidth") = 16, py::arg("visionHeight") = 16)
        .def("cognition", &nlm::advanced::ConfigBuilder::cognition,
             py::arg("prediction") = true, py::arg("attention") = true,
             py::arg("planning") = true, py::arg("selfModel") = true,
             py::arg("socialLearning") = true)
        .def("development", &nlm::advanced::ConfigBuilder::development,
             py::arg("enable") = true, py::arg("stages") = 5)
        .def("build", &nlm::advanced::ConfigBuilder::build)
        .def("buildBrain", &nlm::advanced::ConfigBuilder::buildBrain);
    
    // Bind ExperimentRunner class
    py::class_<nlm::advanced::ExperimentRunner>(m, "ExperimentRunner")
        .def(py::init<>())
        .def("addBrain", &nlm::advanced::ExperimentRunner::addBrain, py::arg("brain"))
        .def("runMultiBrainExperiment", &nlm::advanced::ExperimentRunner::runMultiBrainExperiment,
             py::arg("steps"), py::arg("verbose") = false)
        .def("runLearningComparison", &nlm::advanced::ExperimentRunner::runLearningComparison,
             py::arg("configs"), py::arg("steps"), py::arg("outputFile"))
        .def("runAgentInteractionExperiment", &nlm::advanced::ExperimentRunner::runAgentInteractionExperiment,
             py::arg("numAgents"), py::arg("steps"));
    
    // Bind DataRecorder class
    py::class_<nlm::advanced::DataRecorder>(m, "DataRecorder")
        .def(py::init<>())
        .def("recordTrial", &nlm::advanced::DataRecorder::recordTrial, py::arg("trial"))
        .def("exportToCSV", &nlm::advanced::DataRecorder::exportToCSV, py::arg("filename"))
        .def("exportToJSON", &nlm::advanced::DataRecorder::exportToJSON, py::arg("filename"))
        .def("generateReport", &nlm::advanced::DataRecorder::generateReport);
    
    // Bind CallbackSystem class
    py::class_<nlm::advanced::CallbackSystem>(m, "CallbackSystem")
        .def(py::init<>())
        .def("addDataCallback", &nlm::advanced::CallbackSystem::addDataCallback, py::arg("callback"))
        .def("addEventCallback", &nlm::advanced::CallbackSystem::addEventCallback, py::arg("callback"))
        .def("processData", &nlm::advanced::CallbackSystem::processData, py::arg("input"));
    
    // Bind visualization namespace
    m.def("createTextVisualization", &nlm::advanced::visualization::createTextVisualization,
          py::arg("brain"), py::arg("width") = 50, py::arg("height") = 20);
    
    m.def("createJSONVisualization", &nlm::advanced::visualization::createJSONVisualization,
          py::arg("brain"));
    
    // Add convenience function to create ConfigBuilder
    m.def("createConfigBuilder", []() { return nlm::advanced::ConfigBuilder(); },
          "Create a ConfigBuilder for convenient configuration");
    
    // Add convenience function to create ExperimentRunner
    m.def("createExperimentRunner", []() { return nlm::advanced::ExperimentRunner(); },
          "Create an ExperimentRunner for running experiments");
    
    // Add convenience function to create DataRecorder
    m.def("createDataRecorder", []() { return nlm::advanced::DataRecorder(); },
          "Create a DataRecorder for recording experiment data");
    
    // Add convenience function to create CallbackSystem
    m.def("createCallbackSystem", []() { return nlm::advanced::CallbackSystem(); },
          "Create a CallbackSystem for event processing");
}
