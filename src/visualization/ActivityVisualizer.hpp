#pragma once

namespace nlm {

class VisualizationEngine;

class ActivityVisualizer {
public:
    ActivityVisualizer();
    ~ActivityVisualizer();
    
    // Initialize the visualizer
    bool initialize(VisualizationEngine* engine);
    
    // Update activity visualization data
    void update(const class Brain& brain, double timestep);
    
    // Render activity visualization
    void render();
    
    // Close and cleanup
    void close();
    
    // Configuration
    void setShowMembranePotential(bool show);
    void setShowFiringRates(bool show);
    void setShowHistograms(bool show);
    void setShowHeatmaps(bool show);
    void setTimeWindow(double seconds);
    void setUpdateInterval(double interval);
    void setColorScheme(int scheme);
    
    // Time series configuration
    void setTimeSeriesLength(double seconds);
    void setYRange(float min, float max);
    
    // Population configuration
    void setShowPopulationActivity(bool show);
    void setPopulationAlpha(float alpha);
    void setPopulationLabels(bool show);
    
    // Data export
    bool exportActivityData(const std::string& filepath, ExportFormat format);
    
    // Getters
    bool isActive() const;
    
    // Statistics
    struct ActivityStats {
        double currentTime;                    // Current simulation time (s)
        double simulationDuration;             // Total simulation time (s)
        double averageFiringRate;             // Average firing rate (Hz)
        double maxFiringRate;                  // Maximum firing rate (Hz)
        size_t totalSpikes;                    // Total spikes in time window
        float averageMembranePotential;        // Average membrane potential (mV)
        float averageAdaptationVariable;       // Average adaptation variable
        std::vector<double> firingRateHistory; // Time series of average firing rates
        std::vector<float> membranePotentialHistory; // Time series of average membrane potentials
        std::map<NeuronType, double> typeFiringRates; // Firing rates by neuron type
    };
    
    ActivityStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
