// MemoryPluginInterface.hpp
#pragma once

#include "../PluginBase.hpp"
#include <memory>
#include <vector>
#include "../../memory/NeuralWorkingMemory.hpp"

namespace nlm {

class MemoryPluginInterface : public virtual PluginBase {
public:
    virtual ~MemoryPluginInterface() = default;

    // Memory system creation
    virtual std::unique_ptr<NeuralWorkingMemory> createMemorySystem(const Config& config) = 0;
    virtual std::unique_ptr<NeuralWorkingMemory> createMemorySystem(MemoryType type, const Config& config) = 0;

    // Memory system configuration
    virtual bool configureMemorySystem(NeuralWorkingMemory* system, const Config& config) = 0;
    virtual bool validateMemorySystemConfiguration(const Config& config) const = 0;

    // Memory properties
    virtual size_t getDefaultCapacity() const = 0;
    virtual float getDefaultDecayRate() const = 0;
    virtual float getDefaultConsolidationThreshold() const = 0;
    virtual size_t getMaxCapacity() const = 0;
    virtual size_t getMinCapacity() const = 0;

    // Memory types
    virtual std::vector<std::string> getSupportedMemoryTypes() const = 0;
    virtual bool supportsCustomEncoding() const = 0;
    virtual bool supportsCustomRetrieval() const = 0;

    // Plugin-specific configuration
    virtual std::string getDefaultConfigurationKey() const = 0;
    virtual Config getDefaultConfiguration() const = 0;

    // Memory operations
    virtual float calculateEncodingStrength(const Config& config) const = 0;
    virtual float calculateRetrievalProbability(float age, const Config& config) const = 0;
    virtual bool shouldConsolidate(float age, const Config& config) const = 0;
};

} // namespace nlm