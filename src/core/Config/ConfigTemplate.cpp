#pragma once

#include "ConfigTemplate.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct ConfigTemplate::Impl {
    std::unordered_map<std::string, ConfigAdvanced> templates;
    std::unordered_map<std::string, std::vector<std::string>> templateInheritance;
    std::vector<std::string> templateCategories;
};

ConfigTemplate::ConfigTemplate() : pImpl(std::make_unique<Impl>()) {
}

ConfigTemplate::~ConfigTemplate() = default;

ConfigTemplate::ConfigTemplate(ConfigTemplate&& other) noexcept : 
    pImpl(std::move(other.pImpl)) {
}

ConfigTemplate& ConfigTemplate::operator=(ConfigTemplate&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ConfigTemplate::createTemplate(const std::string& name, const ConfigAdvanced& templateConfig, 
                                     const std::string& category, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (pImpl->templates.find(name) != pImpl->templates.end()) {
        return false;
    }
    
    pImpl->templates[name] = templateConfig;
    
    if (!category.empty()) {
        pImpl->templateCategories.push_back(category);
    }
    
    return true;
}

bool ConfigTemplate::applyTemplate(const std::string& name, ConfigAdvanced& config) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->templates.find(name);
    if (it == pImpl->templates.end()) {
        return false;
    }
    
    config.merge(it->second);
    return true;
}

bool ConfigTemplate::removeTemplate(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->templates.find(name);
    if (it == pImpl->templates.end()) {
        return false;
    }
    
    pImpl->templates.erase(it);
    
    // Remove from categories
    pImpl->templateCategories.erase(
        std::remove(pImpl->templateCategories.begin(), pImpl->templateCategories.end(), name),
        pImpl->templateCategories.end()
    );
    
    return true;
}

bool ConfigTemplate::setInheritance(const std::string& child, const std::string& parent) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (pImpl->templates.find(child) == pImpl->templates.end() || 
        pImpl->templates.find(parent) == pImpl->templates.end()) {
        return false;
    }
    
    pImpl->templateInheritance[child].push_back(parent);
    return true;
}

const ConfigAdvanced* ConfigTemplate::getTemplate(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->templates.find(name);
    if (it == pImpl->templates.end()) {
        return nullptr;
    }
    
    return &it->second;
}

ConfigAdvanced* ConfigTemplate::getTemplate(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->templates.find(name);
    if (it == pImpl->templates.end()) {
        return nullptr;
    }
    
    return &it->second;
}

std::vector<std::string> ConfigTemplate::getTemplateNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->templates.size());
    for (const auto& pair : pImpl->templates) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> ConfigTemplate::getTemplateNames(const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> names;
    
    for (const auto& pair : pImpl->templates) {
        auto it = std::find(pImpl->templateCategories.begin(), 
                          pImpl->templateCategories.end(), category);
        if (it != pImpl->templateCategories.end()) {
            names.push_back(pair.first);
        }
    }
    
    return names;
}

std::vector<std::string> ConfigTemplate::getCategories() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> categories;
    categories.reserve(pImpl->templateCategories.size());
    for (const auto& category : pImpl->templateCategories) {
        categories.push_back(category);
    }
    return categories;
}

bool ConfigTemplate::addCategory(const std::string& category) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (std::find(pImpl->templateCategories.begin(), pImpl->templateCategories.end(), category) != pImpl->templateCategories.end()) {
        return false;
    }
    
    pImpl->templateCategories.push_back(category);
    return true;
}

bool ConfigTemplate::removeCategory(const std::string& category) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::find(pImpl->templateCategories.begin(), pImpl->templateCategories.end(), category);
    if (it == pImpl->templateCategories.end()) {
        return false;
    }
    
    pImpl->templateCategories.erase(it);
    return true;
}

std::vector<std::string> ConfigTemplate::getParentTemplates(const std::string& child) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = pImpl->templateInheritance.find(child);
    if (it == pImpl->templateInheritance.end()) {
        return std::vector<std::string>();
    }
    
    return it->second;
}

bool ConfigTemplate::loadTemplatesFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Parse JSON content
    auto data = ConfigFormatParser::parse(content, "json");
    
    // Process templates (simplified - in real implementation would be more complex)
    for (const auto& pair : data) {
        // Create template from data
        ConfigAdvanced templateConfig;
        templateConfig.set(pair.first, pair.second, ConfigSource::Default);
        
        // Find template name in key or use default
        std::string templateName = "template_" + std::to_string(pImpl->templates.size());
        createTemplate(templateName, templateConfig);
    }
    
    return true;
}

bool ConfigTemplate::saveTemplatesToFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::unordered_map<std::string, ConfigValue> allData;
    
    for (const auto& pair : pImpl->templates) {
        // In real implementation, would serialize each template individually
        // For now, create a combined representation
        for (const auto& entry : pair.second.getAll()) {
            allData[entry.first] = entry.second;
        }
    }
    
    std::string content = ConfigFormatParser::serialize(allData, "json");
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

} // namespace nlm