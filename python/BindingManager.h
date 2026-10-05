#pragma once

#include "BindingManager.h"
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
#include <iostream>

namespace py = pybind11;

namespace nlm {
    
    // Main BindingManager class that orchestrates all Python bindings
    class BindingManager {
    public:
        BindingManager() {
            std::cout << "Initializing NLM Binding Manager..." << std::endl;
            initializeModules();
        }
        
        ~BindingManager() {
            shutdownModules();
            std::cout << "NLM Binding Manager shutdown complete." << std::endl;
        }
        
        // Module management
        bool loadModule(const std::string& moduleName) {
            auto it = moduleRegistry_.find(moduleName);
            if (it == moduleRegistry_.end()) {
                std::cerr << "Error: Module not found: " << moduleName << std::endl;
                return false;
            }
            
            auto module = it->second;
            if (loadedModules_.find(moduleName) != loadedModules_.end()) {
                std::cout << "Module already loaded: " << moduleName << std::endl;
                return true;
            }
            
            // Register the module with pybind11
            try {
                module->registerBindings(pybind11::module_::import("nlm"));
                loadedModules_[moduleName] = module;
                module->initialize();
                std::cout << "Module loaded successfully: " << moduleName << std::endl;
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error loading module " << moduleName << ": " << e.what() << std::endl;
                return false;
            }
        }
        
        bool unloadModule(const std::string& moduleName) {
            auto it = loadedModules_.find(moduleName);
            if (it == loadedModules_.end()) {
                std::cout << "Module not loaded: " << moduleName << std::endl;
                return false;
            }
            
            auto module = it->second;
            module->shutdown();
            loadedModules_.erase(it);
            std::cout << "Module unloaded successfully: " << moduleName << std::endl;
            return true;
        }
        
        std::vector<std::string> getLoadedModules() const {
            std::vector<std::string> loaded;
            loaded.reserve(loadedModules_.size());
            for (const auto& pair : loadedModules_) {
                loaded.push_back(pair.first);
            }
            return loaded;
        }
        
        std::vector<std::string> getAvailableModules() const {
            std::vector<std::string> available;
            available.reserve(moduleRegistry_.size());
            for (const auto& pair : moduleRegistry_) {
                available.push_back(pair.first);
            }
            return available;
        }
        
        // Module information
        bool isModuleLoaded(const std::string& moduleName) const {
            return loadedModules_.find(moduleName) != loadedModules_.end();
        }
        
        bool isModuleAvailable(const std::string& moduleName) const {
            return moduleRegistry_.find(moduleName) != moduleRegistry_.end();
        }
        
        std::string getModuleDescription(const std::string& moduleName) const {
            auto it = moduleRegistry_.find(moduleName);
            if (it != moduleRegistry_.end()) {
                return it->second->getDescription();
            }
            throw std::runtime_error("Module not found: " + moduleName);
        }
        
        std::string getModuleVersion(const std::string& moduleName) const {
            auto it = moduleRegistry_.find(moduleName);
            if (it != moduleRegistry_.end()) {
                return it->second->getVersion();
            }
            throw std::runtime_error("Module not found: " + moduleName);
        }
        
        // Feature management
        bool hasFeature(const std::string& featureName) const {
            return features_.find(featureName) != features_.end();
        }
        
        py::object getFeature(const std::string& featureName) {
            auto it = features_.find(featureName);
            if (it != features_.end()) {
                return it->second;
            }
            throw std::runtime_error("Feature not found: " + featureName);
        }
        
        std::vector<std::string> getAvailableFeatures() const {
            std::vector<std::string> features;
            features.reserve(features_.size());
            for (const auto& pair : features_) {
                features.push_back(pair.first);
            }
            return features;
        }
        
        // Module registration
        void registerModule(std::shared_ptr<BindingModule> module) {
            moduleRegistry_[module->getName()] = module;
            std::cout << "Registered module: " << module->getName() << std::endl;
        }
        
        // Advanced features
        py::object createBrain(const std::string& configPath = "") {
            // Create and configure a brain
            auto config = std::make_shared<Config>();
            if (!configPath.empty()) {
                config->loadFromFile(configPath);
            }
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return py::cast(brain);
        }
        
        py::object createExperimentRunner() {
            // Create experiment runner
            return py::cast(std::make_shared<ExperimentRunner>());
        }
        
        py::object createWorldSimulation(const std::string& configPath = "") {
            // Create world simulation
            auto world = std::make_shared<SimpleWorld>();
            if (!configPath.empty()) {
                auto config = std::make_shared<Config>();
                config->loadFromFile(configPath);
                // Apply configuration to world
            }
            return py::cast(world);
        }
        
        // Utilities
        void printModuleStatus() const {
            std::cout << "=== NLM Binding Module Status ===" << std::endl;
            std::cout << "Total registered modules: " << moduleRegistry_.size() << std::endl;
            std::cout << "Total loaded modules: " << loadedModules_.size() << std::endl;
            std::cout << "Available modules:" << std::endl;
            for (const auto& pair : moduleRegistry_) {
                std::cout << "  - " << pair.first 
                          << " (v" << pair.second->getVersion() << ") "
                          << pair.second->getDescription() << std::endl;
            }
            std::cout << "Loaded modules:" << std::endl;
            for (const auto& pair : loadedModules_) {
                std::cout << "  - " << pair.first << std::endl;
            }
            std::cout << "Available features:" << std::endl;
            for (const auto& feature : getAvailableFeatures()) {
                std::cout << "  - " << feature << std::endl;
            }
            std::cout << "=== Status Complete ===" << std::endl;
        }
        
        // Python-specific initialization
        static void initializePythonBindings() {
            // Initialize all Python bindings when the module is imported
            static BindingManager manager;
        }
        
    private:
        void initializeModules() {
            // Initialize all core modules
            // This would be called automatically when the binding manager is constructed
        }
        
        void shutdownModules() {
            // Shutdown all loaded modules
            for (auto& pair : loadedModules_) {
                pair.second->shutdown();
            }
            loadedModules_.clear();
        }
        
        std::unordered_map<std::string, std::shared_ptr<BindingModule>> moduleRegistry_;
        std::unordered_map<std::string, std::shared_ptr<BindingModule>> loadedModules_;
        std::unordered_map<std::string, py::object> features_;
    };
    
    // Convenience function for Python users
    inline BindingManager& getBindingManager() {
        static BindingManager manager;
        return manager;
    }
}