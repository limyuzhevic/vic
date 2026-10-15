#pragma once

#include "TestBase.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <dlfcn.h>

namespace nlm {

class TestPlugin {
public:
    virtual ~TestPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getContact() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Test creation
    virtual std::vector<std::unique_ptr<TestBase>> createTests() = 0;
    virtual bool canHandle(const std::string& testType) const = 0;
    
    // Plugin capabilities
    virtual std::vector<std::string> getSupportedTestTypes() const = 0;
    virtual std::vector<std::string> getSupportedCategories() const = 0;
    virtual std::vector<std::string> getSupportedPhases() const = 0;
    
    // Configuration
    virtual void configure(const std::map<std::string, std::string>& options) {}
    
    // Extension points
    virtual void onTestStart(const std::string& testName) {}
    virtual void onTestComplete(const std::string& testName, bool success) {}
    virtual void onTestFailure(const std::string& testName, const std::string& error) {}
};

// Dynamic plugin loader
class DynamicPlugin : public TestPlugin {
public:
    DynamicPlugin(const std::string& libraryPath);
    ~DynamicPlugin() override;
    
    bool initialize(const Config& config) override;
    void shutdown() override;
    
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    std::string getContact() const override;
    
    std::vector<std::unique_ptr<TestBase>> createTests() override;
    bool canHandle(const std::string& testType) const override;
    
    std::vector<std::string> getSupportedTestTypes() const override;
    std::vector<std::string> getSupportedCategories() const override;
    std::vector<std::string> getSupportedPhases() const override;
    
private:
    std::string libraryPath_;
    void* handle_;
    bool initialized_;
    
    // Function pointers for plugin interface
    using CreatePluginFunc = TestPlugin*(*)();
    using DestroyPluginFunc = void (*)(TestPlugin*);
    using InitializePluginFunc = bool (*)(TestPlugin*, const Config&);
    using GetTestsFunc = std::vector<std::unique_ptr<TestBase>> (*)(TestPlugin*);
    
    CreatePluginFunc createPlugin_;
    DestroyPluginFunc destroyPlugin_;
    InitializePluginFunc initializePlugin_;
    GetTestsFunc getTests_;
    
    void loadLibrary();
    void unloadLibrary();
};

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    static PluginManager& getInstance();
    
    // Plugin registration
    void registerPlugin(std::unique_ptr<TestPlugin> plugin);
    bool registerDynamicPlugin(const std::string& libraryPath);
    
    // Plugin discovery and loading
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getEnabledPlugins() const;
    bool isPluginEnabled(const std::string& pluginName) const;
    
    // Plugin management
    void enablePlugin(const std::string& pluginName);
    void disablePlugin(const std::string& pluginName);
    void enableAllPlugins();
    void disableAllPlugins();
    void reloadPlugin(const std::string& pluginName);
    void shutdownAllPlugins();
    
    // Test creation from plugins
    std::vector<std::unique_ptr<TestBase>> loadTests(const std::vector<std::string>& categories = {});
    std::vector<std::unique_ptr<TestBase>> loadTestsByPhase(const std::string& phase);
    std::vector<std::unique_ptr<TestBase>> loadTestsByType(const std::string& testType);
    
    // Plugin configuration
    void configurePlugin(const std::string& pluginName, const std::map<std::string, std::string>& options);
    std::map<std::string, std::string> getPluginConfig(const std::string& pluginName) const;
    
    // Plugin metadata
    TestPlugin* getPlugin(const std::string& pluginName);
    std::vector<TestPlugin*> getAllPlugins() const;
    
    // Utility methods
    size_t getPluginCount() const;
    bool hasPlugin(const std::string& pluginName) const;
    std::string getPluginPath(const std::string& pluginName) const;
    
private:
    PluginManager();
    ~PluginManager() = default;
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    std::vector<std::unique_ptr<TestPlugin>> plugins_;
    std::unordered_map<std::string, std::unique_ptr<TestPlugin>> pluginMap_;
    std::unordered_map<std::string, std::string> pluginPaths_;
    std::unordered_map<std::string, bool> pluginEnabled_;
    mutable std::mutex mutex_;
    
    // Plugin discovery
    void discoverBuiltInPlugins();
    void scanPluginDirectories();
    
    // Plugin loading
    void loadPlugin(const std::string& name, std::unique_ptr<TestPlugin> plugin);
    bool validatePlugin(TestPlugin* plugin);
};

// Plugin factory for creating plugins
class PluginFactory {
public:
    static std::unique_ptr<TestPlugin> createPlugin(const std::string& type, 
                                                    const std::map<std::string, std::string>& options = {});
    static std::vector<std::string> getRegisteredPluginTypes();
};

} // namespace nlm
