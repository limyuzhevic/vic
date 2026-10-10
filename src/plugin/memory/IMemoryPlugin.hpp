// Memory plugin base class interface
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include "../PluginBase.hpp"
#include "../memory/Memory.hpp"

namespace nlm {

class IMemoryPlugin : public IPlugin {
public:
    virtual ~IMemoryPlugin() = default;
    
    // Plugin category identification
    virtual PluginCategory getCategory() const override { return PluginCategory::MEMORY; }
    
    // Memory system creation and management
    virtual std::shared_ptr<Memory> createMemorySystem(uint32_t capacity) = 0;
    virtual bool configureMemorySystem(Memory& memory, const std::map<std::string, std::string>& parameters) = 0;
    virtual std::vector<std::string> getMemorySystemTypes() const = 0;
    
    // Memory operations customization
    virtual bool storeMemory(Memory& memory, const std::string& key, const std::string& value, MemoryPriority priority = MemoryPriority::Normal) = 0;
    virtual bool retrieveMemory(Memory& memory, const std::string& key, std::string& value) = 0;
    virtual bool deleteMemory(Memory& memory, const std::string& key) = 0;
    virtual bool clearMemory(Memory& memory) = 0;
    
    // Memory retrieval strategies
    virtual std::vector<std::string> searchMemory(Memory& memory, const std::string& pattern) = 0;
    virtual bool getMemoryStats(Memory& memory, std::map<std::string, uint32_t>& stats) = 0;
    
    // Memory persistence
    virtual bool saveMemoryState(Memory& memory, const std::string& filepath) = 0;
    virtual bool loadMemoryState(Memory& memory, const std::string& filepath) = 0;
    
    // Plugin-specific configuration
    virtual std::map<std::string, std::string> getDefaultParameters() const = 0;
    virtual std::vector<std::string> getSupportedParameterTypes() const = 0;
    
    // Memory management
    virtual bool initializeMemoryManager() = 0;
    virtual void cleanupMemoryManager() = 0;
};

} // namespace nlm