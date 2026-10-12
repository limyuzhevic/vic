#include "MemoryPoolManager.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Config/Config.hpp"
#include <chrono>
#include <unordered_map>
#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include <string>

namespace nlm {

MemoryPoolManager::MemoryPoolManager(std::shared_ptr<Config> config) 
    : config(config), totalAllocated_(0), peakUsage_(0), totalOperations_(0) {
    // Get pool sizes from config or use defaults
    neuronPoolSize_ = config->getOr<size_t>("neuron_pool_size", 1024);
    synapsePoolSize_ = config->getOr<size_t>("synapse_pool_size", 4096);
    enableMemoryStats_ = config->getOr<bool>("enable_memory_stats", true);
    
    // Create specialized pools for different neuron types
    excitatoryNeuronPool_ = std::make_unique<NeuronPool>(neuronPoolSize_ / 4);
    inhibitoryNeuronPool_ = std::make_unique<NeuronPool>(neuronPoolSize_ / 4);
    sensoryNeuronPool_ = std::make_unique<NeuronPool>(neuronPoolSize_ / 4);
    motorNeuronPool_ = std::make_unique<NeuronPool>(neuronPoolSize_ / 4);
    internalNeuronPool_ = std::make_unique<NeuronPool>(neuronPoolSize_ / 4);
    
    // Create synapse pools
    excitatorySynapsePool_ = std::make_unique<SynapsePool>(synapsePoolSize_ * 3 / 4);
    inhibitorySynapsePool_ = std::make_unique<SynapsePool>(synapsePoolSize_ / 8);
    electricalSynapsePool_ = std::make_unique<SynapsePool>(synapsePoolSize_ / 8);
}

MemoryPoolManager::~MemoryPoolManager() {
    // Clear all pools
    excitatoryNeuronPool_.reset();
    inhibitoryNeuronPool_.reset();
    sensoryNeuronPool_.reset();
    motorNeuronPool_.reset();
    internalNeuronPool_.reset();
    excitatorySynapsePool_.reset();
    inhibitorySynapsePool_.reset();
    electricalSynapsePool_.reset();
}

MemoryPoolManager& MemoryPoolManager::getInstance() {
    static MemoryPoolManager instance(nullptr);
    return instance;
}

// Neuron pool management
NeuronId MemoryPoolManager::allocateNeuron(NeuronType type) {
    totalOperations_++;
    NeuronId neuronId;
    
    switch (type) {
        case NeuronType::Excitatory:
            neuronId = excitatoryNeuronPool_->allocate();
            break;
        case NeuronType::Inhibitory:
            neuronId = inhibitoryNeuronPool_->allocate();
            break;
        case NeuronType::Sensory:
            neuronId = sensoryNeuronPool_->allocate();
            break;
        case NeuronType::Motor:
            neuronId = motorNeuronPool_->allocate();
            break;
        case NeuronType::Internal:
            neuronId = internalNeuronPool_->allocate();
            break;
        default:
            neuronId = internalNeuronPool_->allocate();
            break;
    }
    
    if (enableMemoryStats_) {
        totalAllocated_++;
        if (totalAllocated_ > peakUsage_) {
            peakUsage_ = totalAllocated_;
        }
        recordAllocationTime();
    }
    
    return neuronId;
}

void MemoryPoolManager::deallocateNeuron(NeuronId id) {
    totalOperations_++;
    
    // Find which pool contains this neuron and deallocate from it
    // This requires mapping neuron IDs back to pool types
    // For now, use a simple approach - track neuron types separately
    auto it = neuronIdToType_.find(id);
    if (it != neuronIdToType_.end()) {
        NeuronType type = it->second;
        switch (type) {
            case NeuronType::Excitatory:
                excitatoryNeuronPool_->deallocate(id);
                break;
            case NeuronType::Inhibitory:
                inhibitoryNeuronPool_->deallocate(id);
                break;
            case NeuronType::Sensory:
                sensoryNeuronPool_->deallocate(id);
                break;
            case NeuronType::Motor:
                motorNeuronPool_->deallocate(id);
                break;
            case NeuronType::Internal:
                internalNeuronPool_->deallocate(id);
                break;
            default:
                internalNeuronPool_->deallocate(id);
                break;
        }
        neuronIdToType_.erase(it);
        
        if (enableMemoryStats_) {
            totalAllocated_--;
            recordDeallocationTime();
        }
    }
}

void MemoryPoolManager::registerNeuronType(NeuronId id, NeuronType type) {
    neuronIdToType_[id] = type;
}

// Synapse pool management
SynapseId MemoryPoolManager::allocateSynapse(SynapseType type) {
    totalOperations_++;
    SynapseId synapseId;
    
    switch (type) {
        case SynapseType::Excitatory:
            synapseId = excitatorySynapsePool_->allocate();
            break;
        case SynapseType::Inhibitory:
            synapseId = inhibitorySynapsePool_->allocate();
            break;
        case SynapseType::Electrical:
            synapseId = electricalSynapsePool_->allocate();
            break;
        default:
            synapseId = excitatorySynapsePool_->allocate();
            break;
    }
    
    if (enableMemoryStats_) {
        totalAllocated_++;
        if (totalAllocated_ > peakUsage_) {
            peakUsage_ = totalAllocated_;
        }
        recordAllocationTime();
    }
    
    return synapseId;
}

void MemoryPoolManager::deallocateSynapse(SynapseId id) {
    totalOperations_++;
    
    auto it = synapseIdToType_.find(id);
    if (it != synapseIdToType_.end()) {
        SynapseType type = it->second;
        switch (type) {
            case SynapseType::Excitatory:
                excitatorySynapsePool_->deallocate(id);
                break;
            case SynapseType::Inhibitory:
                inhibitorySynapsePool_->deallocate(id);
                break;
            case SynapseType::Electrical:
                electricalSynapsePool_->deallocate(id);
                break;
            default:
                excitatorySynapsePool_->deallocate(id);
                break;
        }
        synapseIdToType_.erase(it);
        
        if (enableMemoryStats_) {
            totalAllocated_--;
            recordDeallocationTime();
        }
    }
}

void MemoryPoolManager::registerSynapseType(SynapseId id, SynapseType type) {
    synapseIdToType_[id] = type;
}

// Global allocation/deallocation for arbitrary types
MemoryPoolManager::AllocationInfo MemoryPoolManager::allocate(size_t size, size_t alignment) {
    // For now, use the generic neuron pool for arbitrary allocations
    // In a full implementation, we'd have more sophisticated allocation
    AllocationInfo info;
    info.ptr = nullptr; // Will be set by caller
    info.size = size;
    info.alignment = alignment;
    return info;
}

template<typename T>
T* MemoryPoolManager::allocate() {
    return std::allocator<T>().allocate(1);
}

template<typename T>
void MemoryPoolManager::deallocate(T* ptr) {
    std::allocator<T>().deallocate(ptr, 1);
}

// Performance monitoring
void MemoryPoolManager::reportPerformance() {
    if (!enableMemoryStats_) return;
    
    std::string report = "MemoryPoolManager Performance:\n";
    report += "Total allocated objects: " + std::to_string(totalAllocated_) + "\n";
    report += "Peak usage: " + std::to_string(peakUsage_) + "\n";
    report += "Total operations: " + std::to_string(totalOperations_) + "\n";
    report += "Allocation efficiency: " + std::to_string(totalOperations_ > 0 ? 
        (static_cast<double>(peakUsage_) / totalOperations_) : 0.0) + "\n";
    
    // Report individual pool statistics
    report += "\nNeuron Pool Statistics:\n";
    report += "Excitatory: " + std::to_string(excitatoryNeuronPool_->size()) + "/" + 
              std::to_string(excitatoryNeuronPool_->capacity()) + "\n";
    report += "Inhibitory: " + std::to_string(inhibitoryNeuronPool_->size()) + "/" + 
              std::to_string(inhibitoryNeuronPool_->capacity()) + "\n";
    
    report += "\nSynapse Pool Statistics:\n";
    report += "Excitatory: " + std::to_string(excitatorySynapsePool_->size()) + "/" + 
              std::to_string(excitatorySynapsePool_->capacity()) + "\n";
    
    NLM_LOG_INFO(report);
}

void MemoryPoolManager::resetStats() {
    totalAllocated_ = 0;
    peakUsage_ = 0;
    totalOperations_ = 0;
    allocationTime_.clear();
    deallocationTime_.clear();
}

// Individual pool accessors
MemoryPoolManager::NeuronPool& MemoryPoolManager::getExcitatoryNeuronPool() {
    return *excitatoryNeuronPool_;
}

MemoryPoolManager::NeuronPool& MemoryPoolManager::getInhibitoryNeuronPool() {
    return *inhibitoryNeuronPool_;
}

MemoryPoolManager::NeuronPool& MemoryPoolManager::getSensoryNeuronPool() {
    return *sensoryNeuronPool_;
}

MemoryPoolManager::NeuronPool& MemoryPoolManager::getMotorNeuronPool() {
    return *motorNeuronPool_;
}

MemoryPoolManager::NeuronPool& MemoryPoolManager::getInternalNeuronPool() {
    return *internalNeuronPool_;
}

MemoryPoolManager::SynapsePool& MemoryPoolManager::getExcitatorySynapsePool() {
    return *excitatorySynapsePool_;
}

MemoryPoolManager::SynapsePool& MemoryPoolManager::getInhibitorySynapsePool() {
    return *inhibitorySynapsePool_;
}

MemoryPoolManager::SynapsePool& MemoryPoolManager::getElectricalSynapsePool() {
    return *electricalSynapsePool_;
}

// Performance metrics
void MemoryPoolManager::recordAllocationTime() {
    auto now = std::chrono::high_resolution_clock::now();
    allocationTime_.push_back(now);
    if (allocationTime_.size() > 100) {
        allocationTime_.pop_front();
    }
}

void MemoryPoolManager::recordDeallocationTime() {
    auto now = std::chrono::high_resolution_clock::now();
    deallocationTime_.push_back(now);
    if (deallocationTime_.size() > 100) {
        deallocationTime_.pop_front();
    }
}

double MemoryPoolManager::getAllocationTime() const {
    if (allocationTime_.empty()) return 0.0;
    auto total = std::chrono::duration<double, std::milli>(
        allocationTime_.back() - allocationTime_.front()).count();
    return total / allocationTime_.size();
}

double MemoryPoolManager::getDeallocationTime() const {
    if (deallocationTime_.empty()) return 0.0;
    auto total = std::chrono::duration<double, std::milli>(
        deallocationTime_.back() - deallocationTime_.front()).count();
    return total / deallocationTime_.size();
}

size_t MemoryPoolManager::getAllocatedCount() const {
    return totalAllocated_;
}

size_t MemoryPoolManager::getPeakUsage() const {
    return peakUsage_;
}

size_t MemoryPoolManager::getTotalAllocated() const {
    return totalOperations_;
}

} // namespace nlm