} // namespace nlm

// Helper method to get concept by ID
ConceptFormation::DiscoveredConcept* ConceptFormation::Impl::getConceptById(ConceptFormation& cf, size_t conceptId) {
    for (auto& concept : cf.concepts_) {
        if (concept.id == conceptId) {
            return &concept;
        }
    }
    return nullptr;
}

void ConceptFormation::updateConceptFlexibility(size_t conceptId, float flexibility) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (concept) {
        concept->avgStability = (concept->avgStability * 0.5f) + (flexibility * 0.5f);
    }
}

void ConceptFormation::updateConceptFormationSpeed(size_t conceptId, float speed) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (concept) {
        // Store speed information for future use
        concept->categoryHint += " (speed:" + std::to_string(speed) + ")";
    }
}

void ConceptFormation::updateConceptMemory(size_t conceptId, float memory) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (concept) {
        // Enhance concept memory based on acetylcholine effects
        for (auto& instance : concept->instances) {
            instance.stability = std::min(1.0f, instance.stability + memory * 0.1f);
        }
        
        // Update average stability
        concept->avgStability = 0.0f;
        for (const auto& instance : concept->instances) {
            concept->avgStability += instance.stability;
        }
        if (!concept->instances.empty()) {
            concept->avgStability /= concept->instances.size();
        }
    }
}