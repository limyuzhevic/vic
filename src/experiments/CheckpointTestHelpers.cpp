// Phase6IntegratedExperiment.cpp - Helper functions for checkpoint test verification

#include <filesystem>
#include <algorithm>
#include <chrono>
#include <sstream>

namespace nlm {

// Helper function to calculate neuron similarity
float calculateNeuronSimilarity(const Neuron* n1, const Neuron* n2) {
    if (!n1 || !n2) return 0.0f;
    
    float v1 = n1->getMembranePotential();
    float v2 = n2->getMembranePotential();
    float t1 = n1->getThreshold();
    float t2 = n2->getThreshold();
    float rest1 = n1->getRestingPotential();
    float rest1 = n1->getRestingPotential();
    float rest2 = n2->getRestingPotential();
    
    float norm1 = std::abs(v1 - rest1) + std::abs(t1 - rest1);
    float norm2 = std::abs(v2 - rest2) + std::abs(t2 - rest2);
    
    if (norm1 < 0.01f || norm2 < 0.01f) return 1.0f;
    
    float diff = std::abs(v1 - v2) + std::abs(t1 - t2);
    return 1.0f - std::min(1.0f, diff / (norm1 + norm2));
}

// Helper function to count firing neurons above threshold
int countHighFiringNeurons(const std::vector<Neuron*>& neurons, float threshold) {
    int count = 0;
    for (const auto* neuron : neurons) {
        if (neuron && neuron->isFiring() && 
            std::abs(neuron->getMembranePotential() - neuron->getRestingPotential()) > threshold) {
            ++count;
        }
    }
    return count;
}

// Helper function to calculate average synaptic weight
float calculateAverageSynapticWeight(const NeuralRegion* region) {
    if (!region) return 0.0f;
    
    auto synapses = region->getSynapses();
    if (synapses.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto* syn : synapses) {
        total += std::abs(syn->getWeight());
    }
    
    return total / synapses.size();
}

// Helper function to find most similar neuron
NeuronId findMostSimilarNeuron(NeuronId targetId, const std::vector<Neuron*>& neurons) {
    NeuronId bestMatch = NeuronId(); // Invalid ID
    float bestSimilarity = 0.0f;
    
    for (const auto* neuron : neurons) {
        if (neuron && neuron->getId() != targetId) {
            float similarity = calculateNeuronSimilarity(targetId, neuron);
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestMatch = neuron->getId();
            }
        }
    }
    
    return bestSimilarity > 0.5f ? bestMatch : NeuronId();
}

// Helper function to format neuron state for comparison
std::string formatNeuronState(const Neuron* neuron) {
    if (!neuron) return "(invalid)";
    
    std::stringstream ss;
    ss << "Neuron " << neuron->getId().index() 
       << " V:" << std::fixed << std::setprecision(2) << neuron->getMembranePotential()
       << " T:" << neuron->getThreshold()
       << " R:" << neuron->getRestingPotential()
       << " F:" << (neuron->isFiring() ? "1" : "0")
       << " Ref:" << neuron->getRefractoryRemaining();
    return ss.str();
}

// Helper function to calculate correlation coefficient
float calculateCorrelation(const std::vector<float>& v1, const std::vector<float>& v2) {
    if (v1.size() != v2.size() || v1.size() < 2) return 0.0f;
    
    float sum1 = std::accumulate(v1.begin(), v1.end(), 0.0f);
    float sum2 = std::accumulate(v2.begin(), v2.end(), 0.0f);
    float sum1sq = std::inner_product(v1.begin(), v1.end(), v1.begin(), 0.0f);
    float sum2sq = std::inner_product(v2.begin(), v2.end(), v2.begin(), 0.0f);
    float sumprod = std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0f);
    
    float n = v1.size();
    float num = n * sumprod - sum1 * sum2;
    float den = std::sqrt((n * sum1sq - sum1 * sum1) * (n * sum2sq - sum2 * sum2));
    
    return den > 0.0f ? num / den : 0.0f;
}

} // namespace nlm
