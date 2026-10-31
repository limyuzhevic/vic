// ConceptFormation Implementation
struct ConceptFormation::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(0)
    , formationThreshold_(0.7f)
    , stabilityThreshold_(0.6f)
    , stabilityWindow_(5)
{
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("ConceptFormation initialized");
}

void ConceptFormation::setFormationThreshold(float t) { formationThreshold_ = t; }
void ConceptFormation::setStabilityWindow(size_t w) { stabilityWindow_ = w; }

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                   const std::vector<float>& features, float reward) {
    if (conceptId >= concepts_.size()) return;
    
    DiscoveredConcept& concept = concepts_[conceptId];
    
    // Create new instance
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;  // Would use current step
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    // Update prototype using averaging
    updatePrototype(conceptId, newPattern);
    
    // Add to concept
    concept.instances.push_back(instance);
    concept.totalObservations++;
    
    // Update stability based on recent observations
    if (concept.instances.size() > stabilityWindow_) {
        concept.instances.erase(concept.instances.begin());
    }
    
    // Recalculate average stability
    float totalStability = 0.0f;
    for (const auto& instance : concept.instances) {
        totalStability += instance.stability;
    }
    concept.avgStability = totalStability / concept.instances.size();
    
    // Update category hint based on features
    // This would be more sophisticated in a real implementation
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    if (conceptId >= concepts_.size()) return;
    
    DiscoveredConcept& concept = concepts_[conceptId];
    
    if (concept.instances.empty()) {
        concept.prototype = newInstance;
    } else {
        // Hebbian averaging: new prototype = old * (1 - alpha) + new * alpha
        float alpha = 0.1f;
        for (size_t i = 0; i < concept.prototype.size() && i < newInstance.size(); ++i) {
            concept.prototype[i] = concept.prototype[i] * (1.0f - alpha) + newInstance[i] * alpha;
        }
    }
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    if (conceptId >= concepts_.size()) return false;
    
    const auto& concept = concepts_[conceptId];
    return concept.avgStability > stabilityThreshold_ && 
           concept.instances.size() >= stabilityWindow_ / 2;
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    if (conceptA >= concepts_.size() || conceptB >= concepts_.size()) return;
    if (conceptA == conceptB) return;
    
    // Merge concept B into concept A
    DiscoveredConcept& target = concepts_[conceptA];
    const DiscoveredConcept& source = concepts_[conceptB];
    
    // Combine instances
    target.instances.insert(target.instances.end(), 
                          source.instances.begin(), source.instances.end());
    
    // Update prototype by averaging
    for (size_t i = 0; i < target.prototype.size() && i < source.prototype.size(); ++i) {
        target.prototype[i] = (target.prototype[i] + source.prototype[i]) * 0.5f;
    }
    
    // Combine associated concept IDs
    target.associatedConceptIds.insert(target.associatedConceptIds.end(),
                                     source.associatedConceptIds.begin(),
                                     source.associatedConceptIds.end());
    
    // Update total observations
    target.totalObservations += source.totalObservations;
    
    // Recalculate stability
    float totalStability = 0.0f;
    for (const auto& instance : target.instances) {
        totalStability += instance.stability;
    }
    target.avgStability = totalStability / target.instances.size();
    
    // Remove concept B
    concepts_.erase(concepts_.begin() + conceptB);
    
    // Update IDs for remaining concepts > conceptB
    for (size_t i = conceptB + 1; i < concepts_.size(); ++i) {
        // No need to update as we erased the element
    }
    
    nextConceptId_ = concepts_.size();
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                        const std::vector<float>& b) const {
    if (a.size() != b.size()) return 0.0f;
    
    float sumSqDiff = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sumSqDiff += diff * diff;
    }
    
    float magnitudeA = 0.0f;
    float magnitudeB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        magnitudeA += a[i] * a[i];
        magnitudeB += b[i] * b[i];
    }
    
    magnitudeA = std::sqrt(magnitudeA);
    magnitudeB = std::sqrt(magnitudeB);
    
    if (magnitudeA < 1e-6f || magnitudeB < 1e-6f) return 0.0f;
    
    return 1.0f - std::sqrt(sumSqDiff / (magnitudeA * magnitudeB));
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                     const std::vector<float>& features,
                                     float reward) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.features = features;
    concept.categoryHint = "unknown";  // Would derive from features
    concept.totalObservations = 1;
    concept.avgStability = 0.5f;
    
    concepts_.push_back(concept);
    return concept.id;
}

const DiscoveredConcept* ConceptFormation::getConcept(size_t conceptId) const {
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            return &concept;
        }
    }
    return nullptr;
}

std::vector<float> ConceptFormation::getConceptPrototype(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return std::vector<float>();
    return concept->prototype;
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(size_t conceptId) const {
    static const std::vector<ConceptInstance> empty;
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return empty;
    return concept->instances;
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    return concept->avgStability;
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 0;
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                         const std::vector<float>& features,
                                         float reward,
                                         SimulationStep currentTime) {
    if (isNovel(pattern)) {
        return createConcept(pattern, features, reward);
    }
    
    size_t conceptId = getMatchingConcept(pattern);
    if (conceptId != 0) {
        updateConcept(conceptId, pattern, features, reward);
    }
    
    return conceptId;
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern) const {
    size_t bestId = 0;
    float bestSimilarity = formationThreshold_;
    
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestId = concept.id;
        }
    }
    
    return bestId;
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                              float similarityThreshold) const {
    return getMatchingConcept(pattern) == 0;
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    return getMatchingConcept(pattern);
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    // Generalization ability = inverse of prototype variance
    float totalVariance = 0.0f;
    for (const auto& instance : concept->instances) {
        float variance = computeSimilarity(instance.pattern, concept->prototype);
        totalVariance += (1.0f - variance);
    }
    
    return concept->instances.empty() ? 0.0f : totalVariance / concept->instances.size();
}

} // namespace nlm