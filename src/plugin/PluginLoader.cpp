// PluginLoader.cpp
#include "PluginLoader.hpp"
#include "PluginBase.hpp"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

namespace {
    std::string lastError_;
    std::mutex mutex_;
}

std::unique_ptr<PluginBase> PluginLoader::loadPlugin(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!validatePluginFile(filepath)) {
        lastError_ = "Invalid plugin file: " + filepath;
        return nullptr;
    }

    try {
        PluginManifest manifest = loadPluginManifest(filepath);
        
        // In a real implementation, we would load and instantiate the actual plugin
        // For now, return a placeholder
        lastError_ = "Plugin loading not yet implemented for: " + filepath;
        return nullptr;
    } catch (const std::exception& e) {
        lastError_ = "Failed to load plugin from " + filepath + ": " + std::string(e.what());
        return nullptr;
    }
}

bool PluginLoader::loadAllPluginsFromDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!std::filesystem::exists(directory)) {
        lastError_ = "Plugin directory does not exist: " + directory;
        return false;
    }

    if (!std::filesystem::is_directory(directory)) {
        lastError_ = "Path is not a directory: " + directory;
        return false;
    }

    std::vector<std::string> pluginFiles = getPluginFiles(directory);
    bool success = true;

    for (const auto& file : pluginFiles) {
        if (!loadPlugin(file)) {
            success = false;
        }
    }

    return success;
}

std::vector<std::string> PluginLoader::getPluginFiles(const std::string& directory) {
    std::vector<std::string> pluginFiles;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && validatePluginFile(entry.path().string())) {
                pluginFiles.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        lastError_ = "Failed to scan directory " + directory + ": " + std::string(e.what());
    }

    return pluginFiles;
}

bool PluginLoader::validatePluginFile(const std::string& filepath) {
    try {
        if (!std::filesystem::exists(filepath)) {
            lastError_ = "Plugin file does not exist: " + filepath;
            return false;
        }

        if (!std::filesystem::is_regular_file(filepath)) {
            lastError_ = "Plugin path is not a file: " + filepath;
            return false;
        }

        // Check for common plugin file extensions
        std::filesystem::path path(filepath);
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        if (extension != ".so" && extension != ".dll" && extension != ".dylib" && extension != ".plugin") {
            lastError_ = "Plugin file has unsupported extension: " + extension;
            return false;
        }

        // Try to load manifest
        PluginManifest manifest = loadPluginManifest(filepath);

        return !manifest.name.empty();
    } catch (const std::exception& e) {
        lastError_ = "Error validating plugin file " + filepath + ": " + std::string(e.what());
        return false;
    }
}

PluginLoader::PluginManifest PluginLoader::loadPluginManifest(const std::string& filepath) {
    PluginManifest manifest;

    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open plugin file: " + filepath);
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key, value;

            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                if (key == "name") manifest.name = value;
                else if (key == "version") manifest.version = value;
                else if (key == "description") manifest.description = value;
                else if (key == "author") manifest.author = value;
                else if (key == "plugin_interface") manifest.pluginInterface = value;
                else if (key == "dependencies") manifest.dependencies = value;
                else if (key == "entry_point") manifest.entryPoint = value;
            }
        }
    } catch (const std::exception& e) {
        lastError_ = "Failed to load plugin manifest from " + filepath + ": " + std::string(e.what());
    }

    return manifest;
}

std::string PluginLoader::getLastError() {
    std::lock_guard<std::mutex> lock(mutex_);
    return lastError_;
}

} // namespace nlm