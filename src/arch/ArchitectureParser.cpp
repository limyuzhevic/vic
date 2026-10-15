#include "ArchitectureParser.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <chrono>

namespace nlm {

struct ArchitectureParser::Impl {
    // Parser registry
    std::map<std::string, std::unique_ptr<IArchitectureParser>> parsers;
    
    // Parsing statistics
    struct ParserStats {
        size_t parseCount;
        double totalParseTime;
        double averageParseTime;
        size_t successfulParses;
        std::chrono::system_clock::time_point startTime;
        
        ParserStats()
            : parseCount(0), totalParseTime(0.0), averageParseTime(0.0), successfulParses(0),
              startTime(std::chrono::system_clock::now()) {}
    };
    
    ParserStats stats;
    
    // Error and warning tracking
    std::vector<std::string> parsingErrors;
    std::vector<std::string> parsingWarnings;
    
    Logger* logger;
};

ArchitectureParser::ArchitectureParser() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
    
    // Register built-in parsers
    pImpl->parsers["yaml"] = std::make_unique<YAMLArchitectureParser>();
    pImpl->parsers["json"] = std::make_unique<JSONArchitectureParser>();
    pImpl->parsers["text"] = std::make_unique<TextArchitectureParser>();
}

ArchitectureParser::~ArchitectureParser() {
    shutdown();
}

ArchitectureParser::ArchitectureParser(ArchitectureParser&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ArchitectureParser& ArchitectureParser::operator=(ArchitectureParser&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ArchitectureParser::initialize() {
    pImpl->logger->info("Architecture parser initialized");
    return true;
}

void ArchitectureParser::update(double timestep) {
    // Update parsers if needed
}

void ArchitectureParser::shutdown() {
    pImpl->logger->info("Architecture parser shutdown");
    pImpl->parsers.clear();
    pImpl->parsingErrors.clear();
    pImpl->parsingWarnings.clear();
}

bool ArchitectureParser::parse(const std::string& sourcePath, NeuralArchitecture& arch) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    pImpl->stats.parseCount++;
    pImpl->parsingErrors.clear();
    pImpl->parsingWarnings.clear();
    
    // Detect file format from extension
    std::string format = detectFormat(sourcePath);
    
    if (format.empty()) {
        pImpl->logger->error("Cannot detect format from file: " + sourcePath);
        pImpl->parsingErrors.push_back("Unknown file format");
        return false;
    }
    
    // Load file content
    std::string sourceString;
    if (!loadFileContent(sourcePath, sourceString)) {
        pImpl->logger->error("Failed to load file: " + sourcePath);
        return false;
    }
    
    return parseFromString(sourceString, arch);
}

bool ArchitectureParser::parseFromString(const std::string& sourceString, NeuralArchitecture& arch) const {
    std::string format = detectFormat(sourceString);
    
    auto it = pImpl->parsers.find(format);
    if (it == pImpl->parsers.end()) {
        pImpl->logger->error("No parser available for format: " + format);
        pImpl->parsingErrors.push_back("Unsupported format: " + format);
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool success = it->second->parse(sourceString, arch);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime);
    
    pImpl->stats.totalParseTime += duration.count();
    pImpl->stats.averageParseTime = pImpl->stats.totalParseTime / pImpl->stats.parseCount;
    
    if (success) {
        pImpl->stats.successfulParses++;
        pImpl->logger->info("Architecture parsed successfully in " + std::to_string(duration.count()) + " seconds");
    }
    
    return success;
}

bool ArchitectureParser::parseFromYAML(const std::string& sourceString, NeuralArchitecture& arch) const {
    auto it = pImpl->parsers.find("yaml");
    if (it == pImpl->parsers.end()) {
        pImpl->logger->error("YAML parser not available");
        return false;
    }
    
    return it->second->parse(sourceString, arch);
}

bool ArchitectureParser::parseFromJSON(const std::string& sourceString, NeuralArchitecture& arch) const {
    auto it = pImpl->parsers.find("json");
    if (it == pImpl->parsers.end()) {
        pImpl->logger->error("JSON parser not available");
        return false;
    }
    
    return it->second->parse(sourceString, arch);
}

bool ArchitectureParser::validateSyntax(const std::string& sourceString, const std::string& format) const {
    std::string detectedFormat = format;
    if (detectedFormat.empty()) {
        detectedFormat = detectFormat(sourceString);
    }
    
    auto it = pImpl->parsers.find(detectedFormat);
    if (it == pImpl->parsers.end()) {
        pImpl->logger->error("No parser available for format: " + detectedFormat);
        return false;
    }
    
    auto errors = it->second->getSyntaxErrors(sourceString);
    pImpl->parsingErrors = errors;
    
    return errors.empty();
}

bool ArchitectureParser::validateSemantics(const NeuralArchitecture& arch) const {
    // Basic semantic validation
    std::vector<std::string> errors;
    
    // Check for duplicate region names
    std::unordered_set<std::string> regionNames;
    for (const auto& region : arch.regions) {
        if (regionNames.find(region.name) != regionNames.end()) {
            errors.push_back("Duplicate region name: " + region.name);
        }
        regionNames.insert(region.name);
    }
    
    // Check for duplicate population names
    std::unordered_set<std::string> populationNames;
    for (const auto& population : arch.populations) {
        if (populationNames.find(population.name) != populationNames.end()) {
            errors.push_back("Duplicate population name: " + population.name);
        }
        populationNames.insert(population.name);
    }
    
    // Check for duplicate connection identifiers
    std::unordered_set<std::string> connectionIds;
    for (const auto& connection : arch.connections) {
        std::string id = connection.sourceRegion + "->" + connection.targetRegion;
        if (connectionIds.find(id) != connectionIds.end()) {
            errors.push_back("Duplicate connection: " + id);
        }
        connectionIds.insert(id);
    }
    
    pImpl->parsingErrors = errors;
    
    if (!errors.empty()) {
        pImpl->logger->error("Architecture semantic validation failed: " + std::to_string(errors.size()) + " errors");
        return false;
    }
    
    return true;
}

std::vector<std::string> ArchitectureParser::getParsingErrors() const {
    return pImpl->parsingErrors;
}

std::vector<std::string> ArchitectureParser::getParsingWarnings() const {
    return pImpl->parsingWarnings;
}

bool ArchitectureParser::supportsFormat(const std::string& format) const {
    return pImpl->parsers.find(format) != pImpl->parsers.end();
}

std::vector<std::string> ArchitectureParser::getSupportedFormats() const {
    std::vector<std::string> formats;
    formats.reserve(pImpl->parsers.size());
    
    for (const auto& parserPair : pImpl->parsers) {
        formats.push_back(parserPair.first);
    }
    
    return formats;
}

std::string ArchitectureParser::detectFormat(const std::string& sourceString) const {
    // Detect format based on content
    if (sourceString.find("regions:") != std::string::npos || 
        sourceString.find("populations:") != std::string::npos) {
        return "yaml";
    } else if (sourceString.find('"regions"') != std::string::npos || 
               sourceString.find('"populations"') != std::string::npos) {
        return "json";
    }
    
    // Try to detect YAML by checking for indentation
    bool hasIndentation = false;
    size_t pos = 0;
    while ((pos = sourceString.find('\n', pos)) != std::string::npos) {
        size_t nextPos = pos + 1;
        if (nextPos < sourceString.length() && 
            (sourceString[nextPos] == ' ' || sourceString[nextPos] == '\t')) {
            hasIndentation = true;
            break;
        }
        pos = nextPos;
    }
    
    if (hasIndentation) {
        return "yaml";
    }
    
    return "text"; // Default to text format
}

bool ArchitectureParser::loadTemplate(const std::string& templatePath, NeuralArchitecture& arch) const {
    return parse(templatePath, arch);
}

bool ArchitectureParser::saveTemplate(const std::string& templatePath, const NeuralArchitecture& arch) const {
    try {
        std::ofstream file(templatePath);
        if (!file) {
            pImpl->logger->error("Failed to open file for writing: " + templatePath);
            return false;
        }
        
        // Use YAML format for templates
        YAML::Emitter emitter;
        archToYAML(arch, emitter);
        
        file << emitter.c_str();
        pImpl->logger->info("Template saved to: " + templatePath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to save template: " + std::string(e.what()));
        return false;
    }
}

std::string ArchitectureParser::highlightSyntax(const std::string& sourceString, const std::string& format) const {
    // Simple syntax highlighting
    std::stringstream ss;
    ss << "<pre><code>";
    
    // Basic highlighting - in real implementation would be more sophisticated
    ss << "<!-- Syntax highlighting for " << format << " -->\n";
    ss << sourceString;
    
    ss << "</code></pre>";
    return ss.str();
}

std::map<size_t, std::string> ArchitectureParser::getErrorLocations(const std::vector<std::string>& errors) const {
    std::map<size_t, std::string> locations;
    
    // Simple error location detection
    for (size_t i = 0; i < errors.size(); ++i) {
        if (errors[i].find("line") != std::string::npos) {
            // Extract line number from error message
            // In real implementation, would parse error messages
            locations[i] = "Line unknown";
        } else {
            locations[i] = "Unknown location";
        }
    }
    
    return locations;
}

bool ArchitectureParser::addParser(const std::string& format, std::function<bool(const std::string&, NeuralArchitecture&)> parser) {
    if (format.empty() || !parser) {
        pImpl->logger->error("Invalid parser format or function");
        return false;
    }
    
    // Check for duplicate
    if (pImpl->parsers.find(format) != pImpl->parsers.end()) {
        pImpl->logger->warning("Parser already exists for format: " + format);
        return false;
    }
    
    // Create a wrapper IArchitectureParser
    class CustomParser : public IArchitectureParser {
        std::string fmt;
        std::function<bool(const std::string&, NeuralArchitecture&)> parseFunc;
        
    public:
        CustomParser(const std::string& f, std::function<bool(const std::string&, NeuralArchitecture&)> func)
            : fmt(f), parseFunc(func) {}
        
        std::string getFormat() const override { return fmt; }
        bool canParse(const std::string& sourceString) const override { return true; }
        bool parse(const std::string& sourceString, NeuralArchitecture& arch) const override {
            return parseFunc(sourceString, arch);
        }
        std::vector<std::string> getSyntaxErrors(const std::string& sourceString) const override {
            return std::vector<std::string>();
        }
    };
    
    pImpl->parsers[format] = std::make_unique<CustomParser>(format, parser);
    pImpl->logger->info("Parser added for format: " + format);
    return true;
}

bool ArchitectureParser::removeParser(const std::string& format) {
    auto it = pImpl->parsers.find(format);
    if (it != pImpl->parsers.end()) {
        pImpl->parsers.erase(it);
        pImpl->logger->info("Parser removed for format: " + format);
        return true;
    }
    
    pImpl->logger->warning("Parser not found for format: " + format);
    return false;
}

std::vector<std::string> ArchitectureParser::getAvailableParsers() const {
    std::vector<std::string> formats;
    formats.reserve(pImpl->parsers.size());
    
    for (const auto& parserPair : pImpl->parsers) {
        formats.push_back(parserPair.first);
    }
    
    return formats;
}

size_t ArchitectureParser::getParseCount() const {
    return pImpl->stats.parseCount;
}

double ArchitectureParser::getAverageParseTime() const {
    if (pImpl->stats.parseCount == 0) return 0.0;
    return pImpl->stats.averageParseTime;
}

std::string ArchitectureParser::getParserStats() const {
    std::stringstream ss;
    ss << "Parser Statistics:\n";
    ss << "=================\n";
    ss << "Total parses: " << pImpl->stats.parseCount << "\n";
    ss << "Successful parses: " << pImpl->stats.successfulParses << "\n";
    ss << "Average parse time: " << pImpl->stats.averageParseTime << " seconds\n";
    ss << "Supported formats: " << pImpl->parsers.size() << "\n";
    
    return ss.str();
}

// Helper functions
bool ArchitectureParser::loadFileContent(const std::string& filepath, std::string& content) const {
    std::ifstream file(filepath);
    if (!file) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    return true;
}

// YAMLArchitectureParser implementations
bool YAMLArchitectureParser::canParse(const std::string& sourceString) const {
    // Simple YAML detection
    return sourceString.find("regions:") != std::string::npos &&
           sourceString.find("populations:") != std::string::npos;
}

bool YAMLArchitectureParser::parse(const std::string& sourceString, NeuralArchitecture& arch) const {
    try {
        YAML::Node root = YAML::Load(sourceString);
        arch = yamlToArchitecture(root);
        return true;
    } catch (const YAML::Exception& e) {
        return false;
    }
}

std::vector<std::string> YAMLArchitectureParser::getSyntaxErrors(const std::string& sourceString) const {
    try {
        YAML::Node root = YAML::Load(sourceString);
        return std::vector<std::string>();
    } catch (const YAML::Exception& e) {
        return std::vector<std::string>(1, "YAML syntax error: " + std::string(e.what()));
    }
}

// JSONArchitectureParser implementations
bool JSONArchitectureParser::canParse(const std::string& sourceString) const {
    return sourceString.find('"regions"') != std::string::npos &&
           sourceString.find('"populations"') != std::string::npos;
}

bool JSONArchitectureParser::parse(const std::string& sourceString, NeuralArchitecture& arch) const {
    try {
        Json::Value root;
        std::stringstream stream(sourceString);
        Json::CharReaderBuilder builder;
        std::string errors;
        
        if (!builder(&stream, &stream, &root, &errors)) {
            return false;
        }
        
        arch = jsonToArchitecture(root);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> JSONArchitectureParser::getSyntaxErrors(const std::string& sourceString) const {
    try {
        Json::Value root;
        std::stringstream stream(sourceString);
        Json::CharReaderBuilder builder;
        std::string errors;
        
        if (!builder(&stream, &stream, &root, &errors)) {
            return std::vector<std::string>(1, "JSON syntax error: " + errors);
        }
        
        return std::vector<std::string>();
    } catch (const std::exception& e) {
        return std::vector<std::string>(1, "JSON parsing error: " + std::string(e.what()));
    }
}

// TextArchitectureParser implementations
bool TextArchitectureParser::canParse(const std::string& sourceString) const {
    // Simple text format detection
    return !sourceString.empty();
}

bool TextArchitectureParser::parse(const std::string& sourceString, NeuralArchitecture& arch) const {
    // In real implementation, would parse custom text format
    // For now, return empty architecture
    return true;
}

std::vector<std::string> TextArchitectureParser::getSyntaxErrors(const std::string& sourceString) const {
    return std::vector<std::string>();
}

// YAML conversion helpers (simplified versions)
NeuralArchitecture yamlToArchitecture(const YAML::Node& node) const {
    NeuralArchitecture arch;
    
    if (node["name"]) arch.name = node["name"].as<std::string>();
    if (node["version"]) arch.version = node["version"].as<std::string>();
    if (node["description"]) arch.description = node["description"].as<std::string>();
    
    // Parse regions, populations, connections...
    // Implementation would be similar to ArchitectureCompiler
    
    return arch;
}

void archToYAML(const NeuralArchitecture& arch, YAML::Emitter& emitter) const {
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "name" << YAML::Value << arch.name;
    emitter << YAML::Key << "version" << YAML::Value << arch.version;
    emitter << YAML::Key << "description" << YAML::Value << arch.description;
    emitter << YAML::EndMap;
}

// JSON conversion helpers (simplified)
NeuralArchitecture jsonToArchitecture(const Json::Value& node) const {
    NeuralArchitecture arch;
    
    if (node.isMember("name")) arch.name = node["name"].asString();
    if (node.isMember("version")) arch.version = node["version"].asString();
    if (node.isMember("description")) arch.description = node["description"].asString();
    
    return arch;
}

void archToJSON(const NeuralArchitecture& arch, Json::Value& node) const {
    node["name"] = arch.name;
    node["version"] = arch.version;
    node["description"] = arch.description;
}

} // namespace nlm
