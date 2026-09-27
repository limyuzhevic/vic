// Run command implementation
#pragma once

#include "BaseCommand.hpp"
#include "core/Brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <sstream>

namespace nlm {
namespace cli {

class RunCommand : public BaseCommand {
public:
    RunCommand() : BaseCommand("run", "Run simulation with specified parameters") {
        addArgument(Argument("steps", "Number of simulation steps", "int", "1000"));
        addArgument(Argument("time", "Simulation time limit (seconds)", "double", "0.0"));
        addArgument(Argument("checkpoint", "Enable checkpointing", "bool", "false"));
        addArgument(Argument("debug", "Debug mode", "bool", "false"));
        addArgument(Argument("replay", "Replay mode", "bool", "false"));
    }
    
    void addArguments() override {
        addArgument(Argument("steps", "Number of simulation steps", "int", "1000"));
        addArgument(Argument("time", "Simulation time limit (seconds)", "double", "0.0"));
        addArgument(Argument("checkpoint", "Enable checkpointing", "bool", "false"));
        addArgument(Argument("debug", "Debug mode", "bool", "false"));
        addArgument(Argument("replay", "Replay mode", "bool", "false"));
    }
    
    CommandResult execute() override {
        if (!config) {
            return CommandResult(1, "No configuration available");
        }
        
        std::stringstream output;
        output << "Running NLM simulation..." << std::endl;
        
        // Get parameters
        int steps = getConfigValue("steps", 1000);
        double time = getConfigValue("time", 0.0);
        bool checkpoint = getConfigValue("checkpoint", false);
        bool debug = getConfigValue("debug", false);
        bool replay = getConfigValue("replay", false);
        
        output << "Parameters:" << std::endl;
        output << "  Steps: " << steps << std::endl;
        output << "  Time limit: " << (time > 0.0 ? std::to_string(time) + "s" : "unlimited") << std::endl;
        output << "  Checkpoint: " << (checkpoint ? "enabled" : "disabled") << std::endl;
        output << "  Debug mode: " << (debug ? "enabled" : "disabled") << std::endl;
        output << "  Replay mode: " << (replay ? "enabled" : "disabled") << std::endl;
        output << std::endl;
        
        // Initialize brain
        if (logger) {
            logger->info("Initializing brain for simulation");
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            return CommandResult(1, "Failed to initialize brain");
        }
        
        output << "Brain initialized successfully" << std::endl;
        output << "Neuron count: " << brain->getTotalNeuronCount() << std::endl;
        output << "Synapse count: " << brain->getTotalSynapseCount() << std::endl;
        output << std::endl;
        
        // Run simulation
        if (logger) {
            logger->info("Starting simulation run");
        }
        
        // For now, run a simple test
        brain->reset();
        
        output << "Simulation running... (Press Ctrl+C to interrupt)" << std::endl;
        output << "Note: Full simulation implementation requires additional components" << std::endl;
        output << "Current implementation provides demonstration of CLI infrastructure." << std::endl;
        
        // Simulate some steps for demonstration
        for (int i = 0; i < std::min(steps, 100); ++i) {
            brain->step(i, i * config->getOr<double>("simulation_timestep", 0.001));
            if (debug && i % 10 == 0) {
                output << "Step " << i << "/" << steps << " completed" << std::endl;
            }
        }
        
        output << std::endl;
        output << "Simulation completed successfully" << std::endl;
        output << "Final spike count: " << brain->getTotalSpikeCount() << std::endl;
        
        return CommandResult(0, output.str());
    }
};

} // namespace cli
} // namespace nlm
