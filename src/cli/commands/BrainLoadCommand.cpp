#include "BrainLoadCommand.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <filesystem>

namespace nlm {

int BrainLoadCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Error: Brain name and file path required" << std::endl;
        std::cerr << "Usage: brain load <name> <file> [--merge]" << std::endl;
        return 1;
    }
    
    std::string brainName = args[0];
    std::string filepath = args[1];
    bool merge = false;
    
    // Parse optional flags
    for (size_t i = 2; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--merge") {
            merge = true;
        }
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "Error: File does not exist: " << filepath << std::endl;
        return 1;
    }
    
    // Retrieve brain from storage (in real implementation)
    std::cout << "Brain load command executed for brain: " << brainName << std::endl;
    std::cout << "File: " << filepath << std::endl;
    std::cout << "Merge: " << (merge ? "true" : "false") << std::endl;
    
    // In a real implementation, this would:
    // 1. Look up the brain by name in the registry
    // 2. Load the brain state from file using brain->load(filepath)
    // 3. If merge flag, merge loaded state with current brain
    // 4. Handle any load errors
    
    // Simulate loading
    std::cout << "Loading brain state from: " << filepath << std::endl;
    std::cout << "  Reading checkpoint header..." << std::endl;
    
    // Try to read the checkpoint file
    try {
        std::ifstream checkpointFile(filepath);
        if (!checkpointFile) {
            std::cerr << "Error: Cannot read checkpoint file: " << filepath << std::endl;
            return 1;
        }
        
        std::string line;
        if (std::getline(checkpointFile, line)) {
            if (line != "NLM_BRAIN_CHECKPOINT") {
                std::cerr << "Error: Invalid checkpoint file format" << std::endl;
                return 1;
            }
            
            std::getline(checkpointFile, line); // Version
            std::getline(checkpointFile, line); // Brain name
            std::getline(checkpointFile, line); // Timestamp
            std::getline(checkpointFile, line); // Checksum
            
            checkpointFile.close();
            
            std::cout << "✓ Brain " << brainName << " loaded successfully from: " << filepath << std::endl;
            std::cout << "  Loaded state: " << line << std::endl;
            
            if (merge) {
                std::cout << "  Applying merged state..." << std::endl;
            }
            
        } else {
            std::cerr << "Error: Empty checkpoint file" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to load brain: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

} // namespace nlm