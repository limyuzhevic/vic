#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_set>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    PredictionSystem* predictionSystem;
    
    // Neural concept clustering for real-time pattern discovery
    struct NeuralCluster {
        size_t id;
        std::vector<float> centroid;
        std::vector<ConceptInstance> instances;
        float stability;
        float confidence;
        size_t observationCount;
        Timestamp lastUpdate;
        
        NeuralCluster() : id(0), stability(0), confidence(0.5f), 
                         observationCount(0), lastUpdate(-1.0f) {}
    };
    
    std::vector<NeuralCluster> clusters;
    
    // Learning parameters
    float learningRate;
    float noveltyThreshold;
    float stabilityDecayRate;
    
    // Neural dynamics for concept formation
    struct ConceptNeuron {
        NeuronId id;
        std::vector<float> weights;
        std::vector<float> activationHistory;
        SimulationStep lastSpike;
        
        ConceptNeuron() : id(INVALID_NEURON_ID), lastSpike(-1) {}
    };
    
    std::vector<ConceptNeuron> conceptNeurons;
    
    // Integration with prediction error systems
    std::vector<float> predictionErrors;
    std::vector<float> neuromodulationSignals;
    
    Impl() : brain(nullptr), predictionSystem(nullptr), learningRate(0.01f),
             noveltyThreshold(0.8f), stabilityDecayRate(0.95f) {}
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , predictionSystem_(nullptr)
    , nextConceptId_(1)
    , formationThreshold_(0.75f)
    , stabilityThreshold_(0.7f)
    , stabilityWindow_(5)
    , conceptStabilityThreshold_(0.8f)
    , hierarchicalConceptEnabled_(false)
    , lastConceptUpdate_(-1.0f) {}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain, PredictionSystem* predictionSystem) {
    pImpl->brain = brain;
    pImpl->predictionSystem = predictionSystem;
    brain_ = brain;
    predictionSystem_ = predictionSystem;
    
    // Initialize concept neurons using brain's neural substrate
    initializeConceptNeurons();
    
    NLM_LOG_INFO("ConceptFormation initialized with real neural concept clustering");
}

void ConceptFormation::initializeConceptNeurons() {
    if (!brain_) return;
    
    pImpl->conceptNeurons.clear();
    
    // Create concept-specific region
    RegionId conceptRegionId = brain_->addRegion("Concept_Dynamics");
    auto* region = brain_->getRegion(conceptRegionId);
    
    if (region) {
        // Create concept neuron population with diverse initial states
        PopulationId conceptPopId = region->addPopulation(30, NeuronType::Internal);
        auto* conceptPop = region->getPopulation(conceptPopId);
        
        if (conceptPop) {
            for (size_t i = 0; i < conceptPop->getNeurons().size(); ++i) {
                auto* neuron = conceptPop->getNeurons()[i];
                
                // Set neuron properties for concept dynamics
                neuron->setThreshold(-35.0f);
                neuron->setRestingPotential(-65.0f);
                neuron->setResetPotential(-65.0f);
                neuron->setLeakConductance(2.0f);
                
                // Enable plasticity for concept learning
                neuron->enablePlasticity(true, true, true);
                
                // Initialize with diverse state
                neuron->initializeRandom(*brain_->getRandomGenerator());
                
                // Store concept neuron
                pImpl->conceptNeurons.push_back(ConceptNeuron{neuron->getId()});
            }
        }
    }
}

void ConceptFormation::updateFromNeuralActivity(const std::vector<float>& neuralPattern,
                                                 SimulationStep currentStep) {
    if (neuralPattern.empty() || !brain_) return;
    
    // Update concept neurons based on neural activity
    updateConceptNeurons(neuralPattern, currentStep);
    
    // Learn new clusters from neural patterns
    learnFromNeuralPattern(neuralPattern, currentStep);
    
    lastConceptUpdate_ = currentStep;
}

void ConceptFormation::integratePredictionError(const std::vector<float>& predicted,
                                                 const std::vector<float>& actual,
                                                 SimulationStep currentStep) {
    if (predicted.size() != actual.size()) return;
    
    // Compute prediction error
    float totalError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        totalError += diff * diff;
    }
    
    float error = std::sqrt(totalError / predicted.size());
    pImpl->predictionErrors.push_back(error);
    
    // Generate neuromodulation signal based on prediction error
    float neuromodulation = std::min(1.0f, error * 2.0f);
    pImpl->neuromodulationSignals.push_back(neuromodulation);
    
    // Update concept stability based on prediction error
    updateConceptStabilityFromPredictionError(error, neuromodulation);
    
    // Keep error history bounded
    if (pImpl->predictionErrors.size() > 1000) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
        pImpl->neuromodulationSignals.erase(pImpl->neuromodulationSignals.begin());
    }
}

void ConceptFormation::learnFromNeuralPattern(const std::vector<float>& pattern,
                                               SimulationStep currentStep) {
    if (pattern.empty()) return;
    
    // Check if pattern matches existing clusters
    size_t bestCluster = findBestMatchingCluster(pattern);
    
    if (bestCluster != std::numeric_limits<size_t>::max()) {
        // Update existing cluster
        updateCluster(pImpl->clusters[bestCluster], pattern, currentStep);
    } else {
        // Create new cluster (novel concept)
        createNewCluster(pattern, currentStep);
    }
    
    // Update concept competitions and cooperation
    updateConceptCompetition();
}

size_t ConceptFormation::findBestMatchingCluster(const std::vector<float>& pattern) const {
    if (pImpl->clusters.empty()) return std::numeric_limits<size_t>::max();
    
    float bestSimilarity = -1.0f;
    size_t bestCluster = std::numeric_limits<size_t>::max();
    
    for (size_t i = 0; i < pImpl->clusters.size(); ++i) {
        float sim = computePatternSimilarity(pattern, pImpl->clusters[i].centroid);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestCluster = i;
        }
    }
    
    // Only return cluster if similarity is above threshold
    if (bestSimilarity > formationThreshold_) {
        return bestCluster;
    }
    
    return std::numeric_limits<size_t>::max();
}

void ConceptFormation::createNewCluster(const std::vector<float>& pattern,
                                         SimulationStep currentStep) {
    NeuralCluster cluster;
    cluster.id = nextConceptId_++;
    cluster.centroid = pattern;
    cluster.stability = 1.0f;
    cluster.confidence = 0.5f;
    cluster.observationCount = 1;
    cluster.lastUpdate = currentStep;
    
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.conceptId = cluster.id;
    instance.observationCount = 1;
    instance.timestamp = currentStep;
    instance.confidence = 0.5f;
    
    cluster.instances.push_back(instance);
    pImpl->clusters.push_back(cluster);
    
    NLM_LOG_INFO("ConceptFormation: Created new concept cluster " + std::to_string(cluster.id));
}

void ConceptFormation::updateCluster(NeuralCluster& cluster,
                                      const std::vector<float>& pattern,
                                      SimulationStep currentStep) {
    // Update centroid using Hebbian learning
    float alpha = learningRate_;
    if (cluster.centroid.size() == pattern.size()) {
        for (size_t i = 0; i < cluster.centroid.size(); ++i) {
            cluster.centroid[i] = cluster.centroid[i] * (1.0f - alpha) + pattern[i] * alpha;
        }
    }
    
    // Add new instance
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.conceptId = cluster.id;
    instance.observationCount = 1;
    instance.timestamp = currentStep;
    instance.confidence = computePatternConfidence(pattern, cluster.centroid);
    
    cluster.instances.push_back(instance);
    cluster.observationCount++;
    cluster.lastUpdate = currentStep;
    
    // Update stability
    cluster.stability = computeClusterStability(cluster);
    
    // Update confidence based on neuromodulation
    if (pImpl->neuromodulationSignals.size() > 0) {
        float avgNeuromod = std::accumulate(pImpl->neuromodulationSignals.end() - std::min(size_t(10), pImpl->neuromodulationSignals.size()),
                                          pImpl->neuromodulationSignals.end(), 0.0f) /
                          std::min(size_t(10), pImpl->neuromodulationSignals.size());
        cluster.confidence = std::min(1.0f, cluster.confidence * (1.0f - alpha) + avgNeuromod * alpha);
    }
}

void ConceptFormation::updateConceptNeurons(const std::vector<float>& pattern,
                                             SimulationStep currentStep) {
    for (auto& neuron : pImpl->conceptNeurons) {
        // Compute activation based on input pattern
        float activation = 0.0f;
        
        for (size_t i = 0; i < std::min<size_t>(pattern.size(), neuron.weights.size()); ++i) {
            activation += pattern[i] * neuron.weights[i];
        }
        
        // Apply sigmoid activation
        activation = 1.0f / (1.0f + std::exp(-activation * 10.0f));
        
        // Update activation history
        neuron.activationHistory.push_back(activation);
        if (neuron.activationHistory.size() > 100) {
            neuron.activationHistory.erase(neuron.activationHistory.begin());
        }
        
        // Update weights based on activation (Hebbian-like)
        if (activation > 0.5f) {
            neuron.lastSpike = currentStep;
            for (float& weight : neuron.weights) {
                weight += activation * 0.01f * (1.0f - std::abs(weight));
            }
        }
    }
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
    // Update concept neurons from this pattern
    updateConceptNeurons(pattern, currentTime);
    
    // Check if this pattern matches existing concepts
    size_t matchingConcept = findBestMatchingCluster(pattern);
    
    if (matchingConcept != std::numeric_limits<size_t>::max()) {
        // Update existing concept
        DiscoveredConcept concept = clusterToDiscoveredConcept(pImpl->clusters[matchingConcept]);
        updateConcept(concept.id, pattern, features, reward);
        return concept.id;
    }
    
    // Check if this is novel enough to form a new concept
    if (isNovel(pattern, formationThreshold_)) {
        // But wait - we need a few observations before committing to a concept
        // This prevents forming concepts from noise
        
        // For now, create concept if sufficiently different from all others
        return createConcept(pattern, features, reward);
    }
    
    return 0;  // Not yet classifiable
}

DiscoveredConcept ConceptFormation::clusterToDiscoveredConcept(const Impl::NeuralCluster& cluster) const {
    DiscoveredConcept concept;
    concept.id = cluster.id;
    concept.prototype = cluster.centroid;
    concept.totalObservations = cluster.observationCount;
    concept.avgStability = cluster.stability;
    concept.confidence = cluster.confidence;
    concept.discoveryTime = cluster.lastUpdate;
    concept.avgReward = 0.0f;  // Would need to track this separately
    concept.activationLevel = cluster.stability;
    
    // Convert cluster instances to concept instances
    for (const auto& instance : cluster.instances) {
        ConceptInstance ci;
        ci.pattern = instance.pattern;
        ci.features = {};  // Features not tracked in clusters
        ci.observationCount = instance.observationCount;
        ci.lastObserved = instance.timestamp;
        ci.avgReward = 0.0f;  // Would need to track this
        ci.stability = 1.0f - concept.stability;  // Inverse relationship
        ci.confidence = instance.confidence;
        ci.conceptId = concept.id;
        
        concept.instances.push_back(ci);
    }
    
    return concept;
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                     const std::vector<float>& features, float reward) {
    // Find the cluster for this concept
    for (auto& cluster : pImpl->clusters) {
        if (cluster.id == conceptId) {
            // Update cluster with new pattern
            updateCluster(cluster, newPattern, lastConceptUpdate_);
            return;
        }
    }
    
    // Create new cluster if not found
    createNewCluster(newPattern, lastConceptUpdate_);
}

void ConceptFormation::clear() {
    pImpl->clusters.clear();
    nextConceptId_ = 1;
    pImpl->predictionErrors.clear();
    pImpl->neuromodulationSignals.clear();
    
    // Reset concept neurons
    for (auto& neuron : pImpl->conceptNeurons) {
        neuron.activationHistory.clear();
        neuron.lastSpike = -1;
    }
}

float ConceptFormation::computePatternSimilarity(const std::vector<float>& a,
                                              const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

float ConceptFormation::computePatternConfidence(const std::vector<float>& pattern,
                                               const std::vector<float>& centroid) const {
    float similarity = computePatternSimilarity(pattern, centroid);
    return std::min(1.0f, similarity * 2.0f);
}

float ConceptFormation::computeClusterStability(const Impl::NeuralCluster& cluster) const {
    if (cluster.instances.empty()) return 0.0f;
    
    float totalStability = 0.0f;
    for (const auto& instance : cluster.instances) {
        totalStability += instance.confidence;
    }
    
    return totalStability / cluster.instances.size();
}

void ConceptFormation::updateConceptStabilityFromPredictionError(float error, float neuromodulation) {
    // Prediction error affects concept stability
    for (auto& cluster : pImpl->clusters) {
        // Reduce stability on large prediction errors
        if (error > 0.5f) {
            cluster.stability *= (1.0f - neuromodulation * 0.1f);
        } else {
            // Enhance stability on small errors
            cluster.stability = std::min(1.0f, cluster.stability * (1.0f + neuromodulation * 0.05f));
        }
        
        cluster.stability = std::max(0.0f, cluster.stability);
    }
}

void ConceptFormation::updateConceptCompetition() {
    // Simple competition model: stronger concepts suppress weaker ones
    if (pImpl->clusters.size() < 2) return;
    
    // Sort clusters by stability
    std::vector<size_t> indices(pImpl->clusters.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(),
              [this](size_t a, size_t b) {
                  return pImpl->clusters[a].stability > pImpl->clusters[b].stability;
              });
    
    // Weaker concepts may be suppressed
    for (size_t i = 1; i < indices.size(); ++i) {
        if (pImpl->clusters[indices[i]].stability < 0.3f) {
            // Weak concept may be suppressed
            pImpl->clusters[indices[i]].confidence *= 0.9f;
        }
    }
}

void ConceptFormation::updateConceptCooperation(size_t conceptId) {
    // Find concept and update associations
    for (auto& cluster : pImpl->clusters) {
        if (cluster.id == conceptId) {
            // Find similar concepts for cooperation
            for (auto& other : pImpl->clusters) {
                if (other.id != conceptId) {
                    float similarity = computePatternSimilarity(cluster.centroid, other.centroid);
                    if (similarity > 0.6f) {
                        // Strengthen association
                        cluster.confidence = std::min(1.0f, cluster.confidence * 1.1f);
                        other.confidence = std::min(1.0f, other.confidence * 1.1f);
                    }
                }
            }
            break;
        }
    }
}

std::vector<size_t> ConceptFormation::getRelevantConcepts(const std::vector<float>& goalState) const {
    std::vector<size_t> relevantConcepts;
    
    if (pImpl->clusters.empty()) return relevantConcepts;
    
    // Find concepts most similar to goal state
    std::vector<std::pair<size_t, float>> similarities;
    
    for (size_t i = 0; i < pImpl->clusters.size(); ++i) {
        float sim = computePatternSimilarity(goalState, pImpl->clusters[i].centroid);
        similarities.push_back({i, sim});
    }
    
    // Sort by similarity (descending)
    std::sort(similarities.begin(), similarities.end(),
              [](const std::pair<size_t, float>& a, const std::pair<size_t, float>& b) {
                  return a.second > b.second;
              });
    
    // Return top concepts (similarity > threshold)
    for (const auto& sim : similarities) {
        if (sim.second > 0.4f) {
            relevantConcepts.push_back(sim.first);
        } else {
            break;
        }
    }
    
    return relevantConcepts;
}

float ConceptFormation::computeClusterStability(size_t clusterId) const {
    for (const auto& cluster : pImpl->clusters) {
        if (cluster.id == clusterId) {
            return cluster.stability;
        }
    }
    return 0.0f;
}

ConceptFormation::ConceptStats ConceptFormation::getStats() const {
    ConceptStats stats;
    stats.totalConcepts = pImpl->clusters.size();
    stats.stableConcepts = 0;
    float totalStability = 0.0f;
    float totalConfidence = 0.0f;
    
    for (const auto& cluster : pImpl->clusters) {
        totalStability += cluster.stability;
        totalConfidence += cluster.confidence;
        
        if (cluster.stability > conceptStabilityThreshold_) {
            stats.stableConcepts++;
        }
    }
    
    if (!pImpl->clusters.empty()) {
        stats.meanConceptStability = totalStability / pImpl->clusters.size();
        stats.meanConceptConfidence = totalConfidence / pImpl->clusters.size();
    }
    
    // Estimate novel concepts (formed recently)
    stats.novelConcepts = 0;
    float currentTime = static_cast<float>(lastConceptUpdate_);
    
    for (const auto& cluster : pImpl->clusters) {
        if (currentTime - cluster.lastUpdate < 100.0f) {  // Recently updated
            stats.novelConcepts++;
        }
    }
    
    return stats;
}

} // namespace nlm