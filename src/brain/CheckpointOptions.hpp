#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace nlm {

// Checkpoint metadata
struct CheckpointMetadata {
    std::string version;                    // Checkpoint version
    std::string softwareVersion;            // Software version that created checkpoint
    std::string description;                 // Human-readable description
    std::chrono::system_clock::time_point creationTime;  // Creation time
    std::string author;                      // Author who created checkpoint
    std::string tags;                        // Tags for categorization
    std::vector<std::string> dependencies;   // Required dependencies
    
    CheckpointMetadata() : version("1.0"), softwareVersion("NLM-6.0"),
                          creationTime(std::chrono::system_clock::now()),
                          author("System") {}
};

// Checkpoint validation result
struct CheckpointValidationResult {
    bool valid = true;                       // Whether checkpoint is valid
    std::string errorMessage;                // Error message if invalid
    std::vector<std::string> warnings;       // Warnings about checkpoint
    double checksum = 0.0;                   // Checksum value
    bool checksumValid = true;               // Whether checksum is valid
    size_t dataSize = 0;                     // Size of checkpoint data
    size_t expectedSize = 0;                 // Expected size of checkpoint data
    
    CheckpointValidationResult() : valid(true), checksumValid(true) {}
};

} // namespace nlm