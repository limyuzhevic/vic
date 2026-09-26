#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct VisualizationTemplate {
    std::string name; // Template name
    std::string description; // Template description
    std::string author; // Template author
    std::string version; // Template version
    json data; // Template data
    std::map<std::string, std::string> parameters; // Template parameters
    std::string category; // Template category ("neural", "memory", "neuromodulation", "plasticity", "development")
    bool isDefault; // Whether this is a default template
    std::string created; // Creation timestamp
    std::string modified; // Modification timestamp
    std::map<std::string, std::string> metadata; // Additional metadata
};

class TemplateSystem {
public:
    TemplateSystem();
    ~TemplateSystem() = default;
    
    // Load templates from directory
    bool loadTemplatesFromDirectory(const std::string& directory);
    
    // Save templates to directory
    bool saveTemplatesToDirectory(const std::string& directory) const;
    
    // Load template from file
    bool loadTemplate(const std::string& filename);
    
    // Save template to file
    bool saveTemplate(const std::string& filename) const;
    
    // Get template by name
    const VisualizationTemplate* getTemplate(const std::string& name) const;
    
    // Get all templates
    const std::vector<VisualizationTemplate>& getTemplates() const;
    
    // Add template
    bool addTemplate(const VisualizationTemplate& template);
    
    // Update template
    bool updateTemplate(const VisualizationTemplate& template);
    
    // Remove template
    bool removeTemplate(const std::string& name);
    
    // Create default templates
    bool createDefaultTemplates();
    
    // Get default template
    const VisualizationTemplate* getDefaultTemplate(const std::string& category) const;
    
    // Find template by category
    std::vector<const VisualizationTemplate*> findTemplatesByCategory(const std::string& category) const;
    
    // Find template by parameter
    std::vector<const VisualizationTemplate*> findTemplatesByParameter(const std::string& key, const std::string& value) const;
    
    // Apply template to visualization
    bool applyTemplate(nlm::NVMVisualization& visualization, const std::string& templateName, const std::map<std::string, std::string>& parameters);
    
    // Create template from current visualization
    bool createTemplateFromVisualization(const nlm::NVMVisualization& visualization, const std::string& name, const std::string& description);
    
    // Validate template
    bool validateTemplate(const VisualizationTemplate& template) const;
    
    // Get supported template categories
    std::vector<std::string> getSupportedCategories() const;
    
    // Get template statistics
    std::map<std::string, int> getTemplateStatistics() const;
    
    // Export templates
    bool exportTemplates(const std::string& filename, const std::string& format = "json") const;
    
    // Import templates
    bool importTemplates(const std::string& filename, const std::string& format = "json");
    
    // Check if template system is available
    bool isAvailable() const;
    
    // Clear all templates
    void clearTemplates();
    
    // Get template count
    size_t getTemplateCount() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Load default templates
    bool loadDefaultTemplates();
    
    // Create neural network templates
    void createNeuralNetworkTemplates();
    
    // Create memory templates
    void createMemoryTemplates();
    
    // Create neuromodulation templates
    void createNeuromodulationTemplates();
    
    // Create plasticity templates
    void createPlasticityTemplates();
    
    // Create development templates
    void createDevelopmentTemplates();
    
    // Convert template to JSON
    json convertTemplateToJSON(const VisualizationTemplate& template) const;
    
    // Convert JSON to template
    bool convertJSONToTemplate(const json& data, VisualizationTemplate& template) const;
    
    // Validate template data
    bool validateTemplateData(const json& data, const std::string& category) const;
    
    // Get default template path
    std::string getDefaultTemplatePath(const std::string& category) const;
    
    // Check if file is a template file
    bool isTemplateFile(const std::string& filename) const;
    
    // Parse template filename
    std::map<std::string, std::string> parseTemplateFilename(const std::string& filename) const;
};

} // namespace nlm