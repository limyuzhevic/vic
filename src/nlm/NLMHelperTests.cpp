//
// Test suite for NLM Helper Classes
//
// This file contains comprehensive tests for the NLM helper classes,
// demonstrating their usage and verifying correctness.
//

#include "nlm/NLMHelpers.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace nlm {

class NLMHelperTests {
public:
    static void runAllTests() {
        std::cout << "Running NLM Helper Class Tests..." << std::endl;
        
        testBrainInitializationHelper();
        testNeuronSearchHelper();
        testMemorySystemInitializer();
        testErrorLoggerHelper();
        testCompetitionHelper();
        testSimilarityCalculatorHelper();
        testResourceCleanupHelper();
        
        std::cout << "All tests passed!" << std::endl;
    }
    
private:
    static void testBrainInitializationHelper() {
        std::cout << "  Testing BrainInitializationHelper..." << std::endl;
        
        BrainInitializationHelper<MockBrainComponent> helper;
        
        // Test simple initialization
        MockBrainComponent* component = nullptr;
        helper.initializeBrainPtr(component, "testComponent");
        assert(component == nullptr);
        
        // Test validation
        assert(helper.validate(nullptr, "nullComponent", false) == false);
        assert(helper.validate(nullptr, "nullComponent", true) == true); // Should abort
        
        // Test cleanup
        MockBrainComponent* allocated = new MockBrainComponent();
        helper.cleanup(allocated);
        assert(allocated == nullptr);
        
        std::cout << "    BrainInitializationHelper tests passed!" << std::endl;
    }
    
    static void testNeuronSearchHelper() {
        std::cout << "  Testing NeuronSearchHelper..." << std::endl;
        
        NeuronSearchHelper::NeuronCacheEntry entry1 {
            1u, NeuronType::Sensory, 1u, 0.5f, 0u, {0.1f, 0.2f, 0.3f}, 1u
        };
        
        NeuronSearchHelper::NeuronCacheEntry entry2 {
            2u, NeuronType::Internal, 2u, 0.8f, 1u, {0.4f, 0.5f, 0.6f}, 2u
        };
        
        NeuronSearchHelper::NeuronCache& cache = NeuronSearchHelper::getCache();
        cache.insertNeuron(entry1);
        cache.insertNeuron(entry2);
        
        // Test getNeuron
        const NeuronSearchHelper::NeuronCacheEntry* found = cache.getNeuron(1u);
        assert(found != nullptr);
        assert(found->id == 1u);
        assert(found->type == NeuronType::Sensory);
        
        // Test getNeuronsByType
        auto sensoryNeurons = cache.getNeuronsByType(NeuronType::Sensory);
        assert(sensoryNeurons.size() == 1);
        assert(sensoryNeurons[0] == 1u);
        
        // Test search with filter
        auto filtered = cache.search([](const NeuronSearchHelper::NeuronCacheEntry& e) {
            return e.activation > 0.5f;
        });
        assert(filtered.size() == 1);
        assert(filtered[0] == 2u);
        
        cache.clear();
        std::cout << "    NeuronSearchHelper tests passed!" << std::endl;
    }
    
    static void testMemorySystemInitializer() {
        std::cout << "  Testing MemorySystemInitializer..." << std::endl;
        
        // This would require actual MemorySystem types, so we'll just test the interface
        // In a real implementation, you would test with concrete memory system types
        std::cout << "    MemorySystemInitializer tests skipped (requires concrete types)!" << std::endl;
    }
    
    static void testErrorLoggerHelper() {
        std::cout << "  Testing ErrorLoggerHelper..." << std::endl;
        
        ErrorLoggerHelper::ErrorLogger& logger = ErrorLoggerHelper::getLogger();
        
        // Clear any existing errors
        logger.clear();
        
        // Test simple error logging
        logger.logError(ErrorCode::NullPointer,
                       "Null pointer access to: testComponent",
                       "test_file.cpp", 123, "testFunction");
        
        // Test error logging with metadata
        std::unordered_map<std::string, std::string> metadata {
            { "operation", "initialize" },
            { "component", "test" },
            { "step", "1" }
        };
        
        logger.logErrorWithMetadata(ErrorCode::SystemError,
                                   "System error occurred",
                                   "test_file.cpp", 456, "anotherFunction",
                                   metadata);
        
        // Test getRecentErrors
        auto recent = logger.getRecentErrors(5);
        assert(recent.size() == 2);
        
        // Test getErrorsByCode
        auto nullPointerErrors = logger.getErrorsByCode(ErrorCode::NullPointer);
        assert(nullPointerErrors.size() == 1);
        
        // Test error statistics
        auto stats = logger.getStats();
        assert(stats.totalErrors == 2);
        assert(stats.errorsByCode.find(ErrorCode::NullPointer) != stats.errorsByCode.end());
        
        logger.clear();
        assert(logger.getStats().totalErrors == 0);
        
        std::cout << "    ErrorLoggerHelper tests passed!" << std::endl;
    }
    
    static void testCompetitionHelper() {
        std::cout << "  Testing CompetitionHelper..." << std::endl;
        
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
        
        std::vector<NeuronId> competitors = {1, 2, 3, 4, 5};
        std::vector<float> scores = {0.8f, 0.3f, 0.9f, 0.4f, 0.6f};
        
        // Test basic competition
        auto result = CompetitionHelper::runCompetition(competitors, scores, config);
        assert(result.winners.size() == 2);  // 0.9 and 0.8 are highest
        assert(result.threshold > 0.0f);
        assert(result.inhibitionLevels.size() == competitors.size());
        assert(result.excitationLevels.size() == competitors.size());
        
        // Test tournament competition
        auto tournamentResult = CompetitionHelper::tournamentCompetition(
            competitors, scores, 3, config);
        assert(tournamentResult.winners.size() <= 2);
        
        // Test round-robin competition
        auto roundRobinResult = CompetitionHelper::roundRobinCompetition(
            competitors, scores, config);
        assert(roundRobinResult.winners.size() == 1);
        assert(roundRobinResult.winners[0] == 3u);  // Highest score
        
        std::cout << "    CompetitionHelper tests passed!" << std::endl;
    }
    
    static void testSimilarityCalculatorHelper() {
        std::cout << "  Testing SimilarityCalculatorHelper..." << std::endl;
        
        std::vector<float> vectorA = {0.1f, 0.5f, 0.3f, 0.9f};
        std::vector<float> vectorB = {0.2f, 0.6f, 0.2f, 0.8f};
        std::vector<float> vectorC = {1.0f, 0.0f, 0.0f, 0.0f};
        
        // Test cosine similarity
        float cosineSim = SimilarityCalculatorHelper::calculateSimilarity(
            vectorA, vectorB, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
        assert(cosineSim >= 0.0f && cosineSim <= 1.0f);
        
        // Test Euclidean similarity
        float euclideanSim = SimilarityCalculatorHelper::calculateSimilarity(
            vectorA, vectorB, SimilarityCalculatorHelper::SimilarityMetric::Euclidean);
        assert(euclideanSim >= 0.0f && euclideanSim <= 1.0f);
        
        // Test with cache
        float cachedCosineSim = SimilarityCalculatorHelper::calculateSimilarityWithCache(
            vectorA, vectorB, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
        assert(std::abs(cosineSim - cachedCosineSim) < 0.001f);
        
        // Test batch similarity
        std::vector<std::vector<float>> vectors = {vectorA, vectorB, vectorC};
        std::vector<float> similarities = SimilarityCalculatorHelper::batchSimilarity(
            vectors, vectorA, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
        assert(similarities.size() == 3);
        
        // Test findSimilarVectors
        std::vector<size_t> similar = SimilarityCalculatorHelper::findSimilarVectors(
            vectors, vectorA, 0.8f, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
        // vectorA vs vectorC should be very similar (1.0)
        assert(std::find(similar.begin(), similar.end(), 2u) != similar.end());
        
        // Test isSimilar
        assert(SimilarityCalculatorHelper::isSimilar(vectorA, vectorC, 0.9f,
                                                    SimilarityCalculatorHelper::SimilarityMetric::Cosine));
        
        SimilarityCalculatorHelper::getCache().clear();
        std::cout << "    SimilarityCalculatorHelper tests passed!" << std::endl;
    }
    
    static void testResourceCleanupHelper() {
        std::cout << "  Testing ResourceCleanupHelper..." << std::endl;
        
        MockResource* resource = new MockResource();
        
        // Test SmartResource
        {
            ResourceCleanupHelper::SmartResource<MockResource> smartResource(
                resource, &ResourceCleanupHelper::deleteResource);
            
            assert(smartResource.get() == resource);
            assert(!smartResource == false); // Not nullptr
            
            MockResource* released = smartResource.release();
            assert(released == resource);
            assert(smartResource.get() == nullptr);
            
            // Cleanup manually
            if (released) ResourceCleanupHelper::deleteResource(released);
        }
        
        resource = new MockResource();
        
        // Test ResourceGuard
        {
            ResourceCleanupHelper::ResourceGuard<MockResource> guard(
                resource,
                [](MockResource* ptr) { 
                    if (ptr) { ptr->cleanup(); delete ptr; } 
                },
                "testResource"
            );
            
            assert(guard.isValid());
            assert(guard.get() == resource);
        }
        
        // Resource should have been cleaned up when guard went out of scope
        assert(MockResource::cleanupCount == 1);
        
        std::cout << "    ResourceCleanupHelper tests passed!" << std::endl;
    }
    
    // Helper classes for testing
    class MockBrainComponent {
    public:
        MockBrainComponent() = default;
        ~MockBrainComponent() = default;
    };
    
    class MockResource {
    public:
        static int cleanupCount;
        
        MockResource() = default;
        ~MockResource() = default;
        
        void cleanup() {
            MockResource::cleanupCount++;
        }
    };
    
    int MockResource::cleanupCount = 0;
};

} // namespace nlm
