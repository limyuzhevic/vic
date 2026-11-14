// Text encoding utilities for NLM
// Provides consistent text-to-neuron-pattern encoding for sensory processing

#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace nlm {

class TextEncoder {
public:
    // Encode a text string into a sparse pattern of neuron indices
    // Uses FNV-1a hash + Splitmix64 to generate 4 unique indices from 15 possible neurons
    // The pattern is deterministic: same word always maps to same pattern
    // Different words map to different (overlapping) patterns
    // Returns empty vector for empty input
    static std::vector<size_t> encode(const std::string& text);
    
    // Get the text neuron offset (neurons 16..30 are dedicated to text)
    static constexpr size_t TEXT_NEURON_OFFSET = 16;
    
    // Get the maximum number of text neurons
    static constexpr size_t MAX_TEXT_NEURONS = 15;
    
    // Get the maximum number of neurons that can be encoded
    static constexpr size_t MAX_NEURONS_PER_PATTERN = 4;
    
    // Validate that an encoded pattern is within valid ranges
    static bool validatePattern(const std::vector<size_t>& pattern);
    
    // Convert encoded indices to actual neuron IDs for sensory injection
    static std::vector<size_t> toNeuronIds(const std::vector<size_t>& indices);
    
private:
    // FNV-1a hash function (from Phase 6 text encoding)
    static uint64_t fnv1aHash(const std::string& input);
    
    // Splitmix64 PRNG for generating random indices from hash
    static uint64_t splitmix64(uint64_t state);
    
    // Normalize text: lowercase, remove whitespace
    static std::string normalizeText(const std::string& text);
};

} // namespace nlm
