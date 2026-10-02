#include "Observation.hpp"

namespace nlm {

float Observation::dotProduct(const std::vector<float>& other) const {
    // Optimized dot product implementation with error checking
    if (data_.size() != other.size()) {
        NLM_LOG_ERROR_WITH_CONTEXT(
            nlm::error::ErrorType::NEURAL_DYNAMICS, 
            "Dot product dimension mismatch"
        );
        return 0.0f;
    }
    
    float result = 0.0f;
    for (size_t i = 0; i < data_.size(); ++i) {
        result += data_[i] * other[i];
    }
    return result;
}

float Observation::magnitude() const {
    // Cached magnitude calculation
    if (timestampDirty_) {
        float sum = 0.0f;
        for (const auto& value : data_) {
            sum += value * value;
        }
        cachedMagnitude_ = std::sqrt(sum);
        timestampDirty_ = false;
    }
    return cachedMagnitude_;
}

void Observation::normalize() {
    // Optimized normalization with single pass
    float mag = magnitude();
    if (mag > 0.0f) {
        for (auto& value : data_) {
            value /= mag;
        }
        timestampDirty_ = true;
    }
}

} // namespace nlm
