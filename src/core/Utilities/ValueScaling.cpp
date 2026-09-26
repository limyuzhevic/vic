// ============================================================================
// VALUE SCALING AND NORMALIZATION UTILITIES IMPLEMENTATION
// ============================================================================

#include "Utilities.hpp"

namespace nlm {

// ============================================================================
// SCALING UTILITIES IMPLEMENTATION
// ============================================================================

template<typename T>
T scaleToRange(T value, T sourceMin, T sourceMax, T targetMin, T targetMax) {
    if (std::isnan(value) || std::isinf(value)) {
        throw std::invalid_argument("Cannot scale NaN or infinite value");
    }
    
    if (sourceMin == sourceMax) {
        if (targetMin == targetMax) {
            return targetMin;
        }
        throw std::invalid_argument("Source range is constant");
    }
    
    // Normalize to [0,1] then scale to target range
    T normalized = (value - sourceMin) / (sourceMax - sourceMin);
    return normalized * (targetMax - targetMin) + targetMin;
}

template<typename T>
T normalizeToSigned(T value, T offset) {
    return (value - offset) * static_cast<T>(2.0) + static_cast<T>(-1.0);
}

template<typename T>
T applyOffsetScale(T value, T offset, T scale) {
    return (value * static_cast<T>(2.0) - offset) * scale;
}

} // namespace nlm
