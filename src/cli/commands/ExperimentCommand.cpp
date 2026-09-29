#include "ExperimentCommand.hpp"
#include <fstream>

namespace nlm {

ExperimentCommand::ExperimentCommand(std::shared_ptr<NLMCLI> cli) : cli(cli) {
}

std::vector<std::string> ExperimentCommand::getSubcommands() const {
    return {"create", "run", "list", "status", "analyze", "export"};
}

int ExperimentCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing subcommand for 'experiment' command. Use one of: create, run, list, status, analyze, export");
        return 1;
    }
    
    std::string subcommand = args[0];
    std::vector<std::string> subArgs(args.begin() + 1, args.end());
    
    return executeSubcommand(subcommand, subArgs);
}

int ExperimentCommand::executeSubcommand(const std::string& subcommand, const std::vector<std::string>& args) {
    if (subcommand == "create") {
        return executeCreate(args);
    } else if (subcommand == "run") {
        return executeRun(args);
    } else if (subcommand == "list") {
        return executeList(args);
    } else if (subcommand == "status") {
        return executeStatus(args);
    } else if (subcommand == "analyze") {
        return executeAnalyze(args);
    } else if (subcommand == "export") {
        return executeExport(args);
    } else {
        printError("Unknown subcommand: " + subcommand);
        return 1;
    }
}

int ExperimentCommand::executeCreate(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing experiment name for 'experiment create' command");
        return 1;
    }
    
    std::string name = args[0];
    std::string configFile;
    
    // Parse config option
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--config" && i + 1 < args.size()) {
            configFile = args[i + 1];
            break;
        }
    }
    
    printInfo("Creating new experiment: " + name);
    if (!configFile.empty()) {
        printInfo("Using configuration file: " + configFile);
    }
    
    // TODO: Implement actual experiment creation logic
    printSuccess("Experiment created successfully: " + name);
    return 0;
}

int ExperimentCommand::executeRun(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing experiment name for 'experiment run' command");
        return 1;
    }
    
    std::string name = args[0];
    printInfo("Running experiment: " + name);
    
    // TODO: Implement actual experiment run logic
    printSuccess("Experiment completed successfully: " + name);
    return 0;
}

int ExperimentCommand::executeList(const std::vector<std::string>& args) {
    printInfo("Listing available experiments...");
    
    // TODO: Implement actual experiment listing logic
    printSuccess("Available experiments (placeholder): experiment1, experiment2, experiment3");
    return 0;
}

int ExperimentCommand::executeStatus(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing experiment name for 'experiment status' command");
        return 1;
    }
    
    std::string name = args[0];
    printInfo("Showing status for experiment: " + name);
    
    // TODO: Implement actual status checking logic
    printSuccess("Experiment status (placeholder): Status for " + name + " is running");
    return 0;
}

int ExperimentCommand::executeAnalyze(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing experiment name for 'experiment analyze' command");
        return 1;
    }
    
    std::string name = args[0];
    printInfo("Analyzing experiment results: " + name);
    
    // TODO: Implement actual analysis logic
    printSuccess("Analysis completed successfully for experiment: " + name);
    return 0;
}

int ExperimentCommand::executeExport(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        printError("Missing experiment name and format for 'experiment export' command. Usage: export name --format format");
        return 1;
    }
    
    std::string name = args[0];
    std::string format = "json";
    
    // Parse format option
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--format" && i + 1 < args.size()) {
            format = args[i + 1];
            break;
        }
    }
    
    printInfo("Exporting experiment data for: " + name + " to format: " + format);
    
    // TODO: Implement actual export logic
    std::string outputFile = name + "." + format;
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        printError("Failed to create output file: " + outputFile);
        return 1;
    }
    
    file << "# Exported experiment data (format: " << format << ")" << std::endl;
    file << "experiment_name: " << name << std::endl;
    
    file.close();
    
    printSuccess("Experiment data exported successfully to: " + outputFile);
    return 0;
}

} // namespace nlm
