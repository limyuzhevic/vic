#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "Config.hpp"

namespace nlm {

// Base configuration source
class ConfigSource {
public:
    virtual ~ConfigSource() = default;
    
    // Load configuration into config object
    virtual bool load(Config& config) const = 0;
    
    // Save configuration from config object
    virtual bool save(const Config& config) const = 0;
    
    // Get source type name
    virtual std::string getTypeName() const = 0;
    
    // Check if source is writable
    virtual bool isWritable() const { return false; }
    
    // Get source priority
    virtual int getPriority() const { return 0; }
};

// JSON configuration source
class JSONConfigSource : public ConfigSource {
public:
    JSONConfigSource(const std::string& filepath, int priority = 0);
    
    bool load(Config& config) const override;
    bool save(const Config& config) const override;
    std::string getTypeName() const override { return "JSON"; }
    bool isWritable() const override { return true; }
    int getPriority() const override { return priority; }

private:
    std::string filepath;
    int priority;
};

// YAML configuration source
class YAMLConfigSource : public ConfigSource {
public:
    YAMLConfigSource(const std::string& filepath, int priority = 0);
    
    bool load(Config& config) const override;
    bool save(const Config& config) const override;
    std::string getTypeName() const override { return "YAML"; }
    bool isWritable() const override { return true; }
    int getPriority() const override { return priority; }

private:
    std::string filepath;
    int priority;
};

// INI configuration source
class INIConfigSource : public ConfigSource {
public:
    INIConfigSource(const std::string& filepath, int priority = 0);
    
    bool load(Config& config) const override;
    bool save(const Config& config) const override;
    std::string getTypeName() const override { return "INI"; }
    bool isWritable() const override { return true; }
    int getPriority() const override { return priority; }

private:
    std::string filepath;
    int priority;
};

// Environment variable configuration source
class EnvironmentConfigSource : public ConfigSource {
public:
    EnvironmentConfigSource(int priority = 0);
    
    bool load(Config& config) const override;
    bool save(const Config& config) const override;
    std::string getTypeName() const override { return "Environment"; }
    bool isWritable() const override { return false; }
    int getPriority() const override { return priority; }
    
    // Add environment variable mapping
    void addMapping(const std::string& envVar, const std::string& configKey);
    
    // Remove environment variable mapping
    void removeMapping(const std::string& envVar);

private:
    int priority;
    std::unordered_map<std::string, std::string> mappings;
};

// Command line configuration source
class CommandLineConfigSource : public ConfigSource {
public:
    CommandLineConfigSource(int priority = 0);
    
    bool load(Config& config) const override;
    bool save(const Config& config) const override;
    std::string getTypeName() const override { return "CommandLine"; }
    bool isWritable() const override { return true; }
    int getPriority() const override { return priority; }
    
    // Parse command line arguments
    bool parseArgs(int argc, char** argv);

private:
    int priority;
};

// Registry configuration source (placeholder for external storage)
class RegistryConfigSource : public ConfigSource {
public:
    RegistryConfigSource(const std::string& registryPath, int priority = 0);
    
    bool load(Config& config) const override;
    bool save(const Config& config) const override;
    std::string getTypeName() const override { return "Registry"; }
    bool isWritable() const override { return true; }
    int getPriority() const override { return priority; }

private:
    std::string registryPath;
    int priority;
};

} // namespace nlm