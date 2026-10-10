#pragma once

#include "Brain.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <functional>
#include <algorithm>

namespace nlm {

class NetworkBuilder {
public:
    NetworkBuilder(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger);
    ~NetworkBuilder();
    
    // Disable copying, enable moving
    NetworkBuilder(const NetworkBuilder&) = delete;
    NetworkBuilder& operator=(const NetworkBuilder&) = delete;
    NetworkBuilder(NetworkBuilder&&) noexcept = default;
    NetworkBuilder& operator=(NetworkBuilder&&) noexcept = default;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Neural network construction API
    struct NeuralNetworkConfig {
        std::string name;
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        float initialWeightRange[2]; // min, max
        float learningRate;
        float plasticityRate;
        bool enableSTDP;
        bool enableHebbian;
        bool enableStructuralPlasticity;
        bool enableNeuromodulation;
        bool enableDevelopment;
        
        // Regional configuration
        struct RegionConfig {
            std::string name;
            size_t neuronCount;
            NeuronType neuronType;
            float excitationInhibitionRatio;
            float baselineFiringRate;
            float inputScale;
            float outputScale;
            std::vector<size_t> connectedRegions; // region indices
            float interRegionConnectionProbability;
            
            RegionConfig() : neuronCount(0), neuronType(NeuronType::Excitatory),
                           excitationInhibitionRatio(1.0f), baselineFiringRate(0.1f),
                           inputScale(1.0f), outputScale(1.0f),
                           interRegionConnectionProbability(0.1f) {}
        };
        
        std::vector<RegionConfig> regionConfigs;
        
        // Connection configuration
        struct ConnectionConfig {
            std::string name;
            size_t sourceNeuronIndex;
            size_t targetNeuronIndex;
            float weight;
            float delay;
            PlasticityFlags plasticityFlags;
            float plasticityRate;
            bool isDirected;
            
            ConnectionConfig() : weight(0.0f), delay(1), plasticityRate(0.01f), isDirected(true) {}
        };
        
        std::vector<ConnectionConfig> connectionConfigs;
        
        // Initialization configuration
        struct InitializationConfig {
            std::string method; // "random", "structured", "heterogeneous"
            float meanWeight;
            float weightStdDev;
            float minWeight;
            float maxWeight;
            float baselineActivity;
            float activityNoise;
            float inputCurrentRange[2]; // min, max
            float thresholdRange[2]; // min, max
            bool enableRandomSeed;
            uint64_t randomSeed;
            
            InitializationConfig()
                : method("random"), meanWeight(0.0f), weightStdDev(0.1f),
                  minWeight(-0.5f), maxWeight(0.5f), baselineActivity(0.0f),
                  activityNoise(0.01f), inputCurrentRange{-1.0f, 1.0f},
                  thresholdRange{0.2f, 0.8f}, enableRandomSeed(false),
                  randomSeed(42) {}
        };
        
        InitializationConfig initialization;
        
        // Topology configuration
        struct TopologyConfig {
            std::string type; // "fully_connected", "random", "clustered", "modular"
            size_t moduleCount;
            float intraModuleConnectionProbability;
            float interModuleConnectionProbability;
            bool smallWorld;
            float smallWorldLambda;
            bool scaleFree;
            float powerLawExponent;
            
            TopologyConfig()
                : type("random"), moduleCount(1), intraModuleConnectionProbability(0.3f),
                  interModuleConnectionProbability(0.05f), smallWorld(false),
                  smallWorldLambda(0.1f), scaleFree(false), powerLawExponent(2.0f) {}
        };
        
        TopologyConfig topology;
        
        // Plasticity configuration
        struct PlasticityConfig {
            struct STDPConfig {
                float learningRate;
                float traceDecay;
                float LTPThreshold;
                float LTDThreshold;
                float timeWindow;
                float weightChangeMax;
                
                STDPConfig() : learningRate(0.01f), traceDecay(0.1f), LTPThreshold(0.0f),
                             LTDThreshold(0.0f), timeWindow(0.01f), weightChangeMax(0.1f) {}
            };
            
            struct HebbianConfig {
                float learningRate;
                float similarityThreshold;
                float maxWeightChange;
                float coincidenceWindow;
                
                HebbianConfig() : learningRate(0.01f), similarityThreshold(0.5f),
                                maxWeightChange(0.1f), coincidenceWindow(0.01f) {}
            };
            
            struct StructuralConfig {
                float synaptogenesisRate;
                float pruningRate;
                float maxSynapsesPerNeuron;
                float connectionRebalancingRate;
                
                StructuralConfig() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
                                  maxSynapsesPerNeuron(1000), connectionRebalancingRate(0.01f) {}
            };
            
            STDPConfig stdp;
            HebbianConfig hebbian;
            StructuralConfig structural;
        };
        
        PlasticityConfig plasticity;
        
        // Neuromodulation configuration
        struct NeuromodulationConfig {
            bool enableDopamine;
            bool enableAcetylcholine;
            bool enableNorepinephrine;
            bool enableSerotonin;
            bool enableCuriosity;
            bool enableNovelty;
            bool enablePredictionError;
            
            float dopamineBaseline;
            float acetylcholineBaseline;
            float norepinephrineBaseline;
            float serotoninBaseline;
            float curiosityBaseline;
            float noveltyBaseline;
            float predictionErrorBaseline;
            
            // Neuromodulator dynamics
            float dopamineTimeConstant;
            float acetylcholineTimeConstant;
            float norepinephrineTimeConstant;
            float serotoninTimeConstant;
            
            NeuromodulationConfig()
                : enableDopamine(true), enableAcetylcholine(true), enableNorepinephrine(true),
                  enableSerotonin(true), enableCuriosity(true), enableNovelty(true),
                  enablePredictionError(true), dopamineBaseline(0.1f),
                  acetylcholineBaseline(0.1f), norepinephrineBaseline(0.1f),
                  serotoninBaseline(0.1f), curiosityBaseline(0.1f), noveltyBaseline(0.1f),
                  predictionErrorBaseline(0.0f),
                  dopamineTimeConstant(0.1f), acetylcholineTimeConstant(0.1f),
                  norepinephrineTimeConstant(0.1f), serotoninTimeConstant(0.1f) {}
        };
        
        NeuromodulationConfig neuromodulation;
        
        // Development configuration
        struct DevelopmentConfig {
            bool enableDevelopment;
            DevelopmentalStage initialStage;
            double developmentDuration;
            float initialPlasticity;
            float finalPlasticity;
            float plasticityDecayRate;
            
            DevelopmentConfig()
                : enableDevelopment(true), initialStage(DevelopmentalStage::Initial),
                  developmentDuration(100.0), initialPlasticity(1.0f), finalPlasticity(0.2f),
                  plasticityDecayRate(0.001f) {}
        };
        
        DevelopmentConfig development;
        
        // Simulation configuration
        struct SimulationConfig {
            double duration;
            double timestep;
            size_t maxSteps;
            bool enableRealTimeUpdates;
            bool enableLogging;
            std::string dataOutputFile;
            
            SimulationConfig()
                : duration(10.0), timestep(0.01), maxSteps(1000),
                  enableRealTimeUpdates(true), enableLogging(true),
                  dataOutputFile("network_simulation_data.csv") {}
        };
        
        SimulationConfig simulation;
        
        // Performance configuration
        struct PerformanceConfig {
            size_t maxMemoryUsage;
            double maxCPUUsage;
            bool enablePerformanceMonitoring;
            std::string performanceOutputFile;
            
            PerformanceConfig()
                : maxMemoryUsage(1024 * 1024 * 1024), maxCPUUsage(0.8),
                  enablePerformanceMonitoring(true), performanceOutputFile("performance_metrics.csv") {}
        };
        
        PerformanceConfig performance;
        
        NetworkNetworkConfig() = default;
    };
    
    // Main network construction API
    bool createCustomNetwork(const NeuralNetworkConfig& config);
    bool createStandardNetwork(const std::string& topologyType = "random");
    bool createModularNetwork(size_t moduleCount, float intraModuleConnProb, float interModuleConnProb);
    bool createClusteredNetwork(size_t clusters, float intraClusterProb, float interClusterProb);
    bool createSmallWorldNetwork(float lambda);
    bool createScaleFreeNetwork(float exponent);
    
    // Region and neuron management
    RegionId addRegion(const std::string& name, NeuronType type = NeuronType::Excitatory);
    bool addPopulationToRegion(RegionId regionId, NeuronType type, size_t neuronCount);
    bool addNeuronsToRegion(RegionId regionId, NeuronType type, size_t neuronCount);
    
    // Connection management
    bool addConnection(NeuronId source, NeuronId target, float weight = 0.0f, float delay = 1);
    bool addDirectedConnection(NeuronId source, NeuronId target, float weight = 0.0f, float delay = 1);
    bool addRandomConnections(float probability, float weightMean = 0.0f, float weightStdDev = 0.1f);
    bool addClusteredConnections(size_t clusters, float intraClusterProb, float interClusterProb);
    bool addModularConnections(size_t modules, float intraModuleProb, float interModuleProb);
    
    // Network optimization
    bool optimizeNetwork();
    bool rebalanceConnections();
    bool pruneWeakConnections(float threshold);
    bool pruneRedundantConnections(float redundancyThreshold);
    bool enhanceConnectivity(float targetDensity);
    
    // Network analysis
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getRegionCount() const;
    float getConnectionDensity() const;
    float getAverageNeuronDegree() const;
    float getAverageSynapseWeight() const;
    float calculateNetworkClusteringCoefficient() const;
    float calculateNetworkPathLength() const;
    float calculateNetworkSmallWorldness() const;
    
    // Network topology analysis
    bool analyzeNetworkTopology();
    std::vector<size_t> getComponentSizes() const;
    size_t getLargestComponentSize() const;
    float getNetworkDiameter() const;
    float getNetworkRadius() const;
    
    // Network properties
    struct NetworkProperties {
        float averageDegree;
        float standardDeviationDegree;
        float clusteringCoefficient;
        float pathLength;
        float characteristicPathLength;
        float smallWorldness;
        float modularity;
        float density;
        float betweennessCentrality;
        float closenessCentrality;
        
        NetworkProperties()
            : averageDegree(0.0f), standardDeviationDegree(0.0f), clusteringCoefficient(0.0f),
              pathLength(0.0f), characteristicPathLength(0.0f), smallWorldness(1.0f),
              modularity(0.0f), density(0.0f), betweennessCentrality(0.0f),
              closenessCentrality(0.0f) {}
    };
    
    NetworkProperties getNetworkProperties() const;
    
    // Network validation
    bool validateNetworkStructure() const;
    bool validateConnectivity() const;
    bool validateNeuronCounts() const;
    bool validateSynapseCounts() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Network export/import
    bool exportNetworkToJSON(const std::string& filename) const;
    bool importNetworkFromJSON(const std::string& filename);
    bool exportNetworkToBinary(const std::string& filename) const;
    bool importNetworkFromBinary(const std::string& filename);
    
    // Network simulation
    bool runNetworkSimulation(double duration, double timestep = 0.01);
    bool runNetworkStep();
    bool pauseNetworkSimulation();
    bool resumeNetworkSimulation();
    bool stopNetworkSimulation();
    bool isNetworkSimulationRunning() const;
    
    // Network exploration
    bool exploreNetwork();
    void printNetworkStructure() const;
    void printRegionInfo() const;
    void printConnectionInfo() const;
    void generateNetworkReport(const std::string& filename = "network_report.txt");
    
    // Configuration management
    void setNetworkName(const std::string& name) { currentNetworkName_ = name; }
    std::string getNetworkName() const { return currentNetworkName_; }
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const { return randomSeed_; }
    void enableNetworkLogging(bool enable) { loggingEnabled_ = enable; }
    bool isNetworkLoggingEnabled() const { return loggingEnabled_; }
    
    // Statistics and monitoring
    struct NetworkStatistics {
        double buildTime;
        double simulationTime;
        size_t neuronsCreated;
        size_t synapsesCreated;
        float connectionDensity;
        float averageFiringRate;
        float totalActivity;
        size_t memoryUsage;
        double cpuUsage;
        
        NetworkStatistics() : buildTime(0.0), simulationTime(0.0), neuronsCreated(0),
                             synapsesCreated(0), connectionDensity(0.0f),
                             averageFiringRate(0.0f), totalActivity(0.0f),
                             memoryUsage(0), cpuUsage(0.0) {}
    };
    
    NetworkStatistics getNetworkStatistics() const { return statistics_; }
    
    // Thread safety
    void lockNetworkBuilder() const;
    void unlockNetworkBuilder() const;
    bool tryLockNetworkBuilder() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainDebugger> debugger_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex builderMutex_;
    std::atomic<bool> isInitialized_;
    std::atomic<bool> isBuilding_;
    std::atomic<bool> isSimulating_;
    
    // Network configuration and state
    std::string currentNetworkName_;
    uint64_t randomSeed_;
    bool loggingEnabled_;
    
    // Network statistics
    NetworkStatistics statistics_;
    double buildStartTime_;
    double buildEndTime_;
    double simulationStartTime_;
    double simulationEndTime_;
    
    // Network components
    std::vector<std::string> createdRegions_;
    std::vector<size_t> regionNeuronCounts_;
    std::vector<NeuronType> regionNeuronTypes_;
    std::vector<float> regionFiringRates_;
    
    // Internal helper methods
    void cleanup();
    bool initializeRandomGenerator();
    bool validateNetworkConfig(const NeuralNetworkConfig& config) const;
    bool setupRegions(const NeuralNetworkConfig& config);
    bool setupConnections(const NeuralNetworkConfig& config);
    bool initializeNeurons(const NeuralNetworkConfig& config);
    bool initializeConnections(const NeuralNetworkConfig& config);
    bool applyPlasticityConfiguration(const NeuralNetworkConfig& config);
    bool applyNeuromodulationConfiguration(const NeuralNetworkConfig& config);
    bool applyDevelopmentConfiguration(const NeuralNetworkConfig& config);
    bool applySimulationConfiguration(const NeuralNetworkConfig& config);
    bool applyPerformanceConfiguration(const NeuralNetworkConfig& config);
    void generateNetworkTopology(const NeuralNetworkConfig& config);
    void generateRandomTopology(size_t totalNeurons, float connectionProbability);
    void generateClusteredTopology(size_t totalNeurons, size_t clusters, 
                                  float intraClusterProb, float interClusterProb);
    void generateModularTopology(size_t totalNeurons, size_t modules,
                               float intraModuleProb, float interModuleProb);
    void generateSmallWorldTopology(size_t totalNeurons, float lambda);
    void generateScaleFreeTopology(size_t totalNeurons, float exponent);
    void setupRegionBasedConnections(const NeuralNetworkConfig& config);
    void setupInterRegionConnections(const NeuralNetworkConfig& config);
    float calculateIntraRegionConnectionProbability(const NeuralNetworkConfig::RegionConfig& region,
                                                   const NeuralNetworkConfig& config) const;
    float calculateInterRegionConnectionProbability(const NeuralNetworkConfig::RegionConfig& sourceRegion,
                                                   const NeuralNetworkConfig::RegionConfig& targetRegion,
                                                   const NeuralNetworkConfig& config) const;
    void setupPlasticitySystems(const NeuralNetworkConfig& config);
    void setupNeuromodulationSystems(const NeuralNetworkConfig& config);
    void setupDevelopmentSystem(const NeuralNetworkConfig& config);
    void setupSimulationSystems(const NeuralNetworkConfig& config);
    void setupPerformanceMonitoring(const NeuralNetworkConfig& config);
    bool applyConnectionConfig(const NeuralNetworkConfig::ConnectionConfig& conn,
                             const NeuralNetworkConfig& config);
    bool createConnection(const NeuralNetworkConfig::ConnectionConfig& conn,
                        const NeuralNetworkConfig& config);
    void optimizeNetworkStructure();
    void rebalanceConnectionWeights();
    void calculateNetworkStatistics();
    void logNetworkBuild(const std::string& message);
    void logNetworkSimulation(const std::string& message);
    bool exportNetworkToJSONInternal(const std::string& filename) const;
    bool importNetworkFromJSONInternal(const std::string& filename);
    bool exportNetworkToBinaryInternal(const std::string& filename) const;
    bool importNetworkFromBinaryInternal(const std::string& filename);
    void generateNetworkTopologyReport() const;
    void generateConnectionReport() const;
    void generatePerformanceReport() const;
    bool validateNetworkInternal() const;
    bool validateNeuronInternal() const;
    bool validateSynapseInternal() const;
};

} // namespace nlm