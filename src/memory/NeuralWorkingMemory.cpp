#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../dynamics/NeuralDynamics.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_set>
#include <numeric>
#include <random>
#include <thread>
#include <mutex>

namespace nlm {

// WorkingMemoryManager Implementation
void WorkingMemoryManager::addEntry(const MemoryEntry& entry) {
    entries_.push_back(entry);
    if (compressionEnabled_ && entry.strength > compressionThreshold_) {
        // Compress if above threshold
        float compressRatio = 1.0f - compressionThreshold_ / entry.strength;
        if (entry.isCompressed) {
            // Further compress existing compressed entry
            float factor = std::pow(0.5f, compressRatio);
            for (auto& e : entries_) {
                if (e.neuronId == entry.neuronId && e.isCompressed) {
                    e.compressedPattern.clear();
                    // Additional compression
                }
            }
        }
    }
}

const MemoryEntry* WorkingMemoryManager::getEntry(NeuronId neuron) const {
    for (const auto& entry : entries_) {
        if (entry.neuronId == neuron) {
            return &entry;
        }
    }
    return nullptr;
}

void WorkingMemoryManager::update(TimestepDuration dt, float baseDecayRate,
                                const std::function<float(NeuronId)>& accessImportance,
                                const std::function<void(NeuronId)>& onConsolidation) {
    if (entries_.empty()) return;
    
    // Apply decay with importance-based modulation
    for (auto& entry : entries_) {
        float importanceFactor = accessImportance(entry.neuronId);
        float decay = baseDecayRate * (1.0f + (1.0f - importanceFactor) * 0.5f);
        entry.strength *= (1.0f - decay * dt);
        
        if (entry.strength < 0.01f) {
            entry.strength = 0.0f;
        }
        
        // Update activation based on strength
        entry.activation = entry.strength * entry.optimalStrength;
    }
    
    // Check for consolidation needs
    if (consolidationEnabled_) {
        for (const auto& entry : entries_) {
            if (entry.importance > consolidationThreshold_ && entry.strength > 0.5f) {
                onConsolidation(entry.neuronId);
            }
        }
    }
    
    // Handle compression if enabled
    if (compressionEnabled_) {
        size_t compressedCount = 0;
        for (const auto& entry : entries_) {
            if (entry.isCompressed) compressedCount++;
        }
        
        if (compressedCount < entries_.size() * 0.5f) {
            // Compress enough entries to reach 50% compression
            std::vector<size_t> compressibleIndices;
            for (size_t i = 0; i < entries_.size(); ++i) {
                if (!entries_[i].isCompressed && entries_[i].strength > compressionThreshold_) {
                    compressibleIndices.push_back(i);
                }
            }
            
            size_t targetCompressed = static_cast<size_t>(entries_.size() * 0.5f);
            if (targetCompressed > compressedCount) {
                size_t toCompress = std::min(targetCompressed - compressedCount, 
                                           compressibleIndices.size());
                for (size_t i = 0; i < toCompress; ++i) {
                    size_t idx = compressibleIndices[i];
                    entries_[idx].isCompressed = true;
                    // Create compressed pattern (simplified PCA-like compression)
                    entries_[idx].compressedPattern = std::vector<float>(10, 0.0f); // Fixed size compression
                    for (size_t j = 0; j < 10; ++j) {
                        entries_[idx].compressedPattern[j] = entries_[idx].activation * 
                                                             std::sin(j * 0.1f * entries_[idx].strength);
                    }
                }
            }
        }
    }
}

void WorkingMemoryManager::evictMemories(size_t targetSize) {
    if (entries_.size() <= targetSize) return;
    
    // Calculate total importance and access counts for statistics
    float totalImportance = 0.0f;
    float totalAccessCount = 0.0f;
    
    for (const auto& entry : entries_) {
        totalImportance += entry.importance;
        totalAccessCount += entry.accessCount;
    }
    
    if (totalImportance > 0.0f) {
        totalImportance = 1.0f / totalImportance;
        totalAccessCount = 1.0f / totalAccessCount;
    }
    
    // Score entries for eviction (lower score = better to evict)
    std::vector<std::pair<float, size_t>> scores;
    for (size_t i = 0; i < entries_.size(); ++i) {
        float score = 0.0f;
        
        switch (evictionPolicy_) {
            case EvictionPolicy::LRU:
                score = static_cast<float>(entries_[i].lastAccess);
                break;
            case EvictionPolicy::LFU:
                score = static_cast<float>(entries_[i].accessCount);
                break;
            case EvictionPolicy::Importance:
                score = entries_[i].importance;
                break;
            case EvictionPolicy::Random:
                static std::mt19937 rng(std::random_device{}());
                score = static_cast<float>(rng() % 1000) / 1000.0f;
                break;
        }
        
        scores.push_back({score, i});
    }
    
    // Sort by score (lowest first for eviction)
    std::sort(scores.begin(), scores.end(), 
              [](const std::pair<float, size_t>& a, const std::pair<float, size_t>& b) {
                  return a.first < b.first;
              });
    
    // Evict from highest score to target size
    size_t toEvict = entries_.size() - targetSize;
    for (size_t i = 0; i < toEvict; ++i) {
        size_t idx = scores[i].second;
        entries_.erase(entries_.begin() + idx);
        // Update scores after erase
        for (auto& pair : scores) {
            if (pair.second > idx) pair.second--;
        }
    }
}

size_t WorkingMemoryManager::getCompressedCount() const {
    size_t count = 0;
    for (const auto& entry : entries_) {
        if (entry.isCompressed) count++;
    }
    return count;
}

float WorkingMemoryManager::getAverageImportance() const {
    if (entries_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& entry : entries_) {
        sum += entry.importance;
    }
    return sum / entries_.size();
}

float WorkingMemoryManager::getCompressionRatio() const {
    if (entries_.empty()) return 0.0f;
    
    size_t compressed = 0;
    for (const auto& entry : entries_) {
        if (entry.isCompressed) compressed++;
    }
    return static_cast<float>(compressed) / entries_.size();
}

void WorkingMemoryManager::evictLRU() {
    if (entries_.empty()) return;
    
    auto it = std::min_element(entries_.begin(), entries_.end(),
        [](const MemoryEntry& a, const MemoryEntry& b) {
            return a.lastAccess < b.lastAccess;
        });
    
    entries_.erase(it);
}

void WorkingMemoryManager::evictLFU() {
    if (entries_.empty()) return;
    
    auto it = std::min_element(entries_.begin(), entries_.end(),
        [](const MemoryEntry& a, const MemoryEntry& b) {
            return a.accessCount < b.accessCount;
        });
    
    entries_.erase(it);
}

void WorkingMemoryManager::evictByImportance() {
    if (entries_.empty()) return;
    
    auto it = std::min_element(entries_.begin(), entries_.end(),
        [](const MemoryEntry& a, const MemoryEntry& b) {
            return a.importance < b.importance;
        });
    
    entries_.erase(it);
}

void WorkingMemoryManager::evictRandom() {
    if (entries_.empty()) return;
    
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, entries_.size() - 1);
    size_t idx = dist(rng);
    
    entries_.erase(entries_.begin() + idx);
}

} // namespace nlm
