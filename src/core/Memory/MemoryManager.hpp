#pragma once

#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;
class ConfigEntry;

// Memory pool for neural objects to reduce allocation/deallocation overhead
// This improves performance by reusing memory blocks for frequently allocated objects

// Neuron memory pool
class NeuronMemoryPool {
public:
    static constexpr size_t DEFAULT_POOL_SIZE = 1000;
    
    NeuronMemoryPool();
    Neuron* allocate();
    void reset();
    size_t size() const;
    
private:
    std::vector<Neuron> pool;
    size_t currentIndex;
};

// Synapse memory pool
class SynapseMemoryPool {
public:
    static constexpr size_t DEFAULT_POOL_SIZE = 5000;
    
    SynapseMemoryPool();
    Synapse* allocate();
    void reset();
    size_t size() const;
    
private:
    std::vector<Synapse> pool;
    size_t currentIndex;
};

// ConfigEntry memory pool
class ConfigEntryMemoryPool {
public:
    static constexpr size_t DEFAULT_POOL_SIZE = 100;
    
    ConfigEntryMemoryPool();
    ConfigEntry* allocate(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "");
    void reset();
    size_t size() const;
    
private:
    std::vector<ConfigEntry> pool;
    size_t currentIndex;
};

// String memory pool
class StringMemoryPool {
public:
    static constexpr size_t DEFAULT_POOL_SIZE = 1000;
    
    StringMemoryPool();
    std::string* allocate(const std::string& str);
    void reset();
    size_t size() const;
    
private:
    std::vector<std::string> pool;
    size_t currentIndex;
};

// Memory manager - central class for all memory pools
class MemoryManager {
public:
    MemoryManager();
    
    Neuron* allocateNeuron();
    Synapse* allocateSynapse();
    ConfigEntry* allocateConfigEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "");
    std::string* allocateString(const std::string& str);
    
    void reset();
    
    size_t getTotalAllocated() const;
    void printStats() const;
    
private:
    std::unique_ptr<NeuronMemoryPool> neuronPool;
    std::unique_ptr<SynapseMemoryPool> synapsePool;
    std::unique_ptr<ConfigEntryMemoryPool> configEntryPool;
    std::unique_ptr<StringMemoryPool> stringPool;
};

// Global memory manager instance
MemoryManager& getMemoryManager();

// Convenience functions for common allocation patterns
Neuron* allocateNeuron();
Synapse* allocateSynapse();
ConfigEntry* allocateConfigEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "");
std::string* allocateString(const std::string& str);

void resetMemoryManager();
void printMemoryStats();

} // namespace nlm
