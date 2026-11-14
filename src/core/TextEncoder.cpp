#include "TextEncoder.hpp"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <unordered_set>

namespace nlm {

std::vector<size_t> TextEncoder::encode(const std::string& text) {
    std::string norm = normalizeText(text);
    if (norm.empty()) return {};
    
    uint64_t h = fnv1aHash(norm);
    std::vector<size_t> out;
    uint64_t state = h;
    
    while (out.size() < MAX_NEURONS_PER_PATTERN) {
        state = splitmix64(state);
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        size_t idx = static_cast<size_t>(z % MAX_TEXT_NEURONS);
        
        if (std::find(out.begin(), out.end(), idx) == out.end()) {
            out.push_back(idx);
        }
    }
    
    return out;
}

bool TextEncoder::validatePattern(const std::vector<size_t>& pattern) {
    if (pattern.size() > MAX_NEURONS_PER_PATTERN) {
        return false;
    }
    for (size_t idx : pattern) {
        if (idx >= MAX_TEXT_NEURONS) {
            return false;
        }
    }
    return true;
}

std::vector<size_t> TextEncoder::toNeuronIds(const std::vector<size_t>& indices) {
    std::vector<size_t> neuronIds;
    neuronIds.reserve(indices.size());
    for (size_t idx : indices) {
        neuronIds.push_back(TEXT_NEURON_OFFSET + idx);
    }
    return neuronIds;
}

uint64_t TextEncoder::fnv1aHash(const std::string& input) {
    uint64_t h = 1469598103934665603ULL;
    for (char c : input) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t TextEncoder::splitmix64(uint64_t state) {
    state += 0x9e3779b97f4a7c15ULL;
    uint64_t z = state;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    z = z ^ (z >> 31);
    return z;
}

std::string TextEncoder::normalizeText(const std::string& text) {
    std::string norm;
    for (char c : text) {
        if (c == ' ' || c == '\t' || c == '\n') continue;
        norm += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return norm;
}

} // namespace nlm
