#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <mutex>
#include <atomic>
#include "../core/Types/Types.hpp"

namespace nlm {

// Plugin version information
struct PluginVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    std::string build;
    
    PluginVersion(uint32_t maj = 1, uint32_t min = 0, uint32_t pat = 0, const std::string& bld = "")
        : major(maj), minor(min), patch(pat), build(bld) {}
    
    bool operator==(const PluginVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool operator<(const PluginVersion& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
};

// Plugin status enumeration
enum class PluginStatus {
    Inactive,
    Loading,
    Loaded,
    Enabled,
    Disabled,
    Unloading,
    Failed,
    Uninstalled
};

// Plugin type enumeration
enum class PluginType {
    Neuron,
    Plasticity,
    Memory,
    Neuromodulator,
    Sensory,
    Motor,
    Environment,
    Cognitive,
    General
};

// Plugin capability flags
struct PluginCapabilities {
    bool supportsDynamicLoading = true;
    bool supportsRuntimeManagement = true;
    bool supportsConfiguration = true;
    bool requiresSandboxing = false;
    bool threadSafe = false;
    bool supportsHotReload = false;
    float performanceImpact = 0.0f;  // 0.0 to 1.0 scale
};

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string description;
    std::string author;
    PluginVersion version;
    PluginType type;
    PluginCapabilities capabilities;
    std::vector<std::string> dependencies;
    std::string license;
    std::string website;
    std::string documentation;
    
    PluginMetadata(const std::string& n = "", const std::string& desc = "",
                  const std::string& auth = "", const PluginVersion& ver = PluginVersion(),
                  PluginType t = PluginType::General)
        : name(n), description(desc), author(auth), version(ver), type(t) {}
};

// Plugin exception base class
class PluginException : public std::runtime_error {
public:
    explicit PluginException(const std::string& message) : std::runtime_error(message) {}
};

class PluginLoadException : public PluginException {
public:
    explicit PluginLoadException(const std::string& message) : PluginException(message) {}
};

class PluginCompatibilityException : public PluginException {
public:
    explicit PluginCompatibilityException(const std::string& message) : PluginException(message) {}
};

} // namespace nlm