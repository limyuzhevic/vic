#pragma once

#include "PluginTemplateGenerator.hpp"

namespace nlm {

class PluginSystem {
public:
    static PluginSystem& getInstance() {
        static PluginSystem instance;
        return instance;
    }
    
    bool initialize(std::shared_ptr<Config> config) {
        if (initialized_) {
            return true;
        }
        
        config_ = config;
        pluginSystemImpl_ = std::make_unique<Impl>(config_);
        
        if (!pluginSystemImpl_->initialize()) {
            return false;
        }
        
        initialized_ = true;
        NLM_LOG_INFO("PluginSystem singleton initialized");
        return true;
    }
    
    static bool loadPlugin(const std::string& pluginPath) {
        if (!getInstance().pluginSystemImpl_) {
            NLM_LOG_ERROR("PluginSystem not initialized");
            return false;
        }
        
        return getInstance().pluginSystemImpl_->loadPlugin(pluginPath);
    }
    
    static bool unloadPlugin(const std::string& pluginName) {
        return getInstance().pluginSystemImpl_->unloadPlugin(pluginName);
    }
    
    static bool enablePlugin(const std::string& pluginName) {
        return getInstance().pluginSystemImpl_->enablePlugin(pluginName);
    }
    
    static bool disablePlugin(const std::string& pluginName) {
        return getInstance().pluginSystemImpl_->disablePlugin(pluginName);
    }
    
    static bool generatePluginTemplate(const std::string& templateType,
                                      const std::string& outputPath,
                                      const std::string& pluginName = "",
                                      const std::string& serviceName = "") {
        if (!getInstance().pluginSystemImpl_) {
            NLM_LOG_ERROR("PluginSystem not initialized");
            return false;
        }
        
        return getInstance().pluginSystemImpl_->generatePluginTemplate(templateType, outputPath);
    }
    
    static void setBrainReference(Brain* brain) {
        if (getInstance().pluginSystemImpl_) {
            getInstance().pluginSystemImpl_->setBrainReference(brain);
        }
    }
    
    static void logStatus() {
        NLM_LOG_INFO("=== PluginSystem Singleton Status ===");
        NLM_LOG_INFO(std::string("Initialized: ") + (getInstance().initialized_ ? "Yes" : "No"));
        
        if (getInstance().pluginSystemImpl_) {
            getInstance().pluginSystemImpl_->logStatus();
        }
    }
    
private:
    PluginSystem() : initialized_(false) {}
    ~PluginSystem() {
        if (pluginSystemImpl_) {
            pluginSystemImpl_->shutdown();
        }
    }
    
    bool initialized_;
    std::shared_ptr<Config> config_;
    std::unique_ptr<Impl> pluginSystemImpl_;
};

} // namespace nlm
