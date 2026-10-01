// PluginRegistry.h
// Plugin discovery and registration system

#pragma once

#include "PluginInterface.h"
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <filesystem>

namespace nlm {

    // Plugin version information
    struct Version {
        int major;
        int minor;
        int patch;
        
        Version(int major = 0, int minor = 0, int patch = 0) : major(major), minor(minor), patch(patch) {}
        
        bool operator==(const Version& other) const {
            return major == other.major && minor == other.minor && patch == other.patch;
        }
        
        bool operator!=(const Version& other) const {
            return !(*this == other);
        }
        
        bool operator<(const Version& other) const {
            if (major != other.major) return major < other.major;
            if (minor != other.minor) return minor < other.minor;
            return patch < other.patch;
        }
    };

    // Plugin symbol signatures for discovery
    namespace PluginSymbols {
        constexpr const char* FACTORY_FUNCTION = "create_plugin";
        constexpr const char* METADATA_FUNCTION = "get_plugin_metadata";
        constexpr const char* INFO_FUNCTION = "get_plugin_info";
        constexpr const char* VERSION_FUNCTION = "get_plugin_version";
        constexpr const char* CAPABILITIES_FUNCTION = "get_plugin_capabilities";
        constexpr const char* COMPATIBILITY_FUNCTION = "get_compatibility_info";
    }

    // Plugin registration info
    struct RegistrationInfo {
        std::string name;
        std::string path;
        Version version;
        PluginMetadata metadata;
        std::vector<std::string> dependencies;
        std::vector<PluginCapability> capabilities;
        std::string compatibleVersion;
        bool isPythonPlugin = false;
        std::string pythonModule;
        bool validated = false;
        std::string error;
    };

    // Plugin compatibility information
    struct CompatibilityInfo {
        std::string compatibleNLMVersion;
        std::vector<std::string> incompatibleVersions;
        bool isCompatible(const std::string& version) const {
            // Simple version comparison - in production, use proper semantic version comparison
            return compatibleNLMVersion.empty() || compatibleNLMVersion == version;
        }
    };

    // Plugin factory function signature
    using PluginFactory = std::function<std::shared_ptr<PluginInterface>()>;

    // Plugin discovery callback
    using DiscoveryCallback = std::function<void(const RegistrationInfo& info)>;

    // PluginRegistry class - handles plugin discovery, registration, and lifecycle
    class PluginRegistry {
    public:
        PluginRegistry();
        ~PluginRegistry();

        // Plugin discovery
        bool discoverPlugins(const std::filesystem::path& directory);
        bool discoverPlugins(const std::vector<std::filesystem::path>& directories);
        void scanDirectory(const std::filesystem::path& directory, bool recursive = true);
        
        // Plugin registration
        bool registerPlugin(const std::filesystem::path& pluginPath);
        bool registerPythonPlugin(const std::string& moduleName);
        bool unregisterPlugin(const std::string& name);
        
        // Plugin lookup
        std::shared_ptr<PluginInterface> getPlugin(const std::string& name) const;
        std::shared_ptr<PluginInterface> getPluginByCapability(PluginCapability capability) const;
        std::vector<std::shared_ptr<PluginInterface>> getPluginsByCategory(const std::string& category) const;
        std::vector<std::string> getPluginNames() const;
        std::vector<std::string> getPluginNamesByCapability(PluginCapability capability) const;
        
        // Plugin information
        const RegistrationInfo* getPluginInfo(const std::string& name) const;
        std::vector<RegistrationInfo> getAllPluginInfo() const;
        bool isPluginRegistered(const std::string& name) const;
        
        // Plugin validation and loading
        bool validatePlugin(const RegistrationInfo& info);
        bool validatePluginDependencies(const RegistrationInfo& info) const;
        bool checkVersionCompatibility(const RegistrationInfo& info) const;
        
        // Plugin management
        bool loadPlugin(const std::string& name);
        bool unloadPlugin(const std::string& name);
        bool configurePlugin(const std::string& name, const std::map<std::string, std::any>& config);
        bool initializePlugin(const std::string& name);
        bool startPlugin(const std::string& name);
        bool stopPlugin(const std::string& name);
        bool unloadPluginCompletely(const std::string& name);
        
        // Plugin state management
        PluginState getPluginState(const std::string& name) const;
        std::map<std::string, PluginState> getAllPluginStates() const;
        
        // Plugin events
        void addDiscoveryCallback(DiscoveryCallback callback);
        void removeDiscoveryCallback(const DiscoveryCallback& callback);
        void notifyPluginLoaded(const std::string& name);
        void notifyPluginUnloaded(const std::string& name);
        
        // Utility methods
        bool saveRegistry(const std::filesystem::path& filePath) const;
        bool loadRegistry(const std::filesystem::path& filePath);
        void clear();
        
        // Plugin search and filtering
        std::vector<std::string> searchPlugins(const std::string& query) const;
        std::vector<std::string> findPluginsByCategory(const std::string& category) const;
        std::vector<std::string> findPluginsByDependency(const std::string& dependency) const;
        
        // Plugin statistics
        size_t getPluginCount() const;
        size_t getLoadedPluginCount() const;
        std::map<std::string, std::string> getPluginStats() const;
        
        // Configuration
        void setPluginSearchPath(const std::vector<std::filesystem::path>& paths);
        const std::vector<std::filesystem::path>& getPluginSearchPath() const;
        
        // Security and validation
        bool validatePluginPath(const std::filesystem::path& path) const;
        bool validatePluginSecurity(const RegistrationInfo& info) const;
        
        // Thread safety
        void lock() const;
        void unlock() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        // Private helper methods
        bool loadDynamicLibrary(const std::filesystem::path& path, RegistrationInfo& info);
        bool loadPythonPlugin(const std::string& moduleName, RegistrationInfo& info);
        void extractPluginMetadata(void* handle, RegistrationInfo& info);
        void extractPluginVersion(void* handle, Version& version);
        void extractPluginCapabilities(void* handle, std::vector<PluginCapability>& capabilities);
        void extractCompatibilityInfo(void* handle, CompatibilityInfo& compatibility);
        std::shared_ptr<PluginInterface> createPluginInstance(const RegistrationInfo& info);
        void cleanupPlugin(void* handle);
        std::string generatePluginName(const std::string& path) const;
        bool checkDuplicateRegistration(const std::string& name) const;
        void updatePluginState(const std::string& name, PluginState state);
        void triggerEvent(const std::string& eventType, const std::string& pluginName, const std::any& data = nullptr);
    };

} // namespace nlm

// Plugin registration macro for C++ plugins
#define REGISTER_PLUGIN(PluginClass) \
    extern "C" NLM_API std::shared_ptr<nlm::PluginInterface> create_plugin() { \
        return std::make_shared<PluginClass>(); \
    } \
    extern "C" NLM_API nlm::PluginMetadata get_plugin_metadata() { \
        static PluginClass instance; \
        return instance.getMetadata(); \
    }

// Plugin info macro
#define PLUGIN_INFO(PluginClass, name, version, description, author) \
    namespace { \
        struct PluginRegistrar { \
            PluginRegistrar() { \
                nlm::PluginMetadata metadata; \
                metadata.name = name; \
                metadata.version = version; \
                metadata.description = description; \
                metadata.author = author; \
                /* Registration logic would go here */ \
            } \
        }; \
        PluginRegistrar registrar; \
    }

// Plugin compatibility macro
#define PLUGIN_COMPATIBILITY(minVersion) \
    extern "C" NLM_API const char* get_compatibility_info() { \
        static std::string compat = "nlm>=" #minVersion; \
        return compat.c_str(); \
    }