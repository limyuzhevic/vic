// Phase 6 experiment class for Python bindings
#pragma once

#include "pybind11/pybind11.h"
#include <string>
#include <memory>

namespace py = pybind11;

namespace nlm {

// Forward declarations
class Brain;
class Config;
class SimpleWorld;
class AgentBrain;

// Phase 6 integration experiment class
class Phase6Experiment {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<Config> config;
    std::string name;
    bool running;
    
    Phase6Experiment(const std::string& experimentName = "Phase6_Integration_Demo")
        : name(experimentName), running(false) {
        // Create default configuration
        config = std::make_shared<Config>();
        
        // Create brain and initialize
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        agent = std::make_shared<AgentBrain>(brain);
        
        // Create world
        world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 12, 12);
        world->reset();
        world->setAgentStart(15.0, 15.0);
        
        // Initialize agent with world
        agent->initialize(world);
        
        // Enable all Phase 6 subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        running = true;
    }
    
    ~Phase6Experiment() {
        if (running) {
            stop();
        }
    }
    
    // Run the Phase 6 integration demo
    bool run(int numSteps) {
        if (!running) {
            return false;
        }
        
        double timestep = 0.001;
        
        for (uint64_t step = 0; step < numSteps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step, step * timestep);
            
            // Decode motor command
            auto motor_cmd = agent->decodeMotorCommand();
            
            // Apply motor command
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            // Apply reward modulation
            agent->applyRewardModulation(0.0, 0.0);  // Placeholder reward
            
            // Update development
            agent->updateDevelopment(timestep);
            
            // Log progress
            if (step % 100 == 0) {
                NLM_LOG_INFO("Phase6Experiment: Step " + std::to_string(step) +
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                           " | Dev Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
            }
        }
        
        return true;
    }
    
    // Stop the experiment
    void stop() {
        if (running) {
            running = false;
            // Save final state
            brain->save((name + "_final_state.bin").c_str());
        }
    }
    
    // Get current brain
    std::shared_ptr<Brain> getBrain() const {
        return brain;
    }
    
    // Get current agent
    std::shared_ptr<AgentBrain> getAgent() const {
        return agent;
    }
    
    // Get current world
    std::shared_ptr<SimpleWorld> getWorld() const {
        return world;
    }
    
    // Get experiment name
    std::string getName() const {
        return name;
    }
    
    // Get experiment status
    bool isRunning() const {
        return running;
    }
    
    // Get simulation statistics
    std::string getStatistics() const {
        if (!running) return "Experiment not running";
        
        std::stringstream ss;
        ss << "=== Phase 6 Integration Statistics ===\n";
        ss << "Experiment: " << name << "\n";
        ss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "Average Firing Rate: " << brain->getAverageFiringRate() << "\n";
        ss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        ss << "Curiosity Level: " << agent->getCuriosityLevel() << "\n";
        ss << "Novelty Level: " << agent->getNoveltyLevel() << "\n";
        ss << "Neuromodulation Level: " << agent->getNeuromodulationLevel() << "\n";
        
        return ss.str();
    }
    
    // Enable all subsystems
    void enableAllSubsystems() {
        if (running) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
    }
    
    // Disable all subsystems
    void disableAllSubsystems() {
        if (running) {
            agent->enableRewardModulation(false);
            agent->enableStructuralPlasticity(false);
            agent->enableDevelopment(false);
            agent->enableCuriosity(false);
        }
    }
};

// Factory function for Phase 6 experiment
inline std::shared_ptr<Phase6Experiment> createPhase6Experiment(const std::string& name = "Phase6_Integration_Demo") {
    return std::make_shared<Phase6Experiment>(name);
}

// Configure Phase 6 experiment bindings
inline void configurePhase6Experiment(py::module_ &m) {
    py::class_<Phase6Experiment>(m, "Phase6Experiment", 
                                 "Phase 6 integration experiment for testing all brain systems together")
        .def(py::init<const std::string&>(), py::arg("name") = "Phase6_Integration_Demo",
             "Create a Phase 6 integration experiment")
        .def("run", &Phase6Experiment::run, py::arg("numSteps"),
             "Run the Phase 6 integration experiment for specified number of steps")
        .def("stop", &Phase6Experiment::stop,
             "Stop the experiment and save final state")
        .def("getBrain", &Phase6Experiment::getBrain,
             "Get the brain used in the experiment")
        .def("getAgent", &Phase6Experiment::getAgent,
             "Get the agent brain interface used in the experiment")
        .def("getWorld", &Phase6Experiment::getWorld,
             "Get the world used in the experiment")
        .def("getName", &Phase6Experiment::getName,
             "Get the experiment name")
        .def("isRunning", &Phase6Experiment::isRunning,
             "Check if experiment is running")
        .def("getStatistics", &Phase6Experiment::getStatistics,
             "Get detailed experiment statistics")
        .def("enableAllSubsystems", &Phase6Experiment::enableAllSubsystems,
             "Enable all Phase 6 subsystems")
        .def("disableAllSubsystems", &Phase6Experiment::disableAllSubsystems,
             "Disable all Phase 6 subsystems");

    // Factory function
    m.def("createPhase6Experiment", &createPhase6Experiment,
          py::arg("name") = "Phase6_Integration_Demo",
          "Create a Phase 6 integration experiment");
}
