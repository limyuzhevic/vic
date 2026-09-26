// ============================================================================
// NOVELTY AND SIMILARITY CALCULATION UTILITIES EXAMPLE
// ============================================================================

#include <iostream>
#include <vector>
#include "Utilities.hpp"

int main() {
    std::cout << "=== Novelty and Similarity Utilities Example ===\n";
    
    // Example vision feature vectors
    std::vector<float> previousVision = {0.1f, 0.2f, 0.8f, 0.3f, 0.5f};
    std::vector<float> currentVision = {0.2f, 0.3f, 0.7f, 0.4f, 0.6f};
    
    // Example 1: Calculate temporal novelty (novelty detection)
    float novelty = nlm::calculateTemporalNovelty(currentVision, previousVision);
    std::cout << "Temporal novelty score: " << novelty << "\n";
    
    // Example 2: Calculate feature similarity (pattern recognition)
    float similarity = nlm::calculateFeatureSimilarity(previousVision, currentVision, 1); // Cosine similarity
    std::cout << "Cosine similarity: " << similarity << "\n";
    
    float euclideanSimilarity = nlm::calculateFeatureSimilarity(previousVision, currentVision, 0); // Euclidean similarity
    std::cout << "Euclidean similarity: " << euclideanSimilarity << "\n";
    
    // Example 3: Calculate weighted difference (general purpose utility)
    float weightedDiff = nlm::calculateWeightedDifference(0.8f, 0.5f, 2.0f);
    std::cout << "Weighted difference between 0.8 and 0.5 (weight 2.0): " << weightedDiff << "\n";
    
    // Example vision processing pipeline
    std::cout << "\nVision Processing Pipeline:\n";
    float rawIntensity = 0.7f;
    float processed = nlm::applyOffsetScale(rawIntensity, -1.0f, 5.0f);
    float signedValue = nlm::normalizeToSigned(processed);
    float scaledBack = nlm::scaleToRange(signedValue, -1.0f, 1.0f, 0.0f, 1.0f);
    
    std::cout << "  Raw intensity: " << rawIntensity << "\n";
    std::cout << "  Offset-scaled: " << processed << "\n";
    std::cout << "  Signed normalized: " << signedValue << "\n";
    std::cout << "  Scaled back to original range: " << scaledBack << "\n";
    
    return 0;
}
