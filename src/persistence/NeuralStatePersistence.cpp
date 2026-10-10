// Neural state persistence implementation
#include "NeuralStatePersistence.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

NeuralStatePersistence::NeuralStatePersistence() : brain_(nullptr), config_(nullptr),
                                         errorCount_(0), stats_(), neuralStateEventCallback_(nullptr) {
}

NeuralStatePersistence::~NeuralStatePersistence() {
    shutdown();
}

bool NeuralStatePersistence::configure(const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        config_ = config;
        
        NLM_LOG_INFO("Neural state persistence configured successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Neural state persistence configuration failed: ") + e.what());
        return false;
    }
}

void NeuralStatePersistence::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    brain_.reset();
    
    NLM_LOG_INFO("Neural state persistence shutdown complete");
}

bool NeuralStatePersistence::saveNeuronState(const std::string& name, const NeuralStatePersistenceOptions& options) {
    return executeNeuralStateOperation(NeuralStateType::NeuronState, "save", name, options);
}

bool NeuralStatePersistence::saveSynapseState(const std::string& name, const NeuralStatePersistenceOptions& options) {
    return executeNeuralStateOperation(NeuralStateType::SynapseState, "save", name, options);
}

bool NeuralStatePersistence::saveConnectivityState(const std::string& name, const NeuralStatePersistenceOptions& options) {
    return executeNeuralStateOperation(NeuralStateType::ConnectivityState, "save", name, options);
}

bool NeuralStatePersistence::savePlasticityState(const std::string& name, const NeuralStatePersistenceOptions& options) {
    return executeNeuralStateOperation(NeuralStateType::PlasticityState, "save", name, options);
}

bool NeuralStatePersistence::saveSpikeHistoryState(const std::string& name, const NeuralStatePersistenceOptions& options) {
    return executeNeuralStateOperation(NeuralStateType::SpikeHistoryState, "save", name, options);
}

bool NeuralStatePersistence::loadNeuronState(const std::string& name) {
    return executeNeuralStateOperation(NeuralStateType::NeuronState, "load", name, NeuralStatePersistenceOptions());
}

bool NeuralStatePersistence::loadSynapseState(const std::string& name) {
    return executeNeuralStateOperation(NeuralStateType::SynapseState, "load", name, NeuralStatePersistenceOptions());
}

bool NeuralStatePersistence::loadConnectivityState(const std::string& name) {
    return executeNeuralStateOperation(NeuralStateType::ConnectivityState, "load", name, NeuralStatePersistenceOptions());
}

bool NeuralStatePersistence::loadPlasticityState(const std::string& name) {
    return executeNeuralStateOperation(NeuralStateType::PlasticityState, "load", name, NeuralStatePersistenceOptions());
}

bool NeuralStatePersistence::loadSpikeHistoryState(const std::string& name) {
    return executeNeuralStateOperation(NeuralStateType::SpikeHistoryState, "load", name, NeuralStatePersistenceOptions());
}

bool NeuralStatePersistence::saveAllNeuralStates(const std::string& name, const NeuralStatePersistenceOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    NeuralStatePersistenceResult result;
    result.success = false;
    
    try {
        status_ = NeuralStatePersistenceStatus::Saving;
        
        // Save all neural state types
        bool neuronSuccess = saveNeuralStateType(NeuralStateType::NeuronState, name, options);
        bool synapseSuccess = saveNeuralStateType(NeuralStateType::SynapseState, name, options);
        bool connectivitySuccess = saveNeuralStateType(NeuralStateType::ConnectivityState, name, options);
        bool plasticitySuccess = saveNeuralStateType(NeuralStateType::PlasticityState, name, options);
        bool spikeHistorySuccess = saveNeuralStateType(NeuralStateType::SpikeHistoryState, name, options);
        
        if (!neuronSuccess || !synapseSuccess || !connectivitySuccess || 
            !plasticitySuccess || !spikeHistorySuccess) {
            result.message = "Failed to save all neural state types";
            logEvent(NeuralStateType::NeuronState, "save_all_failed", name, result);
            return false;
        }
        
        result.success = true;
        result.message = "All neural state types saved successfully";
        
        updateStats(result, NeuralStateType::NeuronState); // Use first type for overall stats
        logEvent(NeuralStateType::NeuronState, "save_all", name, result);
        
        NLM_LOG_INFO("All neural state types saved successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception saving all neural state types: ") + e.what();
        logEvent(NeuralStateType::NeuronState, "save_all_exception", name, result);
        return false;
    }
}

bool NeuralStatePersistence::loadAllNeuralStates(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    NeuralStatePersistenceResult result;
    result.success = false;
    
    try {
        status_ = NeuralStatePersistenceStatus::Loading;
        
        // Load all neural state types
        bool neuronSuccess = loadNeuralStateType(NeuralStateType::NeuronState, name);
        bool synapseSuccess = loadNeuralStateType(NeuralStateType::SynapseState, name);
        bool connectivitySuccess = loadNeuralStateType(NeuralStateType::ConnectivityState, name);
        bool plasticitySuccess = loadNeuralStateType(NeuralStateType::PlasticityState, name);
        bool spikeHistorySuccess = loadNeuralStateType(NeuralStateType::SpikeHistoryState, name);
        
        if (!neuronSuccess || !synapseSuccess || !connectivitySuccess || 
            !plasticitySuccess || !spikeHistorySuccess) {
            result.message = "Failed to load all neural state types";
            logEvent(NeuralStateType::NeuronState, "load_all_failed", name, result);
            return false;
        }
        
        result.success = true;
        result.message = "All neural state types loaded successfully";
        
        updateStats(result, NeuralStateType::NeuronState); // Use first type for overall stats
        logEvent(NeuralStateType::NeuronState, "load_all", name, result);
        
        NLM_LOG_INFO("All neural state types loaded successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception loading all neural state types: ") + e.what();
        logEvent(NeuralStateType::NeuronState, "load_all_exception", name, result);
        return false;
    }
}

bool NeuralStatePersistence::deleteNeuralStates(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Delete neural state files for all types
    for (int type = 0; type < 5; ++type) {
        NeuralStateType stateType = static_cast<NeuralStateType>(type);
        std::string filepath = getNeuralStatePath(stateType, name);
        
        if (std::filesystem::exists(filepath)) {
            if (std::remove(filepath.c_str()) != 0) {
                return false;
            }
        }
    }
    
    return true;
}

bool NeuralStatePersistence::validateConfiguration() const {
    validationErrors_.clear();
    
    if (!config_) {
        validationErrors_.push_back("Configuration is not set");
    }
    
    return validationErrors_.empty();
}

bool NeuralStatePersistence::validateNeuralStateIntegrity() const {
    validationErrors_.clear();
    
    if (!brain_) {
        validationErrors_.push_back("Brain is not set");
        return false;
    }
    
    // In real implementation, would validate brain neural state
    // For demonstration, just check if brain exists
    
    return validationErrors_.empty();
}

void NeuralStatePersistence::updateStats(const NeuralStatePersistenceResult& result, NeuralStateType type) {
    stats_.totalStatesSaved += 1; // Count as one saved operation
    stats_.totalSizeSaved += result.totalSizeSaved;
    stats_.totalTimeSpent += result.elapsedTime;
    
    if (!result.success) {
        stats_.errorCount++;
    }
    
    // Update specific state type stats
    switch (type) {
        case NeuralStateType::NeuronState:
            stats_.neuronStatesSaved++;
            break;
        case NeuralStateType::SynapseState:
            stats_.synapseStatesSaved++;
            break;
        case NeuralStateType::ConnectivityState:
            stats_.connectivityStatesSaved++;
            break;
        case NeuralStateType::PlasticityState:
            stats_.plasticityStatesSaved++;
            break;
        case NeuralStateType::SpikeHistoryState:
            stats_.spikeHistoryStatesSaved++;
            break;
    }
}

void NeuralStatePersistence::logEvent(NeuralStateType type, const std::string& operation,
                                       const std::string& name, const NeuralStatePersistenceResult& result) {
    std::stringstream logMsg;
    logMsg << "[NeuralStatePersistence] " << operation << " - State Type: " << 
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
    if (neuralStateEventCallback_) {
        neuralStateEventCallback_(type, name, result);
    }
}

NeuralStatePersistenceResult NeuralStatePersistence::executeNeuralStateOperation(NeuralStateType type,
                                                                                const std::string& operation,
                                                                                const std::string& name,
                                                                                const NeuralStatePersistenceOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    NeuralStatePersistenceResult result;
    result.success = false;
    
    try {
        if (!validateConfiguration()) {
            errorCount_++;
            result.message = "Invalid configuration";
            logEvent(type, operation + "_failed", name, result);
            return result;
        }
        
        if (!brain_) {
            errorCount_++;
            result.message = "Brain is not configured";
            logEvent(type, operation + "_failed", name, result);
            return result;
        }
        
        bool success = false;
        
        if (operation == "save") {
            success = saveNeuralStateType(type, name, options);
        } else if (operation == "load") {
            success = loadNeuralStateType(type, name);
        }
        
        if (!success) {
            errorCount_++;
            result.message = "Neural state operation failed: " + operation;
            logEvent(type, operation + "_failed", name, result);
            return result;
        }
        
        result.success = true;
        result.message = "Neural state " + operation + "d successfully";
        
        logEvent(type, operation, name, result);
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception in neural state operation: ") + e.what();
        logEvent(type, operation + "_exception", name, result);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    result.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    return result;
}

bool NeuralStatePersistence::saveNeuralStateType(NeuralStateType type, const std::string& name,
                                                 const NeuralStatePersistenceOptions& options) {
    std::string filepath = getNeuralStatePath(type, name);
    
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
        
        // In real implementation, would serialize actual neural state data from brain_
        // For demonstration, just write placeholder data
        
        // Write placeholder state count
        uint64_t stateCount = 1000; // Placeholder
        file.write(reinterpret_cast<const char*>(&stateCount), sizeof(stateCount));
        
        // Write placeholder state data
        for (uint64_t i = 0; i < stateCount; ++i) {
            float state = static_cast<float>(i) * 0.1f; // Placeholder
            file.write(reinterpret_cast<const char*>(&state), sizeof(float));
        }
        
        file.flush();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving neural state type: ") + e.what());
        return false;
    }
}

bool NeuralStatePersistence::loadNeuralStateType(NeuralStateType type, const std::string& name) {
    std::string filepath = getNeuralStatePath(type, name);
    
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open neural state file: " + filepath);
            return false;
        }
        
        // Read type marker
        uint32_t typeMarker = 0;
        file.read(reinterpret_cast<char*>(&typeMarker), sizeof(typeMarker));
        
        if (typeMarker != static_cast<uint32_t>(type)) {
            NLM_LOG_ERROR("Mismatch neural state type in file: " + filepath);
            return false;
        }
        
        // Read options
        uint8_t compressFlag, encryptFlag;
        file.read(reinterpret_cast<char*>(&compressFlag), 1);
        file.read(reinterpret_cast<char*>(&encryptFlag), 1);
        
        // In real implementation, would deserialize actual neural state data into brain_
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
        NLM_LOG_ERROR(std::string("Exception loading neural state type: ") + e.what());
        return false;
    }
}

std::string NeuralStatePersistence::getNeuralStatePath(NeuralStateType type, const std::string& name) const {
    std::string typeStr;
    switch (type) {
        case NeuralStateType::NeuronState: return "data/neural/neuron_state_" + name + ".dat";
        case NeuralStateType::SynapseState: return "data/neural/synapse_state_" + name + ".dat";
        case NeuralStateType::ConnectivityState: return "data/neural/connectivity_state_" + name + ".dat";
        case NeuralStateType::PlasticityState: return "data/neural/plasticity_state_" + name + ".dat";
        case NeuralStateType::SpikeHistoryState: return "data/neural/spike_history_state_" + name + ".dat";
        default: return "data/neural/unknown_state_" + name + ".dat";
    }
}

} // namespace nlm
