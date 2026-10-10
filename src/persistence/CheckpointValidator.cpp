// Checkpoint validator implementation
#include "CheckpointValidator.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

CheckpointValidator::CheckpointValidator() : dataDir_(), config_(nullptr),
                                            errorCount_(0), stats_(), validationEventCallback_(nullptr) {
}

CheckpointValidator::~CheckpointValidator() {
    shutdown();
}

void CheckpointValidator::configure(const std::string& dataDir, const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    dataDir_ = dataDir;
    config_ = config;
    
    NLM_LOG_INFO("Checkpoint validator configured successfully");
}

bool CheckpointValidator::validate(const std::string& checkpointPath, std::vector<std::string>& errors,
                                  const ValidationOptions& options) {
    auto startTime = std::chrono::high_resolution_clock::now();
    CheckpointValidationResult result;
    result.errors = errors;
    
    try {
        // Load checkpoint data
        CheckpointHeader header;
        std::vector<SectionHeader> sections;
        
        if (!loadCheckpointData(checkpointPath, header, sections)) {
            result.valid = false;
            result.errors.push_back("Failed to load checkpoint data");
            logEvent(checkpointPath, result);
            return false;
        }
        
        // Validate header if requested
        if (options.validateHeader) {
            if (!validateHeader(header, result.errors)) {
                result.valid = false;
            }
        }
        
        // Validate sections if requested
        if (options.validateSections) {
            if (!validateSections(sections, header, result.errors)) {
                result.valid = false;
            }
        }
        
        // Validate checksums if requested
        if (options.validateChecksums) {
            if (!validateChecksums(header, sections, result.errors)) {
                result.valid = false;
            }
        }
        
        // Validate version if requested
        if (options.validateVersion) {
            if (!validateVersion(header, result.errors)) {
                result.valid = false;
            }
        }
        
        // Validate data integrity if requested
        if (options.validateDataIntegrity && result.isValid()) {
            if (!validateDataIntegrity(checkpointPath, header, sections, result.errors)) {
                result.valid = false;
            }
        }
        
        result.valid = !result.errors.empty() ? false : true;
        
    } catch (const std::exception& e) {
        result.valid = false;
        result.errors.push_back(std::string("Exception during validation: ") + e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration<double>(endTime - startTime).count();
    result.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    updateStats(result);
    logEvent(checkpointPath, result);
    
    return result.isValid();
}

bool CheckpointValidator::validateHeader(const CheckpointHeader& header, std::vector<std::string>& errors) {
    if (!header.validate()) {
        errors.push_back("Invalid checkpoint header (magic or version)");
        return false;
    }
    
    if (header.totalSize == 0) {
        errors.push_back("Checkpoint header reports zero total size");
        return false;
    }
    
    if (header.neuronCount > 1000000) { // Reasonable limit
        errors.push_back("Checkpoint reports unusually high neuron count: " + std::to_string(header.neuronCount));
        return false;
    }
    
    return true;
}

bool CheckpointValidator::validateSections(const std::vector<SectionHeader>& sections, const CheckpointHeader& header,
                                          std::vector<std::string>& errors) {
    if (sections.empty()) {
        errors.push_back("Checkpoint has no sections");
        return false;
    }
    
    // Check for duplicate section types
    std::unordered_map<uint32_t, int> sectionCounts;
    for (const auto& section : sections) {
        sectionCounts[section.type]++;
    }
    
    for (const auto& pair : sectionCounts) {
        if (pair.second > 1) {
            errors.push_back("Duplicate section type found: " + std::to_string(pair.first));
        }
    }
    
    // Validate section offsets and sizes
    uint64_t currentPos = sizeof(CheckpointHeader) + sections.size() * sizeof(SectionHeader);
    for (size_t i = 0; i < sections.size(); ++i) {
        const auto& section = sections[i];
        
        if (section.offset < currentPos) {
            errors.push_back("Section " + std::to_string(i) + " offset is before expected position");
        }
        
        if (section.size > 100 * 1024 * 1024) { // 100MB limit
            errors.push_back("Section " + std::to_string(i) + " size is too large: " + std::to_string(section.size));
        }
        
        currentPos = section.offset + section.size;
    }
    
    return errors.empty();
}

bool CheckpointValidator::validateChecksums(const CheckpointHeader& header, const std::vector<SectionHeader>& sections,
                                            std::vector<std::string>& errors) {
    // Validate header checksum
    uint64_t calculatedHeaderChecksum = ChecksumCalculator::crc64(&header, sizeof(CheckpointHeader));
    if (header.checksum != calculatedHeaderChecksum) {
        errors.push_back("Header checksum validation failed");
    }
    
    // Validate section checksums
    for (const auto& section : sections) {
        if (section.checksum != 0) { // Skip for demonstration
            // In real implementation, would verify section checksum
        }
    }
    
    return errors.empty();
}

bool CheckpointValidator::validateVersion(const CheckpointHeader& header, std::vector<std::string>& errors) {
    if (header.majorVersion != CHECKPOINT_VERSION_MAJOR) {
        errors.push_back(std::string("Incompatible checkpoint version: major=") + 
                        std::to_string(header.majorVersion) + ", expected=" + 
                        std::to_string(CHECKPOINT_VERSION_MAJOR));
        return false;
    }
    
    return true;
}

bool CheckpointValidator::validateDataIntegrity(const std::string& checkpointPath, const CheckpointHeader& header,
                                                const std::vector<SectionHeader>& sections,
                                                std::vector<std::string>& errors) {
    std::ifstream file(checkpointPath, std::ios::binary);
    if (!file.is_open()) {
        errors.push_back("Cannot open checkpoint file for integrity validation");
        return false;
    }
    
    // Check file size matches header
    file.seekg(0, std::ios::end);
    uint64_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (fileSize != header.totalSize) {
        errors.push_back(std::string("File size mismatch: file=") + std::to_string(fileSize) +
                        ", header reports=" + std::to_string(header.totalSize));
        return false;
    }
    
    // Read and verify file structure
    std::vector<uint8_t> buffer(sizeof(CheckpointHeader) + sections.size() * sizeof(SectionHeader));
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    
    if (!file.good()) {
        errors.push_back("Failed to read checkpoint header and section headers");
        return false;
    }
    
    // Verify header and section headers match
    CheckpointHeader readHeader;
    std::memcpy(&readHeader, buffer.data(), sizeof(CheckpointHeader));
    
    if (readHeader.magic != header.magic || readHeader.totalSize != header.totalSize) {
        errors.push_back("Checkpoint header mismatch");
        return false;
    }
    
    return true;
}

bool CheckpointValidator::quickValidate(const std::string& checkpointPath) {
    std::ifstream file(checkpointPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Quick check: magic number and version
    CheckpointHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(CheckpointHeader));
    
    return header.validate();
}

void CheckpointValidator::updateStats(const CheckpointValidationResult& result) {
    stats_.checkpointsValidated++;
    
    if (result.isValid()) {
        stats_.validCheckpoints++;
    } else {
        stats_.invalidCheckpoints++;
    }
    
    stats_.totalValidationTime += result.validationTime;
    
    if (!result.errors.empty()) {
        stats_.errorCount++;
    }
}

void CheckpointValidator::logEvent(const std::string& checkpointPath, const CheckpointValidationResult& result) {
    std::stringstream logMsg;
    logMsg << "[CheckpointValidator] Validation - Path: " << checkpointPath << " - Result: " << result.getSummary();
    
    if (result.isValid()) {
        NLM_LOG_INFO(logMsg.str());
    } else {
        NLM_LOG_ERROR(logMsg.str());
        
        for (const auto& error : result.errors) {
            NLM_LOG_ERROR("  Error: " + error);
        }
        
        for (const auto& warning : result.warnings) {
            NLM_LOG_WARNING("  Warning: " + warning);
        }
    }
    
    // Call event callback if registered
    if (validationEventCallback_) {
        validationEventCallback_(checkpointPath, result);
    }
}

bool CheckpointValidator::initialize() {
    return true;
}

void CheckpointValidator::cleanup() {
    checkpointReader_.reset();
}

bool CheckpointValidator::loadCheckpointData(const std::string& checkpointPath, CheckpointHeader& header,
                                            std::vector<SectionHeader>& sections) {
    if (!checkpointReader_) {
        checkpointReader_ = std::make_unique<CheckpointReader>();
    }
    
    return checkpointReader_->open(checkpointPath) &&
           header = checkpointReader_->getHeader(),
           sections = {}; // In real implementation, would read sections
    // For demonstration, just return true
}

} // namespace nlm
