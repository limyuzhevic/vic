#pragma once

#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include "../performance/CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Validation functions for checkpoint data

bool validateNeuronData(const NeuronCheckpointData& data) {
    if (data.membranePotential.size() != data.restingPotential.size() ||
        data.membranePotential.size() != data.threshold.size() ||
        data.membranePotential.size() != data.resetPotential.size() ||
        data.membranePotential.size() != data.leakConductance.size()) {
        NLM_LOG_ERROR("Neuron data validation failed: array size mismatch");
        return false;
    }
    
    if (data.firingState.size() != data.membranePotential.size() ||
        data.refractoryRemaining.size() != data.membranePotential.size() ||
        data.refractoryPeriod.size() != data.membranePotential.size() ||
        data.lastSpikeTime.size() != data.membranePotential.size() ||
        data.neuronType.size() != data.membranePotential.size() ||
        data.regionId.size() != data.membranePotential.size() ||
        data.populationId.size() != data.membranePotential.size()) {
        NLM_LOG_ERROR("Neuron data validation failed: additional array size mismatch");
        return false;
    }
    
    // Validate numerical ranges
    for (size_t i = 0; i < data.membranePotential.size(); ++i) {
        if (std::isnan(data.membranePotential[i]) || std::isinf(data.membranePotential[i])) {
            NLM_LOG_ERROR(std::string("Neuron data validation failed: invalid membrane potential at index ") + std::to_string(i));
            return false;
        }
        
        if (std::isnan(data.restingPotential[i]) || std::isinf(data.restingPotential[i])) {
            NLM_LOG_ERROR(std::string("Neuron data validation failed: invalid resting potential at index ") + std::to_string(i));
            return false;
        }
        
        if (data.threshold[i] < -100.0f || data.threshold[i] > 50.0f) {
            NLM_LOG_ERROR(std::string("Neuron data validation failed: threshold out of range at index ") + std::to_string(i));
            return false;
        }
        
        if (data.refractoryRemaining[i] > 1000) {
            NLM_LOG_ERROR(std::string("Neuron data validation failed: refractory remaining out of range at index ") + std::to_string(i));
            return false;
        }
        
        if (data.refractoryPeriod[i] == 0 || data.refractoryPeriod[i] > 1000) {
            NLM_LOG_ERROR(std::string("Neuron data validation failed: refractory period out of range at index ") + std::to_string(i));
            return false;
        }
        
        if (data.lastSpikeTime[i] < -1.0f || data.lastSpikeTime[i] > 1e6f) {
            NLM_LOG_ERROR(std::string("Neuron data validation failed: last spike time out of range at index ") + std::to_string(i));
            return false;
        }
    }
    
    return true;
}

bool validateSynapseData(const SynapseCheckpointData& data) {
    if (data.sourceNeuron.size() != data.destinationNeuron.size() ||
        data.sourceNeuron.size() != data.weight.size() ||
        data.sourceNeuron.size() != data.delay.size() ||
        data.sourceNeuron.size() != data.synapseType.size() ||
        data.sourceNeuron.size() != data.plasticityFlags.size() ||
        data.sourceNeuron.size() != data.eligibilityTrace.size() ||
        data.sourceNeuron.size() != data.efficacy.size() ||
        data.sourceNeuron.size() != data.shortTermDepression.size() ||
        data.sourceNeuron.size() != data.shortTermFacilitation.size()) {
        NLM_LOG_ERROR("Synapse data validation failed: array size mismatch");
        return false;
    }
    
    // Validate numerical ranges
    for (size_t i = 0; i < data.sourceNeuron.size(); ++i) {
        if (data.weight[i] < -1.0f || data.weight[i] > 1.0f) {
            NLM_LOG_ERROR(std::string("Synapse data validation failed: weight out of range at index ") + std::to_string(i));
            return false;
        }
        
        if (data.delay[i] == 0 || data.delay[i] > 1000) {
            NLM_LOG_ERROR(std::string("Synapse data validation failed: delay out of range at index ") + std::to_string(i));
            return false;
        }
        
        if (data.synapseType[i] > static_cast<uint8_t>(SynapseType::Bi_directional)) {
            NLM_LOG_ERROR(std::string("Synapse data validation failed: invalid synapse type at index ") + std::to_string(i));
            return false;
        }
        
        if (std::isnan(data.eligibilityTrace[i]) || std::isinf(data.eligibilityTrace[i])) {
            NLM_LOG_ERROR(std::string("Synapse data validation failed: invalid eligibility trace at index ") + std::to_string(i));
            return false;
        }
        
        if (std::isnan(data.efficacy[i]) || std::isinf(data.efficacy[i])) {
            NLM_LOG_ERROR(std::string("Synapse data validation failed: invalid efficacy at index ") + std::to_string(i));
            return false;
        }
        
        if (data.shortTermDepression[i] < 0.0f || data.shortTermDepression[i] > 1.0f) {
            NLM_LOG_ERROR(std::string("Synapse data validation failed: short term depression out of range at index ") + std::to_string(i));
            return false;
        }
        
        if (data.shortTermFacilitation[i] < 0.0f || data.shortTermFacilitation[i] > 1.0f) {
            NLM_LOG_ERROR(std::string("Synapse data validation failed: short term facilitation out of range at index ") + std::to_string(i));
            return false;
        }
    }
    
    return true;
}

bool validateCheckpointDataIntegrity(const CheckpointHeader& header, 
                                     const NeuronCheckpointData& neurons, 
                                     const SynapseCheckpointData& synapses) {
    // Check that header counts match data
    if (header.neuronCount != neurons.membranePotential.size()) {
        NLM_LOG_ERROR(std::string("Checkpoint integrity failed: neuron count mismatch. Header: ") + 
                     std::to_string(header.neuronCount) + ", Actual: " + 
                     std::to_string(neurons.membranePotential.size()));
        return false;
    }
    
    if (header.synapseCount != synapses.weight.size()) {
        NLM_LOG_ERROR(std::string("Checkpoint integrity failed: synapse count mismatch. Header: ") + 
                     std::to_string(header.synapseCount) + ", Actual: " + 
                     std::to_string(synapses.weight.size()));
        return false;
    }
    
    return true;
}

// Error code enumeration
enum class CheckpointErrorCode {
    Success = 0,
    FileNotFound = 1,
    PermissionDenied = 2,
    DiskFull = 3,
    InvalidFormat = 4,
    DataCorruption = 5,
    VersionMismatch = 6,
    MemoryError = 7,
    Timeout = 8,
    ConfigurationError = 9
};

// Safety checks

bool checkFilePermissions(const std::string& filepath, bool writeAccess = true) {
    std::error_code ec;
    bool exists = std::filesystem::exists(filepath, ec);
    
    if (ec) {
        NLM_LOG_ERROR(std::string("File permission check failed: ") + ec.message());
        return false;
    }
    
    if (writeAccess) {
        // Check if file exists and is writable, or if parent directory is writable
        if (exists) {
            if (!std::filesystem::is_regular_file(filepath, ec) ||
                !std::filesystem::status(filepath, ec).permissions() & std::filesystem::perms::owner_write) {
                NLM_LOG_ERROR(std::string("File permission check failed: file is not writable ") + filepath);
                return false;
            }
        } else {
            // Check parent directory permissions
            std::filesystem::path parent = std::filesystem::path(filepath).parent_path();
            if (!std::filesystem::exists(parent, ec) ||
                !(std::filesystem::status(parent, ec).permissions() & std::filesystem::perms::owner_write)) {
                NLM_LOG_ERROR(std::string("File permission check failed: parent directory is not writable ") + parent.string());
                return false;
            }
        }
    }
    
    return true;
}

bool checkDiskSpace(const std::string& filepath, uint64_t requiredBytes) {
    std::error_code ec;
    std::filesystem::path path(filepath);
    std::filesystem::space_info space = std::filesystem::space(path.parent_path(), ec);
    
    if (ec) {
        NLM_LOG_ERROR(std::string("Disk space check failed: ") + ec.message());
        return false;
    }
    
    if (space.available < requiredBytes) {
        NLM_LOG_ERROR(std::string("Disk space check failed: insufficient space. Required: ") + 
                     std::to_string(requiredBytes) + ", Available: " + std::to_string(space.available));
        return false;
    }
    
    return true;
}

// Recovery utilities

class CheckpointRecovery {
public:
    static bool createBackup(const std::string& originalPath, std::string& backupPath) {
        backupPath = originalPath + ".backup";
        std::error_code ec;
        
        if (std::filesystem::exists(originalPath, ec)) {
            try {
                std::filesystem::copy_file(originalPath, backupPath, 
                                         std::filesystem::copy_options::overwrite_existing, ec);
                if (ec) {
                    NLM_LOG_ERROR(std::string("Failed to create backup: ") + ec.message());
                    return false;
                }
                NLM_LOG_INFO(std::string("Backup created at: ") + backupPath);
                return true;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Exception creating backup: ") + e.what());
                return false;
            }
        }
        return true;  // No backup needed if file doesn't exist
    }
    
    static bool restoreFromBackup(const std::string& originalPath, const std::string& backupPath) {
        std::error_code ec;
        
        if (!std::filesystem::exists(backupPath, ec) || ec) {
            NLM_LOG_ERROR(std::string("Backup file not found for restoration: ") + backupPath);
            return false;
        }
        
        try {
            std::filesystem::copy_file(backupPath, originalPath,
                                     std::filesystem::copy_options::overwrite_existing, ec);
            if (ec) {
                NLM_LOG_ERROR(std::string("Failed to restore from backup: ") + ec.message());
                return false;
            }
            NLM_LOG_INFO(std::string("Restored from backup at: ") + backupPath);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception restoring from backup: ") + e.what());
            return false;
        }
    }
};

// Timeout handler for large operations
class TimeoutHandler {
public:
    static bool checkTimeout(std::chrono::steady_clock::time_point startTime, 
                           std::chrono::duration<double> timeoutDuration) {
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        if (elapsed > timeoutDuration) {
            NLM_LOG_ERROR(std::string("Operation timed out after ") + 
                         std::to_string(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()) + 
                         " seconds");
            return true;
        }
        return false;
    }
};

// Enhanced exception types
class CheckpointException : public std::runtime_error {
public:
    CheckpointException(const std::string& message, CheckpointErrorCode code = CheckpointErrorCode::Success)
        : std::runtime_error(message), errorCode_(code) {}
    
    CheckpointErrorCode getErrorCode() const { return errorCode_; }
    
private:
    CheckpointErrorCode errorCode_;
};

class CheckpointDataCorruptionException : public CheckpointException {
public:
    CheckpointDataCorruptionException(const std::string& message, 
                                     const std::string& corruptedSection = "")
        : CheckpointException(message, CheckpointErrorCode::DataCorruption),
          corruptedSection_(corruptedSection) {}
    
    const std::string& getCorruptedSection() const { return corruptedSection_; }
    
private:
    std::string corruptedSection_;
};

class CheckpointVersionMismatchException : public CheckpointException {
public:
    CheckpointVersionMismatchException(const std::string& message,
                                      uint32_t expectedMajor, uint32_t actualMajor,
                                      uint32_t expectedMinor, uint32_t actualMinor)
        : CheckpointException(message, CheckpointErrorCode::VersionMismatch),
          expectedMajor_(expectedMajor), actualMajor_(actualMajor),
          expectedMinor_(expectedMinor), actualMinor_(actualMinor) {}
    
    void getVersionInfo(uint32_t& expectedMajor, uint32_t& actualMajor,
                       uint32_t& expectedMinor, uint32_t& actualMinor) const {
        expectedMajor = expectedMajor_;
        actualMajor = actualMajor_;
        expectedMinor = expectedMinor_;
        actualMinor = actualMinor_;
    }
    
private:
    uint32_t expectedMajor_;
    uint32_t actualMajor_;
    uint32_t expectedMinor_;
    uint32_t actualMinor_;
};

// Configuration validator
class CheckpointConfigurationValidator {
public:
    static bool validateConfiguration(const std::shared_ptr<Config>& config) {
        if (!config) {
            NLM_LOG_ERROR("Configuration validation failed: config is null");
            return false;
        }
        
        // Check required checkpoint configuration
        try {
            if (auto checkpointDir = config->get<std::string>("checkpoint_dir")) {
                // Validate directory path
                std::filesystem::path dir(*checkpointDir);
                if (dir.is_absolute() && dir.has_root_directory()) {
                    // Absolute path - ensure it's within allowed bounds
                    std::string absPath = std::filesystem::canonical(dir).string();nlm_ext:
                    // Check for path traversal attempts
                    if (absPath.find("..") != std::string::npos) {
                        NLM_LOG_ERROR("Configuration validation failed: invalid checkpoint directory path");
                        return false;
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Configuration validation failed with exception: ") + e.what());
            return false;
        }
        
        // Validate simulation parameters
        try {
            if (auto maxCheckpoints = config->get<size_t>("max_checkpoints")) {
                if (*maxCheckpoints == 0 || *maxCheckpoints > 100) {
                    NLM_LOG_ERROR("Configuration validation failed: max_checkpoints out of valid range (1-100)");
                    return false;
                }
            }
            
            if (auto saveInterval = config->get<uint64_t>("checkpoint_save_interval")) {
                if (*saveInterval == 0 || *saveInterval > 1000000) {
                    NLM_LOG_ERROR("Configuration validation failed: checkpoint_save_interval out of valid range (1-1000000)");
                    return false;
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Configuration validation failed with exception: ") + e.what());
            return false;
        }
        
        return true;
    }
};

} // namespace nlm