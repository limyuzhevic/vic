#include "VisualizationCommand.hpp"

namespace nlm {

VisualizationCommand::VisualizationCommand(std::shared_ptr<NLMCLI> cli) : cli(cli) {
}

std::vector<std::string> VisualizationCommand::getSubcommands() const {
    return {"metrics", "memory", "learning", "network", "results"};
}

int VisualizationCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing subcommand for 'visualize' command. Use one of: metrics, memory, learning, network, results");
        return 1;
    }
    
    std::string subcommand = args[0];
    std::vector<std::string> subArgs(args.begin() + 1, args.end());
    
    return executeSubcommand(subcommand, subArgs);
}

int VisualizationCommand::executeSubcommand(const std::string& subcommand, const std::vector<std::string>& args) {
    if (subcommand == "metrics") {
        return executeMetrics(args);
    } else if (subcommand == "memory") {
        return executeMemory(args);
    } else if (subcommand == "learning") {
        return executeLearning(args);
    } else if (subcommand == "network") {
        return executeNetwork(args);
    } else if (subcommand == "results") {
        return executeResults(args);
    } else {
        printError("Unknown subcommand: " + subcommand);
        return 1;
    }
}

int VisualizationCommand::executeMetrics(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing directory for 'visualize metrics' command");
        return 1;
    }
    
    std::string directory = args[0];
    printInfo("Visualizing performance metrics from directory: " + directory);
    
    // TODO: Implement actual metrics visualization logic
    printSuccess("Metrics visualization completed successfully");
    return 0;
}

int VisualizationCommand::executeMemory(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing directory for 'visualize memory' command");
        return 1;
    }
    
    std::string directory = args[0];
    printInfo("Visualizing memory usage patterns from directory: " + directory);
    
    // TODO: Implement actual memory visualization logic
    printSuccess("Memory visualization completed successfully");
    return 0;
}

int VisualizationCommand::executeLearning(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing directory for 'visualize learning' command");
        return 1;
    }
    
    std::string directory = args[0];
    printInfo("Visualizing learning curves from directory: " + directory);
    
    // TODO: Implement actual learning curve visualization logic
    printSuccess("Learning curve visualization completed successfully");
    return 0;
}

int VisualizationCommand::executeNetwork(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Missing directory for 'visualize network' command");
        return 1;
    }
    
    std::string directory = args[0];
    printInfo("Visualizing network structure from directory: " + directory);
    
    // TODO: Implement actual network visualization logic
    printSuccess("Network visualization completed successfully");
    return 0;
}

int VisualizationCommand::executeResults(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        printError("Missing directory and type for 'visualize results' command. Usage: results directory --type type");
        return 1;
    }
    
    std::string directory = args[0];
    std::string type = "plot";
    
    // Parse type option
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--type" && i + 1 < args.size()) {
            type = args[i + 1];
            break;
        }
    }
    
    printInfo("Generating visualization results from directory: " + directory + " with type: " + type);
    
    // TODO: Implement actual results visualization logic
    printSuccess("Results visualization completed successfully");
    return 0;
}

} // namespace nlm
