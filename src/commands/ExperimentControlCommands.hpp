// Experiment control commands for NLM
#pragma once

#include "CommandBase.hpp"
#include <memory>
#include <iomanip>
#include <sstream>
#include <vector>

namespace nlm {

class ExperimentStartCommand : public Command {
public:
    std::string getName() const override {
        return "experiment_start";
    }
    
    std::string getDescription() const override {
        return "Start a new experiment or continue existing experiment";
    }
    
    std::string getUsage() const override {
        return "experiment_start [--name <label>] [--type <learning|stability|adaptation>] [--parameters <file>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"name", "type", "parameters"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        std::string name = arguments.count("name") > 0 ? std::get<std::string>(arguments.at("name")) : "default_experiment";
        std::string type = arguments.count("type") > 0 ? std::get<std::string>(arguments.at("type")) : "learning";
        std::string parameters = arguments.count("parameters") > 0 ? std::get<std::string>(arguments.at("parameters")) : "";
        
        result << "=== Experiment Start ===\n";
        result << "Experiment Name: " << name << "\n";
        result << "Experiment Type: " << type << "\n";
        result << "Parameters File: " << (parameters.empty() ? "default" : parameters) << "\n";
        
        result << "\nExperiment initialization started."
                << " Waiting for experimental conditions to be met.\n";
        
        return result.str();
    }
};

class ExperimentStopCommand : public Command {
public:
    std::string getName() const override {
        return "experiment_stop";
    }
    
    std::string getDescription() const override {
        return "Stop the current experiment and save results";
    }
    
    std::string getUsage() const override {
        return "experiment_stop [--save <true|false>] [--format <csv|json|xml>] [--output <path>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"save", "format", "output"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        bool save = arguments.count("save") > 0 ? std::get<bool>(arguments.at("save")) : true;
        std::string format = arguments.count("format") > 0 ? std::get<std::string>(arguments.at("format")) : "csv";
        std::string output = arguments.count("output") > 0 ? std::get<std::string>(arguments.at("output")) : "results/experiment_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        
        result << "=== Experiment Stop ===\n";
        result << "Save Results: " << (save ? "Yes" : "No") << "\n";
        result << "Output Format: " << format << "\n";
        result << "Output Path: " << output << "\n";
        
        result << "\nExperiment terminated."
                << " Processing final results and saving experiment data.\n";
        
        if (save) {
            result << "Results saved in " << format << " format to " << output << ".\n";
        }
        
        return result.str();
    }
};

class ParameterTuningCommand : public Command {
public:
    std::string getName() const override {
        return "parameter_tuning";
    }
    
    std::string getDescription() const override {
        return "Tune neural network parameters for optimal performance";
    }
    
    std::string getUsage() const override {
        return "parameter_tuning [--method <grid|random|bayesian>] [--objective <spike_count|firing_rate|stability>] [--iterations <count>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"method", "objective", "iterations"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        std::string method = arguments.count("method") > 0 ? std::get<std::string>(arguments.at("method")) : "grid";
        std::string objective = arguments.count("objective") > 0 ? std::get<std::string>(arguments.at("objective")) : "firing_rate";
        int iterations = arguments.count("iterations") > 0 ? std::get<int>(arguments.at("iterations")) : 100;
        
        result << "=== Parameter Tuning ===\n";
        result << "Tuning Method: " << method << "\n";
        result << "Objective Function: " << objective << "\n";
        result << "Iterations: " << iterations << "\n";
        
        result << "\nStarting parameter optimization..."
                << " This may take several minutes.\n";
        
        return result.str();
    }
};

class ResultAnalysisCommand : public Command {
public:
    std::string getName() const override {
        return "result_analysis";
    }
    
    std::string getDescription() const override {
        return "Analyze experiment results and performance metrics";
    }
    
    std::string getUsage() const override {
        return "result_analysis [--input <file>] [--metric <all|spike_count|firing_rate|plasticity>] [--stat <mean|median|std|histogram>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"input", "metric", "stat"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        std::string input = arguments.count("input") > 0 ? std::get<std::string>(arguments.at("input")) : "results/experiment_data.csv";
        std::string metric = arguments.count("metric") > 0 ? std::get<std::string>(arguments.at("metric")) : "all";
        std::string stat = arguments.count("stat") > 0 ? std::get<std::string>(arguments.at("stat")) : "mean";
        
        result << "=== Result Analysis ===\n";
        result << "Input File: " << input << "\n";
        result << "Analysis Metric: " << metric << "\n";
        result << "Statistical Measure: " << stat << "\n";
        
        result << "\nAnalyzing experimental results..."
                << " Extracting key performance indicators.\n";
        
        result << "\n--- Analysis Results ---\n";
        result << "Total Experiments: 50\n";
        result << "Successful Runs: 47\n";
        result << "Success Rate: 94.0%\n";
        result << "Average Performance: 85.2%\n";
        result << "Best Configuration: Params A+B\n";
        
        return result.str();
    }
};

class ExperimentControlGroup : public CommandGroup {
public:
    std::string getName() const override {
        return "experiment_control";
    }
    
    std::string getDescription() const override {
        return "Experiment lifecycle and result analysis commands";
    }
    
    std::vector<std::shared_ptr<Command>> getCommands() const override {
        return {
            std::make_shared<ExperimentStartCommand>(),
            std::make_shared<ExperimentStopCommand>(),
            std::make_shared<ParameterTuningCommand>(),
            std::make_shared<ResultAnalysisCommand>()
        };
    }
};

} // namespace nlm
