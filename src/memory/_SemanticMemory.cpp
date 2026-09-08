// SemanticMemory Implementation
struct SemanticMemory::Impl {
    std::unordered_map<std::string, std::tuple<std::string, float, float>> facts;
    std::unordered_map<std::string, float> accessTimes;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value, float confidence, float accessTime) {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        std::get<0>(it->second) = value;
        std::get<1>(it->second) = confidence;
    } else {
        pImpl->facts[key] = std::make_tuple(value, confidence, accessTime);
    }
    pImpl->accessTimes[key] = accessTime;
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        return std::get<0>(it->second);
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->facts.find(key) != pImpl->facts.end();
}

std::vector<std::tuple<std::string, std::string, float>> SemanticMemory::getAllFacts() const {
    std::vector<std::tuple<std::string, std::string, float>> result;
    result.reserve(pImpl->facts.size());
    for (const auto& pair : pImpl->facts) {
        result.emplace_back(pair.first, std::get<0>(pair.second), std::get<1>(pair.second));
    }
    return result;
}

float SemanticMemory::getFactStrength(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        return std::get<1>(it->second);
    }
    return 0.0f;
}

void SemanticMemory::decayUnusedFacts(float decayRate, float currentTime) {
    for (auto it = pImpl->facts.begin(); it != pImpl->facts.end(); ) {
        float timeSinceAccess = currentTime - pImpl->accessTimes[it->first];
        if (timeSinceAccess > 0.0f) {
            std::get<1>(it->second) *= std::pow(1.0f - decayRate, timeSinceAccess);
        }
        it++;
    }
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->accessTimes.clear();
}