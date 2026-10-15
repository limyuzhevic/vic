#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <regex>
#include "../arch/ArchitectureCompiler.hpp"

namespace nlm {

class ArchitectureParser {
public:
    ArchitectureParser();
    ~ArchitectureParser();
    
    // Disable copying, enable moving
    ArchitectureParser(const ArchitectureParser&) = delete;
    ArchitectureParser& operator=(const ArchitectureParser&) = delete;
    ArchitectureParser(ArchitectureParser&&) noexcept;
    ArchitectureParser& operator=(ArchitectureParser&&) noexcept;
    
    // Parsing lifecycle
    bool initialize();
    void update(double timestep);
    void shutdown();
    
    // Parsing methods
    bool parse(const std::string& sourcePath, NeuralArchitecture& arch) const;
    bool parseFromString(const std::string& sourceString, NeuralArchitecture& arch) const;
    bool parseFromYAML(const std::string& sourceString, NeuralArchitecture& arch) const;
    bool parseFromJSON(const std::string& sourceString, NeuralArchitecture& arch) const;
    
    // Schema validation
    bool validateSyntax(const std::string& sourceString, const std::string& format = "auto") const;
    bool validateSemantics(const NeuralArchitecture& arch) const;
    std::vector<std::string> getParsingErrors() const;
    std::vector<std::string> getParsingWarnings() const;
    
    // File format support
    bool supportsFormat(const std::string& format) const;
    std::vector<std::string> getSupportedFormats() const;
    std::string detectFormat(const std::string& sourceString) const;
    
    // Template loading
    bool loadTemplate(const std::string& templatePath, NeuralArchitecture& arch) const;
    bool saveTemplate(const std::string& templatePath, const NeuralArchitecture& arch) const;
    
    // Syntax highlighting and error reporting
    std::string highlightSyntax(const std::string& sourceString, const std::string& format) const;
    std::map<size_t, std::string> getErrorLocations(const std::vector<std::string>& errors) const;
    
    // Custom parsers
    bool addParser(const std::string& format, std::function<bool(const std::string&, NeuralArchitecture&)> parser);
    bool removeParser(const std::string& format);
    std::vector<std::string> getAvailableParsers() const;
    
    // Statistics
    size_t getParseCount() const;
    double getAverageParseTime() const;
    std::string getParserStats() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Architecture Parser Interface
class IArchitectureParser {
public:
    virtual ~IArchitectureParser() = default;
    virtual std::string getFormat() const = 0;
    virtual bool canParse(const std::string& sourceString) const = 0;
    virtual bool parse(const std::string& sourceString, NeuralArchitecture& arch) const = 0;
    virtual std::vector<std::string> getSyntaxErrors(const std::string& sourceString) const = 0;
};

// Built-in parser implementations
class YAMLArchitectureParser : public IArchitectureParser {
public:
    std::string getFormat() const override { return "yaml"; }
    bool canParse(const std::string& sourceString) const override;
    bool parse(const std::string& sourceString, NeuralArchitecture& arch) const override;
    std::vector<std::string> getSyntaxErrors(const std::string& sourceString) const override;
};

class JSONArchitectureParser : public IArchitectureParser {
public:
    std::string getFormat() const override { return "json"; }
    bool canParse(const std::string& sourceString) const override;
    bool parse(const std::string& sourceString, NeuralArchitecture& arch) const override;
    std::vector<std::string> getSyntaxErrors(const std::string& sourceString) const override;
};

class TextArchitectureParser : public IArchitectureParser {
public:
    std::string getFormat() const override { return "text"; }
    bool canParse(const std::string& sourceString) const override;
    bool parse(const std::string& sourceString, NeuralArchitecture& arch) const override;
    std::vector<std::string> getSyntaxErrors(const std::string& sourceString) const override;
};

} // namespace nlm
