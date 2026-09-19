// NLM Enhanced Documentation and User Experience
// Provides improved documentation organization, examples, and user guidance

#ifndef NLM_DOCUMENTATION_H
#define NLM_DOCUMENTATION_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "core/Config/Config.hpp"

namespace nlm {

class NLMDocumentation {
public:
    struct Example {
        std::string title;
        std::string description;
        std::string code;
        std::string difficulty; // "beginner", "intermediate", "advanced"
        std::string category;   // "basic", "brain", "world", "agent", "experiments"
    };
    
    struct Tutorial {
        std::string title;
        std::string content;
        std::vector<Example> examples;
        int order; // For progressive learning
    };
    
    struct APIReference {
        std::string className;
        std::string brief;
        std::map<std::string, std::string> methods;
        std::map<std::string, std::string> properties;
        std::vector<std::string> examples;
    };
    
    NLMDocumentation();
    ~NLMDocumentation();
    
    // Generate documentation
    std::string generateUserGuide() const;
    std::string generateAPITutorials() const;
    std::string generateQuickReference() const;
    std::string generateExamplesCatalog() const;
    
    // Get documentation content
    std::string getUserGuide() const { return userGuideContent; }
    std::string getAPITutorials() const { return apiTutorialsContent; }
    std::string getQuickReference() const { return quickReferenceContent; }
    std::string getExamplesCatalog() const { return examplesCatalogContent; }
    
    // Get examples by category
    std::vector<Example> getExamplesByCategory(const std::string& category) const;
    std::vector<Example> getBeginnerExamples() const;
    std::vector<Example> getAdvancedExamples() const;
    
    // Get tutorials
    std::vector<Tutorial> getTutorials() const { return tutorials; }
    
    // Get API references
    std::vector<APIReference> getAPIReferences() const { return apiRefs; }
    
    // Generate configuration help
    std::string generateConfigurationHelp() const;
    std::string generateExperimentHelp() const;
    
    // Generate example configurations
    std::string generateExampleConfigurations() const;
    
    // Get documentation statistics
    int getTotalExamplesCount() const;
    int getBeginnerExamplesCount() const;
    int getIntermediateExamplesCount() const;
    int getAdvancedExamplesCount() const;
    
private:
    std::string userGuideContent;
    std::string apiTutorialsContent;
    std::string quickReferenceContent;
    std::string examplesCatalogContent;
    
    std::vector<Example> examples;
    std::vector<Tutorial> tutorials;
    std::vector<APIReference> apiRefs;
    
    // Initialize documentation content
    void initializeExamples();
    void initializeTutorials();
    void initializeAPIReferences();
    
    // Generate documentation sections
    std::string generateGettingStartedSection() const;
    std::string generateBrainSection() const;
    std::string generateWorldSection() const;
    std::string generateAgentSection() const;
    std::string generateExperimentsSection() const;
    std::string generateAdvancedTopicsSection() const;
    
    std::string generateBraiMethodsSection(const APIReference& apiRef) const;
    std::string generateWorldMethodsSection(const APIReference& apiRef) const;
    std::string generateAgentMethodsSection(const APIReference& apiRef) const;
    
    // Format code examples
    std::string formatCodeExample(const std::string& code, const std::string& language = "python") const;
    
    // Format configuration examples
    std::string formatConfigExample(const std::string& config) const;
    
    // Generate progressive learning paths
    std::string generateProgressiveLearningPath() const;
    std::string generateAdvancedUserPath() const;
};

// Enhanced examples for different user levels
class NLMExampleGenerator {
public:
    struct GeneratedExample {
        std::string title;
        std::string description;
        std::string pythonCode;
        std::string cppCode;
        std::string expectedOutput;
        std::string learningObjective;
    };
    
    NLMExampleGenerator();
    ~NLMExampleGenerator();
    
    // Generate examples based on user level and interests
    std::vector<GeneratedExample> generateExamples(int userLevel, const std::string& focusArea) const;
    
    // Generate specific example
    GeneratedExample generateExample(const std::string& exampleType, int userLevel) const;
    
    // Get available example types
    std::vector<std::string> getAvailableExampleTypes() const;
    
    // Generate example with custom parameters
    GeneratedExample generateCustomExample(const std::string& title,
                                          const std::string& description,
                                          int neuronCount,
                                          int simulationSteps,
                                          bool includeWorld = false,
                                          bool includeAgent = false) const;
    
private:
    std::vector<std::string> availableExampleTypes;
    
    GeneratedExample generateBasicBrainExample(int userLevel) const;
    GeneratedExample generateWorldInteractionExample(int userLevel) const;
    GeneratedExample generateAgentLearningExample(int userLevel) const;
    GeneratedExample generateExperimentExample(int userLevel) const;
    GeneratedExample generateAdvancedConfigurationExample(int userLevel) const;
    GeneratedExample generatePerformanceBenchmarkExample(int userLevel) const;
};

// Interactive documentation system
class InteractiveDocumentation {
public:
    struct InteractiveElement {
        std::string id;
        std::string type; // "code", "visualization", "example", "concept"
        std::string content;
        std::string trigger; // What triggers this element
        std::string nextElement; // Next element in sequence
    };
    
    InteractiveDocumentation();
    ~InteractiveDocumentation();
    
    // Generate interactive learning sequence
    std::vector<InteractiveElement> generateLearningSequence(const std::string& userLevel) const;
    
    // Get interactive examples
    std::vector<InteractiveElement> getInteractiveExamples() const { return interactiveExamples; }
    
    // Get quick start guide
    std::string getQuickStartGuide() const;
    
    // Get help for specific topic
    std::string getTopicHelp(const std::string& topic) const;
    
    // Get code playground examples
    std::vector<std::string> getCodeExamples(const std::string& topic) const;
    
private:
    std::vector<InteractiveElement> interactiveExamples;
    
    void initializeInteractiveExamples();
    
    InteractiveElement createCodeExample(const std::string& title,
                                        const std::string& code,
                                        const std::string& description) const;
    
    InteractiveElement createConceptExample(const std::string& title,
                                           const std::string& concept,
                                           const std::string& visual) const;
};

// Documentation utilities
class DocumentationUtils {
public:
    static std::string sanitizeCode(const std::string& code);
    static std::string formatForDocumentation(const std::string& text);
    static std::string escapeXML(const std::string& text);
    static std::string escapeHTML(const std::string& text);
    static std::string createHyperlink(const std::string& text, const std::string& url);
    static std::string createCodeBlock(const std::string& code, const std::string& language = "python");
    static std::string createList(const std::vector<std::string>& items, const std::string& type = "unordered");
    static std::string createTable(const std::vector<std::vector<std::string>>& data);
    
    static std::string generateExampleConfiguration(const std::string& scenario);
    static std::string generateTroubleshootingGuide();
    static std::string generateFAQ(const std::string& topic);
    
    static bool validateExampleCode(const std::string& code, const std::string& language);
    static std::string getExampleOutput(const std::string& code, const std::string& language);
};

// Global documentation instances
extern NLMDocumentation globalDocumentation;
extern NLMExampleGenerator globalExampleGenerator;
extern InteractiveDocumentation globalInteractiveDocumentation;

} // namespace nlm

#endif // NLM_DOCUMENTATION_H