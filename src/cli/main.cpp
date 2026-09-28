#include "cli/NLMCli.hpp"

// Simple CLI main function
int main(int argc, char* argv[]) {
    nlm::NLMCli cli;
    
    // Parse command line arguments
    if (!cli.parseArguments(argc, argv)) {
        return 1;
    }
    
    // Execute the requested command
    try {
        cli.executeCommand();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
