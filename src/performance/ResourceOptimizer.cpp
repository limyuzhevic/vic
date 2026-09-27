// Implementation of ResourceOptimizer

#include "nlm/performance/ResourceOptimizer.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <random>

namespace nlm {

ResourceOptimizer::ResourceOptimizer(
    bool enableAutoTuning,
    std::chrono::milliseconds tuningInterval,
    size_t maxSuggestions)
    : enableAutoTuning_(enableAutoTuning)
    , tuningInterval_(tuningInterval)
    , maxSuggestions_(maxSuggestions) {
}

void ResourceOptimizer::start() {
    if (running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    running_.store(true, std::memory_order_relaxed);
    optimizationThread_ = std::thread(&ResourceOptimizer::optimizationThreadFunc, this);
}

void ResourceOptimizer::stop() {
    if (!running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    {
        std::unique_lock<std::mutex> lock(stopMutex_);
        running_.store(false, std::memory_order_relaxed);
        stopCondition_.notify_one();
    }
    
    if (optimizationThread_.joinable()) {
        optimizationThread_.join();
    }
}

void ResourceOptimizer::optimizationThreadFunc() {
    while (running_.load(std::memory_order_relaxed)) {
        std::unique_lock<std::mutex> lock(stopMutex_);
        if (stopCondition_.wait_for(lock, tuningInterval_, 
                                   [this] { return !running_.load(std::memory_order_relaxed); })) {
            break;
        }
        lock.unlock();
        
        analyzeSIMDOpportunities();
        analyzeCacheOptimizations();
        analyzeParallelOptimizations();
        analyzeMemoryPoolOptimizations();
        analyzeResourceAllocation();
        
        generateSuggestions();
    }
}

void ResourceOptimizer::analyzeSIMDOpportunities() {
    std::lock_guard<std::mutex> lock(simdMutex_);
    
    // Simulate SIMD analysis based on performance metrics
    SIMDOptimization optimization;
    optimization.operationName = "matrix_multiply";
    optimization.currentImplementation = "scalar";
    optimization.simdImplementation = "AVX-512";
    optimization.vectorWidth = 16;
    optimization.expectedSpeedup = 8.0;
    optimization.isHardwareSupported = true;
    optimization.instructionSet = "AVX-512";
    optimization.implementationNotes = "Use _mm512_* intrinsics for 16-element vector operations";
    optimization.requiresAlignment = true;
    
    simdOpportunities_.push_back(optimization);
    
    // Additional SIMD opportunities
    SIMDOptimization optimization2;
    optimization2.operationName = "neuron_update";
    optimization2.currentImplementation = "scalar_loop";
    optimization2.simdImplementation = "SSE";
    optimization2.vectorWidth = 4;
    optimization2.expectedSpeedup = 4.0;
    optimization2.isHardwareSupported = true;
    optimization2.instructionSet = "SSE";
    optimization2.implementationNotes = "Use _mm_load_ps and _mm_store_ps for 4-element vector operations";
    optimization2.requiresAlignment = false;
    
    simdOpportunities_.push_back(optimization2);
    
    // Maintain history
    while (simdOpportunities_.size() > 100) {
        simdOpportunities_.erase(simdOpportunities_.begin());
    }
}

void ResourceOptimizer::analyzeCacheOptimizations() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    // Simulate cache analysis based on memory access patterns
    CacheOptimization optimization;
    optimization.dataStructure = "neuron_state_array";
    optimization.currentLayout = "AoS (Array of Structures)";
    optimization.recommendedLayout = "SoA (Structure of Arrays)";
    optimization.cacheMissesReduced = 30;
    optimization.memoryAccessesImproved = 40;
    optimization.dataAccessPattern = "sequential_access";
    optimization.cacheBlockingTechnique = "cache_blocking(64)";
    optimization.requiresCodeChanges = true;
    optimization.implementationComplexity = "medium";
    
    cacheOptimizations_.push_back(optimization);
    
    // Additional cache optimizations
    CacheOptimization optimization2;
    optimization2.dataStructure = "synapse_weights";
    optimization2.currentLayout = "sparse_matrix";
    optimization2.recommendedLayout = "block_sparse_matrix";
    optimization2.cacheMissesReduced = 25;
    optimization2.memoryAccessesImproved = 35;
    optimization2.dataAccessPattern = "cached_block_access";
    optimization2.cacheBlockingTechnique = "blocked_matrix_multiply";
    optimization2.requiresCodeChanges = true;
    optimization2.implementationComplexity = "high";
    
    cacheOptimizations_.push_back(optimization2);
    
    // Maintain history
    while (cacheOptimizations_.size() > 100) {
        cacheOptimizations_.erase(cacheOptimizations_.begin());
    }
}

void ResourceOptimizer::analyzeParallelOptimizations() {
    std::lock_guard<std::mutex> lock(parallelMutex_);
    
    // Simulate parallel analysis based on performance metrics
    ParallelOptimization optimization;
    optimization.operationName = "neuron_updates";
    optimization.currentParallelization = "sequential";
    optimization.recommendedParallelization = "threadpool";
    optimization.expectedParallelSpeedup = 4.0;
    optimization.currentThreadCount = 1;
    optimization.recommendedThreadCount = 4;
    optimization.parallelizationTechnique = "threadpool";
    optimization.synchronizationPoints = {"step_boundary", "spike_delivery"};
    optimization.requiresHardwareChanges = false;
    optimization.threadAffinityStrategy = "cpu_bind(0-3)";
    
    parallelOptimizations_.push_back(optimization);
    
    // Additional parallel optimizations
    ParallelOptimization optimization2;
    optimization2.operationName = "plasticity_updates";
    optimization2.currentParallelization = "manual_locking";
    optimization2.recommendedParallelization = "work_stealing";
    optimization2.expectedParallelSpeedup = 6.0;
    optimization2.currentThreadCount = 2;
    optimization2.recommendedThreadCount = 8;
    optimization2.parallelizationTechnique = "work_stealing";
    optimization2.synchronizationPoints = {"spike_delivery", "plasticity_update"};
    optimization2.requiresHardwareChanges = false;
    optimization2.threadAffinityStrategy = "dynamic_affinity";
    
    parallelOptimizations_.push_back(optimization2);
    
    // Maintain history
    while (parallelOptimizations_.size() > 100) {
        parallelOptimizations_.erase(parallelOptimizations_.begin());
    }
}

void ResourceOptimizer::analyzeMemoryPoolOptimizations() {
    std::lock_guard<std::mutex> lock(poolMutex_);
    
    // Simulate memory pool analysis
    MemoryPoolOptimization optimization;
    optimization.poolName = "neuron_pool";
    optimization.currentConfiguration = "block_size=1024, initial_blocks=4";
    optimization.recommendedConfiguration = "block_size=2048, initial_blocks=8";
    optimization.currentBlockSize = 1024;
    optimization.recommendedBlockSize = 2048;
    optimization.currentPreallocatedBlocks = 4;
    optimization.recommendedPreallocatedBlocks = 8;
    optimization.expectedFragmentationReduction = 15;
    optimization.expectedAllocationSpeedup = 20;
    optimization.requiresRuntimeChanges = false;
    optimization.allocationPatterns = {"high_allocation_rate", "large_objects"};
    
    memoryPoolOptimizations_.push_back(optimization);
    
    // Additional memory pool optimizations
    MemoryPoolOptimization optimization2;
    optimization2.poolName = "synapse_pool";
    optimization2.currentConfiguration = "block_size=512, initial_blocks=2";
    optimization2.recommendedConfiguration = "block_size=1024, initial_blocks=4";
    optimization2.currentBlockSize = 512;
    optimization2.recommendedBlockSize = 1024;
    optimization2.currentPreallocatedBlocks = 2;
    optimization2.recommendedPreallocatedBlocks = 4;
    optimization2.expectedFragmentationReduction = 20;
    optimization2.expectedAllocationSpeedup = 25;
    optimization2.requiresRuntimeChanges = true;
    optimization2.allocationPatterns = {"medium_allocation_rate", "irregular_sizes"};
    
    memoryPoolOptimizations_.push_back(optimization2);
    
    // Maintain history
    while (memoryPoolOptimizations_.size() > 100) {
        memoryPoolOptimizations_.erase(memoryPoolOptimizations_.begin());
    }
}

void ResourceOptimizer::analyzeResourceAllocation() {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    
    // Simulate resource allocation analysis
    ResourceAllocationOptimization optimization;
    optimization.type = ResourceAllocationOptimization::ResourceType::MEMORY;
    optimization.currentAllocation = 1024 * 1024 * 1024; // 1 GB
    optimization.recommendedAllocation = 2048 * 1024 * 1024; // 2 GB
    optimization.peakUsage = 1536 * 1024 * 1024; // 1.5 GB
    optimization.utilizationRate = 0.75;
    optimization.costSavings = 500.0; // $500 savings
    optimization.allocationStrategy = "elastic_scaling";
    optimization.scalingPolicy = "based_on_workload";
    optimization.isDynamicAllocation = true;
    
    resourceAllocations_.push_back(optimization);
    
    // Additional resource allocations
    ResourceAllocationOptimization optimization2;
    optimization2.type = ResourceAllocationOptimization::ResourceType::CPU;
    optimization2.currentAllocation = 4; // 4 cores
    optimization2.recommendedAllocation = 8; // 8 cores
    optimization2.peakUsage = 6;
    optimization2.utilizationRate = 0.75;
    optimization2.costSavings = 200.0; // $200 savings
    optimization2.allocationStrategy = "overcommit_and_throttle";
    optimization2.scalingPolicy = "based_on_cpu_load";
    optimization2.isDynamicAllocation = true;
    
    resourceAllocations_.push_back(optimization2);
    
    // Maintain history
    while (resourceAllocations_.size() > 100) {
        resourceAllocations_.erase(resourceAllocations_.begin());
    }
}

void ResourceOptimizer::generateSuggestions() {
    std::lock_guard<std::mutex> lock(suggestionsMutex_);
    
    // Generate SIMD suggestions
    for (const auto& opportunity : simdOpportunities_) {
        OptimizationSuggestion suggestion;
        suggestion.type = OptimizationSuggestion::OptimizationType::SIMD_VECTORIZATION;
        suggestion.description = "Implement " + opportunity.instructionSet + " vectorization for " + opportunity.operationName;
        suggestion.expectedImprovement = opportunity.expectedSpeedup / 10.0; // Normalize to 0-1
        suggestion.implementationCost = 0.3;
        suggestion.estimatedOverhead = 0;
        suggestion.impactArea = "compute_bound";
        suggestion.prerequisites = {"hardware_support_check", "alignment_fixup"};
        suggestion.suggestedImplementation = "Replace scalar loops with " + opportunity.instructionSet + " intrinsics";
        suggestion.timestamp = std::chrono::high_resolution_clock::now();
        suggestion.metadata["vector_width"] = std::to_string(opportunity.vectorWidth);
        suggestion.metadata["instruction_set"] = opportunity.instructionSet;
        suggestion.isImplemented = false;
        
        suggestions_.push_back(suggestion);
        
        // Call callbacks
        for (const auto& callback : optimizationCallbacks_) {
            callback(suggestion);
        }
    }
    
    // Generate cache optimization suggestions
    for (const auto& opportunity : cacheOptimizations_) {
        OptimizationSuggestion suggestion;
        suggestion.type = OptimizationSuggestion::OptimizationType::CACHE_OPTIMIZATION;
        suggestion.description = "Optimize cache layout for " + opportunity.dataStructure + " using " + opportunity.recommendedLayout;
        suggestion.expectedImprovement = (opportunity.cacheMissesReduced + opportunity.memoryAccessesImproved) / 100.0;
        suggestion.implementationCost = 0.5;
        suggestion.estimatedOverhead = 0;
        suggestion.impactArea = "memory_bound";
        suggestion.prerequisites = {"data_structure_redesign", "code_refactoring"};
        suggestion.suggestedImplementation = "Convert " + opportunity.currentLayout + " to " + opportunity.recommendedLayout;
        suggestion.timestamp = std::chrono::high_resolution_clock::now();
        suggestion.metadata["cache_miss_reduction"] = std::to_string(opportunity.cacheMissesReduced);
        suggestion.isImplemented = false;
        
        suggestions_.push_back(suggestion);
        
        // Call callbacks
        for (const auto& callback : optimizationCallbacks_) {
            callback(suggestion);
        }
    }
    
    // Generate parallel processing suggestions
    for (const auto& opportunity : parallelOptimizations_) {
        OptimizationSuggestion suggestion;
        suggestion.type = OptimizationSuggestion::OptimizationType::PARALLELIZATION;
        suggestion.description = "Parallelize " + opportunity.operationName + " using " + opportunity.recommendedParallelization;
        suggestion.expectedImprovement = opportunity.expectedParallelSpeedup / 8.0;
        suggestion.implementationCost = 0.4;
        suggestion.estimatedOverhead = 10; // Estimated thread overhead
        suggestion.impactArea = "compute_bound";
        suggestion.prerequisites = {"thread_pool_implementation", "synchronization_points"};
        suggestion.suggestedImplementation = "Use " + opportunity.parallelizationTechnique + " with " + 
                                             std::to_string(opportunity.recommendedThreadCount) + " threads";
        suggestion.timestamp = std::chrono::high_resolution_clock::now();
        suggestion.metadata["expected_speedup"] = std::to_string(opportunity.expectedParallelSpeedup);
        suggestion.isImplemented = false;
        
        suggestions_.push_back(suggestion);
        
        // Call callbacks
        for (const auto& callback : optimizationCallbacks_) {
            callback(suggestion);
        }
    }
    
    // Generate memory pool suggestions
    for (const auto& opportunity : memoryPoolOptimizations_) {
        OptimizationSuggestion suggestion;
        suggestion.type = OptimizationSuggestion::OptimizationType::MEMORY_POOL_TUNING;
        suggestion.description = "Tune memory pool " + opportunity.poolName + ": " + opportunity.recommendedConfiguration;
        suggestion.expectedImprovement = (opportunity.expectedFragmentationReduction + opportunity.expectedAllocationSpeedup) / 100.0;
        suggestion.implementationCost = opportunity.requiresRuntimeChanges ? 0.6 : 0.3;
        suggestion.estimatedOverhead = 0;
        suggestion.impactArea = "memory_management";
        suggestion.prerequisites = {"pool_resize_support", "allocation_tracking"};
        suggestion.suggestedImplementation = "Update pool configuration to block_size=" + std::to_string(opportunity.recommendedBlockSize) + ", "
                                             "initial_blocks=" + std::to_string(opportunity.recommendedPreallocatedBlocks);
        suggestion.timestamp = std::chrono::high_resolution_clock::now();
        suggestion.metadata["block_size_change"] = std::to_string(opportunity.recommendedBlockSize - opportunity.currentBlockSize);
        suggestion.metadata["fragmentation_reduction"] = std::to_string(opportunity.expectedFragmentationReduction);
        suggestion.isImplemented = false;
        
        suggestions_.push_back(suggestion);
        
        // Call callbacks
        for (const auto& callback : optimizationCallbacks_) {
            callback(suggestion);
        }
    }
    
    // Generate resource allocation suggestions
    for (const auto& optimization : resourceAllocations_) {
        OptimizationSuggestion suggestion;
        suggestion.type = OptimizationSuggestion::OptimizationType::RESOURCE_ALLOCATION;
        suggestion.description = "Optimize " + resourceTypeToString(optimization.type) + " allocation using " + optimization.allocationStrategy;
        suggestion.expectedImprovement = optimization.utilizationRate * 0.1; // 10% improvement
        suggestion.implementationCost = 0.2;
        suggestion.estimatedOverhead = 0;
        suggestion.impactArea = "resource_efficiency";
        suggestion.prerequisites = {"monitoring_infrastructure", "auto_scaling_setup"};
        suggestion.suggestedImplementation = "Implement " + optimization.scalingPolicy + " with " + optimization.allocationStrategy;
        suggestion.timestamp = std::chrono::high_resolution_clock::now();
        suggestion.metadata["resource_type"] = resourceTypeToString(optimization.type);
        suggestion.metadata["current_allocation"] = std::to_string(optimization.currentAllocation);
        suggestion.metadata["recommended_allocation"] = std::to_string(optimization.recommendedAllocation);
        suggestion.isImplemented = false;
        
        suggestions_.push_back(suggestion);
        
        // Call resource alert callbacks
        for (const auto& callback : resourceAlertCallbacks_) {
            callback(optimization);
        }
    }
    
    // Maintain suggestions limit
    while (suggestions_.size() > maxSuggestions_) {
        suggestions_.erase(suggestions_.begin());
    }
}

void ResourceOptimizer::updatePerformanceMetrics(const std::unordered_map<std::string, double>& metrics) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    performanceMetrics_ = metrics;
    
    // Add to historical metrics
    auto now = std::chrono::high_resolution_clock::now();
    historicalMetrics_.push_back(std::make_pair(now, metrics));
    
    // Maintain history size
    while (historicalMetrics_.size() > 1000) {
        historicalMetrics_.pop_front();
    }
}

std::vector<OptimizationSuggestion> ResourceOptimizer::getOptimizationSuggestions() const {
    std::lock_guard<std::mutex> lock(suggestionsMutex_);
    
    std::vector<OptimizationSuggestion> suggestions;
    
    // Sort by expected improvement (highest first)
    auto sortedSuggestions = suggestions_;
    std::sort(sortedSuggestions.begin(), sortedSuggestions.end(),
              [](const OptimizationSuggestion& a, const OptimizationSuggestion& b) {
                  return a.expectedImprovement > b.expectedImprovement;
              });
    
    suggestions = sortedSuggestions;
    
    return suggestions;
}

std::vector<SIMDOptimization> ResourceOptimizer::getSIMDOpportunities() const {
    std::lock_guard<std::mutex> lock(simdMutex_);
    return simdOpportunities_;
}

std::vector<CacheOptimization> ResourceOptimizer::getCacheOptimizations() const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    return cacheOptimizations_;
}

std::vector<ParallelOptimization> ResourceOptimizer::getParallelOptimizations() const {
    std::lock_guard<std::mutex> lock(parallelMutex_);
    return parallelOptimizations_;
}

std::vector<MemoryPoolOptimization> ResourceOptimizer::getMemoryPoolOptimizations() const {
    std::lock_guard<std::mutex> lock(poolMutex_);
    return memoryPoolOptimizations_;
}

std::vector<ResourceAllocationOptimization> ResourceOptimizer::getResourceAllocationOptimizations() const {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    return resourceAllocations_;
}

std::vector<std::string> ResourceOptimizer::getRecommendations(const std::string& area) const {
    std::lock_guard<std::mutex> lock(suggestionsMutex_);
    
    std::vector<std::string> recommendations;
    
    for (const auto& suggestion : suggestions_) {
        if (area.empty() || suggestion.suggestedImplementation.find(area) != std::string::npos) {
            recommendations.push_back(suggestion.description);
        }
    }
    
    return recommendations;
}

void ResourceOptimizer::addOptimizationCallback(const OptimizationCallback& callback) {
    std::lock_guard<std::mutex> lock(suggestionsMutex_);
    optimizationCallbacks_.push_back(callback);
}

void ResourceOptimizer::addResourceAlertCallback(const ResourceAlertCallback& callback) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    resourceAlertCallbacks_.push_back(callback);
}

ResourceOptimizer::OptimizationStats ResourceOptimizer::getOptimizationStats() const {
    std::lock_guard<std::mutex> lock(suggestionsMutex_);
    
    OptimizationStats stats;
    
    stats.totalSuggestionsGenerated = suggestions_.size();
    
    size_t implementedCount = 0;
    double totalImprovement = 0.0;
    double totalCost = 0.0;
    
    for (const auto& suggestion : suggestions_) {
        if (suggestion.isImplemented) {
            implementedCount++;
        }
        
        totalImprovement += suggestion.expectedImprovement;
        totalCost += suggestion.implementationCost;
    }
    
    stats.implementedSuggestions = implementedCount;
    stats.averageExpectedImprovement = suggestions_.empty() ? 0.0 : totalImprovement / suggestions_.size();
    stats.averageImplementationCost = suggestions_.empty() ? 0.0 : totalCost / suggestions_.size();
    
    if (!historicalMetrics_.empty()) {
        auto oldestTime = historicalMetrics_.front().first;
        auto newestTime = historicalMetrics_.back().first;
        stats.totalAnalysisTime = newestTime - oldestTime;
    }
    
    if (!historicalMetrics_.empty()) {
        stats.lastOptimizationTime = historicalMetrics_.back().first;
    }
    
    return stats;
}

void ResourceOptimizer::markSuggestionImplemented(const std::string& suggestionId) {
    std::lock_guard<std::mutex> lock(suggestionsMutex_);
    
    for (auto& suggestion : suggestions_) {
        // Simple implementation - in reality we'd need proper suggestion IDs
        if (!suggestion.isImplemented) {
            suggestion.isImplemented = true;
            break;
        }
    }
}

std::string ResourceOptimizer::getPerformanceProfile() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    std::stringstream profile;
    profile << "=== NLM Performance Profile ===" << std::endl;
    
    if (performanceMetrics_.empty()) {
        profile << "No performance metrics available." << std::endl;
        return profile.str();
    }
    
    profile << "Current Performance Metrics:" << std::endl;
    for (const auto& [metricName, value] : performanceMetrics_) {
        profile << "  " << metricName << ": " << value << std::endl;
    }
    
    profile << std::endl;
    
    // Calculate summary statistics
    double totalValue = 0.0;
    size_t count = 0;
    
    for (const auto& [metricName, value] : performanceMetrics_) {
        if (metricName.find("time") != std::string::npos) {
            totalValue += value;
            count++;
        }
    }
    
    if (count > 0) {
        profile << "Average Time Metrics: " << (totalValue / count) << std::endl;
    }
    
    return profile.str();
}

std::string ResourceOptimizer::generateReport(const std::string& format,
                                             bool includeHistoricalData) const {
    std::stringstream report;
    
    if (format == "text") {
        report << "=== NLM Resource Optimization Report ===" << std::endl;
        
        auto stats = getOptimizationStats();
        report << "Optimization Statistics:" << std::endl;
        report << "  Total Suggestions Generated: " << stats.totalSuggestionsGenerated << std::endl;
        report << "  Implemented Suggestions: " << stats.implementedSuggestions << std::endl;
        report << "  Average Expected Improvement: " << (stats.averageExpectedImprovement * 100) << "%" << std::endl;
        report << "  Average Implementation Cost: " << (stats.averageImplementationCost * 10) << "/10" << std::endl;
        report << std::endl;
        
        // Get top recommendations
        report << "Top Optimization Recommendations:" << std::endl;
        auto suggestions = getOptimizationSuggestions();
        for (size_t i = 0; i < std::min(size_t(5), suggestions.size()); ++i) {
            auto& suggestion = suggestions[i];
            report << "  " << (i + 1) << ". " << suggestion.description << std::endl;
            report << "     Expected Improvement: " << (suggestion.expectedImprovement * 100) << "%" << std::endl;
            report << "     Implementation Cost: " << (suggestion.implementationCost * 10) << "/10" << std::endl;
        }
        
    } else if (format == "json") {
        // JSON report generation
        report << "{\"resource_optimization_report\": {" << std::endl;
        
        auto stats = getOptimizationStats();
        report << "  \"optimization_stats\": {" << std::endl;
        report << "    \"total_suggestions_generated\": " << stats.totalSuggestionsGenerated << "," << std::endl;
        report << "    \"implemented_suggestions\": " << stats.implementedSuggestions << "," << std::endl;
        report << "    \"average_expected_improvement\": " << stats.averageExpectedImprovement << "," << std::endl;
        report << "    \"average_implementation_cost\": " << stats.averageImplementationCost << std::endl;
        report << "  }," << std::endl;
        
        report << "}";
    }
    
    return report.str();
}

void ResourceOptimizer::saveData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    
    // Save optimization data in a simple format
    file << "# NLM Resource Optimization Data" << std::endl;
    file << "# Generated: " << timestampToString(std::chrono::high_resolution_clock::now()) << std::endl;
    file << std::endl;
    
    // Write optimization suggestions
    file << "OPTIMIZATION_SUGGESTIONS:" << std::endl;
    for (const auto& suggestion : suggestions_) {
        file << "  " << suggestion.description << ": expected_improvement=" << suggestion.expectedImprovement << ", "
             << "implemented=" << (suggestion.isImplemented ? "true" : "false") << std::endl;
    }
}

void ResourceOptimizer::loadData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for reading: " + filename);
    }
    
    std::string line;
    std::string currentSuggestion;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line.find(":") != std::string::npos) {
            // Suggestion line
            currentSuggestion = line;
            OptimizationSuggestion suggestion;
            // Parse suggestion details (simplified)
            size_t colonPos = line.find(":");
            suggestion.description = line.substr(0, colonPos);
            std::string rest = line.substr(colonPos + 1);
            
            // Parse key-value pairs
            size_t eqPos;
            while ((eqPos = rest.find("=", 0)) != std::string::npos) {
                size_t commaPos = rest.find(",", 0);
                std::string key = rest.substr(0, eqPos);
                std::string value;
                
                if (commaPos != std::string::npos) {
                    value = rest.substr(eqPos + 1, commaPos - eqPos - 1);
                    rest = rest.substr(commaPos + 1);
                } else {
                    value = rest.substr(eqPos + 1);
                    rest = "";
                }
                
                if (key == "expected_improvement") {
                    suggestion.expectedImprovement = std::stod(value);
                } else if (key == "implemented") {
                    suggestion.isImplemented = (value == "true");
                }
            }
            
            suggestions_.push_back(suggestion);
        }
    }
}

void ResourceOptimizer::reset() {
    std::lock_guard<std::mutex> lock(suggestionsMutex_);
    
    suggestions_.clear();
    simdOpportunities_.clear();
    cacheOptimizations_.clear();
    parallelOptimizations_.clear();
    memoryPoolOptimizations_.clear();
    resourceAllocations_.clear();
    
    performanceMetrics_.clear();
    historicalMetrics_.clear();
}

std::string ResourceOptimizer::resourceTypeToString(ResourceAllocationOptimization::ResourceType type) const {
    switch (type) {
        case ResourceAllocationOptimization::ResourceType::MEMORY: return "memory";
        case ResourceAllocationOptimization::ResourceType::CPU: return "CPU";
        case ResourceAllocationOptimization::ResourceType::GPU: return "GPU";
        case ResourceAllocationOptimization::ResourceType::BANDWIDTH: return "bandwidth";
        default: return "unknown";
    }
}

std::string ResourceOptimizer::timestampToString(const std::chrono::high_resolution_clock::time_point& timestamp) const {
    auto duration = timestamp.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto milliseconds = std::chrono::duration_cast<std::milliseconds>(duration % std::chrono::seconds(1));
    
    std::stringstream ss;
    ss << seconds.count() << "." << std::setfill('0') << std::setw(3) << milliseconds.count();
    return ss.str();
}