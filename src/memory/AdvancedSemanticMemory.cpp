#pragma once

#include "AdvancedSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <set>

namespace nlm {

AdvancedSemanticMemory::AdvancedSemanticMemory()
    : brain_(nullptr)
{
    NLM_LOG_INFO("AdvancedSemanticMemory initialized");
}

AdvancedSemanticMemory::~AdvancedSemanticMemory() {
    NLM_LOG_INFO("AdvancedSemanticMemory destroyed");
}

void AdvancedSemanticMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize AdvancedSemanticMemory with null brain");
        return;
    }
    brain_ = brain;
    NLM_LOG_INFO("AdvancedSemanticMemory initialized with brain reference");
}

void AdvancedSemanticMemory::storeFact(const std::string& concept,
                                      const std::string& attribute,
                                      const std::string& value,
                                      float baseRelevance) {
    // Find or create concept
    if (concepts_.find(concept) == concepts_.end()) {
        concepts_[concept] = SemanticConcept(concept);
    }

    SemanticConcept& semanticConcept = concepts_[concept];
    semanticConcept.attributes[attribute] = value;

    // Update activation based on relevance
    semanticConcept.activationLevel = std::min(1.0f, semanticConcept.activationLevel + baseRelevance);
    semanticConcept.lastAccessed = 0; // Would be set by simulation

    // Update reverse lookup index
    reverseLookupIndex_[value].push_back(concept);

    NLM_LOG_DEBUG("Stored semantic fact: " + concept + "." + attribute + " = " + value);
}

std::string AdvancedSemanticMemory::retrieveFact(const std::string& concept,
                                                 const std::string& attribute) const {
    auto it = concepts_.find(concept);
    if (it == concepts_.end()) {
        return "";
    }

    const SemanticConcept& semanticConcept = it->second;

    auto attrIt = semanticConcept.attributes.find(attribute);
    if (attrIt == semanticConcept.attributes.end()) {
        return "";
    }

    return attrIt->second;
}

void AdvancedSemanticMemory::establishConnection(const std::string& conceptA,
                                                 const std::string& conceptB,
                                                 float connectionStrength) {
    // Ensure both concepts exist
    if (concepts_.find(conceptA) == concepts_.end()) {
        concepts_[conceptA] = SemanticConcept(conceptA);
    }
    if (concepts_.find(conceptB) == concepts_.end()) {
        concepts_[conceptB] = SemanticConcept(conceptB);
    }

    // Create connection
    SemanticConnection connection(conceptA, conceptB, connectionStrength);
    semanticConnections_.push_back(connection);

    // Update related concepts
    concepts_[conceptA].relatedConcepts.push_back(conceptB);
    concepts_[conceptB].relatedConcepts.push_back(conceptA);

    NLM_LOG_DEBUG("Established semantic connection: " + conceptA + " <-> " + conceptB +
                  " with strength " + std::to_string(connectionStrength));
}

float AdvancedSemanticMemory::computeSemanticSimilarity(const std::string& conceptA,
                                                       const std::string& conceptB) const {
    auto itA = concepts_.find(conceptA);
    auto itB = concepts_.find(conceptB);

    if (itA == concepts_.end() || itB == concepts_.end()) {
        return 0.0f;
    }

    const SemanticConcept& conceptAData = itA->second;
    const SemanticConcept& conceptBData = itB->second;

    // Count shared attributes
    int sharedAttributes = 0;
    for (const auto& attr : conceptAData.attributes) {
        if (conceptBData.attributes.find(attr.first) != conceptBData.attributes.end()) {
            sharedAttributes++;
        }
    }

    // Compute similarity
    float similarity = static_cast<float>(sharedAttributes) / std::max(conceptAData.attributes.size(), conceptBData.attributes.size());

    return similarity;
}

std::string AdvancedSemanticMemory::completePattern(const std::string& partialConcept,
                                                    const std::string& partialAttribute) const {
    std::string resultConcept = partialConcept;
    std::string resultAttribute = partialAttribute;

    // Try to complete concept
    if (partialConcept.empty()) {
        // Find most similar concept with matching attribute
        for (const auto& pair : reverseLookupIndex_) {
            for (const auto& concept : pair.second) {
                auto it = concepts_.find(concept);
                if (it != concepts_.end() && !it->second.attributes.empty()) {
                    resultConcept = concept;
                    break;
                }
            }
            if (!resultConcept.empty()) {
                break;
            }
        }
    }

    // Try to complete attribute
    if (partialAttribute.empty()) {
        // Find most common attribute
        std::unordered_map<std::string, int> attributeCount;
        for (const auto& concept : concepts_) {
            for (const auto& attr : concept.second.attributes) {
                attributeCount[attr.first]++;
            }
        }

        if (!attributeCount.empty()) {
            std::string mostCommonAttr = attributeCount.begin()->first;
            int maxCount = attributeCount.begin()->second;

            for (const auto& pair : attributeCount) {
                if (pair.second > maxCount) {
                    mostCommonAttr = pair.first;
                    maxCount = pair.second;
                }
            }

            resultAttribute = mostCommonAttr;
        }
    }

    return resultConcept + "." + resultAttribute;
}

void AdvancedSemanticMemory::generalizePattern(const std::string& specificConcept) {
    auto it = concepts_.find(specificConcept);
    if (it == concepts_.end()) {
        return;
    }

    const SemanticConcept& sourceConcept = it->second;

    // Find similar concepts
    std::vector<std::string> similarConcepts;
    for (const auto& pair : concepts_) {
        if (pair.first != specificConcept) {
            float similarity = computeSemanticSimilarity(specificConcept, pair.first);
            if (similarity > 0.7f) { // Threshold for similarity
                similarConcepts.push_back(pair.first);
            }
        }
    }

    if (similarConcepts.empty()) {
        return;
    }

    // Create generalized concept
    std::string generalizedConcept = "general_" + specificConcept;

    // Merge attributes from similar concepts
    SemanticConcept generalizedSemanticConcept(generalizedConcept);

    for (const auto& similarConceptId : similarConcepts) {
        auto similarIt = concepts_.find(similarConceptId);
        if (similarIt != concepts_.end()) {
            for (const auto& attr : similarIt->second.attributes) {
                generalizedSemanticConcept.attributes[attr.first] = attr.second;
            }
        }
    }

    // Add the source concept's attributes
    for (const auto& attr : sourceConcept.attributes) {
        generalizedSemanticConcept.attributes[attr.first] = attr.second;
    }

    concepts_[generalizedConcept] = generalizedSemanticConcept;

    NLM_LOG_DEBUG("Created generalized concept: " + generalizedConcept);
}

void AdvancedSemanticMemory::createAbstraction(const std::vector<std::string>& relatedConcepts) {
    if (relatedConcepts.size() < 2) {
        return;
    }

    std::string abstractionName = "abstract_";
    for (const auto& concept : relatedConcepts) {
        abstractionName += concept[0];
    }

    SemanticConcept abstractionConcept(abstractionName);

    // Extract common attributes from related concepts
    std::unordered_map<std::string, int> attributeCounts;

    for (const auto& conceptId : relatedConcepts) {
        auto it = concepts_.find(conceptId);
        if (it != concepts_.end()) {
            for (const auto& attr : it->second.attributes) {
                attributeCounts[attr.first]++;
            }
        }
    }

    // Add attributes that appear in most concepts
    int threshold = relatedConcepts.size() / 2;
    for (const auto& pair : attributeCounts) {
        if (pair.second >= threshold) {
            abstractionConcept.attributes[pair.first] = "unknown"; // We don't have values
        }
    }

    concepts_[abstractionName] = abstractionConcept;

    NLM_LOG_DEBUG("Created abstraction: " + abstractionName);
}

std::vector<std::tuple<std::string, std::string, std::string>> AdvancedSemanticMemory::getAllFacts() const {
    std::vector<std::tuple<std::string, std::string, std::string>> facts;

    for (const auto& conceptPair : concepts_) {
        const SemanticConcept& concept = conceptPair.second;

        for (const auto& attrPair : concept.attributes) {
            facts.emplace_back(concept.id, attrPair.first, attrPair.second);
        }
    }

    return facts;
}

void AdvancedSemanticMemory::clear() {
    concepts_.clear();
    semanticConnections_.clear();
    reverseLookupIndex_.clear();

    NLM_LOG_INFO("AdvancedSemanticMemory cleared");
}

void AdvancedSemanticMemory::updateConceptActivation(const std::string& conceptId, float delta) {
    auto it = concepts_.find(conceptId);
    if (it != concepts_.end()) {
        it->second.activationLevel = std::min(1.0f, it->second.activationLevel + delta);
    }
}

void AdvancedSemanticMemory::applySemanticDecay() {
    for (auto& pair : concepts_) {
        pair.second.activationLevel *= 0.95f;
    }

    for (auto& connection : semanticConnections_) {
        connection.strength *= 0.9f;
    }
}

void AdvancedSemanticMemory::completePatternRequests() {
    // Process pending pattern completion requests
    // Implementation would be added here
}

void AdvancedSemanticMemory::generalizeFromSimilarity(const std::string& conceptA,
                                                     const std::string& conceptB) {
    float similarity = computeSemanticSimilarity(conceptA, conceptB);

    if (similarity > 0.8f) {
        // Create generalized version
        std::string generalized = "general_" + conceptA + "_and_" + conceptB;
        // Implementation would add generalized concept here
    }
}

} // namespace nlm