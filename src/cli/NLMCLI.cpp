#include "cli/NLMCLI.hpp"

using namespace nlm;
namespace cli {

NLMCLI::NLMCLI() : config_file_("configs/default.cfg"), output_dir_("."), quiet_mode_(false), verbose_mode_(false) {
}

int NLMCLI::run(int argc, char** argv) {
    printBanner("NLM CLI");
    
    std::map<std::string, std::string> args;
    if (!parseArguments(argc, argv, args)) {
        return 1;
    }
    
    std::string command = args["command"];
    return executeCommand(command, args);
}

void NLMCLI::printBanner(const std::string& title) const {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Neural Learning Machine                             ║
    ║     Command Line Interface                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    if (!title.empty()) {
        std::cout << "    Title: " << title << std::endl;
    }
}

std::string NLMCLI::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool NLMCLI::parseArguments(int argc, char** argv, std::map<std::string, std::string>& args) {
    if (argc < 2) {
        printBanner("NLM CLI Help");
        std::cout << "Usage: nlm-cli <command> [options]" << std::endl;
        std::cout << "Available commands: run, experiment, benchmark, config, visualize, analyze, help" << std::endl;
        return false;
    }
    
    args["command"] = argv[1];
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quiet" || arg == "-q") {
            args["quiet"] = "true";
        } else if (arg == "--verbose" || arg == "-v") {
            args["verbose"] = "true";
        } else if (arg.substr(0, 2) == "--config") {
            args["config_file"] = arg.find('=') != std::string::npos ? 
                arg.substr(arg.find('=') + 1) : (i + 1 < argc ? argv[++i] : "");
        } else if (arg.substr(0, 2) == "--output") {
            args["output_dir"] = arg.find('=') != std::string::npos ? 
                arg.substr(arg.find('=') + 1) : (i + 1 < argc ? argv[++i] : "");
        }
    }
    
    return true;
}

bool NLMCLI::executeCommand(const std::string& command, const std::map<std::string, std::string>& args) {
    if (command == "run") {
        std::cout << "Run command not yet implemented" << std::endl;
        std::cout << "Usage: nlm-cli run --config <file> [--steps <n>] [--duration <sec>] [--parallel] [--threads <n>]" << std::endl;
        return false;
    } else if (command == "experiment") {
        std::cout << "Experiment command not yet implemented" << std::endl;
        std::cout << "Usage: nlm-cli experiment <experiment_name> [--phase <p>] [--params <file>] [--parallel]" << std::endl;
        return false;
    } else if (command == "benchmark") {
        std::cout << "Benchmark command not yet implemented" << std::endl;
        std::cout << "Usage: nlm-cli benchmark [--neurons <n>] [--steps <n>] [--format <fmt>] [--benchmark <b>]" << std::endl;
        return false;
    } else if (command == "config") {
        std::cout << "Config command not yet implemented" << std::endl;
        std::cout << "Usage: nlm-cli config [--action <act>] [--output <file>] [--validate]" << std::endl;
        return false;
    } else if (command == "visualize") {
        std::cout << "Visualize command not yet implemented" << std::endl;
        std::cout << "Usage: nlm-cli visualize <data_file> [--format <fmt>] [--type <type>]" << std::endl;
        return false;
    } else if (command == "analyze") {
        std::cout << "Analyze command not yet implemented" << std::endl;
        std::cout << "Usage: nlm-cli analyze <input_file> [--output <file>] [--type <type>]" << std::endl;
        return false;
    } else if (command == "help") {
        printBanner("NLM CLI Help");
        std::cout << "Available commands:" << std::endl;
        std::cout << "  run        - Run a simulation with specified configuration" << std::endl;
        std::cout << "  experiment - Run a specific experiment (Phase 3, 4, 5, 6)" << std::endl;
        std::cout << "  benchmark  - Run performance benchmarks" << std::endl;
        std::cout << "  config     - Configuration management (create, validate, view)" << std::endl;
        std::cout << "  visualize  - Create visualizations of brain activity" << std::endl;
        std::cout << "  analyze    - Analyze simulation results" << std::endl;
        std::cout << "  help       - Show help for specific commands" << std::endl;
        return true;
    }
    
    std::cerr << "Unknown command: " << command << std::endl;
    return false;
}

} // namespace cli
} // namespace nlm
