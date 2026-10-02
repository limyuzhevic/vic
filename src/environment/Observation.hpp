#pragma once

#include "src/core/Types/Types.hpp"
#include "src/error/ErrorHandling.hpp"
#include <vector>
#include <chrono>

namespace nlm {

// Enhanced observation class with performance optimizations
class Observation {
public:
    // Fast accessors with cache
    const std::vector<float>& getData() const { return data_; }
    size_t dimension() const { return data_.size(); }
    
    // Optimized operations
    float getValue(size_t index) const {
        return (index < data_.size()) ? data_[index] : 0.0f;
    }
    
    // Vectorized operations
    float dotProduct(const std::vector<float>& other) const;
    float magnitude() const;
    void normalize();
    
    // Performance monitoring
    Timestamp getTimestamp() const { return timestamp_; }
    void updateTimestamp() { timestamp_ = std::chrono::high_resolution_clock::now(); }
    
private:
    mutable bool timestampDirty_ = true;
    mutable Timestamp timestamp_;
    mutable float cachedMagnitude_ = -1.0f;  // Cached value
    
    std::vector<float> data_;
};

} // namespace nlm
