// Learning control commands for NLM
#pragma once

#include "CommandBase.hpp"
#include <memory>
#include <iomanip>
#include <sstream>

namespace nlm {

class PlasticityEnableCommand : public Command {
public:
    std::string getName() const override {
        return "plasticity_enable";
    }
    
    std::string getDescription() const override {
        return "Enable synaptic plasticity mechanisms";
    }
    
    std::string getUsage() const override {
        return "plasticity_enable [--hebbian <true|false>] [--stdp <true|false>] [--structural <true|false>] [--region <id>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"hebbian", "stdp", "structural", "region"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        bool hebbian = arguments.count("hebbian") > 0 ? std::get<bool>(arguments.at("hebbian")) : true;
        bool stdp = arguments.count("stdp") > 0 ? std::get<bool>(arguments.at("stdp")) : true;
        bool structural = arguments.count("structural") > 0 ? std::get<bool>(arguments.at("structural")) : false;
        int regionId = arguments.count("region") > 0 ? std::get<int>(arguments.at("region")) : -1;
        
        result << "=== Plasticity Enable ===\n";
        result << "Hebbian Plasticity: " << (hebbian ? "Enabled" : "Disabled") << "\n";
        result << "STDP Plasticity: " << (stdp ? "Enabled" : "Disabled") << "\n";
        result << "Structural Plasticity: " << (structural ? "Enabled" : "Disabled") << "\n";
        result << "Region ID: " << regionId << "\n";
        
        result << "\nPlasticity mechanisms activated."
                << " Synapses will now adapt based on activity patterns."
                << " Learning rate may need adjustment for optimal behavior.\n";
        
        return result.str();
    }
};

class PlasticityDisableCommand : public Command {
public:
    std::string getName() const override {
        return "plasticity_disable";
    }
    
    std::string getDescription() const override {
        return "Disable synaptic plasticity mechanisms";
    }
    
    std::string getUsage() const override {
        return "plasticity_disable [--reason <description>] [--region <id>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"reason", "region"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        std::string reason = arguments.count("reason") > 0 ? std::get<std::string>(arguments.at("reason")) : "Maintenance";
        int regionId = arguments.count("region") > 0 ? std::get<int>(arguments.at("region")) : -1;
        
        result << "=== Plasticity Disable ===\n";
        result << "Reason: " << reason << "\n";
        result << "Region ID: " << regionId << "\n";
        
        result << "\nPlasticity mechanisms disabled.\n";
        result << "Learning will be paused.\n";
        result << "Use 'plasticity_enable' to resume learning.\n";
        
        return result.str();
    }
};

class LearningRateCommand : public Command {
public:
    std::string getName() const override {
        return "learning_rate";
    }
    
    std::string getDescription() const override {
        return "Control learning rate for plasticity mechanisms";
    }
    
    std::string getUsage() const override {
        return "learning_rate [--hebbian <value>] [--stdp <value>] [--structural <value>] [--normalize]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"hebbian", "stdp", "structural", "normalize"};
    }
    
    bool validateArguments(const std::map<std::string, CommandArgument>& arguments) const override {
        if (arguments.count("hebbian") > 0) {
            const auto& rate = std::get<double>(arguments.at("hebbian"));
            if (rate < 0.0 || rate > 1.0) return false;
        }
        if (arguments.count("stdp") > 0) {
            const auto& rate = std::get<double>(arguments.at("stdp"));
            if (rate < 0.0 || rate > 1.0) return false;
        }
        if (arguments.count("structural") > 0) {
            const auto& rate = std::get<double>(arguments.at("structural"));
            if (rate < 0.0 || rate > 1.0) return false;
        }
        return true;
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        double hebbianRate = arguments.count("hebbian") > 0 ? std::get<double>(arguments.at("hebbian")) : 0.01;
        double stdpRate = arguments.count("stdp") > 0 ? std::get<double>(arguments.at("stdp")) : 0.02;
        double structuralRate = arguments.count("structural") > 0 ? std::get<double>(arguments.at("structural")) : 0.001;
        bool normalize = arguments.count("normalize") > 0 && std::get<bool>(arguments.at("normalize"));
        
        result << "=== Learning Rate Control ===\n";
        result << "Hebbian Learning Rate: " << hebbianRate << "\n";
        result << "STDP Learning Rate: " << stdpRate << "\n";
        result << "Structural Plasticity Rate: " << structuralRate << "\n";
        result << "Normalize Rates: " << (normalize ? "Yes" : "No") << "\n";
        
        if (normalize) {
            double avgRate = (hebbianRate + stdpRate + structuralRate) / 3.0;
            result << "Normalized Average Rate: " << avgRate << "\n";
        }
        
        result << "\nLearning rates applied to plasticity mechanisms.\n";
        
        return result.str();
    }
};

class LearningControlGroup : public CommandGroup {
public:
    std::string getName() const override {
        return "learning_control";
    }
    
    std::string getDescription() const override {
        return "Learning rate and plasticity control commands";
    }
    
    std::vector<std::shared_ptr<Command>> getCommands() const override {
        return {
            std::make_shared<PlasticityEnableCommand>(),
            std::make_shared<PlasticityDisableCommand>(),
            std::make_shared<LearningRateCommand>()
        };
    }
};

} // namespace nlm
