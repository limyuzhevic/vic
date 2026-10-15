#include "ArchitectureCompiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <chrono>

namespace nlm {

struct ArchitectureCompiler::Impl {
    // Compiler configuration
    std::string compilerPath;
    std::map<std::string, NeuralArchitecture> templates;
    std::vector<std::function<bool(NeuralArchitecture&)>> compilerExtensions;
    
    // Compilation statistics
    struct CompilationStats {
        size_t compilationCount;
        double totalCompilationTime;
        double averageCompilationTime;
        size_t successfulCompilations;
        std::chrono::system_clock::time_point startTime;
        
        CompilationStats()
            : compilationCount(0), totalCompilationTime(0.0), averageCompilationTime(0.0),
              successfulCompilations(0), startTime(std::chrono::system_clock::now()) {}
    };
    
    CompilationStats stats;
    
    // Error and warning handling
    std::vector<std::string> compilationErrors;
    std::vector<std::string> warnings;
    
    Logger* logger;
};

ArchitectureCompiler::ArchitectureCompiler() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
}

ArchitectureCompiler::~ArchitectureCompiler() {
    shutdown();
}

ArchitectureCompiler::ArchitectureCompiler(ArchitectureCompiler&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ArchitectureCompiler& ArchitectureCompiler::operator=(ArchitectureCompiler&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ArchitectureCompiler::initialize(const std::string& compilerPath) {
    pImpl->compilerPath = compilerPath;
    pImpl->logger->info("Architecture compiler initialized");
    return true;
}

void ArchitectureCompiler::update(double timestep) {
    // Update compiler extensions and check for template changes
}

void ArchitectureCompiler::shutdown() {
    pImpl->logger->info("Architecture compiler shutdown");
    pImpl->templates.clear();
    pImpl->compilerExtensions.clear();
    pImpl->compilationErrors.clear();
    pImpl->warnings.clear();
}

bool ArchitectureCompiler::compile(const NeuralArchitecture& arch, const std::string& outputPath) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    pImpl->stats.compilationCount++;
    pImpl->compilationErrors.clear();
    pImpl->warnings.clear();
    
    // Validate architecture
    if (!validateArchitecture(arch)) {
        pImpl->logger->error("Architecture validation failed");
        return false;
    }
    
    // Optimize architecture
    NeuralArchitecture optimizedArch = arch;
    if (!optimizeArchitecture(optimizedArch)) {
        pImpl->logger->warning("Architecture optimization failed, using original");
    }
    
    // Generate code
    std::string generatedCode;
    try {
        generatedCode = generateCode(optimizedArch);
    } catch (const std::exception& e) {
        pImpl->logger->error("Code generation failed: " + std::string(e.what()));
        pImpl->compilationErrors.push_back("Code generation failed: " + std::string(e.what()));
        return false;
    }
    
    // Save to file if output path provided
    if (!outputPath.empty()) {
        std::ofstream file(outputPath);
        if (!file) {
            pImpl->logger->error("Failed to create output file: " + outputPath);
            pImpl->compilationErrors.push_back("Failed to create output file: " + outputPath);
            return false;
        }
        
        file << generatedCode;
        if (!file.good()) {
            pImpl->logger->error("Failed to write to output file: " + outputPath);
            pImpl->compilationErrors.push_back("Failed to write to output file: " + outputPath);
            return false;
        }
        
        pImpl->logger->info("Architecture compiled and saved to: " + outputPath);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime);
    
    pImpl->stats.totalCompilationTime += duration.count();
    pImpl->stats.averageCompilationTime = pImpl->stats.totalCompilationTime / pImpl->stats.compilationCount;
    pImpl->stats.successfulCompilations++;
    
    pImpl->logger->info("Architecture compiled successfully in " + std::to_string(duration.count()) + " seconds");
    return true;
}

bool ArchitectureCompiler::compile(const std::string& sourcePath, const std::string& outputPath) {
    // Load architecture from file
    NeuralArchitecture arch;
    
    // Try YAML first
    if (endsWith(sourcePath, ".yaml") || endsWith(sourcePath, ".yml")) {
        if (!loadArchitectureFromYAML(sourcePath, arch)) {
            pImpl->logger->error("Failed to load architecture from YAML: " + sourcePath);
            return false;
        }
    } else if (endsWith(sourcePath, ".json")) {
        if (!loadArchitectureFromJSON(sourcePath, arch)) {
            pImpl->logger->error("Failed to load architecture from JSON: " + sourcePath);
            return false;
        }
    } else {
        pImpl->logger->error("Unsupported architecture file format: " + sourcePath);
        return false;
    }
    
    return compile(arch, outputPath);
}

bool ArchitectureCompiler::compileFromString(const std::string& sourceString, const std::string& outputPath) {
    // Try to detect format from content
    if (sourceString.find("regions:") != std::string::npos || 
        sourceString.find("populations:") != std::string::npos) {
        return loadArchitectureFromYAMLString(sourceString) && compile(arch, outputPath);
    } else if (sourceString.find('"regions"') != std::string::npos || 
               sourceString.find('"populations"') != std::string::npos) {
        return loadArchitectureFromJSONString(sourceString) && compile(arch, outputPath);
    }
    
    pImpl->logger->error("Could not determine architecture format from string");
    return false;
}

std::string ArchitectureCompiler::generateCode(const NeuralArchitecture& arch) const {
    std::stringstream ss;
    
    // Generate header
    ss << generateHeader(arch);
    
    // Generate source code for regions
    for (const auto& region : arch.regions) {
        ss << "// Region: " << region.name << "\n";
        ss << "namespace " << region.name << " {\n";
        
        // Generate population code
        for (const auto& population : region.populations) {
            ss << "// Population: " << population.name << "\n";
            ss << "class " << population.name << " : public Neuron {\n";
            ss << "public:\n";
            ss << "    " << population.name << "() : Neuron() {}\n";
            ss << "    void configure() override {\n";
            
            // Add neuron configuration based on type
            ss << "        // TODO: Add neuron type-specific configuration\n";
            
            ss << "    }\n";
            ss << "};\n\n";
        }
        
        // Generate connection rules
        for (const auto& connection : region.internalConnections) {
            ss << "// Connection: " << connection.sourceRegion << " -> " << connection.targetRegion << "\n";
            ss << "void connect_" << connection.sourceRegion << "_to_" << connection.targetRegion << "() {\n";
            ss << "    // TODO: Implement connection creation\n";
            ss << "}\n\n";
        }
        
        ss << "}\n\n";
    }
    
    // Generate main brain integration
    ss << "// Brain integration code\n";
    ss << "void initializeBrainComponents() {\n";
    ss << "    // TODO: Initialize all generated components\n";
    ss << "}\n\n";
    
    ss << "void compileGeneratedArchitecture() {\n";
    ss << "    // TODO: Connect all generated regions and populations\n";
    ss << "}\n";
    
    return ss.str();
}

std::string ArchitectureCompiler::generateHeader(const NeuralArchitecture& arch) const {
    std::stringstream ss;
    
    ss << "// Auto-generated neural architecture: " << arch.name << "\n";
    ss << "// Version: " << arch.version << "\n";
    ss << "// Description: " << arch.description << "\n";
    ss << "// Generated by NLM Architecture Compiler\n";
    ss << "\n";
    
    ss << "#pragma once\n";
    ss << "\n";
    ss << "#include \"../brain/Neuron.hpp\"\n";
    ss << "#include \"../brain/NeuralRegion.hpp\"\n";
    ss << "#include \"../plasticity/PlasticityRule.hpp\"\n";
    ss << "\n";
    
    ss << "namespace nlm {\n";
    ss << "namespace architecture {\n";
    
    return ss.str();
}

std::string ArchitectureCompiler::generateSource(const NeuralArchitecture& arch) const {
    std::stringstream ss;
    ss << "// Source file for architecture: " << arch.name << "\n";
    ss << "\n";
    ss << "#include \"ArchitectureCompiler.hpp\"\n";
    ss << "\n";
    ss << "namespace nlm {\n";
    ss << "namespace architecture {\n";
    
    // Implementation would go here
    ss << "// Implementation of generated architecture code\n";
    
    ss << "}\n"; // namespace architecture
    ss << "}\n"; // namespace nlm
    
    return ss.str();
}

bool ArchitectureCompiler::validateArchitecture(const NeuralArchitecture& arch) const {
    std::vector<std::string> errors;
    
    // Validate name
    if (arch.name.empty()) {
        errors.push_back("Architecture name cannot be empty");
    }
    
    // Validate regions
    std::unordered_set<std::string> regionNames;
    for (const auto& region : arch.regions) {
        if (region.name.empty()) {
            errors.push_back("Region name cannot be empty");
        }
        
        if (regionNames.find(region.name) != regionNames.end()) {
            errors.push_back("Duplicate region name: " + region.name);
        }
        regionNames.insert(region.name);
    }
    
    // Validate populations
    std::unordered_set<std::string> populationNames;
    for (const auto& population : arch.populations) {
        if (population.name.empty()) {
            errors.push_back("Population name cannot be empty");
        }
        
        if (populationNames.find(population.name) != populationNames.end()) {
            errors.push_back("Duplicate population name: " + population.name);
        }
        populationNames.insert(population.name);
    }
    
    // Validate connections
    for (const auto& connection : arch.connections) {
        if (connection.sourceRegion.empty() || connection.targetRegion.empty()) {
            errors.push_back("Connection regions cannot be empty");
        }
        
        // Check if referenced regions exist
        if (std::find_if(arch.regions.begin(), arch.regions.end(),
                        [&connection](const RegionSpec& r) { return r.name == connection.sourceRegion; }) == arch.regions.end()) {
            errors.push_back("Source region not found: " + connection.sourceRegion);
        }
        
        if (std::find_if(arch.regions.begin(), arch.regions.end(),
                        [&connection](const RegionSpec& r) { return r.name == connection.targetRegion; }) == arch.regions.end()) {
            errors.push_back("Target region not found: " + connection.targetRegion);
        }
    }
    
    pImpl->compilationErrors = errors;
    
    if (!errors.empty()) {
        pImpl->logger->error("Architecture validation failed: " + std::to_string(errors.size()) + " errors");
        return false;
    }
    
    return true;
}

std::vector<std::string> ArchitectureCompiler::getCompilationErrors() const {
    return pImpl->compilationErrors;
}

std::vector<std::string> ArchitectureCompiler::getWarnings() const {
    return pImpl->warnings;
}

bool ArchitectureCompiler::optimizeArchitecture(NeuralArchitecture& arch) {
    bool optimized = false;
    
    // Optimize connectivity patterns
    if (optimizeConnectivity(arch)) {
        optimized = true;
        pImpl->warnings.push_back("Connectivity pattern optimized");
    }
    
    // Optimize region placement
    if (optimizeRegionPlacement(arch)) {
        optimized = true;
        pImpl->warnings.push_back("Region placement optimized");
    }
    
    return optimized;
}

bool ArchitectureCompiler::optimizeConnectivity(NeuralArchitecture& arch) {
    bool changed = false;
    
    for (auto& connection : arch.connections) {
        // Simple optimization: remove low-probability connections
        if (connection.connectionProbability < 0.01) {
            connection.connectionProbability = 0.0;
            changed = true;
            pImpl->warnings.push_back("Low-probability connection removed: " + connection.sourceRegion + " -> " + connection.targetRegion);
        }
    }
    
    return changed;
}

bool ArchitectureCompiler::optimizeRegionPlacement(NeuralArchitecture& arch) {
    // Simple optimization: ensure all referenced regions exist
    bool changed = false;
    
    std::vector<RegionSpec> validRegions;
    for (const auto& region : arch.regions) {
        // Check if region is referenced by any population or connection
        bool isReferenced = false;
        
        for (const auto& population : arch.populations) {
            if (population.name.find(region.name) != std::string::npos) {
                isReferenced = true;
                break;
            }
        }
        
        for (const auto& connection : arch.connections) {
            if (connection.sourceRegion == region.name || connection.targetRegion == region.name) {
                isReferenced = true;
                break;
            }
        }
        
        if (isReferenced) {
            validRegions.push_back(region);
        } else {
            changed = true;
            pImpl->warnings.push_back("Unused region removed: " + region.name);
        }
    }
    
    arch.regions = validRegions;
    return changed;
}

bool ArchitectureCompiler::addArchitectureTemplate(const std::string& name, const NeuralArchitecture& templateArch) {
    if (name.empty()) {
        pImpl->logger->error("Template name cannot be empty");
        return false;
    }
    
    // Check for duplicate
    if (pImpl->templates.find(name) != pImpl->templates.end()) {
        pImpl->logger->warning("Template already exists: " + name);
        return false;
    }
    
    pImpl->templates[name] = templateArch;
    pImpl->logger->info("Architecture template added: " + name);
    return true;
}

bool ArchitectureCompiler::removeArchitectureTemplate(const std::string& name) {
    auto it = pImpl->templates.find(name);
    if (it != pImpl->templates.end()) {
        pImpl->templates.erase(it);
        pImpl->logger->info("Architecture template removed: " + name);
        return true;
    }
    
    pImpl->logger->warning("Template not found: " + name);
    return false;
}

bool ArchitectureCompiler::updateArchitectureTemplate(const std::string& name, const NeuralArchitecture& templateArch) {
    auto it = pImpl->templates.find(name);
    if (it != pImpl->templates.end()) {
        it->second = templateArch;
        pImpl->logger->info("Architecture template updated: " + name);
        return true;
    }
    
    pImpl->logger->warning("Template not found: " + name);
    return false;
}

NeuralArchitecture ArchitectureCompiler::getArchitectureTemplate(const std::string& name) const {
    auto it = pImpl->templates.find(name);
    if (it != pImpl->templates.end()) {
        return it->second;
    }
    
    return NeuralArchitecture(); // Return empty architecture if not found
}

std::vector<std::string> ArchitectureCompiler::getAvailableTemplates() const {
    std::vector<std::string> names;
    names.reserve(pImpl->templates.size());
    
    for (const auto& templatePair : pImpl->templates) {
        names.push_back(templatePair.first);
    }
    
    return names;
}

size_t ArchitectureCompiler::getCompilationCount() const {
    return pImpl->stats.compilationCount;
}

double ArchitectureCompiler::getAverageCompilationTime() const {
    if (pImpl->stats.compilationCount == 0) return 0.0;
    return pImpl->stats.averageCompilationTime;
}

std::string ArchitectureCompiler::getCompilationStats() const {
    std::stringstream ss;
    ss << "Compilation Statistics:\n";
    ss << "=====================\n";
    ss << "Total compilations: " << pImpl->stats.compilationCount << "\n";
    ss << "Successful compilations: " << pImpl->stats.successfulCompilations << "\n";
    ss << "Failed compilations: " << (pImpl->stats.compilationCount - pImpl->stats.successfulCompilations) << "\n";
    ss << "Average compilation time: " << pImpl->stats.averageCompilationTime << " seconds\n";
    ss << "Total compilation time: " << pImpl->stats.totalCompilationTime << " seconds\n";
    ss << "Available templates: " << pImpl->templates.size() << "\n";
    ss << "Compiler extensions: " << pImpl->compilerExtensions.size() << "\n";
    
    return ss.str();
}

bool ArchitectureCompiler::loadTemplateFromYAML(const std::string& filepath, NeuralArchitecture& arch) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open YAML file: " + filepath);
            return false;
        }
        
        YAML::Node root = YAML::Load(file);
        arch = yamlToArchitecture(root);
        
        pImpl->logger->info("Architecture loaded from YAML: " + filepath);
        return true;
    } catch (const YAML::Exception& e) {
        pImpl->logger->error("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

bool ArchitectureCompiler::loadTemplateFromJSON(const std::string& filepath, NeuralArchitecture& arch) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open JSON file: " + filepath);
            return false;
        }
        
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errors;
        
        if (!builder(&file, &file, &root, &errors)) {
            pImpl->logger->error("JSON parsing error: " + errors);
            return false;
        }
        
        arch = jsonToArchitecture(root);
        
        pImpl->logger->info("Architecture loaded from JSON: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool ArchitectureCompiler::loadArchitectureFromYAML(const std::string& filepath) {
    NeuralArchitecture tempArch;
    if (loadTemplateFromYAML(filepath, tempArch)) {
        arch = tempArch;
        return true;
    }
    return false;
}

bool ArchitectureCompiler::loadArchitectureFromJSON(const std::string& filepath) {
    NeuralArchitecture tempArch;
    if (loadTemplateFromJSON(filepath, tempArch)) {
        arch = tempArch;
        return true;
    }
    return false;
}

bool ArchitectureCompiler::loadArchitectureFromYAMLString(const std::string& sourceString) {
    try {
        YAML::Node root = YAML::Load(sourceString);
        arch = yamlToArchitecture(root);
        return true;
    } catch (const YAML::Exception& e) {
        pImpl->logger->error("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

bool ArchitectureCompiler::loadArchitectureFromJSONString(const std::string& sourceString) {
    try {
        Json::Value root;
        std::stringstream stream(sourceString);
        Json::CharReaderBuilder builder;
        std::string errors;
        
        if (!builder(&stream, &stream, &root, &errors)) {
            pImpl->logger->error("JSON parsing error: " + errors);
            return false;
        }
        
        arch = jsonToArchitecture(root);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool ArchitectureCompiler::saveTemplateToYAML(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open YAML file for writing: " + filepath);
            return false;
        }
        
        YAML::Emitter emitter;
        archToYAML(arch, emitter);
        
        file << emitter.c_str();
        pImpl->logger->info("Architecture saved to YAML: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to save YAML file: " + std::string(e.what()));
        return false;
    }
}

bool ArchitectureCompiler::saveTemplateToJSON(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open JSON file for writing: " + filepath);
            return false;
        }
        
        Json::Value root;
        archToJSON(arch, root);
        
        Json::StreamWriterBuilder writer;
        writer["indentation"] = "  ";
        
        std::unique_ptr<Json::StreamWriter> writerInstance(writer.newStreamWriter());
        writerInstance->write(root, &file);
        
        pImpl->logger->info("Architecture saved to JSON: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to save JSON file: " + std::string(e.what()));
        return false;
    }
}

bool ArchitectureCompiler::addCompilerExtension(const std::string& name, std::function<bool(NeuralArchitecture&)> extension) {
    if (name.empty() || !extension) {
        pImpl->logger->error("Invalid compiler extension name or function");
        return false;
    }
    
    // Check for duplicate
    for (const auto& existing : pImpl->compilerExtensions) {
        // In real implementation, would need to track extension names
    }
    
    pImpl->compilerExtensions.push_back(extension);
    pImpl->logger->info("Compiler extension added: " + name);
    return true;
}

bool ArchitectureCompiler::removeCompilerExtension(const std::string& name) {
    // In real implementation, would remove by name
    pImpl->logger->warning("Extension removal not fully implemented: " + name);
    return true;
}

std::vector<std::string> ArchitectureCompiler::getCompilerExtensions() const {
    std::vector<std::string> names;
    // In real implementation, would track extension names
    return names;
}

// YAML conversion functions
NeuralArchitecture ArchitectureCompiler::yamlToArchitecture(const YAML::Node& node) const {
    NeuralArchitecture arch;
    
    if (node["name"]) arch.name = node["name"].as<std::string>();
    if (node["version"]) arch.version = node["version"].as<std::string>();
    if (node["description"]) arch.description = node["description"].as<std::string>();
    
    // Convert regions
    if (node["regions"]) {
        YAML::Node regions = node["regions"];
        for (auto it = regions.begin(); it != regions.end(); ++it) {
            arch.regions.push_back(yamlToRegionSpec(*it));
        }
    }
    
    // Convert populations
    if (node["populations"]) {
        YAML::Node populations = node["populations"];
        for (auto it = populations.begin(); it != populations.end(); ++it) {
            arch.populations.push_back(yamlToPopulationSpec(*it));
        }
    }
    
    // Convert connections
    if (node["connections"]) {
        YAML::Node connections = node["connections"];
        for (auto it = connections.begin(); it != connections.end(); ++it) {
            arch.connections.push_back(yamlToConnectionSpec(*it));
        }
    }
    
    return arch;
}

RegionSpec ArchitectureCompiler::yamlToRegionSpec(const YAML::Node& node) const {
    RegionSpec region;
    
    if (node["name"]) region.name = node["name"].as<std::string>();
    if (node["type"]) region.type = node["type"].as<std::string>();
    if (node["connectivityPattern"]) region.connectivityPattern = node["connectivityPattern"].as<std::string>();
    
    // Convert parameters
    if (node["parameters"]) {
        YAML::Node params = node["parameters"];
        for (auto it = params.begin(); it != params.end(); ++it) {
            region.parameters[it->first.as<std::string>()] = it->second.as<double>();
        }
    }
    
    // Convert populations
    if (node["populations"]) {
        YAML::Node pops = node["populations"];
        for (auto it = pops.begin(); it != pops.end(); ++it) {
            region.populations.push_back(yamlToPopulationSpec(*it));
        }
    }
    
    return region;
}

PopulationSpec ArchitectureCompiler::yamlToPopulationSpec(const YAML::Node& node) const {
    PopulationSpec pop;
    
    if (node["name"]) pop.name = node["name"].as<std::string>();
    if (node["neuronType"]) pop.neuronType = node["neuronType"].as<std::string>();
    if (node["neuronCount"]) pop.neuronCount = node["neuronCount"].as<size_t>();
    
    // Convert parameters
    if (node["parameters"]) {
        YAML::Node params = node["parameters"];
        for (auto it = params.begin(); it != params.end(); ++it) {
            pop.parameters[it->first.as<std::string>()] = it->second.as<double>();
        }
    }
    
    return pop;
}

ConnectionSpec ArchitectureCompiler::yamlToConnectionSpec(const YAML::Node& node) const {
    ConnectionSpec conn;
    
    if (node["sourceRegion"]) conn.sourceRegion = node["sourceRegion"].as<std::string>();
    if (node["targetRegion"]) conn.targetRegion = node["targetRegion"].as<std::string>();
    if (node["connectionType"]) conn.connectionType = node["connectionType"].as<std::string>();
    if (node["connectionProbability"]) conn.connectionProbability = node["connectionProbability"].as<double>();
    
    return conn;
}

void ArchitectureCompiler::archToYAML(const NeuralArchitecture& arch, YAML::Emitter& emitter) const {
    emitter << YAML::BeginMap;
    
    emitter << YAML::Key << "name" << YAML::Value << arch.name;
    emitter << YAML::Key << "version" << YAML::Value << arch.version;
    emitter << YAML::Key << "description" << YAML::Value << arch.description;
    
    // Convert regions
    emitter << YAML::Key << "regions";
    emitter << YAML::Value << YAML::BeginSeq;
    for (const auto& region : arch.regions) {
        yamlEmitter << YAML::BeginMap;
        yamlEmitter << YAML::Key << "name" << YAML::Value << region.name;
        yamlEmitter << YAML::Key << "type" << YAML::Value << region.type;
        yamlEmitter << YAML::Key << "connectivityPattern" << YAML::Value << region.connectivityPattern;
        
        if (!region.parameters.empty()) {
            yamlEmitter << YAML::Key << "parameters";
            yamlEmitter << YAML::Value << YAML::BeginMap;
            for (const auto& param : region.parameters) {
                yamlEmitter << YAML::Key << param.first << YAML::Value << param.second;
            }
            yamlEmitter << YAML::EndMap;
        }
        
        yamlEmitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;
    
    // Similar for populations and connections...
    
    emitter << YAML::EndMap;
}

// Similar JSON conversion functions...

bool ArchitectureCompiler::endsWith(const std::string& str, const std::string& suffix) const {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace nlm
