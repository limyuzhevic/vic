//
// Example: Refactoring Brain.cpp to use NLM Helper Classes
//
// This file demonstrates how to apply the NLM helper classes to refactor
// common code patterns in the Brain.cpp file.
//

#include "Brain.hpp"
#include "nlm/NLMHelpers.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

namespace nlm {
    
    // Example: Refactor Brain initialization to use BrainInitializationHelper
    class BrainRefactored {
    public:
        BrainRefactored(std::shared_ptr<Config> config) : config_(config) {
            // Use BrainInitializationHelper for safe pointer initialization
            auto& initHelper = getInitializationHelper();
            
            // Initialize spike system
            if (!initHelper.initializeWithErrorCheck(spikeSystem_, "spikeSystem")) {
                throw std::runtime_error("Failed to initialize spike system");
            }
            
            // Initialize other systems...
        }
        
        // Example of using NeuronSearchHelper
        void registerNeuron(Neuron* neuron) {
            auto& searchHelper = getNeuronSearchHelper();
            
            NeuronSearchHelper::NeuronCacheEntry entry {
                neuron->getId(),
                neuron->getType(),
                neuron->getRegionId(),
                neuron->getState().membranePotential,
                neuron->getPopulationId(),
                std::vector<float>(), // Pattern would be populated from neuron
                0 // Last accessed step
            };
            
            searchHelper.getCache().insertNeuron(entry);
        }
        
        // Example of using MemorySystemInitializer
        bool initializeMemorySystems() {
            MemorySystemInitializer<NeuralWorkingMemory>::MemoryConfig config {
                maxCapacity: 1000,
                decayRate: 0.01f,
                autoCleanup: true,
                name: "workingMemory",
                parameters: {{ "pattern_size", "256" }}
            };
            
            auto& initHelper = getMemorySystemInitializer<NeuralWorkingMemory>();
            
            return initHelper.initialize(workingMemory_, getBrain(), config);
        }
        
        // Example of using CompetitionHelper
        std::vector<NeuronId> runNeutronCompetition(const std::vector<NeuronId>& competitors,
                                                 const std::vector<float>& scores) {
            CompetitionHelper::CompetitionConfig config {
                inhibitionStrength: 0.5f,
                excitationStrength: 1.5f,
                globalInhibition: 0.3f,
                thresholdMultiplier: 1.0f,
                maxWinners: 3,
                normalizeScores: true,
                applyInhibition: true,
                applyExcitation: true
            };
            
            auto& competitionHelper = getCompetitionHelper();
            auto result = competitionHelper.runCompetition(competitors, scores, config);
            
            return result.winners;
        }
        
        // Example of using SimilarityCalculatorHelper
        float calculateNeuronSimilarity(const Neuron* neuron1, const Neuron* neuron2) {
            std::vector<float> pattern1 = extractPatternFromNeuron(neuron1);
            std::vector<float> pattern2 = extractPatternFromNeuron(neuron2);
            
            auto& similarityHelper = getSimilarityCalculatorHelper();
            
            // Use cached similarity calculation
            return similarityHelper.calculateSimilarityWithCache(
                pattern1, pattern2, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
        }
        
        // Example of using ResourceCleanupHelper
        void cleanupResources() {
            auto& cleanupHelper = getResourceCleanupHelper();
            
            // Use ResourceGuard for automatic cleanup
            auto guard = cleanupHelper.createGuard(
                spikeSystem_.get(),
                [](void* ptr) { delete static_cast<SpikeSystem*>(ptr); },
                "spikeSystem"
            );
            
            // Cleanup will happen automatically when guard goes out of scope
        }
        
    private:
        // Static helper instances
        static BrainInitializationHelper<SpikeSystem>& getInitializationHelper() {
            static BrainInitializationHelper<SpikeSystem> helper;
            return helper;
        }
        
        static NeuronSearchHelper& getNeuronSearchHelper() {
            static NeuronSearchHelper helper;
            return helper;
        }
        
        template<typename MemorySystem>
        static MemorySystemInitializer<MemorySystem>& getMemorySystemInitializer() {
            static MemorySystemInitializer<MemorySystem> helper;
            return helper;
        }
        
        static CompetitionHelper& getCompetitionHelper() {
            static CompetitionHelper helper;
            return helper;
        }
        
        static SimilarityCalculatorHelper& getSimilarityCalculatorHelper() {
            static SimilarityCalculatorHelper helper;
            return helper;
        }
        
        static ResourceCleanupHelper& getResourceCleanupHelper() {
            static ResourceCleanupHelper helper;
            return helper;
        }
        
        std::shared_ptr<Config> config_;
        SpikeSystem* spikeSystem_ = nullptr;
        NeuralWorkingMemory* workingMemory_ = nullptr;
        
        Brain* getBrain() { return reinterpret_cast<Brain*>(this); }
        
        std::vector<float> extractPatternFromNeuron(const Neuron* neuron) {
            // Extract pattern from neuron for similarity calculation
            std::vector<float> pattern(256, 0.0f); // 256-dimensional pattern
            
            // Populate pattern from neuron state...
            return pattern;
        }
    };

    // Example: Refactor common error logging patterns
    void refactorErrorLogging() {
        auto& errorLogger = ErrorLoggerHelper::getLogger();
        
        // Simple error logging
        errorLogger.logError(ErrorCode::NullPointer, 
                           "Null pointer access to: spikeSystem",
                           __FILE__, __LINE__, __func__);
        
        // Error logging with metadata
        std::unordered_map<std::string, std::string> metadata {
            { "operation", "initialize" },
            { "component", "spikeSystem" },
            { "step", "1" }
        };
        
        errorLogger.logErrorWithMetadata(ErrorCode::SystemError,
                                        "Failed to initialize system",
                                        __FILE__, __LINE__, __func__,
                                        metadata);
        
        // Get error statistics
        auto stats = errorLogger.getStats();
        // Use stats for error analysis...
    }

    // Example: Refactor resource cleanup patterns
    void refactorResourceCleanup() {
        auto& cleanupHelper = ResourceCleanupHelper::getInstance();
        
        // Using ResourceGuard for automatic cleanup
        auto guard = cleanupHelper.createGuard(
            new SpikeSystem(),
            [](SpikeSystem* ptr) { delete ptr; },
            "temporarySpikeSystem"
        );
        
        // Do work with the resource...
        
        // Resource automatically cleaned up when guard goes out of scope
    }

    // Example: Refactor similarity calculation patterns
    void refactorSimilarityCalculation() {
        auto& similarityHelper = SimilarityCalculatorHelper::getInstance();
        
        std::vector<float> pattern1 = {0.1f, 0.5f, 0.3f, 0.9f};
        std::vector<float> pattern2 = {0.2f, 0.6f, 0.2f, 0.8f};
        
        // Calculate similarity with caching
        float cosineSim = similarityHelper.calculateSimilarityWithCache(
            pattern1, pattern2, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
        
        float euclideanSim = similarityHelper.calculateSimilarityWithCache(
            pattern1, pattern2, SimilarityCalculatorHelper::SimilarityMetric::Euclidean);
        
        // Batch similarity calculation
        std::vector<std::vector<float>> patterns = {pattern1, pattern2, {0.3f, 0.4f, 0.5f, 0.6f}};
        std::vector<float> similarities = similarityHelper.batchSimilarity(patterns, pattern1);
        
        // Find similar patterns
        std::vector<size_t> similar = similarityHelper.findSimilarVectors(
            patterns, pattern1, 0.8f);
    }

    // Example: Refactor competition patterns
    void refactorCompetitionPatterns() {
        auto& competitionHelper = CompetitionHelper::getInstance();
        
        std::vector<NeuronId> competitors = {1, 2, 3, 4, 5};
        std::vector<float> scores = {0.8f, 0.3f, 0.9f, 0.4f, 0.6f};
        
        CompetitionHelper::CompetitionConfig config {
            inhibitionStrength: 0.5f,
            excitationStrength: 1.5f,
            globalInhibition: 0.3f,
            thresholdMultiplier: 1.0f,
            maxWinners: 2,
            normalizeScores: true,
            applyInhibition: true,
            applyExcitation: true
        };
        
        // Run tournament competition
        auto tournamentResult = competitionHelper.tournamentCompetition(
            competitors, scores, 3, config);
        
        // Run round-robin competition
        auto roundRobinResult = competitionHelper.roundRobinCompetition(
            competitors, scores, config);
    }

    // Example: Refactor memory system initialization
    void refactorMemorySystemInitialization() {
        auto& memoryInitializer = MemorySystemInitializer<NeuralWorkingMemory>::getInstance();
        
        // Create initialization context
        MemorySystemInitializer<NeuralWorkingMemory>::MemoryConfig config {
            maxCapacity: 1000,
            decayRate: 0.01f,
            autoCleanup: true,
            name: "workingMemory",
            parameters: {{ "pattern_size", "256" }}
        };
        
        MemorySystemInitializer<NeuralWorkingMemory>::InitializationContext context(
            getBrainPtr(), config);
        
        // Initialize memory system with validation
        if (memoryInitializer.initialize(workingMemory_, getBrainPtr(), config)) {
            if (memoryInitializer.validate(workingMemory_, context)) {
                // Memory system successfully initialized
            }
        }
    }

} // namespace nlm
