#include "PluginSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace nlm {
    namespace {
        // Global plugin registry instance
        class PluginRegistryImpl {
        private:
            std::unordered_map<std::string, PluginFactory> factories;
            std::unordered_map<std::string, std::string> metadata;
            std::unordered_map<std::string, PluginType> pluginTypes;
            
        public:
            void registerPlugin(const std::string& name, 
                              PluginFactory factory,
                              const std::string& version,
                              const std::string& description,
                              PluginType type) {
                factories[name] = factory;
                metadata[name] = version;
                pluginTypes[name] = type;
                
                // Store description in metadata as a special entry
                metadata["desc_" + name] = description;
                
                NLM_LOG_INFO("Registered plugin: " + name + " (v" + version + ")");
            }
            
            std::unique_ptr<Plugin> createPlugin(const std::string& name) {
                auto it = factories.find(name);
                if (it == factories.end()) {
                    NLM_LOG_ERROR("Plugin not found: " + name);
                    return nullptr;
                }
                return it->second();
            }
            
            std::vector<std::string> getRegisteredPlugins() {
                std::vector<std::string> result;
                result.reserve(factories.size());
                for (const auto& pair : factories) {
                    result.push_back(pair.first);
                }
                return result;
            }
            
            std::string getPluginVersion(const std::string& name) {
                auto it = metadata.find(name);
                return it != metadata.end() ? it->second : "0.0.0";
            }
            
            std::string getPluginDescription(const std::string& name) {
                auto it = metadata.find("desc_" + name);
                return it != metadata.end() ? it->second : "";
            }
            
            PluginType getPluginType(const std::string& name) {
                auto it = pluginTypes.find(name);
                return it != pluginTypes.end() ? it->second : PluginType::UTILITY;
            }
            
            bool isPluginRegistered(const std::string& name) {
                return factories.find(name) != factories.end();
            }
        };
        
        PluginRegistryImpl& getRegistryInstance() {
            static PluginRegistryImpl instance;
            return instance;
        }
    }
    
    void PluginRegistry::registerPlugin(const std::string& name, 
                                      PluginFactory factory,
                                      const std::string& version,
                                      const std::string& description) {
        getRegistryInstance().registerPlugin(name, factory, version, description, PluginType::UTILITY);
    }
    
    std::unique_ptr<Plugin> PluginRegistry::createPlugin(const std::string& name) {
        return getRegistryInstance().createPlugin(name);
    }
    
    std::vector<std::string> PluginRegistry::getRegisteredPlugins() {
        return getRegistryInstance().getRegisteredPlugins();
    }
    
    std::string PluginRegistry::getPluginVersion(const std::string& name) {
        return getRegistryInstance().getPluginVersion(name);
    }
    
    std::string PluginRegistry::getPluginDescription(const std::string& name) {
        return getRegistryInstance().getPluginDescription(name);
    }
    
    PluginType PluginRegistry::getPluginType(const std::string& name) {
        return getRegistryInstance().getPluginType(name);
    }
    
    bool PluginRegistry::isPluginRegistered(const std::string& name) {
        return getRegistryInstance().isPluginRegistered(name);
    }
} // namespace nlm