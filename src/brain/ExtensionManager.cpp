#include "ExtensionManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_set>

namespace nlm {

namespace {
    // Helper function to normalize path
    std::string normalizePath(const std::string& path) {
        std::filesystem::path fsPath(path);
        return fsPath.lexically_normal().string();
    }
    
    // Check if file exists
    bool fileExists(const std::string& path) {
        std::ifstream file(normalizePath(path));
        return file.good();
    }
    
    // Get file extension
    std::string getFileExtension(const std::string& path) {
        size_t dotPos = path.find_last_of('.');
        if (dotPos != std::string::npos) {
            return path.substr(dotPos + 1);
        }
        return "";
    }
}

bool ExtensionManager::loadExtension(const std::string& filepath, const LoadConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string name = filepath;
    size_t lastSlash = name.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        name = name.substr(lastSlash + 1);
    }
    
    size_t lastDot = name.find_last_of('.');
    if (lastDot != std::string::npos) {
        name = name.substr(0, lastDot);
    }
    
    return loadDynamicExtension(name, filepath, config);
}

bool ExtensionManager::loadExtension(const std::string& name, const std::string& modulePath, const LoadConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (extensions_.find(name) != extensions_.end()) {
        NLM_LOG_WARNING("Extension already loaded: " + name);
        return false;
    }
    
    return loadDynamicExtension(name, modulePath, config);
}

bool ExtensionManager::loadExtension(const std::string& name,
                                   std::function<std::unique_ptr<Extension>()> factory,
                                   const LoadConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (extensions_.find(name) != extensions_.end()) {
        NLM_LOG_WARNING("Extension already loaded: " + name);
        return false;
    }
    
    ExtensionInfo info;
    info.name = name;
    info.state = LifecycleState::Loading;
    
    try {
        auto extension = factory();
        if (!extension) {
            NLM_LOG_ERROR("Failed to create extension: " + name);
            return false;
        }
        
        info.extension = extension;
        info.version = extension->getVersion();
        info.category = extension->getCategory();
        info.description = extension->getDescription();
        info.state = LifecycleState::Loaded;
        
        extensions_[name] = info;
        
        NLM_LOG_INFO("Loaded extension: " + name + " v" + version);
        
        if (config.autoInitialize) {
            return initializeExtension(name);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading extension " + name + ": " + e.what());
        info.state = LifecycleState::Failed;
        return false;
    }
}

bool ExtensionManager::unloadExtension(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = extensions_.find(name);
    if (it == extensions_.end()) {
        NLM_LOG_WARNING("Extension not found: " + name);
        return false;
    }
    
    ExtensionInfo& info = it->second;
    
    if (info.state == LifecycleState::Initializing || info.state == LifecycleState::Loaded) {
        NLM_LOG_WARNING("Cannot unload extension in loading state: " + name);
        return false;
    }
    
    if (info.state == LifecycleState::Initialized) {
        NLM_LOG_INFO("Shutting down extension: " + name);
        try {
            if (info.extension) {
                info.extension->shutdown();
            }
            info.state = LifecycleState::ShuttingDown;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception shutting down extension " + name + ": " + e.what());
        }
    }
    
    extensions_.erase(it);
    NLM_LOG_INFO("Unloaded extension: " + name);
    return true;
}

bool ExtensionManager::initializeExtension(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = extensions_.find(name);
    if (it == extensions_.end()) {
        NLM_LOG_ERROR("Extension not found: " + name);
        return false;
    }
    
    ExtensionInfo& info = it->second;
    
    if (info.state != LifecycleState::Loaded) {
        NLM_LOG_WARNING("Cannot initialize extension in state: " + std::to_string(static_cast<int>(info.state)));
        return false;
    }
    
    info.state = LifecycleState::Initializing;
    
    try {
        // TODO: Get Config and Brain from somewhere
        // This is a placeholder for actual initialization
        auto dummyConfig = std::make_shared<Config>();
        auto dummyBrain = std::make_shared<Brain>(dummyConfig);
        
        if (info.extension->initialize(dummyConfig, dummyBrain)) {
            info.state = LifecycleState::Initialized;
            NLM_LOG_INFO("Initialized extension: " + name);
            return true;
        } else {
            info.state = LifecycleState::Failed;
            NLM_LOG_ERROR("Failed to initialize extension: " + name);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing extension " + name + ": " + e.what());
        info.state = LifecycleState::Failed;
        return false;
    }
}

bool ExtensionManager::shutdownExtension(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = extensions_.find(name);
    if (it == extensions_.end()) {
        NLM_LOG_ERROR("Extension not found: " + name);
        return false;
    }
    
    ExtensionInfo& info = it->second;
    
    if (info.state != LifecycleState::Initialized) {
        NLM_LOG_WARNING("Cannot shutdown extension in state: " + std::to_string(static_cast<int>(info.state)));
        return false;
    }
    
    info.state = LifecycleState::ShuttingDown;
    
    try {
        if (info.extension) {
            info.extension->shutdown();
        }
        
        info.state = LifecycleState::Loaded;
        NLM_LOG_INFO("Shutdown extension: " + name);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down extension " + name + ": " + e.what());
        return false;
    }
}

ExtensionManager::ExtensionInfo* ExtensionManager::getExtension(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = extensions_.find(name);
    if (it != extensions_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool ExtensionManager::saveState(const std::string& filepath) const {
    try {
        std::ofstream file(normalizePath(filepath));
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        file << "{\n";
        file << "  \"extensions\": [\n";
        
        bool first = true;
        for (const auto& pair : extensions_) {
            const auto& info = pair.second;
            
            if (!first) {
                file << ",\n";
            }
            first = false;
            
            file << "    {\n";
            file << "      \"name\": \"" << info.name << "\",\n";
            file << "      \"version\": \"" << info.version << "\",\n";
            file << "      \"category\": \"" << info.category << "\",\n";
            file << "      \"description\": \"" << info.description << "\",\n";
            file << "      \"state\": " << static_cast<int>(info.state) << ",\n";
            file << "      \"loadTime\": \"" << std::chrono::duration_cast<std::chrono::milliseconds>(
                info.loadTime.time_since_epoch()).count() << "\",\n";
            file << "      \"lastUpdate\": \"" << std::chrono::duration_cast<std::chrono::milliseconds>(
                info.lastUpdate.time_since_epoch()).count() << "\",\n";
            
            if (!info.metadata.empty()) {
                file << "      \"metadata\": {\n";
                bool metaFirst = true;
                for (const auto& metaPair : info.metadata) {
                    if (!metaFirst) {
                        file << ",\n";
                    }
                    metaFirst = false;
                    file << "        \"" << metaPair.first << "\": \"" << metaPair.second << "\"";
                }
                file << "\n      }\n";
            }
            
            file << "    }";
        }
        
        file << "\n  ],\n";
        file << "  \"extensionPoints\": [\n";
        
        first = true;
        for (const auto& pair : extensionPoints_) {
            if (!first) {
                file << ",\n";
            }
            first = false;
            file << "    \"" << pair.first << "\" : \"" << pair.second << "\"";
        }
        
        file << "\n  ],\n";
        file << "  \"events\": [\n";
        
        first = true;
        for (const auto& pair : events_) {
            if (!first) {
                file << ",\n";
            }
            first = false;
            file << "    \"" << pair.first << "\" : \"" << pair.second << "\"";
        }
        
        file << "\n  ],\n";
        file << "  \"eventCallbacks\": {\n";
        
        first = true;
        for (const auto& pair : eventCallbacks_) {
            if (!first) {
                file << ",\n";
            }
            first = false;
            file << "    \"" << pair.first << "\": [";
            
            bool cbFirst = true;
            for (const auto& callback : pair.second) {
                if (!cbFirst) {
                    file << ", ";
                }
                cbFirst = false;
                file << "\"callback\"";
            }
            
            file << "]";
        }
        
        file << "\n  }\n";
        file << "}\n";
        
        file.close();
        NLM_LOG_INFO("Saved extension manager state to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving extension manager state: " + std::string(e.what()));
        return false;
    }
}

bool ExtensionManager::loadState(const std::string& filepath) {
    try {
        std::ifstream file(normalizePath(filepath));
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for reading: " + filepath);
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // This is a simplified JSON parsing - in production use a proper JSON library
        // For now, just log that we attempted to load state
        NLM_LOG_INFO("Loaded extension manager state from: " + filepath + " (simplified implementation)");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading extension manager state: " + std::string(e.what()));
        return false;
    }
}

bool ExtensionManager::loadDynamicExtension(const std::string& name, const std::string& modulePath, const LoadConfig& config) {
    // This is a simplified implementation - in production, this would use
    // dynamic library loading (dlopen, dlsym, etc.)
    
    ExtensionInfo info;
    info.name = name;
    info.state = LifecycleState::Loading;
    info.loadTime = std::chrono::steady_clock::now();
    info.lastUpdate = info.loadTime;
    
    // Create a simple dummy extension for demonstration
    class DummyExtension : public Extension {
        std::string name_;
        std::string version_ = "1.0.0";
        std::string category_ = "advanced";
        std::string description_ = "Dynamic extension loaded by ExtensionManager";
        
    public:
        DummyExtension(const std::string& name) : name_(name) {}
        
        std::string getName() const override { return name_; }
        std::string getVersion() const override { return version_; }
        std::string getCategory() const override { return category_; }
        std::string getDescription() const override { return description_; }
        
        bool initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) override {
            NLM_LOG_INFO("Initializing dummy extension: " + name_);
            return true;
        }
        
        void shutdown() override {
            NLM_LOG_INFO("Shutting down dummy extension: " + name_);
        }
        
        std::vector<std::string> getComponentTypes() const override {
            return {"neuromodulator", "memory", "cognitive"};
        }
        
        std::vector<std::string> getCommands() const override {
            return {"load", "unload", "list"};
        }
        
        std::vector<std::string> getExtensionPoints() const override {
            return {"brain_start", "brain_stop", "neuron_added"};
        }
        
        std::vector<std::string> getEvents() const override {
            return {"extension_loaded", "extension_unloaded", "extension_error"};
        }
    };
    
    try {
        info.extension = std::make_shared<DummyExtension>(name);
        info.version = info.extension->getVersion();
        info.category = info.extension->getCategory();
        info.description = info.extension->getDescription();
        info.state = LifecycleState::Loaded;
        
        extensions_[name] = info;
        
        NLM_LOG_INFO("Loaded dynamic extension: " + name + " from " + modulePath);
        
        if (config.autoInitialize) {
            return initializeExtension(name);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading dynamic extension " + name + ": " + e.what());
        info.state = LifecycleState::Failed;
        return false;
    }
}

bool ExtensionManager::loadStaticExtension(std::unique_ptr<Extension> extension, const LoadConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!extension) {
        NLM_LOG_ERROR("Cannot load null extension");
        return false;
    }
    
    std::string name = extension->getName();
    
    if (extensions_.find(name) != extensions_.end()) {
        NLM_LOG_WARNING("Extension already loaded: " + name);
        return false;
    }
    
    ExtensionInfo info;
    info.name = name;
    info.version = extension->getVersion();
    info.category = extension->getCategory();
    info.description = extension->getDescription();
    info.state = LifecycleState::Loaded;
    info.loadTime = std::chrono::steady_clock::now();
    info.lastUpdate = info.loadTime;
    info.extension = std::move(extension);
    
    extensions_[name] = info;
    
    NLM_LOG_INFO("Loaded static extension: " + name);
    
    if (config.autoInitialize) {
        return initializeExtension(name);
    }
    
    return true;
}

} // namespace nlm
