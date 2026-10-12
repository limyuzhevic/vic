// NLM Helper Classes Comprehensive Unit Tests
// Phase 3: Advanced NLM Helper Classes Tests

#include "nlm/NLMHelpers.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <thread>
#include <cmath>

namespace nlm {

void runAllTests() {
    std::cout << "=== NLM Helper Classes Tests ===" << std::endl;
    
    std::cout << "Running BrainInitializationHelper tests..." << std::endl;
    testBrainInitializationHelper();
    
    std::cout << "Running NeuronSearchHelper tests..." << std::endl;
    testNeuronSearchHelper();
    
    std::cout << "Running MemorySystemInitializer tests..." << std::endl;
    testMemorySystemInitializer();
    
    std::cout << "Running ErrorLoggerHelper tests..." << std::endl;
    testErrorLoggerHelper();
    
    std::cout << "Running CompetitionHelper tests..." << std::endl;
    testCompetitionHelper();
    
    std::cout << "Running SimilarityCalculatorHelper tests..." << std::endl;
    testSimilarityCalculatorHelper();
    
    std::cout << "Running ResourceCleanupHelper tests..." << std::endl;
    testResourceCleanupHelper();
    
    std::cout << "=== All NLM Helper Classes Tests PASSED ===" << std::endl;
}

namespace test_brain_initialization_helper {

class MockBrainComponent {
public:
    MockBrainComponent() = default;
    ~MockBrainComponent() = default;
    
    bool initialized = false;
};

void testBrainInitializationHelper() {
    std::cout << "  Testing BrainInitializationHelper..." << std::endl;
    
    BrainInitializationHelper<MockBrainComponent> helper;
    
    // Test simple initialization
    MockBrainComponent* component = nullptr;
    helper.initializeBrainPtr(component, "testComponent");
    assert(component == nullptr);
    
    // Test validation
    assert(helper.validate(nullptr, "nullComponent", false) == false);
    // Note: assertOnFail=true would abort, so we skip that test
    
    // Test cleanup
    MockBrainComponent* allocated = new MockBrainComponent();
    helper.cleanup(allocated);
    assert(allocated == nullptr);
    
    // Test safe initialization with error check
    MockBrainComponent* safePtr = nullptr;
    bool initResult = helper.initializeWithErrorCheck(safePtr, "safeComponent", "file.cpp", 123, "testFunc");
    assert(initResult);
    assert(safePtr != nullptr);
    helper.cleanup(safePtr);
    
    std::cout << "    BrainInitializationHelper tests passed!" << std::endl;
}

} // namespace test_brain_initialization_helper

namespace test_neuron_search_helper {

void testNeuronSearchHelper() {
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
    
    // Test getNeuron (O(1) lookup)
    const NeuronSearchHelper::NeuronCacheEntry* found = cache.getNeuron(1u);
    assert(found != nullptr);
    assert(found->id == 1u);
    assert(found->type == NeuronType::Sensory);
    
    // Test getNeuronsByType (O(n) search through cache)
    auto sensoryNeurons = cache.getNeuronsByType(NeuronType::Sensory);
    assert(sensoryNeurons.size() == 1);
    assert(sensoryNeurons[0] == 1u);
    
    // Test search with filter (O(n) search)
    auto filtered = cache.search([](const NeuronSearchHelper::NeuronCacheEntry& e) {
        return e.activation > 0.5f;
    });
    assert(filtered.size() == 1);
    assert(filtered[0] == 2u);
    
    // Test thread safety (basic)
    auto& cache2 = NeuronSearchHelper::getCache();
    cache2.insertNeuron(entry1);
    
    // Clear cache
    cache2.clear();
    
    std::cout << "    NeuronSearchHelper tests passed!" << std::endl;
}

} // namespace test_neuron_search_helper

namespace test_memory_system_initializer {

class MockMemorySystem {
public:
    MockMemorySystem() = default;
    ~MockMemorySystem() = default;
    
    void initialize(void*) {
        initialized_ = true;
    }
    
    void setCapacity(size_t capacity) {
        capacity_ = capacity;
    }
    
    void setDecayRate(float rate) {
        decayRate_ = rate;
    }
    
    size_t getCapacity() const {
        return capacity_;
    }
    
    float getDecayRate() const {
        return decayRate_;
    }
    
    bool getInitialized() const {
        return initialized_;
    }
    
private:
    bool initialized_ = false;
    size_t capacity_ = 0;
    float decayRate_ = 0.0f;
};

void testMemorySystemInitializer() {
    std::cout << "  Testing MemorySystemInitializer..." << std::endl;
    
    MemorySystemInitializer<MockMemorySystem> initializer;
    
    // Test configuration structure
    MemorySystemInitializer<MockMemorySystem>::MemoryConfig config {
        1000,  // maxCapacity
        0.01f, // decayRate
        true,  // autoCleanup
        "testMemory", // name
        {} // parameters
    };
    
    // Test initializer with validation
    MockMemorySystem memorySystem;
    bool initResult = initializer.initialize(memorySystem, nullptr, config, true);
    assert(initResult);
    assert(memorySystem.getInitialized());
    assert(memorySystem.getCapacity() == 1000);
    assert(memorySystem.getDecayRate() == 0.01f);
    
    // Test validation
    MemorySystemInitializer<MockMemorySystem>::InitializationContext context(nullptr, config);
    bool validationResult = initializer.validate(memorySystem, context);
    assert(validationResult);
    
    // Test with null brain (should fail validation)
    MemorySystemInitializer<MockMemorySystem>::InitializationContext nullContext(nullptr, config);
    bool validationNullResult = initializer.validate(memorySystem, nullContext);
    assert(!validationNullResult);
    
    std::cout << "    MemorySystemInitializer tests passed!" << std::endl;
}

} // namespace test_memory_system_initializer

namespace test_error_logger_helper {

void testErrorLoggerHelper() {
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
    
    // Test getRecentErrors (O(1) access via vector)
    auto recent = logger.getRecentErrors(5);
    assert(recent.size() == 2);
    
    // Test getErrorsByCode (O(n) search)
    auto nullPointerErrors = logger.getErrorsByCode(ErrorCode::NullPointer);
    assert(nullPointerErrors.size() == 1);
    
    // Test error statistics (O(n) aggregation)
    auto stats = logger.getStats();
    assert(stats.totalErrors == 2);
    assert(stats.errorsByCode.find(ErrorCode::NullPointer) != stats.errorsByCode.end());
    
    logger.clear();
    assert(logger.getStats().totalErrors == 0);
    
    std::cout << "    ErrorLoggerHelper tests passed!" << std::endl;
}

} // namespace test_error_logger_helper

namespace test_competition_helper {

void testCompetitionHelper() {
    std::cout << "  Testing CompetitionHelper..." << std::endl;
    
    CompetitionHelper::CompetitionConfig config {
        0.5f,    // inhibitionStrength
        1.5f,    // excitationStrength
        0.3f,    // globalInhibition
        1.0f,    // thresholdMultiplier
        2,       // maxWinners
        true,    // normalizeScores
        true,    // applyInhibition
        true     // applyExcitation
    };
    
    std::vector<NeuronId> competitors = {1, 2, 3, 4, 5};
    std::vector<float> scores = {0.8f, 0.3f, 0.9f, 0.4f, 0.6f};
    
    // Test basic competition (O(n) threshold calculation)
    auto result = CompetitionHelper::runCompetition(competitors, scores, config);
    assert(result.winners.size() == 2);  // 0.9 and 0.8 are highest
    assert(result.threshold > 0.0f);
    assert(result.inhibitionLevels.size() == competitors.size());
    assert(result.excitationLevels.size() == competitors.size());
    
    // Test tournament competition (O(n log k) tournament)
    auto tournamentResult = CompetitionHelper::tournamentCompetition(
        competitors, scores, 3, config);
    assert(tournamentResult.winners.size() <= 2);
    
    // Test round-robin competition (O(n) winner-take-all)
    auto roundRobinResult = CompetitionHelper::roundRobinCompetition(
        competitors, scores, config);
    assert(roundRobinResult.winners.size() == 1);
    assert(roundRobinResult.winners[0] == 3u);  // Highest score
    
    std::cout << "    CompetitionHelper tests passed!" << std::endl;
}

} // namespace test_competition_helper

namespace test_similarity_calculator_helper {

void testSimilarityCalculatorHelper() {
    std::cout << "  Testing SimilarityCalculatorHelper..." << std::endl;
    
    std::vector<float> vectorA = {0.1f, 0.5f, 0.3f, 0.9f};
    std::vector<float> vectorB = {0.2f, 0.6f, 0.2f, 0.8f};
    std::vector<float> vectorC = {1.0f, 0.0f, 0.0f, 0.0f};
    
    // Test cosine similarity (O(n) dot product calculation)
    float cosineSim = SimilarityCalculatorHelper::calculateSimilarity(
        vectorA, vectorB, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
    assert(cosineSim >= 0.0f && cosineSim <= 1.0f);
    
    // Test Euclidean similarity (O(n) distance calculation)
    float euclideanSim = SimilarityCalculatorHelper::calculateSimilarity(
        vectorA, vectorB, SimilarityCalculatorHelper::SimilarityMetric::Euclidean);
    assert(euclideanSim >= 0.0f && euclideanSim <= 1.0f);
    
    // Test with cache (O(1) cache lookup)
    float cachedCosineSim = SimilarityCalculatorHelper::calculateSimilarityWithCache(
        vectorA, vectorB, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
    assert(std::abs(cosineSim - cachedCosineSim) < 0.001f);
    
    // Test batch similarity (O(n*k) batch processing)
    std::vector<std::vector<float>> vectors = {vectorA, vectorB, vectorC};
    std::vector<float> similarities = SimilarityCalculatorHelper::batchSimilarity(
        vectors, vectorA, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
    assert(similarities.size() == 3);
    
    // Test findSimilarVectors (O(n*k) search)
    std::vector<size_t> similar = SimilarityCalculatorHelper::findSimilarVectors(
        vectors, vectorA, 0.8f, SimilarityCalculatorHelper::SimilarityMetric::Cosine);
    // vectorA vs vectorC should be very similar (1.0)
    assert(std::find(similar.begin(), similar.end(), 2u) != similar.end());
    
    // Test isSimilar (O(1) cache access)
    assert(SimilarityCalculatorHelper::isSimilar(vectorA, vectorC, 0.9f,
                                                SimilarityCalculatorHelper::SimilarityMetric::Cosine));
    
    SimilarityCalculatorHelper::getCache().clear();
    
    std::cout << "    SimilarityCalculatorHelper tests passed!" << std::endl;
}

} // namespace test_similarity_calculator_helper

namespace test_resource_cleanup_helper {

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

void testResourceCleanupHelper() {
    std::cout << "  Testing ResourceCleanupHelper..." << std::endl;
    
    MockResource* resource = new MockResource();
    
    // Test SmartResource (RAII)
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
    
    // Test ResourceGuard (RAII with automatic cleanup)
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
    assert(MockResource::cleanupCount == 2);
    
    std::cout << "    ResourceCleanupHelper tests passed!" << std::endl;
}

} // namespace test_resource_cleanup_helper

} // namespace nlm
