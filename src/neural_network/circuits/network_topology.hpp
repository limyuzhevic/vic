#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

/**
 * Neural Circuit Architecture
 * 
 * Topological organization of neural circuits:
 * - Grid topology (regular 2D/3D arrangement)
 * - Map topology (functional mapping)
 * - Hierarchical topology (multiple levels)
 * - Small-world networks
 * - Scale-free networks
 * - Modular networks
 * - Random networks
 * - Lattice networks
 * 
 * Connection patterns:
 * - Local connectivity (within regions)
 * - Global connectivity (between regions)
 * - Distance-dependent connectivity
 * - Activity-dependent connectivity
 * - Topographically organized connectivity
 */
class NeuralCircuitTopology {
public:
    NeuralCircuitTopology();
    ~NeuralCircuitTopology();

    // ===== TOPOLOGY TYPES =====

    // Topology types
    enum class TopologyType {
        GRID,              // Regular grid (2D or 3D)
        MAP,               // Functional map
        HIERARCHICAL,      // Multi-level hierarchy
        SMALL_WORLD,       // Small-world network
        SCALE_FREE,        // Scale-free network
        MODULAR,           // Modular network
        RANDOM,            // Random network
        LATTICE,           // Lattice network
        RING,              // Ring network
        STAR,              // Star network
    };

    // ===== GRID TOPOLOGY =====

    // Grid topology parameters
    struct GridTopology {
        // Grid dimensions
        size_t rows;                // Number of rows
        size_t cols;                // Number of columns
        size_t depth;               // Number of depth levels (3D)
        float spacing;              // Spacing between neurons (μm)
        
        // Grid organization
        bool wrap;                  // Whether grid wraps around edges
        float edgeDecay;            // Signal decay at edges
        float boundaryAttraction;   // Boundary effect on connectivity
        
        // Grid-specific connectivity
        float localConnectionRadius; // Radius for local connectivity (μm)
        float localConnectionProbability; // Probability of local connections
        float longRangeConnectionProbability; // Probability of long-range connections
    };

    void setGridTopology(const GridTopology& params) { 
        gridTopology_ = params; 
    }

    // ===== MAP TOPOLOGY =====

    // Map topology parameters
    struct MapTopology {
        // Map organization
        float mapScale;             // Map scale factor
        float topographicPrecision; // Precision of topographic mapping
        float mapContinuity;        // Continuity in map representation
        float mapSmoothness;        // Smoothness of map transitions
        
        // Map connectivity
        float localConnectivity;    // Local connectivity strength
        float globalConnectivity;   // Global connectivity strength
        float crossMapConnectivity;  // Connectivity between maps
        
        // Map types
        enum class MapType {
            SENSORY,           // Sensory map
            MOTOR,             // Motor map
            ASSOCIATION,       // Association map
            HIGHER_ORDER,      // Higher-order map
        };

        MapType mapType;            // Type of map
    };

    void setMapTopology(const MapTopology& params) { 
        mapTopology_ = params; 
    }

    // ===== HIERARCHICAL TOPOLOGY =====

    // Hierarchical topology parameters
    struct HierarchicalTopology {
        // Hierarchy levels
        size_t levels;              // Number of hierarchy levels
        float levelDecay;            // Decay factor between levels
        float levelConnectionStrength; // Connection strength between levels
        
        // Hierarchical organization
        bool bidirectional;          // Whether connections are bidirectional
        float feedbackStrength;      // Feedback connection strength
        float feedforwardStrength;   // Feedforward connection strength
        
        // Hierarchical specific parameters
        float topLevelSize;          // Size of top level
        float bottomLevelSize;       // Size of bottom level
        float levelExpansionRatio;   // Expansion ratio between levels
    };

    void setHierarchicalTopology(const HierarchicalTopology& params) { 
        hierarchicalTopology_ = params; 
    }

    // ===== SMALL-WORLD TOPOLOGY =====

    // Small-world topology parameters
    struct SmallWorldTopology {
        // Network parameters
        size_t neurons;              // Number of neurons
        size_t neighbors;            // Number of nearest neighbors
        float reconnectionProbability; // Probability of long-range connections
        float clusteringCoefficient;  // Desired clustering coefficient
        
        // Small-world specific parameters
        float gamma;                 // Range parameter for local connections
        float beta;                  // Range parameter for long-range connections
        float p;                     // Rewiring probability
        
        // Structural properties
        float smallWorldStrength;    // Strength of small-world property
        float clusteringDecay;       // Clustering decay rate
        float pathLengthDecay;       // Path length decay rate
    };

    void setSmallWorldTopology(const SmallWorldTopology& params) { 
        smallWorldTopology_ = params; 
    }

    // ===== SCALE-FREE TOPOLOGY =====

    // Scale-free topology parameters
    struct ScaleFreeTopology {
        // Power-law parameters
        float powerLawExponent;      // Power-law exponent (typically 2-3)
        size_t minDegree;            // Minimum degree
        size_t maxDegree;            // Maximum degree
        
        // Generation parameters
        float attachmentProbability; // Probability of preferential attachment
        float growthRate;             // Network growth rate
        float removalRate;            // Node removal rate
        
        // Scale-free specific properties
        float degreeDistribution;    // Degree distribution measure
        float clusteringScaleFree;   // Clustering coefficient
        float pathLengthScaleFree;   // Average path length
    };

    void setScaleFreeTopology(const ScaleFreeTopology& params) { 
        scaleFreeTopology_ = params; 
    }

    // ===== MODULAR TOPOLOGY =====

    // Modular topology parameters
    struct ModularTopology {
        // Module organization
        size_t modules;              // Number of modules
        float intraModuleConnectionDensity; // Intra-module connection density
        float interModuleConnectionDensity; // Inter-module connection density
        float moduleSizeVariation;   // Variation in module sizes
        
        // Module-specific parameters
        float modularityCoefficient; // Modularity coefficient (0-1)
        float moduleInterconnection; // Inter-module interconnection strength
        float moduleIsolation;       // Module isolation factor
        
        // Module types
        enum class ModuleType {
            SENSORY,           // Sensory module
            MOTOR,             // Motor module
            ASSOCIATION,       // Association module
            DECISION,          // Decision module
            COGNITIVE,         // Cognitive module
        };

        std::vector<ModuleType> moduleTypes; // Type of each module
    };

    void setModularTopology(const ModularTopology& params) { 
        modularTopology_ = params; 
    }

    // ===== CONNECTION PATTERNS =====

    // Connection patterns
    enum class ConnectionPattern {
        LOCAL_ONLY,          // Only local connections
        GLOBAL_ONLY,         // Only global connections
        LOCAL_GLOBAL,        // Both local and global connections
        DISTANCE_DEPENDENT,  // Distance-dependent connectivity
        ACTIVITY_DEPENDENT,  // Activity-dependent connectivity
        TOPOGRAPHIC,        // Topographically organized
        STATE_DEPENDENT,    // State-dependent connectivity
    };

    // Distance-dependent connectivity parameters
    struct DistanceDependentConnectivity {
        float decayConstant;    // Decay constant for distance dependence
        float minimumDistance;  // Minimum distance for connection
        float maximumDistance;  // Maximum distance for connection
        float connectionStrength; // Base connection strength
        float distanceExponent;  // Exponent for distance dependence
    };

    void setDistanceDependentConnectivity(const DistanceDependentConnectivity& params) { 
        distanceDependentConn_ = params; 
    }

    // Activity-dependent connectivity parameters
    struct ActivityDependentConnectivity {
        float activityThreshold; // Activity threshold for connectivity
        float connectionLearningRate; // Learning rate for activity-dependent connectivity
        float activityMemory;     // Memory of past activities
        float temporalWindow;     // Temporal window for activity dependence
    };

    void setActivityDependentConnectivity(const ActivityDependentConnectivity& params) { 
        activityDependentConn_ = params; 
    }

    // ===== NETWORK MOTIFS =====

    // Neural circuit network motifs
    enum class NetworkMotif {
        FEEDBACK_LOOP,            // Feedback loop
        FEEDFORWARD_LOOP,         // Feedforward loop
        INHIBITORY_FEEDBACK,      // Inhibitory feedback
        EXCITATORY_INHIBITION,    // Excitatory-inhibitory pair
        RECEPTIVE_FIELD,          // Receptive field organization
        LATERAL_INHIBITION,       // Lateral inhibition
        PARIETAL_FRONTAL,         // Parietal-frontal connection
        CEREBELLAR_VOLTAGE,       // Cerebellar voltage control
        NOISE_REDUCTION,          // Noise reduction motif
        TEMPORAL_DECODING,        // Temporal decoding motif
        SPATIAL_DECODING,         // Spatial decoding motif
        CONCEPT_FORMATION,        // Concept formation motif
        ATTENTIONAL_SELECTION,    // Attentional selection motif
        MEMORY_CONSOLIDATION,      // Memory consolidation motif
    };

    // Enable/disable network motifs
    void enableNetworkMotif(NetworkMotif motif, bool enable);
    bool isNetworkMotifEnabled(NetworkMotif motif) const;

    // Network motif parameters
    struct NetworkMotifParams {
        float motifStrength;       // Strength of motif influence
        float motifGain;           // Motif gain factor
        float motifTimeConstant;   // Motif time constant
        float motifThreshold;      // Motif activation threshold
    };

    void setNetworkMotifParams(NetworkMotif motif, const NetworkMotifParams& params);

    // ===== NETWORK INITIALIZATION STRATEGIES =====

    // Network initialization strategies
    enum class InitializationStrategy {
        UNIFORM_RANDOM,           // Uniform random initialization
        CLUSTERED,                // Clustered initialization
        GRADIENT,                 // Gradient-based initialization
        ACTIVITY_DRIVEN,          // Activity-driven initialization
        STRUCTURED,               // Structured initialization
        HIERARCHICAL,             // Hierarchical initialization
        SPATIOTEMPORAL,           // Spatiotemporal initialization
        ADAPTIVE,                 // Adaptive initialization
        DYNAMIC,                  // Dynamic initialization
    };

    // Initialization strategy parameters
    struct InitializationStrategyParams {
        float initializationStrength;   // Initialization strength
        float initializationRate;       // Initialization rate
        float initializationTemperature; // Temperature for initialization
        float initializationBias;       // Bias in initialization
    };

    void setInitializationStrategy(InitializationStrategy strategy, 
                                   const InitializationStrategyParams& params);

    // ===== UTILITY =====

    // Get topology type
    TopologyType getTopologyType() const { return topologyType_; }
    void setTopologyType(TopologyType type) { topologyType_ = type; }

    // Get connection pattern
    ConnectionPattern getConnectionPattern() const { return connectionPattern_; }
    void setConnectionPattern(ConnectionPattern pattern) { connectionPattern_ = pattern; }

    // Calculate network metrics
    struct NetworkMetrics {
        float averageDegree;           // Average degree of neurons
        float clusteringCoefficient;   // Clustering coefficient
        float characteristicPathLength; // Characteristic path length
        float smallWorldLambda;        // Small-worldness parameter
        float modularityQ;             // Modularity coefficient
        float participationRatio;      // Participation ratio
        float efficiency;              // Network efficiency
        float robustness;              // Network robustness
        float synchronizability;       // Synchronizability
    };

    NetworkMetrics calculateNetworkMetrics() const;

    // Get neurons in topological neighborhood
    std::vector<NeuronId> getTopologicalNeighbors(NeuronId neuronId, 
                                               float radius = 1.0f) const;

    // Calculate distance between neurons in topology
    float calculateTopologicalDistance(NeuronId neuron1Id, NeuronId neuron2Id) const;

    // ===== OVERRIDE =====

    // Override Brain for brain integration
    class IntegratedNeuralCircuit : public Brain {
    public:
        IntegratedNeuralCircuit(std::shared_ptr<Config> config,
                               std::shared_ptr<NeuralCircuitTopology> topology);
        ~IntegratedNeuralCircuit() override;

        // Initialize circuit with topology
        bool initialize() override;

        // Apply topology to brain
        void applyTopology();

        // Update circuit with topology
        void updateCircuitTopology(TimestepDuration dt);

        // Get circuit metrics
        NetworkMetrics getCircuitMetrics() const;

    private:
        std::shared_ptr<NeuralCircuitTopology> topology_;
        NetworkMetrics currentMetrics_;
    };

private:
    // Topology type
    TopologyType topologyType_;
    ConnectionPattern connectionPattern_;

    // Topology-specific parameters
    GridTopology gridTopology_;
    MapTopology mapTopology_;
    HierarchicalTopology hierarchicalTopology_;
    SmallWorldTopology smallWorldTopology_;
    ScaleFreeTopology scaleFreeTopology_;
    ModularTopology modularTopology_;
    DistanceDependentConnectivity distanceDependentConn_;
    ActivityDependentConnectivity activityDependentConn_;

    // Network motifs
    std::map<NetworkMotif, bool> enabledMotifs_;
    std::map<NetworkMotif, NetworkMotifParams> motifParams_;

    // Initialization strategies
    std::map<InitializationStrategy, InitializationStrategyParams> initStrategyParams_;
};

} // namespace nlm
