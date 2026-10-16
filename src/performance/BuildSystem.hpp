// Build System Optimization and Profile-Guided Optimization
// 
// Provides comprehensive build system optimization for NLM performance,
// including compiler optimization flags, profile-guided optimization, and
// performance-tuned build configurations.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>

namespace nlm {

/**
 * Compiler Optimization Levels
 * 
 * Defines different optimization levels for the build system.
 */
enum class OptimizationLevel {
    None,           // No optimizations (debug)
    Debug,          // Debug with minimal optimizations
    Release,        // Standard release optimizations
    Optimized,      // Advanced optimizations
    ProfileGuided,  // Profile-guided optimization
    Maximum         // Maximum possible optimizations
};

/**
 * SIMD Configuration
 * 
 * Configuration for SIMD (Single Instruction, Multiple Data) optimizations.
 */
struct SIMDConfig {
    bool enable = true;
    std::string target = "AVX2";  // AVX2, SSE, AVX-512
    bool autoDetect = true;
    bool conservative = false;
    std::vector<std::string> features;
};

/**
 * Linker Configuration
 * 
 * Configuration for linker optimization and memory layout.
 */
struct LinkerConfig {
    bool enableLTO = true;             // Link Time Optimization
    bool enableFDO = false;            // Profile-Guided Optimization
    bool optimizeForSize = false;      // Optimize for binary size
    bool optimizeForSpeed = true;     // Optimize for execution speed
    std::string sectionOrdering = "default";
    bool mergeAllConstants = false;
};

/**
 * Build Configuration
 * 
 * Comprehensive build configuration for NLM performance optimization.
 */
struct BuildConfig {
    // General settings
    std::string projectName = "NLM";
    std::string version = "1.0.0";
    std::string buildType = "Release";
    std::string compiler = "g++";
    std::string standard = "c++17";
    
    // Optimization settings
    OptimizationLevel optimizationLevel = OptimizationLevel::Optimized;
    SIMDConfig simdConfig;
    LinkerConfig linkerConfig;
    
    // Performance settings
    bool enableProfiler = false;
    bool enableSanitizers = false;
    bool enableCoverage = false;
    std::string profileDir = "./profiles";
    
    // Parallel build settings
    size_t parallelJobs = std::thread::hardware_concurrency();
    bool useDistCC = false;
    std::string distCCHosts;
    
    // Platform-specific settings
    bool enableWarnings = true;
    std::vector<std::string> extraFlags;
    std::vector<std::string> systemLibraries;
    
    // Output settings
    std::string outputDirectory = "./bin";
    std::string libraryDirectory = "./lib";
    std::string objectDirectory = "./obj";
    
    // External dependencies
    std::map<std::string, std::string> dependencies;
};

/**
 * BuildSystem - High-performance build system for NLM
 * 
 * Provides comprehensive build system with optimization flags,
 * profile-guided optimization, and performance-tuned configurations.
 */
class BuildSystem {
public:
    BuildSystem(const BuildConfig& config);
    ~BuildSystem();
    
    // Disable copying
    BuildSystem(const BuildSystem&) = delete;
    BuildSystem& operator=(const BuildSystem&) = delete;
    
    /**
     * Generate build commands for different configurations
     */
    std::vector<std::string> generateBuildCommands(OptimizationLevel level) const;
    
    /**
     * Generate CMake configuration
     */
    std::string generateCMakeConfig() const;
    
    /**
     * Generate build scripts
     */
    void generateBuildScripts(const std::string& outputDir) const;
    
    /**
     * Run profile-guided optimization
     */
    bool runProfileGuidedOptimization(const std::string& profileDataDir, 
                                     const std::string& outputDir);
    
    /**
     * Collect profiling data
     */
    bool collectProfilingData(const std::string& executable,
                             const std::vector<std::string>& args,
                             const std::string& outputDir);
    
    /**
     * Generate optimization reports
     */
    std::string generateOptimizationReport() const;
    
    /**
     * Validate build configuration
     */
    bool validateConfig() const;
    
    /**
     * Get optimal configuration for platform
     */
    static BuildConfig getOptimalConfig(const std::string& platform = "auto");
    
    /**
     * Get compiler detection info
     */
    static std::string detectCompilerInfo();
    
    /**
     * Check for compiler support
     */
    static bool checkCompilerSupport(const std::string& flag);

private:
    /**
     * Generate compiler flags for optimization level
     */
    std::vector<std::string> generateCompilerFlags(OptimizationLevel level) const;
    
    /**
     * Generate linker flags for optimization level
     */
    std::vector<std::string> generateLinkerFlags(OptimizationLevel level) const;
    
    /**
     * Generate SIMD flags
     */
    std::vector<std::string> generateSIMDFlags() const;
    
    /**
     * Generate FDO (Profile-Guided Optimization) flags
     */
    std::vector<std::string> generateFDOFlags() const;
    
    /**
     * Generate sanitizer flags
     */
    std::vector<std::string> generateSanitizerFlags() const;
    
    /**
     * Generate coverage flags
     */n    std::vector<std::string> generateCoverageFlags() const;
    
    /**
     * Generate platform-specific flags
     */
    std::vector<std::string> generatePlatformFlags() const;
    
    /**
     * Run single command with timeout
     */
    bool runCommand(const std::string& command, const std::string& description,
                   double timeoutSeconds = 300.0) const;
    
    /**
     * Run command with profile data collection
     */
    bool runCommandWithProfile(const std::string& command,
                              const std::string& profileFile,
                              const std::string& description) const;
    
    /**
     * Parse compiler output for errors
     */
    std::vector<std::string> parseCompilerErrors(const std::string& output) const;
    
    /**
     * Generate build log entry
     */
    void logBuildStep(const std::string& step, const std::string& status,
                     double durationSeconds, bool success) const;
    
    /**
     * Check if build system is available
     */
    static bool checkBuildSystemAvailability();
    
    /**
     * Get current platform information
     */
    static std::string getPlatformInfo();
    
    /**
     * Get SIMD capabilities
     */
    static std::vector<std::string> detectSIMDCapabilities();

private:
    BuildConfig config_;
    mutable std::mutex buildMutex_;
    std::string buildLogFile_;
};

/**
 * CMakeBuildSystem - CMake-based build system for NLM
 * 
 * Provides CMake configuration generation and build automation.
 */
class CMakeBuildSystem : public BuildSystem {
public:
    explicit CMakeBuildSystem(const BuildConfig& config);
    ~CMakeBuildSystem() override;
    
    // Disable copying
    CMakeBuildSystem(const CMakeBuildSystem&) = delete;
    CMakeBuildSystem& operator=(const CMakeBuildSystem&) = delete;
    
    /**
     * Generate complete CMake configuration
     */
    std::string generateCompleteCMakeConfig() const;
    
    /**
     * Generate CMakeLists.txt with optimizations
     */
    void generateCMakeLists(const std::string& outputDir) const;
    
    /**
     * Generate CMakePresets.json for development
     */
    void generateCMakePresets(const std::string& outputDir) const;
    
    /**
     * Generate compile_commands.json for tooling
     */
    void generateCompileCommands(const std::string& outputDir) const;
    
    /**
     * Create build directory structure
     */
    void createBuildDirectoryStructure(const std::string& buildDir) const;
    
    /**
     * Get CMake generator for platform
     */
    static std::string getCMakeGenerator(const std::string& platform = "auto");
    
    /**
     * Get recommended CMake build type
     */
    static std::string getRecommendedCMakeBuildType(OptimizationLevel level);

private:
    /**
     * Generate CMake project configuration
     */
    std::string generateCMakeProjectConfig() const;
    
    /**
     * Generate CMake target configurations
     */
    std::string generateCMakeTargets() const;
    
    /**
     * Generate CMake compiler configuration
     */
    std::string generateCMakeCompilerConfig() const;
    
    /**
     * Generate CMake dependency configuration
     */
    std::string generateCMakeDependencyConfig() const;
    
    /**
     * Generate CMake optimization configuration
     */
    std::string generateCMakeOptimizationConfig() const;
    
    /**
     * Generate CMake installation configuration
     */
    std::string generateCMakeInstallationConfig() const;
    
    /**
     * Generate CMake testing configuration
     */
    std::string generateCMakeTestingConfig() const;
    
    /**
     * Generate CMake installation configuration
     */
    std::string generateCMakeInstallationConfig() const;
    
    /**
     * Generate CMake testing configuration
     */
    std::string generateCMakeTestingConfig() const;
    
    /**
     * Generate CMake installation configuration
     */
    std::string generateCMakeInstallationConfig() const;
    
    /**
     * Generate CMake testing configuration
     */
    std::string generateCMakeTestingConfig() const;
};

/**
 * MakefileGenerator - Makefile-based build system for NLM
 * 
 * Provides Makefile generation with optimization support.
 */
class MakefileGenerator {
public:
    MakefileGenerator(const BuildConfig& config);
    ~MakefileGenerator();
    
    // Disable copying
    MakefileGenerator(const MakefileGenerator&) = delete;
    MakefileGenerator& operator=(const MakefileGenerator&) = delete;
    
    /**
     * Generate Makefile
     */
    void generateMakefile(const std::string& outputDir) const;
    
    /**
     * Generate Makefile for specific target
     */
    void generateMakefileForTarget(const std::string& target,
                                  const std::string& outputDir) const;
    
    /**
     * Generate build script
     */
    void generateBuildScript(const std::string& outputDir) const;
    
    /**
     * Generate clean script
     */
    void generateCleanScript(const std::string& outputDir) const;
    
    /**
     * Get Makefile variables
     */
    static std::map<std::string, std::string> getMakefileVariables(const BuildConfig& config);
    
    /**
     * Get optimization flags for Makefile
     */
    static std::vector<std::string> getMakefileOptimizationFlags(OptimizationLevel level);
    
    /**
     * Get SIMD flags for Makefile
     */
    static std::vector<std::string> getMakefileSIMDFlags(const SIMDConfig& simdConfig);

private:
    BuildConfig config_;
    
    /**
     * Generate Makefile header
     */
    std::string generateMakefileHeader() const;
    
    /**
     * Generate Makefile targets
     */
    std::string generateMakefileTargets() const;
    
    /**
     * Generate Makefile variables section
     */
    std::string generateMakefileVariables() const;
    
    /**
     * Generate Makefile rules
     */
    std::string generateMakefileRules() const;
    
    /**
     * Generate Makefile dependencies
     */
    std::string generateMakefileDependencies() const;
    
    /**
     * Generate Makefile installation rules
     */
    std::string generateMakefileInstallationRules() const;
    
    /**
     * Generate Makefile test rules
     */
    std::string generateMakefileTestRules() const;
};

/**
 * ProfileAnalyzer - Profile data analysis for optimization
 * 
 * Provides analysis of profiling data to generate optimization recommendations.
 */
class ProfileAnalyzer {
public:
    ProfileAnalyzer();
    ~ProfileAnalyzer();
    
    // Disable copying
    ProfileAnalyzer(const ProfileAnalyzer&) = delete;
    ProfileAnalyzer& operator=(const ProfileAnalyzer&) = delete;
    
    /**
     * Analyze profile data
     */
    bool analyzeProfileData(const std::string& profileFile,
                           std::map<std::string, double>& profileData);
    
    /**
     * Generate optimization recommendations
     */
    std::vector<std::string> generateOptimizationRecommendations(
        const std::map<std::string, double>& profileData) const;
    
    /**
     * Get hot spots from profile data
     */
    std::vector<std::pair<std::string, double>> getHotSpots(
        const std::map<std::string, double>& profileData,
        size_t topCount = 10) const;
    
    /**
     * Get performance bottlenecks
     */
    std::vector<std::string> getPerformanceBottlenecks(
        const std::map<std::string, double>& profileData) const;
    
    /**
     * Get memory usage analysis
     */
    std::string getMemoryUsageAnalysis(
        const std::map<std::string, double>& profileData) const;
    
    /**
     * Get thread usage analysis
     */
    std::string getThreadUsageAnalysis(
        const std::map<std::string, double>& profileData) const;
    
    /**
     * Validate profile data
     */
    bool validateProfileData(const std::string& profileFile) const;
    
    /**
     * Export profile analysis
     */
    bool exportProfileAnalysis(const std::string& outputFile,
                              const std::string& profileFile) const;

private:
    /**
     * Parse profile data from common formats
     */
    std::map<std::string, double> parseProfileData(const std::string& profileFile) const;
    
    /**
     * Calculate hotspots from profile data
     */
    std::vector<std::pair<std::string, double>> calculateHotspots(
        const std::map<std::string, double>& profileData) const;
    
    /**
     * Analyze cache performance
     */
    std::string analyzeCachePerformance(
        const std::map<std::string, double>& profileData) const;
    
    /**
     * Analyze branch prediction
     */
    std::string analyzeBranchPrediction(
        const std::map<std::string, double>& profileData) const;
    
    /**
     * Analyze memory hierarchy
     */
    std::string analyzeMemoryHierarchy(
        const std::map<std::string, double>& profileData) const;
};

/**
 * OptimizationProfiler - Profiling for optimization validation
 * 
 * Provides profiling infrastructure for validating optimization effectiveness.
 */
class OptimizationProfiler {
public:
    OptimizationProfiler(const std::string& name);
    ~OptimizationProfiler();
    
    // Disable copying
    OptimizationProfiler(const OptimizationProfiler&) = delete;
    OptimizationProfiler& operator=(const OptimizationProfiler&) = delete;
    
    /**
     * Start profiling a function
     */
    void startProfiling(const std::string& functionName);
    
    /**
     * Stop profiling a function
     */
    void stopProfiling(const std::string& functionName);
    
    /**
     * Record function execution time
     */
    void recordFunctionTime(const std::string& functionName, double timeSeconds);
    
    /**
     * Record memory allocation
     */
    void recordMemoryAllocation(size_t bytes, const std::string& allocator = "default");
    
    /**
     * Record cache performance
     */
    void recordCachePerformance(double hits, double misses, double size);
    
    /**
     * Record branch prediction statistics
     */
    void recordBranchPredictionStats(double correct, double mispredicted);
    
    /**
     * Get profiling results
     */
    std::map<std::string, double> getProfilingResults() const;
    
    /**
     * Get performance statistics
     */
    std::string getPerformanceStatistics() const;
    
    /**
     * Get optimization report
     */
    std::string getOptimizationReport() const;
    
    /**
     * Reset profiling data
     */
    void reset();
    
    /**
     * Export profiling data
     */
    bool exportProfilingData(const std::string& outputFile) const;
    
    /**
     * Import profiling data
     */
    bool importProfilingData(const std::string& inputFile);

private:
    struct ProfilingData {
        std::map<std::string, std::vector<double>> functionTimes;
        std::vector<double> memoryAllocations;
        std::vector<std::tuple<double, double, double>> cacheStats;
        std::vector<std::pair<double, double>> branchStats;
        double totalTime = 0.0;
        size_t totalCalls = 0;
    };
    
    mutable std::mutex profilerMutex_;
    std::string profilerName_;
    ProfilingData data_;
    std::chrono::high_resolution_clock::time_point sessionStart_;
};

/**
 * BuildAnalyzer - Build analysis and optimization recommendations
 * 
 * Provides analysis of build configuration and optimization recommendations.
 */
class BuildAnalyzer {
public:
    BuildAnalyzer();
    ~BuildAnalyzer();
    
    // Disable copying
    BuildAnalyzer(const BuildAnalyzer&) = delete;
    BuildAnalyzer& operator=(const BuildAnalyzer&) = delete;
    
    /**
     * Analyze build configuration
     */
    bool analyzeBuildConfig(const BuildConfig& config);
    
    /**
     * Generate optimization recommendations
     */
    std::vector<std::string> generateBuildRecommendations(const BuildConfig& config) const;
    
    /**
     * Get performance impact analysis
     */
    std::string getPerformanceImpactAnalysis(const BuildConfig& config) const;
    
    /**
     * Get optimization potential score
     */
    double getOptimizationPotentialScore(const BuildConfig& config) const;
    
    /**
     * Get platform compatibility analysis
     */
    std::string getPlatformCompatibilityAnalysis(const BuildConfig& config) const;
    
    /**
     * Get dependency analysis
     */
    std::string getDependencyAnalysis(const BuildConfig& config) const;
    
    /**
     * Validate build configuration
     */
    bool validateBuildConfig(const BuildConfig& config) const;
    
    /**
     * Check for potential conflicts
     */
    std::vector<std::string> checkForConflicts(const BuildConfig& config) const;
    
    /**
     * Get recommended optimization levels
     */
    std::vector<OptimizationLevel> getRecommendedOptimizationLevels(const BuildConfig& config) const;
    
    /**
     * Compare optimization levels
     */
    std::string compareOptimizationLevels(const BuildConfig& config) const;
};

/**
 * BuildSystemFactory - Factory for creating build systems
 * 
 * Provides factory methods for creating different build system types.
 */
class BuildSystemFactory {
public:
    /**
     * Create build system based on configuration
     */
    static std::unique_ptr<BuildSystem> createBuildSystem(const BuildConfig& config,
                                                      const std::string& type = "auto");
    
    /**
     * Create CMake build system
     */
    static std::unique_ptr<CMakeBuildSystem> createCMakeBuildSystem(const BuildConfig& config);
    
    /**
     * Create Makefile build system
     */
    static std::unique_ptr<MakefileGenerator> createMakefileBuildSystem(const BuildConfig& config);
    
    /**
     * Create profile analyzer
     */
    static std::unique_ptr<ProfileAnalyzer> createProfileAnalyzer();
    
    /**
     * Create optimization profiler
     */
    static std::unique_ptr<OptimizationProfiler> createOptimizationProfiler(const std::string& name);
    
    /**
     * Create build analyzer
     */
    static std::unique_ptr<BuildAnalyzer> createBuildAnalyzer();
    
    /**
     * Get available build system types
     */
    static std::vector<std::string> getAvailableBuildSystemTypes();
    
    /**
     * Get build system type for platform
     */
    static std::string getBuildSystemForPlatform(const std::string& platform = "auto");
    
    /**
     * Create build configuration for optimization level
     */
    static BuildConfig createBuildConfigForOptimizationLevel(OptimizationLevel level);
    
    /**
     * Create build configuration for platform
     */
    static BuildConfig createBuildConfigForPlatform(const std::string& platform = "auto");
};

// Convenience functions
inline BuildConfig getOptimizedBuildConfig(OptimizationLevel level = OptimizationLevel::Optimized) {
    return BuildSystemFactory::createBuildConfigForOptimizationLevel(level);
}

inline BuildConfig getPlatformBuildConfig(const std::string& platform = "auto") {
    return BuildSystemFactory::createBuildConfigForPlatform(platform);
}

inline std::unique_ptr<BuildSystem> createBuildSystem(const BuildConfig& config,
                                                  const std::string& type = "auto") {
    return BuildSystemFactory::createBuildSystem(config, type);
}

} // namespace nlm

// Global build system instances
namespace nlm {

inline BuildSystem& getDefaultBuildSystem() {
    static BuildSystem instance(BuildSystemFactory::createBuildConfigForPlatform());
    return instance;
}

inline ProfileAnalyzer& getDefaultProfileAnalyzer() {
    static ProfileAnalyzer analyzer;
    return analyzer;
}

inline BuildAnalyzer& getDefaultBuildAnalyzer() {
    static BuildAnalyzer analyzer;
    return analyzer;
}

} // namespace nlm

// Build system usage example
namespace nlm {

inline void demonstrateBuildOptimization() {
    // Create optimized build configuration
    BuildConfig config = BuildSystemFactory::getOptimizedBuildConfig(OptimizationLevel::Optimized);
    
    // Create build system
    auto buildSystem = BuildSystemFactory::createBuildSystem(config);
    
    // Validate configuration
    if (buildSystem->validateConfig()) {
        std::cout << "Build configuration validated successfully" << std::endl;
        
        // Generate build commands
        auto commands = buildSystem->generateBuildCommands(OptimizationLevel::Optimized);
        
        for (const auto& cmd : commands) {
            std::cout << "Build command: " << cmd << std::endl;
        }
        
        // Generate CMake configuration
        std::string cmakeConfig = buildSystem->generateCMakeConfig();
        std::cout << "CMake configuration generated" << std::endl;
        
        // Generate optimization report
        std::string report = buildSystem->generateOptimizationReport();
        std::cout << "Optimization report:" << std::endl;
        std::cout << report << std::endl;
    } else {
        std::cout << "Build configuration validation failed" << std::endl;
    }
}

} // namespace nlm
