#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "<string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cmath>
#include <queue>

namespace nlm {

// Advanced neuron selection strategies
enum class NeuronSelectionStrategy {
    Random,
    DistanceBased,
    ActivityBased,
    TypeBased,
    ConnectivityBased,
    ClusterBased,
    Adaptive
};

// Connectivity pattern types
enum class ConnectivityPattern {
    AllToAll,
    Random,
    SmallWorld,
    ScaleFree,
    Modular,
    Hierarchical,
    Custom
};

// Advanced region management for expert users
class RegionManager {
public:
    // Region creation and management
    static NeuralRegion* createRegion(const std::string& name = "Region",
                                     const std::vector<NeuronType>& neuronTypes = {});
    static bool deleteRegion(NeuralRegion* region);
    static bool renameRegion(NeuralRegion* region, const std::string& newName);
    static std::vector<NeuralRegion*> getAllRegions();
    static NeuralRegion* findRegion(const std::string& name);
    static NeuralRegion* findRegion(RegionId id);
    
    // Neuron management
    static Neuron* addNeuronToRegion(NeuralRegion* region, NeuronType type = NeuronType::Internal);
    static bool removeNeuronFromRegion(NeuralRegion* region, NeuronId neuronId);
    static bool moveNeuronBetweenRegions(NeuronId neuronId, NeuralRegion* targetRegion);
    static bool duplicateNeuron(NeuralRegion* sourceRegion, NeuralRegion* targetRegion,
                               NeuronId neuronId, NeuronType newType = NeuronType::Internal);
    
    // Synapse management
    static Synapse* addSynapseToRegion(NeuralRegion* region, NeuronId sourceId, NeuronId targetId,
                                      SynapticWeight weight = 0.0f, Delay delay = 1,
                                      SynapseType type = SynapseType::Excitatory);
    static bool removeSynapseFromRegion(NeuralRegion* region, SynapseId synapseId);
    static bool updateSynapseProperties(NeuralRegion* region, SynapseId synapseId,
                                        const std::string& property, const std::string& value);
    static bool createReciprocalPair(NeuralRegion* region, NeuronId neuronA, NeuronId neuronB,
                                    SynapticWeight weightA = 0.0f, SynapticWeight weightB = 0.0f);
    
    // Connectivity management
    static bool establishConnection(NeuralRegion* region, NeuronId sourceId, NeuronId targetId,
                                    SynapticWeight weight = 0.0f, Delay delay = 1,
                                    SynapseType type = SynapseType::Excitatory);
    static bool removeConnection(NeuralRegion* region, SynapseId synapseId);
    static bool updateConnectionWeight(NeuralRegion* region, SynapseId synapseId, SynapticWeight weight);
    static bool createNetworkPattern(NeuralRegion* region, ConnectivityPattern pattern,
                                     float connectionProbability = 0.1f, float weightScale = 1.0f);
    static bool optimizeConnectivity(NeuralRegion* region);
    
    // Neuron selection and manipulation
    static std::vector<Neuron*> selectNeurons(NeuralRegion* region, 
                                              size_t count,
                                              NeuronSelectionStrategy strategy = NeuronSelectionStrategy::Random,
                                              const std::vector<NeuronType>& preferredTypes = {});
    static std::vector<Neuron*> selectByDistance(NeuralRegion* region, NeuronId centerNeuron,
                                                 size_t maxDistance, size_t count);
    static std::vector<Neuron*> selectByActivity(NeuralRegion* region, 
                                                 float activityThreshold,
                                                 size_t maxCount = 100,
                                                 bool ascending = true);
    static std::vector<Neuron*> selectByType(NeuralRegion* region, NeuronType type, size_t count);
    static std::vector<Neuron*> selectByConnectivity(NeuralRegion* region,
                                                     size_t minInDegree,
                                                     size_t maxInDegree,
                                                     size_t count);
    static std::vector<std::vector<Neuron*>> clusterNeurons(NeuralRegion* region,
                                                            size_t numClusters,
                                                            const std::string& clusteringMethod = "kmeans");
    
    // Advanced connectivity analysis
    static float calculateClusteringCoefficient(NeuralRegion* region);
    static float calculatePathLength(NeuralRegion* region);
    static float calculateCharacteristicPathLength(NeuralRegion* region);
    static float calculateNetworkDensity(NeuralRegion* region);
    static float calculateBetweennessCentrality(NeuralRegion* region, NeuronId neuronId);
    static float calculateEigenvectorCentrality(NeuralRegion* region, NeuronId neuronId);
    static float calculateAssortativity(NeuralRegion* region);
    static float calculateModularity(NeuralRegion* region, const std::vector<std::vector<Neuron*>>& communities);
    
    // Community detection
    static std::vector<std::vector<Neuron*>> detectCommunities(NeuralRegion* region,
                                                               const std::string& method = "louvain",
                                                               float resolution = 1.0f);
    static std::vector<std::vector<Neuron*>> detectCommunitiesLouvain(NeuralRegion* region,
                                                                       float resolution = 1.0f);
    static std::vector<std::vector<Neuron*>> detectCommunitiesKMeans(NeuralRegion* region,
                                                                     size_t numClusters,
                                                                     const std::vector<Neuron*>& seedNeurons);
    static std::vector<std::vector<Neuron*>> detectCommunitiesInfomap(NeuralRegion* region);
    
    // Region operations
    static bool mergeRegions(NeuralRegion* region1, NeuralRegion* region2);
    static bool splitRegion(NeuralRegion* sourceRegion, RegionId newRegionId,
                            const std::vector<NeuronId>& neuronsToMove);
    static bool duplicateRegion(NeuralRegion* sourceRegion, const std::string& newName);
    static bool partitionRegion(NeuralRegion* region, size_t numPartitions,
                                const std::vector<std::vector<NeuronId>>& partitionNeurons);
    static bool integrateWithExternalRegion(NeuralRegion* region, NeuralRegion* externalRegion,
                                            float connectionProbability = 0.1f);
    
    // Advanced connectivity patterns
    static bool createSmallWorldNetwork(NeuralRegion* region, float rewiringProbability = 0.1f);
    static bool createScaleFreeNetwork(NeuralRegion* region, float alpha = 2.0f);
    static bool createHierarchicalNetwork(NeuralRegion* region, size_t levels = 3);
    static bool createModularNetwork(NeuralRegion* region, size_t numModules,
                                     float intraModuleDensity = 0.3f,
                                     float interModuleDensity = 0.01f);
    static bool createCustomConnectivity(NeuralRegion* region,
                                         const std::vector<std::pair<NeuronId, NeuronId>>& connections,
                                         const std::vector<float>& connectionWeights);
    
    // Network analysis and optimization
    static std::string analyzeNetworkTopology(NeuralRegion* region);
    static float calculateNetworkRobustness(NeuralRegion* region);
    static float calculateNetworkEfficiency(NeuralRegion* region);
    static bool optimizeNetworkForInfoTransmission(NeuralRegion* region);
    static bool optimizeNetworkForSynchronization(NeuralRegion* region);
    static bool optimizeNetworkForRobustness(NeuralRegion* region);
    
    // Export/import for external tools
    static bool exportConnectivityToFile(NeuralRegion* region, const std::string& filepath);
    static bool importConnectivityFromFile(NeuralRegion* region, const std::string& filepath);
    static std::string exportGraphML(NeuralRegion* region);
    static std::string exportGraphViz(NeuralRegion* region);
    static std::string exportJSON(NeuralRegion* region);
    
    // Real-time monitoring
    static void startNetworkMonitoring(NeuralRegion* region);
    static void stopNetworkMonitoring(NeuralRegion* region);
    static std::map<std::string, float> getNetworkMetrics(NeuralRegion* region);
    static float getRealTimeClusteringCoefficient(NeuralRegion* region, size_t windowSize = 100);
    static float getRealTimePathLength(NeuralRegion* region, size_t windowSize = 100);
    static float getRealTimeNetworkDensity(NeuralRegion* region, size_t windowSize = 100);
    
    // Advanced features
    static bool createAttractorNetwork(NeuralRegion* region, size_t attractorSize, float attractorStrength);
    static bool createResonanceNetwork(NeuralRegion* region, float resonanceFrequency, float couplingStrength);
    static bool createHomeostaticNetwork(NeuralRegion* region, float targetFiringRate, float homeostaticGain);
    static bool createPlasticityNetwork(NeuralRegion* region, float learningRate, float timeConstant);
    static bool createMemoryNetwork(NeuralRegion* region, size_t memoryCapacity, float memoryDecay);
};

// Advanced connectivity analyzer
class ConnectivityAnalyzer {
public:
    // Graph algorithms
    static std::vector<std::vector<float>> calculateShortestPaths(NeuralRegion* region);
    static std::vector<std::vector<float>> calculateFloydWarshall(NeuralRegion* region);
    static std::vector<std::vector<float>> calculateFloydWarshallOptimized(NeuralRegion* region);
    static float calculateMinimumSpanningTreeWeight(NeuralRegion* region);
    static std::vector<std::vector<Neuron*>> calculateAllConnectedComponents(NeuralRegion* region);
    static std::vector<std::vector<Neuron*>> calculateStronglyConnectedComponents(NeuralRegion* region);
    
    // Centrality measures
    static std::map<NeuronId, float> calculateDegreeCentrality(NeuralRegion* region);
    static std::map<NeuronId, float> calculateBetweennessCentralityAll(NeuralRegion* region);
    static std::map<NeuronId, float> calculateClosenessCentrality(NeuralRegion* region);
    static std::map<NeuronId, float> calculateEigenvectorCentralityAll(NeuralRegion* region);
    static std::map<NeuronId, float> calculatePageRank(NeuralRegion* region, float dampingFactor = 0.85f);
    
    // Community analysis
    static float calculateModularity(NeuralRegion* region, const std::vector<std::vector<Neuron*>>& communities);
    static float calculateModularityOptimized(NeuralRegion* region, const std::vector<std::vector<Neuron*>>& communities);
    static float calculateNewmanGirvanModularity(NeuralRegion* region, const std::vector<std::vector<Neuron*>>& communities);
    static float calculateLouvainModularity(NeuralRegion* region, const std::vector<std::vector<Neuron*>>& communities);
    
    // Network robustness
    static float calculateNetworkRobustness(NeuralRegion* region);
    static float calculateTargetedAttackRobustness(NeuralRegion* region);
    static float calculateRandomAttackRobustness(NeuralRegion* region);
    static std::vector<float> calculateRobustnessVsAttackType(NeuralRegion* region);
    
    // Efficiency measures
    static float calculateNetworkEfficiency(NeuralRegion* region);
    static float calculateGlobalEfficiency(NeuralRegion* region);
    static float calculateLocalEfficiency(NeuralRegion* region);
    static float calculateCharacteristicPathLength(NeuralRegion* region);
    static float calculateAverageClusteringCoefficient(NeuralRegion* region);
    
    // Community detection algorithms
    static std::vector<std::vector<Neuron*>> louvainAlgorithm(NeuralRegion* region, float resolution = 1.0f);
    static std::vector<std::vector<Neuron*>> infomapAlgorithm(NeuralRegion* region);
    static std::vector<std::vector<Neuron*>> labelPropagationAlgorithm(NeuralRegion* region);
    static std::vector<std::vector<Neuron*>> spectralClustering(NeuralRegion* region, size_t numClusters);
    static std::vector<std::vector<Neuron*>> hierarchicalClustering(NeuralRegion* region,
                                                                    float threshold = 0.5f);
    
    // Analysis reports
    static std::string generateNetworkReport(NeuralRegion* region);
    static std::string generateCommunityReport(NeuralRegion* region, const std::vector<std::vector<Neuron*>>& communities);
    static std::string generateCentralityReport(NeuralRegion* region, const std::map<NeuronId, float>& centralities);
    static std::string generateRobustnessReport(NeuralRegion* region);
    static std::string generateEfficiencyReport(NeuralRegion* region);
    
    // Visualization data
    static std::string exportNetworkForVisualization(NeuralRegion* region, const std::string& format = "graphviz");
    static std::string exportCommunitiesForVisualization(NeuralRegion* region,
                                                          const std::vector<std::vector<Neuron*>>& communities,
                                                          const std::string& format = "graphviz");
    static std::string exportCentralityForVisualization(NeuralRegion* region,
                                                        const std::map<NeuronId, float>& centralities,
                                                        const std::string& format = "graphviz");
    
    // Advanced analysis
    static std::map<std::string, float> performComprehensiveAnalysis(NeuralRegion* region);
    static bool detectAnomalies(NeuralRegion* region, std::vector<std::string>& anomalies);
    static bool identifyCriticalNodes(NeuralRegion* region, std::vector<NeuronId>& criticalNodes,
                                     float threshold = 0.8f);
    static bool identifyBottlenecks(NeuralRegion* region, std::vector<std::pair<NeuronId, float>>& bottlenecks);
};

// Advanced region synthesizer
class RegionSynthesizer {
public:
    // Region synthesis from specifications
    static NeuralRegion* synthesizeRegionFromSpec(const std::string& specFile);
    static NeuralRegion* synthesizeRegion(const std::string& name, size_t neuronCount,
                                          const std::vector<NeuronType>& neuronTypes,
                                          ConnectivityPattern pattern,
                                          float connectionProbability = 0.1f);
    static NeuralRegion* synthesizeFunctionalRegion(const std::string& function,
                                                    size_t neuronCount = 100,
                                                    float specializationLevel = 1.0f);
    static NeuralRegion* synthesizeDevelopmentalRegion(DevelopmentalStage stage,
                                                       size_t neuronCount = 50,
                                                       float plasticityLevel = 1.0f);
    static NeuralRegion* synthesizeMemoryRegion(size_t capacity = 100,
                                                float decayRate = 0.01f,
                                                bool enableReplay = true);
    static NeuralRegion* synthesizeMotorRegion(size_t outputNeurons = 10,
                                              float responseSpeed = 1.0f,
                                              bool enableFeedback = true);
    static NeuralRegion* synthesizeSensoryRegion(size_t inputNeurons = 20,
                                                float sensitivity = 1.0f,
                                                bool enableAdaptation = true);
    
    // Region assembly
    static NeuralRegion* assembleRegion(const std::vector<NeuralRegion*>& subRegions,
                                        const std::string& assemblyMode = "sequential");
    static NeuralRegion* connectRegions(const std::vector<NeuralRegion*>& regions,
                                        float interRegionConnection = 0.1f);
    static NeuralRegion* optimizeRegionAssembly(NeuralRegion* region);
    static NeuralRegion* balanceRegionAssembly(NeuralRegion* region);
    
    // Region transformation
    static NeuralRegion* transformRegion(NeuralRegion* sourceRegion,
                                         const std::string& transformation,
                                         float parameter = 1.0f);
    static NeuralRegion* scaleRegion(NeuralRegion* region, float scaleFactor);
    static NeuralRegion* rotateRegion(NeuralRegion* region, float angle);
    static NeuralRegion* translateRegion(NeuralRegion* region, float dx, float dy);
    static NeuralRegion* applyFilterRegion(NeuralRegion* region, const std::string& filter);
    
    // Region export/import
    static bool saveRegionToFile(NeuralRegion* region, const std::string& filepath);
    static NeuralRegion* loadRegionFromFile(const std::string& filepath);
    static std::string exportRegionToString(NeuralRegion* region);
    static NeuralRegion* importRegionFromString(const std::string& data);
    
    // Region validation
    static bool validateRegion(NeuralRegion* region);
    static std::vector<std::string> validateRegionConnectivity(NeuralRegion* region);
    static std::vector<std::string> validateRegionFunctionality(NeuralRegion* region);
    static std::vector<std::string> validateRegionEfficiency(NeuralRegion* region);
    
    // Region comparison
    static float compareRegions(NeuralRegion* region1, NeuralRegion* region2);
    static std::string analyzeRegionDifference(NeuralRegion* region1, NeuralRegion* region2);
    static bool isRegionSubset(NeuralRegion* region1, NeuralRegion* region2);
    static bool isRegionSuperset(NeuralRegion* region1, NeuralRegion* region2);
    static std::vector<NeuralRegion*> findSimilarRegions(const std::vector<NeuralRegion*>& regions,
                                                        float similarityThreshold = 0.7f);
    
    // Region optimization
    static bool optimizeRegionForLearning(NeuralRegion* region);
    static bool optimizeRegionForMemory(NeuralRegion* region);
    static bool optimizeRegionForSpeed(NeuralRegion* region);
    static bool optimizeRegionForRobustness(NeuralRegion* region);
    static bool optimizeRegionForPlasticity(NeuralRegion* region);
};

} // namespace nlm
