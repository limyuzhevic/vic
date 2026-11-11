#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Phase 6 text encoder: a teacher word becomes a sparse sensory
// pattern, never a string the brain can match on. Lower-case, trim,
// FNV-1a hash, then splitmix to pick 4 distinct neurons out of 16
// dedicated text neurons. Different words get different (overlapping,
// as in real sensory codes) patterns; the same word always gets the
// same pattern, which is what makes paired learning possible at all.
// The pattern is injected on sensoryVision_ neurons 16..30 — the
// tail vision neurons that vision itself never uses (vision is 16
// rays, the vision group has 31 neurons), so text has its own
// channel and cannot masquerade as a visual scene.
std::vector<size_t> AgentBrain::encodeText(const std::string& text) {
    std::string norm;
    for (char c : text) {
        if (c == ' ' || c == '\t' || c == '\n') continue;
        norm += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (norm.empty()) return {};
    uint64_t h = 1469598103934665603ULL;
    for (char c : norm) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        h *= 1099511628211ULL;
    }
    std::vector<size_t> out;
    uint64_t state = h;
    while (out.size() < 4) {
        // splitmix64
        state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        size_t idx = static_cast<size_t>(z % 15);
        if (std::find(out.begin(), out.end(), idx) == out.end()) out.push_back(idx);
    }
    return out;
}

} // namespace nlm