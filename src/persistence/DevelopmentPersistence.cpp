// Development persistence implementation
#include "DevelopmentPersistence.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

DevelopmentPersistence::DevelopmentPersistence() : developmentSystem_(nullptr), config_(nullptr),
                                         errorCount_(0), stats_(), developmentEventCallback_(nullptr) {
}

DevelopmentPersistence::~DevelopmentPersistence() {
    shutdown();
}

bool DevelopmentPersistence::configure(const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        config_ = config;
        
        NLM_LOG_INFO("Development persistence configured successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Development persistence configuration failed: ") + e.what());
        return false;
    }
}

void DevelopmentPersistence::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    developmentSystem_.reset();
    
    NLM_LOG_INFO("Development persistence shutdown complete");
}

bool DevelopmentPersistence::saveDevelopmentalStage(const std::string& name, const DevelopmentPersistenceOptions& options) {
    return executeDevelopmentOperation(DevelopmentSystemComponent::DevelopmentalStage, "save", name, options);
}

bool DevelopmentPersistence::savePlasticityChanges(const std::string& name, const DevelopmentPersistenceOptions& options) {
    return executeDevelopmentOperation(DevelopmentSystemComponent::PlasticityChanges, "save", name, options);
}

bool DevelopmentPersistence::saveStructuralChanges(const std::string& name, const DevelopmentPersistenceOptions& options) {
    return executeDevelopmentOperation(DevelopmentSystemComponent::StructuralChanges, "save", name, options);
}

bool DevelopmentPersistence::saveMaturationState(const std::string& name, const DevelopmentPersistenceOptions& options) {
    return executeDevelopmentOperation(DevelopmentSystemComponent::MaturationState, "save", name, options);
}

bool DevelopmentPersistence::loadDevelopmentalStage(const std::string& name) {
    return executeDevelopmentOperation(DevelopmentSystemComponent::DevelopmentalStage, "load", name, DevelopmentPersistenceOptions());
}

bool DevelopmentPersistence::loadPlasticityChanges(const std::string& name) {
    return executeDevelopmentOperation(DevelopmentSystemComponent::PlasticityChanges, "load", name, DevelopmentPersistenceOptions());
}

bool DevelopmentPersistence::loadStructuralChanges(const std::string& name) {
    return executeDevelopmentOperation(DevelopmentSystemComponent::StructuralChanges, "load", name, DevelopmentPersistenceOptions());
}

bool DevelopmentPersistence::loadMaturationState(const std::string& name) {
    return executeDevelopmentOperation(DevelopmentSystemComponent::MaturationState, "load", name, DevelopmentPersistenceOptions());
}

bool DevelopmentPersistence::saveAllDevelopmentStates(const std::string& name, const DevelopmentPersistenceOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    DevelopmentPersistenceResult result;
    result.success = false;
    
    try {
        status_ = DevelopmentPersistenceStatus::Saving;
        
        // Save all development system components
        bool developmentalStageSuccess = saveDevelopmentSystemComponent("DevelopmentalStage", name, options);
        bool plasticityChangesSuccess = saveDevelopmentSystemComponent("PlasticityChanges", name, options);
        bool structuralChangesSuccess = saveDevelopmentSystemComponent("StructuralChanges", name, options);
        bool maturationStateSuccess = saveDevelopmentSystemComponent("MaturationState", name, options);
        
        if (!developmentalStageSuccess || !plasticityChangesSuccess || !structuralChangesSuccess || !maturationStateSuccess) {
            result.message = "Failed to save all development system components";
            logEvent("DevelopmentalStage", "save_all_failed", name, result);
            return false;
        }
        
        result.success = true;
        result.message = "All development system components saved successfully";
        
        updateStats(result, "DevelopmentalStage"); // Use first component for overall stats
        logEvent("DevelopmentalStage", "save_all", name, result);
        
        NLM_LOG_INFO("All development system components saved successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception saving all development system components: ") + e.what();
        logEvent("DevelopmentalStage", "save_all_exception", name, result);
        return false;
    }
}

bool DevelopmentPersistence::loadAllDevelopmentStates(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    DevelopmentPersistenceResult result;
    result.success = false;
    
    try {
        status_ = DevelopmentPersistenceStatus::Loading;
        
        // Load all development system components
        bool developmentalStageSuccess = loadDevelopmentSystemComponent("DevelopmentalStage", name);
        bool plasticityChangesSuccess = loadDevelopmentSystemComponent("PlasticityChanges", name);
        bool structuralChangesSuccess = loadDevelopmentSystemComponent("StructuralChanges", name);
        bool maturationStateSuccess = loadDevelopmentSystemComponent("MaturationState", name);
        
        if (!developmentalStageSuccess || !plasticityChangesSuccess || !structuralChangesSuccess || !maturationStateSuccess) {
            result.message = "Failed to load all development system components";
            logEvent("DevelopmentalStage", "load_all_failed", name, result);
            return false;
        }
        
        result.success = true;
        result.message = "All development system components loaded successfully";
        
        updateStats(result, "DevelopmentalStage"); // Use first component for overall stats
        logEvent("DevelopmentalStage", "load_all", name, result);
        
        NLM_LOG_INFO("All development system components loaded successfully: " + name);
        return true;
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception loading all development system components: ") + e.what();
        logEvent("DevelopmentalStage", "load_all_exception", name, result);
        return false;
    }
}

bool DevelopmentPersistence::deleteDevelopmentStates(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Delete development state files for all components
    std::vector<std::string> components = {"DevelopmentalStage", "PlasticityChanges", "StructuralChanges", "MaturationState"};
    for (const auto& component : components) {
        std::string filepath = getDevelopmentStatePath(component, name);
        
        if (std::filesystem::exists(filepath)) {
            if (std::remove(filepath.c_str()) != 0) {
                return false;
            }
        }
    }
    
    return true;
}

bool DevelopmentPersistence::validateConfiguration() const {
    validationErrors_.clear();
    
    if (!config_) {
        validationErrors_.push_back("Configuration is not set");
    }
    
    return validationErrors_.empty();
}

bool DevelopmentPersistence::validateDevelopmentIntegrity() const {
    validationErrors_.clear();
    
    if (!developmentSystem_) {
        validationErrors_.push_back("Development system is not set");
        return false;
    }
    
    // In real implementation, would validate development system state
    // For demonstration, just check if system exists
    
    return validationErrors_.empty();
}

void DevelopmentPersistence::updateStats(const DevelopmentPersistenceResult& result, DevelopmentSystemComponent component) {
    stats_.totalStagesSaved += result.totalStages;
    stats_.totalChangesSaved += result.totalChanges;
    stats_.totalSizeSaved += result.totalSizeSaved;
    stats_.totalTimeSpent += result.elapsedTime;
    
    if (!result.success) {
        stats_.errorCount++;
    }
    
    // Update specific component stats
    switch (component) {
        case DevelopmentSystemComponent::DevelopmentalStage:
            stats_.developmentalStagesSaved++;
            break;
        case DevelopmentSystemComponent::PlasticityChanges:
            stats_.plasticityChangesSaved++;
            break;
        case DevelopmentSystemComponent::StructuralChanges:
            stats_.structuralChangesSaved++;
            break;
        case DevelopmentSystemComponent::MaturationState:
            stats_.maturationStatesSaved++;
            break;
    }
}

void DevelopmentPersistence::logEvent(DevelopmentSystemComponent component, const std::string& operation,
                                       const std::string& name, const DevelopmentPersistenceResult& result) {
    std::stringstream logMsg;
    logMsg << "[DevelopmentPersistence] " << operation << " - Component: " << 
           static_cast<int>(component) << " - Name: " << name;
    
    if (!result.message.empty()) {
        logMsg << " - " << result.message;
    }
    
    if (result.success) {
        NLM_LOG_INFO(logMsg.str());
    } else {
        NLM_LOG_ERROR(logMsg.str());
    }
    
    // Call event callback if registered
    if (developmentEventCallback_) {
        developmentEventCallback_(component, name, result);
    }
}

DevelopmentPersistenceResult DevelopmentPersistence::executeDevelopmentOperation(DevelopmentSystemComponent component,
                                                                                  const std::string& operation,
                                                                                  const std::string& name,
                                                                                  const DevelopmentPersistenceOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    DevelopmentPersistenceResult result;
    result.success = false;
    
    try {
        if (!validateConfiguration()) {
            errorCount_++;
            result.message = "Invalid configuration";
            logEvent(component, operation + "_failed", name, result);
            return result;
        }
        
        if (!developmentSystem_) {
            errorCount_++;
            result.message = "Development system is not configured";
            logEvent(component, operation + "_failed", name, result);
            return result;
        }
        
        bool success = false;
        
        if (operation == "save") {
            success = saveDevelopmentSystemComponent(component, name, options);
        } else if (operation == "load") {
            success = loadDevelopmentSystemComponent(component, name);
        }
        
        if (!success) {
            errorCount_++;
            result.message = "Development operation failed: " + operation;
            logEvent(component, operation + "_failed", name, result);
            return result;
        }
        
        result.success = true;
        result.message = "Development system " + operation + "d successfully";
        
        logEvent(component, operation, name, result);
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception in development operation: ") + e.what();
        logEvent(component, operation + "_exception", name, result);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    result.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    return result;
}

bool DevelopmentPersistence::saveDevelopmentSystemComponent(DevelopmentSystemComponent component, const std::string& name,
                                                            const DevelopmentPersistenceOptions& options) {
    std::string filepath = getDevelopmentStatePath(component, name);
    
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Write component marker
        uint32_t componentMarker = static_cast<uint32_t>(component);
        file.write(reinterpret_cast<const char*>(&componentMarker), sizeof(componentMarker));
        
        // Write options
        uint8_t compressFlag = options.compress ? 1 : 0;
        uint8_t encryptFlag = options.encrypt ? 1 : 0;
        file.write(reinterpret_cast<const char*>(&compressFlag), 1);
        file.write(reinterpret_cast<const char*>(&encryptFlag), 1);
        
        // In real implementation, would serialize actual development system data from developmentSystem_
        // For demonstration, just write placeholder data
        
        // Write stage count
        uint64_t stageCount = 3; // Placeholder
        file.write(reinterpret_cast<const char*>(&stageCount), sizeof(stageCount));
        
        // Write placeholder stage data
        for (uint64_t i = 0; i < stageCount; ++i) {
            float stage = static_cast<float>(i) * 0.5f; // Placeholder
            file.write(reinterpret_cast<const char*>(&stage), sizeof(float));
        }
        
        file.flush();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving development system component: ") + e.what());
        return false;
    }
}

bool DevelopmentPersistence::loadDevelopmentSystemComponent(DevelopmentSystemComponent component, const std::string& name) {
    std::string filepath = getDevelopmentStatePath(component, name);
    
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open development system component file: " + filepath);
            return false;
        }
        
        // Read component marker
        uint32_t componentMarker = 0;
        file.read(reinterpret_cast<char*>(&componentMarker), sizeof(componentMarker));
        
        if (componentMarker != static_cast<uint32_t>(component)) {
            NLM_LOG_ERROR("Mismatch development system component type in file: " + filepath);
            return false;
        }
        
        // Read options
        uint8_t compressFlag, encryptFlag;
        file.read(reinterpret_cast<char*>(&compressFlag), 1);
        file.read(reinterpret_cast<char*>(&encryptFlag), 1);
        
        // In real implementation, would deserialize actual development system data into developmentSystem_
        // For demonstration, just read and ignore placeholder data
        
        // Read stage count
        uint64_t stageCount = 0;
        file.read(reinterpret_cast<char*>(&stageCount), sizeof(stageCount));
        
        // Skip placeholder stage data
        for (uint64_t i = 0; i < stageCount; ++i) {
            float stage = 0.0f;
            file.read(reinterpret_cast<char*>(&stage), sizeof(float));
        }
        
        file.close();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading development system component: ") + e.what());
        return false;
    }
}

std::string DevelopmentPersistence::getDevelopmentStatePath(DevelopmentSystemComponent component, const std::string& name) const {
    std::string componentStr;
    switch (component) {
        case DevelopmentSystemComponent::DevelopmentalStage: return "data/development/developmental_stage_" + name + ".dat";
        case DevelopmentSystemComponent::PlasticityChanges: return "data/development/plasticity_changes_" + name + ".dat";
        case DevelopmentSystemComponent::StructuralChanges: return "data/development/structural_changes_" + name + ".dat";
        case DevelopmentSystemComponent::MaturationState: return "data/development/maturation_state_" + name + ".dat";
        default: return "data/development/unknown_component_" + name + ".dat";
    }
}

} // namespace nlm
