// ==============================================================================
// Memory Visualizer
// ==============================================================================
#ifndef MEMORYVISUALIZER_HPP
#define MEMORYVISUALIZER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

namespace nlm {
namespace debug {

/**
 * @class MemoryVisualizer
 * @brief Memory visualization tool for NLM
 * 
 * Provides tools for visualizing memory systems including working memory,
 * episodic memory, and memory allocation patterns.
 */
class MemoryVisualizer {
public:
    /**
     * @enum MemoryVisualizationType
     * @brief Type of memory visualization
     */
    enum class MemoryVisualizationType {
        MEMORY_ALLOCATION,
        MEMORY_USAGE,
        MEMORY_PATTERNS,
        MEMORY_ACCESS_PATTERNS,
        MEMORY_CONSOLIDATION,
        MEMORY_REPLAY,
        MEMORY_COMPACTION,
        MEMORY_LEAKAGE,
        MEMORY_EFFICIENCY,
        TEMPORAL_SYNCHRONIZATION
    };

    /**
     * @enum MemoryType
     * @brief Type of memory
     */
    enum class MemoryType {
        WORKING,
        EPISODIC,
        SEMANTIC,
        PROCEEDURAL,
        ASSOCIATIVE
    };

    /**
     * @struct MemoryData
     * @brief Memory data
     */
    struct MemoryData {
        std::chrono::steady_clock::time_point timestamp;
        uint64_t totalMemory;
        uint64_t usedMemory;
        uint64_t availableMemory;
        uint64_t allocatedChunks;
        uint64_t freedChunks;
        double memoryUtilization;
        double memoryEfficiency;
        std::map<std::string, double> memoryComponents;
    };

    /**
     * @struct MemoryAllocation
     * @brief Memory allocation
     */
    struct MemoryAllocation {
        std::string id;
        uint64_t address;
        uint64_t size;
        std::chrono::steady_clock::time_point allocationTime;
        std::chrono::steady_clock::time_point deallocationTime;
        std::string allocator;
        std::map<std::string, std::string> metadata;
    };

    MemoryVisualizer();
    ~MemoryVisualizer();

    /**
     * @brief Visualize memory system
     * @param memory Memory system reference
     * @param filename Output file path
     * @param type Visualization type
     * @return true if visualization was created
     */
    bool visualize(const Memory& memory, const std::string& filename, MemoryVisualizationType type = MemoryVisualizationType::MEMORY_USAGE);

    /**
     * @brief Visualize working memory
     * @param workingMemory Working memory reference
     * @param filename Output file path
     * @param type Visualization type
     * @return true if visualization was created
     */
    bool visualize(const NeuralWorkingMemory& workingMemory, const std::string& filename, MemoryVisualizationType type = MemoryVisualizationType::MEMORY_ALLOCATION);

    /**
     * @brief Visualize episodic memory
     * @param episodicMemory Episodic memory reference
     * @param filename Output file path
     * @param type Visualization type
     * @return true if visualization was created
     */
    bool visualize(const NeuralEpisodicMemory& episodicMemory, const std::string& filename, MemoryVisualizationType type = MemoryVisualizationType::MEMORY_PATTERNS);

    /**
     * @brief Generate memory usage report
     * @param memory Memory system reference
     * @return Memory usage report
     */
    struct MemoryUsageReport {
        uint64_t totalMemory;
        uint64_t peakMemory;
        uint64_t currentMemory;
        double utilizationRate;
        uint64_t allocationCount;
        uint64_t deallocationCount;
        double averageAllocationSize;
        double averageDeallocationSize;
        std::chrono::steady_clock::time_point lastAllocation;
        std::chrono::steady_clock::time_point lastDeallocation;
    };

    MemoryUsageReport generateMemoryUsageReport(const Memory& memory);

    /**
     * @brief Generate memory allocation timeline
     * @param memory Memory system reference
     * @param duration Duration
     * @return Memory allocation timeline
     */
    std::vector<MemoryData> generateMemoryAllocationTimeline(const Memory& memory, double duration = 60.0);

    /**
     * @brief Generate memory allocation heatmap
     * @param memory Memory system reference
     * @return Memory allocation heatmap
     */
    std::vector<std::vector<double>> generateMemoryAllocationHeatmap(const Memory& memory);

    /**
     * @brief Generate memory access patterns
     * @param memory Memory system reference
     * @param duration Duration
     * @return Memory access patterns
     */
    std::map<std::string, std::vector<double>> generateMemoryAccessPatterns(const Memory& memory, double duration = 60.0);

    /**
     * @brief Generate memory consolidation visualization
     * @param episodicMemory Episodic memory reference
     * @param consolidationTime Consolidation time
     * @return Consolidation visualization
     */
    std::vector<std::pair<std::chrono::steady_clock::time_point, double>> generateMemoryConsolidationVisualization(const NeuralEpisodicMemory& episodicMemory, double consolidationTime = 3600.0);

    /**
     * @brief Generate memory replay visualization
     * @param episodicMemory Episodic memory reference
     * @param replayStartTime Replay start time
     * @param replayEndTime Replay end time
     * @return Replay visualization
     */
    std::vector<std::pair<std::chrono::steady_clock::time_point, double>> generateMemoryReplayVisualization(const NeuralEpisodicMemory& episodicMemory, std::chrono::steady_clock::time_point replayStartTime, std::chrono::steady_clock::time_point replayEndTime);

    /**
     * @brief Detect memory leaks
     * @param memory Memory system reference
     * @param threshold Leak threshold
     * @return Vector of potential memory leaks
     */
    std::vector<std::string> detectMemoryLeaks(const Memory& memory, double threshold = 1.0);

    /**
     * @brief Generate memory compaction visualization
     * @param memory Memory system reference
     * @param compactionIterations Compaction iterations
     * @return Compaction visualization
     */
    std::vector<std::vector<MemoryAllocation>> generateMemoryCompactionVisualization(const Memory& memory, size_t compactionIterations = 10);

    /**
     * @brief Generate memory efficiency analysis
     * @param memory Memory system reference
     * @return Memory efficiency analysis
     */
    struct MemoryEfficiencyAnalysis {
        double fragmentationIndex;
        double utilizationEfficiency;
        double allocationEfficiency;
        double deallocationEfficiency;
        double memoryAccessEfficiency;
        double overallEfficiencyScore;
    };

    MemoryEfficiencyAnalysis generateMemoryEfficiencyAnalysis(const Memory& memory);

    /**
     * @brief Generate memory timeline visualization
     * @param memory Memory system reference
     * @param filename Output file path
     */
    void generateMemoryTimelineVisualization(const Memory& memory, const std::string& filename);

    /**
     * @brief Generate memory heatmap visualization
     * @param memory Memory system reference
     * @param filename Output file path
     */
    void generateMemoryHeatmapVisualization(const Memory& memory, const std::string& filename);

    /**
     * @brief Generate memory allocation visualization
     * @param memory Memory system reference
     * @param filename Output file path
     */
    void generateMemoryAllocationVisualization(const Memory& memory, const std::string& filename);

    /**
     * @brief Get memory visualization types
     * @return Vector of memory visualization types
     */
    std::vector<MemoryVisualizationType> getAvailableVisualizationTypes() const;

    /**
     * @brief Get memory types
     * @return Vector of memory types
     */
    std::vector<MemoryType> getAvailableMemoryTypes() const;

    /**
     * @brief Format memory data for display
     * @param data Memory data
     * @return Formatted string
     */
    std::string formatMemoryData(const MemoryData& data) const;

    /**
     * @brief Format memory allocation for display
     * @param allocation Memory allocation
     * @return Formatted string
     */
    std::string formatMemoryAllocation(const MemoryAllocation& allocation) const;

    /**
     * @brief Format memory usage report for display
     * @param report Memory usage report
     * @return Formatted string
     */
    std::string formatMemoryUsageReport(const MemoryUsageReport& report) const;

protected:
    /**
     * @brief Create allocation visualization
     * @param memory Memory system reference
     * @param filename Output file path
     */
    void createAllocationVisualization(const Memory& memory, const std::string& filename);

    /**
     * @brief Create usage visualization
     * @param memory Memory system reference
     * @param filename Output file path
     */
    void createUsageVisualization(const Memory& memory, const std::string& filename);

    /**
     * @brief Create pattern visualization
     * @param memory Memory system reference
     * @param filename Output file path
     */
    void createPatternVisualization(const Memory& memory, const std::string& filename);

    /**
     * @brief Create timeline visualization
     * @param memory Memory system reference
     * @param filename Output file path
     */
    void createTimelineVisualization(const Memory& memory, const std::string& filename);

    /**
     * @brief Create heatmap visualization
     * @param memory Memory system reference
     * @param filename Output file path
     */
    void createHeatmapVisualization(const Memory& memory, const std::string& filename);

    /**
     * @brief Calculate memory efficiency
     * @param memory Memory system reference
     * @return Memory efficiency
     */
    double calculateMemoryEfficiency(const Memory& memory) const;

    /**
     * @brief Calculate fragmentation index
     * @param memory Memory system reference
     * @return Fragmentation index
     */
    double calculateFragmentationIndex(const Memory& memory) const;

    /**
     * @brief Calculate utilization efficiency
     * @param memory Memory system reference
     * @return Utilization efficiency
     */
    double calculateUtilizationEfficiency(const Memory& memory) const;

    /**
     * @brief Calculate allocation efficiency
     * @param memory Memory system reference
     * @return Allocation efficiency
     */
    double calculateAllocationEfficiency(const Memory& memory) const;

    /**
     * @brief Calculate deallocation efficiency
     * @param memory Memory system reference
     * @return Deallocation efficiency
     */
    double calculateDeallocationEfficiency(const Memory& memory) const;

    /**
     * @brief Generate color map
     * @param value Value to map
     * @return Color
     */
    std::string generateColorMap(double value) const;

    /**
     * @brief Generate size map
     * @param value Value to map
     * @return Size
     */
    double generateSizeMap(double value) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // MEMORYVISUALIZER_HPP
