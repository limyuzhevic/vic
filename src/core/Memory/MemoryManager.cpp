// Memory pool for neural objects to reduce allocation/deallocation overhead
// This improves performance by reusing memory blocks for frequently allocated objects

#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// Memory pool for Neuron objects
class NeuronMemoryPool {
public:
    static constexpr size_t DEFAULT_POOL_SIZE = 1000;
    
    NeuronMemoryPool() : currentIndex(0) {
        pool.resize(DEFAULT_POOL_SIZE);
    }
    
    Neuron* allocate() {
        if (currentIndex >= pool.size()) {
            // Expand pool if needed
            pool.resize(pool.size() + DEFAULT_POOL_SIZE);
        }
        return &pool[currentIndex++];
    }
    
    void reset() {
        currentIndex = 0;
    }
    
    size_t size() const {
        return currentIndex;
    }
    
private:
    std::vector<Neuron> pool;
    size_t currentIndex;
};

// Memory pool for Synapse objects
class SynapseMemoryPool {
public:
    static constexpr size_t DEFAULT_POOL_SIZE = 5000;
    
    SynapseMemoryPool() : currentIndex(0) {
        pool.resize(DEFAULT_POOL_SIZE);
    }
    
    Synapse* allocate() {
        if (currentIndex >= pool.size()) {
            // Expand pool if needed
            pool.resize(pool.size() + DEFAULT_POOL_SIZE);
        }
        return &pool[currentIndex++];
    }
    
    void reset() {
        currentIndex = 0;
    }
    
    size_t size() const {
        return currentIndex;
    }
    
private:
    std::vector<Synapse> pool;
    size_t currentIndex;
};

// Memory pool for ConfigEntry objects
class ConfigEntryMemoryPool {
public:
    static constexpr size_t DEFAULT_POOL_SIZE = 100;
    
    ConfigEntryMemoryPool() : currentIndex(0) {
        pool.resize(DEFAULT_POOL_SIZE);
    }
    
    ConfigEntry* allocate(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "") {
        if (currentIndex >= pool.size()) {
            // Expand pool if needed
            pool.resize(pool.size() + DEFAULT_POOL_SIZE);
        }
        auto* entry = &pool[currentIndex++];
        entry->key = key;
        entry->value = value;
        entry->source = source;
        entry->description = description;
        return entry;
    }
    
    void reset() {
        currentIndex = 0;
    }
    
    size_t size() const {
        return currentIndex;
    }
    
private:
    std::vector<ConfigEntry> pool;
    size_t currentIndex;
};

// Memory pool for std::string objects (common in Config)
class StringMemoryPool {
public:
    static constexpr size_t DEFAULT_POOL_SIZE = 1000;
    
    StringMemoryPool() : currentIndex(0) {
        pool.resize(DEFAULT_POOL_SIZE);
    }
    
    std::string* allocate(const std::string& str) {
        if (currentIndex >= pool.size()) {
            // Expand pool if needed
            pool.resize(pool.size() + DEFAULT_POOL_SIZE);
        }
        auto* stringPtr = &pool[currentIndex++];
        *stringPtr = str;
        return stringPtr;
    }
    
    void reset() {
        currentIndex = 0;
    }
    
    size_t size() const {
        return currentIndex;
    }
    
private:
    std::vector<std::string> pool;
    size_t currentIndex;
};

// Memory manager - central class for all memory pools
class MemoryManager {
public:
    MemoryManager() {
        neuronPool = std::make_unique<NeuronMemoryPool>();
        synapsePool = std::make_unique<SynapseMemoryPool>();
        configEntryPool = std::make_unique<ConfigEntryMemoryPool>();
        stringPool = std::make_unique<StringMemoryPool>();
    }
    
    Neuron* allocateNeuron() {
        return neuronPool->allocate();
    }
    
    Synapse* allocateSynapse() {
        return synapsePool->allocate();
    }
    
    ConfigEntry* allocateConfigEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "") {
        return configEntryPool->allocate(key, value, source, description);
    }
    
    std::string* allocateString(const std::string& str) {
        return stringPool->allocate(str);
    }
    
    void reset() {
        neuronPool->reset();
        synapsePool->reset();
        configEntryPool->reset();
        stringPool->reset();
    }
    
    size_t getTotalAllocated() const {
        return neuronPool->size() + synapsePool->size() + configEntryPool->size() + stringPool->size();
    }
    
    void printStats() const {
        std::cout << "Memory Pool Statistics:" << std::endl;
        std::cout << "  Neurons: " << neuronPool->size() << std::endl;
        std::cout << "  Synapses: " << synapsePool->size() << std::endl;
        std::cout << "  Config Entries: " << configEntryPool->size() << std::endl;
        std::cout << "  Strings: " << stringPool->size() << std::endl;
        std::cout << "  Total: " << getTotalAllocated() << std::endl;
    }
    
private:
    std::unique_ptr<NeuronMemoryPool> neuronPool;
    std::unique_ptr<SynapseMemoryPool> synapsePool;
    std::unique_ptr<ConfigEntryMemoryPool> configEntryPool;
    std::unique_ptr<StringMemoryPool> stringPool;
};

// Global memory manager instance
static MemoryManager globalMemoryManager;

// Convenience functions for common allocation patterns
inline Neuron* allocateNeuron() {
    return globalMemoryManager.allocateNeuron();
}

inline Synapse* allocateSynapse() {
    return globalMemoryManager.allocateSynapse();
}

inline ConfigEntry* allocateConfigEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "") {
    return globalMemoryManager.allocateConfigEntry(key, value, source, description);
}

inline std::string* allocateString(const std::string& str) {
    return globalMemoryManager.allocateString(str);
}

inline void resetMemoryManager() {
    globalMemoryManager.reset();
}

inline void printMemoryStats() {
    globalMemoryManager.printStats();
}

} // namespace nlm
