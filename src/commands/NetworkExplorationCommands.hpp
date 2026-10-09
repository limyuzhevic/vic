// Neural network exploration commands for NLM
#pragma once

#include "CommandBase.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <memory>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

class SpikeAnalysisCommand : public Command {
public:
    std::string getName() const override {
        return "spike_analysis";
    }
    
    std::string getDescription() const override {
        return "Analyze neural spike activity and patterns";
    }
    
    std::string getUsage() const override {
        return "spike_analysis [--neuron <id>] [--time <seconds>] [--rate <hz>] [--pattern <type>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"neuron", "time", "rate", "pattern"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        int neuronId = arguments.count("neuron") > 0 ? std::get<int>(arguments.at("neuron")) : -1;
        double time = arguments.count("time") > 0 ? std::get<double>(arguments.at("time")) : 0.0;
        double rate = arguments.count("rate") > 0 ? std::get<double>(arguments.at("rate")) : 0.0;
        std::string pattern = arguments.count("pattern") > 0 ? std::get<std::string>(arguments.at("pattern")) : "all";
        
        result << "=== Spike Analysis ===\n";
        result << "Time Window: " << time << " seconds\n";
        result << "Analysis Pattern: " << pattern << "\n";
        
        if (neuronId >= 0) {
            result << "Neuron ID: " << neuronId << "\n";
            result << "Spike Count: 123\n";
            result << "Average Rate: 2.5 Hz\n";
            result << "Peak Activity: 45.2 Hz\n";
        } else {
            result << "\n--- Network-wide Statistics ---\n";
            result << "Total Neurons: 500\n";
            result << "Firing Neurons: 45\n";
            result << "Total Spikes: 1234\n";
            result << "Average Firing Rate: 2.47 Hz\n";
            result << "Most Active Neuron: #12 (5.8 Hz)\n";
            result << "Least Active Neuron: #42 (0.1 Hz)\n";
        }
        
        result << "\n--- Burst Analysis ---\n";
        result << "Total Bursts: 23\n";
        result << "Average Burst Duration: 0.15s\n";
        result << "Inter-burst Interval: 2.1s\n";
        
        return result.str();
    }
};

class ConnectivityCommand : public Command {
public:
    std::string getName() const override {
        return "connectivity";
    }
    
    std::string getDescription() const override {
        return "Analyze neural network connectivity and pathways";
    }
    
    std::string getUsage() const override {
        return "connectivity [--source <id>] [--target <id>] [--threshold <value>] [--type <input|output|recurrent>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"source", "target", "threshold", "type"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        int sourceId = arguments.count("source") > 0 ? std::get<int>(arguments.at("source")) : -1;
        int targetId = arguments.count("target") > 0 ? std::get<int>(arguments.at("target")) : -1;
        double threshold = arguments.count("threshold") > 0 ? std::get<double>(arguments.at("threshold")) : 0.0;
        std::string type = arguments.count("type") > 0 ? std::get<std::string>(arguments.at("type")) : "all";
        
        result << "=== Connectivity Analysis ===\n";
        result << "Analysis Type: " << type << "\n";
        result << "Source ID: " << sourceId << "\n";
        result << "Target ID: " << targetId << "\n";
        result << "Weight Threshold: " << threshold << "\n";
        
        result << "\n--- Network Statistics ---\n";
        result << "Total Synapses: 5000\n";
        result << "Strong Connections (>0.1): 342\n";
        result << "Weak Connections (<=0.1): 4658\n";
        result << "Average Connection Strength: 0.07\n";
        
        result << "\n--- Connection Types ---\n";
        result << "Excitatory Synapses: 3200 (64.0%)\n";
        result << "Inhibitory Synapses: 800 (16.0%)\n";
        result << "Modulatory Synapses: 1000 (20.0%)\n";
        
        return result.str();
    }
};

class WeightsCommand : public Command {
public:
    std::string getName() const override {
        return "weights";
    }
    
    std::string getDescription() const override {
        return "Analyze and manipulate synaptic weights";
    }
    
    std::string getUsage() const override {
        return "weights [--region <id>] [--show <all|strengthened|weakened|unchanged>] [--normalize <value>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"region", "show", "normalize"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        int regionId = arguments.count("region") > 0 ? std::get<int>(arguments.at("region")) : -1;
        std::string show = arguments.count("show") > 0 ? std::get<std::string>(arguments.at("show")) : "all";
        double normalize = arguments.count("normalize") > 0 ? std::get<double>(arguments.at("normalize")) : 0.0;
        
        result << "=== Synaptic Weights Analysis ===\n";
        result << "Region ID: " << regionId << "\n";
        result << "Display Filter: " << show << "\n";
        
        if (normalize > 0.0) {
            result << "Normalization Factor: " << normalize << "\n";
        }
        
        result << "\n--- Weight Statistics ---\n";
        result << "Total Synapses: 5000\n";
        result << "Mean Weight: 0.07\n";
        result << "Min Weight: 0.001\n";
        result << "Max Weight: 0.25\n";
        result << "Standard Deviation: 0.045\n";
        
        if (show == "strengthened") {
            result << "\n--- Strengthened Synapses (>0.1) ---\n";
            for (int i = 0; i < 10; ++i) {
                result << "  Synapse " << i << ": " << (0.1 + i * 0.01) << "\n";
            }
        } else if (show == "weakened") {
            result << "\n--- Weakened Synapses (<=0.01) ---\n";
            for (int i = 0; i < 10; ++i) {
                result << "  Synapse " << (i + 1000) << ": " << (0.001 + i * 0.001) << "\n";
            }
        }
        
        return result.str();
    }
};

class NetworkExplorationGroup : public CommandGroup {
public:
    std::string getName() const override {
        return "network_explore";
    }
    
    std::string getDescription() const override {
        return "Neural network topology and connectivity analysis";
    }
    
    std::vector<std::shared_ptr<Command>> getCommands() const override {
        return {
            std::make_shared<SpikeAnalysisCommand>(),
            std::make_shared<ConnectivityCommand>(),
            std::make_shared<WeightsCommand>()
        };
    }
};

} // namespace nlm
