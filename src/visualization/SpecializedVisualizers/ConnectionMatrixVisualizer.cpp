#include "ConnectionMatrixVisualizer.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace nlm {

struct ConnectionMatrixVisualizer::Impl {
    // Matrix data storage
    std::vector<std::vector<ConnectionMatrixElement>> matrix;
    size_t matrixSize;                     // Size of the matrix (n x n)
    std::vector<NeuronId> neuronIds;       // Mapping from matrix indices to neuron IDs
    std::map<NeuronId, size_t> neuronIndexMap; // Reverse mapping
    
    // Analysis data
    ConnectionMatrixAnalyzer matrixAnalyzer;
    std::vector<NetworkCommunity> communities;
    std::vector<std::vector<float>> pathLengths;   // Distance matrix
    std::vector<size_t> degreeDistribution;  // Degree distribution
    std::map<std::string, float> hubMetrics; // Hub metrics
    
    // Configuration
    ConnectionMatrixConfig config;
    bool advancedAnalysisEnabled; // Whether advanced analysis is enabled
    
    // Interaction state
    bool mousePressed;           // Whether mouse button is pressed
    float lastMouseX;            // Last mouse X position
    float lastMouseY;            // Last mouse Y position
    std::vector<ConnectionMatrixElement> selectedElements; // Selected elements
    ConnectionMatrixElement hoveredElement;   // Currently hovered element
    
    // Rendering state
    float zoomLevel;             // Current zoom level
    float panX;                  // X pan offset
    float panY;                  // Y pan offset
    bool showDebugInfo;          // Whether to show debug info
    bool showGrid;              // Whether to show grid lines
    
    // Performance optimization
    bool lodEnabled;             // Whether level of detail is enabled
    size_t maxMatrixSize;        // Maximum matrix size to display
    bool sparseOptimization;    // Whether sparse matrix optimization is enabled
    std::vector<bool> elementVisible;  // Visibility of elements
    
    // Colors and styling
    std::map<std::string, SDL_Color> colorMap; // Color map for visualization
    std::map<std::string, std::vector<float>> gradientStops; // Gradient stops
    
    // Font rendering
    TTF_Font* font;              // Font for text rendering
    TTF_Font* fontSmall;         // Small font
    
    // Statistics
    uint64_t totalConnections;   // Total number of connections
    float meanWeight;            // Mean synaptic weight
    float totalWeight;           // Total synaptic weight
    float maxWeight;             // Maximum weight
    float minWeight;             // Minimum weight
    
    Impl() : matrixSize(0), advancedAnalysisEnabled(false),
             mousePressed(false), lastMouseX(0.0f), lastMouseY(0.0f),
             hoveredElement(), zoomLevel(1.0f), panX(0.0f), panY(0.0f),
             showDebugInfo(false), showGrid(true), lodEnabled(true), 
             maxMatrixSize(100), sparseOptimization(true), totalConnections(0),
             meanWeight(0.0f), totalWeight(0.0f), maxWeight(0.0f), minWeight(0.0f),
             font(nullptr), fontSmall(nullptr) {
        
        // Initialize color map
        colorMap["weight"] = {255, 100, 100, 255};         // Red for weights
        colorMap["delay"] = {100, 255, 100, 255};          // Green for delays
        colorMap["type_excitatory"] = {100, 100, 255, 255};     // Blue for excitatory
        colorMap["type_inhibitory"] = {255, 100, 255, 255};    // Magenta for inhibitory
        colorMap["type_modulatory"] = {200, 200, 100, 255};   // Yellow for modulatory
        colorMap["plastic"] = {255, 150, 0, 255};              // Orange for plastic
        colorMap["active"] = {255, 200, 0, 255};               // Yellow for active
        colorMap["background"] = {30, 30, 30, 255};          // Dark gray
        colorMap["grid"] = {60, 60, 60, 255};                // Grid lines
        colorMap["text"] = {200, 200, 200, 255};             // Light gray
        colorMap["community"] = {150, 150, 150, 255};        // Gray for communities
    }
};

ConnectionMatrixVisualizer::ConnectionMatrixVisualizer() : pImpl(std::make_unique<Impl>()) {
}

ConnectionMatrixVisualizer::~ConnectionMatrixVisualizer() {
    close();
}

bool ConnectionMatrixVisualizer::initialize() {
    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
        pImpl->font = nullptr;
    }
    
    if (pImpl->fontSmall) {
        TTF_CloseFont(pImpl->fontSmall);
        pImpl->fontSmall = nullptr;
    }
    
    // Load fonts
    pImpl->font = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 10);
    pImpl->fontSmall = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 8);
    
    if (!pImpl->font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    
    pImpl->selectedElements.clear();
    pImpl->hoveredElement = ConnectionMatrixElement();
    
    return true;
}

void ConnectionMatrixVisualizer::update() {
    // Update matrix elements from brain data
    // Note: This would require access to brain instance
    // For now, placeholder implementation
    
    // Compute matrix analysis
    computeMatrixAnalysis();
    
    // Analyze community structure if enabled
    if (pImpl->advancedAnalysisEnabled) {
        analyzeCommunityStructure();
        computePathLengthDistribution();
        analyzeHubStructure();
    }
}

void ConnectionMatrixVisualizer::render() {
    if (pImpl->matrix.empty()) {
        return;
    }
    
    // Clear background
    SDL_SetRenderDrawColor(pImpl->renderer, pImpl->colorMap["background"].r,
                          pImpl->colorMap["background"].g, pImpl->colorMap["background"].b,
                          pImpl->colorMap["background"].a);
    SDL_RenderClear(pImpl->renderer);
    
    // Apply transformation
    SDL_RenderSetScale(pImpl->renderer, pImpl->zoomLevel, pImpl->zoomLevel);
    SDL_RenderSetLogicalTranslation(pImpl->renderer, -pImpl->panX, -pImpl->panY);
    
    // Render grid
    if (pImpl->showGrid) {
        renderMatrixGrid();
    }
    
    // Render matrix background
    renderMatrixBackground();
    
    // Render matrix elements
    renderMatrixElements();
    
    // Render selection
    if (!pImpl->selectedElements.empty()) {
        renderSelection();
    }
    
    // Render statistics
    if (pImpl->config.showStatistics) {
        renderStatistics();
    }
    
    // Render legend
    renderLegend();
    
    // Render debug info
    if (pImpl->showDebugInfo) {
        renderDebugInfo();
    }
    
    // Reset transformation
    SDL_RenderSetScale(pImpl->renderer, 1.0f, 1.0f);
    SDL_RenderSetLogicalTranslation(pImpl->renderer, 0, 0);
}

void ConnectionMatrixVisualizer::close() {
    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
        pImpl->font = nullptr;
    }
    
    if (pImpl->fontSmall) {
        TTF_CloseFont(pImpl->fontSmall);
        pImpl->fontSmall = nullptr;
    }
}

bool ConnectionMatrixVisualizer::isActive() const {
    return pImpl->initialized;
}

void ConnectionMatrixVisualizer::setUpdateRate(double hz) {
    // Note: Controlled by visualization engine
}

double ConnectionMatrixVisualizer::getUpdateRate() const {
    return 60.0;
}

void ConnectionMatrixVisualizer::visualizeConnectionMatrix(const Brain& brain) {
    // Create connection matrix from brain data
    pImpl->matrix.clear();
    pImpl->neuronIds.clear();
    pImpl->neuronIndexMap.clear();
    
    // Get all neurons
    auto neurons = brain.getAllNeurons();
    pImpl->neuronIds.reserve(neurons.size());
    
    for (const auto& neuron : neurons) {
        pImpl->neuronIds.push_back(neuron->getId());
        pImpl->neuronIndexMap[neuron->getId()] = pImpl->neuronIds.size() - 1;
    }
    
    pImpl->matrixSize = pImpl->neuronIds.size();
    pImpl->matrix.resize(pImpl->matrixSize, std::vector<ConnectionMatrixElement>(pImpl->matrixSize));
    
    // Get all regions to access synapses
    auto regions = brain.getRegions();
    
    for (const auto& region : regions) {
        for (const auto& synapse : region->getSynapses()) {
            size_t preIdx = pImpl->neuronIndexMap.find(synapse->getSourceId()) != pImpl->neuronIndexMap.end()
                          ? pImpl->neuronIndexMap[synapse->getSourceId()]
                          : pImpl->matrixSize; // Out of bounds if not found
            size_t postIdx = pImpl->neuronIndexMap.find(synapse->getTargetId()) != pImpl->neuronIndexMap.end()
                           ? pImpl->neuronIndexMap[synapse->getTargetId()]
                           : pImpl->matrixSize; // Out of bounds if not found
            
            if (preIdx < pImpl->matrixSize && postIdx < pImpl->matrixSize) {
                // Create matrix element
                ConnectionMatrixElement element;
                element.synapseId = synapse->getId();
                element.preNeuronId = synapse->getSourceId();
                element.postNeuronId = synapse->getTargetId();
                element.weight = synapse->getWeight();
                element.delay = synapse->getDelay();
                element.type = synapse->getType();
                element.isActive = synapse->isActive();
                element.isPlastic = synapse->hasPlasticityEnabled(); // Placeholder
                element.lastModified = 0.0f; // Placeholder
                
                // Set matrix element
                pImpl->matrix[preIdx][postIdx] = element;
                
                // For undirected matrices, set both directions
                if (!pImpl->config.showDiagonal && preIdx == postIdx) {
                    // Skip diagonal for undirected view
                    continue;
                }
                
                if (preIdx != postIdx) {
                    pImpl->matrix[postIdx][preIdx] = element; // Symmetric for undirected
                }
            }
        }
    }
    
    // Compute matrix analysis
    computeMatrixAnalysis();
    
    // Analyze community structure
    analyzeCommunityStructure();
}

void ConnectionMatrixVisualizer::computeMatrixAnalysis() {
    pImpl->matrixAnalyzer = ConnectionMatrixAnalyzer();
    pImpl->degreeDistribution.clear();
    pImpl->hubMetrics.clear();
    
    if (pImpl->matrix.empty()) {
        return;
    }
    
    // Compute basic statistics
    float totalWeight = 0.0f;
    float minWeight = std::numeric_limits<float>::max();
    float maxWeight = std::numeric_limits<float>::lowest();
    size_t connectionCount = 0;
    
    for (size_t i = 0; i < pImpl->matrixSize; ++i) {
        for (size_t j = 0; j < pImpl->matrixSize; ++j) {
            if (pImpl->config.showOffDiagonal && i == j && !pImpl->config.showDiagonal) {
                continue; // Skip diagonal
            }
            
            const auto& element = pImpl->matrix[i][j];
            if (element.weight > 0.0f) {
                totalWeight += element.weight;
                minWeight = std::min(minWeight, element.weight);
                maxWeight = std::max(maxWeight, element.weight);
                connectionCount++;
            }
        }
    }
    
    pImpl->matrixAnalyzer.totalWeight = totalWeight;
    pImpl->matrixAnalyzer.meanWeight = connectionCount > 0 ? totalWeight / connectionCount : 0.0f;
    pImpl->matrixAnalyzer.minWeight = minWeight;
    pImpl->matrixAnalyzer.maxWeight = maxWeight;
    pImpl->matrixAnalyzer.connectionCount = connectionCount;
    
    // Calculate matrix density
    size_t maxPossibleConnections = pImpl->matrixSize * (pImpl->matrixSize - 1); // Undirected, no self-connections
    if (pImpl->config.showDiagonal) {
        maxPossibleConnections = pImpl->matrixSize * pImpl->matrixSize; // Include diagonal
    }
    
    pImpl->matrixAnalyzer.density = maxPossibleConnections > 0 ? 
        static_cast<float>(connectionCount) / maxPossibleConnections : 0.0f;
    
    // Compute degree distribution
    std::vector<size_t> degrees(pImpl->matrixSize, 0);
    for (size_t i = 0; i < pImpl->matrixSize; ++i) {
        for (size_t j = 0; j < pImpl->matrixSize; ++j) {
            if (pImpl->config.showOffDiagonal && i == j && !pImpl->config.showDiagonal) {
                continue;
            }
            
            if (pImpl->matrix[i][j].weight > 0.0f) {
                degrees[i]++;
                degrees[j]++; // Undirected matrix
            }
        }
    }
    
    pImpl->matrixAnalyzer.degreeDistribution = degrees;
    
    // Find maximum degree for histogram
    size_t maxDegree = 0;
    for (size_t degree : degrees) {
        maxDegree = std::max(maxDegree, degree);
    }
    
    // Create degree histogram
    pImpl->matrixAnalyzer.degreeHistogram.resize(maxDegree + 1, 0);
    for (size_t degree : degrees) {
        pImpl->matrixAnalyzer.degreeHistogram[degree]++;
    }
    
    // Calculate clustering coefficient (simplified)
    float totalClustering = 0.0f;
    size_t triangleCount = 0;
    size_t possibleTriangles = 0;
    
    for (size_t i = 0; i < pImpl->matrixSize; ++i) {
        for (size_t j = i + 1; j < pImpl->matrixSize; ++j) {
            if (pImpl->matrix[i][j].weight <= 0.0f) continue;
            
            // Count common neighbors
            size_t commonNeighbors = 0;
            for (size_t k = 0; k < pImpl->matrixSize; ++k) {
                if (k == i || k == j) continue;
                if (pImpl->matrix[i][k].weight > 0.0f && pImpl->matrix[j][k].weight > 0.0f) {
                    commonNeighbors++;
                }
            }
            
            triangleCount += commonNeighbors;
            possibleTriangles += (pImpl->matrixSize - 2);
        }
    }
    
    pImpl->matrixAnalyzer.clusteringCoefficient = possibleTriangles > 0 ? 
        static_cast<float>(triangleCount) / possibleTriangles : 0.0f;
    
    // Update statistics
    pImpl->totalConnections = connectionCount;
    pImpl->meanWeight = pImpl->matrixAnalyzer.meanWeight;
    pImpl->totalWeight = totalWeight;
    pImpl->maxWeight = maxWeight;
    pImpl->minWeight = minWeight;
}

void ConnectionMatrixVisualizer::analyzeCommunityStructure() {
    // Analyze community structure in the matrix
    // Placeholder implementation
    pImpl->communities.clear();
}

void ConnectionMatrixVisualizer::computePathLengthDistribution() {
    // Compute shortest path lengths between all nodes
    // Placeholder implementation
    pImpl->pathLengths.clear();
}

void ConnectionMatrixVisualizer::analyzeHubStructure() {
    // Analyze hub structure and identify hub neurons
    // Placeholder implementation
    pImpl->hubMetrics.clear();
}

void ConnectionMatrixVisualizer::zoom(float factor) {
    pImpl->zoomLevel *= factor;
    pImpl->zoomLevel = std::max(0.1f, std::min(10.0f, pImpl->zoomLevel));
}

void ConnectionMatrixVisualizer::pan(float dx, float dy) {
    pImpl->panX += dx;
    pImpl->panY += dy;
}

void ConnectionMatrixVisualizer::selectElement(const ConnectionMatrixElement& element) {
    if (pImpl->config.enableSelection) {
        pImpl->selectedElements.push_back(element);
    }
}

void ConnectionMatrixVisualizer::clearSelection() {
    pImpl->selectedElements.clear();
}

std::vector<ConnectionMatrixElement> ConnectionMatrixVisualizer::getSelectedElements() const {
    return pImpl->selectedElements;
}

void ConnectionMatrixVisualizer::exportMatrixData(const std::string& filename, const std::string& format) {
    std::ofstream file(filename);
    if (file.is_open()) {
        if (format == "csv") {
            file << "PreNeuronId,PostNeuronId,Weight,Delay,Type,Active,Plastic" << std::endl;
            for (size_t i = 0; i < pImpl->matrixSize; ++i) {
                for (size_t j = 0; j < pImpl->matrixSize; ++j) {
                    if (pImpl->matrix[i][j].weight <= 0.0f) continue;
                    
                    const auto& element = pImpl->matrix[i][j];
                    file << element.preNeuronId.index() << ","
                         << element.postNeuronId.index() << ","
                         << element.weight << ","
                         << element.delay << ","
                         << static_cast<int>(element.type) << ","
                         << (element.isActive ? "1" : "0") << ","
                         << (element.isPlastic ? "1" : "0") << std::endl;
                }
            }
        } else {
            file << "Connection matrix exported to " << filename << std::endl;
        }
        file.close();
    }
}

void ConnectionMatrixVisualizer::exportAnalysisResults(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "=== Connection Matrix Analysis ===" << std::endl;
        file << "Matrix size: " << pImpl->matrixSize << "x" << pImpl->matrixSize << std::endl;
        file << "Total connections: " << pImpl->matrixAnalyzer.connectionCount << std::endl;
        file << "Matrix density: " << pImpl->matrixAnalyzer.density << std::endl;
        file << "Total weight: " << pImpl->matrixAnalyzer.totalWeight << std::endl;
        file << "Mean weight: " << pImpl->matrixAnalyzer.meanWeight << std::endl;
        file << "Min weight: " << pImpl->matrixAnalyzer.minWeight << std::endl;
        file << "Max weight: " << pImpl->matrixAnalyzer.maxWeight << std::endl;
        file << "Clustering coefficient: " << pImpl->matrixAnalyzer.clusteringCoefficient << std::endl;
        file.close();
    }
}

std::string ConnectionMatrixVisualizer::analyzeConnectionTopology() const {
    std::stringstream ss;
    ss << "=== Connection Topology Analysis ===" << std::endl;
    ss << "Matrix size: " << pImpl->matrixSize << "x" << pImpl->matrixSize << std::endl;
    ss << "Total connections: " << pImpl->matrixAnalyzer.connectionCount << std::endl;
    ss << "Matrix density: " << pImpl->matrixAnalyzer.density << std::endl;
    ss << "Average degree: " << (pImpl->matrixAnalyzer.degreeHistogram.empty() ? 0.0f :
                                (std::accumulate(pImpl->matrixAnalyzer.degreeHistogram.begin(),
                                               pImpl->matrixAnalyzer.degreeHistogram.end(), 0.0) /
                                 std::accumulate(pImpl->matrixAnalyzer.degreeHistogram.begin(),
                                               pImpl->matrixAnalyzer.degreeHistogram.end(), 0)))
       << std::endl;
    return ss.str();
}

std::vector<std::string> ConnectionMatrixVisualizer::analyzeHubStructure() const {
    std::vector<std::string> hubAnalysis;
    
    if (pImpl->matrixAnalyzer.degreeHistogram.empty()) {
        return hubAnalysis;
    }
    
    // Find hub neurons (high degree)
    size_t maxDegree = pImpl->matrixAnalyzer.degreeHistogram.size() - 1;
    size_t hubThreshold = static_cast<size_t>(maxDegree * 0.8);  // Top 20% as hubs
    
    hubAnalysis.push_back("Hub Analysis:");
    hubAnalysis.push_back("  Max degree: " + std::to_string(maxDegree));
    hubAnalysis.push_back("  Hub threshold: " + std::to_string(hubThreshold));
    hubAnalysis.push_back("  Number of hubs: " + std::to_string(
        std::accumulate(pImpl->matrixAnalyzer.degreeHistogram.begin() + hubThreshold,
                       pImpl->matrixAnalyzer.degreeHistogram.end(), 0)));
    
    return hubAnalysis;
}

void ConnectionMatrixVisualizer::enableLOD(bool enable) {
    pImpl->lodEnabled = enable;
}

void ConnectionMatrixVisualizer::setMaxMatrixSize(size_t size) {
    pImpl->maxMatrixSize = size;
}

void ConnectionMatrixVisualizer::enableSparseMatrixOptimization(bool enable) {
    pImpl->sparseOptimization = enable;
}

void ConnectionMatrixVisualizer::renderMatrixBackground() {
    // Render matrix background
    // Placeholder implementation
}

void ConnectionMatrixVisualizer::renderMatrixGrid() {
    // Render matrix grid lines
    // Placeholder implementation
}

void ConnectionMatrixVisualizer::renderMatrixElements() {
    // Render matrix elements
    // Placeholder implementation
}

void ConnectionMatrixVisualizer::renderSelection() {
    // Render selected elements
    // Placeholder implementation
}

void ConnectionMatrixVisualizer::renderStatistics() {
    // Render statistics
    // Placeholder implementation
}

void ConnectionMatrixVisualizer::renderLegend() {
    // Render legend
    // Placeholder implementation
}

void ConnectionMatrixVisualizer::renderDebugInfo() {
    // Render debug information
    std::string debugText = 
        "Matrix Size: " + std::to_string(pImpl->matrixSize) + "x" + std::to_string(pImpl->matrixSize) + "\n" +
        "Total Connections: " + std::to_string(pImpl->totalConnections) + "\n" +
        "Matrix Density: " + std::to_string(pImpl->matrixAnalyzer.density) + "\n" +
        "Mean Weight: " + std::to_string(pImpl->meanWeight) + "\n" +
        "Max Weight: " + std::to_string(pImpl->maxWeight) + "\n" +
        "Min Weight: " + std::to_string(pImpl->minWeight) + "\n" +
        "Zoom: " + std::to_string(pImpl->zoomLevel);
    
    // Note: Would need proper text rendering implementation
}

} // namespace nlm
