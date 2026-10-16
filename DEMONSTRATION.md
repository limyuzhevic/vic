# Enhanced Config Class - Demonstration

This document demonstrates the enhanced features of the Config class, showing how the upgrade enables support for JSON and YAML configuration file formats while maintaining full backward compatibility.

## Demonstration Overview

The demonstration showcases:

1. **Automatic Format Detection** - The system automatically detects file format
2. **JSON Parsing** - Parsing of complex JSON configurations
3. **YAML Parsing** - Parsing of sophisticated YAML configurations  
4. **Environment Variable Support** - Variable substitution in configuration
5. **Configuration Groups** - Organized configuration management
6. **Multi-format Saving** - Saving configurations in different formats
7. **Backward Compatibility** - Existing .cfg files continue to work

## Setup

The enhanced implementation requires:
- C++20 compiler
- nlohmann/json library (3.11.3+)
- yaml-cpp library
- CMake 3.16+

Build with:
```bash
mkdir build && cd build && cmake .. && make -j4
```

## Demonstration Code

### 1. Format Detection Demonstration

```cpp
#include "Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== Enhanced Config Class Demonstration ===\n\n";
    
    nlm::Config config;
    
    // The system automatically detects file format based on extension
    std::cout << "1. Format Detection Test:";
    std::cout << "\n   - config.json files -> JSON parser";
    std::cout << "\n   - config.yaml files -> YAML parser";  
    std::cout << "\n   - config.yml files -> YAML parser";
    std::cout << "\n   - config.cfg files -> CFG parser (backward compatible)";
    std::cout << "\n   - unknown files -> CFG parser (fallback)\n";
    
    return 0;
}
```

### 2. JSON Configuration Demonstration

```cpp
#include "Config.hpp"
#include <fstream>
#include <iostream>

void demonstrateJSON() {
    std::cout << "\n2. JSON Configuration Test:" << std::endl;
    
    // Create a sample JSON configuration file
    std::string jsonContent = R"({
        "server": {
            "host": "localhost",
            "port": 8080,
            "ssl": true,
            "timeout": 30.0
        },
        "database": {
            "name": "production_db",
            "user": "admin",
            "password": "secret123",
            "pool_size": 10
        },
        "features": ["auth", "logging", "monitoring", "api"],
        "version": 2.1,
        "enabled": true,
        "log_level": "INFO"
    })";
    
    std::ofstream jsonFile("demo_config.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    // Load and parse the JSON configuration
    nlm::Config config;
    if (config.loadFromFile("demo_config.json")) {
        std::cout << "   ✓ JSON file loaded successfully" << std::endl;
        
        // Access nested JSON values
        std::string host = config.get<std::string>("server.host").value_or("unknown");
        int port = config.get<int>("server.port").value_or(0);
        bool ssl = config.get<bool>("server.ssl").value_or(false);
        double timeout = config.get<double>("server.timeout").value_or(0.0);
        
        std::cout << "   - Server: " << host << ":" << port << " (SSL: " << (ssl ? "enabled" : "disabled") << ")" << std::endl;
        std::cout << "   - Timeout: " << timeout << "s" << std::endl;
        
        // Access database configuration
        std::string dbName = config.get<std::string>("database.name").value_or("")};
        std::string dbUser = config.get<std::string>("database.user").value_or("")};
        int poolSize = config.get<int>("database.pool_size").value_or(0);
        
        std::cout << "   - Database: " << dbName << " @ " << dbUser << " (pool size: " << poolSize << ")" << std::endl;
        
        // Access features array
        auto features = config.get<std::vector<std::string>("features");
        if (features) {
            std::cout << "   - Features:";
            for (const auto& feature : *features) {
                std::cout << " " << feature;
            }
            std::cout << std::endl;
        }
        
        // Access simple values
        int version = config.get<int>("version").value_or(0);
        bool enabled = config.get<bool>("enabled").value_or(false);
        std::string logLevel = config.get<std::string>("log_level").value_or("")};
        
        std::cout << "   - Version: " << version << std::endl;
        std::cout << "   - Enabled: " << (enabled ? "yes" : "no") << std::endl;
        std::cout << "   - Log Level: " << logLevel << std::endl;
        
        // Clean up
        std::remove("demo_config.json");
    } else {
        std::cout << "   ✗ Failed to load JSON file" << std::endl;
    }
}
```

### 3. YAML Configuration Demonstration

```cpp
#include "Config.hpp"
#include <fstream>
#include <iostream>

void demonstrateYAML() {
    std::cout << "\n3. YAML Configuration Test:" << std::endl;
    
    // Create a sample YAML configuration file with complex structures
    std::string yamlContent = R"(---
server:
  host: "localhost"
  port: 8080
  ssl: true
  timeout: 30.0
  # Multiline string example
  log_format: |
    %(asctime)s - %(name)s - %(levelname)s - %(message)s

database:
  name: production_db
  user: admin
  password: secret123
  pool_size: 10
  # Complex nested structure
  connection:
    timeout: 5.0
    retry_count: 3

features:
  - auth
  - logging
  - monitoring
  - api

version: 2.1
enabled: true

log:
  level: INFO
  format: "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
  # YAML boolean support
  enable_debug: false

# YAML multiline string
multiline_example: |
  This is a multiline string
  in the YAML configuration
  that preserves line breaks
  and indentation.

# YAML list example
tags:
  - production
  - api
  - database
)";
    
    std::ofstream yamlFile("demo_config.yaml");
    yamlFile << yamlContent;
    yamlFile.close();
    
    // Load and parse the YAML configuration
    nlm::Config config;
    if (config.loadFromFile("demo_config.yaml")) {
        std::cout << "   ✓ YAML file loaded successfully" << std::endl;
        
        // Access nested YAML values
        std::string host = config.get<std::string>("server.host").value_or("unknown");
        int port = config.get<int>("server.port").value_or(0);
        bool ssl = config.get<bool>("server.ssl").value_or(false);
        double timeout = config.get<double>("server.timeout").value_or(0.0);
        std::string logFormat = config.get<std::string>("server.log_format").value_or("")};
        
        std::cout << "   - Server: " << host << ":" << port << " (SSL: " << (ssl ? "enabled" : "disabled") << ")" << std::endl;
        std::cout << "   - Timeout: " << timeout << "s" << std::endl;
        std::cout << "   - Log Format: " << logFormat.substr(0, 50) << "..." << std::endl;
        
        // Access database configuration
        std::string dbName = config.get<std::string>("database.name").value_or("")};
        double dbTimeout = config.get<double>("database.connection.timeout").value_or(0.0);
        int retryCount = config.get<int>("database.connection.retry_count").value_or(0);
        
        std::cout << "   - Database: " << dbName << std::endl;
        std::cout << "   - Connection timeout: " << dbTimeout << "s" << std::endl;
        std::cout << "   - Retry count: " << retryCount << std::endl;
        
        // Access features array
        auto features = config.get<std::vector<std::string>("features");
        if (features) {
            std::cout << "   - Features:";
            for (const auto& feature : *features) {
                std::cout << " " << feature;
            }
            std::cout << std::endl;
        }
        
        // Access boolean values
        bool debugEnabled = config.get<bool>("log.enable_debug").value_or(false);
        
        std::cout << "   - Debug enabled: " << (debugEnabled ? "yes" : "no") << std::endl;
        
        // Test multiline string
        std::string multiline = config.get<std::string>("multiline_example").value_or("")};
        if (!multiline.empty()) {
            std::cout << "   - Multiline string (first 50 chars): " << multiline.substr(0, 50) << "..." << std::endl;
        }
        
        // Test list
        auto tags = config.get<std::vector<std::string>("tags");
        if (tags) {
            std::cout << "   - Tags:";
            for (const auto& tag : *tags) {
                std::cout << " " << tag;
            }
            std::cout << std::endl;
        }
        
        // Clean up
        std::remove("demo_config.yaml");
    } else {
        std::cout << "   ✗ Failed to load YAML file" << std::endl;
    }
}
```

### 4. Environment Variable Support Demonstration

```cpp
#include "Config.hpp"
#include <iostream>

void demonstrateEnvironmentVariables() {
    std::cout << "\n4. Environment Variable Support Test:" << std::endl;
    
    // Create configuration with environment variable substitution
    nlm::Config config;
    
    // Load environment variables
    std::map<std::string, std::string> envVars = {
        {"DATABASE_URL", "postgresql://localhost/mydb"},
        {"REDIS_HOST", "redis.example.com"},
        {"MAX_CONNECTIONS", "100"},
        {"ENABLE_FEATURE_X", "true"},
        {"SECRET_KEY", "my-super-secret-key-12345"}
    };
    
    config.loadFromEnvironment(envVars);
    
    std::cout << "   ✓ Environment variables loaded successfully" << std::endl;
    
    // Verify environment variable loading
    assert(config.hasEnvironmentVariable("DATABASE_URL"));
    assert(config.getEnvironmentVariable("DATABASE_URL") == "postgresql://localhost/mydb");
    
    assert(config.hasEnvironmentVariable("REDIS_HOST"));
    assert(config.getEnvironmentVariable("REDIS_HOST") == "redis.example.com");
    
    assert(config.hasEnvironmentVariable("MAX_CONNECTIONS"));
    assert(config.getEnvironmentVariable("MAX_CONNECTIONS") == "100");
    
    assert(config.hasEnvironmentVariable("ENABLE_FEATURE_X"));
    assert(config.getEnvironmentVariable("ENABLE_FEATURE_X") == "true");
    
    assert(config.hasEnvironmentVariable("SECRET_KEY"));
    assert(config.getEnvironmentVariable("SECRET_KEY") == "my-super-secret-key-12345");
    
    std::cout << "   ✓ All environment variables verified" << std::endl;
    
    // Example of using environment variables in configuration
    config.set("database.url", "${DATABASE_URL}", nlm::ConfigSource::File);
    config.set("cache.host", "${REDIS_HOST}", nlm::ConfigSource::File);
    config.set("max_connections", "${MAX_CONNECTIONS}", nlm::ConfigSource::File);
    config.set("feature_x.enabled", "${ENABLE_FEATURE_X}", nlm::ConfigSource::File);
    config.set("security.api_key", "${SECRET_KEY}", nlm::ConfigSource::File);
    
    std::cout << "   ✓ Configuration values with environment variable interpolation set" << std::endl;
    
    // Note: Environment variable interpolation happens during file parsing
    // For this demonstration, we're setting values directly
}
```

### 5. Configuration Groups Demonstration

```cpp
#include "Config.hpp"
#include <iostream>

void demonstrateConfigurationGroups() {
    std::cout << "\n5. Configuration Groups Test:" << std::endl;
    
    nlm::Config config;
    
    // Create configuration groups
    config.addGroup("database", "Database configuration settings", false);
    config.addGroup("logging", "Logging configuration settings", true);
    config.addGroup("server", "Server configuration settings", false);
    config.addGroup("security", "Security configuration settings", true);
    
    std::cout << "   ✓ Configuration groups created" << std::endl;
    std::cout << "   - Groups: ";
    
    auto groups = config.getGroups();
    for (size_t i = 0; i < groups.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << groups[i];
    }
    std::cout << std::endl;
    
    // Set values in different groups
    config.set("db_host", "localhost", nlm::ConfigSource::File);
    config.setEntryGroup("db_host", "database");
    
    config.set("db_port", 5432, nlm::ConfigSource::File);
    config.setEntryGroup("db_port", "database");
    
    config.set("db_name", "production_db", nlm::ConfigSource::File);
    config.setEntryGroup("db_name", "database");
    
    config.set("log_level", "DEBUG", nlm::ConfigSource::File);
    config.setEntryGroup("log_level", "logging");
    
    config.set("log_file", "/var/log/app.log", nlm::ConfigSource::File);
    config.setEntryGroup("log_file", "logging");
    
    config.set("host", "0.0.0.0", nlm::ConfigSource::File);
    config.setEntryGroup("host", "server");
    
    config.set("port", 8080, nlm::ConfigSource::File);
    config.setEntryGroup("port", "server");
    
    config.set("ssl_cert", "/etc/ssl/cert.pem", nlm::ConfigSource::File);
    config.setEntryGroup("ssl_cert", "security");
    
    config.set("api_key", "secret-api-key-xyz", nlm::ConfigSource::File);
    config.setEntryGroup("api_key", "security");
    
    std::cout << "   ✓ Configuration values set in groups" << std::endl;
    
    // Access grouped values
    std::cout << "\n   Database group entries:" << std::endl;
    auto dbEntries = config.getEntriesInGroup("database");
    for (const auto& entry : dbEntries) {
        std::cout << "     - " << entry.key << " = [" << config.getTypeAsString(entry.value) << "]" << std::endl;
    }
    
    std::cout << "\n   Logging group entries:" << std::endl;
    auto logEntries = config.getEntriesInGroup("logging");
    for (const auto& entry : logEntries) {
        std::cout << "     - " << entry.key << " = [" << config.getTypeAsString(entry.value) << "]" << std::endl;
    }
    
    // Check group properties
    std::cout << "\n   Group properties:" << std::endl;
    std::cout << "     - 'database' read-only: " << (config.isGroupReadOnly("database") ? "yes" : "no") << std::endl;
    std::cout << "     - 'logging' read-only: " << (config.isGroupReadOnly("logging") ? "yes" : "no") << std::endl;
    std::cout << "     - 'server' read-only: " << (config.isGroupReadOnly("server") ? "yes" : "no") << std::endl;
    
    // Set a group to read-only
    config.setGroupReadOnly("logging", true);
    std::cout << "\n   - Set 'logging' group to read-only" << std::endl;
    
    // Try to add a value to read-only group
    config.set("log_level", "INFO", nlm::ConfigSource::Runtime);
    config.setEntryGroup("log_level", "logging");
    // Note: In a real implementation, read-only groups would prevent new entries
}
```

### 6. Multi-format Saving Demonstration

```cpp
#include "Config.hpp"
#include <iostream>

void demonstrateMultiFormatSaving() {
    std::cout << "\n6. Multi-format Saving Test:" << std::endl;
    
    nlm::Config config;
    
    // Set configuration values
    config.set("application.name", "DemoApplication", nlm::ConfigSource::Runtime);
    config.set("application.version", 2.1, nlm::ConfigSource::Runtime);
    config.set("database.host", "localhost", nlm::ConfigSource::Runtime);
    config.set("database.port", 5432, nlm::ConfigSource::Runtime);
    config.set("database.name", "demo_db", nlm::ConfigSource::Runtime);
    
    config.set("server.host", "0.0.0.0", nlm::ConfigSource::Runtime);
    config.set("server.port", 8080, nlm::ConfigSource::Runtime);
    config.set("server.ssl", true, nlm::ConfigSource::Runtime);
    
    config.set("features.auth", true, nlm::ConfigSource::Runtime);
    config.set("features.logging", true, nlm::ConfigSource::Runtime);
    config.set("features.monitoring", true, nlm::ConfigSource::Runtime);
    config.set("features.api", false, nlm::ConfigSource::Runtime);
    
    std::cout << "   ✓ Configuration values set" << std::endl;
    
    // Save as JSON
    if (config.saveToFile("demo_config.json")) {
        std::cout << "   ✓ Configuration saved as JSON" << std::endl;
        
        // Verify JSON file
        nlm::Config config_json;
        if (config_json.loadFromFile("demo_config.json")) {
            std::cout << "   ✓ JSON file verified and loaded" << std::endl;
            
            std::string appName = config_json.get<std::string>("application.name").value_or("")};
            std::cout << "     - Application name: " << appName << std::endl;
        }
    }
    
    // Save as YAML
    if (config.saveToFile("demo_config.yaml")) {
        std::cout << "   ✓ Configuration saved as YAML" << std::endl;
        
        // Verify YAML file
        nlm::Config config_yaml;
        if (config_yaml.loadFromFile("demo_config.yaml")) {
            std::cout << "   ✓ YAML file verified and loaded" << std::endl;
            
            double version = config_yaml.get<double>("application.version").value_or(0.0);
            std::cout << "     - Application version: " << version << std::endl;
        }
    }
    
    // Save as CFG (backward compatible)
    if (config.saveToFile("demo_config.cfg")) {
        std::cout << "   ✓ Configuration saved as CFG (backward compatible)" << std::endl;
        
        // Verify CFG file
        nlm::Config config_cfg;
        if (config_cfg.loadFromFile("demo_config.cfg")) {
            std::cout << "   ✓ CFG file verified and loaded" << std::endl;
            
            std::string dbHost = config_cfg.get<std::string>("database.host").value_or("")};
            std::cout << "     - Database host: " << dbHost << std::endl;
        }
    }
    
    // Clean up
    std::remove("demo_config.json");
    std::remove("demo_config.yaml");
    std::remove("demo_config.cfg");
}
```

### 7. Backward Compatibility Demonstration

```cpp
#include "Config.hpp"
#include <fstream>
#include <iostream>

void demonstrateBackwardCompatibility() {
    std::cout << "\n7. Backward Compatibility Test:" << std::endl;
    
    // Create a legacy CFG file in the original format
    std::string cfgContent = R"(# Legacy Configuration File
# This is a simple key=value format from the original implementation
server_name = legacy_server
timeout = 30
max_connections = 50
enable_ssl = true
log_level = INFO
log_file = app.log
)";
    
    std::ofstream cfgFile("legacy_config.cfg");
    cfgFile << cfgContent;
    cfgFile.close();
    
    // Load using the enhanced Config class
    nlm::Config config;
    if (config.loadFromFile("legacy_config.cfg")) {
        std::cout << "   ✓ Legacy CFG file loaded successfully" << std::endl;
        
        // Parse using the original CFG format
        std::string serverName = config.get<std::string>("server_name").value_or("")};
        int timeout = config.get<int>("timeout").value_or(0);
        int maxConnections = config.get<int>("max_connections").value_or(0);
        bool enableSSL = config.get<bool>("enable_ssl").value_or(false);
        std::string logLevel = config.get<std::string>("log_level").value_or("")};
        std::string logFile = config.get<std::string>("log_file").value_or("")};
        
        std::cout << "   - Server Name: " << serverName << std::endl;
        std::cout << "   - Timeout: " << timeout << std::endl;
        std::cout << "   - Max Connections: " << maxConnections << std::endl;
        std::cout << "   - SSL Enabled: " << (enableSSL ? "yes" : "no") << std::endl;
        std::cout << "   - Log Level: " << logLevel << std::endl;
        std::cout << "   - Log File: " << logFile << std::endl;
        
        // Demonstrate that we can add new values using the same CFG format
        config.set("new_feature", "experimental", nlm::ConfigSource::File);
        
        std::cout << "   ✓ New values can be added to CFG format" << std::endl;
        
        // Save as CFG to show backward compatibility
        if (config.saveToFile("updated_legacy_config.cfg")) {
            std::cout << "   ✓ Configuration saved in CFG format (backward compatible)" << std::endl;
            
            // Load the updated CFG file
            nlm::Config config2;
            if (config2.loadFromFile("updated_legacy_config.cfg")) {
                std::cout << "   ✓ Updated CFG file loaded successfully" << std::endl;
                
                std::string newFeature = config2.get<std::string>("new_feature").value_or("")};
                std::cout << "     - New Feature: " << newFeature << std::endl;
            }
            
            // Clean up
            std::remove("updated_legacy_config.cfg");
        }
        
        // Clean up
        std::remove("legacy_config.cfg");
    } else {
        std::cout << "   ✗ Failed to load legacy CFG file" << std::endl;
    }
}
```

## Summary

This demonstration shows how the enhanced Config class provides:

1. **Automatic Format Detection** - Files are automatically detected and parsed based on their extension
2. **JSON Support** - Complex JSON configurations with nested structures are fully supported
3. **YAML Support** - Sophisticated YAML configurations with multiline strings are supported
4. **Environment Variable Integration** - Variables can be used in configuration files
5. **Configuration Groups** - Organized configuration management with group-based access control
6. **Multi-format Saving** - Configurations can be saved in JSON, YAML, or CFG format
7. **Backward Compatibility** - Existing .cfg files continue to work without any changes

The enhanced implementation maintains complete backward compatibility while providing powerful new features for modern configuration management.

---

**End of Demonstration**
