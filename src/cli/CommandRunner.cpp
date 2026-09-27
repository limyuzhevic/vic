#include "cli/CommandBase.hpp"
#include "cli/CommandRunner.hpp"

using namespace nlm;
namespace cli {

CommandRunner::CommandRunner() {
}

CommandRunner::~CommandRunner() {
    // Cleanup commands
    for (auto& pair : commands_) {
        delete pair.second;
    }
}

void CommandRunner::registerCommand(CommandBase* command) {
    commands_[command->getName()] = command;
}

bool CommandRunner::executeCommand(const std::string& name, const std::map<std::string, std::string>& args) {
    auto it = commands_.find(name);
    if (it == commands_.end()) {
        std::cerr << "Error: Unknown command: " << name << std::endl;
        return false;
    }
    
    try {
        return it->second->execute(args);
    } catch (const std::exception& e) {
        std::cerr << "Error executing command " << name << ": " << e.what() << std::endl;
        return false;
    }
}

void CommandRunner::printHelp(const std::string& name) const {
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        it->second->printHelp();
    } else {
        std::cerr << "Error: Unknown command: " << name << std::endl;
    }
}

void CommandRunner::printAllHelp() const {
    std::cout << "Available commands:" << std::endl;
    for (const auto& pair : commands_) {
        std::cout << "  " << pair.first << " - " << pair.second->getDescription() << std::endl;
    }
}

} // namespace cli
