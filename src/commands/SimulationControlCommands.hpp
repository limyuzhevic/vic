// Simulation control commands for NLM
#pragma once

#include "CommandBase.hpp"
#include <memory>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace nlm {

class SimulationStepCommand : public Command {
public:
    std::string getName() const override {
        return "step";
    }
    
    std::string getDescription() const override {
        return "Advance simulation by one step";
    }
    
    std::string getUsage() const override {
        return "step [--steps <count>] [--dt <delta_time>] [--pause]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"steps", "dt", "pause"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        int steps = arguments.count("steps") > 0 ? std::get<int>(arguments.at("steps")) : 1;
        double dt = arguments.count("dt") > 0 ? std::get<double>(arguments.at("dt")) : 0.001;
        bool pause = arguments.count("pause") > 0 && std::get<bool>(arguments.at("pause"));
        
        result << "=== Simulation Step ===\n";
        result << "Steps to advance: " << steps << "\n";
        result << "Time step (dt): " << dt << " s\n";
        result << "Pause Mode: " << (pause ? "Enabled" : "Disabled") << "\n";
        
        result << "\nAdvancing simulation by " << steps << " step" << (steps != 1 ? "s" : "") << "...\n";
        
        // Simulate step advancement
        for (int i = 0; i < steps; ++i) {
            result << "Step " << i + 1 << "/" << steps << " completed.\n";
        }
        
        return result.str();
    }
};

class SimulationPauseCommand : public Command {
public:
    std::string getName() const override {
        return "pause";
    }
    
    std::string getDescription() const override {
        return "Pause the simulation";
    }
    
    std::string getUsage() const override {
        return "pause [--reason <description>] [--duration <seconds>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"reason", "duration"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        std::string reason = arguments.count("reason") > 0 ? std::get<std::string>(arguments.at("reason")) : "Manual pause";
        double duration = arguments.count("duration") > 0 ? std::get<double>(arguments.at("duration")) : 0.0;
        
        result << "=== Simulation Pause ===\n";
        result << "Pause Reason: " << reason << "\n";
        result << "Duration: " << duration << " seconds" << (duration > 0 ? " (temporary)" : " (indefinite)") << "\n";
        
        result << "\nSimulation paused."
                << " Use 'resume' to continue.\n";
        
        return result.str();
    }
};

class SimulationResetCommand : public Command {
public:
    std::string getName() const override {
        return "reset";
    }
    
    std::string getDescription() const override {
        return "Reset the simulation to initial state";
    }
    
    std::string getUsage() const override {
        return "reset [--keep_config] [--confirm]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"keep_config", "confirm"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        bool keepConfig = arguments.count("keep_config") > 0 && std::get<bool>(arguments.at("keep_config"));
        bool confirm = arguments.count("confirm") > 0 && std::get<bool>(arguments.at("confirm"));
        
        result << "=== Simulation Reset ===\n";
        result << "Keep Configuration: " << (keepConfig ? "Yes" : "No") << "\n";
        result << "Confirmation Required: " << (confirm ? "Yes" : "No") << "\n";
        
        if (!confirm) {
            result << "\nWARNING: Simulation reset will clear all synaptic weights and state.\n";
            result << "Run with --confirm to proceed.\n";
            return result.str();
        }
        
        result << "\nResetting simulation..."
                << " All neurons, synapses, and memories will be cleared.\n";
        
        if (keepConfig) {
            result << "Configuration settings preserved.\n";
        }
        
        return result.str();
    }
};

class CheckpointCommand : public Command {
public:
    std::string getName() const override {
        return "checkpoint";
    }
    
    std::string getDescription() const override {
        return "Create a simulation checkpoint";
    }
    
    std::string getUsage() const override {
        return "checkpoint [--name <label>] [--compress] [--description <text>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"name", "compress", "description"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        std::string name = arguments.count("name") > 0 ? std::get<std::string>(arguments.at("name")) : "checkpoint_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        bool compress = arguments.count("compress") > 0 && std::get<bool>(arguments.at("compress"));
        std::string description = arguments.count("description") > 0 ? std::get<std::string>(arguments.at("description")) : "Simulation snapshot";
        
        result << "=== Simulation Checkpoint ===\n";
        result << "Checkpoint Name: " << name << "\n";
        result << "Compression: " << (compress ? "Enabled" : "Disabled") << "\n";
        result << "Description: " << description << "\n";
        
        result << "\nCreating checkpoint..."
                << " Saving brain state, weights, and configuration.\n";
        
        if (compress) {
            result << "Compressing checkpoint data.\n";
        }
        
        return result.str();
    }
};

class SimulationControlGroup : public CommandGroup {
public:
    std::string getName() const override {
        return "simulation_control";
    }
    
    std::string getDescription() const override {
        return "Simulation state and execution control";
    }
    
    std::vector<std::shared_ptr<Command>> getCommands() const override {
        return {
            std::make_shared<SimulationStepCommand>(),
            std::make_shared<SimulationPauseCommand>(),
            std::make_shared<SimulationResetCommand>(),
            std::make_shared<CheckpointCommand>()
        };
    }
};

} // namespace nlm
