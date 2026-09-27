pragma once

#include "MemoryManager.hpp"
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {
namespace CustomAllocators {

// Forward declarations for neural component implementations
struct NeuronImpl;
struct SynapseImpl;
struct PlasticityRuleImpl;

// Neuron pool allocator for high-frequency neuron creation/destruction
class NeuronAllocator {
private:
    struct NeuronBlock {
        void* neurons;
        size_t neuronCount;
        bool isFree;
        uint64_t blockId;
        
        NeuronBlock(size_t count, uint64_t id)
            : neurons(nullptr), neuronCount(count), isFree(true), blockId(id) {
            neurons = std::malloc(count * sizeof(NeuronImpl));
            if (!neurons) throw std::bad_alloc();
        }
        
        ~NeuronBlock() {
            std::free(neurons);
        }
    };
    
    std::vector<NeuronBlock> blocks;
    std::unordered_map<uint64_t, size_t> neuronToBlock;
    std::unordered_map<uint64_t, size_t> neuronIndex;
    size_t neuronSize;
    uint64_t nextBlockId;
    mutable std::mutex mutex;
    
public:
    NeuronAllocator() : neuronSize(sizeof(NeuronImpl)), nextBlockId(0) {}
    
    ~NeuronAllocator() {
        for (auto& block : blocks) {
            std::free(block.neurons);
        }
    }
    
    void* allocateNeuron() {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Try to find a free neuron in existing blocks
        for (auto& block : blocks) {
            if (block.isFree) {
                // This is a whole free block - we need smaller allocations
                // For now, just mark as allocated and return
                block.isFree = false;
                void* neuronPtr = block.neurons;
                // Initialize neuron
                new (neuronPtr) NeuronImpl();
                uint64_t neuronId = nextBlockId * 1000 + 0;  // Simple ID generation
                neuronToBlock[neuronId] = block.blockId;
                neuronIndex[neuronId] = 0;
                return neuronPtr;
            }
        }
        
        // Need to create a new block
        NeuronBlock newBlock(100, nextBlockId++);  // 100 neurons per block
        newBlock.isFree = false;
        blocks.push_back(std::move(newBlock));
        
        // Initialize all neurons in the block
        char* blockData = static_cast<char*>(blocks.back().neurons);
        for (size_t i = 0; i < blocks.back().neuronCount; ++i) {
            new (blockData + i * neuronSize) NeuronImpl();
        }
        
        void* neuronPtr = blocks.back().neurons;
        uint64_t neuronId = blocks.back().blockId * 1000 + 0;
        neuronToBlock[neuronId] = blocks.back().blockId;
        neuronIndex[neuronId] = 0;
        
        return neuronPtr;
    }
    
    void freeNeuron(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Find the block containing this neuron
        for (auto& block : blocks) {
            if (block.neurons == ptr) {
                // Destroy neuron
                reinterpret_cast<NeuronImpl*>(ptr)->~NeuronImpl();
                break;
            }
        }
    }
    
    NeuronImpl* getNeuron(uint64_t neuronId) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto blockIt = neuronToBlock.find(neuronId);
        if (blockIt == neuronToBlock.end()) return nullptr;
        
        size_t blockId = blockIt->second;
        auto indexIt = neuronIndex.find(neuronId);
        if (indexIt == neuronIndex.end()) return nullptr;
        
        size_t index = indexIt->second;
        
        for (auto& block : blocks) {
            if (block.blockId == blockId) {
                return reinterpret_cast<NeuronImpl*>(
                    static_cast<char*>(block.neurons) + index * neuronSize
                );
            }
        }
        
        return nullptr;
    }
    
    size_t getTotalNeuronCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t total = 0;
        for (const auto& block : blocks) {
            if (!block.isFree) {
                total += block.neuronCount;
            }
        }
        return total;
    }
    
    size_t getMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t total = 0;
        for (const auto& block : blocks) {
            total += block.neuronCount * neuronSize;
        }
        return total;
    }
    
    std::string getStats() const {
        std::stringstream ss;
        ss << "Neuron Allocator Statistics:\n";
        ss << "  Neuron size: " << neuronSize << " bytes\n";
        ss << "  Number of blocks: " << blocks.size() << "\n";
        ss << "  Total neurons allocated: " << getTotalNeuronCount() << "\n";
        ss << "  Total memory usage: " << getMemoryUsage() << " bytes\n";
        ss << "  Average neurons per block: " << (blocks.size() > 0 ? 
            static_cast<float>(getTotalNeuronCount()) / blocks.size() : 0.0f) << "\n";
        
        return ss.str();
    }
};

// Synapse pool allocator for high-frequency synapse operations
class SynapseAllocator {
private:
    struct SynapseBlock {
        void* synapses;
        size_t synapseCount;
        bool isFree;
        uint64_t blockId;
        
        SynapseBlock(size_t count, uint64_t id)
            : synapses(nullptr), synapseCount(count), isFree(true), blockId(id) {
            synapses = std::malloc(count * sizeof(SynapseImpl));
            if (!synapses) throw std::bad_alloc();
        }
        
        ~SynapseBlock() {
            std::free(synapses);
        }
    };
    
    std::vector<SynapseBlock> blocks;
    std::unordered_map<uint64_t, size_t> synapseToBlock;
    std::unordered_map<uint64_t, size_t> synapseIndex;
    size_t synapseSize;
    uint64_t nextBlockId;
    mutable std::mutex mutex;
    
public:
    SynapseAllocator() : synapseSize(sizeof(SynapseImpl)), nextBlockId(0) {}
    
    ~SynapseAllocator() {
        for (auto& block : blocks) {
            std::free(block.synapses);
        }
    }
    
    void* allocateSynapse() {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Try to find a free synapse in existing blocks
        for (auto& block : blocks) {
            if (block.isFree) {
                block.isFree = false;
                void* synapsePtr = block.synapses;
                new (synapsePtr) SynapseImpl();
                uint64_t synapseId = nextBlockId * 1000 + 0;
                synapseToBlock[synapseId] = block.blockId;
                synapseIndex[synapseId] = 0;
                return synapsePtr;
            }
        }
        
        // Need to create a new block
        SynapseBlock newBlock(1000, nextBlockId++);  // 1000 synapses per block
        newBlock.isFree = false;
        blocks.push_back(std::move(newBlock));
        
        // Initialize all synapses in the block
        char* blockData = static_cast<char*>(blocks.back().synapses);
        for (size_t i = 0; i < blocks.back().synapseCount; ++i) {
            new (blockData + i * synapseSize) SynapseImpl();
        }
        
        void* synapsePtr = blocks.back().synapses;
        uint64_t synapseId = blocks.back().blockId * 1000 + 0;
        synapseToBlock[synapseId] = blocks.back().blockId;
        synapseIndex[synapseId] = 0;
        
        return synapsePtr;
    }
    
    void freeSynapse(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto& block : blocks) {
            if (block.synapses == ptr) {
                reinterpret_cast<SynapseImpl*>(ptr)->~SynapseImpl();
                break;
            }
        }
    }
    
    SynapseImpl* getSynapse(uint64_t synapseId) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto blockIt = synapseToBlock.find(synapseId);
        if (blockIt == synapseToBlock.end()) return nullptr;
        
        size_t blockId = blockIt->second;
        auto indexIt = synapseIndex.find(synapseId);
        if (indexIt == synapseIndex.end()) return nullptr;
        
        size_t index = indexIt->second;
        
        for (auto& block : blocks) {
            if (block.blockId == blockId) {
                return reinterpret_cast<SynapseImpl*>(
                    static_cast<char*>(block.synapses) + index * synapseSize
                );
            }
        }
        
        return nullptr;
    }
    
    size_t getTotalSynapseCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t total = 0;
        for (const auto& block : blocks) {
            if (!block.isFree) {
                total += block.synapseCount;
            }
        }
        return total;
    }
    
    size_t getMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t total = 0;
        for (const auto& block : blocks) {
            total += block.synapseCount * synapseSize;
        }
        return total;
    }
    
    std::string getStats() const {
        std::stringstream ss;
        ss << "Synapse Allocator Statistics:\n";
        ss << "  Synapse size: " << synapseSize << " bytes\n";
        ss << "  Number of blocks: " << blocks.size() << "\n";
        ss << "  Total synapses allocated: " << getTotalSynapseCount() << "\n";
        ss << "  Total memory usage: " << getMemoryUsage() << " bytes\n";
        ss << "  Average synapses per block: " << (blocks.size() > 0 ? 
            static_cast<float>(getTotalSynapseCount()) / blocks.size() : 0.0f) << "\n";
        
        return ss.str();
    }
};

// Memory pool for neural plasticity rule objects (STDP, Hebbian, etc.)
class PlasticityRuleAllocator {
private:
    struct PlasticityBlock {
        void* rules;
        size_t ruleCount;
        bool isFree;
        uint64_t blockId;
        
        PlasticityBlock(size_t count, uint64_t id)
            : rules(nullptr), ruleCount(count), isFree(true), blockId(id) {
            rules = std::malloc(count * sizeof(PlasticityRuleImpl));
            if (!rules) throw std::bad_alloc();
        }
        
        ~PlasticityBlock() {
            std::free(rules);
        }
    };
    
    std::vector<PlasticityBlock> blocks;
    std::unordered_map<uint64_t, size_t> ruleToBlock;
    std::unordered_map<uint64_t, size_t> ruleIndex;
    size_t ruleSize;
    uint64_t nextBlockId;
    mutable std::mutex mutex;
    
public:
    PlasticityRuleAllocator() : ruleSize(sizeof(PlasticityRuleImpl)), nextBlockId(0) {}
    
    ~PlasticityRuleAllocator() {
        for (auto& block : blocks) {
            std::free(block.rules);
        }
    }
    
    void* allocateRule() {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Try to find a free rule in existing blocks
        for (auto& block : blocks) {
            if (block.isFree) {
                block.isFree = false;
                void* rulePtr = block.rules;
                new (rulePtr) PlasticityRuleImpl();
                uint64_t ruleId = nextBlockId * 1000 + 0;
                ruleToBlock[ruleId] = block.blockId;
                ruleIndex[ruleId] = 0;
                return rulePtr;
            }
        }
        
        // Need to create a new block
        PlasticityBlock newBlock(200, nextBlockId++);  // 200 rules per block
        newBlock.isFree = false;
        blocks.push_back(std::move(newBlock));
        
        // Initialize all rules in the block
        char* blockData = static_cast<char*>(blocks.back().rules);
        for (size_t i = 0; i < blocks.back().ruleCount; ++i) {
            new (blockData + i * ruleSize) PlasticityRuleImpl();
        }
        
        void* rulePtr = blocks.back().rules;
        uint64_t ruleId = blocks.back().blockId * 1000 + 0;
        ruleToBlock[ruleId] = blocks.back().blockId;
        ruleIndex[ruleId] = 0;
        
        return rulePtr;
    }
    
    void freeRule(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto& block : blocks) {
            if (block.rules == ptr) {
                reinterpret_cast<PlasticityRuleImpl*>(ptr)->~PlasticityRuleImpl();
                break;
            }
        }
    }
    
    PlasticityRuleImpl* getRule(uint64_t ruleId) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto blockIt = ruleToBlock.find(ruleId);
        if (blockIt == ruleToBlock.end()) return nullptr;
        
        size_t blockId = blockIt->second;
        auto indexIt = ruleIndex.find(ruleId);
        if (indexIt == ruleIndex.end()) return nullptr;
        
        size_t index = indexIt->second;
        
        for (auto& block : blocks) {
            if (block.blockId == blockId) {
                return reinterpret_cast<PlasticityRuleImpl*>(
                    static_cast<char*>(block.rules) + index * ruleSize
                );
            }
        }
        
        return nullptr;
    }
    
    size_t getTotalRuleCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t total = 0;
        for (const auto& block : blocks) {
            if (!block.isFree) {
                total += block.ruleCount;
            }
        }
        return total;
    }
    
    size_t getMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t total = 0;
        for (const auto& block : blocks) {
            total += block.ruleCount * ruleSize;
        }
        return total;
    }
    
    std::string getStats() const {
        std::stringstream ss;
        ss << "Plasticity Rule Allocator Statistics:\n";
        ss << "  Rule size: " << ruleSize << " bytes\n";
        ss << "  Number of blocks: " << blocks.size() << "\n";
        ss << "  Total rules allocated: " << getTotalRuleCount() << "\n";
        ss << "  Total memory usage: " << getMemoryUsage() << " bytes\n";
        ss << "  Average rules per block: " << (blocks.size() > 0 ? 
            static_cast<float>(getTotalRuleCount()) / blocks.size() : 0.0f) << "\n";
        
        return ss.str();
    }
};

// Global allocator instances for core neural components
inline NeuronAllocator& getNeuronAllocator() {
    static NeuronAllocator allocator;
    return allocator;
}

inline SynapseAllocator& getSynapseAllocator() {
    static SynapseAllocator allocator;
    return allocator;
}

inline PlasticityRuleAllocator& getPlasticityRuleAllocator() {
    static PlasticityRuleAllocator allocator;
    return allocator;
}

// Convenience functions for allocating core neural components
inline void* allocateNeuron() {
    return getNeuronAllocator().allocateNeuron();
}

inline void freeNeuron(void* ptr) {
    getNeuronAllocator().freeNeuron(ptr);
}

inline NeuronImpl* getNeuron(uint64_t neuronId) {
    return getNeuronAllocator().getNeuron(neuronId);
}

inline void* allocateSynapse() {
    return getSynapseAllocator().allocateSynapse();
}

inline void freeSynapse(void* ptr) {
    getSynapseAllocator().freeSynapse(ptr);
}

inline SynapseImpl* getSynapse(uint64_t synapseId) {
    return getSynapseAllocator().getSynapse(synapseId);
}

inline void* allocatePlasticityRule() {
    return getPlasticityRuleAllocator().allocateRule();
}

inline void freePlasticityRule(void* ptr) {
    getPlasticityRuleAllocator().freeRule(ptr);
}

inline PlasticityRuleImpl* getPlasticityRule(uint64_t ruleId) {
    return getPlasticityRuleAllocator().getRule(ruleId);
}

inline std::string getAllocatorStats() {
    std::stringstream ss;
    ss << getNeuronAllocator().getStats() << "\n";
    ss << getSynapseAllocator().getStats() << "\n";
    ss << getPlasticityRuleAllocator().getStats() << "\n";
    return ss.str();
}

} // namespace CustomAllocators
} // namespace nlm
