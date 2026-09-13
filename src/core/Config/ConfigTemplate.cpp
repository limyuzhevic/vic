// NLM Configuration Template Manager Implementation
// Simplified implementation for config template management

#include "core/Config/ConfigTemplate.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace nlm {

struct ConfigTemplateManager::Impl {
    std::map<ConfigTemplateType, std::shared_ptr<ConfigTemplate>> templates;
};

ConfigTemplateManager::ConfigTemplateManager() : pImpl(std::make_unique<Impl>()) {}

ConfigTemplateManager::~ConfigTemplateManager() = default;

void ConfigTemplateManager::registerTemplate(std::unique_ptr<ConfigTemplate> template) {
    if (template) {
        pImpl->templates[template->getType()] = std::move(template);
    }
}

std::shared_ptr<ConfigTemplate> ConfigTemplateManager::getTemplate(ConfigTemplateType type) const {
    auto it = pImpl->templates.find(type);
    if (it != pImpl->templates.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<ConfigTemplate>> ConfigTemplateManager::getAllTemplates() const {
    std::vector<std::shared_ptr<ConfigTemplate>> result;
    result.reserve(pImpl->templates.size());
    for (const auto& pair : pImpl->templates) {
        result.push_back(pair.second);
    }
    return result;
}

ValidationResult ConfigTemplateManager::applyTemplates(std::shared_ptr<Config> config, 
                                                     const std::vector<ConfigTemplateType>& templateTypes) const {
    ValidationResult combinedResult;
    
    for (const auto& type : templateTypes) {
        auto templatePtr = getTemplate(type);
        if (templatePtr && templatePtr->canApply(config)) {
            // Apply template
            templatePtr->applyToConfig(config);
            
            // Validate against template
            ValidationResult result = templatePtr->validate(config);
            if (!result.isValid) {
                for (const auto& error : result.errors) {
                    combinedResult.addError("In " + templatePtr->getName() + ": " + error);
                }
            }
            for (const auto& warning : result.warnings) {
                combinedResult.addWarning("In " + templatePtr->getName() + ": " + warning);
            }
        }
    }
    
    return combinedResult;
}

ValidationResult ConfigTemplateManager::validateAgainstTemplates(const std::shared_ptr<Config>& config) const {
    ValidationResult combinedResult;
    
    for (const auto& pair : pImpl->templates) {
        auto templatePtr = pair.second;
        if (templatePtr->canApply(config)) {
            ValidationResult result = templatePtr->validate(config);
            if (!result.isValid) {
                for (const auto& error : result.errors) {
                    combinedResult.addError("In " + templatePtr->getName() + ": " + error);
                }
            }
            for (const auto& warning : result.warnings) {
                combinedResult.addWarning("In " + templatePtr->getName() + ": " + warning);
            }
        }
    }
    
    return combinedResult;
}

std::shared_ptr<Config> ConfigTemplateManager::createFromTemplate(const std::vector<ConfigTemplateType>& templateTypes) const {
    auto config = std::make_shared<Config>();
    
    for (const auto& type : templateTypes) {
        auto templatePtr = getTemplate(type);
        if (templatePtr) {
            templatePtr->applyToConfig(config);
        }
    }
    
    return config;
}

} // namespace nlm