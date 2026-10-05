#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

namespace py = pybind11;

namespace nlm {
    
    // Advanced Plugin System - Plugin Interface for extensibility
    class PluginInterface {
    public:
        virtual ~PluginInterface() = default;
        
        virtual std::string getName() const = 0;
        virtual std::string getVersion() const = 0;
        virtual std::string getDescription() const = 0;
        
        // Plugin lifecycle
        virtual void initialize() {}
        virtual void shutdown() {}
        
        // Feature system
        virtual bool hasFeature(const std::string& feature) const = 0;
        virtual py::object getFeature(const std::string& feature) = 0;
        
        // Dependency management
        virtual std::vector<std::string> getDependencies() const { return {}; }
        virtual bool validateDependencies() const { return true; }
        
        // Configuration
        virtual void configure(const py::dict& config) {}
        virtual py::dict getConfiguration() const { return py::dict(); }
        
        // Statistics
        virtual py::dict getStatistics() const { return py::dict(); }
        
        // Error handling
        virtual void handleError(const std::exception& error) {}
        
    protected:
        // Helper method for plugin discovery
        template<typename T>
        bool hasFeatureOfType() const {
            return hasFeature(typeid(T).name());
        }
        
        // Utility for feature validation
        bool validateFeature(const std::string& feature, const py::object& expectedType) {
            try {
                py::object featureObj = getFeature(feature);
                return py::isinstance(featureObj, expectedType);
            } catch (const std::exception& e) {
                return false;
            }
        }
    };
    
    // Dynamic module loader for runtime plugin loading
    class ModuleLoader {
    public:
        struct ModuleInfo {
            std::string name;
            std::string path;
            std::string version;
            std::string description;
            bool isLoaded;
            std::filesystem::file_time_type lastModified;
        };
        
        ModuleLoader() {
            scanModuleDirectory();
        }
        
        // Module discovery
        std::vector<ModuleInfo> discoverModules() const {
            std::vector<ModuleInfo> modules;
            for (const auto& pair : moduleCache_) {
                if (pair.second.isLoaded) {
                    modules.push_back(pair.second);
                }
            }
            return modules;
        }
        
        // Module loading
        std::shared_ptr<BindingModule> loadModule(const std::string& modulePath) {
            auto it = moduleCache_.find(modulePath);
            if (it == moduleCache_.end()) {
                std::cerr << "Error: Module not cached: " << modulePath << std::endl;
                return nullptr;
            }
            
            const ModuleInfo& info = it->second;
            if (info.isLoaded) {
                std::cout << "Module already loaded: " << info.name << std::endl;
                return nullptr;
            }
            
            try {
                // In a real implementation, this would load the shared library
                // and create a BindingModule instance
                std::cout << "Loading module: " << info.name 
                          << " from " << info.path << std::endl;
                
                // Placeholder for actual module loading
                // For now, return nullptr to indicate not implemented
                return nullptr;
                
            } catch (const std::exception& e) {
                std::cerr << "Error loading module " << modulePath << ": " << e.what() << std::endl;
                return nullptr;
            }
        }
        
        // Module unloading
        bool unloadModule(const std::string& modulePath) {
            auto it = moduleCache_.find(modulePath);
            if (it == moduleCache_.end()) {
                return false;
            }
            
            ModuleInfo& info = it->second;
            if (!info.isLoaded) {
                return false;
            }
            
            // Placeholder for actual module unloading
            info.isLoaded = false;
            std::cout << "Unloaded module: " << info.name << std::endl;
            return true;
        }
        
        // Module information
        bool isModuleLoaded(const std::string& modulePath) const {
            auto it = moduleCache_.find(modulePath);
            return it != moduleCache_.end() && it->second.isLoaded;
        }
        
        ModuleInfo getModuleInfo(const std::string& modulePath) const {
            auto it = moduleCache_.find(modulePath);
            if (it != moduleCache_.end()) {
                return it->second;
            }
            throw std::runtime_error("Module not found: " + modulePath);
        }
        
        // Module caching
        void cacheModule(const ModuleInfo& info) {
            moduleCache_[info.path] = info;
        }
        
        // Module directory scanning
        void scanModuleDirectory() {
            std::string moduleDir = "modules";
            if (!std::filesystem::exists(moduleDir)) {
                std::filesystem::create_directory(moduleDir);
                std::cout << "Created module directory: " << moduleDir << std::endl;
            }
            
            // Scan for Python modules (.so, .pyd files)
            for (const auto& entry : std::filesystem::directory_iterator(moduleDir)) {
                if (entry.is_regular_file()) {
                    std::string extension = entry.path().extension().string();
                    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                    
                    if (extension == ".so" || extension == ".dylib" || extension == ".pyd") {
                        ModuleInfo info;
                        info.path = entry.path().string();
                        info.name = entry.path().stem().string();
                        info.isLoaded = false;
                        info.version = "1.0.0"; // Default version
                        info.description = "Dynamically loaded module";
                        
                        if (std::filesystem::exists(entry.path())) {
                            info.lastModified = std::filesystem::last_write_time(entry.path());
                        }
                        
                        cacheModule(info);
                    }
                }
            }
            
            std::cout << "Discovered " << moduleCache_.size() << " modules in " << moduleDir << std::endl;
        }
        
        // Module validation
        bool validateModule(const ModuleInfo& info) const {
            // Placeholder for module validation logic
            return !info.path.empty() && 
                   std::filesystem::exists(info.path()) &&
                   !info.name.empty();
        }
        
        // Module statistics
        py::dict getModuleStatistics() const {
            py::dict stats;
            stats["total_modules"] = static_cast<int>(moduleCache_.size());
            
            int loadedCount = 0;
            for (const auto& pair : moduleCache_) {
                if (pair.second.isLoaded) {
                    loadedCount++;
                }
            }
            stats["loaded_modules"] = loadedCount;
            
            return stats;
        }
        
    private:
        std::unordered_map<std::string, ModuleInfo> moduleCache_;
    };
    
    // Centralized binding registry for plugin management
    class BindingRegistry {
    public:
        static BindingRegistry& getInstance() {
            static BindingRegistry instance;
            return instance;
        }
        
        // Module registration
        void registerModule(std::shared_ptr<BindingModule> module) {
            std::string name = module->getName();
            moduleRegistry_[name] = module;
            
            // Register module's features
            registerFeatures(module);
            
            std::cout << "Registered module: " << name 
                      << " (v" << module->getVersion() << ")" 
                      << " - " << module->getDescription() << std::endl;
        }
        
        std::shared_ptr<BindingModule> getModule(const std::string& name) {
            auto it = moduleRegistry_.find(name);
            if (it != moduleRegistry_.end()) {
                return it->second;
            }
            throw std::runtime_error("Module not found: " + name);
        }
        
        bool hasModule(const std::string& name) const {
            return moduleRegistry_.find(name) != moduleRegistry_.end();
        }
        
        std::vector<std::string> getModuleNames() const {
            std::vector<std::string> names;
            names.reserve(moduleRegistry_.size());
            for (const auto& pair : moduleRegistry_) {
                names.push_back(pair.first);
            }
            return names;
        }
        
        // Feature registration
        void registerFeature(const std::string& featureName, const py::object& feature) {
            features_[featureName] = feature;
        }
        
        py::object getFeature(const std::string& featureName) {
            auto it = features_.find(featureName);
            if (it != features_.end()) {
                return it->second;
            }
            throw std::runtime_error("Feature not found: " + featureName);
        }
        
        bool hasFeature(const std::string& featureName) const {
            return features_.find(featureName) != features_.end();
        }
        
        std::vector<std::string> getFeatureNames() const {
            std::vector<std::string> names;
            names.reserve(features_.size());
            for (const auto& pair : features_) {
                names.push_back(pair.first);
            }
            return names;
        }
        
        // Module lifecycle management
        void initializeAllModules() {
            std::cout << "Initializing all registered modules..." << std::endl;
            for (auto& pair : moduleRegistry_) {
                try {
                    pair.second->initialize();
                    std::cout << "Initialized module: " << pair.first << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Error initializing module " << pair.first << ": " << e.what() << std::endl;
                }
            }
        }
        
        void shutdownAllModules() {
            std::cout << "Shutting down all registered modules..." << std::endl;
            for (auto& pair : moduleRegistry_) {
                try {
                    pair.second->shutdown();
                    std::cout << "Shutdown module: " << pair.first << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Error shutting down module " << pair.first << ": " << e.what() << std::endl;
                }
            }
        }
        
        // Module statistics and diagnostics
        py::dict getRegistryStatistics() const {
            py::dict stats;
            stats["total_modules"] = static_cast<int>(moduleRegistry_.size());
            stats["total_features"] = static_cast<int>(features_.size());
            
            py::list moduleNames;
            for (const auto& pair : moduleRegistry_) {
                moduleNames.append(pair.first);
            }
            stats["module_names"] = moduleNames;
            
            py::list featureNames;
            for (const auto& pair : features_) {
                featureNames.append(pair.first);
            }
            stats["feature_names"] = featureNames;
            
            return stats;
        }
        
        // Configuration management
        void configureModules(const py::dict& config) {
            for (auto& pair : moduleRegistry_) {
                try {
                    // Extract module-specific configuration
                    py::dict moduleConfig;
                    // In a real implementation, this would parse nested configuration
                    pair.second->configure(moduleConfig);
                } catch (const std::exception& e) {
                    std::cerr << "Error configuring module " << pair.first << ": " << e.what() << std::endl;
                }
            }
        }
        
        // Error handling and recovery
        void handleModuleError(const std::string& moduleName, const std::exception& error) {
            auto it = moduleRegistry_.find(moduleName);
            if (it != moduleRegistry_.end()) {
                try {
                    it->second->handleError(error);
                } catch (const std::exception& e) {
                    std::cerr << "Error handling error for module " << moduleName << ": " << e.what() << std::endl;
                }
            }
        }
        
    private:
        BindingRegistry() = default;
        
        void registerFeatures(std::shared_ptr<BindingModule> module) {
            // Register module's features
            // In a real implementation, this would introspect the module's capabilities
            // and register them with the feature system
        }
        
        std::unordered_map<std::string, std::shared_ptr<BindingModule>> moduleRegistry_;
        std::unordered_map<std::string, py::object> features_;
    };
}