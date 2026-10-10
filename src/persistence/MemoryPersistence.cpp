// Memory persistence implementation
#include "MemoryPersistence.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

MemoryPersistence::MemoryPersistence() : config_(nullptr), errorCount_(0), 
                                        stats_(), memoryEventCallback_(nullptr) {
}

MemoryPersistence::~MemoryPersistence() {
    shutdown();
}

bool MemoryPersistence::configure(const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        config_ = config;
        
        NLM_LOG_INFO("Memory persistence configured successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Memory persistence configuration failed: ") + e.what());
        return false;
    }
}

void MemoryPersistence::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Clear all memory systems
    workingMemory_.reset();
    episodicMemory_.reset();
    semanticMemory_.reset();
    proceduralMemory_.reset();
    associativeMemory_.reset();
    
    NLM_LOG_INFO("Memory persistence shutdown complete");
}

bool MemoryPersistence::saveWorkingMemory(const std::string& name, const MemoryPersistenceOptions& options) {
    return executeMemoryOperation(MemorySystemType::Working, "save", name, options);
}

bool MemoryPersistence::saveEpisodicMemory(const std::string& name, const MemoryPersistenceOptions& options) {
    return executeMemoryOperation(MemorySystemType::Episodic, "save", name, options);
}

bool MemoryPersistence::saveSemanticMemory(const std::string& name, const MemoryPersistenceOptions& options) {
    return executeMemoryOperation(MemorySystemType::Semantic, "save", name, options);
}

bool MemoryPersistence::saveProceduralMemory(const std::string& name, const MemoryPersistenceOptions& options) {
    return executeMemoryOperation(MemorySystemType::Procedural, "save", name, options);
}

bool MemoryPersistence::saveAssociativeMemory(const std::string& name, const MemoryPersistenceOptions& options) {
    return executeMemoryOperation(MemorySystemType::Associative, "save", name, options);
}

bool MemoryPersistence::loadWorkingMemory(const std::string& name) {
    return executeMemoryOperation(MemorySystemType::Working, "load", name, MemoryPersistenceOptions());
}

bool MemoryPersistence::loadEpisodicMemory(const std::string& name) {
    return executeMemoryOperation(MemorySystemType::Episodic, "load", name, MemoryPersistenceOptions());
}

bool MemoryPersistence::loadSemanticMemory(const std::string& name) {
    return executeMemoryOperation(MemorySystemType::Semantic, "load", name, MemoryPersistenceOptions());
}

bool MemoryPersistence::loadProceduralMemory(const std::string& name) {
    return executeMemoryOperation(MemorySystemType::Procedural, "load", name, MemoryPersistenceOptions());
}

bool MemoryPersistence::loadAssociativeMemory(const std::string& name) {
    return executeMemoryOperation(MemorySystemType::Associative, "load", name, MemoryPersistenceOptions());
}

bool MemoryPersistence::saveAllMemories(const std::string& name, const MemoryPersistenceOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    MemoryPersistenceResult result;
    result.success = false;
    
    try {
        status_ = MemoryPersistenceStatus::Saving;
        
        // Save all memory systems
        bool workingSuccess = saveMemorySystem(MemorySystemType::Working, name, options);
        bool episodicSuccess = saveMemorySystem(MemorySystemType::Episodic, name, options);
        bool semanticSuccess = saveMemorySystem(MemorySystemType::Semantic, name, options);
        bool proceduralSuccess = saveMemorySystem(MemorySystemType::Procedural, name, options);
        bool associativeSuccess = saveMemorySystem(MemorySystemType::Associative, name, options);
        
        if (!workingSuccess || !episodicSuccess || !semanticSuccess || 
            !proceduralSuccess || !associativeSuccess) {
            result.message = "Failed to save all memory systems";
            logEvent(MemorySystemType::Working, "save_all_failed", name, result);
            return false;
        }
        
        result.success = true;
        result.message = "All memory systems saved successfully";
        
        updateStats(result, MemorySystemType::Working); // Use first type for overall stats
        logEvent(MemorySystemType::Working, "save_all", name, result);
        
        NLM_LOG_INFO("All memory systems saved successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception saving all memory systems: ") + e.what();
        logEvent(MemorySystemType::Working, "save_all_exception", name, result);
        return false;
    }
}

bool MemoryPersistence::loadAllMemories(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    MemoryPersistenceResult result;
    result.success = false;
    
    try {
        status_ = MemoryPersistenceStatus::Loading;
        
        // Load all memory systems
        bool workingSuccess = loadMemorySystem(MemorySystemType::Working, name);
        bool episodicSuccess = loadMemorySystem(MemorySystemType::Episodic, name);
        bool semanticSuccess = loadMemorySystem(MemorySystemType::Semantic, name);
        bool proceduralSuccess = loadMemorySystem(MemorySystemType::Procedural, name);
        bool associativeSuccess = loadMemorySystem(MemorySystemType::Associative, name);
        
        if (!workingSuccess || !episodicSuccess || !semanticSuccess || 
            !proceduralSuccess || !associativeSuccess) {
            result.message = "Failed to load all memory systems";
            logEvent(MemorySystemType::Working, "load_all_failed", name, result);
            return false;
        }
        
        result.success = true;
        result.message = "All memory systems loaded successfully";
        
        updateStats(result, MemorySystemType::Working); // Use first type for overall stats
        logEvent(MemorySystemType::Working, "load_all", name, result);
        
        NLM_LOG_INFO("All memory systems loaded successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception loading all memory systems: ") + e.what();
        logEvent(MemorySystemType::Working, "load_all_exception", name, result);
        return false;
    }
}

bool MemoryPersistence::deleteMemories(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Delete memory files for all systems
    for (int type = 0; type < 5; ++type) {
        MemorySystemType memType = static_cast<MemorySystemType>(type);
        std::string filepath = getMemoryPath(memType, name);
        
        if (std::filesystem::exists(filepath)) {
            if (std::remove(filepath.c_str()) != 0) {
                return false;
            }
        }
    }
    
    return true;
}

bool MemoryPersistence::validateConfiguration() const {
    validationErrors_.clear();
    
    if (!config_) {
        validationErrors_.push_back("Configuration is not set");
    }
    
    return validationErrors_.empty();
}

bool MemoryPersistence::validateMemoryIntegrity() const {
    validationErrors_.clear();
    
    // Validate each memory system
    if (workingMemory_ && !workingMemory_->validate()) {
        validationErrors_.push_back("Working memory integrity validation failed");
    }
    
    if (episodicMemory_ && !episodicMemory_->validate()) {
        validationErrors_.push_back("Episodic memory integrity validation failed");
    }
    
    if (semanticMemory_ && !semanticMemory_->validate()) {
        validationErrors_.push_back("Semantic memory integrity validation failed");
    }
    
    if (proceduralMemory_ && !proceduralMemory_->validate()) {
        validationErrors_.push_back("Procedural memory integrity validation failed");
    }
    
    if (associativeMemory_ && !associativeMemory_->validate()) {
        validationErrors_.push_back("Associative memory integrity validation failed");
    }
    
    return validationErrors_.empty();
}

void MemoryPersistence::updateStats(const MemoryPersistenceResult& result, MemorySystemType type) {
    stats_.totalItemsSaved += result.totalItems;
    stats_.totalSizeSaved += result.totalSize;
    stats_.totalTimeSpent += result.elapsedTime;
    
    if (!result.success) {
        stats_.errorCount++;
    }
    
    // Update specific system stats
    switch (type) {
        case MemorySystemType::Working:
            stats_.workingMemorySaved++;
            break;
        case MemorySystemType::Episodic:
            stats_.episodicMemorySaved++;
            break;
        case MemorySystemType::Semantic:
            stats_.semanticMemorySaved++;
            break;
        case MemorySystemType::Procedural:
            stats_.proceduralMemorySaved++;
            break;
        case MemorySystemType::Associative:
            stats_.associativeMemorySaved++;
            break;
    }
}

void MemoryPersistence::logEvent(MemorySystemType type, const std::string& operation,
                                 const std::string& name, const MemoryPersistenceResult& result) {
    std::stringstream logMsg;
    logMsg << "[MemoryPersistence] " << operation << " - System: " << 
           static_cast<int>(type) << " - Name: " << name;
    
    if (!result.message.empty()) {
        logMsg << " - " << result.message;
    }
    
    if (result.success) {
        NLM_LOG_INFO(logMsg.str());
    } else {
        NLM_LOG_ERROR(logMsg.str());
    }
    
    // Call event callback if registered
    if (memoryEventCallback_) {
        memoryEventCallback_(type, name, result);
    }
}

MemoryPersistenceResult MemoryPersistence::executeMemoryOperation(MemorySystemType type,
                                                                   const std::string& operation,
                                                                   const std::string& name,
                                                                   const MemoryPersistenceOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    MemoryPersistenceResult result;
    result.success = false;
    
    try {
        if (!validateConfiguration()) {
            errorCount_++;
            result.message = "Invalid configuration";
            logEvent(type, operation + "_failed", name, result);
            return result;
        }
        
        bool success = false;
        
        if (operation == "save") {
            success = saveMemorySystem(type, name, options);
        } else if (operation == "load") {
            success = loadMemorySystem(type, name);
        }
        
        if (!success) {
            errorCount_++;
            result.message = "Memory system operation failed: " + operation;
            logEvent(type, operation + "_failed", name, result);
            return result;
        }
        
        result.success = true;
        result.message = "Memory system " + operation + "d successfully";
        
        logEvent(type, operation, name, result);
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception in memory operation: ") + e.what();
        logEvent(type, operation + "_exception", name, result);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    result.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    return result;
}

bool MemoryPersistence::saveMemorySystem(MemorySystemType type, const std::string& name,
                                        const MemoryPersistenceOptions& options) {
    std::string filepath = getMemoryPath(type, name);
    
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple serialization for demonstration
        // In real implementation, would serialize the actual memory system data
        
        // Write format marker
        file.write("NLM_MEM_V1", 9);
        
        // Write options
        uint8_t compressFlag = options.compress ? 1 : 0;
        uint8_t encryptFlag = options.encrypt ? 1 : 0;
        file.write(reinterpret_cast<const char*>(&compressFlag), 1);
        file.write(reinterpret_cast<const char*>(&encryptFlag), 1);
        
        file.flush();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving memory system: ") + e.what());
        return false;
    }
}

bool MemoryPersistence::loadMemorySystem(MemorySystemType type, const std::string& name) {
    std::string filepath = getMemoryPath(type, name);
    
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open memory file: " + filepath);
            return false;
        }
        
        // Read format marker
        char formatMarker[10] = {0};
        file.read(formatMarker, 9);
        
        if (std::string(formatMarker, 9) != "NLM_MEM_V1") {
            NLM_LOG_ERROR("Invalid memory file format: " + filepath);
            return false;
        }
        
        // Read options
        uint8_t compressFlag, encryptFlag;
        file.read(reinterpret_cast<char*>(&compressFlag), 1);
        file.read(reinterpret_cast<char*>(&encryptFlag), 1);
        
        file.close();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading memory system: ") + e.what());
        return false;
    }
}

std::string MemoryPersistence::getMemoryPath(MemorySystemType type, const std::string& name) const {
    std::string typeStr;
    switch (type) {
        case MemorySystemType::Working: return "data/mem/working_" + name + ".mem";
        case MemorySystemType::Episodic: return "data/mem/episodic_" + name + ".mem";
        case MemorySystemType::Semantic: return "data/mem/semantic_" + name + ".mem";
        case MemorySystemType::Procedural: return "data/mem/procedural_" + name + ".mem";
        case MemorySystemType::Associative: return "data/mem/associative_" + name + ".mem";
        default: return "data/mem/unknown_" + name + ".mem";
    }
}

} // namespace nlm
