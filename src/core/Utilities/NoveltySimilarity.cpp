// ============================================================================
// NOVELTY AND SIMILARITY CALCULATION UTILITIES IMPLEMENTATION
// ============================================================================

#include "Utilities.hpp"

namespace nlm {

// ============================================================================
// NOVELTY AND SIMILARITY UTILITIES IMPLEMENTATION
// ============================================================================

float calculateTemporalNovelty(const std::vector<float>& currentState,
                              const std::vector<float>& previousState,
                              float decayRate) {
    if (currentState.size() != previousState.size()) {
        throw std::invalid_argument("State vectors must have same size");
    }
    
    float totalDifference = 0.0f;
    for (size_t i = 0; i < currentState.size(); ++i) {
        float diff = std::abs(currentState[i] - previousState[i]);
        totalDifference += diff * std::pow(decayRate, static_cast<float>(i));
    }
    
    return totalDifference / currentState.size();
}

float calculateFeatureSimilarity(const std::vector<float>& features1,
                                const std::vector<float>& features2,
                                int similarityMetric) {
    if (features1.size() != features2.size()) {
        throw std::invalid_argument("Feature vectors must have same size");
    }
    
    if (similarityMetric == 0) { // Euclidean similarity
        float sumSq = 0.0f;
        for (size_t i = 0; i < features1.size(); ++i) {
            float diff = features1[i] - features2[i];
            sumSq += diff * diff;
        }
        return 1.0f / (1.0f + std::sqrt(sumSq));
    } else { // Cosine similarity (default)
        float dotProduct = 0.0f;
        float norm1 = 0.0f;
        float norm2 = 0.0f;
        
        for (size_t i = 0; i < features1.size(); ++i) {
            dotProduct += features1[i] * features2[i];
            norm1 += features1[i] * features1[i];
            norm2 += features2[i] * features2[i];
        }
        
        norm1 = std::sqrt(norm1);
        norm2 = std::sqrt(norm2);
        
        if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
        
        return dotProduct / (norm1 * norm2);
    }
}

} // namespace nlm
