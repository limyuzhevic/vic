#include "TemplateManager.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct TemplateManager::Impl {
    std::string name;
    std::string description;
    std::string content;
    nlohmann::json metadata;
};

TemplateManager::TemplateManager() : pImpl(std::make_unique<Impl>()) {}

TemplateManager::~TemplateManager() = default;

bool TemplateManager::loadTemplate(const std::string& templateName) {
    std::string filepath = getTemplatePath(templateName);
    if (!templateExists(templateName)) {
        return false;
    }
    
    return loadFromFile(filepath);
}

bool TemplateManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    pImpl->content = buffer.str();
    
    // Parse JSON
    try {
        pImpl->metadata = nlohmann::json::parse(pImpl->content);
        
        // Extract metadata
        parseMetadata(pImpl->content);
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool TemplateManager::applyToConfig(std::shared_ptr<Config> config) const {
    if (!config) {
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(pImpl->content);
        return config->fromJSON(jsonData);
    } catch (const std::exception& e) {
        return false;
    }
}

std::string TemplateManager::getContent() const {
    return pImpl->content;
}

std::string TemplateManager::getName() const {
    return pImpl->name;
}

std::string TemplateManager::getDescription() const {
    return pImpl->description;
}

std::vector<std::string> TemplateManager::getAvailableTemplates() {
    std::vector<std::string> templates;
    std::string templatesDir = "configs/templates";
    
    if (!std::filesystem::exists(templatesDir)) {
        return templates;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(templatesDir)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            
            // Check if it's a JSON file
            if (filename.size() >= 5 && filename.substr(filename.size() - 5) == ".json") {
                // Remove .json extension
                templates.push_back(filename.substr(0, filename.size() - 5));
            }
        }
    }
    
    return templates;
}

std::string TemplateManager::getTemplatePath(const std::string& templateName) {
    std::filesystem::path templatesDir = "configs/templates";
    std::filesystem::path templatePath = templatesDir / (templateName + ".json");
    return templatePath.string();
}

bool TemplateManager::templateExists(const std::string& templateName) {
    std::filesystem::path templatePath = getTemplatePath(templateName);
    return std::filesystem::exists(templatePath);
}

void TemplateManager::parseMetadata(const std::string& metadata) {
    // Extract name and description from JSON
    try {
        nlohmann::json jsonData = nlohmann::json::parse(metadata);
        
        if (jsonData.contains("nlm")) {
            pImpl->name = "NLM Configuration Template";
            pImpl->description = "Neural Learning Machine configuration template";
            
            if (jsonData["nlm"].contains("description")) {
                pImpl->description = jsonData["nlm"]["description"].get<std::string>();
            }
        }
    } catch (const std::exception& e) {
        // Use default metadata
        pImpl->name = templateName;
        pImpl->description = "Neural Learning Machine configuration template";
    }
}

} // namespace nlm
