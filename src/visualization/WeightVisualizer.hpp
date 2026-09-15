#pragma once

namespace nlm {

class VisualizationEngine;

class WeightVisualizer {
public:
    WeightVisualizer();
    ~WeightVisualizer();
    
    // Initialize the visualizer
    bool initialize(VisualizationEngine* engine);
    
    // Update weight matrix visualization data
    void update(const class Brain& brain, double timestep);
    
    // Render weight matrix
    void render();
    
    // Close and cleanup
    void close();
    
    // Configuration
    void setShowExcitationInhibition(bool show);
    void setShowRegionalConnectivity(bool show);
    void setWeightScale(float scale);
    void setColorScheme(int scheme);
    void setShowStatistics(bool show);
    void setFilterByType(bool filter);
    void setFilterByRegion(bool filter);
    
    // Matrix configuration
    void setMatrixSize(size_t rows, size_t cols);
    void setShowDiagonal(bool show);
    void setSymmetrize(bool symmetrize);
    
    // Data export
    bool exportWeightData(const std::string& filepath, ExportFormat format);
    
    // Getters
    bool isActive() const;
    
    // Statistics
    struct WeightStats {
        float totalWeight;                       // Sum of all weights
        float averageWeight;                     // Average weight
        float maxWeight;                         // Maximum weight
        float minWeight;                         // Minimum weight
        float excitationInhibitionRatio;         // E/I balance
        float totalExcitatory;                   // Sum of excitatory weights
        float totalInhibitory;                   // Sum of inhibitory weights
        size_t strongConnections;                // Connections above threshold
        float meanDegree;                        // Average number of connections per neuron
        float connectionDensity;                 // Fraction of possible connections
        
        // Regional connectivity
        std::map<std::pair<int, int>, float> regionConnectivity; // Weight between regions
        std::vector<float> regionalWeights;      // Average weight per region
        
        // Type-specific statistics
        std::map<std::pair<NeuronType, NeuronType>, float> typeConnectivity; // Weights by type
        std::map<NeuronType, float> typeTotalInput;   // Total input to each type
        std::map<NeuronType, float> typeTotalOutput;  // Total output from each type
    };
    
    WeightStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
