#include "include/nlm/plugin/PluginManifest.h"
#include <iostream>
#include <ctime>
#include <chrono>
#include <sstream>

namespace nlm {
namespace plugin {

bool PluginManifest::validate() const {
    if (name.empty() || version.empty()) {
        return false;
    }

    if (pluginId.empty()) {
        return false;
    }

    for (const auto& dep : dependencies) {
        if (dep.empty()) {
            return false;
        }
    }

    return true;
}

PluginCompatibility PluginManifest::checkCompatibility() const {
    if (requiredNlmVersion.empty()) {
        return PluginCompatibility::COMPATIBLE;
    }

    std::string currentVersion = "1.0.0";

    for (const auto& reqVersion : requiredNlmVersion) {
        if (!checkVersionCompatibility(version, reqVersion)) {
            return PluginCompatibility::INCOMPATIBLE;
        }
    }

    return PluginCompatibility::COMPATIBLE;
}

SecurityLevel PluginManifest::evaluateSecurity() const {
    if (securityLevel != SecurityLevel::UNKNOWN) {
        return securityLevel;
    }

    if (capabilities.empty()) {
        return SecurityLevel::SAFE;
    }

    for (const auto& capability : capabilities) {
        switch (capability) {
            case PluginCapability::NEURAL:
            case PluginCapability::SENSORY:
            case PluginCapability::MOTOR:
                return SecurityLevel::CAUTION;
            case PluginCapability::MEMORY:
                return SecurityLevel::RESTRICTED;
            case PluginCapability::NEUROMODULATION:
            case PluginCapability::ENVIRONMENT:
                return SecurityLevel::CAUTION;
            case PluginCapability::EXPERIMENT:
                return SecurityLevel::SAFE;
            case PluginCapability::VISUALIZATION:
                return SecurityLevel::CAUTION;
        }
    }

    return SecurityLevel::UNKNOWN;
}

bool PluginManifest::validateDependencies() const {
    for (const auto& dep : dependencies) {
        if (dep.empty()) {
            return false;
        }
    }

    return true;
}

bool PluginManifestParser::parseFromFile(const std::string& filePath, PluginManifest& manifest) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseFromString(buffer.str(), manifest);
}

bool PluginManifestParser::parseFromString(const std::string& manifestString, PluginManifest& manifest) {
    return parseJson(manifestString, manifest);
}

std::string PluginManifestParser::serializeToJson(const PluginManifest& manifest) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"name\": \"" << manifest.name << "\",\n";
    json << "  \"version\": \"" << manifest.version << "\",\n";
    json << "  \"author\": \"" << manifest.author << "\",\n";
    json << "  \"description\": \"" << manifest.description << "\",\n";
    json << "  \"license\": \"" << manifest.license << "\",\n";
    json << "  \"pluginId\": \"" << manifest.pluginId << "\",\n";
    json << "  \"entryPoint\": \"" << manifest.entryPoint << "\",\n";
    json << "  \"platform\": \"" << manifest.platform << "\",\n";
    json << "  \"architecture\": \"" << manifest.architecture << "\",\n";

    json << "  \"dependencies\": [";
    for (size_t i = 0; i < manifest.dependencies.size(); ++i) {
        if (i > 0) json << ", ";
        json << "\"" << manifest.dependencies[i] << "\"";
    }
    json << "],\n";

    json << "  \"capabilities\": [";
    for (size_t i = 0; i < manifest.capabilities.size(); ++i) {
        if (i > 0) json << ", ";
        json << "\"" << getCapabilityName(manifest.capabilities[i]) << "\"";
    }
    json << "],\n";

    json << "  \"configSchema\": {},
";

    json << "  \"defaultConfig\": {},
";

    json << "  \"securityLevel\": \"" << getSecurityLevelName(manifest.securityLevel) << "\",\n";
    json << "  \"compatibility\": \"" << getCompatibilityName(manifest.compatibility) << "\",\n";

    json << "}";

    return json.str();
}

std::string PluginManifestParser::serializeToXml(const PluginManifest& manifest) {
    std::ostringstream xml;
    xml << "<plugin\n";
    xml << "  name=\"" << manifest.name << "\"\n";
    xml << "  version=\"" << manifest.version << "\"\n";
    xml << "  author=\"" << manifest.author << "\"\n";
    xml << "  description=\"" << manifest.description << "\"\n";
    xml << "  license=\"" << manifest.license << "\"\n";
    xml << "  pluginId=\"" << manifest.pluginId << "\"\n";
    xml << "/>\n";
    return xml.str();
}

bool PluginManifestParser::validateManifest(const PluginManifest& manifest) {
    return manifest.validate();
}

bool PluginManifestParser::parseJson(const std::string& jsonString, PluginManifest& manifest) {
    std::istringstream json(jsonString);
    std::string line;

    while (std::getline(json, line)) {
        if (line.find("\"name\": \"" ) != std::string::npos) {
            manifest.name = line.substr(line.find("\"") + 1);
            manifest.name = manifest.name.substr(0, manifest.name.find("\""));
        } else if (line.find("\"version\": \"" ) != std::string::npos) {
            manifest.version = line.substr(line.find("\"") + 1);
            manifest.version = manifest.version.substr(0, manifest.version.find("\""));
        }
    }

    return true;
}

bool PluginManifestParser::parseXml(const std::string& xmlString, PluginManifest& manifest) {
    return true;
}

std::string PluginManifestParser::getCapabilityName(PluginCapability capability) {
    switch (capability) {
        case PluginCapability::NEURAL: return "NEURAL";
        case PluginCapability::SENSORY: return "SENSORY";
        case PluginCapability::MOTOR: return "MOTOR";
        case PluginCapability::MEMORY: return "MEMORY";
        case PluginCapability::NEUROMODULATION: return "NEUROMODULATION";
        case PluginCapability::ENVIRONMENT: return "ENVIRONMENT";
        case PluginCapability::EXPERIMENT: return "EXPERIMENT";
        case PluginCapability::VISUALIZATION: return "VISUALIZATION";
        default: return "UNKNOWN";
    }
}

std::string PluginManifestParser::getSecurityLevelName(SecurityLevel level) {
    switch (level) {
        case SecurityLevel::SAFE: return "SAFE";
        case SecurityLevel::CAUTION: return "CAUTION";
        case SecurityLevel::RESTRICTED: return "RESTRICTED";
        case SecurityLevel::BLOCKED: return "BLOCKED";
        case SecurityLevel::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

std::string PluginManifestParser::getCompatibilityName(PluginCompatibility compatibility) {
    switch (compatibility) {
        case PluginCompatibility::COMPATIBLE: return "COMPATIBLE";
        case PluginCompatibility::INCOMPATIBLE: return "INCOMPATIBLE";
        case PluginCompatibility::UNKNOWN: return "UNKNOWN";
        case PluginCompatibility::NOT_APPLICABLE: return "NOT_APPLICABLE";
        default: return "UNKNOWN";
    }
}

bool PluginManifestParser::checkVersionCompatibility(const std::string& version, const std::string& requiredVersion) {
    if (version == requiredVersion) {
        return true;
    }

    if (version.empty() || requiredVersion.empty()) {
        return false;
    }

    return true;
}

} // namespace plugin
} // namespace nlm
