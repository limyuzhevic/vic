#pragma once

namespace nlm {

class VisualizationEngine;

class DevelopmentVisualizer {
public:
    DevelopmentVisualizer();
    ~DevelopmentVisualizer();
    
    // Initialize the visualizer
    bool initialize(VisualizationEngine* engine);
    
    // Update development visualization data
    void update(const class Brain& brain, double timestep);
    
    // Render development visualization
    void render();
    
    // Close and cleanup
    void close();
    
    // Configuration
    void setShowDevelopmentStage(bool show);
    void setShowMaturation(bool show);
    void setShowPruning(bool show);
    void setShowSynaptogenesis(bool show);
    void setShowPlasticity(bool show);
    void setColorByStage(bool color);
    void setTimeScale(float scale);
    
    // Stage-specific visualization
    void setShowStageMarkers(bool show);
    void setShowCriticalPeriod(bool show);
    void setShowMaturationProgress(bool show);
    
    // Data export
    bool exportDevelopmentData(const std::string& filepath, ExportFormat format);
    
    // Getters
    bool isActive() const;
    
    // Statistics
    struct DevelopmentStats {
        DevelopmentalStage currentStage;           // Current developmental stage
        float stageProgress;                       // Progress through current stage (0-1)
        size_t neuronsCreated;                     // Total neurons created during development
        size_t synapsesCreated;                     // Total synapses created
        size_t synapsesEliminated;                  // Total synapses eliminated (pruning)
        float totalPlasticityChange;               // Total weight change due to plasticity
        float synaptogenesisRate;                   // Current synaptogenesis rate
        float pruningRate;                         // Current pruning rate
        std::vector<float> stageProgression;        // Progress through stages over time
        std::vector<float> plasticityHistory;      // Plasticity changes over time
        std::vector<float> connectivityHistory;    // Connectivity over time
    };
    
    DevelopmentStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
