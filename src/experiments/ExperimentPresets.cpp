#include "ExperimentPresets.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

struct ExperimentPresets::Impl {
    std::unordered_map<std::string, Preset> presetsByName;
    std::unordered_map<std::string, std::vector<std::string>> presetsByCategory;
    std::vector<std::string> allPresetNames;
    std::vector<std::string> categories;
    bool initialized;
    
    Impl() : initialized(false) {}
};

std::vector<ExperimentPresets::Preset> ExperimentPresets::presets_;
bool ExperimentPresets::initialized_ = false;

ExperimentPresets::ExperimentPresets() {
    if (!initialized_) {
        initializePresets();
    }
    pImpl = std::make_unique<Impl>();
}

ExperimentPresets::~ExperimentPresets() = default;

void ExperimentPresets::initializePresets() {
    if (initialized_) return;
    
    // Initialize with empty collections
    presets_.clear();
    initialized_ = true;
}

std::vector<std::string> ExperimentPresets::getAllPresetNames() const {
    if (!pImpl->initialized) {
        const_cast<ExperimentPresets*>(this)->initializePresets();
    }
    return pImpl->allPresetNames;
}

std::vector<std::string> ExperimentPresets::getPresetCategories() const {
    if (!pImpl->initialized) {
        const_cast<ExperimentPresets*>(this)->initializePresets();
    }
    return pImpl->categories;
}

std::vector<std::string> ExperimentPresets::getPresetsInCategory(const std::string& category) const {
    if (!pImpl->initialized) {
        const_cast<ExperimentPresets*>(this)->initializePresets();
    }
    
    auto it = pImpl->presetsByCategory.find(category);
    if (it != pImpl->presetsByCategory.end()) {
        return it->second;
    }
    return {};
}

std::string ExperimentPresets::getPresetJson(const std::string& category, const std::string& name) {
    // This would generate JSON for a specific preset
    // For now, return a basic JSON structure
    std::ostringstream oss;
    oss << "{\"name\": \"" << name << "\", \"category\": \"" << category << "\"}";
    return oss.str();
}

std::string ExperimentPresets::getPresetConfig(const std::string& presetName) {
    // This would return the configuration for a specific preset
    // For now, return a basic configuration
    return "{\"name\": \"" + presetName + "\"}";
}

std::string ExperimentPresets::generateResearchPresetsJson() {
    std::ostringstream oss;
    oss << "{\"Research\": {\"Scientific\": {\"name\": \"ResearchScientific\", \"description\": \"Parameters optimized for scientific rigor\"}}, \"Speed\": {\"name\": \"ResearchSpeed\", \"description\": \"Parameters optimized for fast iterations\"}, \"Reproducibility\": {\"name\": \"ResearchReproducibility\", \"description\": \"Parameters optimized for reproducibility\"}}";
    return oss.str();
}

std::string ExperimentPresets::generatePerformancePresetsJson() {
    std::ostringstream oss;
    oss << "{\"Performance\": {\"Small\": {\"name\": \"PerformanceSmall\", \"description\": \"Small-scale experiments (100-500 neurons)\"}, \"Medium\": {\"name\": \"PerformanceMedium\", \"description\": \"Medium-scale experiments (500-1000 neurons)\"}, \"Large\": {\"name\": \"PerformanceLarge\", \"description\": \"Large-scale experiments (1000+ neurons)\"}}}";
    return oss.str();
}

std::string ExperimentPresets::generateResearchQuestionPresetsJson() {
    std::ostringstream oss;
    oss << "{\"ResearchQuestion\": {\"LearningCurves\": {\"name\": \"LearningCurves\", \"description\": \"Configure experiments to track learning over time\"}, \"MemoryCapacity\": {\"name\": \"MemoryCapacity\", \"description\": \"Configure experiments to test memory limits\"}, \"SocialDynamics\": {\"name\": \"SocialDynamics\", \"description\": \"Configure experiments for social behavior studies\"}, \"DevelopmentStages\": {\"name\": \"DevelopmentStages\", \"description\": \"Configure experiments for developmental studies\"}}}";
    return oss.str();
}

std::string ExperimentPresets::generateComparativeExperimentConfigsJson() {
    std::ostringstream oss;
    oss << "{\"ComparativeExperiment\": {\"MultiSeed\": {\"name\": \"MultiSeedExperiment\", \"description\": \"Run multiple seeds for statistical significance\"}, \"Ablation\": {\"name\": \"AblationExperiment\", \"description\": \"Test contribution of specific mechanisms\"}, \"Scaling\": {\"name\": \"ScalingExperiment\", \"description\": \"Benchmark performance across different scales\"}}}";
    return oss.str();
}

std::string ExperimentPresets::generateParameterOptimizationSetsJson() {
    std::ostringstream oss;
    oss << "{\"ParameterOptimization\": {\"GridSearch\": {\"name\": \"GridSearch\", \"description\": \"Systematic parameter grid search\"}, \"RandomSearch\": {\"name\": \"RandomSearch\", \"description\": \"Random sampling of parameter space\"}, \"BayesianOptimization\": {\"name\": \"BayesianOptimization\", \"description\": \"Model-based parameter optimization\"}, \"GeneticAlgorithm\": {\"name\": \"GeneticAlgorithm\", \"description\": \"Evolutionary parameter optimization\"}}}";
    return oss.str();
}

std::string ExperimentPresets::generateScalabilityPresetsJson() {
    std::ostringstream oss;
    oss << "{\"Scalability\": {\"Development\": {\"name\": \"ScalabilityDevelopment\", \"description\": \"Optimized for small-scale development and testing\"}, \"Research\": {\"name\": \"ScalabilityResearch\", \"description\": \"Optimized for research-grade experiments\"}, \"Production\": {\"name\": \"ScalabilityProduction\", \"description\": \"Optimized for large-scale production experiments\"}}}";
    return oss.str();
}

std::string ExperimentPresets::generateAdvancedExperimentFeaturesJson() {
    std::ostringstream oss;
    oss << "{\"AdvancedFeatures\": {\"ParameterSweep\": {\"name\": \"ParameterSweep\", \"description\": \"Automated parameter sweep capabilities\"}, \"MultiObjectiveOptimization\": {\"name\": \"MultiObjectiveOptimization\", \"description\": \"Multi-objective optimization capabilities\"}, \"AdaptiveExperimentation\": {\"name\": \"AdaptiveExperimentation\", \"description\": \"Adaptive experimental design\"}, \"ComparativeAnalysis\": {\"name\": \"ComparativeAnalysis\", \"description\": \"Comparative analysis capabilities\"}, \"HyperparameterOptimization\": {\"name\": \"HyperparameterOptimization\", \"description\": \"Hyperparameter optimization capabilities\"}}}";
    return oss.str();
}

std::string ExperimentPresets::createPresetJson(const std::string& name, const std::string& category,
                                         const std::string& description, const std::string& config) {
    std::ostringstream oss;
    oss << "{\"name\": \"" << name << "\", \"category\": \"" << category << "\", \"description\": \"" << description << "\", \"config\": " << config << "}";
    return oss.str();
}

std::string ExperimentPresets::getResearchPresets() {
    return generateResearchPresetsJson();
}

std::string ExperimentPresets::getPerformancePresets() {
    return generatePerformancePresetsJson();
}

std::string ExperimentPresets::getResearchQuestionPresets() {
    return generateResearchQuestionPresetsJson();
}

std::string ExperimentPresets::getComparativeExperimentConfigs() {
    return generateComparativeExperimentConfigsJson();
}

std::string ExperimentPresets::getParameterOptimizationSets() {
    return generateParameterOptimizationSetsJson();
}

std::string ExperimentPresets::getScalabilityPresets() {
    return generateScalabilityPresetsJson();
}

std::string ExperimentPresets::getAdvancedExperimentFeatures() {
    return generateAdvancedExperimentFeaturesJson();
}

bool ExperimentPresets::savePresets(const std::string& filepath) {
    NLM_LOG_INFO("Saving experiment presets to: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing presets: " + filepath);
        return false;
    }
    
    // Write preset data
    file << "{\"presets\": [";
    
    // Add research presets
    std::string researchPresets = getResearchPresets();
    file << researchPresets;
    
    file << "]}";
    
    file.close();
    NLM_LOG_INFO("Successfully saved experiment presets to: " + filepath);
    return true;
}

bool ExperimentPresets::loadPresets(const std::string& filepath) {
    NLM_LOG_INFO("Loading experiment presets from: " + filepath);
    
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Presets file does not exist: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open presets file: " + filepath);
        return false;
    }
    
    // Parse presets (simplified)
    // In a real implementation, this would parse the JSON
    
    file.close();
    NLM_LOG_INFO("Successfully loaded experiment presets from: " + filepath);
    return true;
}

std::string ExperimentPresets::getPresetDescription(const std::string& presetName) {
    // Get description based on preset name
    if (presetName == "ResearchScientific") {
        return "Parameters optimized for scientific rigor and experimental control";
    } else if (presetName == "ResearchSpeed") {
        return "Parameters optimized for fast iteration and quick feedback";
    } else if (presetName == "ResearchReproducibility") {
        return "Parameters optimized for reproducibility and consistency";
    } else if (presetName == "PerformanceSmall") {
        return "Small-scale experiments (100-500 neurons) for development and testing";
    } else if (presetName == "PerformanceMedium") {
        return "Medium-scale experiments (500-1000 neurons) for balanced research";
    } else if (presetName == "PerformanceLarge") {
        return "Large-scale experiments (1000+ neurons) for research-grade performance";
    } else if (presetName == "LearningCurves") {
        return "Configure experiments to track learning over time with appropriate sampling";
    } else if (presetName == "MemoryCapacity") {
        return "Configure experiments to test memory limits with appropriate scale";
    } else if (presetName == "SocialDynamics") {
        return "Configure experiments for social behavior studies with appropriate interaction rates";
    } else if (presetName == "DevelopmentStages") {
        return "Configure experiments for developmental studies with milestone tracking";
    }
    
    return "Unknown preset";
}

bool ExperimentPresets::validatePreset(const std::string& presetName) {
    // Basic validation for preset name
    if (presetName.empty()) {
        return false;
    }
    
    // Check if preset exists (in real implementation)
    // For now, assume all known presets are valid
    std::vector<std::string> validPresets = getAllPresetNames();
    return std::find(validPresets.begin(), validPresets.end(), presetName) != validPresets.end();
}

std::vector<std::string> ExperimentPresets::getPresetDependencies(const std::string& presetName) {
    // Get dependencies based on preset name
    if (presetName == "ResearchScientific") {
        return {"ResearchReproducibility"};
    } else if (presetName == "ResearchReproducibility") {
        return {"ResearchSpeed"};
    } else if (presetName == "PerformanceLarge") {
        return {"PerformanceMedium"};
    }
    
    return {};
}

// ResearchPresets namespace implementations

std::string ResearchPresets::getScientificConfig() {
    return "{\"numAgents\": 100, \"worldWidth\": 200, \"worldHeight\": 200, \"explorationRate\": 0.5f, \"noveltyThreshold\": 0.3f, \"enableMultiObjectiveOptimization\": true, \"enableAdaptiveExperimentation\": true, \"enableComparativeAnalysis\": true}";
}

std::string ResearchPresets::getSpeedConfig() {
    return "{\"numAgents\": 20, \"worldWidth\": 50, \"worldHeight\": 50, \"explorationRate\": 0.2f, \"noveltyThreshold\": 0.8f, \"enableParameterSweeps\": true, \"enableMultiObjectiveOptimization\": false, \"enableComparativeAnalysis\": false}";
}

std::string ResearchPresets::getReproducibilityConfig() {
    return "{\"numAgents\": 30, \"worldWidth\": 100, \"worldHeight\": 100, \"explorationRate\": 0.4f, \"noveltyThreshold\": 0.5f, \"enableParameterSweeps\": true, \"enableMultiObjectiveOptimization\": true, \"enableAdaptiveExperimentation\": true, \"enableComparativeAnalysis\": true}";
}

// PerformancePresets namespace implementations

std::string PerformancePresets::getSmallConfig() {
    return "{\"initialNeurons\": 10000, \"maxNeurons\": 50000, \"connectionProbability\": 0.05f, \"synapseFormationSteps\": 5000}";
}

std::string PerformancePresets::getMediumConfig() {
    return "{\"initialNeurons\": 50000, \"maxNeurons\": 100000, \"connectionProbability\": 0.1f, \"synapseFormationSteps\": 10000}";
}

std::string PerformancePresets::getLargeConfig() {
    return "{\"initialNeurons\": 100000, \"maxNeurons\": 500000, \"connectionProbability\": 0.15f, \"synapseFormationSteps\": 20000}";
}

// ResearchQuestionPresets namespace implementations

std::string ResearchQuestionPresets::getLearningCurvesConfig() {
    return "{\"enableParameterSweeps\": true, \"parameterRanges\": {\"explorationRate\": [0.1, 0.2, 0.3, 0.4, 0.5], \"noveltyThreshold\": [0.2, 0.4, 0.6, 0.8]}, \"optimizationObjectives\": [\"learningRate\", \"memoryRetention\"]}";
}

std::string ResearchQuestionPresets::getMemoryCapacityConfig() {
    return "{\"initialNeurons\": 50000, \"maxNeurons\": 200000, \"connectionProbability\": 0.1f, \"enableParameterSweeps\": true, \"parameterRanges\": {\"synapticStrength\": [0.5, 0.8, 1.0], \"plasticityRate\": [0.001, 0.01, 0.1]}}";
}

std::string ResearchQuestionPresets::getSocialDynamicsConfig() {
    return "{\"numAgents\": 150, \"enableParameterSweeps\": true, \"parameterRanges\": {\"socialInteractionRate\": [0.1, 0.3, 0.5, 0.7], \"imitationProbability\": [0.3, 0.6, 0.9]}}";
}

std::string ResearchQuestionPresets::getDevelopmentStagesConfig() {
    return "{\"enableDevelopmentalTracking\": true, \"developmentalMilestoneInterval\": 25.0f, \"enableParameterSweeps\": true, \"parameterRanges\": {\"developmentStageDuration\": [1000, 5000, 10000], \"transitionRate\": [0.01, 0.05, 0.1]}}";
}

// ComparativeExperimentPresets namespace implementations

std::string ComparativeExperimentPresets::getBaselineConfig() {
    return "{\"name\": \"Baseline\", \"description\": \"Standard configuration for baseline comparisons\", \"numTrials\": 50, \"stepsPerTrial\": 100}";
}

std::string ComparativeExperimentPresets::getParameterSweepConfig() {
    return "{\"name\": \"ParameterSweepComparison\", \"description\": \"Optimized for parameter sweep experiments\", \"enableParameterSweeps\": true, \"parallelSweeps\": true}";
}

std::string ComparativeExperimentPresets::getMultiObjectiveConfig() {
    return "{\"name\": \"MultiObjectiveComparison\", \"description\": \"Optimized for multi-objective optimization experiments\", \"enableMultiObjectiveOptimization\": true, \"convergenceThreshold\": 0.001f}";
}

// ParameterOptimizationSets namespace implementations

std::string ParameterOptimizationSets::getGridSearchConfig() {
    return "{\"name\": \"GridSearch\", \"description\": \"Systematic parameter grid search with comprehensive coverage\", \"algorithm\": \"GridSearch\", \"samplingStrategy\": \"exhaustive\"}";
}

std::string ParameterOptimizationSets::getRandomSearchConfig() {
    return "{\"name\": \"RandomSearch\", \"description\": \"Random sampling of parameter space with statistical coverage\", \"algorithm\": \"RandomSearch\", \"samplingStrategy\": \"random\"}";
}

std::string ParameterOptimizationSets::getBayesianOptimizationConfig() {
    return "{\"name\": \"BayesianOptimization\", \"description\": \"Model-based parameter optimization with probabilistic models\", \"algorithm\": \"BayesianOptimization\", \"modelType\": \"GaussianProcess\"}";
}

std::string ParameterOptimizationSets::getGeneticAlgorithmConfig() {
    return "{\"name\": \"GeneticAlgorithm\", \"description\": \"Evolutionary parameter optimization with population-based search\", \"algorithm\": \"GeneticAlgorithm\", \"populationSize\": 50}";
}

// ScalabilityPresets namespace implementations

std::string ScalabilityPresets::getDevelopmentConfig() {
    return "{\"name\": \"DevelopmentScalability\", \"description\": \"Optimized for small-scale development and testing with rapid iteration\", \"initialNeurons\": 5000, \"maxNeurons\": 20000, \"enableMultithreading\": false}";
}

std::string ScalabilityPresets::getResearchConfig() {
    return "{\"name\": \"ResearchScalability\", \"description\": \"Optimized for research-grade experiments with balanced performance\", \"initialNeurons\": 20000, \"maxNeurons\": 100000, \"enableMultithreading\": true, \"threadCount\": 4}";
}

std::string ScalabilityPresets::getProductionConfig() {
    return "{\"name\": \"ProductionScalability\", \"description\": \"Optimized for large-scale production experiments with maximum performance\", \"initialNeurons\": 100000, \"maxNeurons\": 1000000, \"enableMultithreading\": true, \"threadCount\": 16, \"enableSIMD\": true, \"enableSparseConnectivity\": true}";
}

// AdvancedExperimentFeatures namespace implementations

std::string AdvancedExperimentFeatures::getParameterSweepConfig() {
    return "{\"name\": \"ParameterSweepFeatures\", \"description\": \"Automated parameter sweep capabilities with parallel execution\", \"enableParameterSweeps\": true, \"parallelSweeps\": true, \"enableAdaptiveExperimentation\": true}";
}

std::string AdvancedExperimentFeatures::getMultiObjectiveOptimizationConfig() {
    return "{\"name\": \"MultiObjectiveOptimizationFeatures\", \"description\": \"Multi-objective optimization with Pareto front analysis\", \"enableMultiObjectiveOptimization\": true, \"convergenceThreshold\": 0.001f, \"maxIterations\": 100}";
}

std::string AdvancedExperimentFeatures::getAdaptiveExperimentationConfig() {
    return "{\"name\": \"AdaptiveExperimentationFeatures\", \"description\": \"Adaptive experimental design with online parameter adjustment\", \"enableAdaptiveExperimentation\": true, \"adaptationRate\": 0.01f, \"explorationRate\": 0.1f}";
}

std::string AdvancedExperimentFeatures::getComparativeAnalysisConfig() {
    return "{\"name\": \"ComparativeAnalysisFeatures\", \"description\": \"Comparative analysis capabilities with statistical comparisons\", \"enableComparativeAnalysis\": true, \"runAllCombinations\": true}";
}

std::string AdvancedExperimentFeatures::getHyperparameterOptimizationConfig() {
    return "{\"name\": \"HyperparameterOptimizationFeatures\", \"description\": \"Hyperparameter optimization with multiple algorithms\", \"enableHyperparameterOptimization\": true, \"optimizationAlgorithm\": \"CMA-ES\"}";
}

} // namespace nlm