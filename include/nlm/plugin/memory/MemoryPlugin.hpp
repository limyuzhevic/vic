#pragma once

#include "NLMPlugin.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Forward declarations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

/**
 * @class MemoryPlugin
 * @brief Plugin interface for custom memory systems
 */
class MemoryPlugin : public virtual NLMPlugin {
public:
    virtual ~MemoryPlugin() = default;
    
    // Type check
    bool isMemoryPlugin() const override { return true; }
    
    // Memory factory methods
    virtual void* getMemoryFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for memory
    virtual NeuralWorkingMemory* createWorkingMemory() = 0;
    virtual NeuralEpisodicMemory* createEpisodicMemory() = 0;
    virtual NeuralAssociativeMemory* createAssociativeMemory() = 0;
    virtual bool canCreateMemorySystem(const std::string& memoryType) const = 0;
    virtual std::vector<std::string> getSupportedMemorySystemTypes() const = 0;
    
    // Memory configuration
    virtual bool configureMemorySystem(void* memory, const std::string& config) = 0;
    virtual std::string getMemorySystemConfig(void* memory) const = 0;
};

/**
 * @class NeuromodulationPlugin
 * @brief Plugin interface for custom neuromodulators
 */
class NeuromodulationPlugin : public virtual NLMPlugin {
public:
    virtual ~NeuromodulationPlugin() = default;
    
    // Type check
    bool isNeuromodulationPlugin() const override { return true; }
    
    // Neuromodulation factory methods
    virtual void* getNeuromodulationFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for neuromodulation
    virtual class Neuromodulator* createNeuromodulator() = 0;
    virtual bool canCreateNeuromodulator(const std::string& neuromodulatorType) const = 0;
    virtual std::vector<std::string> getSupportedNeuromodulatorTypes() const = 0;
    
    // Neuromodulation configuration
    virtual bool configureNeuromodulator(void* neuromodulator, const std::string& config) = 0;
    virtual std::string getNeuromodulatorConfig(void* neuromodulator) const = 0;
};

} // namespace nlm