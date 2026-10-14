#include "AdvancedConfig.hpp"
#include <iostream>

namespace nlm {

void AdvancedConfig::applyToConfig(Config& config) const {
    // Performance settings
    config.set("performance.num_threads", performanceSettings_.numThreads);
    config.set("performance.enable_simd", performanceSettings_.enableSIMD);
    config.set("performance.enable_parallel_processing", performanceSettings_.enableParallelProcessing);
    config.set("performance.memory_pool_size", performanceSettings_.memoryPoolSize);
    config.set("performance.enable_memory_pools", performanceSettings_.enableMemoryPools);
    config.set("performance.max_events", performanceSettings_.maxEvents);
    config.set("performance.enable_event_cache", performanceSettings_.enableEventCache);
    config.set("performance.cache_size", performanceSettings_.cacheSize);
    config.set("performance.enable_lazy_evaluation", performanceSettings_.enableLazyEvaluation);
    config.set("performance.enable_delta_compression", performanceSettings_.enableDeltaCompression);
    
    // Plasticity settings
    config.set("plasticity.stdp.learning_rate", plasticitySettings_.stdpLearningRate);
    config.set("plasticity.stdp.time_constant", plasticitySettings_.stdpTimeConstant);
    config.set("plasticity.stdp.ltp_weight", plasticitySettings_.stdpLTPWeight);
    config.set("plasticity.stdp.ltd_weight", plasticitySettings_.stdpLTDWeight);
    config.set("plasticity.hebbian.threshold", plasticitySettings_.hebbianThreshold);
    config.set("plasticity.hebbian.decay_rate", plasticitySettings_.hebbianDecayRate);
    config.set("plasticity.structural.synaptogenesis_rate", plasticitySettings_.synaptogenesisRate);
    config.set("plasticity.structural.pruning_rate", plasticitySettings_.pruningRate);
    config.set("plasticity.structural.max_synapses_per_neuron", plasticitySettings_.maxSynapsesPerNeuron);
    config.set("plasticity.neuromodulation.dopamine_modulator", plasticitySettings_.dopaminePlasticityModulator);
    config.set("plasticity.neuromodulation.curiosity_modulator", plasticitySettings_.curiosityPlasticityModulator);
    
    // Memory settings
    config.set("memory.working.capacity", memorySettings_.workingMemoryCapacity);
    config.set("memory.working.decay_rate", memorySettings_.workingMemoryDecayRate);
    config.set("memory.working.enable_compression", memorySettings_.enableWorkingMemoryCompression);
    config.set("memory.episodic.max_episodes", memorySettings_.maxEpisodicEpisodes);
    config.set("memory.episodic.consolidation_threshold", memorySettings_.consolidationThreshold);
    config.set("memory.episodic.replay_interval", memorySettings_.replayInterval);
    config.set("memory.associative.size", memorySettings_.associativeMemorySize);
    config.set("memory.associative.decay_rate", memorySettings_.associationDecayRate);
    config.set("memory.enable_pooling", memorySettings_.enableMemoryPooling);
    config.set("memory.enable_compression", memorySettings_.enableMemoryCompression);
    
    // Visualization settings
    config.set("visualization.default_format", visualizationSettings_.defaultOutputFormat);
    config.set("visualization.enable_progress_reporting", visualizationSettings_.enableProgressReporting);
    config.set("visualization.enable_detailed_logging", visualizationSettings_.enableDetailedLogging);
    config.set("visualization.enable_visual_plots", visualizationSettings_.enableVisualPlots);
    config.set("visualization.plot_directory", visualizationSettings_.plotDirectory);
    config.set("visualization.generate_statistics", visualizationSettings_.generateStatistics);
    config.set("visualization.enable_debug_mode", visualizationSettings_.enableDebugMode);
    config.set("visualization.debug_output_path", visualizationSettings_.debugOutputPath);
    config.set("visualization.track_performance_metrics", visualizationSettings_.trackPerformanceMetrics);
    
    // Export/Import settings
    config.set("export.enable_json", exportImportSettings_.enableJSONExport);
    config.set("export.enable_binary", exportImportSettings_.enableBinaryExport);
    config.set("export.enable_text", exportImportSettings_.enableTextExport);
    config.set("export.enable_compression", exportImportSettings_.enableCompression);
    config.set("export.compression_algorithm", exportImportSettings_.compressionAlgorithm);
    config.set("export.compression_level", exportImportSettings_.compressionLevel);
    config.set("import.validate_on_import", exportImportSettings_.validateOnImport);
    config.set("import.verify_checksums", exportImportSettings_.verifyChecksums);
    
    // Network topology settings
    config.set("network.default_region_count", networkTopologySettings_.defaultRegionCount);
    config.set("network.min_neurons_per_region", networkTopologySettings_.minNeuronsPerRegion);
    config.set("network.max_neurons_per_region", networkTopologySettings_.maxNeuronsPerRegion);
    config.set("network.max_populations_per_region", networkTopologySettings_.maxPopulationsPerRegion);
    config.set("network.enable_population_balancing", networkTopologySettings_.enablePopulationBalancing);
    config.set("network.default_connection_probability", networkTopologySettings_.defaultConnectionProbability);
    config.set("network.enable_small_world_topology", networkTopologySettings_.enableSmallWorldTopology);
    config.set("network.small_world_clustering", networkTopologySettings_.smallWorldClustering);
    config.set("network.small_world_path_length", networkTopologySettings_.smallWorldPathLength);
    
    // Learning settings
    config.set("learning.developmental_stage_duration", learningSettings_.developmentalStageDuration);
    config.set("learning.enable_neuromodulation", learningSettings_.enableNeuromodulation);
    config.set("learning.enable_curiosity_driven_learning", learningSettings_.enableCuriosityDrivenLearning);
    config.set("learning.enable_reward_based_learning", learningSettings_.enableRewardBasedLearning);
    config.set("learning.enable_structural_adaptation", learningSettings_.enableStructuralAdaptation);
    config.set("learning.enable_functional_adaptation", learningSettings_.enableFunctionalAdaptation);
    config.set("learning.adaptation_rate", learningSettings_.adaptationRate);
    config.set("learning.enable_stdp", learningSettings_.enableSTDP);
    config.set("learning.enable_hebbian", learningSettings_.enableHebbian);
    config.set("learning.enable_random", learningSettings_.enableRandom);
    config.set("learning.random_learning_rate", learningSettings_.randomLearningRate);
}

AdvancedConfig AdvancedConfig::createDefault() {
    AdvancedConfig config;
    // Set default values for all settings
    return config;
}

AdvancedConfig AdvancedConfig::createOptimized() {
    AdvancedConfig config;
    // Performance optimizations
    config.getPerformanceSettings().enableSIMD = true;
    config.getPerformanceSettings().enableMemoryPools = true;
    config.getPerformanceSettings().enableEventCache = true;
    config.getPerformanceSettings().numThreads = 4;
    
    // Advanced plasticity
    config.getPlasticitySettings().stdpLearningRate = 0.02f;
    config.getPlasticitySettings().stdpTimeConstant = 15.0f;
    config.getPlasticitySettings().dopaminePlasticityModulator = 1.5f;
    
    // Memory optimization
    config.getMemorySettings().enableWorkingMemoryCompression = true;
    config.getMemorySettings().enableMemoryPooling = true;
    config.getMemorySettings().maxEpisodicEpisodes = 2000;
    
    // Learning optimization
    config.getLearningSettings().adaptationRate = 0.02f;
    config.getLearningSettings().enableCuriosityDrivenLearning = true;
    
    return config;
}

} // namespace nlm