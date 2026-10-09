#pragma once

#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <filesystem>

namespace nlm {

ConfigTemplateManager::ConfigTemplateManager() : nextVersion_(1) {}

ConfigTemplateManager::~ConfigTemplateManager() = default;

bool ConfigTemplateManager::saveTemplate(const std::string& name, const Config& config) {
    try {
        std::lock_guard<std::mutex> lock(templatesMutex_);
        
        TemplateInfo info;
        info.name = name;
        info.description = "Template: " + name;
        info.createdAt = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        info.version = nextVersion_++;
        info.configData = config.toString();
        
        templates_[name] = info;
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigTemplateManager::loadTemplate(const std::string& name, Config& config) {
    try {
        std::lock_guard<std::mutex> lock(templatesMutex_);
        
        auto it = templates_.find(name);
        if (it == templates_.end()) {
            return false;
        }
        
        return config.loadFromString(it->second.configData);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigTemplateManager::deleteTemplate(const std::string& name) {
    try {
        std::lock_guard<std::mutex> lock(templatesMutex_);
        
        return templates_.erase(name) > 0;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> ConfigTemplateManager::listTemplates() const {
    try {
        std::lock_guard<std::mutex> lock(templatesMutex_);
        
        std::vector<std::string> names;
        names.reserve(templates_.size());
        
        for (const auto& pair : templates_) {
            names.push_back(pair.first);
        }
        
        return names;
        
    } catch (const std::exception& e) {
        return std::vector<std::string>();
    }
}

bool ConfigTemplateManager::createTemplate(const std::string& name, const std::string& baseTemplate) {
    try {
        std::lock_guard<std::mutex> lock(templatesMutex_);
        
        TemplateInfo newInfo;
        newInfo.name = name;
        newInfo.description = "Generated from template: " + baseTemplate;
        newInfo.createdAt = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        newInfo.version = nextVersion_++;
        
        if (!baseTemplate.empty()) {
            auto it = templates_.find(baseTemplate);
            if (it != templates_.end()) {
                newInfo.configData = it->second.configData;
            }
        }
        
        templates_[name] = newInfo;
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::string ConfigTemplateManager::getTemplateDescription(const std::string& name) const {
    try {
        std::lock_guard<std::mutex> lock(templatesMutex_);
        
        auto it = templates_.find(name);
        if (it == templates_.end()) {
            return "Template not found: " + name;
        }
        
        return it->second.description + " (v" + std::to_string(it->second.version) + ")";
        
    } catch (const std::exception& e) {
        return "Error: " + std::string(e.what());
    }
}

bool ConfigTemplateManager::exportTemplate(const std::string& name, const std::string& filepath) const {
    try {
        std::lock_guard<std::mutex> lock(templatesMutex_);
        
        auto it = templates_.find(name);
        if (it == templates_.end()) {
            return false;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Template Name: " << it->second.name << std::endl;
        file << "Description: " << it->second.description << std::endl;
        file << "Version: " << it->second.version << std::endl;
        file << "Created At: " << it->second.createdAt << std::endl;
        file << "Config Data:" << std::endl;
        file << it->second.configData;
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigTemplateManager::importTemplate(const std::string& name, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        file.close();
        
        // Parse template data
        TemplateInfo info;
        info.name = name;
        info.version = nextVersion_++;
        
        size_t pos = content.find("Config Data:");
        if (pos != std::string::npos) {
            pos += std::string("Config Data:").length();
            // Skip newlines
            while (pos < content.length() && (content[pos] == '\n' || content[pos] == '\r')) {
                pos++;
            }
            info.configData = content.substr(pos);
        } else {
            info.configData = content;
        }
        
        std::lock_guard<std::mutex> lock(templatesMutex_);
        templates_[name] = info;
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigTemplateManager::validateTemplate(const std::string& name) const {
    try {
        std::lock_guard<std::mutex> lock(templatesMutex_);
        
        auto it = templates_.find(name);
        if (it == templates_.end()) {
            return false;
        }
        
        Config config;
        if (!config.loadFromString(it->second.configData)) {
            return false;
        }
        
        return config.validate();
        
    } catch (const std::exception& e) {
        return false;
    }
}

} // namespace nlm
