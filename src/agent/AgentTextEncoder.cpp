// Text encoding utilities for Phase 6 text processing
// Extracted from AgentBrain.cpp to improve maintainability

#include "AgentTextEncoder.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <unordered_map>
#include <cmath>

namespace nlm {

// FNV-1a hash function for text encoding
static uint64_t fnv1a64(const std::string& str) {
    uint64_t hash = 1469598103934665603ULL;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= 1099511628211ULL;
    }
    return hash;
}

// Splitmix64 PRNG for generating distinct indices
static std::vector<size_t> splitmix64Sample(uint64_t state, size_t count, size_t maxSize) {
    std::vector<size_t> out;
    while (out.size() < count && out.size() < maxSize) {
        state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        size_t idx = static_cast<size_t>(z % maxSize);
        if (std::find(out.begin(), out.end(), idx) == out.end()) {
            out.push_back(idx);
        }
    }
    return out;
}

// Normalize text by removing whitespace and converting to lowercase
static std::string normalizeText(const std::string& text) {
    std::string norm;
    for (char c : text) {
        if (c == ' ' || c == '\t' || c == '\n') continue;
        norm += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return norm;
}

// Phase 6 text encoding: a teacher word becomes a sparse sensory pattern
// Different words get different (overlapping, as in real sensory codes) patterns
// The same word always gets the same pattern, which makes paired learning possible
std::vector<size_t> AgentTextEncoder::encodeText(const std::string& text, size_t maxNeurons) {
    std::string norm = normalizeText(text);
    if (norm.empty()) return {};
    
    // Use FNV-1a hash for deterministic encoding
    uint64_t hash = fnv1a64(norm);
    
    // Generate 4 distinct indices using splitmix64
    return splitmix64Sample(hash, 4, maxNeurons);
}

// Alternative encoding using simple hash for smaller patterns
std::vector<size_t> AgentTextEncoder::encodeTextSimple(const std::string& text, size_t maxNeurons) {
    std::string norm = normalizeText(text);
    if (norm.empty()) return {};
    
    uint64_t hash = fnv1a64(norm);
    
    // Use different sampling strategies based on requirements
    if (maxNeurons <= 8) {
        // For small neuron groups, use modulo sampling
        std::vector<size_t> out;
        for (size_t i = 0; i < 4 && i < maxNeurons; ++i) {
            out.push_back(static_cast<size_t>(hash) % maxNeurons);
            hash = hash ^ (hash >> 13) * 0x5deece66d;
        }
        return out;
    } else {
        // For larger groups, use splitmix64 for better distribution
        return splitmix64Sample(hash, 4, maxNeurons);
    }
}

// Text hashing utilities for comparing text patterns
uint64_t AgentTextEncoder::computeTextHash(const std::string& text) {
    return fnv1a64(normalizeText(text));
}

// Check if two texts produce the same encoded pattern
bool AgentTextEncoder::samePattern(const std::string& text1, const std::string& text2) {
    return computeTextHash(text1) == computeTextHash(text2);
}

// Generate multiple related patterns for a given text (for testing)
std::vector<std::vector<size_t>> AgentTextEncoder::generatePatternVariations(
    const std::string& text, size_t maxNeurons, size_t variationCount) {
    
    std::vector<std::vector<size_t>> variations;
    uint64_t baseHash = fnv1a64(normalizeText(text));
    
    for (size_t v = 0; v < variationCount; ++v) {
        // Apply different hash transformations
        uint64_t hash = baseHash + v * 0x9e3779b97f4a7c15ULL;
        hash = (hash ^ (hash >> 11)) * 0xc3a5c3d4;
        
        auto pattern = splitmix64Sample(hash, 4, maxNeurons);
        variations.push_back(pattern);
    }
    
    return variations;
}

// Validate text encoding by checking pattern properties
bool AgentTextEncoder::validatePattern(const std::vector<size_t>& pattern, size_t expectedSize) {
    if (pattern.size() != expectedSize) return false;
    
    // Check for duplicates
    std::unordered_set<size_t> seen;
    for (size_t idx : pattern) {
        if (seen.find(idx) != seen.end()) return false;
        seen.insert(idx);
    }
    
    // Check bounds
    for (size_t idx : pattern) {
        if (idx >= expectedSize) return false;
    }
    
    return true;
}

// Get statistics about a text encoding
TextEncodingStats AgentTextEncoder::getStats(const std::string& text, size_t maxNeurons) {
    TextEncodingStats stats;
    
    stats.originalText = text;
    stats.normalizedText = normalizeText(text);
    stats.hash = computeTextHash(text);
    stats.encoding = encodeText(text, maxNeurons);
    stats.patternValid = validatePattern(stats.encoding, maxNeurons);
    
    // Compute encoding characteristics
    if (!stats.encoding.empty()) {
        size_t maxIdx = *std::max_element(stats.encoding.begin(), stats.encoding.end());
        stats.maxIndex = maxIdx;
        stats.encodingDensity = 4.0f / maxNeurons;
    }
    
    return stats;
}

// Batch text encoding for multiple texts
std::vector<TextEncodingStats> AgentTextEncoder::batchEncode(
    const std::vector<std::string>& texts, size_t maxNeurons) {
    
    std::vector<TextEncodingStats> results;
    results.reserve(texts.size());
    
    for (const auto& text : texts) {
        results.push_back(getStats(text, maxNeurons));
    }
    
    return results;
}

// Text similarity measures for comparing encoded patterns
float AgentTextEncoder::computeSimilarity(
    const std::vector<size_t>& pattern1,
    const std::vector<size_t>& pattern2) {
    
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    // Jaccard similarity for sets
    std::unordered_set<size_t> set1(pattern1.begin(), pattern1.end());
    std::unordered_set<size_t> set2(pattern2.begin(), pattern2.end());
    
    size_t intersection = 0;
    for (size_t idx : set1) {
        if (set2.find(idx) != set2.end()) {
            intersection++;
        }
    }
    
    size_t unionSize = set1.size() + set2.size() - intersection;
    return unionSize > 0 ? static_cast<float>(intersection) / unionSize : 0.0f;
}

// Detect if a text is likely a "stop word" or common term
bool AgentTextEncoder::isCommonText(const std::string& text) {
    // Simple stop word list (can be expanded)
    static const std::unordered_set<std::string> stopWords = {
        "the", "a", "an", "and", "or", "but", "in", "on", "at", "to",
        "for", "of", "with", "by", "is", "are", "was", "were", "be",
        "been", "have", "has", "had", "do", "does", "did", "will",
        "would", "can", "could", "should", "may", "might", "must"
    };
    
    std::string norm = normalizeText(text);
    return stopWords.find(norm) != stopWords.end();
}

// Extract significant words from text for encoding
std::vector<std::string> AgentTextEncoder::extractSignificantWords(
    const std::string& text, size_t maxWords) {
    
    std::vector<std::string> words;
    std::string current;
    
    for (char c : text) {
        if (std::isalpha(c)) {
            current += std::tolower(c);
        } else if (!current.empty()) {
            if (!isCommonText(current)) {
                if (words.size() < maxWords) {
                    words.push_back(current);
                }
            }
            current.clear();
        }
    }
    
    if (!current.empty() && !isCommonText(current)) {
        if (words.size() < maxWords) {
            words.push_back(current);
        }
    }
    
    return words;
}

} // namespace nlm