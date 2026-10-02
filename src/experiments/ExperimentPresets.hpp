#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace nlm {

/**
 * Experiment Preset System
 * 
 * Centralized configuration system for NLM experiments with predefined sets of parameters
 * for different research questions, performance requirements, and computational resources.
 * 
 * Presets are organized into categories:
 * - Research presets: Optimized for specific scientific questions
 * - Performance presets: Optimized for different computational scales
 * - Research question presets: Tailored for specific experimental designs
 */
class ExperimentPresets {
public:
    ExperimentPresets();
    ~ExperimentPresets();
    
    /**
     * Get all available preset names
     */
    static std::vector<std::string> getAllPresetNames() const;
    
    /**
     * Get all preset categories
     */
    static std::vector<std::string> getPresetCategories() const;
    
    /**
     * Get all presets in a category
     */
    static std::vector<std::string> getPresetsInCategory(const std::string& category) const;
    
    /**
     * Get preset by category and name
     */
    static std::string getPresetJson(const std::string& category, const std::string& name);
    
    /**
     * Get preset configuration as string
     */
    static std::string getPresetConfig(const std::string& presetName);
    
    /**
     * Get research presets
     */
    static std::string getResearchPresets();
    
    /**
     * Get performance presets
     */
    static std::string getPerformancePresets();
    
    /**
     * Get research question presets
     */
    static std::string getResearchQuestionPresets();
    
    /**
     * Get comparative experiment configurations
     */
    static std::string getComparativeExperimentConfigs();
    
    /**
     * Get parameter optimization sets
     */
    static std::string getParameterOptimizationSets();
    
    /**
     * Get scalability presets
     */
    static std::string getScalabilityPresets();
    
    /**
     * Get advanced experiment features
     */
    static std::string getAdvancedExperimentFeatures();
    
    /**
     * Save presets to file
     */
    static bool savePresets(const std::string& filepath);
    
    /**
     * Load presets from file
     */
    static bool loadPresets(const std::string& filepath);
    
    /**
     * Get preset description
     */
    static std::string getPresetDescription(const std::string& presetName);
    
    /**
     * Get preset validation
     */
    static bool validatePreset(const std::string& presetName);
    
    /**
     * Get preset dependencies
     */
    static std::vector<std::string> getPresetDependencies(const std::string& presetName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    static void initializePresets();
    static std::string generateResearchPresetsJson();
    static std::string generatePerformancePresetsJson();
    static std::string generateResearchQuestionPresetsJson();
    static std::string generateComparativeExperimentConfigsJson();
    static std::string generateParameterOptimizationSetsJson();
    static std::string generateScalabilityPresetsJson();
    static std::string generateAdvancedExperimentFeaturesJson();
    static std::string createPresetJson(const std::string& name, const std::string& category,
                                         const std::string& description, const std::string& config);
    
    // Preset data structure
    struct Preset {
        std::string name;
        std::string category;
        std::string description;
        std::string configJson;
        std::vector<std::string> dependencies;
        bool validated;
    };
    
    static std::vector<Preset> presets_;
    static bool initialized_;
};

/**
 * Research preset configurations
 * Optimized for different scientific research objectives
 */
namespace ResearchPresets {
    
    /**
     * Scientific rigor preset
     * Maximizes experimental control and measurement precision
     */
    static const std::string Scientific = "ResearchScientific";
    
    /**
     * Speed optimization preset
     * Minimizes experiment duration while maintaining quality
     */
    static const std::string Speed = "ResearchSpeed";
    
    /**
     * Reproducibility preset
     * Maximizes experimental reproducibility and consistency
     */
    static const std::string Reproducibility = "ResearchReproducibility";
    
    /**
     * Get scientific preset configuration
     */
    static std::string getScientificConfig();
    
    /**
     * Get speed preset configuration
     */
    static std::string getSpeedConfig();
    
    /**
     * Get reproducibility preset configuration
     */
    static std::string getReproducibilityConfig();
}

/**
 * Performance preset configurations
 * Optimized for different computational resource levels
 */
namespace PerformancePresets {
    
    /**
     * Small-scale preset
     * 100-500 neurons, suitable for development and testing
     */
    static const std::string Small = "PerformanceSmall";
    
    /**
     * Medium-scale preset
     * 500-1000 neurons, balanced research and performance
     */
    static const std::string Medium = "PerformanceMedium";
    
    /**
     * Large-scale preset
     * 1000+ neurons, research-grade performance
     */
    static const std::string Large = "PerformanceLarge";
    
    /**
     * Get small-scale preset configuration
     */
    static std::string getSmallConfig();
    
    /**
     * Get medium-scale preset configuration
     */
    static std::string getMediumConfig();
    
    /**
     * Get large-scale preset configuration
     */
    static std::string getLargeConfig();
}

/**
 * Research question preset configurations
 * Tailored for specific experimental designs and research questions
 */
namespace ResearchQuestionPresets {
    
    /**
     * Learning curves preset
     * Configure experiments to track learning over time
     */
    static const std::string LearningCurves = "LearningCurves";
    
    /**
     * Memory capacity preset
     * Configure experiments to test memory limits
     */
    static const std::string MemoryCapacity = "MemoryCapacity";
    
    /**
     * Social dynamics preset
     * Configure experiments for social behavior studies
     */
    static const std::string SocialDynamics = "SocialDynamics";
    
    /**
     * Development stages preset
     * Configure experiments for developmental studies
     */
    static const std::string DevelopmentStages = "DevelopmentStages";
    
    /**
     * Get learning curves configuration
     */
    static std::string getLearningCurvesConfig();
    
    /**
     * Get memory capacity configuration
     */
    static std::string getMemoryCapacityConfig();
    
    /**
     * Get social dynamics configuration
     */
    static std::string getSocialDynamicsConfig();
    
    /**
     * Get development stages configuration
     */
    static std::string getDevelopmentStagesConfig();
}

/**
 * Comparative experiment configurations
 * Presets for running comparative analyses across different configurations
 */
namespace ComparativeExperimentPresets {
    
    /**
     * Baseline comparison preset
     * Standard configuration for baseline comparisons
     */
    static const std::string Baseline = "ComparativeBaseline";
    
    /**
     * Parameter sweep comparison preset
     * Optimized for parameter sweep experiments
     */
    static const std::string ParameterSweep = "ComparativeParameterSweep";
    
    /**
     * Multi-objective comparison preset
     * Optimized for multi-objective optimization experiments
     */
    static const std::string MultiObjective = "ComparativeMultiObjective";
    
    /**
     * Get baseline comparison configuration
     */
    static std::string getBaselineConfig();
    
    /**
     * Get parameter sweep comparison configuration
     */
    static std::string getParameterSweepConfig();
    
    /**
     * Get multi-objective comparison configuration
     */
    static std::string getMultiObjectiveConfig();
}

/**
 * Parameter optimization sets
 * Presets for hyperparameter optimization algorithms
 */
namespace ParameterOptimizationSets {
    
    /**
     * Grid search optimization set
     * Systematic parameter grid search
     */
    static const std::string GridSearch = "GridSearch";
    
    /**
     * Random search optimization set
     * Random sampling of parameter space
     */
    static const std::string RandomSearch = "RandomSearch";
    
    /**
     * Bayesian optimization set
     * Model-based parameter optimization
     */
    static const std::string BayesianOptimization = "BayesianOptimization";
    
    /**
     * Genetic algorithm set
     * Evolutionary parameter optimization
     */
    static const std::string GeneticAlgorithm = "GeneticAlgorithm";
    
    /**
     * Get grid search configuration
     */
    static std::string getGridSearchConfig();
    
    /**
     * Get random search configuration
     */
    static std::string getRandomSearchConfig();
    
    /**
     * Get Bayesian optimization configuration
     */
    static std::string getBayesianOptimizationConfig();
    
    /**
     * Get genetic algorithm configuration
     */
    static std::string getGeneticAlgorithmConfig();
}

/**
 * Scalability preset configurations
 * Optimized for different computational scales
 */
namespace ScalabilityPresets {
    
    /**
     * Development scalability preset
     * Optimized for small-scale development and testing
     */
    static const std::string Development = "ScalabilityDevelopment";
    
    /**
     * Research scalability preset
     * Optimized for research-grade experiments
     */
    static const std::string Research = "ScalabilityResearch";
    
    /**
     * Production scalability preset
     * Optimized for large-scale production experiments
     */
    static const std::string Production = "ScalabilityProduction";
    
    /**
     * Get development scalability configuration
     */
    static std::string getDevelopmentConfig();
    
    /**
     * Get research scalability configuration
     */
    static std::string getResearchConfig();
    
    /**
     * Get production scalability configuration
     */
    static std::string getProductionConfig();
}

/**
 * Advanced experiment features
 * Configuration for advanced experimental capabilities
 */
namespace AdvancedExperimentFeatures {
    
    /**
     * Parameter sweep features
     * Configuration for automated parameter sweep capabilities
     */
    static const std::string ParameterSweep = "ParameterSweep";
    
    /**
     * Multi-objective optimization features
     * Configuration for multi-objective optimization capabilities
     */
    static const std::string MultiObjectiveOptimization = "MultiObjectiveOptimization";
    
    /**
     * Adaptive experimentation features
     * Configuration for adaptive experimental design
     */
    static const std::string AdaptiveExperimentation = "AdaptiveExperimentation";
    
    /**
     * Comparative analysis features
     * Configuration for comparative analysis capabilities
     */
    static const std::string ComparativeAnalysis = "ComparativeAnalysis";
    
    /**
     * Hyperparameter optimization features
     * Configuration for hyperparameter optimization capabilities
     */
    static const std::string HyperparameterOptimization = "HyperparameterOptimization";
    
    /**
     * Get parameter sweep configuration
     */
    static std::string getParameterSweepConfig();
    
    /**
     * Get multi-objective optimization configuration
     */
    static std::string getMultiObjectiveOptimizationConfig();
    
    /**
     * Get adaptive experimentation configuration
     */
    static std::string getAdaptiveExperimentationConfig();
    
    /**
     * Get comparative analysis configuration
     */
    static std::string getComparativeAnalysisConfig();
    
    /**
     * Get hyperparameter optimization configuration
     */
    static std::string getHyperparameterOptimizationConfig();
}

} // namespace nlm