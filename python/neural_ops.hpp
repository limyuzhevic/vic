#pragma once

// Advanced neural network operations for NLM
// Provides custom neural population creation, synapse manipulation and analysis,
// plasticity control and tuning, and developmental stage management

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <random>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <set>

#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/development/DevelopmentSystem.hpp"

namespace nlm {
namespace python {

// Custom neural population configuration
struct CustomPopulationConfig {
    std::string name;
    NeuronType neuronType;
    size_t neuronCount;
    std::string connectionPattern;
    float connectionDensity;
    float meanSynapticStrength;
    float stdSynapticStrength;
    std::map<std::string, std::variant<int, float, bool, std::string>> properties;
    bool isActive;
    std::chrono::system_clock::time_point createdAt;
    std::map<std::string, std::string> metadata;
};

// Synapse configuration
struct SynapseConfig {
    NeuronId sourceNeuron;
    NeuronId targetNeuron;
    SynapseType synapseType;
    float initialWeight;
    float maxWeight;
    float minWeight;
    float learningRate;
    bool isPlastic;
    std::string plasticityRule;
    std::map<std::string, std::variant<int, float, bool, std::string>> properties;
};

// Plasticity tuning parameters
struct PlasticityTuningParams {
    std::string ruleName;
    float learningRate;
    float stabilizationFactor;
    float explorationFactor;
    float consolidationFactor;
    std::map<std::string, std::variant<int, float, bool, std::string>> additionalParameters;
    bool isOptimized;
    double lastOptimizationScore;
};

// Developmental stage configuration
struct DevelopmentalStageConfig {
    DevelopmentalStage stage;
    std::string description;
    std::map<std::string, std::variant<int, float, bool, std::string>> parameters;
    std::vector<std::string> prerequisites;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool isActive;
    bool autoAdvance;
    float progress;
    std::map<std::string, std::string> metadata;
};

// Neural network operation statistics
struct NeuralOperationStats {
    std::chrono::system_clock::time_point operationTime;
    std::string operationType;
    size_t neuronCount;
    size_t synapseCount;
    double executionTime;
    double memoryUsage;
    std::map<std::string, double> performanceMetrics;
    std::map<std::string, std::string> metadata;
};

// Advanced neural network operations class
class AdvancedNeuralOperations {
public:
    // Custom neural population creation
    static std::vector<PopulationId> createCustomNeuralPopulation(std::shared_ptr<Brain> brain,
                                                                 const std::string& regionName,
                                                                 const CustomPopulationConfig& config);
    static std::vector<PopulationId> createNeuralPopulation(std::shared_ptr<Brain> brain,
                                                          const std::string& regionName,
                                                          NeuronType neuronType,
                                                          size_t neuronCount,
                                                          const std::string& connectionPattern = "random",
                                                          float connectionDensity = 0.1f,
                                                          float meanSynapticStrength = 1.0f);
    static std::vector<PopulationId> createConnectedPopulations(std::shared_ptr<Brain> brain,
                                                               const std::vector<std::string>& regionNames,
                                                               NeuronType neuronType,
                                                               size_t neuronsPerRegion,
                                                               const std::string& interRegionPattern = "symmetric");
    static CustomPopulationConfig createPopulationConfig(const std::string& name,
                                                        NeuronType neuronType,
                                                        size_t neuronCount,
                                                        const std::string& connectionPattern = "random",
                                                        float connectionDensity = 0.1f,
                                                        float meanSynapticStrength = 1.0f,
                                                        const std::map<std::string, std::variant<int, float, bool, std::string>>& properties = {});
    static bool configureExistingPopulation(std::shared_ptr<Brain> brain,
                                           PopulationId populationId,
                                           const CustomPopulationConfig& newConfig);
    static bool deleteNeuralPopulation(std::shared_ptr<Brain> brain, PopulationId populationId);
    static std::vector<CustomPopulationConfig> getAllPopulationConfigs(std::shared_ptr<Brain> brain);
    static CustomPopulationConfig getPopulationConfig(std::shared_ptr<Brain> brain, PopulationId populationId);
    
    // Synapse manipulation and analysis
    static SynapseId connectNeurons(std::shared_ptr<Brain> brain,
                                   NeuronId sourceNeuron,
                                   NeuronId targetNeuron,
                                   float weight = 1.0f,
                                   SynapseType synapseType = SynapseType::Excitatory);
    static bool disconnectNeurons(std::shared_ptr<Brain> brain,
                                  NeuronId sourceNeuron,
                                  NeuronId targetNeuron);
    static bool adjustSynapseWeight(std::shared_ptr<Brain> brain,
                                   SynapseId synapseId,
                                   float newWeight,
                                   float learningRate = 0.01f);
    static bool setSynapsePlasticity(std::shared_ptr<Brain> brain,
                                    SynapseId synapseId,
                                    bool isPlastic,
                                    const std::string& plasticityRule = "stdp");
    static std::vector<std::pair<NeuronId, NeuronId>> findStrongestConnections(std::shared_ptr<Brain> brain,
                                                                             size_t topN = 10);
    static std::vector<std::pair<NeuronId, NeuronId>> findWeakestConnections(std::shared_ptr<Brain> brain,
                                                                             size_t bottomN = 10);
    static std::vector<std::pair<NeuronId, NeuronId>> findConnectedNeuronPairs(std::shared_ptr<Brain> brain,
                                                                             NeuronType neuronType = NeuronType::Excitatory,
                                                                             float minWeight = 0.0f);
    static float computeConnectionStrength(std::shared_ptr<Brain> brain,
                                          NeuronId sourceNeuron,
                                          NeuronId targetNeuron);
    static double computeNetInput(std::shared_ptr<Brain> brain, NeuronId neuronId);
    static float getNeuronExcitability(std::shared_ptr<Brain> brain, NeuronId neuronId);
    static float setNeuronExcitability(std::shared_ptr<Brain> brain, NeuronId neuronId, float excitability);
    static std::map<NeuronId, std::vector<std::pair<NeuronId, float>>> analyzeNeuronConnectivity(std::shared_ptr<Brain> brain, NeuronId neuronId);
    static std::map<NeuronId, float> computeNeuronParticipation(std::shared_ptr<Brain> brain);
    static std::vector<std::vector<std::pair<NeuronId, float>>> computeNetworkTopology(std::shared_ptr<Brain> brain);
    
    // Plasticity control and tuning
    static bool enableSTDP(std::shared_ptr<Brain> brain, float learningRate = 0.01f,
                          float ltpWeight = 0.01f, float ltdWeight = 0.012f);
    static bool disableSTDP(std::shared_ptr<Brain> brain);
    static bool enableHebbianPlasticity(std::shared_ptr<Brain> brain, float learningRate = 0.005f);
    static bool disableHebbianPlasticity(std::shared_ptr<Brain> brain);
    static bool enableStructuralPlasticity(std::shared_ptr<Brain> brain, bool enable = true);
    static bool isStructuralPlasticityEnabled(std::shared_ptr<Brain> brain);
    static void configureSTDPParameters(std::shared_ptr<Brain> brain,
                                       const PlasticityTuningParams& params);
    static void configureHebbianParameters(std::shared_ptr<Brain> brain,
                                          const PlasticityTuningParams& params);
    static STDP::STDPConfig getSTDPConfig(std::shared_ptr<Brain> brain);
    static Hebbian::HebbianConfig getHebbianConfig(std::shared_ptr<Brain> brain);
    static std::vector<PlasticityTuningParams> getAvailablePlasticityRules();
    static PlasticityTuningParams optimizePlasticityForLearning(std::shared_ptr<Brain> brain);
    static PlasticityTuningParams optimizePlasticityForStability(std::shared_ptr<Brain> brain);
    static PlasticityTuningParams optimizePlasticityForMemory(std::shared_ptr<Brain> brain);
    static void applyPlasticityTuning(std::shared_ptr<Brain> brain, const PlasticityTuningParams& params);
    static std::vector<double> simulatePlasticityDynamics(std::shared_ptr<Brain> brain,
                                                         const PlasticityTuningParams& params,
                                                         size_t simulationSteps = 1000);
    
    // Developmental stage management
    static bool triggerCriticalPeriod(std::shared_ptr<Brain> brain, DevelopmentalStage stage = DevelopmentalStage::CriticalPeriod);
    static bool isCriticalPeriodActive(std::shared_ptr<Brain> brain);
    static DevelopmentalStage getCurrentDevelopmentalStage(std::shared_ptr<Brain> brain);
    static bool setDevelopmentalStage(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static void advanceDevelopmentStage(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static std::vector<DevelopmentalStage> getAvailableDevelopmentalStages(std::shared_ptr<Brain> brain);
    static DevelopmentalStageConfig getDevelopmentalStageConfig(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static bool setDevelopmentalStageConfig(std::shared_ptr<Brain> brain, const DevelopmentalStageConfig& config);
    static std::chrono::system_clock::time_point getDevelopmentalStageStartTime(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static std::chrono::system_clock::time_point getDevelopmentalStageEndTime(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static float getDevelopmentalStageProgress(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static bool isDevelopmentalStageComplete(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static std::vector<std::string> getDevelopmentalStagePrerequisites(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static bool canAdvanceToDevelopmentalStage(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static void autoAdvanceDevelopmentalStages(std::shared_ptr<Brain> brain, bool enable = true);
    static std::vector<DevelopmentalStageConfig> getAllDevelopmentalStageConfigs(std::shared_ptr<Brain> brain);
    static void resetDevelopmentalStages(std::shared_ptr<Brain> brain);
    static void pauseDevelopment(std::shared_ptr<Brain> brain);
    static void resumeDevelopment(std::shared_ptr<Brain> brain);
    static bool isDevelopmentPaused(std::shared_ptr<Brain> brain);
    
    // Network analysis and statistics
    static std::vector<std::vector<double>> computeAdjacencyMatrix(std::shared_ptr<Brain> brain);
    static std::vector<std::vector<double>> computeCorrelationMatrix(std::shared_ptr<Brain> brain);
    static std::map<std::string, double> computeGraphMetrics(std::shared_ptr<Brain> brain);
    static std::map<NeuronId, double> computeNeuronBetweennessCentrality(std::shared_ptr<Brain> brain);
    static std::map<NeuronId, double> computeNeuronClusteringCoefficient(std::shared_ptr<Brain> brain);
    static std::map<NeuronId, double> computeNeuronEigenvectorCentrality(std::shared_ptr<Brain> brain);
    static std::vector<std::vector<NeuronId>> findConnectedComponents(std::shared_ptr<Brain> brain);
    static std::vector<NeuronId> findLargestConnectedComponent(std::shared_ptr<Brain> brain);
    static size_t getNetworkSize(std::shared_ptr<Brain> brain);
    static size_t getTotalSynapseCount(std::shared_ptr<Brain> brain);
    static float computeNetworkDensity(std::shared_ptr<Brain> brain);
    static float computeNetworkModularity(std::shared_ptr<Brain> brain);
    static std::vector<std::vector<double>> computeCommunityStructure(std::shared_ptr<Brain> brain);
    
    // Network operations utilities
    static std::vector<NeuronId> getAllNeuronIds(std::shared_ptr<Brain> brain);
    static std::vector<SynapseId> getAllSynapseIds(std::shared_ptr<Brain> brain);
    static std::vector<PopulationId> getAllPopulationIds(std::shared_ptr<Brain> brain);
    static std::vector<RegionId> getAllRegionIds(std::shared_ptr<Brain> brain);
    static std::vector<NeuronId> getNeuronsByType(std::shared_ptr<Brain> brain, NeuronType neuronType);
    static std::vector<SynapseId> getSynapsesByType(std::shared_ptr<Brain> brain, SynapseType synapseType);
    static std::vector<NeuronId> getActiveNeurons(std::shared_ptr<Brain> brain);
    static std::vector<NeuronId> getFiringNeurons(std::shared_ptr<Brain> brain);
    static size_t getActiveNeuronCount(std::shared_ptr<Brain> brain);
    static size_t getFiringNeuronCount(std::shared_ptr<Brain> brain);
    static float getNetworkActivityLevel(std::shared_ptr<Brain> brain);
    static float computeFiringRate(std::shared_ptr<Brain> brain);
    static float computeSyncLevel(std::shared_ptr<Brain> brain);
    
    // Save and load network configurations
    static bool saveNetworkToFile(std::shared_ptr<Brain> brain, const std::string& filepath);
    static std::shared_ptr<Brain> loadNetworkFromFile(const std::string& filepath);
    static std::string exportNetworkToJSON(std::shared_ptr<Brain> brain);
    static std::shared_ptr<Brain> importNetworkFromJSON(const std::string& jsonString);
    static bool exportNetworkToBinary(std::shared_ptr<Brain> brain, const std::string& filepath);
    static std::shared_ptr<Brain> importNetworkFromBinary(const std::string& filepath);
    
    // Statistics and monitoring
    static std::vector<NeuralOperationStats> getOperationHistory(size_t maxHistory = 1000);
    static void clearOperationHistory();
    static std::map<std::string, double> getNetworkStatistics(std::shared_ptr<Brain> brain);
    static std::vector<std::pair<std::string, double>> getNetworkStatisticsSorted(std::shared_ptr<Brain> brain);
    static double calculateNetworkRobustness(std::shared_ptr<Brain> brain);
    static double calculateNetworkEfficiency(std::shared_ptr<Brain> brain);
    static double calculateSmallWorldness(std::shared_ptr<Brain> brain);
    static std::string generateNetworkReport(std::shared_ptr<Brain> brain);
    
    // Network operations control
    static void pauseNetworkOperations(std::shared_ptr<Brain> brain);
    static void resumeNetworkOperations(std::shared_ptr<Brain> brain);
    static bool isNetworkOperationsPaused(std::shared_ptr<Brain> brain);
    static void setMaxOperationTime(double maxTimeSeconds);
    static double getMaxOperationTime();
    static void setMemoryLimit(size_t memoryLimitBytes);
    static size_t getMemoryLimit();
    static size_t getCurrentMemoryUsage(std::shared_ptr<Brain> brain);
    
    // Singleton instance management
    static AdvancedNeuralOperations& getInstance();
    
private:
    AdvancedNeuralOperations() = default;
    ~AdvancedNeuralOperations() = default;
    AdvancedNeuralOperations(const AdvancedNeuralOperations&) = delete;
    AdvancedNeuralOperations& operator=(const AdvancedNeuralOperations&) = delete;
    
    static std::vector<NeuralOperationStats> operationHistory_;
    static std::mutex mutex_;
    static double maxOperationTime_;
    static size_t memoryLimit_;
    static bool operationsPaused_;
};