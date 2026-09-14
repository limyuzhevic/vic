#include "SemanticMemory.hpp"

namespace nlm {

struct SemanticMemory::Impl {
    std::unordered_map<std::string, std::string> facts;
    std::unordered_map<std::string, std::vector<std::string>> associations;
    
    Impl() {
        // Pre-populate with some basic knowledge
        facts["who_we_are"] = "NLM is an experimental artificial developmental brain";
        facts["purpose"] = "To investigate computational brain-like systems";
        facts["goal"] = "Create a neural system that learns from experience";
        
        // Create associations
        associations["who_we_are"].push_back("purpose");
        associations["who_we_are"].push_back("goal");
    }
};

SemanticMemory::SemanticMemory() : pImpl(std::make_unique<Impl>()) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    pImpl->facts[key] = value;
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    return (it != pImpl->facts.end()) ? it->second : "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->facts.find(key) != pImpl->facts.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(pImpl->facts.size());
    for (const auto& pair : pImpl->facts) {
        result.emplace_back(pair);
    }
    return result;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->associations.clear();
}

} // namespace nlm
