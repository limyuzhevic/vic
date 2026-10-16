// BindingGenerator.hpp - Core interface for NLM Python binding code generation
// 
// This header defines the BindingGenerator class that automatically produces
// pybind11 bindings from NLM class definitions. Supports complex types,
// nested classes, and template classes with configurable output levels.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_set>
#include <regex>
#include <fstream>
#include <sstream>

// Forward declarations for NLM types
namespace nlm {
    class Config;
    class Brain;
    class Neuron;
    class Synapse;
    class SensoryInput;
    class Action;
    template<typename T> class MemorySystem;
}

// Configuration for binding generation
struct BindingConfig {
    enum class Level { SIMPLE, ADVANCED, FULL };
    
    // Binding configuration options
    Level bindingLevel = Level::ADVANCED;
    std::string outputDirectory = "generated_bindings";
    std::string moduleName = "pynlm";
    bool enableDocumentation = true;
    bool enableErrorHandling = true;
    bool enableTypeConversion = true;
    bool generateConvenienceMethods = true;
    bool maintainBackwardCompatibility = true;
    
    // Include paths and compilation settings
    std::vector<std::string> includePaths;
    std::vector<std::string> compileFlags;
    
    // Custom binding overrides (manual bindings take precedence)
    std::map<std::string, std::string> customBindings;
    std::unordered_set<std::string> excludeClasses;
    std::unordered_set<std::string> includeOnlyClasses;
    
    // Documentation configuration
    std::string documentationTemplate;
    std::string examplesDirectory;
    
    // Extension hooks for custom binding logic
    std::string extensionHookPath;
};

// Class metadata extracted from headers
struct ClassMetadata {
    std::string name;
    std::string namespaceName;
    std::string headerFile;
    std::string documentation;
    bool isTemplate = false;
    std::string templateParams;
    bool hasPimpl = false;
    std::vector<std::string> baseClasses;
    std::vector<std::string> publicMethods;
    std::vector<std::string> privateMethods;
    std::vector<std::string> properties;
    std::map<std::string, std::string> typeAliases;
    std::unordered_set<std::string> dependencies;
    bool isEnum = false;
    std::map<std::string, int> enumValues;
};

// Method metadata for binding generation
struct MethodMetadata {
    std::string name;
    std::string returnType;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::string documentation;
    bool isConst = false;
    bool isStatic = false;
    bool isOperator = false;
    std::string operatorType;
    bool generateGetter = false;
    bool generateSetter = false;
};

// Property metadata for automatic getter/setter generation
struct PropertyMetadata {
    std::string name;
    std::string type;
    std::string getterMethod;
    std::string setterMethod;
    bool isReadOnly = false;
};

// Type conversion rules for pybind11
struct TypeConversion {
    std::string cppType;
    std::string pyType;
    std::string conversionFunction;
    bool isPrimitive = false;
    bool isContainer = false;
    bool isSmartPointer = false;
    std::string defaultValue;
};

class BindingGenerator {
public:
    // Constructor
    BindingGenerator(const BindingConfig& config);
    
    // Main generation interface
    bool generateAllBindings();
    bool generateBindingsForClass(const std::string& className);
    bool generateSimpleBindings();
    bool generateAdvancedBindings();
    
    // Core processing pipeline
    bool parseHeaderFiles();
    bool extractClassMetadata();
    bool generateBindingCode();
    bool writeGeneratedFiles();
    bool validateGeneratedCode();
    
    // Class parsing and analysis
    bool parseHeaderFile(const std::string& filePath);
    bool extractClassFromSource(const std::string& source, ClassMetadata& metadata);
    bool extractMethodsFromSource(const std::string& source, ClassMetadata& metadata);
    bool extractPropertiesFromSource(const std::string& source, ClassMetadata& metadata);
    
    // Template-based code generation
    std::string generateSimpleBinding(const ClassMetadata& metadata) const;
    std::string generateAdvancedBinding(const ClassMetadata& metadata) const;
    std::string generateConvenienceMethods(const ClassMetadata& metadata) const;
    std::string generateTypeConversions(const ClassMetadata& metadata) const;
    std::string generateErrorHandling(const ClassMetadata& metadata) const;
    std::string generateDocumentation(const ClassMetadata& metadata) const;
    
    // Specialized generators for different construct types
    std::string generateEnumBinding(const ClassMetadata& metadata) const;
    std::string generateValueTypeBinding(const ClassMetadata& metadata) const;
    std::string generateReferenceTypeBinding(const ClassMetadata& metadata) const;
    std::string generateContainerTypeBinding(const ClassMetadata& metadata) const;
    
    // Utility functions
    std::string getPybindType(const std::string& cppType) const;
    std::string getConversionFunction(const std::string& cppType) const;
    std::string escapeString(const std::string& str) const;
    std::string generateDocstring(const std::string& documentation) const;
    bool isStandardType(const std::string& type) const;
    bool isContainerType(const std::string& type) const;
    bool isSmartPointerType(const std::string& type) const;
    
    // Configuration and customization
    void setConfig(const BindingConfig& config) { config_ = config; }
    const BindingConfig& getConfig() const { return config_; }
    
    // Session management
    void addCustomBinding(const std::string& className, const std::string& bindingCode);
    void excludeClass(const std::string& className);
    void includeOnlyClass(const std::string& className);
    std::vector<std::string> getGeneratedClasses() const { return generatedClasses_; }
    std::vector<std::string> getExcludedClasses() const { return excludedClasses_; }
    
    // Integration with existing bindings
    bool mergeWithExistingBindings(const std::string& existingBindingsPath);
    std::string generateMergedBindings() const;
    
    // Validation and verification
    bool validateGeneratedCode(const std::string& generatedCode) const;
    bool checkForConflicts(const std::string& generatedCode) const;
    
    // Progress and logging
    void setProgressCallback(std::function<void(const std::string&)> callback);
    void log(const std::string& message) const;
    
    // Statistics
    struct GenerationStats {
        size_t classesProcessed = 0;
        size_t methodsProcessed = 0;
        size_t linesGenerated = 0;
        size_t filesGenerated = 0;
        std::chrono::duration<double> generationTime;
    };
    
    const GenerationStats& getStats() const { return stats_; }
    
    // Initialization and cleanup
    bool initialize();
    void cleanup();
    
    // Extension point for custom binding logic
    bool executeCustomExtension(const ClassMetadata& metadata, std::string& output);

private:
    BindingConfig config_;
    std::map<std::string, ClassMetadata> classMetadata_;
    std::vector<std::string> generatedClasses_;
    std::vector<std::string> excludedClasses_;
    std::vector<std::string> templateCache_;
    std::map<std::string, std::string> customBindings_;
    GenerationStats stats_;
    std::function<void(const std::string&)> progressCallback_;
    
    // Type conversion database
    std::map<std::string, TypeConversion> typeConversions_;
    
    // Parsing utilities
    std::vector<std::string> extractIncludeDirectives(const std::string& source) const;
    std::vector<std::string> extractNamespaces(const std::string& source) const;
    bool parseMethodSignature(const std::string& signature, MethodMetadata& metadata) const;
    std::string normalizeTypeName(const std::string& type) const;
    
    // Template loading and processing
    bool loadTemplates();
    std::string applyTemplate(const std::string& templateName, const ClassMetadata& metadata, const std::map<std::string, std::string>& context) const;
    
    // File operations
    bool ensureOutputDirectory() const;
    bool writeFile(const std::string& path, const std::string& content) const;
    std::string readFile(const std::string& path) const;
    
    // Statistics tracking
    void updateStats(const std::string& operation, size_t count = 0) const;
};