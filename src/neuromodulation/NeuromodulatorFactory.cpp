// Factory implementation
namespace {
    Neuromodulator* createDopamine() {
        return new Dopamine();
    }
    
    Neuromodulator* createCuriosity() {
        return new Curiosity();
    }
    
    Neuromodulator* createNovelty() {
        return new Novelty();
    }
    
    Neuromodulator* createPredictionError() {
        return new PredictionError();
    }
    
    // Initialize registry
    const std::unordered_map<NeuromodulatorType, Neuromodulator*(*)()> creatorRegistry = {
        {NeuromodulatorType::Dopamine, createDopamine},
        {NeuromodulatorType::Curiosity, createCuriosity},
        {NeuromodulatorType::Novelty, createNovelty},
        {NeuromodulatorType::PredictionError, createPredictionError}
    };
}

Neuromodulator* NeuromodulatorFactory::create(NeuromodulatorType type) {
    auto it = creatorRegistry.find(type);
    if (it != creatorRegistry.end()) {
        NLM_LOG_INFO("Creating neuromodulator: " << static_cast<int>(type));
        return it->second();
    }
    NLM_LOG_WARN("Unknown neuromodulator type: " << static_cast<int>(type));
    return nullptr;
}

std::unique_ptr<Neuromodulator> NeuromodulatorFactory::createUnique(NeuromodulatorType type) {
    auto it = creatorRegistry.find(type);
    if (it != creatorRegistry.end()) {
        NLM_LOG_INFO("Creating unique neuromodulator: " << static_cast<int>(type));
        return std::unique_ptr<Neuromodulator>(it->second());
    }
    NLM_LOG_WARN("Unknown neuromodulator type: " << static_cast<int>(type));
    return nullptr;
}

bool NeuromodulatorFactory::isTypeAvailable(NeuromodulatorType type) {
    return creatorRegistry.find(type) != creatorRegistry.end();
}

std::vector<NeuromodulatorType> NeuromodulatorFactory::getAvailableTypes() {
    std::vector<NeuromodulatorType> types;
    types.reserve(creatorRegistry.size());
    for (const auto& pair : creatorRegistry) {
        types.push_back(pair.first);
    }
    return types;
}

} // namespace nlm