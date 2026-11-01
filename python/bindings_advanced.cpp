// Phase 6: Comprehensive Python bindings for NLM
// Provides high-level APIs for brain simulation, training, and control

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// High-level Python API
class PythonBrain {
public:
    PythonBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    void initialize() {
        brain_->initialize();
    }
    
    void step(uint64_t step, double time = 0.0) {
        brain_->step(step, time);
    }
    
    void step(uint64_t step) {
        brain_->step(step);
    }
    
    float getTotalReward() const {
        // Get accumulated reward from agent's neuromodulation
        if (auto* agent = getAgent()) {
            return agent->getNeuromodulationLevel();
        }
        return 0.0f;
    }
    
    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    size_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    size_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    float getDopamineLevel() const {
        if (auto* dopamine = brain_->getDopamine()) {
            return dopamine->getLevel();
        }
        return 0.0f;
    }
    
    float getCuriosityLevel() const {
        if (auto* curiosity = brain_->getCuriosity()) {
            return curiosity->getLevel();
        }
        return 0.0f;
    }
    
    float getNoveltyLevel() const {
        if (auto* novelty = brain_->getNovelty()) {
            return novelty->getLevel();
        }
        return 0.0f;
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    std::vector<float> getWorkingMemory() const {
        auto* wm = brain_->getWorkingMemory();
        if (!wm) return {};
        // Return active traces
        auto neurons = wm->getMemoryNeurons();
        auto activations = wm->getMemoryActivations();
        return activations;
    }
    
    // Simulation control methods
    bool save(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    void reset() {
        brain_->reset();
    }
    
    std::string getStatus() const {
        std::ostringstream oss;
        oss << "Neurons: " << brain_->getTotalNeuronCount()
            << " Synapses: " << brain_->getTotalSynapseCount()
            << " Total Spikes: " << brain_->getTotalSpikeCount()
            << " Avg Firing Rate: " << brain_->getAverageFiringRate()
            << " Developmental Stage: " << static_cast<int>(brain_->getDevelopmentalStage());
        return oss.str();
    }
    
    // Integration with agent brain for world interaction
    void connectAgent(std::shared_ptr<AgentBrain> agent) {
        agent_ = agent;
    }
    
    AgentBrain* getAgent() {
        return agent_.get();
    }
    
    // Batch processing for efficiency
    void batchStep(uint64_t startStep, uint64_t endStep, double timestep) {
        for (uint64_t step = startStep; step < endStep; ++step) {
            brain_->step(step, step * timestep);
        }
    }
    
    // Performance monitoring
    struct PerformanceMetrics {
        double wallClockTime;
        float avgFiringRate;
        float rewardRate;
        size_t spikeEfficiency;
        float memoryUtilization;
    };
    
    PerformanceMetrics getPerformanceMetrics(double startTime) const {
        PerformanceMetrics metrics;
        metrics.avgFiringRate = brain_->getAverageFiringRate();
        metrics.rewardRate = getTotalReward() / (startTime > 0 ? startTime : 1.0);
        metrics.spikeEfficiency = getTotalSpikeCount() / std::max<size_t>(1, getTotalNeuronCount());
        
        auto* wm = brain_->getWorkingMemory();
        if (wm) {
            metrics.memoryUtilization = static_cast<float>(wm->getActiveTraces()) / wm->getCapacity();
        } else {
            metrics.memoryUtilization = 0.0f;
        }
        
        // Wall clock time would need to be tracked externally
        metrics.wallClockTime = 0.0; // Placeholder
        
        return metrics;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
};

// World simulation helper
class PythonWorld {
public:
    PythonWorld() : world_(std::make_shared<SimpleWorld>()) {}
    
    void configure(float width, float height, size_t visionWidth = 16, size_t visionHeight = 16) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    void reset() {
        world_->reset();
    }
    
    void update(double timestep) {
        world_->update(timestep);
    }
    
    SensoryPercept getSensoryPercept() const {
        return world_->getSensoryPercept();
    }
    
    AgentBody getAgentBody() const {
        return world_->getAgentBody();
    }
    
    ActionResult applyAction(const Action& action, double time) {
        return world_->applyMotorCommand(action.getType(), time);
    }
    
    std::string getStatus() const {
        std::ostringstream oss;
        const auto& body = world_->getAgentBody();
        oss << "Position: (" << body.x << ", " << body.y << ") "
            << "Orientation: " << body.orientation << " "
            << "Energy: " << body.energy << " Health: " << body.health;
        return oss.str();
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Experiment runner helper
class PythonExperiment {
public:
    PythonExperiment() : experiment_(std::make_unique<Phase6IntegratedExperiment>()) {}
    
    bool runPhase6Demo() {
        // Create experiment and run with default config
        Phase6Demo demo;
        // The demo will print to stdout, we'll capture it
        try {
            demo.main(0, nullptr);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool runPhase6IntegrationTest() {
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment_->run(config);
        
        // Print results for user to see
        std::cout << "=== Phase 6 Integration Results ===" << std::endl;
        std::cout << "Total Reward: " << result.totalReward << std::endl;
        std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
        std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << "Working Memory Integrated: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO") << std::endl;
        std::cout << "Episodic Memory Integrated: " << (result.memoryEpisodicMemoryIntegrated ? "YES" : "NO") << std::endl;
        std::cout << "Neuromodulation Integrated: " << (result.neuromodulationIntegrated ? "YES" : "NO") << std::endl;
        
        return result.totalReward > 0; // Success if we got reward
    }
    
private:
    std::unique_ptr<Phase6IntegratedExperiment> experiment_;
};

// High-level training utilities
class PythonTraining {
public:
    // Training episode with automatic data collection
    struct TrainingEpisode {
        uint64_t steps;
        float totalReward;
        float avgFiringRate;
        float finalEnergy;
        float finalNovelty;
        std::vector<float> actionDistribution;
    };
    
    std::vector<TrainingEpisode> trainBrain(
        std::shared_ptr<PythonBrain> brain,
        std::shared_ptr<PythonWorld> world,
        uint64_t numEpisodes,
        uint64_t stepsPerEpisode,
        double timestep = 0.001,
        bool enableExploration = true
    ) {
        std::vector<TrainingEpisode> episodes;
        
        for (uint64_t episode = 0; episode < numEpisodes; ++episode) {
            TrainingEpisode ep;
            ep.steps = 0;
            ep.totalReward = 0.0f;
            ep.avgFiringRate = 0.0f;
            ep.finalEnergy = 0.0f;
            ep.finalNovelty = 0.0f;
            ep.actionDistribution.assign(7, 0.0f);  // Wait, MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact
            
            // Reset for new episode
            world->reset();
            brain->reset();
            
            for (uint64_t step = 0; step < stepsPerEpisode; ++step) {
                // Get sensory input
                SensoryPercept percept = world->getSensoryPercept();
                
                // Process sensory input
                if (brain->getAgent()) {
                    brain->getAgent()->processSensoryInput(percept);
                }
                
                // Simulate brain
                brain->step(step, step * timestep);
                
                // Get action
                MotorCommand action = MotorCommand::Wait;
                if (brain->getAgent()) {
                    action = brain->getAgent()->decodeMotorCommand();
                }
                
                // Apply action to world
                ActionResult result = world->applyAction(Action(action), step * timestep);
                
                // Apply reward modulation
                if (brain->getAgent()) {
                    brain->getAgent()->applyRewardModulation(result.reward, 0.0f);
                }
                
                // Update world
                world->update(timestep);
                
                // Update metrics
                ep.steps++;
                ep.totalReward += result.reward;
                ep.finalEnergy = world->getAgentBody().energy;
                
                // Track action distribution
                if (action == MotorCommand::MoveForward) ep.actionDistribution[1]++;
                else if (action == MotorCommand::MoveBackward) ep.actionDistribution[2]++;
                else if (action == MotorCommand::TurnLeft) ep.actionDistribution[3]++;
                else if (action == MotorCommand::TurnRight) ep.actionDistribution[4]++;
                else if (action == MotorCommand::LookLeft) ep.actionDistribution[5]++;
                else if (action == MotorCommand::LookRight) ep.actionDistribution[6]++;
                else if (action == MotorCommand::Interact) ep.actionDistribution[7]++;
            }
            
            // Calculate averages
            if (ep.steps > 0) {
                ep.avgFiringRate = brain->getAverageFiringRate();
                ep.finalNovelty = brain->getNoveltyLevel();
                
                // Normalize action distribution
                float totalActions = std::accumulate(ep.actionDistribution.begin(), ep.actionDistribution.end(), 0.0f);
                if (totalActions > 0) {
                    for (float& count : ep.actionDistribution) {
                        count /= totalActions;
                    }
                }
            }
            
            episodes.push_back(ep);
            
            std::cout << "Episode " << episode << "/" << numEpisodes 
                      << ": Reward=" << ep.totalReward 
                      << " Energy=" << ep.finalEnergy 
                      << " Novelty=" << ep.finalNovelty << std::endl;
        }
        
        return episodes;
    }
    
    // Utility function to convert between Python and C++ types
    static py::dict episodeToDict(const TrainingEpisode& episode) {
        py::dict d;
        d["steps"] = episode.steps;
        d["total_reward"] = episode.totalReward;
        d["avg_firing_rate"] = episode.avgFiringRate;
        d["final_energy"] = episode.finalEnergy;
        d["final_novelty"] = episode.finalNovelty;
        d["action_distribution"] = episode.actionDistribution;
        return d;
    }
};

// Module initialization
PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        Advanced NLM Python Bindings
        ----------------------------
        
        This module provides enhanced Python APIs for the NLM (Neural Learning Machine) framework.
        It includes high-level interfaces for brain simulation, world interaction, and training.
        
        Key features:
        - High-level Brain and World simulation APIs
        - Training utilities for reinforcement learning
        - Performance monitoring and metrics
        - Integration with existing NLM systems
        
        The basic NLM module provides core functionality; this extension adds convenience methods
        for easier use in Python applications.
    )pbdoc";
    
    // Enum definitions
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("LookLeft", ActionType::LookLeft)
        .value("LookRight", ActionType::LookRight)
        .value("Interact", ActionType::Interact)
        .value("Wait", ActionType::Wait)
        .export_values();
    
    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // High-level classes
    py::class_<PythonBrain>(m, "PythonBrain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &PythonBrain::initialize)
        .def("step", py::overload_cast<uint64_t, double>(&PythonBrain::step), py::arg("step"), py::arg("time") = 0.0)
        .def("step", py::overload_cast<uint64_t>(&PythonBrain::step), py::arg("step"))
        .def("get_total_reward", &PythonBrain::getTotalReward)
        .def("get_average_firing_rate", &PythonBrain::getAverageFiringRate)
        .def("get_total_spike_count", &PythonBrain::getTotalSpikeCount)
        .def("get_total_neuron_count", &PythonBrain::getTotalNeuronCount)
        .def("get_total_synapse_count", &PythonBrain::getTotalSynapseCount)
        .def("get_dopamine_level", &PythonBrain::getDopamineLevel)
        .def("get_curiosity_level", &PythonBrain::getCuriosityLevel)
        .def("get_novelty_level", &PythonBrain::getNoveltyLevel)
        .def("get_developmental_stage", &PythonBrain::getDevelopmentalStage)
        .def("get_working_memory", &PythonBrain::getWorkingMemory)
        .def("save", &PythonBrain::save, py::arg("filepath"))
        .def("load", &PythonBrain::load, py::arg("filepath"))
        .def("reset", &PythonBrain::reset)
        .def("get_status", &PythonBrain::getStatus)
        .def("batch_step", &PythonBrain::batchStep, py::arg("start_step"), py::arg("end_step"), py::arg("timestep"))
        .def("get_performance_metrics", &PythonBrain::getPerformanceMetrics, py::arg("start_time") = 0.0);
    
    // Agent brain interface
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand)
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predicted_reward") = 0.0f)
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel)
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel)
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel)
        .def("get_prediction_error", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity, py::arg("enable"));
    
    // World simulation
    py::class_<PythonWorld>(m, "PythonWorld")
        .def(py::init<>())
        .def("configure", &PythonWorld::configure, py::arg("width"), py::arg("height"), py::arg("vision_width") = 16, py::arg("vision_height") = 16)
        .def("reset", &PythonWorld::reset)
        .def("update", &PythonWorld::update, py::arg("timestep"))
        .def("get_sensory_percept", &PythonWorld::getSensoryPercept)
        .def("get_agent_body", &PythonWorld::getAgentBody)
        .def("get_status", &PythonWorld::getStatus);
    
    // Experiment utilities
    py::class_<PythonExperiment>(m, "PythonExperiment")
        .def(py::init<>())
        .def("run_phase6_demo", &PythonExperiment::runPhase6Demo)
        .def("run_phase6_integration_test", &PythonExperiment::runPhase6IntegrationTest);
    
    // Training utilities
    py::class_<PythonTraining>(m, "PythonTraining")
        .def(py::init<>())
        .def("train_brain", &PythonTraining::train_brain, py::arg("brain"), py::arg("world"), py::arg("num_episodes"), 
                       py::arg("steps_per_episode"), py::arg("timestep") = 0.001, py::arg("enable_exploration") = true);
    
    // Convenience factory functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<PythonBrain> {
        return std::make_shared<PythonBrain>(config);
    }, py::arg("config"), "Create a Python brain with configuration");
    
    m.def("create_world", []() -> std::shared_ptr<PythonWorld> {
        return std::make_shared<PythonWorld>();
    }, "Create a new Python world");
    
    m.def("create_agent", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");
    
    // Constants
    m.attr("DEFAULT_TIMESTEP") = 0.001;
    m.attr("DEFAULT_NEURON_COUNT") = 500;
    m.attr("DEFAULT_MAX_STEPS") = 2000;
    
    // Constants for reward and neuromodulation
    m.attr("RESOURCE_REWARD") = 10.0f;
    m.attr("HAZARD_COST") = -15.0f;
    m.attr("MOVEMENT_COST") = -0.01f;
    m.attr("TURN_COST") = -0.005f;
}

} // namespace nlm
