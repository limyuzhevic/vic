#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <optional>
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <cstring>
#include <cerrno>

// Include core error and logging headers
#include "../core/Error/Error.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"

// Wrapper for NeuronId hash to avoid compilation issues with std::unordered_map
struct NeuronIdHash {
    size_t operator()(const NeuronId& id) const {
        return std::hash<uint64_t>()(id.value);
    }
};

namespace nlm {

// ==============================================================================
// 1. BrainInitializationHelper
// ==============================================================================

/**
 * Helper class for safe brain pointer initialization and validation
 * Encapsulates common patterns for initializing brain_ pointers and validating them
 */
template<typename T>
class BrainInitializationHelper {
public:
    // Simple initialization template
    static T* initializeBrainPtr(T*& ptr, const std::string& name = "brain") {
        static_assert(std::is_pointer_v<T*>, "T must be a pointer type");
        ptr = nullptr;
        if constexpr (std::is_default_constructible_v<T>) {
            // For types that can be default constructed
            return ptr;
        }
        return ptr;
    }
    
    // Validation macros
    static bool validate(T* ptr, const std::string& name, bool assertOnFail = false) {
        if (!ptr) {
            if (assertOnFail) {
                ErrorReporter::getInstance().report(
                    ErrorReporter::nullPointerError(name, "", 0, ""));
                std::abort();
            }
            return false;
        }
        return true;
    }
    
    // Safe initialization with error handling
    static bool initializeWithErrorCheck(T*& ptr, const std::string& name,
                                         const std::string& file = "",
                                         int line = 0,
                                         const std::string& func = "") {
        ptr = nullptr;
        if constexpr (std::is_default_constructible_v<T>) {
            try {
                ptr = new T();
                if (!validate(ptr, name, false)) {
                    ErrorReporter::getInstance().report(
                        ErrorReporter::nullPointerError(name, file, line, func));
                    delete ptr;
                    ptr = nullptr;
                    return false;
                }
                return true;
            } catch (const std::exception& e) {
                ErrorReporter::getInstance().report(
                    NLMError(ErrorCode::MemoryAllocationFailed,
                           "Failed to initialize brain component " + name + ": " + e.what(),
                           file, line, func));
                return false;
            }
        }
        return false;
    }
    
    // Cleanup template
    static void cleanup(T*& ptr) {
        if (ptr) {
            delete ptr;
            ptr = nullptr;
        }
    }
};

// ==============================================================================
// 2. NeuronSearchHelper
// ==============================================================================

/**
 * Centralized neuron lookup functionality with caching
 * Supports different search strategies (by ID, type, region, etc.)
 * Thread-safe with optimized caching
 */
class NeuronSearchHelper {
public:
    struct NeuronCacheEntry {
        NeuronId id;
        NeuronType type;
        RegionId regionId;
        float activation;
        size_t populationId;
        std::vector<float> pattern;
        SimulationStep lastAccessed;
    };
    
    class NeuronCache {
    public:
        explicit NeuronCache(size_t capacity = 10000) : capacity_(capacity) {}
        
        // Get neuron by ID with cache lookup
        const NeuronCacheEntry* getNeuron(NeuronId id) const {
            std::shared_lock<std::shared_mutex> lock(cacheMutex_);
            auto it = cache_.find(id);
            if (it != cache_.end()) {
                it->second.lastAccessed = getCurrentStep();
                return &it->second;
            }
            return nullptr;
        }
        
        // Insert neuron into cache
        void insertNeuron(const NeuronCacheEntry& entry) {
            std::unique_lock<std::shared_mutex> lock(cacheMutex_);
            cache_[entry.id] = entry;
            
            // Evict if over capacity
            if (cache_.size() > capacity_) {
                evictOldest();
            }
        }
        
        // Clear cache
        void clear() {
            std::unique_lock<std::shared_mutex> lock(cacheMutex_);
            cache_.clear();
        }
        
        // Get all neurons by type
        std::vector<NeuronId> getNeuronsByType(NeuronType type) const {
            std::shared_lock<std::shared_mutex> lock(cacheMutex_);
            std::vector<NeuronId> result;
            for (const auto& pair : cache_) {
                if (pair.second.type == type) {
                    result.push_back(pair.first);
                }
            }
            return result;
        }
        
        // Get all neurons by region
        std::vector<NeuronId> getNeuronsByRegion(RegionId regionId) const {
            std::shared_lock<std::shared_mutex> lock(cacheMutex_);
            std::vector<NeuronId> result;
            for (const auto& pair : cache_) {
                if (pair.second.regionId == regionId) {
                    result.push_back(pair.first);
                }
            }
            return result;
        }
        
        // Search with filter function
        template<typename FilterFunc>
        std::vector<NeuronId> search(FilterFunc filter) const {
            std::shared_lock<std::shared_mutex> lock(cacheMutex_);
            std::vector<NeuronId> result;
            for (const auto& pair : cache_) {
                if (filter(pair.second)) {
                    result.push_back(pair.first);
                }
            }
            return result;
        }
        
    private:
        void evictOldest() {
            if (cache_.empty()) return;
            
            auto oldest = cache_.begin();
            auto latestTime = oldest->second.lastAccessed;
            
            for (auto it = std::next(cache_.begin()); it != cache_.end(); ++it) {
                if (it->second.lastAccessed < latestTime) {
                    oldest = it;
                    latestTime = it->second.lastAccessed;
                }
            }
            
            cache_.erase(oldest);
        }
        
        SimulationStep getCurrentStep() const {
            return static_cast<SimulationStep>(std::chrono::steady_clock::now().time_since_epoch().count());
        }
        
        mutable std::shared_mutex cacheMutex_;
        std::unordered_map<NeuronId, NeuronCacheEntry> cache_;
        size_t capacity_;
    };
    
    // Static cache instance
    static NeuronCache& getCache() {
        static NeuronCache cache;
        return cache;
    }
    
    // Clear cache
    static void clearCache() {
        getCache().clear();
    }
};

// ==============================================================================
// 3. MemorySystemInitializer
// ==============================================================================

/**
 * Template for initializing memory systems
 * Provides consistency across all memory implementations
 * Supports dependency injection
 */
template<typename MemorySystem>
class MemorySystemInitializer {
public:
    struct MemoryConfig {
        size_t maxCapacity;
        float decayRate;
        bool autoCleanup;
        std::string name;
        std::unordered_map<std::string, std::string> parameters;
    };
    
    class InitializationContext {
    public:
        InitializationContext() = default;
        InitializationContext(Brain* brain, const MemoryConfig& config)
            : brain_(brain), config_(config) {}
        
        Brain* getBrain() const { return brain_; }
        const MemoryConfig& getConfig() const { return config_; }
        
        template<typename T>
        T* getService() const {
            // Dependency injection - in real implementation, would use DI container
            return nullptr;
        }
        
    private:
        Brain* brain_ = nullptr;
        MemoryConfig config_;
    };
    
    // Template initializer for different memory system types
    static bool initialize(MemorySystem& memorySystem,
                          Brain* brain,
                          const MemoryConfig& config,
                          bool validatePreconditions = true) {
        if (!memorySystem.getBrain() && !brain) {
            if (validatePreconditions) {
                ErrorReporter::getInstance().report(
                    ErrorReporter::nullPointerError("brain for memory system", "", 0, ""));
                return false;
            }
        }
        
        memorySystem.initialize(brain);
        
        // Apply configuration based on system type
        if constexpr (requires { memorySystem.setCapacity(0); }) {
            memorySystem.setCapacity(config.maxCapacity);
        }
        
        if constexpr (requires { memorySystem.setDecayRate(0.0f); }) {
            memorySystem.setDecayRate(config.decayRate);
        }
        
        return true;
    }
    
    // Validate memory system
    static bool validate(const MemorySystem& memorySystem,
                        const InitializationContext& context) {
        if (!context.getBrain()) {
            ErrorReporter::getInstance().report(
                ErrorReporter::nullPointerError("brain in validation", "", 0, ""));
            return false;
        }
        
        // Type-specific validation
        if constexpr (requires { memorySystem.getCapacity(); }) {
            size_t capacity = memorySystem.getCapacity();
            if (capacity == 0) {
                ErrorReporter::getInstance().report(
                    ErrorReporter::validationError("memory capacity", "zero"));
                return false;
            }
        }
        
        return true;
    }
};

// ==============================================================================
// 4. ErrorLoggerHelper
// ==============================================================================

/**
 * Centralized error logging with consistent formatting
 * Provides error code tracking and supports different log levels and contexts
 */
class ErrorLoggerHelper {
public:
    struct ErrorLogEntry {
        std::chrono::system_clock::time_point timestamp;
        ErrorCode code;
        std::string message;
        std::string file;
        int line;
        std::string function;
        LogLevel level;
        std::string context;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    class ErrorLogger {
    public:
        explicit ErrorLogger(size_t maxErrors = 1000) : maxErrors_(maxErrors) {}
        
        // Log error with automatic formatting
        void logError(ErrorCode code, const std::string& message,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "",
                     LogLevel level = LogLevel::Error,
                     const std::string& context = "") {
            ErrorLogEntry entry {
                std::chrono::system_clock::now(),
                code,
                message,
                file,
                line,
                function,
                level,
                context
            };
            
            std::lock_guard<std::mutex> lock(errorMutex_);
            errors_.push_back(entry);
            
            // Trim if over capacity
            if (errors_.size() > maxErrors_) {
                errors_.erase(errors_.begin());
            }
            
            // Forward to error reporter for immediate reporting
            NLMError nlmError(code, message, file, line, function);
            ErrorReporter::getInstance().report(nlmError);
        }
        
        // Log with additional metadata
        void logErrorWithMetadata(ErrorCode code, const std::string& message,
                                 const std::string& file = "", int line = 0,
                                 const std::string& function = "",
                                 const std::unordered_map<std::string, std::string>& metadata = {}) {
            ErrorLogEntry entry {
                std::chrono::system_clock::now(),
                code,
                message,
                file,
                line,
                function,
                LogLevel::Error,
                "",
                metadata
            };
            
            std::lock_guard<std::mutex> lock(errorMutex_);
            errors_.push_back(entry);
        }
        
        // Get recent errors
        std::vector<ErrorLogEntry> getRecentErrors(size_t count = 10) const {
            std::shared_lock<std::shared_mutex> lock(errorMutex_);
            size_t start = errors_.size() > count ? errors_.size() - count : 0;
            return std::vector<ErrorLogEntry>(errors_.begin() + start, errors_.end());
        }
        
        // Get errors by code
        std::vector<ErrorLogEntry> getErrorsByCode(ErrorCode code) const {
            std::shared_lock<std::shared_mutex> lock(errorMutex_);
            std::vector<ErrorLogEntry> result;
            for (const auto& entry : errors_) {
                if (entry.code == code) {
                    result.push_back(entry);
                }
            }
            return result;
        }
        
        // Clear all errors
        void clear() {
            std::unique_lock<std::shared_mutex> lock(errorMutex_);
            errors_.clear();
        }
        
        // Get error statistics
        struct ErrorStats {
            size_t totalErrors = 0;
            std::unordered_map<ErrorCode, size_t> errorsByCode;
            std::unordered_map<LogLevel, size_t> errorsByLevel;
            std::unordered_map<std::string, size_t> errorsByContext;
        };
        
        ErrorStats getStats() const {
            std::shared_lock<std::shared_mutex> lock(errorMutex_);
            ErrorStats stats;
            stats.totalErrors = errors_.size();
            
            for (const auto& entry : errors_) {
                stats.errorsByCode[entry.code]++;
                stats.errorsByLevel[entry.level]++;
                if (!entry.context.empty()) {
                    stats.errorsByContext[entry.context]++;
                }
            }
            
            return stats;
        }
        
    private:
        mutable std::shared_mutex errorMutex_;
        std::vector<ErrorLogEntry> errors_;
        size_t maxErrors_;
    };
    
    // Static error logger instance
    static ErrorLogger& getLogger() {
        static ErrorLogger logger;
        return logger;
    }
    
    // Convenience logging functions
    static void log(ErrorCode code, const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "") {
        getLogger().logError(code, message, file, line, function);
    }
    
    static void logNullPointer(const std::string& name,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "") {
        getLogger().logError(ErrorCode::NullPointer, 
                           "Null pointer access to: " + name, 
                           file, line, function);
    }
    
    static void logValidationError(const std::string& field,
                                  const std::string& value,
                                  const std::string& file = "", int line = 0,
                                  const std::string& function = "") {
        getLogger().logError(ErrorCode::ValidationFailed,
                           "Validation failed for " + field + ": " + value,
                           file, line, function);
    }
};

// ==============================================================================
// 5. CompetitionHelper
// ==============================================================================

/**
 * Common competition and winner selection algorithms
 * Optimized for neural competition patterns
 * Supports different competition types (inhibitory, excitatory, etc.)
 */
class CompetitionHelper {
public:
    struct CompetitionResult {
        std::vector<NeuronId> winners;
        std::vector<float> winnerScores;
        float threshold;
        size_t maxWinners;
        std::vector<float> inhibitionLevels;
        std::vector<float> excitationLevels;
    };
    
    struct CompetitionConfig {
        float inhibitionStrength = 0.5f;
        float excitationStrength = 1.5f;
        float globalInhibition = 0.3f;
        float thresholdMultiplier = 1.0f;
        size_t maxWinners = 1;
        bool normalizeScores = true;
        bool applyInhibition = true;
        bool applyExcitation = true;
    };
    
    // Main competition algorithm
    static CompetitionResult runCompetition(const std::vector<NeuronId>& competitors,
                                          const std::vector<float>& scores,
                                          const CompetitionConfig& config = {}) {
        CompetitionResult result;
        result.maxWinners = config.maxWinners;
        result.threshold = calculateThreshold(scores, config);
        result.inhibitionLevels.assign(competitors.size(), config.inhibitionStrength);
        result.excitationLevels.assign(competitors.size(), config.excitationStrength);
        
        // Find winners based on threshold
        for (size_t i = 0; i < competitors.size(); ++i) {
            if (scores[i] >= result.threshold && result.winners.size() < result.maxWinners) {
                result.winners.push_back(competitors[i]);
                result.winnerScores.push_back(scores[i]);
            }
        }
        
        // Apply competition effects
        if (config.applyInhibition) {
            applyInhibition(competitors, scores, result, config.globalInhibition);
        }
        
        if (config.applyExcitation) {
            applyExcitation(competitors, scores, result);
        }
        
        return result;
    }
    
    // Tournament-style competition
    static CompetitionResult tournamentCompetition(const std::vector<NeuronId>& competitors,
                                                  const std::vector<float>& scores,
                                                  size_t tournamentSize = 4,
                                                  const CompetitionConfig& config = {}) {
        if (competitors.size() <= tournamentSize) {
            return runCompetition(competitors, scores, config);
        }
        
        std::vector<NeuronId> selectedWinners;
        std::vector<float> selectedScores;
        
        // Run multiple tournaments
        for (size_t t = 0; t < competitors.size(); t += tournamentSize) {
            std::vector<NeuronId> tournamentCompetitors;
            std::vector<float> tournamentScores;
            
            size_t end = std::min(t + tournamentSize, competitors.size());
            for (size_t i = t; i < end; ++i) {
                tournamentCompetitors.push_back(competitors[i]);
                tournamentScores.push_back(scores[i]);
            }
            
            auto tournamentResult = runCompetition(tournamentCompetitors, tournamentScores, config);
            
            // Add tournament winners to overall winners
            for (size_t i = 0; i < std::min(tournamentResult.winners.size(), 
                                          selectedWinners.size() + config.maxWinners - selectedWinners.size()); ++i) {
                size_t idx = &tournamentResult.winners[i] - &tournamentCompetitors[0];
                selectedWinners.push_back(tournamentResult.winners[i]);
                selectedScores.push_back(tournamentResult.winnerScores[i]);
            }
        }
        
        return CompetitionResult {
            selectedWinners,
            selectedScores,
            calculateThreshold(selectedScores, config),
            config.maxWinners,
            std::vector<float>(competitors.size(), config.inhibitionStrength),
            std::vector<float>(competitors.size(), config.excitationStrength)
        };
    }
    
    // Round-robin competition
    static CompetitionResult roundRobinCompetition(const std::vector<NeuronId>& competitors,
                                                  const std::vector<float>& scores,
                                                  const CompetitionConfig& config = {}) {
        // Simple winner-take-all based on highest score
        if (scores.empty()) {
            return CompetitionResult{};
        }
        
        size_t maxScoreIndex = std::max_element(scores.begin(), scores.end()) - scores.begin();
        
        return CompetitionResult {
            { competitors[maxScoreIndex] },
            { scores[maxScoreIndex] },
            calculateThreshold(scores, config),
            config.maxWinners,
            std::vector<float>(competitors.size(), config.inhibitionStrength),
            std::vector<float>(competitors.size(), config.excitationStrength)
        };
    }
    
private:
    static float calculateThreshold(const std::vector<float>& scores,
                                   const CompetitionConfig& config) {
        if (scores.empty()) return 0.0f;
        
        float sum = std::accumulate(scores.begin(), scores.end(), 0.0f);
        float avg = sum / scores.size();
        
        if (config.normalizeScores) {
            float maxScore = *std::max_element(scores.begin(), scores.end());
            if (maxScore > 0.0f) {
                avg /= maxScore;
            }
        }
        
        return avg * config.thresholdMultiplier;
    }
    
    static void applyInhibition(const std::vector<NeuronId>& competitors,
                               const std::vector<float>& scores,
                               CompetitionResult& result,
                               float globalInhibition) {
        for (size_t i = 0; i < competitors.size(); ++i) {
            bool isWinner = std::find(result.winners.begin(), result.winners.end(), competitors[i]) != result.winners.end();
            
            if (!isWinner) {
                // Apply inhibition based on relative strength
                float inhibition = globalInhibition * result.inhibitionLevels[i];
                
                // Find closest winner for distance-based inhibition
                float minDistance = std::numeric_limits<float>::max();
                for (const auto& winner : result.winners) {
                    auto winnerIt = std::find(competitors.begin(), competitors.end(), winner);
                    if (winnerIt != competitors.end()) {
                        size_t winnerIdx = winnerIt - competitors.begin();
                        float distance = std::abs(scores[i] - scores[winnerIdx]);
                        minDistance = std::min(minDistance, distance);
                    }
                }
                
                if (minDistance < std::numeric_limits<float>::max()) {
                    // Weaken inhibition for neurons close to winners
                    inhibition *= std::max(0.1f, 1.0f - minDistance);
                }
                
                result.inhibitionLevels[i] = inhibition;
            }
        }
    }
    
    static void applyExcitation(const std::vector<NeuronId>& competitors,
                               const std::vector<float>& scores,
                               CompetitionResult& result) {
        for (size_t i = 0; i < competitors.size(); ++i) {
            bool isWinner = std::find(result.winners.begin(), result.winners.end(), competitors[i]) != result.winners.end();
            
            if (isWinner) {
                // Stronger excitation for winners
                result.excitationLevels[i] *= 2.0f;
            }
        }
    }
};

// ==============================================================================
// 6. SimilarityCalculatorHelper
// ==============================================================================

/**
 * Centralized similarity calculation utilities
 * Supports different similarity metrics (cosine, Euclidean, etc.)
 * Provides caching for repeated calculations
 */
class SimilarityCalculatorHelper {
public:
    enum class SimilarityMetric {
        Cosine,
        Euclidean,
        DotProduct,
        Manhattan,
        Chebyshev,
        PearsonCorrelation
    };
    
    struct SimilarityCacheEntry {
        std::string key;
        float similarity;
        SimulationStep lastUsed;
        SimilarityMetric metric;
        std::vector<float> vectorA;
        std::vector<float> vectorB;
    };
    
    class SimilarityCache {
    public:
        explicit SimilarityCache(size_t capacity = 5000) : capacity_(capacity) {}
        
        // Get cached similarity
        std::optional<float> getSimilarity(const std::string& key) const {
            std::shared_lock<std::shared_mutex> lock(cacheMutex_);
            auto it = cache_.find(key);
            if (it != cache_.end()) {
                it->second.lastUsed = getCurrentStep();
                return it->second.similarity;
            }
            return std::nullopt;
        }
        
        // Cache similarity
        void cacheSimilarity(const std::string& key, float similarity,
                           SimilarityMetric metric,
                           const std::vector<float>& vectorA,
                           const std::vector<float>& vectorB) {
            std::unique_lock<std::shared_mutex> lock(cacheMutex_);
            cache_[key] = SimilarityCacheEntry {
                key, similarity, getCurrentStep(), metric, vectorA, vectorB
            };
            
            // Evict if over capacity
            if (cache_.size() > capacity_) {
                evictOldest();
            }
        }
        
        // Clear cache
        void clear() {
            std::unique_lock<std::shared_mutex> lock(cacheMutex_);
            cache_.clear();
        }
        
        // Generate cache key
        static std::string generateKey(const std::vector<float>& vectorA,
                                      const std::vector<float>& vectorB,
                                      SimilarityMetric metric) {
            std::hash<float> hasher;
            std::size_t hash = 0;
            
            for (float val : vectorA) {
                hash ^= hasher(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            
            for (float val : vectorB) {
                hash ^= hasher(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            
            hash ^= static_cast<size_t>(metric) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            
            return std::to_string(hash);
        }
        
    private:
        void evictOldest() {
            if (cache_.empty()) return;
            
            auto oldest = cache_.begin();
            auto latestTime = oldest->second.lastUsed;
            
            for (auto it = std::next(cache_.begin()); it != cache_.end(); ++it) {
                if (it->second.lastUsed < latestTime) {
                    oldest = it;
                    latestTime = it->second.lastUsed;
                }
            }
            
            cache_.erase(oldest);
        }
        
        SimulationStep getCurrentStep() const {
            return static_cast<SimulationStep>(std::chrono::steady_clock::now().time_since_epoch().count());
        }
        
        mutable std::shared_mutex cacheMutex_;
        std::unordered_map<std::string, SimilarityCacheEntry> cache_;
        size_t capacity_;
    };
    
    // Static cache instance
    static SimilarityCache& getCache() {
        static SimilarityCache cache;
        return cache;
    }
    
    // Calculate similarity based on metric
    static float calculateSimilarity(const std::vector<float>& a,
                                     const std::vector<float>& b,
                                     SimilarityMetric metric = SimilarityMetric::Cosine) {
        if (a.size() != b.size() || a.empty()) {
            return 0.0f;
        }
        
        switch (metric) {
            case SimilarityMetric::Cosine:
                return cosineSimilarity(a, b);
            case SimilarityMetric::Euclidean:
                return 1.0f / (1.0f + euclideanDistance(a, b));
            case SimilarityMetric::DotProduct:
                return dotProductSimilarity(a, b);
            case SimilarityMetric::Manhattan:
                return manhattanSimilarity(a, b);
            case SimilarityMetric::Chebyshev:
                return chebyshevSimilarity(a, b);
            case SimilarityMetric::PearsonCorrelation:
                return pearsonCorrelation(a, b);
            default:
                return 0.0f;
        }
    }
    
    // Calculate similarity with caching
    static float calculateSimilarityWithCache(const std::vector<float>& a,
                                              const std::vector<float>& b,
                                              SimilarityMetric metric = SimilarityMetric::Cosine) {
        std::string key = SimilarityCache::generateKey(a, b, metric);
        
        // Try cache first
        auto cached = getCache().getSimilarity(key);
        if (cached.has_value()) {
            return cached.value();
        }
        
        // Calculate and cache
        float similarity = calculateSimilarity(a, b, metric);
        getCache().cacheSimilarity(key, similarity, metric, a, b);
        
        return similarity;
    }
    
    // Batch similarity calculations
    static std::vector<float> batchSimilarity(const std::vector<std::vector<float>>& vectors,
                                             const std::vector<float>& reference,
                                             SimilarityMetric metric = SimilarityMetric::Cosine) {
        std::vector<float> results;
        results.reserve(vectors.size());
        
        for (const auto& vec : vectors) {
            results.push_back(calculateSimilarity(vec, reference, metric));
        }
        
        return results;
    }
    
    // Multi-vector similarity search
    static std::vector<size_t> findSimilarVectors(const std::vector<std::vector<float>>& vectors,
                                                 const std::vector<float>& target,
                                                 float threshold,
                                                 SimilarityMetric metric = SimilarityMetric::Cosine) {
        std::vector<size_t> result;
        
        for (size_t i = 0; i < vectors.size(); ++i) {
            float similarity = calculateSimilarity(vectors[i], target, metric);
            if (similarity >= threshold) {
                result.push_back(i);
            }
        }
        
        return result;
    }
    
    // Similarity threshold check with caching
    static bool isSimilar(const std::vector<float>& a,
                         const std::vector<float>& b,
                         float threshold,
                         SimilarityMetric metric = SimilarityMetric::Cosine) {
        return calculateSimilarityWithCache(a, b, metric) >= threshold;
    }
    
private:
    static float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }
    
    static float dotProductSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        float dot = 0.0f;
        
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
        }
        
        // Normalize by size
        return dot / a.size();
    }
    
    static float manhattanSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        float sum = 0.0f;
        
        for (size_t i = 0; i < a.size(); ++i) {
            sum += std::abs(a[i] - b[i]);
        }
        
        return 1.0f / (1.0f + sum);
    }
    
    static float chebyshevSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        float maxDiff = 0.0f;
        
        for (size_t i = 0; i < a.size(); ++i) {
            float diff = std::abs(a[i] - b[i]);
            maxDiff = std::max(maxDiff, diff);
        }
        
        return 1.0f / (1.0f + maxDiff);
    }
    
    static float pearsonCorrelation(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size() || a.size() < 2) return 0.0f;
        
        float sumA = std::accumulate(a.begin(), a.end(), 0.0f);
        float sumB = std::accumulate(b.begin(), b.end(), 0.0f);
        float sumAB = 0.0f;
        float sumA2 = 0.0f;
        float sumB2 = 0.0f;
        
        for (size_t i = 0; i < a.size(); ++i) {
            sumAB += a[i] * b[i];
            sumA2 += a[i] * a[i];
            sumB2 += b[i] * b[i];
        }
        
        float n = static_cast<float>(a.size());
        float numerator = n * sumAB - sumA * sumB;
        float denominator = std::sqrt((n * sumA2 - sumA * sumA) * (n * sumB2 - sumB * sumB));
        
        if (denominator < 0.0001f) return 0.0f;
        
        return numerator / denominator;
    }
    
    static float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b) {
        float sum = 0.0f;
        
        for (size_t i = 0; i < a.size(); ++i) {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        
        return std::sqrt(sum);
    }
};

// ==============================================================================
// 7. ResourceCleanupHelper
// ==============================================================================

/**
 * Template for safe resource cleanup
 * Provides RAII patterns for resource management
 * Supports automatic cleanup in error conditions
 */
class ResourceCleanupHelper {
public:
    template<typename Resource>
    class ResourceGuard {
    public:
        ResourceGuard(Resource* resource, std::function<void(Resource*)> cleanupFunc,
                     const std::string& name = "resource")
            : resource_(resource), cleanupFunc_(cleanupFunc), name_(name) {
            if (!validateResource(resource)) {
                throw NLMError(ErrorCode::NullPointer,
                             "Null resource in guard for " + name_,
                             "", 0, "");
            }
        }
        
        ~ResourceGuard() {
            if (resource_ && cleanupFunc_) {
                try {
                    cleanupFunc_(resource_);
                } catch (const std::exception& e) {
                    ErrorReporter::getInstance().report(
                        NLMError(ErrorCode::SystemError,
                               "Exception in resource cleanup for " + name_ + ": " + e.what(),
                               "", 0, ""));
                }
            }
        }
        
        // Prevent copying
        ResourceGuard(const ResourceGuard&) = delete;
        ResourceGuard& operator=(const ResourceGuard&) = delete;
        
        // Allow moving
        ResourceGuard(ResourceGuard&& other) noexcept
            : resource_(other.resource_), cleanupFunc_(std::move(other.cleanupFunc_)), name_(std::move(other.name_)) {
            other.resource_ = nullptr;
            other.cleanupFunc_ = nullptr;
        }
        
        ResourceGuard& operator=(ResourceGuard&& other) noexcept {
            if (this != &other) {
                cleanup();
                resource_ = other.resource_;
                cleanupFunc_ = std::move(other.cleanupFunc_);
                name_ = std::move(other.name_);
                other.resource_ = nullptr;
                other.cleanupFunc_ = nullptr;
            }
            return *this;
        }
        
        // Access resource
        Resource* get() const { return resource_; }
        Resource& operator*() const { return *resource_; }
        Resource* operator->() const { return resource_; }
        
        // Release ownership
        Resource* release() {
            Resource* temp = resource_;
            resource_ = nullptr;
            return temp;
        }
        
        // Check if resource is valid
        bool isValid() const { return resource_ != nullptr; }
        
        // Reset resource
        void reset(Resource* newResource) {
            if (resource_ != newResource) {
                cleanup();
                resource_ = newResource;
            }
        }
        
    private:
        void cleanup() {
            if (resource_ && cleanupFunc_) {
                cleanupFunc_(resource_);
                resource_ = nullptr;
            }
        }
        
        bool validateResource(Resource* resource) const {
            return resource != nullptr;
        }
        
        Resource* resource_;
        std::function<void(Resource*)> cleanupFunc_;
        std::string name_;
    };
    
    // Template for multiple resource cleanup
    template<typename... Resources>
    class MultiResourceGuard {
    public:
        MultiResourceGuard() = default;
        
        template<typename Resource>
        void addResource(Resource* resource, std::function<void(Resource*)> cleanupFunc,
                        const std::string& name = "resource") {
            guards_.emplace_back(resource, cleanupFunc, name);
        }
        
        // Release all resources
        void releaseAll() {
            for (auto& guard : guards_) {
                guard.release();
            }
            guards_.clear();
        }
        
        // Clear all guards without cleaning up
        void clear() {
            guards_.clear();
        }
        
        // Check if all resources are valid
        bool areAllValid() const {
            for (const auto& guard : guards_) {
                if (!guard.isValid()) return false;
            }
            return true;
        }
        
    private:
        std::vector<ResourceGuard<Resources>...> guards_;
    };
    
    // Exception-safe cleanup template
    template<typename T>
    class ExceptionSafeCleanup {
    public:
        ExceptionSafeCleanup(T* resource, std::function<void(T*)> cleanupFunc,
                            const std::string& name = "resource")
            : guard_(resource, cleanupFunc, name) {}
        
        ~ExceptionSafeCleanup() {
            if (guard_.isValid()) {
                // Resource is still owned, cleanup
                cleanupFunc_(guard_.get());
            }
        }
        
        // Prevent exceptions from propagating
        void forceCleanup() {
            if (guard_.isValid()) {
                cleanupFunc_(guard_.get());
                guard_.release();
            }
        }
        
    private:
        ResourceGuard<T> guard_;
    };
    
    // Smart pointer for automatic resource management
    template<typename T>
    class SmartResource {
    public:
        explicit SmartResource(T* resource = nullptr,
                              std::function<void(T*)> cleanupFunc = nullptr)
            : resource_(resource), cleanupFunc_(cleanupFunc) {}
        
        ~SmartResource() {
            reset();
        }
        
        // Disable copying
        SmartResource(const SmartResource&) = delete;
        SmartResource& operator=(const SmartResource&) = delete;
        
        // Allow moving
        SmartResource(SmartResource&& other) noexcept
            : resource_(other.resource_), cleanupFunc_(std::move(other.cleanupFunc_)) {
            other.resource_ = nullptr;
        }
        
        SmartResource& operator=(SmartResource&& other) noexcept {
            if (this != &other) {
                reset();
                resource_ = other.resource_;
                cleanupFunc_ = std::move(other.cleanupFunc_);
                other.resource_ = nullptr;
            }
            return *this;
        }
        
        // Access
        T* get() const { return resource_; }
        T& operator*() const { return *resource_; }
        T* operator->() const { return resource_; }
        
        // Release ownership
        T* release() {
            T* temp = resource_;
            resource_ = nullptr;
            return temp;
        }
        
        // Reset
        void reset(T* newResource = nullptr) {
            if (resource_ != newResource) {
                if (resource_ && cleanupFunc_) {
                    cleanupFunc_(resource_);
                }
                resource_ = newResource;
            }
        }
        
        // Comparison
        bool operator==(std::nullptr_t) const { return resource_ == nullptr; }
        bool operator!=(std::nullptr_t) const { return resource_ != nullptr; }
        bool operator!() const { return resource_ == nullptr; }
        
    private:
        T* resource_;
        std::function<void(T*)> cleanupFunc_;
    };
    
    // Atomic resource operations for thread safety
    template<typename T>
    class AtomicResource {
    public:
        explicit AtomicResource(T* resource = nullptr,
                               std::function<void(T*)> cleanupFunc = nullptr)
            : resource_(resource), cleanupFunc_(cleanupFunc) {}
        
        // Thread-safe access
        T* get() const {
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return resource_;
        }
        
        // Thread-safe assignment
        void set(T* resource) {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            reset(resource);
        }
        
        // Thread-safe reset
        void reset(T* newResource = nullptr) {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            if (resource_ != newResource) {
                if (resource_ && cleanupFunc_) {
                    cleanupFunc_(resource_);
                }
                resource_ = newResource;
            }
        }
        
        // Thread-safe cleanup
        void cleanup() {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            reset();
        }
        
    private:
        T* resource_;
        std::function<void(T*)> cleanupFunc_;
        mutable std::shared_mutex mutex_;
    };
    
    // Cleanup utilities
    static void deleteResource(void* ptr) {
        delete static_cast<T*>(ptr);
    }
    
    static void deleteArrayResource(void* ptr) {
        delete[] static_cast<T*>(ptr);
    }
    
    static void nullCleanup(void*) {
        // Do nothing - used when you want to take ownership without cleanup
    }
};

// ==============================================================================
// Helper function implementations
// ==============================================================================

// Clear all helper caches
inline void clearAllCaches() {
    NeuronSearchHelper::clearCache();
    SimilarityCalculatorHelper::getCache().clear();
    ErrorLoggerHelper::getLogger().clear();
}

} // namespace nlm