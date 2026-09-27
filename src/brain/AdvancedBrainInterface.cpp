#include "AdvancedBrainInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "ExtensionSystem.hpp"
#include "ExtensionManager.hpp"
#include "factory/ComponentFactory.hpp"
#include "commands/AdvancedBrainCommands.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

namespace {
    // Helper function to normalize path
    std::string normalizePath(const std::string& path) {
        std::filesystem::path fsPath(path);
        return fsPath.lexically_normal().string();
    }
    
    // Check if directory exists
    bool directoryExists(const std::string& path) {
        std::filesystem::path fsPath(normalizePath(path));
        return std::filesystem::is_directory(fsPath);
    }
}

bool AdvancedBrainInterface::initialize(const Config& config, std::shared_ptr<Config> globalConfig, std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (isInitialized_) {
        NLM_LOG_WARNING("AdvancedBrainInterface already initialized");
        return true;
    }
    
    config_ = config;
    globalConfig_ = globalConfig;
    brain_ = brain;
    
    try {
        // Initialize components
        if (config_.enableExtensionSystem) {
            if (!initializeExtensionSystem()) {
                initError_ = "Failed to initialize extension system";
                return false;
            }
        }
        
        if (config_.enableExtensionManager) {
            if (!initializeExtensionManager()) {
                initError_ = "Failed to initialize extension manager";
                return false;
            }
        }
        
        if (config_.enableAdvancedCommands) {
            if (!initializeCommandRegistry()) {
                initError_ = "Failed to initialize command registry";
                return false;
            }
        }
        
        if (config_.enableComponentFactory) {
            if (!initializeComponentFactory()) {
                initError_ = "Failed to initialize component factory";
                return false;
            }
        }
        
        if (config_.registerExtensionPointHandlers()) {
            if (!registerExtensionPointHandlers()) {
                NLM_LOG_WARNING("Failed to register some extension point handlers");
            }
        }
        
        if (config_.autoLoadExtensions && config_.enablePluginArchitecture) {
            if (config_.extensionPath.empty() || !directoryExists(config_.extensionPath)) {
                NLM_LOG_INFO("Extension path not configured or doesn't exist: " + config_.extensionPath);
            } else {
                if (!loadExtensionsFromPath(config_.extensionPath)) {
                    NLM_LOG_WARNING("Failed to load some extensions from path: " + config_.extensionPath);
                }
            }
        }
        
        isInitialized_ = true;
        
        NLM_LOG_INFO("AdvancedBrainInterface initialized successfully");
        
        // Emit initialization event
        if (extensionSystem_) {
            extensionSystem_->emitEvent("brain_interface_initialized", std::any(), "advanced_brain_interface");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception initializing AdvancedBrainInterface: " + e.what()));
        isInitialized_ = false;
        initError_ = std::string("Exception: " + e.what());
        return false;
    }
}

void AdvancedBrainInterface::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!isInitialized_) {
        return;
    }
    
    try {
        // Emit shutdown event
        if (extensionSystem_) {
            extensionSystem_->emitEvent("brain_interface_shutdown", std::any(), "advanced_brain_interface");
        }
        
        // Clear extension listeners
        extensionListeners_.clear();
        
        // Shutdown components
        if (extensionSystem_) {
            extensionSystem_->shutdown();
            extensionSystem_.reset();
        }
        
        if (extensionManager_) {
            extensionManager_->clearAllExtensions();
            extensionManager_.reset();
        }
        
        if (commandRegistry_) {
            commandRegistry_->clear();
            commandRegistry_.reset();
        }
        
        if (componentFactory_) {
            componentFactory_.reset();
        }
        
        globalConfig_.reset();
        brain_.reset();
        
        isInitialized_ = false;
        initError_.clear();
        
        NLM_LOG_INFO("AdvancedBrainInterface shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception shutting down AdvancedBrainInterface: " + e.what()));
    }
}

bool AdvancedBrainInterface::addExtensionListener(std::unique_ptr<ExtensionListener> listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!listener) {
        NLM_LOG_ERROR("Cannot add null extension listener");
        return false;
    }
    
    extensionListeners_.push_back(std::move(listener));
    
    NLM_LOG_INFO("Added extension listener: " + extensionListeners_.back()->getName());
    
    return true;
}

bool AdvancedBrainInterface::removeExtensionListener(const std::string& listenerName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::remove_if(extensionListeners_.begin(), extensionListeners_.end(),
                             [&listenerName](const auto& listener) {
                                 return listener->getName() == listenerName;
                             });
    
    if (it != extensionListeners_.end()) {
        NLM_LOG_INFO("Removed extension listener: " + listenerName);
        extensionListeners_.erase(it, extensionListeners_.end());
        return true;
    }
    
    return false;
}

bool AdvancedBrainInterface::executeCommand(const std::string& name,
                                           const std::vector<std::string>& args,
                                           const std::unordered_map<std::string, std::any>& parameters) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!isInitialized_) {
        NLM_LOG_ERROR("AdvancedBrainInterface not initialized");
        return false;
    }
    
    if (!commandRegistry_) {
        NLM_LOG_ERROR("Command registry not initialized");
        return false;
    }
    
    return commandRegistry_->executeCommand(name, args, globalConfig_, brain_);
}

bool AdvancedBrainInterface::registerExtensionPointHandler(const std::string& pointName,
                                                         ExtensionPointHandler handler,
                                                         const std::string& handlerName,
                                                         int priority) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!extensionSystem_) {
        NLM_LOG_ERROR("Extension system not initialized");
        return false;
    }
    
    return extensionSystem_->registerExtensionPointHandler(pointName, handler, handlerName, priority);
}

bool AdvancedBrainInterface::emitEvent(const std::string& eventName,
                                      const std::any& data,
                                      const std::string& sourceExtension) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!isInitialized_) {
        NLM_LOG_ERROR("AdvancedBrainInterface not initialized");
        return false;
    }
    
    if (!extensionSystem_) {
        NLM_LOG_ERROR("Extension system not initialized");
        return false;
    }
    
    return extensionSystem_->emitEvent(eventName, data, sourceExtension);
}

bool AdvancedBrainInterface::loadExtensionsFromPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!extensionSystem_) {
        NLM_LOG_ERROR("Extension system not initialized");
        return false;
    }
    
    return extensionSystem_->loadExtensionsFromPath(path);
}

std::string AdvancedBrainInterface::getStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream status;
    status << "AdvancedBrainInterface Status:\n";
    status << "  Initialized: " << (isInitialized_ ? "Yes" : "No") << "\n";
    
    if (isInitialized_) {
        if (extensionSystem_) {
            status << "  Extension System: " << (extensionSystem_->getExtensionPoints().size()) << " extension points\n";
        }
        
        if (extensionManager_) {
            status << "  Extension Manager: " << extensionManager_->getExtensionCount() << " extensions loaded\n";
        }
        
        if (commandRegistry_) {
            status << "  Command Registry: " << commandRegistry_->getCommandCount() << " commands registered\n";
        }
        
        status << "  Extension Listeners: " << extensionListeners_.size() << "\n";
        
        if (config_.enableExtensionSystem && extensionSystem_) {
            status << "  Extension Points: " << extensionSystem_->getExtensionPoints().size() << "\n";
        }
        
        if (config_.enableEventSystem && extensionSystem_) {
            status << "  Events: " << extensionSystem_->getEvents().size() << "\n";
        }
    } else {
        if (!initError_.empty()) {
            status << "  Error: " << initError_ << "\n";
        }
    }
    
    return status.str();
}

bool AdvancedBrainInterface::initializeExtensionSystem() {
    ExtensionSystem::Config extConfig;
    extConfig.enableExtensionPoints = config_.enableExtensionSystem;
    extConfig.enableEventSystem = config_.enableEventSystem;
    extConfig.enablePluginArchitecture = config_.enablePluginArchitecture;
    extConfig.autoDiscoverExtensions = config_.autoLoadExtensions;
    extConfig.extensionPath = config_.extensionPath;
    extConfig.enableLogging = config_.enableLogging;
    extConfig.enableDebugging = config_.enableDebugMode;
    
    extensionSystem_ = std::make_unique<ExtensionSystem>();
    return extensionSystem_->initialize(extConfig, globalConfig_, brain_);
}

bool AdvancedBrainInterface::initializeExtensionManager() {
    extensionManager_ = std::make_unique<ExtensionManager>();
    return true;
}

bool AdvancedBrainInterface::initializeCommandRegistry() {
    commandRegistry_ = std::make_unique<AdvancedBrainCommandRegistry>();
    
    // Register advanced commands
    return registerAdvancedCommands();
}

bool AdvancedBrainInterface::initializeComponentFactory() {
    componentFactory_ = std::make_unique<ComponentFactory<void>>();
    return true;
}

bool AdvancedBrainInterface::registerExtensionPointHandlers() {
    if (!extensionSystem_) {
        return false;
    }
    
    bool success = true;
    
    // Register brain step extension point handler
    registerExtensionPointHandler("brain_step", 
        [this](const std::string& pointName, const std::any& data, const std::unordered_map<std::string, std::any>& params) {
            // Emit event for brain step
            emitEvent("brain_step_completed", data, "advanced_brain_interface");
            return true;
        }, "advanced_brain_interface", 100);
    
    // Register neuron spike extension point handler
    registerExtensionPointHandler("neuron_spike",
        [this](const std::string& pointName, const std::any& data, const std::unordered_map<std::string, std::any>& params) {
            // Emit event for neuron spike
            emitEvent("neuron_spike_detected", data, "advanced_brain_interface");
            return true;
        }, "advanced_brain_interface", 90);
    
    // Register extension loaded extension point handler
    registerExtensionPointHandler("extension_loaded",
        [this](const std::string& pointName, const std::any& data, const std::unordered_map<std::string, std::any>& params) {
            // Notify extension listeners
            notifyExtensionListeners("extension_loaded", "advanced_brain_interface");
            return true;
        }, "advanced_brain_interface", 80);
    
    return success;
}

void AdvancedBrainInterface::notifyExtensionListeners(const std::string& eventType,
                                                     const std::string& extensionName,
                                                     const std::string& message) {
    for (const auto& listener : extensionListeners_) {
        try {
            if (eventType == "extension_loaded") {
                listener->onExtensionLoaded(extensionName);
            } else if (eventType == "extension_unloaded") {
                listener->onExtensionUnloaded(extensionName);
            } else if (eventType == "extension_error") {
                listener->onExtensionError(extensionName, message);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in extension listener " + listener->getName() + ": " + e.what()));
        }
    }
}

} // namespace nlm
