// Text encoding utilities for Phase 6 text processing
// Extracted from AgentBrain.cpp to improve maintainability

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// Text encoding statistics structure
struct TextEncodingStats {
    std::string originalText;
    std::string normalizedText;
    uint64_t hash;
    std::vector<size_t> encoding;
    bool patternValid;
    size_t maxIndex;
    float encodingDensity;
};

// Text encoder class for Phase 6 text processing
class AgentTextEncoder {
public:
    // Main text encoding method for Phase 6
    // Encodes a text string into a sparse sensory pattern
    static std::vector<size_t> encodeText(const std::string& text, size_t maxNeurons);
    
    // Simple text encoding for smaller patterns
    static std::vector<size_t> encodeTextSimple(const std::string& text, size_t maxNeurons);
    
    // Compute text hash for deterministic encoding
    static uint64_t computeTextHash(const std::string& text);
    
    // Check if two texts produce the same encoded pattern
    static bool samePattern(const std::string& text1, const std::string& text2);
    
    // Generate pattern variations for testing
    static std::vector<std::vector<size_t>> generatePatternVariations(
        const std::string& text, size_t maxNeurons, size_t variationCount);
    
    // Validate pattern properties
    static bool validatePattern(const std::vector<size_t>& pattern, size_t expectedSize);
    
    // Get encoding statistics
    static TextEncodingStats getStats(const std::string& text, size_t maxNeurons);
    
    // Batch processing for multiple texts
    static std::vector<TextEncodingStats> batchEncode(
        const std::vector<std::string>& texts, size_t maxNeurons);
    
    // Text similarity measures
    static float computeSimilarity(
        const std::vector<size_t>& pattern1,
        const std::vector<size_t>& pattern2);
    
    // Common text detection
    static bool isCommonText(const std::string& text);
    
    // Significant word extraction
    static std::vector<std::string> extractSignificantWords(
        const std::string& text, size_t maxWords);
    
private:
    // Helper functions
    static std::string normalizeText(const std::string& text);
    static uint64_t fnv1a64(const std::string& str);
    static std::vector<size_t> splitmix64Sample(uint64_t state, size_t count, size_t maxSize);
    
    // Stop word list for text filtering
    static const std::unordered_set<std::string> getStopWords();
};

} // namespace nlm