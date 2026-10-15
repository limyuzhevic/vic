// CLI application base class
#include "CLI.hpp"

using namespace nlm;

struct NLMApplication::Impl {
    std::string name;
    std::string version;
    CLI cli;
    std::unordered_map<std::string, CommandHandler> commands;
};

NLMApplication::NLMApplication(const std::string& name, const std::string& version)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->version = version;
    pImpl->cli = CLI(name);
}

NLMApplication::~NLMApplication() = default;

int NLMApplication::run(int argc, char** argv) {
    // Register commands and options
    registerCommands();
    registerOptions();
    
    // Parse arguments
    if (!pImpl->cli.parse(argc, argv)) {
        if (pImpl->cli.getCLI().hasHelp()) {
            handleHelp();
        } else if (pImpl->cli.getCLI().hasVersion()) {
            handleVersion();
        }
        return 1;
    }
    
    // Validate
    if (!pImpl->cli.validate()) {
        std::cerr << "Error: " << pImpl->cli.getError() << std::endl;
        return 1;
    }
    
    // Execute command
    std::string command = pImpl->cli.getCommand();
    if (command.empty()) {
        std::cerr << "Error: No command specified" << std::endl;
        getCLI().printHelp();
        return 1;
    }
    
    auto it = pImpl->commands.find(command);
    if (it == pImpl->commands.end()) {
        std::cerr << "Error: Unknown command: " << command << std::endl;
        getCLI().printHelp();
        return 1;
    }
    
    it->second(pImpl->cli);
    return 0;
}

CLI& NLMApplication::getCLI() {
    return pImpl->cli;
}

std::string NLMApplication::getName() const {
    return pImpl->name;
}

std::string NLMApplication::getVersion() const {
    return pImpl->version;
}

void NLMApplication::registerCommands() {}
void NLMApplication::registerOptions() {}

void NLMApplication::handleHelp() const {
    std::cout << "Help requested for " << getName() << std::endl;
}

void NLMApplication::handleVersion() const {
    std::cout << getName() << " version " << getVersion() << std::endl;
}

void NLMApplication::handleConfig() const {
    // Default implementation
}
