#include "AdvancedCLI.hpp"

namespace nlm {
namespace cli {

AdvancedCLI::AdvancedCLI(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : brain_(brain), config_(config), verbose_(false), quiet_(false) {}

void AdvancedCLI::registerCommand(std::unique_ptr<Command> command) {
    commands_[command->getName()] = std::move(command);
}

bool AdvancedCLI::executeCommand(const std::string& commandName, const std::vector<std::string>& args) {
    auto it = commands_.find(commandName);
    if (it == commands_.end()) {
        if (!quiet_) {
            std::cerr << "Error: Unknown command: " << commandName << std::endl;
        }
        return false;
    }
    
    // Create context (simplified - in real implementation would be more complex)
    CommandContext context(brain_, config_, std::cout, std::cin);
    
    // Validate command
    if (!it->second->validate(context)) {
        if (!quiet_) {
            std::cerr << "Error: Command validation failed for: " << commandName << std::endl;
        }
        return false;
    }
    
    // Execute command
    try {
        it->second->execute(context);
        history_.push_back(commandName + (args.empty() ? "" : " " + join(args, " ")));
        return true;
    } catch (const std::exception& e) {
        if (!quiet_) {
            std::cerr << "Error executing command " << commandName << ": " << e.what() << std::endl;
        }
        return false;
    }
}

bool AdvancedCLI::executeLine(const std::string& line) {
    if (line.empty() || line == "exit" || line == "quit") {
        return false;
    }
    
    std::vector<std::string> tokens = split(line);
    if (tokens.empty()) {
        return true;
    }
    
    std::string commandName = toLower(tokens[0]);
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    return executeCommand(commandName, args);
}

void AdvancedCLI::showHelp(const std::string& commandName) const {
    if (commandName.empty()) {
        std::cout << "NLM Advanced CLI - Available Commands:" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        for (const auto& pair : commands_) {
            std::cout << "  " << std::left << std::setw(20) << pair.first 
                     << "- " << pair.second->getDescription() << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Use 'help <command>' for more information about a specific command." << std::endl;
    } else {
        auto it = commands_.find(commandName);
        if (it == commands_.end()) {
            std::cerr << "Error: Unknown command: " << commandName << std::endl;
            return;
        }
        
        std::cout << "Command: " << it->second->getName() << std::endl;
        std::cout << "Description: " << it->second->getDescription() << std::endl;
        std::cout << "Usage: " << it->second->getUsage() << std::endl;
    }
}

void AdvancedCLI::showCommands() const {
    showHelp();
}

std::vector<std::string> AdvancedCLI::completeCommand(const std::string& partial) const {
    std::vector<std::string> matches;
    std::string lowerPartial = toLower(partial);
    
    for (const auto& pair : commands_) {
        if (toLower(pair.first).find(lowerPartial) == 0) {
            matches.push_back(pair.first);
        }
    }
    
    return matches;
}

std::string AdvancedCLI::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string AdvancedCLI::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::vector<std::string> AdvancedCLI::split(const std::string& str) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (tokenStream >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
    if (vec.empty()) return "";
    std::ostringstream oss;
    oss << vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        oss << delimiter << vec[i];
    }
    return oss.str();
}

} // namespace cli
} // namespace nlm
