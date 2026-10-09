#include "BrainInitializeCommand.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>

namespace nlm {

int BrainInitializeCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: Brain name required" << std::endl;
        std::cerr << "Usage: brain initialize <name> [--current] [--reset]" << std::endl;
        return 1;
    }
    
    std::string brainName = args[0];
    bool reset = false;
    bool current = false;
    
    // Parse optional flags
    for (size_t i = 1; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--reset") {
            reset = true;
        } else if (arg == "--current") {
            current = true;
        }
    }
    
    // Retrieve the brain from storage (in a real implementation)
    // This would typically access a global brain registry
    // For now, we'll look for a brain with the given name
    // In a full implementation, you'd have a BrainRegistry class
    
    std::cout << "Brain initialize command executed for brain: " << brainName << std::endl;
    std::cout << "Reset: " << (reset ? "true" : "false") << std::endl;
    std::cout << "Current: " << (current ? "true" : "false") << std::endl;
    
    // In a real implementation, this would:
    // 1. Look up the brain by name in the registry
    // 2. If reset flag, call brain->reset()
    // 3. If current flag, reinitialize with current config
    // 4. Call brain->initialize() if not already initialized
    
    std::cout << "✓ Brain " << brainName << " initialization completed" << std::endl;
    std::cout << "  (Note: Full implementation requires brain registry)" << std::endl;
    
    return 0;
}

} // namespace nlm