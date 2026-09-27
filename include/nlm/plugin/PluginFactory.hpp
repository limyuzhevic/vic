#ifndef NLM_PLUGIN_PLUGINFACTORY_HPP
#define NLM_PLUGIN_PLUGINFACTORY_HPP

#include "PluginBase.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>

namespace nlm {
namespace plugin {

class PluginFactory {
public:
    PluginFactory();
    ~PluginFactory();
    
    // Plugin creation
    template<typename TPlugin, typename... Args>
    static std::unique_ptr<TPlugin> createPlugin(Args&&... args);
    
    template<typename TPlugin>
    static std::unique_ptr<TPlugin> createPlugin(const PluginMetadata& metadata);
    
    template<typename TPlugin>
    static std::unique_ptr<TPlugin> createPlugin(std::unique_ptr<IPlugin> plugin);
    
    // Plugin cloning
    template<typename TPlugin>
    static std::unique_ptr<TPlugin> clonePlugin(const std::string& name, const std::string& targetVersion);
    
    // Plugin instantiation
    static std::unique_ptr<IPlugin> instantiatePlugin(const std::string& name, const PluginMetadata& metadata);
    static std::unique_ptr<IPlugin> instantiatePlugin(const std::string& name, const std::string& path);
    
    // Plugin template loading
    static bool loadPluginTemplate(const std::string& templateName, const std::string& outputPath);
    static bool savePluginTemplate(const std::string& name, const std::string& templatePath);
    static std::vector<std::string> getAvailableTemplates() const;
    
    // Plugin factory registration
    template<typename TPlugin>
    static void registerPluginType(const std::string& name);
    
    static std::unique_ptr<IPlugin> createPluginByName(const std::string& name, const PluginMetadata& metadata);
    static std::string getPluginTypeName(const std::string& name);
    
    // Plugin validation
    static bool validatePluginFactory() const;
    static std::vector<std::string> getPluginFactoryErrors() const;
    
private:
    static std::unordered_map<std::string, std::function<std::unique_ptr<IPlugin>(const PluginMetadata&)>> pluginFactories_;
    static std::unordered_map<std::string, std::string> pluginTypeNames_;
    static mutable std::mutex mutex_;
};

// PluginBuilder for constructing plugins
class PluginBuilder {
public:
    PluginBuilder();
    ~PluginBuilder();
    
    // Basic plugin construction
    PluginBuilder& setName(const std::string& name);
    PluginBuilder& setVersion(const PluginVersion& version);
    PluginBuilder& setVersion(int major, int minor, int patch);
    PluginBuilder& setType(PluginType type);
    PluginBuilder& setDescription(const std::string& description);
    PluginBuilder& setAuthor(const std::string& author);
    PluginBuilder& setLicense(const std::string& license);
    
    // Plugin configuration
    PluginBuilder& addDependency(const std::string& dependency);
    PluginBuilder& addDependency(const std::string& name, const PluginVersion& version);
    PluginBuilder& addConfiguration(const std::string& key, const std::string& value);
    PluginBuilder& addConfiguration(const std::unordered_map<std::string, std::string>& config);
    PluginBuilder& addExtension(const std::string& name, void* extension);
    
    // Plugin lifecycle configuration
    PluginBuilder& setAutoInitialize(bool autoInitialize);
    PluginBuilder& setAutoActivate(bool autoActivate);
    PluginBuilder& setAutoShutdown(bool autoShutdown);
    PluginBuilder& setPluginThreadSafe(bool threadSafe);
    
    // Plugin security and isolation
    PluginBuilder& setSecurityPolicy(const std::string& policy);
    PluginBuilder& enableSandboxing(bool enabled);
    PluginBuilder& addResourceLimit(const std::string& resource, uint64_t limit);
    
    // Plugin composition
    PluginBuilder& addPrerequisite(const std::string& prerequisite);
    PluginBuilder& addConflict(const std::string& conflict);
    PluginBuilder& setPluginPriority(int priority);
    
    // Plugin validation
    PluginBuilder& setValidationEnabled(bool enabled);
    PluginBuilder& addValidationRule(const std::string& rule);
    
    // Plugin testing
    PluginBuilder& setTestEnabled(bool enabled);
    PluginBuilder& addTestCase(const std::string& testCase);
    
    // Build the plugin
    std::unique_ptr<IPlugin> build() const;
    template<typename TPlugin, typename... Args>
    std::unique_ptr<TPlugin> build(Args&&... args) const;
    
    // Reset builder state
    void reset();
    
    // Plugin factory methods
    static PluginBuilder createNeuronPlugin(const std::string& name);
    static PluginBuilder createSynapsePlugin(const std::string& name);
    static PluginBuilder createMemoryPlugin(const std::string& name);
    static PluginBuilder createPlasticityPlugin(const std::string& name);
    static PluginBuilder createWorldPlugin(const std::string& name);
    static PluginBuilder createAnalysisPlugin(const std::string& name);
    static PluginBuilder createConfigurationPlugin(const std::string& name);
    static PluginBuilder createPerformancePlugin(const std::string& name);
    
private:
    PluginMetadata metadata_;
    std::unordered_map<std::string, void*> extensions_;
    std::vector<std::string> dependencies_;
    std::vector<std::string> prerequisites_;
    std::vector<std::string> conflicts_;
    std::unordered_map<std::string, std::string> configuration_;
    std::unordered_map<std::string, uint64_t> resourceLimits_;
    std::vector<std::string> validationRules_;
    std::vector<std::string> testCases_;
    
    bool autoInitialize_;
    bool autoActivate_;
    bool autoShutdown_;
    bool threadSafe_;
    bool sandboxingEnabled_;
    bool validationEnabled_;
    bool testEnabled_;
    std::string securityPolicy_;
    int priority_;
};

// PluginComposer for combining multiple plugins
class PluginComposer {
public:
    PluginComposer();
    ~PluginComposer();
    
    // Plugin composition
    bool composePlugins(const std::vector<std::string>& pluginNames, const std::string& composedName);
    bool composePlugin(const std::string& pluginName, const std::string& compositionName);
    bool decomposePlugin(const std::string& composedPluginName, const std::string& componentPluginName);
    
    // Plugin nesting
    bool nestPlugin(const std::string& parentName, const std::string& childName);
    bool unnestPlugin(const std::string& parentName, const std::string& childName);
    std::vector<std::string> getPluginChildren(const std::string& parentName) const;
    std::string getPluginParent(const std::string& childName) const;
    
    // Plugin inheritance
    bool setPluginInheritance(const std::string& childName, const std::string& parentName);
    std::vector<std::string> getPluginInheritanceChain(const std::string& name) const;
    
    // Plugin combination
    bool combinePlugins(const std::vector<std::string>& pluginNames, const std::string& combinedName);
    bool splitPlugin(const std::string& combinedName, std::vector<std::string>& componentNames);
    
    // Plugin validation
    bool validateComposition(const std::vector<std::string>& pluginNames) const;
    bool validateNesting(const std::string& parentName, const std::string& childName) const;
    bool validateInheritance(const std::string& childName, const std::string& parentName) const;
    
    // Plugin lifecycle
    bool enableComposition(const std::string& composedName);
    bool disableComposition(const std::string& composedName);
    bool activateComposition(const std::string& composedName);
    bool deactivateComposition(const std::string& composedName);
    
    // Plugin configuration
    bool configureComposition(const std::string& composedName, const std::unordered_map<std::string, std::string>& config);
    std::unordered_map<std::string, std::string> getCompositionConfig(const std::string& composedName) const;
    
    // Plugin statistics
    struct CompositionStats {
        std::string composedName;
        std::vector<std::string> componentNames;
        uint64_t loadCount = 0;
        uint64_t activateCount = 0;
        uint64_t errorCount = 0;
        std::chrono::duration<double> averageCompositionTime;
        std::string lastError;
    };
    
    std::vector<CompositionStats> getAllCompositionStats() const;
    CompositionStats getCompositionStats(const std::string& composedName) const;
    
    // Plugin validation and optimization
    bool optimizeComposition(const std::string& composedName);
    bool validateCompositionConstraints(const std::string& composedName) const;
    std::vector<std::string> getCompositionWarnings(const std::string& composedName) const;
    
private:
    mutable std::shared_mutex mutex_;
    
    struct Composition {
        std::string name;
        std::vector<std::string> componentNames;
        std::vector<std::string> inheritanceChain;
        std::unordered_map<std::string, std::string> config;
        bool enabled;
        CompositionStats stats;
    };
    
    std::unordered_map<std::string, Composition> compositions_;
    std::unordered_map<std::string, std::vector<std::string>> pluginToCompositions_;
    std::unordered_map<std::string, std::vector<std::string>> compositionToPlugins_;
    
    bool validatePluginAvailability(const std::vector<std::string>& pluginNames) const;
    bool checkCompositionConstraints(const Composition& composition) const;
};

// PluginTemplate for plugin templates
class PluginTemplate {
public:
    PluginTemplate();
    ~PluginTemplate();
    
    // Template creation
    bool createTemplate(const std::string& name, const PluginMetadata& baseMetadata);
    bool createTemplateFromPlugin(const std::string& templateName, const std::string& pluginName);
    
    // Template management
    bool updateTemplate(const std::string& name, const PluginMetadata& newMetadata);
    bool deleteTemplate(const std::string& name);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Template configuration
    bool configureTemplate(const std::string& name, const std::unordered_map<std::string, std::string>& config);
    std::unordered_map<std::string, std::string> getTemplateConfig(const std::string& name) const;
    
    // Template instantiation
    std::unique_ptr<IPlugin> instantiateTemplate(const std::string& name, const std::string& instanceName);
    bool saveTemplateInstance(const std::string& templateName, const std::string& instanceName);
    std::vector<std::string> getTemplateInstances(const std::string& name) const;
    
    // Template validation
    bool validateTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateErrors(const std::string& name) const;
    
private:
    mutable std::shared_mutex mutex_;
    
    struct Template {
        std::string name;
        PluginMetadata metadata;
        std::unordered_map<std::string, std::string> config;
        std::vector<std::string> instances;
        bool enabled;
    };
    
    std::unordered_map<std::string, Template> templates_;
    std::unordered_map<std::string, std::vector<std::string>> templateToInstances_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINFACTORY_HPP