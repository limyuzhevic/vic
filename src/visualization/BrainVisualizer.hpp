#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"

namespace nlm {

// Visualization data structures
struct NeuronVisualizationData {
    NeuronId id;
    float x, y, z;           // Position in 3D space
    float membranePotential;  // Current membrane potential
    float threshold;          // Spike threshold
    bool isActive;            // Currently active (firing)
    bool isSynapsing;         // Currently in synaptic transmission
    float lastSpikeTime;      // Time of last spike
    int firingRate;           // Recent firing rate
    NeuronType type;          // Neuron type (excitatory/inhibitory/sensory/motor)
    RegionId regionId;        // Which brain region this neuron belongs to
};

struct SynapseVisualizationData {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId targetNeuron;
    float weight;             // Synaptic strength
    float delay;              // Transmission delay
    float lastTransmission;    // Time of last transmission
    bool isActive;            // Currently transmitting
    float toxicity;           // STDP toxicity
    bool hebbianActive;       // Currently using Hebbian plasticity
    bool stdpActive;          // Currently using STDP plasticity
};

struct MemoryVisualizationData {
    MemoryId id;
    std::string type;         // working, episodic, associative
    std::vector<MemoryTrace> traces;
    std::vector<MemoryPattern> patterns;
    float activationLevel;     // How active this memory system is
    float strength;            // System strength (1.0 = full strength)
};

struct NeuromodulationVisualizationData {
    std::string type;         // dopamine, curiosity, novelty, prediction_error
    float concentration;       // Current level
    float effect;             // Current effect on neural activity
    float history[10];        // Recent history for visualization
    float targetLevel;        // Target level for homeostasis
    bool isModulating;         // Currently modulating neurons
};

struct CognitiveVisualizationData {
    std::string type;         // prediction, planning, concept, attention
    float activation;          // Current activation level
    float confidence;          // Confidence in predictions/plans
    std::vector<std::string> activeConcepts;
    std::vector<std::string> plannedActions;
    float focusLevel;          // Attention focus
};

struct DevelopmentVisualizationData {
    DevelopmentalStage stage;  // Current developmental stage
    float plasticityRate;      // Current plasticity rate
    float maturationLevel;     // Overall maturation
    float capabilityLevel;     // Current capabilities
    float age;                // Simulated age
    std::vector<float> capabilityHistory;  // History of capability development
};

struct PerformanceMetricsVisualizationData {
    float averageFiringRate;   // Average firing rate across brain
    float spikeCount;          // Total spikes in recent period
    float learningProgress;    // Learning progress metric
    float memoryRetention;     // Memory retention level
    float predictionAccuracy;  // Prediction accuracy
    float explorationRate;     // Exploration vs exploitation balance
    std::vector<float> timeSeriesData;  // Time series for metrics
};

// Base visualizer interface
class BrainVisualizer {
public:
    virtual ~BrainVisualizer() = default;
    
    virtual void initialize() = 0;
    virtual void update(const Brain& brain) = 0;
    virtual void render() = 0;
    virtual void close() = 0;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Configuration
    virtual void setUpdateRate(double hz) = 0;
    virtual double getUpdateRate() const = 0;
    
    // Filter/scope
    virtual void setRegionFilter(std::vector<RegionId> regions) = 0;
    virtual void setNeuronTypeFilter(std::vector<NeuronType> types) = 0;
    virtual void setFocusNeuron(NeuronId neuron) = 0;
    
    // Output options
    virtual void setOutputFormat(const std::string& format) = 0;
    virtual void setDetailLevel(int level) = 0;
    virtual void setColorScheme(const std::string& scheme) = 0;
};

// Factory for creating visualizers
class VisualizationFactory {
public:
    static std::unique_ptr<BrainVisualizer> createNeuralNetworkVisualizer(const std::string& type = "default");
    static std::unique_ptr<BrainVisualizer> createMemoryVisualizer(const std::string& type = "default");
    static std::unique_ptr<BrainVisualizer> createNeuromodulationVisualizer(const std::string& type = "default");
    static std::unique_ptr<BrainVisualizer> createCognitiveVisualizer(const std::string& type = "default");
    static std::unique_ptr<BrainVisualizer> createDevelopmentVisualizer(const std::string& type = "default");
    static std::unique_ptr<BrainVisualizer> createPerformanceVisualizer(const std::string& type = "default");
    
    static std::unique_ptr<BrainVisualizer> createVisualizer(const std::string& name, const std::string& type = "default");
    
    static std::vector<std::string> getAvailableVisualizers();
    static std::vector<std::string> getVisualizerTypes(const std::string& name);
};

} // namespace nlm