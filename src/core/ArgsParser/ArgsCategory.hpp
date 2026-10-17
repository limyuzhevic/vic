#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <functional>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace nlm {

// Forward declarations
class ArgsParser;
class ArgsValidator;
class PresetManager;

// Argument categories for organization
enum class ArgsCategory {
    Core,
    Brain,
    Plasticity,
    Neuromodulation,
    Simulation,
    Output,
    Debugging
};

// Argument type definitions
using ArgsValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Argument metadata for validation and help
struct ArgMetadata {
    std::string name;
    std::string description;
    std::string example;
    std::string defaultValue;
    bool required;
    ArgsCategory category;
    bool hasShortOption;
    std::string shortOption;
    bool hasLongOption;
    std::string longOption;
    std::string type;
    std::string validationRegex;
    std::function<bool(const ArgsValue&)> validator;
    std::function<ArgsValue()> presetFactory;
    
    ArgMetadata()
        : name(), description(), example(), defaultValue(), required(false),
          category(ArgsCategory::Core), hasShortOption(false), shortOption(),
          hasLongOption(false), longOption(), type(), validationRegex(),
          validator(nullptr), presetFactory(nullptr) {}
    
    ArgMetadata(const std::string& n, const std::string& desc,
                const std::string& ex, const std::string& def = "",
                bool req = false, ArgsCategory cat = ArgsCategory::Core,
                const std::string& shortOpt = "",
                const std::string& longOpt = "")
        : name(n), description(desc), example(ex), defaultValue(def),
          required(req), category(cat), hasShortOption(!shortOpt.empty()),
          shortOption(shortOpt), hasLongOption(!longOpt.empty()),
          longOption(longOpt), type(), validationRegex(),
          validator(nullptr), presetFactory(nullptr) {
        if (hasShortOption) type = determineTypeFromShortOption(shortOpt);
        if (hasLongOption) type = determineTypeFromLongOption(longOpt);
    }
    
private:
    std::string determineTypeFromShortOption(const std::string& opt) {
        if (opt.length() == 1) {
            char c = opt[0];
            if (c >= '0' && c <= '9') return "int";
            if (c == 'f' || c == 'd') return "double";
            if (c == 'b') return "bool";
            if (c == 's') return "string";
        }
        return "string";
    }
    
    std::string determineTypeFromLongOption(const std::string& opt) {
        if (opt.find("int") != std::string::npos) return "int";
        if (opt.find("double") != std::string::npos || 
            opt.find("float") != std::string::npos ||
            opt.find("time") != std::string::npos) return "double";
        if (opt.find("bool") != std::string::npos ||
            opt.find("enable") != std::string::npos ||
            opt.find("disable") != std::string::npos) return "bool";
        if (opt.find("str") != std::string::npos ||
            opt.find("file") != std::string::npos ||
            opt.find("path") != std::string::npos) return "string";
        return "string";
    }
};

// Configuration schema entry
struct ConfigSchema {
    std::string key;
    ArgsValue defaultValue;
    ConfigSource source;
    std::string description;
    ArgsCategory category;
    std::function<bool(const ArgsValue&)> validator;
    
    ConfigSchema(const std::string& k, const ArgsValue& v, ConfigSource s,
                 const std::string& desc, ArgsCategory cat = ArgsCategory::Core)
        : key(k), defaultValue(v), source(s), description(desc), category(cat),
          validator(nullptr) {}
};

// Command line argument definition
struct CmdArg {
    std::string shortOption;
    std::string longOption;
    std::string value;
    bool hasValue;
    
    CmdArg() : shortOption(), longOption(), value(), hasValue(false) {}
    CmdArg(const std::string& shortOpt, const std::string& longOpt, 
           const std::string& val, bool hasVal)
        : shortOption(shortOpt), longOption(longOpt), value(val), hasValue(hasVal) {}
};

// File format detection result
struct FileFormatInfo {
    std::string filename;
    std::string detectedFormat;
    bool isValid;
    double confidence;
    
    FileFormatInfo(const std::string& fname, const std::string& format,
                   bool valid, double conf)
        : filename(fname), detectedFormat(format), isValid(valid),
          confidence(conf) {}
};

// Preset configuration
struct Preset {
    std::string name;
    std::vector<ConfigSchema> schema;
    std::string description;
    double priority;
    
    Preset(const std::string& n, const std::string& desc, double prio = 1.0)
        : name(n), description(desc), priority(prio) {}
};