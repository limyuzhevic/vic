// Neuromodulation persistence implementation
#include "NeuromodulationPersistence.hpp"
#include "../neuromodulation/NeuromodulationEngine.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/DopamineSystem.hpp"
#include "../neuromodulation/CuriositySystem.hpp"
#include "../neuromodulation/NoveltySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

NeuromodulationPersistence::NeuromodulationPersistence() : neuromodulationEngine_(nullptr), config_(nullptr),
                                                 errorCount_(0), stats_(), neuromodulationEventCallback_(nullptr) {
}

NeuromodulationPersistence::~NeuromodulationPersistence() {
    shutdown();
}

bool NeuromodulationPersistence::configure(const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        config_ = config;
        
        NLM_LOG_INFO("Neuromodulation persistence configured successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Neuromodulation persistence configuration failed: ") + e.what());
        return false;
    }
}

void NeuromodulationPersistence::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    neuromodulationEngine_.reset();
    
    NLM_LOG_INFO("Neuromodulation persistence shutdown complete");
}

bool NeuromodulationPersistence::saveNeuromodulatorState(const std::string& name, const NeuromodulationPersistenceOptions& options) {
    return executeNeuromodulationOperation(NeuromodulationStateType::NeuromodulatorState, "save", name, options);
}

bool NeuromodulationPersistence::saveDopamineState(const std::string& name, const NeuromodulationPersistenceOptions& options) {
    return executeNeuromodulationOperation(NeuromodulationStateType::DopamineState, "save", name, options);
}

bool NeuromodulationPersistence::saveCuriosityState(const std::string& name, const NeuromodulationPersistenceOptions& options) {
    return executeNeuromodulationOperation(NeuromodulationStateType::CuriosityState, "save", name, options);
}

bool NeuromodulationPersistence::saveNoveltyState(const std::string& name, const NeuromodulationPersistenceOptions& options) {
    return executeNeuromodulationOperation(NeuromodulationStateType::NoveltyState, "save", name, options);
}

bool NeuromodulationPersistence::saveNeuromodulationNetworkState(const std::string& name, const NeuromodulationPersistenceOptions& options) {
    return executeNeuromodulationOperation(NeuromodulationStateType::NeuromodulationNetworkState, "save", name, options);
}

bool NeuromodulationPersistence::loadNeuromodulatorState(const std::string& name) {
    return executeNeuromodulationOperation(NeuromodulationStateType::NeuromodulatorState, "load", name, NeuromodulationPersistenceOptions());
}

bool NeuromodulationPersistence::loadDopamineState(const std::string& name) {
    return executeNeuromodulationOperation(NeuromodulationStateType::DopamineState, "load", name, NeuromodulationPersistenceOptions());
}

bool NeuromodulationPersistence::loadCuriosityState(const std::string& name) {
    return executeNeuromodulationOperation(NeuromodulationStateType::CuriosityState, "load", name, NeuromodulationPersistenceOptions());
}

bool NeuromodulationPersistence::loadNoveltyState(const std::string& name) {
    return executeNeuromodulationOperation(NeuromodulationStateType::NoveltyState, "load", name, NeuromodulationPersistenceOptions());
}

bool NeuromodulationPersistence::loadNeuromodulationNetworkState(const std::string& name) {
    return executeNeuromodulationOperation(NeuromodulationStateType::NeuromodulationNetworkState, "load", name, NeuromodulationPersistenceOptions());
}

bool NeuromodulationPersistence::saveAllNeuromodulationStates(const std::string& name, const NeuromodulationPersistenceOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    NeuromodulationPersistenceResult result;
    result.success = false;
    
    try {
        status_ = NeuromodulationPersistenceStatus::Saving;
        
        // Save all neuromodulation state types
        bool neuromodulatorSuccess = saveNeuromodulationStateType(NeuromodulationStateType::NeuromodulatorState, name, options);
        bool dopamineSuccess = saveNeuromodulationStateType(NeuromodulationStateType::DopamineState, name, options);
        bool curiositySuccess = saveNeuromodulationStateType(NeuromodulationStateType::CuriosityState, name, options);
        bool noveltySuccess = saveNeuromodulationStateType(NeuromodulationStateType::NoveltyState, name, options);
        bool networkSuccess = saveNeuromodulationStateType(NeuromodulationStateType::NeuromodulationNetworkState, name, options);
        
        if (!neuromulatorSuccess || !dopamineSuccess || !curiositySuccess || 
            !noveltySuccess || !networkSuccess) {
            result.message = "Failed to save all neuromodulation state types";
            logEvent(NeuromodulationStateType::NeuromodulatorState, "save_all_failed", name, result);
            return false;
        }
        
        result.success = true;
        result.message = "All neuromodulation state types saved successfully";
        
        updateStats(result, NeuromodulationStateType::NeuromodulatorState); // Use first type for overall stats
        logEvent(NeuromodulationStateType::NeuromodulatorState, "save_all", name, result);
        
        NLM_LOG_INFO("All neuromodulation state types saved successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception saving all neuromodulation state types: ") + e.what();
        logEvent(NeuromodulationStateType::NeuromodulatorState, "save_all_exception", name, result);
        return false;
    }
}

bool NeuromodulationPersistence::loadAllNeuromodulationStates(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    NeuromodulationPersistenceResult result;
    result.success = false;
    
    try {
        status_ = NeuromodulationPersistenceStatus::Loading;
        
        // Load all neuromodulation state types
        bool neuromodulatorSuccess = loadNeuromodulationStateType(NeuromodulationStateType::NeuromodulatorState, name);
        bool dopamineSuccess = loadNeuromodulationStateType(NeuromodulationStateType::DopamineState, name);
        bool curiositySuccess = loadNeuromodulationStateType(NeuromodulationStateType::CuriosityState, name);
        bool noveltySuccess = loadNeuromodulationStateType(NeuromodulationStateType::NoveltyState, name);
        bool networkSuccess = loadNeuromodulationStateType(NeuromodulationStateType::NeuromodulationNetworkState, name);
        
        if (!neuromulatorSuccess || !dopamineSuccess || !curiositySuccess || 
            !noveltySuccess || !networkSuccess) {
            result.message = "Failed to load all neuromodulation state types";
            logEvent(NeuromodulationStateType::NeuromodulatorState, "load_all_failed", name, result);
            return false;
        }
        
        result.success = true;
        result.message = "All neuromodulation state types loaded successfully";
        
        updateStats(result, NeuromodulationStateType::NeuromodulatorState); // Use first type for overall stats
        logEvent(NeuromodulationStateType::NeuromodulatorState, "load_all", name, result);
        
        NLM_LOG_INFO("All neuromodulation state types loaded successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception loading all neuromodulation state types: ") + e.what();
        logEvent(NeuromodulationStateType::NeuromodulatorState, "load_all_exception", name, result);
        return false;
    }
}

bool NeuromodulationPersistence::deleteNeuromodulationStates(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Delete neuromodulation state files for all types
    for (int type = 0; type < 5; ++type) {
        NeuromodulationStateType stateType = static_cast<NeuromodulationStateType>(type);
        std::string filepath = getNeuromodulationStatePath(stateType, name);
        
        if (std::filesystem::exists(filepath)) {
            if (std::remove(filepath.c_str()) != 0) {
                return false;
            }
        }
    }
    
    return true;
}

bool NeuromodulationPersistence::validateConfiguration() const {
    validationErrors_.clear();
    
    if (!config_) {
        validationErrors_.push_back("Configuration is not set");
    }
    
    return validationErrors_.empty();
}

bool NeuromodulationPersistence::validateNeuromodulationIntegrity() const {
    validationErrors_.clear();
    
    if (!neuromodulationEngine_) {
        validationErrors_.push_back("Neuromodulation engine is not set");
        return false;
    }
    
    // In real implementation, would validate neuromodulation engine state
    // For demonstration, just check if engine exists
    
    return validationErrors_.empty();
}

void NeuromodulationPersistence::updateStats(const NeuromodulationPersistenceResult& result, NeuromodulationStateType type) {
    stats_.totalStatesSaved += 1; // Count as one saved operation
    stats_.totalSizeSaved += result.totalSizeSaved;
    stats_.totalTimeSpent += result.elapsedTime;
    
    if (!result.success) {
        stats_.errorCount++;
    }
    
    // Update specific state type stats
    switch (type) {
        case NeuromodulationStateType::NeuromodulatorState:
            stats_.neuromodulatorStatesSaved++;
            break;
        case NeuromodulationStateType::DopamineState:
            stats_.dopamineStatesSaved++;
            break;
        case NeuromodulationStateType::CuriosityState:
            stats_.curiosityStatesSaved++;
            break;
        case NeuromodulationStateType::NoveltyState:
            stats_.noveltyStatesSaved++;
            break;
        case NeuromodulationStateType::NeuromodulationNetworkState:
            stats_.networkStatesSaved++;
            break;
    }
}

void NeuromodulationPersistence::logEvent(NeuromodulationStateType type, const std::string& operation,
                                           const std::string& name, const NeuromodulationPersistenceResult& result) {
    std::stringstream logMsg;
    logMsg << "[NeuromodulationPersistence] " << operation << " - State Type: " << 
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
    if (neuromodulationEventCallback_) {
        neuromodulationEventCallback_(type, name, result);
    }
}

NeuromodulationPersistenceResult NeuromodulationPersistence::executeNeuromodulationOperation(NeuromodulationStateType type,
                                                                                            const std::string& operation,
                                                                                            const std::string& name,
                                                                                            const NeuromodulationPersistenceOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    NeuromodulationPersistenceResult result;
    result.success = false;
    
    try {
        if (!validateConfiguration()) {
            errorCount_++;
            result.message = "Invalid configuration";
            logEvent(type, operation + "_failed", name, result);
            return result;
        }
        
        if (!neuromodulationEngine_) {
            errorCount_++;
            result.message = "Neuromodulation engine is not configured";
            logEvent(type, operation + "_failed", name, result);
            return result;
        }
        
        bool success = false;
        
        if (operation == "save") {
            success = saveNeuromodulationStateType(type, name, options);
        } else if (operation == "load") {
            success = loadNeuromodulationStateType(type, name);
        }
        
        if (!success) {
            errorCount_++;
            result.message = "Neuromodulation operation failed: " + operation;
            logEvent(type, operation + "_failed", name, result);
            return result;
        }
        
        result.success = true;
        result.message = "Neuromodulation " + operation + "d successfully";
        
        logEvent(type, operation, name, result);
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception in neuromodulation operation: ") + e.what();
        logEvent(type, operation + "_exception", name, result);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    result.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    return result;
}

bool NeuromodulationPersistence::saveNeuromodulationStateType(NeuromodulationStateType type, const std::string& name,
                                                               const NeuromodulationPersistenceOptions& options) {
    std::string filepath = getNeuromodulationStatePath(type, name);
    
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Write type marker
        uint32_t typeMarker = static_cast<uint32_t>(type);
        file.write(reinterpret_cast<const char*>(&typeMarker), sizeof(typeMarker));
        
        // Write options
        uint8_t compressFlag = options.compress ? 1 : 0;
        uint8_t encryptFlag = options.encrypt ? 1 : 0;
        file.write(reinterpret_cast<const char*>(&compressFlag), 1);
        file.write(reinterpret_cast<const char*>(&encryptFlag), 1);
        
        // In real implementation, would serialize actual neuromodulation state data from neuromodulationEngine_
        // For demonstration, just write placeholder data
        
        // Write state count
        uint64_t stateCount = 500; // Placeholder
        file.write(reinterpret_cast<const char*>(&stateCount), sizeof(stateCount));
        
        // Write placeholder state data
        for (uint64_t i = 0; i < stateCount; ++i) {
            float state = static_cast<float>(i) * 0.05f; // Placeholder
            file.write(reinterpret_cast<const char*>(&state), sizeof(float));
        }
        
        file.flush();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving neuromodulation state type: ") + e.what());
        return false;
    }
}

bool NeuromodulationPersistence::loadNeuromodulationStateType(NeuromodulationStateType type, const std::string& name) {
    std::string filepath = getNeuromodulationStatePath(type, name);
    
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open neuromodulation state file: " + filepath);
            return false;
        }
        
        // Read type marker
        uint32_t typeMarker = 0;
        file.read(reinterpret_cast<char*>(&typeMarker), sizeof(typeMarker));
        
        if (typeMarker != static_cast<uint32_t>(type)) {
            NLM_LOG_ERROR("Mismatch neuromodulation state type in file: " + filepath);
            return false;
        }
        
        // Read options
        uint8_t compressFlag, encryptFlag;
        file.read(reinterpret_cast<char*>(&compressFlag), 1);
        file.read(reinterpret_cast<char*>(&encryptFlag), 1);
        
        // In real implementation, would deserialize actual neuromodulation state data into neuromodulationEngine_
        // For demonstration, just read and ignore placeholder data
        
        // Read state count
        uint64_t stateCount = 0;
        file.read(reinterpret_cast<char*>(&stateCount), sizeof(stateCount));
        
        // Skip placeholder state data
        for (uint64_t i = 0; i < stateCount; ++i) {
            float state = 0.0f;
            file.read(reinterpret_cast<char*>(&state), sizeof(float));
        }
        
        file.close();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading neuromodulation state type: ") + e.what());
        return false;
    }
}

std::string NeuromodulationPersistence::getNeuromodulationStatePath(NeuromodulationStateType type, const std::string& name) const {
    std::string typeStr;
    switch (type) {
        case NeuromodulationStateType::NeuromodulatorState: return "data/neuromodulation/neuromodulator_state_" + name + ".dat";
        case NeuromodulationStateType::DopamineState: return "data/neuromodulation/dopamine_state_" + name + ".dat";
        case NeuromodulationStateType::CuriosityState: return "data/neuromodulation/curiosity_state_" + name + ".dat";
        case NeuromodulationStateType::NoveltyState: return "data/neuromodulation/novelty_state_" + name + ".dat";
        case NeuromodulationStateType::NeuromodulationNetworkState: return "data/neuromodulation/network_state_" + name + ".dat";
        default: return "data/neuromodulation/unknown_state_" + name + ".dat";
    }
}

} // namespace nlm
