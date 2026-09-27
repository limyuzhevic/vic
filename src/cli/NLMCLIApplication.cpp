#include "cli/NLMCLIApplication.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "brain/Brain.hpp"

using namespace nlm;
namespace nlm_cli {

NLMCLIApplication::NLMCLIApplication() {
    initializeCommands();
    printBanner("NLM CLI Application");
}

void NLMCLIApplication::initializeCommands() {
    registerCommands();
    
    NLM_LOG_INFO("CLI", "Registered " + std::to_string(commands_.size()) + " commands:");
    for (const auto& pair : commands_) {
        NLM_LOG_INFO("CLI", "  " + pair.first + " - " + pair.second->getDescription());
    }
}

bool NLMCLIApplication::parseArguments(int argc, char** argv) {
    if (argc < 2) {
        printGeneralHelp();
        NLM_LOG_WARNING("CLI", "No arguments provided, showing help");
        return false;
    }
    
    std::string command = argv[1];
    
    // Parse global options
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quiet" || arg == "-q") {
            quiet_mode_ = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose_mode_ = true;
        } else if (arg.substr(0, 2) == "--config") {
            config_file_ = arg.find('=') != std::string::npos ? 
                arg.substr(arg.find('=') + 1) : argv[++i];
        } else if (arg.substr(0, 2) == "--output") {
            output_dir_ = arg.find('=') != std::string::npos ? 
                arg.substr(arg.find('=') + 1) : argv[++i];
        }
    }
    
    // For now, we'll just handle basic command execution
    if (commands_.find(command) != commands_.end()) {
        std::map<std::string, std::string> args;
        NLM_LOG_INFO("CLI", "Executing command: " + command);
        return true;
    }
    
    NLM_LOG_ERROR("CLI", "Unknown command: " + command, "Use --help for available commands");
    printGeneralHelp();
    return false;
}

int NLMCLIApplication::executeCommand() {
    if (commands_.empty()) {
        NLM_LOG_ERROR("CLI", "No commands registered", "Initialize commands before executing");
        return 1;
    }
    
    NLM_LOG_INFO("CLI", "Command execution not fully implemented yet");
    return 0;
}

void NLMCLIApplication::printCommandHelp(const std::string& command_name) {
    auto it = commands_.find(command_name);
    if (it != commands_.end()) {
        it->second->printHelp();
    } else {
        NLM_LOG_ERROR("CLI", "Unknown command: " + command_name, "Available commands can be listed with --help");
    }
}

void NLMCLIApplication::printGeneralHelp() const {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Neural Learning Machine                             ║
    ║     Command Line Interface                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "─────────────────────" << std::endl;
    
    for (const auto& pair : commands_) {
        std::cout << "  " << pair.first << std::endl;
        std::cout << "    " << pair.second->getDescription() << std::endl;
    }
}

void NLMCLIApplication::registerCommands() {
    commands_["run"] = std::make_unique<RunCommand>();
    commands_["experiment"] = std::make_unique<ExperimentCommand>();
    commands_["benchmark"] = std::make_unique<BenchmarkCommand>();
    commands_["config"] = std::make_unique<ConfigCommand>();
    commands_["visualize"] = std::make_unique<VisualizeCommand>();
    commands_["analyze"] = std::make_unique<AnalyzeCommand>();
}

void NLMCLIApplication::printBanner(const std::string& title) const {
    if (quiet_mode_) return;
    
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

std::string NLMCLIApplication::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm_cli
