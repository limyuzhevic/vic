#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_set>
#include <regex>
#include "../core/Config/Config.hpp"
#include "../config/AdvancedConfig.hpp"
#include "AdvancedConfig.hpp"

namespace nlm {

class ArchitectureCompiler {
public:
    ArchitectureCompiler();
    ~ArchitectureCompiler();
    
    // Disable copying, enable moving
    ArchitectureCompiler(const ArchitectureCompiler&) = delete;
    ArchitectureCompiler& operator=(const ArchitectureCompiler&) = delete;
    ArchitectureCompiler(ArchitectureCompiler&&) noexcept;
    ArchitectureCompiler& operator=(ArchitectureCompiler&&) noexcept;
    
    // Compilation lifecycle
    bool initialize(const std::string& compilerPath = "");
    void update(double timestep);
    void shutdown();
    
    // Architecture compilation
    bool compile(const NeuralArchitecture& arch, const std::string& outputPath = "");
    bool compile(const std::string& sourcePath, const std::string& outputPath = "");
    bool compileFromString(const std::string& sourceString, const std::string& outputPath = "");
    
    // Code generation
    std::string generateCode(const NeuralArchitecture& arch) const;
    std::string generateHeader(const NeuralArchitecture& arch) const;
    std::string generateSource(const NeuralArchitecture& arch) const;
    
    // Validation and analysis
    bool validateArchitecture(const NeuralArchitecture& arch) const;
    std::vector<std::string> getCompilationErrors() const;
    std::vector<std::string> getWarnings() const;
    
    // Architecture optimization
    bool optimizeArchitecture(NeuralArchitecture& arch);
    bool optimizeConnectivity(NeuralArchitecture& arch);
    bool optimizeRegionPlacement(NeuralArchitecture& arch);
    
    // Template management
    bool addArchitectureTemplate(const std::string& name, const NeuralArchitecture& templateArch);
    bool removeArchitectureTemplate(const std::string& name);
    bool updateArchitectureTemplate(const std::string& name, const NeuralArchitecture& templateArch);
    NeuralArchitecture getArchitectureTemplate(const std::string& name) const;
    std::vector<std::string> getAvailableTemplates() const;
    
    // Compilation statistics
    size_t getCompilationCount() const;
    double getAverageCompilationTime() const;
    std::string getCompilationStats() const;
    
    // File format support
    bool loadTemplateFromYAML(const std::string& filepath);
    bool loadTemplateFromJSON(const std::string& filepath);
    bool saveTemplateToYAML(const std::string& filepath) const;
    bool saveTemplateToJSON(const std::string& filepath) const;
    
    // Custom compiler extensions
    bool addCompilerExtension(const std::string& name, std::function<bool(NeuralArchitecture&)> extension);
    bool removeCompilerExtension(const std::string& name);
    std::vector<std::string> getCompilerExtensions() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural architecture definition structure
struct NeuralArchitecture {
    std::string name;
    std::string version;
    std::string description;
    
    std::vector<RegionSpec> regions;
    std::vector<PopulationSpec> populations;
    std::vector<ConnectionRule> connections;
    
    std::map<std::string, std::string> metadata;
    
    NeuralArchitecture()
        : version("1.0.0"), name("UnnamedArchitecture"), description("Untitled neural architecture") {}
    
    NeuralArchitecture(const std::string& archName, const std::string& archVersion = "1.0.0")
        : name(archName), version(archVersion), description("Untitled neural architecture") {}
};

// Region specification
struct RegionSpec {
    std::string name;
    std::string type;
    std::vector<std::string> neuronTypes;
    std::string connectivityPattern;
    std::map<std::string, double> parameters;
    std::vector<PopulationSpec> populations;
    std::vector<ConnectionRule> internalConnections;
    
    RegionSpec() : type("cortical"), connectivityPattern("fully_connected") {}
    RegionSpec(const std::string& regionName) : name(regionName), type("cortical"), connectivityPattern("fully_connected") {}
};

// Population specification
struct PopulationSpec {
    std::string name;
    std::string neuronType;
    size_t neuronCount;
    std::map<std::string, double> parameters;
    std::vector<PopulationSpec> connectedPopulations;
    
    PopulationSpec() : neuronCount(100) {}
    PopulationSpec(const std::string& popName, const std::string& type, size_t count)
        : name(popName), neuronType(type), neuronCount(count) {}
};

// Connection rule specification
struct ConnectionRule {
    std::string sourceRegion;
    std::string targetRegion;
    std::string connectionType; // "feedforward", "feedback", "recurrent"
    double connectionProbability;
    std::map<std::string, double> weightParameters;
    std::vector<std::string> plasticityRules;
    std::map<std::string, std::string> metadata;
    
    ConnectionRule()
        : connectionProbability(0.5), connectionType("feedforward") {}
    
    ConnectionRule(const std::string& src, const std::string& tgt)
        : sourceRegion(src), targetRegion(tgt), connectionProbability(0.5), connectionType("feedforward") {}
};

} // namespace nlm
