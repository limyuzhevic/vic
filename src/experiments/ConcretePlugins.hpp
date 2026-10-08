#pragma once

#include "../experiments/Experiment.hpp"
#include <map>
#include <vector>

namespace nlm {

// Concrete implementation of experiment plugin (e.g., data collection plugin)
class DataCollectionPlugin : public ExperimentPlugin {
public:
    DataCollectionPlugin() = default;
    ~DataCollectionPlugin() override = default;
    
    void initialize(const Experiment& experiment) override {
        experimentName_ = experiment.getName();
        // Initialize data collection
        dataCollected_ = false;
    }
    
    void process() override {
        // Simulate data collection from experiment
        dataCollected_ = true;
        // In real implementation, this would collect metrics, step data, etc.
    }
    
    const std::string& getName() const override {
        static const std::string name = "DataCollection";
        return name;
    }
    
    bool isActive() const override {
        return active_;
    }
    
    void setActive(bool active) override {
        active_ = active;
    }
    
private:
    std::string experimentName_;
    bool dataCollected_ = false;
    bool active_ = true;
};

// Concrete implementation of experiment plugin (e.g., analysis plugin)
class AnalysisPlugin : public ExperimentPlugin {
public:
    AnalysisPlugin() = default;
    ~AnalysisPlugin() override = default;
    
    void initialize(const Experiment& experiment) override {
        experimentName_ = experiment.getName();
        // Initialize analysis capabilities
        analysisDone_ = false;
    }
    
    void process() override {
        // Simulate analysis of experiment data
        analysisDone_ = true;
        // In real implementation, this would perform statistical analysis
    }
    
    const std::string& getName() const override {
        static const std::string name = "Analysis";
        return name;
    }
    
    bool isActive() const override {
        return active_;
    }
    
    void setActive(bool active) override {
        active_ = active;
    }
    
private:
    std::string experimentName_;
    bool analysisDone_ = false;
    bool active_ = true;
};

// Concrete implementation of experiment plugin (e.g., visualization plugin)
class VisualizationPlugin : public ExperimentPlugin {
public:
    VisualizationPlugin() = default;
    ~VisualizationPlugin() override = default;
    
    void initialize(const Experiment& experiment) override {
        experimentName_ = experiment.getName();
        // Initialize visualization capabilities
        visualizationsCreated_ = false;
    }
    
    void process() override {
        // Simulate creation of visualizations
        visualizationsCreated_ = true;
        // In real implementation, this would create plots, charts, etc.
    }
    
    const std::string& getName() const override {
        static const std::string name = "Visualization";
        return name;
    }
    
    bool isActive() const override {
        return active_;
    }
    
    void setActive(bool active) override {
        active_ = active;
    }
    
private:
    std::string experimentName_;
    bool visualizationsCreated_ = false;
    bool active_ = true;
};

// Concrete implementation of plugin factory
class DefaultPluginFactory : public PluginFactory {
public:
    DefaultPluginFactory() = default;
    ~DefaultPluginFactory() override = default;
    
    std::unique_ptr<ExperimentPlugin> createPlugin(
        const std::string& type,
        const std::vector<float>& parameters) override {
        if (type == "data_collection") {
            auto plugin = std::make_unique<DataCollectionPlugin>();
            // Apply parameters if any
            if (!parameters.empty()) {
                // In real implementation, would set plugin parameters
            }
            return plugin;
        } else if (type == "analysis") {
            auto plugin = std::make_unique<AnalysisPlugin>();
            return plugin;
        } else if (type == "visualization") {
            auto plugin = std::make_unique<VisualizationPlugin>();
            return plugin;
        }
        return nullptr;
    }
    
    std::vector<std::string> getAvailablePluginTypes() const override {
        return {"data_collection", "analysis", "visualization"};
    }
    
    std::vector<float> getDefaultParameters(const std::string& type) const override {
        if (type == "data_collection") {
            return {1.0f};  // Sample parameter
        } else if (type == "analysis") {
            return {1.0f, 0.5f};  // Multiple parameters
        } else if (type == "visualization") {
            return {1.0f, 0.5f, 0.8f};  // Multiple parameters
        }
        return {};
    }
};

// Concrete implementation of plugin manager
class SimplePluginManager : public PluginManager {
public:
    SimplePluginManager() = default;
    ~SimplePluginManager() override = default;
    
    void registerPluginFactory(std::unique_ptr<PluginFactory> factory) override {
        factories_.push_back(std::move(factory));
    }
    
    std::unique_ptr<ExperimentPlugin> createPlugin(
        const std::string& type,
        const std::vector<float>& parameters) override {
        for (const auto& factory : factories_) {
            std::unique_ptr<ExperimentPlugin> plugin = factory->createPlugin(type, parameters);
            if (plugin) {
                plugin->initialize(currentExperiment_);
                plugins_[plugin->getName()] = std::move(plugin);
                return std::move(plugins_[plugin->getName()]);
            }
        }
        return nullptr;
    }
    
    std::vector<std::string> getRegisteredPluginTypes() const override {
        std::vector<std::string> types;
        for (const auto& factory : factories_) {
            auto pluginTypes = factory->getAvailablePluginTypes();
            for (const auto& type : pluginTypes) {
                if (std::find(types.begin(), types.end(), type) == types.end()) {
                    types.push_back(type);
                }
            }
        }
        return types;
    }
    
    std::vector<ExperimentPlugin*> getPlugins() const override {
        std::vector<ExperimentPlugin*> pluginPointers;
        for (const auto& pair : plugins_) {
            pluginPointers.push_back(pair.second.get());
        }
        return pluginPointers;
    }
    
    bool setPluginActive(const std::string& pluginName, bool active) override {
        auto it = plugins_.find(pluginName);
        if (it != plugins_.end()) {
            it->second->setActive(active);
            return true;
        }
        return false;
    }
    
    bool removePlugin(const std::string& pluginName) override {
        auto it = plugins_.find(pluginName);
        if (it != plugins_.end()) {
            plugins_.erase(it);
            return true;
        }
        return false;
    }
    
    void clear() override {
        plugins_.clear();
        factories_.clear();
    }
    
    void setCurrentExperiment(const Experiment& experiment) {
        currentExperiment_ = experiment;
    }
    
private:
    std::map<std::string, std::unique_ptr<ExperimentPlugin>> plugins_;
    std::vector<std::unique_ptr<PluginFactory>> factories_;
    Experiment currentExperiment_;
};

// Concrete implementation of plugin loader (file-based)
class FilePluginLoader : public PluginLoader {
public:
    FilePluginLoader() = default;
    ~FilePluginLoader() override = default;
    
    bool loadFromFile(const std::string& filepath) override {
        // In real implementation, would load plugin from file
        // For now, simulate successful load
        loadedPlugins_.push_back(filepath);
        return true;
    }
    
    bool loadFromDirectory(const std::string& directory) override {
        // In real implementation, would scan directory for plugin files
        // For now, simulate successful load
        loadedPlugins_.push_back(directory);
        return true;
    }
    
    std::vector<std::string> getLoadedPluginNames() const override {
        return loadedPlugins_;
    }
    
    bool unloadPlugin(const std::string& pluginName) override {
        auto it = std::find(loadedPlugins_.begin(), loadedPlugins_.end(), pluginName);
        if (it != loadedPlugins_.end()) {
            loadedPlugins_.erase(it);
            return true;
        }
        return false;
    }
    
private:
    std::vector<std::string> loadedPlugins_;
};

} // namespace nlm
