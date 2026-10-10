// PluginManifest.cpp - Implementation of plugin manifest loader
#include "PluginManifest.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

bool PluginManifestLoader::loadManifest(const std::string& manifestPath, PluginManifest& manifest) {
    try {
        std::ifstream file(manifestPath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        manifest.name = j.value("name", "");
        manifest.version = j.value("version", "");
        manifest.author = j.value("author", "");
        manifest.description = j.value("description", "");
        manifest.email = j.value("email", "");
        manifest.website = j.value("website", "");
        
        // Parse category
        if (j.contains("category")) {
            std::string categoryStr = j["category"];
            if (categoryStr == "NEURON") manifest.category = PluginCategory::NEURON;
            else if (categoryStr == "SYNAPSE") manifest.category = PluginCategory::SYNAPSE;
            else if (categoryStr == "MEMORY") manifest.category = PluginCategory::MEMORY;
            else if (categoryStr == "COGNITION") manifest.category = PluginCategory::COGNITION;
            else if (categoryStr == "NEUROMODULATION") manifest.category = PluginCategory::NEUROMODULATION;
            else if (categoryStr == "VISUALIZATION") manifest.category = PluginCategory::VISUALIZATION;
            else manifest.category = PluginCategory::UNKNOWN;
        }
        
        // Parse version compatibility
        if (j.contains("dependencies")) {
            manifest.minCoreVersion = j["dependencies"].value("minCoreVersion", "");
            manifest.maxCoreVersion = j["dependencies"].value("maxCoreVersion", "");
            
            if (j["dependencies"].contains("compatibleVersions")) {
                for (const auto& version : j["dependencies"]["compatibleVersions"]) {
                    manifest.compatibleVersions.insert(version);
                }
            }
        }
        
        // Parse other fields
        if (j.contains("dependencies")) {
            for (const auto& dep : j["dependencies"]) {
                manifest.dependencies.push_back(dep);
            }
        }
        
        if (j.contains("optionalDependencies")) {
            for (const auto& optDep : j["optionalDependencies"].items()) {
                manifest.optionalDependencies[optDep.key()] = optDep.value();
            }
        }
        
        if (j.contains("features")) {
            for (const auto& feature : j["features"]) {
                manifest.features.push_back(feature);
            }
        }
        
        if (j.contains("tags")) {
            for (const auto& tag : j["tags"]) {
                manifest.tags.push_back(tag);
            }
        }
        
        // Parse paths and metadata
        manifest.pluginFilePath = j.value("pluginFilePath", "");
        manifest.configFilePath = j.value("configFilePath", "");
        manifest.isSharedLibrary = j.value("isSharedLibrary", true);
        manifest.buildHash = j.value("buildHash", "");
        manifest.license = j.value("license", "");
        manifest.isPublic = j.value("isPublic", false);
        
        // Parse custom metadata
        if (j.contains("customMetadata")) {
            for (const auto& item : j["customMetadata"].items()) {
                manifest.customMetadata[item.key()] = item.value();
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool PluginManifestLoader::saveManifest(const std::string& manifestPath, const PluginManifest& manifest) {
    try {
        json j;
        
        j["name"] = manifest.name;
        j["version"] = manifest.version;
        j["author"] = manifest.author;
        j["description"] = manifest.description;
        j["email"] = manifest.email;
        j["website"] = manifest.website;
        
        // Convert category to string
        std::string categoryStr;
        switch (manifest.category) {
            case PluginCategory::NEURON: categoryStr = "NEURON"; break;
            case PluginCategory::SYNAPSE: categoryStr = "SYNAPSE"; break;
            case PluginCategory::MEMORY: categoryStr = "MEMORY"; break;
            case PluginCategory::COGNITION: categoryStr = "COGNITION"; break;
            case PluginCategory::NEUROMODULATION: categoryStr = "NEUROMODULATION"; break;
            case PluginCategory::VISUALIZATION: categoryStr = "VISUALIZATION"; break;
            default: categoryStr = "UNKNOWN";
        }
        j["category"] = categoryStr;
        
        // Dependencies
        json deps;
        deps["minCoreVersion"] = manifest.minCoreVersion;
        deps["maxCoreVersion"] = manifest.maxCoreVersion;
        
        json compatibleVersions;
        for (const auto& version : manifest.compatibleVersions) {
            compatibleVersions.push_back(version);
        }
        deps["compatibleVersions"] = compatibleVersions;
        
        json depList;
        for (const auto& dep : manifest.dependencies) {
            depList.push_back(dep);
        }
        deps["runtime"] = depList;
        
        j["dependencies"] = deps;
        
        json optionalDeps;
        for (const auto& optDep : manifest.optionalDependencies) {
            optionalDeps[optDep.first] = optDep.second;
        }
        j["optionalDependencies"] = optionalDeps;
        
        // Features and tags
        json features;
        for (const auto& feature : manifest.features) {
            features.push_back(feature);
        }
        j["features"] = features;
        
        json tags;
        for (const auto& tag : manifest.tags) {
            tags.push_back(tag);
        }
        j["tags"] = tags;
        
        // File paths and metadata
        j["pluginFilePath"] = manifest.pluginFilePath;
        j["configFilePath"] = manifest.configFilePath;
        j["isSharedLibrary"] = manifest.isSharedLibrary;
        j["buildHash"] = manifest.buildHash;
        j["license"] = manifest.license;
        j["isPublic"] = manifest.isPublic;
        
        // Custom metadata
        json customMetadata;
        for (const auto& item : manifest.customMetadata) {
            customMetadata[item.first] = item.second;
        }
        j["customMetadata"] = customMetadata;
        
        std::ofstream file(manifestPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(4);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string PluginManifestLoader::generateManifestPath(const std::string& pluginPath) {
    std::filesystem::path path(pluginPath);
    std::string filename = path.stem().string() + ".plugin.json";
    return (std::filesystem::path(pluginPath).parent_path() / filename).string();
}

} // namespace nlm