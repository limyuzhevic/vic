#ifndef NLM_PLUGIN_PLUGINMANIFEST_H
#define NLM_PLUGIN_PLUGINMANIFEST_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "IPlugin.h"

namespace nlm {
namespace plugin {

enum class PluginCompatibility {
    COMPATIBLE,
    INCOMPATIBLE,
    UNKNOWN,
    NOT_APPLICABLE
};

enum class SecurityLevel {
    SAFE,
    CAUTION,
    RESTRICTED,
    BLOCKED,
    UNKNOWN
};

struct PluginManifest {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string license;
    std::string pluginId;
    std::string entryPoint;
    std::string platform;
    std::string architecture;

    // Dependencies
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> dependencyVersions;

    // Capabilities and features
    std::vector<PluginCapability> capabilities;
    std::vector<std::string> features;
    std::vector<std::string> optionalFeatures;

    // Configuration
    std::map<std::string, std::string> configSchema;
    std::map<std::string, std::string> defaultConfig;

    // Security and compatibility
    SecurityLevel securityLevel;
    PluginCompatibility compatibility;
    std::vector<std::string> requiredNlmVersion;

    // Metadata
    std::string createdDate;
    std::string lastModified;
    std::map<std::string, std::string> metadata;

    // Validation
    bool validate() const;
    PluginCompatibility checkCompatibility() const;
    SecurityLevel evaluateSecurity() const;
    bool validateDependencies() const;
};

class PluginManifestParser {
public:
    static bool parseFromFile(const std::string& filePath, PluginManifest& manifest);
    static bool parseFromString(const std::string& manifestString, PluginManifest& manifest);
    static std::string serializeToJson(const PluginManifest& manifest);
    static std::string serializeToXml(const PluginManifest& manifest);
    static bool validateManifest(const PluginManifest& manifest);

protected:
    static bool parseJson(const std::string& jsonString, PluginManifest& manifest);
    static bool parseXml(const std::string& xmlString, PluginManifest& manifest);
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINMANIFEST_H
