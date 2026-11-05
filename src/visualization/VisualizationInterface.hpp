// NLM Visualization Interface
// Phase 6: Visualization and debugging tools for brain simulation

#pragma once

#include <string>
#include <vector>

namespace nlm {

class Brain;

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Show comprehensive brain state visualization
    void showBrainState(const Brain& brain, const std::string& title) const;
    
    // Show simulation step progress
    void showSimulationStep(const Brain& brain, size_t step) const;
    
    // Show synapse activity visualization
    void showSynapseActivity(const Brain& brain, size_t topN = 10) const;
    
    // Show neuromodulation activity
    void showNeuromodulationActivity(const Brain& brain) const;
    
    // Show memory system statistics
    void showMemoryStats(const Brain& brain) const;
    
    // Show performance metrics
    void showPerformanceMetrics(const Brain& brain, double wallClockTime) const;
    
    // Show development system status
    void showDevelopmentStatus(const Brain& brain) const;
    
    // Show checkpoint status
    void showCheckpointStatus(const Brain& brain) const;
};

} // namespace nlm
