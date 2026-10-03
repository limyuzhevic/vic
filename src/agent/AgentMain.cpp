#include "agent/AgentCLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <iostream>
#include <string>

using namespace nlm;

int main(int argc, char** argv) {
    // Initialize logging
    Logger::initialize("nlm_agent");
    
    try {
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration from command line
        config->loadFromArgs(argc, argv);
        
        // Create AgentBrain and Brain (these would need to be properly initialized)
        // For now, we'll create stubs since we're focusing on CLI implementation
        std::shared_ptr<AgentBrain> agentBrain = nullptr;
        std::shared_ptr<Brain> brain = nullptr;
        
        // Create CLI interface
        AgentCLI cli(agentBrain, brain, config);
        
        // Parse and execute command
        if (!cli.parseArguments(argc, argv)) {
            std::cerr << "Error: Failed to parse arguments." << std::endl;
            return 1;
        }
        
        return cli.executeCommand();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Fatal error: ") + e.what());
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
