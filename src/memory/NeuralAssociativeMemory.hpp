#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    bool initialize(class Brain* brain);
    void clear();
    
    // Simple associative memory implementation
    void storePattern(const std::vector<NeuronId>& pattern, float strength);
    float retrievePattern(const std::vector<NeuronId>& pattern) const;
    
    size_t getPatternCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm