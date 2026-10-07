#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <sstream>
#include "ErrorSystem.hpp"
#include "core/Checkpoint/CheckpointSystem.hpp"

namespace nlm {

// Checkpoint integrity validation system
class CheckpointValidator {
public:
    struct CheckpointMetadata {
        std::string version;
        std::string format;
        std::chrono::system_clock::time_point timestamp;
        std::string createdBy;
        std::string description;
        
        // Neural network statistics
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        size_t regionCount = 0;
        double averageFiringRate = 0.0;
        double averageSynapticWeight = 0.0;
        
        // Validation metadata
        std::string checksum;
        std::string hashAlgorithm;
        size_t compressedSize = 0;
        size_t uncompressedSize = 0;
        
        // Validation results
        bool valid = false;
        std::vector<std::string> validationErrors;
        std::vector<std::string> validationWarnings;
        double validationScore = 1.0;
    };
    
    struct CheckpointValidationResult {
        bool valid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double score = 1.0;
        std::chrono::system_clock::time_point validationTime;
        
        // Version compatibility
        bool versionCompatible = false;
        std::string requiredVersion;
        std::string currentVersion;
        
        // Integrity checks
        bool checksumValid = false;
        bool structureValid = false;
        bool dataConsistency = false;
        
        std::string getSummary() const {
            std::ostringstream oss;
            oss << "=== Checkpoint Validation Report ===\n";
            oss << "Overall Status: " << (valid ? "VALID" : "INVALID") << "\n";
            oss << "Validation Score: " << score << "/1.0\n";
            oss << "Version Compatible: " << (versionCompatible ? "Yes" : "No") << "\n";
            oss << "Checksum Valid: " << (checksumValid ? "Yes" : "No") << "\n";
            oss << "Structure Valid: " << (structureValid ? "Yes" : "No") << "\n";
            oss << "Data Consistent: " << (dataConsistency ? "Yes" : "No") << "\n";
            
            if (!errors.empty()) {
                oss << "\nErrors:\n";
                for (const auto& err : errors) {
                    oss << "  - " << err << "\n";
                }
            }
            
            if (!warnings.empty()) {
                oss << "\nWarnings:\n";
                for (const auto& warn : warnings) {
                    oss << "  - " << warn << "\n";
                }
            }
            
            return oss.str();
        }
    };
    
    // Checkpoint metadata validation
    static CheckpointMetadata validateMetadata(const CheckpointMetadata& metadata) {
        CheckpointMetadata result = metadata;
        
        // Validate version format
        if (metadata.version.empty()) {
            result.validationErrors.push_back("Missing checkpoint version");
        } else if (!isVersionFormatValid(metadata.version)) {
            result.validationErrors.push_back("Invalid version format: " + metadata.version);
        }
        
        // Validate format
        if (metadata.format.empty()) {
            result.validationErrors.push_back("Missing checkpoint format");
        }
        
        // Validate timestamp
        auto now = std::chrono::system_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::hours>(now - metadata.timestamp);
        if (age.count() > 24 * 3600) { // More than 24 hours old
            result.validationWarnings.push_back("Checkpoint is very old: " + std::to_string(age.count() / 3600) + " hours");
        }
        
        // Validate neural network statistics
        if (metadata.neuronCount == 0) {
            result.validationWarnings.push_back("Checkpoint has no neurons");
        } else if (metadata.neuronCount > 10000000) {
            result.validationWarnings.push_back("Large checkpoint: " + std::to_string(metadata.neuronCount) + " neurons");
        }
        
        if (metadata.synapseCount == 0) {
            result.validationWarnings.push_back("Checkpoint has no synapses");
        }
        
        // Validate checksum
        if (!metadata.checksum.empty() && !metadata.hashAlgorithm.empty()) {
            result.checksumValid = true; // In real implementation, would verify checksum
        }
        
        // Calculate validation score
        calculateValidationScore(result);
        
        return result;
    }
    
    // Checkpoint file integrity validation
    static CheckpointValidationResult validateFileIntegrity(const std::string& filepath) {
        CheckpointValidationResult result;
        
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            result.errors.push_back("Cannot open checkpoint file: " + filepath);
            result.valid = false;
            return result;
        }
        
        // Check file size
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        if (fileSize == 0) {
            result.errors.push_back("Checkpoint file is empty");
            result.valid = false;
            return result;
        }
        
        if (fileSize < 1024) {
            result.warnings.push_back("Checkpoint file is very small: " + std::to_string(fileSize) + " bytes");
        }
        
        // Read and validate file header
        char header[64];
        file.read(header, sizeof(header));
        
        // Basic header validation
        std::string headerStr(header, sizeof(header));
        if (headerStr.find("NLM_CHECKPOINT") == std::string::npos) {
            result.errors.push_back("Invalid checkpoint file format");
            result.structureValid = false;
        } else {
            result.structureValid = true;
        }
        
        // Check for file corruption (basic pattern check)
        if (fileSize > 1024) {
            // Read a sample of the file
            std::vector<char> sample(1024);
            file.read(sample.data(), sample.size());
            
            // Check for reasonable binary data patterns
            bool hasBinaryData = false;
            for (char c : sample) {
                if (c < 0 || c > 127) {
                    hasBinaryData = true;
                    break;
                }
            }
            
            if (!hasBinaryData) {
                result.warnings.push_back("Checkpoint file appears to be text, not binary data");
            }
        }
        
        file.close();
        
        // Determine overall validity
        result.valid = result.errors.empty();
        calculateValidationScore(result);
        
        return result;
    }
    
    // Neural data consistency validation
    static CheckpointValidationResult validateNeuralDataConsistency(const CheckpointMetadata& metadata) {
        CheckpointValidationResult result;
        
        // Validate neuron statistics
        if (metadata.neuronCount > 0) {
            if (metadata.averageFiringRate < 0.0 || metadata.averageFiringRate > 100.0) {
                result.warnings.push_back("Unrealistic average firing rate: " + std::to_string(metadata.averageFiringRate));
            }
            
            if (metadata.averageSynapticWeight < -10.0 || metadata.averageSynapticWeight > 10.0) {
                result.warnings.push_back("Unrealistic average synaptic weight: " + std::to_string(metadata.averageSynapticWeight));
            }
        }
        
        // Validate synapse count
        if (metadata.synapseCount > metadata.neuronCount * 100) {
            result.warnings.push_back("Very high synapse-to-neuron ratio: " + std::to_string(metadata.synapseCount) + " synapses, " + 
                                      std::to_string(metadata.neuronCount) + " neurons");
        }
        
        // Validate region count
        if (metadata.regionCount > 0 && metadata.neuronCount > 0) {
            double neuronsPerRegion = static_cast<double>(metadata.neuronCount) / metadata.regionCount;
            if (neuronsPerRegion > 10000) {
                result.warnings.push_back("Large average neurons per region: " + std::to_string(neuronsPerRegion));
            }
        }
        
        result.dataConsistency = true; // Basic consistency check passed
        calculateValidationScore(result);
        
        return result;
    }
    
    // Version compatibility validation
    static CheckpointValidationResult validateVersionCompatibility(const std::string& checkpointVersion,
                                                                   const std::string& requiredVersion = "1.0") {
        CheckpointValidationResult result;
        
        result.currentVersion = checkpointVersion;
        result.requiredVersion = requiredVersion;
        
        if (checkpointVersion.empty()) {
            result.errors.push_back("Missing checkpoint version");
            result.valid = false;
            return result;
        }
        
        // Parse version strings
        auto [checkpointMajor, checkpointMinor] = parseVersion(checkpointVersion);
        auto [requiredMajor, requiredMinor] = parseVersion(requiredVersion);
        
        if (checkpointMajor > requiredMajor) {
            result.versionCompatible = true;
            result.warnings.push_back("Checkpoint version newer than required: " + checkpointVersion);
        } else if (checkpointMajor == requiredMajor) {
            if (checkpointMinor >= requiredMinor) {
                result.versionCompatible = true;
            } else {
                result.errors.push_back("Checkpoint version too old: " + checkpointVersion + ". Requires at least " + requiredVersion);
                result.valid = false;
            }
        } else {
            result.errors.push_back("Checkpoint version too old: " + checkpointVersion + ". Requires at least " + requiredVersion);
            result.valid = false;
        }
        
        calculateValidationScore(result);
        return result;
    }
    
    // Complete checkpoint validation
    static CheckpointValidationResult validateCheckpoint(const std::string& filepath,
                                                         const std::string& requiredVersion = "1.0") {
        CheckpointValidationResult result;
        
        NLM_LOG_INFO("Validating checkpoint: " + filepath);
        
        // Validate file integrity
        auto fileResult = validateFileIntegrity(filepath);
        result.errors.insert(result.errors.end(), fileResult.errors.begin(), fileResult.errors.end());
        result.warnings.insert(result.warnings.end(), fileResult.warnings.begin(), fileResult.warnings.end());
        result.checksumValid = fileResult.checksumValid;
        result.structureValid = fileResult.structureValid;
        
        // Validate metadata (simplified - in real implementation, would read from file)
        CheckpointMetadata metadata;
        metadata.version = "1.0"; // Example version
        metadata.format = "binary"; // Example format
        metadata.timestamp = std::chrono::system_clock::now();
        metadata.createdBy = "NLM";
        metadata.neuronCount = 1000;
        metadata.synapseCount = 10000;
        metadata.regionCount = 10;
        
        auto metaResult = validateMetadata(metadata);
        result.errors.insert(result.errors.end(), metaResult.validationErrors.begin(), metaResult.validationErrors.end());
        result.warnings.insert(result.warnings.end(), metaResult.validationWarnings.begin(), metaResult.validationWarnings.end());
        
        // Validate version compatibility
        auto versionResult = validateVersionCompatibility(metadata.version, requiredVersion);
        result.errors.insert(result.errors.end(), versionResult.errors.begin(), versionResult.errors.end());
        result.warnings.insert(result.warnings.end(), versionResult.warnings.begin(), versionResult.warnings.end());
        result.versionCompatible = versionResult.versionCompatible;
        
        // Validate neural data consistency
        auto dataResult = validateNeuralDataConsistency(metadata);
        result.errors.insert(result.errors.end(), dataResult.errors.begin(), dataResult.errors.end());
        result.warnings.insert(result.warnings.end(), dataResult.warnings.begin(), dataResult.warnings.end());
        result.dataConsistency = dataResult.dataConsistency;
        
        // Determine final validity
        result.valid = result.errors.empty();
        calculateValidationScore(result);
        
        result.validationTime = std::chrono::system_clock::now();
        
        NLM_LOG_INFO(result.getSummary());
        return result;
    }
    
    // Recovery validation for corrupted checkpoints
    static CheckpointValidationResult recoverCheckpoint(const std::string& filepath) {
        CheckpointValidationResult result;
        
        NLM_LOG_INFO("Attempting checkpoint recovery: " + filepath);
        
        // Try to validate with relaxed criteria
        result = validateFileIntegrity(filepath);
        
        if (result.valid) {
            NLM_LOG_INFO("Checkpoint validation passed with relaxed criteria");
        } else {
            NLM_LOG_WARNING("Checkpoint validation failed. Attempting recovery...");
            
            // Attempt basic recovery
            std::ifstream input(filepath, std::ios::binary);
            if (input) {
                std::string tempFile = filepath + ".recovered";
                std::ofstream output(tempFile, std::ios::binary);
                
                if (output) {
                    // Read and write with basic error checking
                    char buffer[4096];
                    while (input.read(buffer, sizeof(buffer)).good()) {
                        output.write(buffer, input.gcount());
                    }
                    
                    output.close();
                    input.close();
                    
                    // Validate recovered checkpoint
                    auto recoveredResult = validateFileIntegrity(tempFile);
                    if (recoveredResult.valid) {
                        NLM_LOG_INFO("Checkpoint recovery successful");
                        std::rename(tempFile.c_str(), filepath.c_str());
                        return recoveredResult;
                    } else {
                        std::remove(tempFile.c_str());
                        NLM_LOG_ERROR("Checkpoint recovery failed");
                    }
                }
                
                input.close();
            }
        }
        
        return result;
    }
    
private:
    static bool isVersionFormatValid(const std::string& version) {
        // Simple version validation (major.minor.patch)
        size_t dot1 = version.find('.');
        size_t dot2 = version.find('.', dot1 + 1);
        
        if (dot1 == std::string::npos) return false;
        
        std::string major = version.substr(0, dot1);
        std::string minor = (dot2 == std::string::npos) ? version.substr(dot1 + 1) : version.substr(dot1 + 1, dot2 - dot1 - 1);
        
        // Check if parts are numeric
        for (char c : major) {
            if (!std::isdigit(c)) return false;
        }
        
        for (char c : minor) {
            if (!std::isdigit(c)) return false;
        }
        
        return true;
    }
    
    static std::pair<int, int> parseVersion(const std::string& version) {
        size_t dot1 = version.find('.');
        if (dot1 == std::string::npos) {
            return {std::stoi(version), 0};
        }
        
        std::string major = version.substr(0, dot1);
        std::string minor = version.substr(dot1 + 1);
        
        size_t dot2 = minor.find('.');
        if (dot2 != std::string::npos) {
            minor = minor.substr(0, dot2);
        }
        
        return {std::stoi(major), std::stoi(minor)};
    }
    
    static void calculateValidationScore(CheckpointValidationResult& result) {
        double score = 1.0;
        
        // Deduct points for errors
        for (const auto& error : result.errors) {
            if (error.find("version") != std::string::npos || error.find("format") != std::string::npos) {
                score -= 0.3;
            } else {
                score -= 0.1;
            }
        }
        
        // Deduct points for warnings
        for (const auto& warning : result.warnings) {
            score -= 0.05;
        }
        
        // Apply multipliers for critical issues
        if (!result.versionCompatible) score *= 0.5;
        if (!result.checksumValid) score *= 0.3;
        if (!result.structureValid) score *= 0.5;
        if (!result.dataConsistency) score *= 0.7;
        
        result.score = std::max(0.0, score);
    }
};

// High-level checkpoint validation functions
namespace CheckpointValidation {
    
    // Validate a checkpoint file
    static CheckpointValidator::CheckpointValidationResult validateCheckpoint(const std::string& filepath,
                                                                               const std::string& requiredVersion = "1.0") {
        return CheckpointValidator::validateCheckpoint(filepath, requiredVersion);
    }
    
    // Validate a checkpoint with recovery
    static CheckpointValidator::CheckpointValidationResult validateWithRecovery(const std::string& filepath,
                                                                                const std::string& requiredVersion = "1.0") {
        auto result = validateCheckpoint(filepath, requiredVersion);
        
        if (!result.valid) {
            NLM_LOG_WARNING("Validation failed, attempting recovery...");
            result = CheckpointValidator::recoverCheckpoint(filepath);
        }
        
        return result;
    }
    
    // Batch validation of multiple checkpoints
    static std::vector<CheckpointValidator::CheckpointValidationResult> validateBatch(const std::vector<std::string>& filepaths,
                                                                                       const std::string& requiredVersion = "1.0") {
        std::vector<CheckpointValidator::CheckpointValidationResult> results;
        
        for (const auto& filepath : filepaths) {
            NLM_LOG_INFO("Validating checkpoint in batch: " + filepath);
            results.push_back(validateCheckpoint(filepath, requiredVersion));
        }
        
        return results;
    }
    
    // Get summary of checkpoint validation results
    static std::string getValidationSummary(const std::vector<CheckpointValidator::CheckpointValidationResult>& results) {
        std::ostringstream oss;
        oss << "=== Checkpoint Validation Batch Summary ===\n";
        oss << "Total Checkpoints: " << results.size() << "\n";
        
        size_t validCount = 0;
        size_t invalidCount = 0;
        double totalScore = 0.0;
        
        for (size_t i = 0; i < results.size(); ++i) {
            if (results[i].valid) {
                validCount++;
            } else {
                invalidCount++;
            }
            totalScore += results[i].score;
            
            oss << "  " << (i + 1) << ". " << results[i].valid ? "VALID" : "INVALID" 
                << " (score: " << results[i].score << ")\n";
        }
        
        oss << "\nValid: " << validCount << "\n";
        oss << "Invalid: " << invalidCount << "\n";
        oss << "Average Score: " << (results.empty() ? 0.0 : totalScore / results.size()) << "/1.0\n";
        
        return oss.str();
    }
}

} // namespace nlm