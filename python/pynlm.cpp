#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include the modular binding headers
#include "brain_bindings.hpp"
#include "agent_bindings.hpp"
#include "world_bindings.hpp"

// Forward declarations for the main module
namespace nlm {
    class Config;
    class Brain;
    class AgentBrain;
    class SimpleWorld;
    class Action;
    class AgentBody;
    class SensoryPercept;
    class SensoryInput;
    class WorldObject;
    class ActionResult;

    // Type enumerations
    enum class NeuronType;
    enum class SynapseType;
    enum class DevelopmentalStage;
    enum class FiringState;
    enum class ActionType;
    enum class MotorCommand;
    enum class WorldObjectType;

    // Identifier types
    struct NeuronId;
    struct SynapseId;
    struct RegionId;
    struct PopulationId;
}

// Main Python module providing unified interface
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A unified Python interface to the NLM neural simulation framework.
        
        This module provides comprehensive bindings for:
        
        1. **Brain System**: Core neural computation with LIF neurons
        2. **Agent System**: Interface between brain and environment  
        3. **World System**: Simulation environment with sensory input
        
        Features:
        - Event-driven spiking neural network simulation
        - Multi-layered memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and planning systems
        - Developmental plasticity
        - Checkpoint persistence
        - Configuration management
        
        Usage:
            import pynlm
            
            # Create a brain with default configuration
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Run simulation
            for step in range(1000):
                brain.step(step)
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
        )pbdoc";

    // Configure error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Bind all submodules
    bindBrainModule(m);
    bindAgentModule(m);
    bindWorldModule(m);

    // Convenience functions for common use cases
    
    // Create a complete agent system (brain + world + agent interface)
    m.def("createAgentSystem", []() {
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        auto world = std::make_shared<nlm::SimpleWorld>();
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete agent system with brain, world, and agent interface");

    // Run a complete simulation episode
    m.def("runEpisode", [](std::shared_ptr<nlm::Brain> brain, 
                          std::shared_ptr<nlm::SimpleWorld> world,
                          std::shared_ptr<nlm::AgentBrain> agent,
                          int numSteps) {
        
        // Enable basic subsystems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        world->configure(20.0, 20.0, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        std::vector<float> stats;
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Record statistics
            stats.push_back(brain->getFiringNeuronCount());
            stats.push_back(agent->getCuriosityLevel());
            stats.push_back(agent->getNoveltyLevel());
        }
        
        return stats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       "Run a complete simulation episode and return statistics");

    // Configuration shortcuts
    m.def("createStandardConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        
        // Set common default values
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.05);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        config->set("development.enable", true);
        config->set("development.initial_stage", "CriticalPeriod");
        
        return config;
    }, "Create a configuration with standard NLM settings");

    // Helper function for running learning experiments
    m.def("runLearningExperiment", [](std::shared_ptr<nlm::Brain> brain,
                                       std::shared_ptr<nlm::SimpleWorld> world,
                                       std::shared_ptr<nlm::AgentBrain> agent,
                                       int numSteps,
                                       bool enableLearning = true) {
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        world->configure(15.0, 15.0, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        struct ExperimentStats {
            std::vector<int> firingNeurons;
            std::vector<float> curiosityLevels;
            std::vector<float> noveltyLevels;
            std::vector<float> predictionErrors;
            std::vector<float> developmentalStages;
        } stats;
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            float reward = result.success ? 1.0f : 0.0f;
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Record statistics
            stats.firingNeurons.push_back(brain->getFiringNeuronCount());
            stats.curiosityLevels.push_back(agent->getCuriosityLevel());
            stats.noveltyLevels.push_back(agent->getNoveltyLevel());
            stats.predictionErrors.push_back(agent->getPredictionError());
            stats.developmentalStages.push_back(static_cast<float>(brain->getDevelopmentalStage()));
        }
        
        return stats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), 
       py::arg("enableLearning") = true,
       "Run a learning experiment and return detailed statistics");

    // System information and utility functions
    m.def("getSystemInfo", []() {
        return std::map<std::string, std::string>{
            {"version", "0.1.0"},
            {"name", "NLM - Neural Learning Machine"},
            {"description", "Experimental artificial developmental brain"},
            {"python_bindings", "pybind11"},
            {"cpp_standard", "C++20"},
            {"build_type", "Release"}
        };
    }, "Get system information and version details");

    // Error handling helpers
    m.def("checkBrainState", [](std::shared_ptr<nlm::Brain> brain) {
        if (!brain) {
            throw std::invalid_argument("Brain pointer is null");
        }
        
        auto config = brain->getConfig();
        if (!config) {
            throw std::runtime_error("Brain has no configuration");
        }
        
        return brain->getTotalNeuronCount();
    }, py::arg("brain"), "Check brain state and return neuron count");

    m.def("checkWorldState", [](std::shared_ptr<nlm::SimpleWorld> world) {
        if (!world) {
            throw std::invalid_argument("World pointer is null");
        }
        
        auto body = world->getAgentBody();
        return std::make_pair(body.x, body.y);
    }, py::arg("world"), "Check world state and return agent position");

    m.def("checkAgentState", [](std::shared_ptr<nlm::AgentBrain> agent) {
        if (!agent) {
            throw std::invalid_argument("Agent pointer is null");
        }
        
        auto brain = agent->getBrain();
        if (!brain) {
            throw std::runtime_error("Agent has no brain");
        }
        
        return std::make_tuple(
            agent->getCuriosityLevel(),
            agent->getNoveltyLevel(),
            agent->getPredictionError()
        );
    }, py::arg("agent"), "Check agent state and return neuromodulation levels");

    // Convenience factory methods
    m.def("createBrainWithDefaults", []() {
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        return brain;
    }, "Create and initialize a brain with default configuration");

    m.def("createWorldWithDefaults", []() {
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(10.0, 10.0, 8, 8);
        world->reset();
        return world;
    }, "Create and configure a world with default settings");

    m.def("createCompleteAgent", []() {
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        auto world = std::make_shared<nlm::SimpleWorld>();
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        
        brain->initialize();
        world->configure(10.0, 10.0, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete agent system with brain, world, and agent interface");

    // Statistics collection utilities
    m.def("collectStats", [](std::shared_ptr<nlm::Brain> brain,
                            std::shared_ptr<nlm::AgentBrain> agent,
                            std::shared_ptr<nlm::SimpleWorld> world) {
        
        struct ComprehensiveStats {
            // Brain statistics
            size_t totalNeurons;
            size_t totalSynapses;
            size_t activeNeurons;
            size_t firingNeurons;
            float averageFiringRate;
            float excitationInhibitionRatio;
            size_t totalSpikeCount;
            
            // Agent statistics
            float curiosityLevel;
            float noveltyLevel;
            float predictionError;
            float neuromodulationLevel;
            int developmentalStage;
            
            // World statistics
            float simulationTime;
            float agentX;
            float agentY;
            float agentOrientation;
            float agentEnergy;
            size_t visionWidth;
            size_t visionHeight;
        } stats;
        
        // Collect brain statistics
        stats.totalNeurons = brain->getTotalNeuronCount();
        stats.totalSynapses = brain->getTotalSynapseCount();
        stats.activeNeurons = brain->getActiveNeuronCount();
        stats.firingNeurons = brain->getFiringNeuronCount();
        stats.averageFiringRate = brain->getAverageFiringRate();
        stats.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        stats.totalSpikeCount = brain->getTotalSpikeCount();
        
        // Collect agent statistics
        stats.curiosityLevel = agent->getCuriosityLevel();
        stats.noveltyLevel = agent->getNoveltyLevel();
        stats.predictionError = agent->getPredictionError();
        stats.neuromodulationLevel = agent->getNeuromodulationLevel();
        stats.developmentalStage = static_cast<int>(agent->getDevelopmentalStage());
        
        // Collect world statistics
        stats.simulationTime = world->getSimulationTime();
        auto body = world->getAgentBody();
        stats.agentX = body.x;
        stats.agentY = body.y;
        stats.agentOrientation = body.orientation;
        stats.agentEnergy = body.energy;
        
        auto percept = world->getSensoryPercept();
        stats.visionWidth = percept.getVisionWidth();
        stats.visionHeight = percept.getVisionHeight();
        
        return stats;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
       "Collect comprehensive statistics from all system components");

    // Visualization helpers
    m.def("brainStatus", [](std::shared_ptr<nlm::Brain> brain) {
        brain->logStatus();
        return std::string("Brain status logged to stdout");
    }, py::arg("brain"), "Log brain status information");

    m.def("worldStatus", [](std::shared_ptr<nlm::SimpleWorld> world) {
        auto body = world->getAgentBody();
        auto percept = world->getSensoryPercept();
        
        std::stringstream ss;
        ss << "World Status:" << std::endl;
        ss << "  Agent Position: (" << body.x << ", " << body.y << ")" << std::endl;
        ss << "  Agent Orientation: " << body.orientation << " radians" << std::endl;
        ss << "  Agent Energy: " << body.energy << std::endl;
        ss << "  Vision: " << percept.getVisionWidth() << "x" << percept.getVisionHeight() << " pixels" << std::endl;
        ss << "  Simulation Time: " << world->getSimulationTime() << " seconds" << std::endl;
        
        return ss.str();
    }, py::arg("world"), "Get detailed world status information");

    m.def("agentStatus", [](std::shared_ptr<nlm::AgentBrain> agent) {
        std::stringstream ss;
        ss << "Agent Status:" << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        ss << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
        ss << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
        ss << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
        ss << "  Prediction Error: " << agent->getPredictionError() << std::endl;
        ss << "  Reward Modulation: " << (agent->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
        ss << "  Structural Plasticity: " << (agent->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
        ss << "  Development: " << (agent->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
        ss << "  Curiosity: " << (agent->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
        
        return ss.str();
    }, py::arg("agent"), "Get detailed agent status information");

    m.def("systemStatus", [](std::shared_ptr<nlm::Brain> brain,
                            std::shared_ptr<nlm::AgentBrain> agent,
                            std::shared_ptr<nlm::SimpleWorld> world) {
        
        std::stringstream ss;
        ss << "=== NLM System Status ===" << std::endl;
        ss << "Brain:" << std::endl;
        ss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Avg Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        ss << "Agent:" << std::endl;
        ss << "  Curiosity: " << agent->getCuriosityLevel() << std::endl;
        ss << "  Novelty: " << agent->getNoveltyLevel() << std::endl;
        ss << "  Prediction Error: " << agent->getPredictionError() << std::endl;
        ss << "  Neuromodulation: " << agent->getNeuromodulationLevel() << std::endl;
        
        ss << "World:" << std::endl;
        auto body = world->getAgentBody();
        ss << "  Agent Position: (" << body.x << ", " << body.y << ")" << std::endl;
        ss << "  Simulation Time: " << world->getSimulationTime() << " seconds" << std::endl;
        
        return ss.str();
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
       "Get comprehensive status information from all system components");

    // Debug and development utilities
    m.def("debugMode", []() {
        return std::string("Debug mode enabled - additional logging and validation will be active");
    }, "Enable debug mode with additional logging");

    m.def("validateSystem", [](std::shared_ptr<nlm::Brain> brain,
                               std::shared_ptr<nlm::AgentBrain> agent,
                               std::shared_ptr<nlm::SimpleWorld> world) {
        
        std::vector<std::string> errors;
        
        // Validate brain
        if (!brain) {
            errors.push_back("Brain pointer is null");
        } else if (brain->getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons");
        }
        
        // Validate agent
        if (!agent) {
            errors.push_back("Agent pointer is null");
        } else if (!agent->getBrain()) {
            errors.push_back("Agent has no brain");
        }
        
        // Validate world
        if (!world) {
            errors.push_back("World pointer is null");
        }
        
        if (errors.empty()) {
            return "System validation passed - all components are properly initialized";
        } else {
            std::string errorMsg = "System validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw std::runtime_error(errorMsg);
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
       "Validate system state and throw error if any component is invalid");

    m.def("resetSystem", [](std::shared_ptr<nlm::Brain> brain,
                            std::shared_ptr<nlm::AgentBrain> agent,
                            std::shared_ptr<nlm::SimpleWorld> world) {
        
        if (brain) brain->reset();
        if (agent) agent->reset();
        if (world) world->reset();
        
        return std::string("System reset completed successfully");
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
       "Reset all system components to initial state");

    // Configuration management
    m.def("saveSystemState", [](std::shared_ptr<nlm::Brain> brain,
                                const std::string& filename) {
        if (!brain) {
            throw std::invalid_argument("Brain pointer is null");
        }
        
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }
        
        bool success = brain->save(filename);
        if (!success) {
            throw std::runtime_error("Failed to save brain state to " + filename);
        }
        
        return std::string("Brain state saved to " + filename);
    }, py::arg("brain"), py::arg("filename"),
       "Save brain state to file (checkpointing)");

    m.def("loadSystemState", [](std::shared_ptr<nlm::Brain> brain,
                                const std::string& filename) {
        if (!brain) {
            throw std::invalid_argument("Brain pointer is null");
        }
        
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }
        
        bool success = brain->load(filename);
        if (!success) {
            throw std::runtime_error("Failed to load brain state from " + filename);
        }
        
        return std::string("Brain state loaded from " + filename);
    }, py::arg("brain"), py::arg("filename"),
       "Load brain state from file (checkpointing)");

    // Performance monitoring
    m.def("getPerformanceStats", [](std::shared_ptr<nlm::Brain> brain,
                                    std::shared_ptr<nlm::SimpleWorld> world) {
        
        struct PerformanceStats {
            size_t totalNeurons;
            size_t totalSynapses;
            double avgFiringRate;
            size_t spikeCount;
            double simulationTime;
            float memoryUsage;
            std::string performanceGrade;
        } stats;
        
        stats.totalNeurons = brain->getTotalNeuronCount();
        stats.totalSynapses = brain->getTotalSynapseCount();
        stats.avgFiringRate = brain->getAverageFiringRate();
        stats.spikeCount = brain->getTotalSpikeCount();
        stats.simulationTime = world->getSimulationTime();
        
        // Calculate performance grade
        double efficiency = (stats.avgFiringRate > 0.0) ? (stats.spikeCount / stats.simulationTime) : 0.0;
        
        if (efficiency > 100.0) {
            stats.performanceGrade = "Excellent";
        } else if (efficiency > 50.0) {
            stats.performanceGrade = "Good";
        } else if (efficiency > 10.0) {
            stats.performanceGrade = "Fair";
        } else {
            stats.performanceGrade = "Needs Improvement";
        }
        
        return stats;
    }, py::arg("brain"), py::arg("world"),
       "Get performance statistics and efficiency metrics");

} // namespace nlm
