#pragma once

#include "../core/Types/Types.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace nlm {

// Custom plasticity rule designer for expert users
class PlasticityDesigner {
public:
    // Plasticity rule types
    enum class RuleType {
        Hebbian,
        STDP,
        RewardModulatedSTDP,
        TripletSTDP,
        CustomFunction,
        AdaptiveLTP_LTD,
        Homeostatic
    };
    
    // Rule configuration parameters
    struct PlasticityRuleConfig {
        RuleType type;
        std::string name;
        float learningRate = 0.01f;
        float ltpWeight = 0.02f;
        float ltdWeight = 0.015f;
        float timeConstant = 20.0f;
        float maxWeight = 1.0f;
        float minWeight = -1.0f;
        float targetFiringRate = 10.0f;
        std::map<std::string, float> customParameters;
        
        // Validation
        bool isValid() const {
            return learningRate >= 0.0f && ltpWeight >= 0.0f && ltdWeight >= 0.0f;
        }
    };
    
    // Create plasticity rules
    static std::unique_ptr<PlasticityRule> createRule(const PlasticityRuleConfig& config);
    static std::unique_ptr<PlasticityRule> createAdaptivePlasticity(const PlasticityRuleConfig& config);
    static std::unique_ptr<PlasticityRule> createHomeostaticPlasticity(const PlasticityRuleConfig& config);
    
    // Rule testing and validation
    static bool testRuleEffect(const PlasticityRuleConfig& config, const std::vector<float>& prePattern, 
                               const std::vector<float>& postPattern, float expectedDelta);
    static bool validateRuleParameters(const PlasticityRuleConfig& config);
    static bool optimizeRuleParameters(PlasticityRuleConfig& config, const std::vector<float>& trainingData);
    
    // Rule composition
    static std::unique_ptr<PlasticityRule> combineRules(const std::vector<std::unique_ptr<PlasticityRule>>& rules,
                                                         const std::vector<float>& weights);
    static std::unique_ptr<PlasticityRule> modifyRule(std::unique_ptr<PlasticityRule> rule, 
                                                     const PlasticityRuleConfig& newConfig);
    
    // Specialized rule types for expert users
    static PlasticityRuleConfig createSTDPConfig(float ltp, float ltd, float tau = 20.0f, 
                                                 float wmax = 1.0f, float wmin = -0.1f);
    static PlasticityRuleConfig createHebbianConfig(float lr = 0.01f, float wmax = 1.0f, 
                                                    float lambda = 0.1f, float beta = 0.5f);
    static PlasticityRuleConfig createRewardModulatedConfig(float lr = 0.01f, float scale = 1.0f,
                                                            float decay = 0.99f);
    static PlasticityRuleConfig createHomeostaticConfig(float targetFR = 10.0f, float tau = 100.0f,
                                                        float gmax = 1.0f, float gmin = 0.0f);
    
    // Rule analysis
    static std::map<std::string, float> analyzeRuleDynamics(const PlasticityRuleConfig& config);
    static float calculateWeightStability(const PlasticityRuleConfig& config, const std::vector<float>& weightHistory);
    static std::vector<float> simulateRule(const PlasticityRuleConfig& config, 
                                           const std::vector<float>& inputPatterns,
                                           size_t steps = 1000);
    
    // Rule export/import for sharing
    static std::string exportRule(const PlasticityRuleConfig& config);
    static bool importRule(PlasticityRuleConfig& config, const std::string& ruleString);
    static bool saveRuleToFile(const PlasticityRuleConfig& config, const std::string& filepath);
    static bool loadRuleFromFile(PlasticityRuleConfig& config, const std::string& filepath);
    
    // Performance analysis
    static float estimateComputationalCost(const PlasticityRuleConfig& config);
    static size_t estimateMemoryUsage(const PlasticityRuleConfig& config);
    static float predictLearningSpeed(const PlasticityRuleConfig& config, const std::vector<float>& data);
    
    // Advanced features
    static std::unique_ptr<PlasticityRule> createMetaPlasticityRule(const PlasticityRuleConfig& baseConfig,
                                                                     const PlasticityRuleConfig& metaConfig);
    static std::unique_ptr<PlasticityRule> createPlasticityNetwork(const std::vector<PlasticityRuleConfig>& networkConfig);
    static float evaluateRuleSet(const std::vector<PlasticityRuleConfig>& ruleSet,
                                const std::vector<std::vector<float>>& testPatterns);
    
    // Debugging and analysis tools
    static std::vector<float> debugRule(std::unique_ptr<PlasticityRule> rule, 
                                        const std::vector<float>& preSpikes,
                                        const std::vector<float>& postSpikes);
    static std::string getRuleDiagnostics(const PlasticityRuleConfig& config);
    static std::map<std::string, float> profileRule(std::unique_ptr<PlasticityRule> rule,
                                                    const std::vector<std::vector<float>>& testData);
};

// Advanced plasticity learning system
class PlasticityLearningSystem {
public:
    // Learning algorithms
    static bool learnOptimalParameters(std::vector<PlasticityRuleConfig>& candidateRules,
                                        const std::vector<std::vector<float>>& trainingData,
                                        size_t generations = 100,
                                        float mutationRate = 0.1f);
    static bool evolvePlasticityRules(std::vector<PlasticityRuleConfig>& rules,
                                     const std::vector<std::vector<float>>& performanceMetrics,
                                     float selectionPressure = 0.8f);
    static bool reinforcementLearnPlasticity(std::vector<PlasticityRuleConfig>& rules,
                                             const std::vector<float>& rewards,
                                             const std::vector<std::vector<float>>& states);
    
    // Rule adaptation
    static bool adaptToEnvironment(PlasticityRuleConfig& rule, const std::vector<float>& environmentalFactors);
    static bool maintainPlasticityBalance(PlasticityRuleConfig& rule, float targetBalance);
    static bool preventCatastrophicForgetting(PlasticityRuleConfig& rule, const std::vector<float>& memoryTrace);
    
    // Rule transfer and generalization
    static bool transferLearning(const PlasticityRuleConfig& sourceRule, 
                                 PlasticityRuleConfig& targetRule,
                                 const std::vector<float>& transferData);
    static bool generalizeRule(const PlasticityRuleConfig& trainedRule,
                              PlasticityRuleConfig& generalizedRule,
                              const std::vector<float>& testData);
    static bool metaLearnRuleSet(std::vector<PlasticityRuleConfig>& ruleSet,
                                 const std::vector<std::vector<float>>& metaTrainingData);
    
    // Advanced learning features
    static std::vector<PlasticityRuleConfig> generateRulePopulation(size_t populationSize);
    static float evaluateRulePerformance(const PlasticityRuleConfig& rule, 
                                         const std::vector<std::vector<float>>& testPatterns);
    static bool simulatePlasticityEvolution(std::vector<PlasticityRuleConfig>& population,
                                            const std::vector<std::vector<float>>& environmentChanges,
                                            size_t generations = 50);
    
    // Learning analysis and diagnostics
    static std::map<std::string, float> analyzeLearningProgress(const std::vector<PlasticityRuleConfig>& ruleSet,
                                                                 const std::vector<float>& performanceHistory);
    static std::vector<float> getLearningCurve(const std::vector<PlasticityRuleConfig>& rules,
                                               const std::vector<std::vector<float>>& data);
    static bool diagnoseLearningProblems(const std::vector<PlasticityRuleConfig>& rules,
                                         const std::vector<std::vector<float>>& data,
                                         std::vector<std::string>& problems);
};

} // namespace nlm
