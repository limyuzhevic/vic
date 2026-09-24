// Phase 6 Integration Configuration Structure
// Advanced configuration options for the complete integrated brain

#pragma once

namespace nlm {

struct Phase6Config {
    // Basic configuration
    size_t neuronCount = 1000;              // Number of neurons
    size_t maxSteps = 3000;                // Maximum simulation steps
    double timestep = 0.001;              // Simulation timestep (s)
    
    // Memory configuration
    bool enableWorkingMemory = true;       // Enable working memory integration
    bool enableEpisodicMemory = true;      // Enable episodic memory storage
    bool enableAssociativeMemory = true;   // Enable associative memory
    size_t maxEpisodicEpisodes = 1000;     // Maximum episodic memory episodes
    
    // Prediction system configuration
    bool enableTemporalPrediction = true;   // Enable temporal sequence prediction
    bool enableActionConsequencePrediction = true;  // Enable action-consequence prediction
    size_t predictionHorizon = 10;         // Number of steps to predict ahead
    size_t sequenceMemorySize = 100;       // Size of temporal sequence memory
    
    // Cognitive system configuration
    bool enableAttention = true;           // Enable attentional selection
    bool enablePlanning = true;            // Enable action planning
    bool enableConceptFormation = true;    // Enable concept formation
    bool enableSelfModel = false;          // Enable self-model (not in current Phase 6)
    bool enableSocialLearning = false;     // Enable social learning (not in current Phase 6)
    
    // Neuromodulation configuration
    bool enableDopamine = true;            // Enable dopamine signaling
    bool enableCuriosity = true;           // Enable curiosity-driven exploration
    bool enableNovelty = true;             // Enable novelty detection
    bool enablePredictionError = true;     // Enable prediction error signaling
    
    // Development configuration
    bool enableDevelopment = true;         // Enable developmental stages
    bool enableSynaptogenesis = true;      // Enable new synapse formation
    bool enablePruning = true;             // Enable synapse pruning
    bool enableMaturation = true;          // Enable neural maturation
    
    // Persistence configuration
    bool enableCheckpointing = true;       // Enable checkpoint save/load
    bool enableReplay = true;              // Enable memory replay for consolidation
    std::string checkpointDir = "./checkpoints";  // Checkpoint directory
    std::string checkpointFile = "";       // Specific checkpoint file to load/save
    size_t maxCheckpoints = 10;            // Maximum number of checkpoints to keep
    
    // Advanced experimental controls
    bool enableAblationStudies = false;    // Enable ablation studies (systematic removal)
    bool enableParameterSweep = false;     // Enable parameter sweep experiments
    bool enableComparativeStudies = false;  // Enable comparative studies across configurations
    
    // Performance and debugging
    bool enableProfiling = false;          // Enable performance profiling
    bool enableDetailedLogging = false;    // Enable detailed logging
    bool enableValidation = false;         // Enable system validation tests
    
    // Output and visualization
    std::string outputFormat = "text";    // Output format: "text", "json", "csv"
    std::string outputFile = "";           // Output file path
    bool enableRealTimeVisualization = false;  // Enable real-time visualization (requires GUI)
    
    // Experiment controls
    bool runIntegrationVerification = true;  // Run quick integration verification
    bool runSystemValidation = false;       // Run comprehensive system validation
    bool runPerformanceBenchmark = false;  // Run performance benchmark
    
    // Convenience methods
    void enableAllSystems() {
        enableWorkingMemory = true;
        enableEpisodicMemory = true;
        enableAssociativeMemory = true;
        enableTemporalPrediction = true;
        enableActionConsequencePrediction = true;
        enableAttention = true;
        enablePlanning = true;
        enableConceptFormation = true;
        enableDopamine = true;
        enableCuriosity = true;
        enableNovelty = true;
        enablePredictionError = true;
        enableDevelopment = true;
        enableSynaptogenesis = true;
        enablePruning = true;
        enableMaturation = true;
        enableCheckpointing = true;
        enableReplay = true;
    }
    
    void disableAllSystems() {
        enableWorkingMemory = false;
        enableEpisodicMemory = false;
        enableAssociativeMemory = false;
        enableTemporalPrediction = false;
        enableActionConsequencePrediction = false;
        enableAttention = false;
        enablePlanning = false;
        enableConceptFormation = false;
        enableDopamine = false;
        enableCuriosity = false;
        enableNovelty = false;
        enablePredictionError = false;
        enableDevelopment = false;
        enableSynaptogenesis = false;
        enablePruning = false;
        enableMaturation = false;
        enableCheckpointing = false;
        enableReplay = false;
    }
    
    void enableMemorySystems() {
        enableWorkingMemory = true;
        enableEpisodicMemory = true;
        enableAssociativeMemory = true;
    }
    
    void enableCognitiveSystems() {
        enableAttention = true;
        enablePlanning = true;
        enableConceptFormation = true;
    }
    
    void enableNeuromodulationSystems() {
        enableDopamine = true;
        enableCuriosity = true;
        enableNovelty = true;
        enablePredictionError = true;
    }
    
    void enableDevelopmentSystems() {
        enableDevelopment = true;
        enableSynaptogenesis = true;
        enablePruning = true;
        enableMaturation = true;
    }
    
    void enablePersistenceSystems() {
        enableCheckpointing = true;
        enableReplay = true;
    }
    
    // Validate configuration
    bool isValid() const {
        if (neuronCount == 0) return false;
        if (maxSteps == 0) return false;
        if (timestep <= 0.0) return false;
        if (maxEpisodicEpisodes == 0) return false;
        if (predictionHorizon == 0) return false;
        if (sequenceMemorySize == 0) return false;
        return true;
    }
    
    // Get configuration summary
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Phase 6 Configuration Summary:" << std::endl;
        ss << "  Neurons: " << neuronCount << std::endl;
        ss << "  Max steps: " << maxSteps << std::endl;
        ss << "  Timestep: " << timestep << "s" << std::endl;
        ss << "  Memory: " << (enableWorkingMemory ? "Enabled" : "Disabled") << ", "
           << (enableEpisodicMemory ? "Enabled" : "Disabled") << ", "
           << (enableAssociativeMemory ? "Enabled" : "Disabled") << std::endl;
        ss << "  Prediction: " << (enableTemporalPrediction ? "Enabled" : "Disabled") << ", "
           << (enableActionConsequencePrediction ? "Enabled" : "Disabled") << std::endl;
        ss << "  Cognition: " << (enableAttention ? "Attention" : "---") << ", "
           << (enablePlanning ? "Planning" : "---") << ", "
           << (enableConceptFormation ? "Concept Formation" : "---") << std::endl;
        ss << "  Neuromodulation: " << (enableDopamine ? "DA" : "---") << ", "
           << (enableCuriosity ? "Curiosity" : "---") << ", "
           << (enableNovelty ? "Novelty" : "---") << std::endl;
        ss << "  Development: " << (enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        ss << "  Persistence: " << (enableCheckpointing ? "Checkpointing" : "---") << ", "
           << (enableReplay ? "Replay" : "---") << std::endl;
        
        return ss.str();
    }
};

} // namespace nlm
