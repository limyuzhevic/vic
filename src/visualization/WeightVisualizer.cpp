#include "WeightVisualizer.hpp"
#include "VisualizationEngine.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

namespace nlm {

struct WeightVisualizer::Impl {
    VisualizationEngine* engine;
    bool active;
    
    // Weight data
    std::vector<std::vector<float>> weightMatrix;  // 2D matrix of weights
    std::vector<std::vector<float>> regionalConnectivity; // Regional connectivity matrix
    
    // Configuration
    bool showExcitationInhibition;
    bool showRegionalConnectivity;
    float weightScale;
    int colorScheme;
    bool showStatistics;
    bool filterByType;
    bool filterByRegion;
    
    // Matrix display settings
    size_t matrixRows;
    size_t matrixCols;
    bool showDiagonal;
    bool symmetrize;
    
    // Performance tracking
    double lastUpdateTime;
    
    Impl()
        : active(false)
        , showExcitationInhibition(true)
        , showRegionalConnectivity(true)
        , weightScale(1.0f)
        , colorScheme(0)
        , showStatistics(true)
        , filterByType(false)
        , filterByRegion(false)
        , matrixRows(0)
        , matrixCols(0)
        , showDiagonal(false)
        , symmetrize(false)
        , lastUpdateTime(0.0) {}
};

WeightVisualizer::WeightVisualizer() : pImpl(new Impl) {}
WeightVisualizer::~WeightVisualizer() = default;

bool WeightVisualizer::initialize(VisualizationEngine* engine) {
    pImpl->engine = engine;
    pImpl->active = true;
    
    // Initialize backend-specific resources
    // TODO: Implement based on backend
    
    return true;
}

void WeightVisualizer::update(const Brain& brain, double timestep) {
    if (!pImpl->active) return;
    
    // Update weight matrix data
    pImpl->weightMatrix.clear();
    pImpl->regionalConnectivity.clear();
    
    // Get all regions
    const auto& regions = brain.getRegions();
    pImpl->matrixRows = regions.size();
    pImpl->matrixCols = regions.size();
    
    // Build regional connectivity matrix
    pImpl->regionalConnectivity.resize(pImpl->matrixRows, std::vector<float>(pImpl->matrixCols, 0.0f));
    
    for (size_t i = 0; i < regions.size(); ++i) {
        for (size_t j = 0; j < regions.size(); ++j) {
            if (i == j && !pImpl->showDiagonal) {
                continue;
            }
            
            // Calculate average connectivity between regions i and j
            float totalWeight = 0.0f;
            size_t synapseCount = 0;
            
            // Get all synapses in region i
            const auto& regionI = *regions[i];
            for (const auto* syn : regionI.getSynapses()) {
                // Find destination region
                NeuronId destId = syn->getDestinationNeuron();
                // TODO: Get region ID from neuron ID
                // For now, assume all connections within region have weight
                if (i == j) {  // Same region
                    totalWeight += std::abs(syn->getWeight());
                    synapseCount++;
                }
            }
            
            pImpl->regionalConnectivity[i][j] = 
                synapseCount > 0 ? totalWeight / synapseCount : 0.0f;
        }
    }
    
    if (pImpl->symmetrize) {
        // Make matrix symmetric
        for (size_t i = 0; i < pImpl->matrixRows; ++i) {
            for (size_t j = 0; j < i; ++j) {
                float avg = (pImpl->regionalConnectivity[i][j] + pImpl->regionalConnectivity[j][i]) * 0.5f;
                pImpl->regionalConnectivity[i][j] = avg;
                pImpl->regionalConnectivity[j][i] = avg;
            }
        }
    }
    
    pImpl->lastUpdateTime = 0.0;  // TODO: Get from clock
}

void WeightVisualizer::render() {
    if (!pImpl->active) return;
    
    // Render weight matrix visualization
    // TODO: Implement backend-specific rendering
    
    // Example rendering steps:
    // 1. Setup heatmap visualization
    // 2. For each cell in matrix:
    //    - Determine color based on weight and scheme
    //    - Draw rectangle with appropriate color and size
    // 3. Draw axis labels (region IDs)
    // 4. Draw statistics overlay if enabled
    // 5. Highlight strong connections if any
}

void WeightVisualizer::close() {
    pImpl->active = false;
    
    // Cleanup backend-specific resources
    // TODO: Implement based on backend
}

void WeightVisualizer::setShowExcitationInhibition(bool show) {
    pImpl->showExcitationInhibition = show;
}

void WeightVisualizer::setShowRegionalConnectivity(bool show) {
    pImpl->showRegionalConnectivity = show;
}

void WeightVisualizer::setWeightScale(float scale) {
    pImpl->weightScale = scale;
}

void WeightVisualizer::setColorScheme(int scheme) {
    pImpl->colorScheme = scheme;
}

void WeightVisualizer::setShowStatistics(bool show) {
    pImpl->showStatistics = show;
}

void WeightVisualizer::setFilterByType(bool filter) {
    pImpl->filterByType = filter;
}

void WeightVisualizer::setFilterByRegion(bool filter) {
    pImpl->filterByRegion = filter;
}

void WeightVisualizer::setMatrixSize(size_t rows, size_t cols) {
    pImpl->matrixRows = rows;
    pImpl->matrixCols = cols;
    pImpl->regionalConnectivity.resize(rows, std::vector<float>(cols, 0.0f));
}

void WeightVisualizer::setShowDiagonal(bool show) {
    pImpl->showDiagonal = show;
}

void WeightVisualizer::setSymmetrize(bool symmetrize) {
    pImpl->symmetrize = symmetrize;
}

bool WeightVisualizer::exportWeightData(const std::string& filepath, ExportFormat format) {
    // Export weight data in requested format
    // TODO: Implement format-specific export
    return false;
}

bool WeightVisualizer::isActive() const {
    return pImpl->active;
}

WeightVisualizer::WeightStats WeightVisualizer::getStats() const {
    WeightStats stats;
    
    // Calculate statistics from regional connectivity matrix
    if (pImpl->regionalConnectivity.empty()) {
        return stats;
    }
    
    float totalWeight = 0.0f;
    float maxWeight = -std::numeric_limits<float>::infinity();
    float minWeight = std::numeric_limits<float>::infinity();
    size_t count = 0;
    
    for (const auto& row : pImpl->regionalConnectivity) {
        for (float weight : row) {
            if (std::abs(weight) > 0.0001f) {  // Ignore near-zero weights
                totalWeight += std::abs(weight);
                maxWeight = std::max(maxWeight, std::abs(weight));
                minWeight = std::min(minWeight, std::abs(weight));
                count++;
            }
        }
    }
    
    stats.totalWeight = totalWeight;
    stats.averageWeight = count > 0 ? totalWeight / count : 0.0f;
    stats.maxWeight = maxWeight;
    stats.minWeight = minWeight;
    
    // Calculate E/I ratio
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    for (const auto& row : pImpl->regionalConnectivity) {
        for (float weight : row) {
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    stats.excitationInhibitionRatio = totalInhibitory > 0.0f ? totalExcitatory / totalInhibitory : 
                                       totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
    
    stats.totalExcitatory = totalExcitatory;
    stats.totalInhibitory = totalInhibitory;
    
    // Count strong connections (above threshold)
    float threshold = stats.averageWeight * 2.0f;
    stats.strongConnections = 0;
    for (const auto& row : pImpl->regionalConnectivity) {
        for (float weight : row) {
            if (std::abs(weight) > threshold) {
                stats.strongConnections++;
            }
        }
    }
    
    // Calculate mean degree (average connections per region)
    stats.meanDegree = count / static_cast<float>(pImpl->matrixRows * pImpl->matrixCols);
    
    // Connection density
    stats.connectionDensity = static_cast<float>(count) / 
                             (pImpl->matrixRows * pImpl->matrixCols - (pImpl->showDiagonal ? 0 : pImpl->matrixRows));
    
    // Regional weights (average weight per region)
    stats.regionalWeights.resize(pImpl->matrixRows, 0.0f);
    for (size_t i = 0; i < pImpl->matrixRows; ++i) {
        float rowSum = 0.0f;
        size_t rowCount = 0;
        for (size_t j = 0; j < pImpl->matrixCols; ++j) {
            if (i != j || pImpl->showDiagonal) {
                if (std::abs(pImpl->regionalConnectivity[i][j]) > 0.0001f) {
                    rowSum += std::abs(pImpl->regionalConnectivity[i][j]);
                    rowCount++;
                }
            }
        }
        stats.regionalWeights[i] = rowCount > 0 ? rowSum / rowCount : 0.0f;
    }
    
    // TODO: Implement type-specific statistics
    // Would need access to neuron types and their connections
    
    return stats;
}

} // namespace nlm
