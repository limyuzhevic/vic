#pragma once

#include "PluginBase.hpp"
#include "../memory/Memory.hpp"

namespace nlm {

enum class MemoryStorageType {
    STANDARD,
    HIERARCHICAL,
    SPATIAL,
    CUSTOM
};

enum class MemoryAccessPattern {
    SEQUENTIAL,
    RANDOM,
    FREQUENCY,
    RECENCY,
    CUSTOM
};

class MemoryQuery {
public:
    std::string pattern;
    double similarityThreshold;
    std::vector<std::string> tags;
    MemoryAccessPattern accessPattern;
    
    MemoryQuery() : similarityThreshold(0.5), accessPattern(MemoryAccessPattern::SEQUENTIAL) {}
};

class MemoryPattern {
public:
    std::string id;
    std::vector<float> pattern;
    float strength;
    double timestamp;
    std::vector<std::string> tags;
    
    MemoryPattern() : strength(0.0), timestamp(0.0) {}
};

class MemorySystemPlugin : public NLMPlugin {
public:
    // Memory extension
    virtual void storeMemory(const MemoryPattern& pattern, float strength) = 0;
    virtual MemoryPattern retrieveMemory(const MemoryQuery& query) = 0;
    virtual void updateMemory(double deltaTime) = 0;
    virtual void consolidateMemory() = 0;
    
    // Memory mechanisms
    virtual MemoryStorageType getStorageType() const = 0;
    virtual MemoryAccessPattern getAccessPattern() const = 0;
    virtual double getMemoryCapacity() const = 0;
    virtual double getMemoryDecayRate() const = 0;
};

} // namespace nlm
