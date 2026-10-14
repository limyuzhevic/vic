#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/Config/Config.hpp"

namespace nlm {

class TemplateManager {
public:
    TemplateManager();
    ~TemplateManager();
    
    // Load a template
    bool loadTemplate(const std::string& templateName);
    
    // Apply template to configuration
    bool applyToConfig(std::shared_ptr<Config> config) const;
    
    // Get template content as JSON string
    std::string getContent() const;
    
    // Get template name
    std::string getName() const;
    
    // Get template description
    std::string getDescription() const;
    
    // Get available template names
    static std::vector<std::string> getAvailableTemplates();
    
    // Get template file path
    static std::string getTemplatePath(const std::string& templateName);
    
    // Check if template exists
    static bool templateExists(const std::string& templateName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Load template from file
    bool loadFromFile(const std::string& filepath);
    
    // Parse template metadata
    void parseMetadata(const std::string& metadata);
};

} // namespace nlm
