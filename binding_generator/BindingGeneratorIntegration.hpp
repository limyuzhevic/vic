#ifndef BINDING_GENERATOR_INTEGRATION_HPP
#define BINDING_GENERATOR_INTEGRATION_HPP

#include "BindingGenerator.hpp"
#include "BindingGeneratorTemplates.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace NlmBindingIntegration {

// Integration class for managing binding generation and existing bindings
class BindingIntegration {
public:
    // Constructor
    BindingIntegration(const std::string& projectRoot);
    
    // Integration methods
    bool integrateWithExistingBindings(const std::string& existingBindingsPath);
    bool generateMergedBindings(const std::string& outputPath);
    bool createBuildSystemIntegration(const BindingConfig& config);
    bool createUsageExamples(const std::string& examplesDir);
    bool createMigrationGuide(const std::string& guidePath);
    
    // File operations
    bool backupExistingBindings(const std::string& sourcePath);
    bool mergeGeneratedWithExisting(const std::string& generatedCode, const std::string& existingCode);
    bool createConfigurationFiles(const BindingConfig& config);
    bool createBuildScripts(const BindingConfig& config);
    
    // Validation and testing
    bool validateGeneratedBindings(const std::string& generatedPath);
    bool testGeneratedBindings(const std::string& testPath);
    bool runIntegrationTests();
    
    // Configuration management
    std::string getProjectRoot() const { return projectRoot_; }
    std::string getPythonBindingsDir() const { return pythonBindingsDir_; }
    std::string getGeneratedBindingsDir() const { return generatedBindingsDir_; }
    
    // Session management
    bool initialize();
    void cleanup();

private:
    std::string projectRoot_;
    std::string pythonBindingsDir_;
    std::string generatedBindingsDir_;
    std::string backupDir_;
    
    // Integration state
    bool bindingsMerged_ = false;
    bool buildSystemIntegrated_ = false;
    bool examplesCreated_ = false;
    bool migrationGuideCreated_ = false;
    
    // Integration helpers
    std::string readExistingBindings(const std::string& path);
    std::string parseExistingBindings(const std::string& source);
    std::string extractBindingsSection(const std::string& source, const std::string& sectionName);
    std::string extractFunctionDefinitions(const std::string& source);
    std::string extractClassDefinitions(const std::string& source);
    std::string extractEnumDefinitions(const std::string& source);
    
    // Merging strategies
    std::string mergeStrategyPreferExisting(const std::string& generated, const std::string& existing);
    std::string mergeStrategyPreferGenerated(const std::string& generated, const std::string& existing);
    std::string mergeStrategySelective(const std::string& generated, const std::string& existing);
    std::string mergeStrategySmart(const std::string& generated, const std::string& existing);
    
    // Build system integration
    bool createCMakeLists(const BindingConfig& config);
    bool createPythonSetupPy(const BindingConfig& config);
    bool createBuildSystemFiles(const BindingConfig& config);
    bool generateBuildScripts(const BindingConfig& config);
    
    // Testing and validation
    bool validateCppSyntax(const std::string& source);
    bool validatePythonInterface(const std::string& source);
    bool validateAgainstExistingAPI(const std::string& generated);
    
    // Example generation
    std::string generateConfigExamples() const;
    std::string generateBrainExamples() const;
    std::string generateNeuronExamples() const;
    std::string generateSynapseExamples() const;
    std::string generateSimpleUsageExample() const;
    std::string generateAdvancedUsageExample() const;
    
    // Migration guide generation
    std::string generateMigrationGuide() const;
    std::string generateUpgradeSteps(const std::string& bindingLevel) const;
    std::string generateCompatibilityNotes() const;
    
    // File utilities
    bool ensureDirectory(const std::string& path);
    bool writeFile(const std::string& path, const std::string& content);
    std::string readFile(const std::string& path);
    bool copyFile(const std::string& src, const std::string& dst);
    std::string getCurrentTimestamp() const;
    std::string generateUniqueFilename(const std::string& baseName, const std::string& extension) const;
    
    // Logging
    void log(const std::string& message) const;
    
    // Statistics
    struct IntegrationStats {
        size_t filesCreated = 0;
        size_t filesModified = 0;
        size_t bindingsGenerated = 0;
        size_t conflictsResolved = 0;
        std::chrono::duration<double> integrationTime;
    };
    
    IntegrationStats stats_;
};

} // namespace NlmBindingIntegration

#endif // BINDING_GENERATOR_INTEGRATION_HPP