// Build System Enhancements and Debugging Tools
// Comprehensive build system improvements and debugging infrastructure for NLM advanced features

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>

namespace nlm {
namespace build {

// Build configuration types
enum class BuildConfiguration {
    Debug,
    Release,
    RelWithDebInfo,
    MinSizeRel
};

// Build target types
enum class BuildTarget {
    All,
    Core,
    Plugins,
    Factories,
    NeuronModels,
    PlasticityRules,
    LearningAlgorithms,
    Examples,
    Tests
};

// Build system options
struct BuildOptions {
    BuildConfiguration config = BuildConfiguration::Release;
    bool enablePlugins = true;
    bool enableFactories = true;
    bool enableTests = true;
    bool enableBenchmarks = true;
    bool enableDocumentation = true;
    bool enableProfiling = false;
    bool enableInstrumentation = false;
    std::string buildDirectory = "build";
    std::string installDirectory = "install";
    std::string outputDirectory = "output";
    std::vector<std::string> additionalFlags;
    std::vector<std::string> libraryPaths;
    std::vector<std::string> includePaths;
    std::unordered_map<std::string, std::string> definitions;
    
    // Performance options
    size_t parallelJobs = std::thread::hardware_concurrency();
    bool useDistcc = false;
    bool useDistccMirror = false;
    std::string distccHost = "distcc-server";
    
    // Component-specific options
    struct PluginOptions {
        bool buildAllPlugins = true;
        std::vector<std::string> enabledPlugins;
        std::vector<std::string> disabledPlugins;
        bool buildExamples = true;
        std::string pluginDirectory = "plugins";
    } pluginOptions;
    
    struct FactoryOptions {
        bool buildAllFactories = true;
        std::vector<std::string> enabledFactories;
        std::vector<std::string> disabledFactories;
        std::string factoryDirectory = "factories";
    } factoryOptions;
    
    struct TestOptions {
        bool buildUnitTests = true;
        bool buildIntegrationTests = true;
        bool buildPerformanceTests = true;
        bool runTestsAutomatically = true;
        std::string testDirectory = "tests";
        std::string benchmarkDirectory = "benchmarks";
    } testOptions;
    
    struct DocumentationOptions {
        bool buildAPIReference = true;
        bool buildUserGuide = true;
        bool buildExamplesDocumentation = true;
        bool buildTutorials = true;
        std::string docOutputFormat = "html";
        std::string docTheme = "default";
    } documentationOptions;
};

// Build target dependency
struct BuildTargetInfo {
    BuildTarget target;
    std::string name;
    std::string description;
    std::vector<BuildTarget> dependencies;
    std::function<bool(const BuildOptions&)> buildCondition;
    std::function<void(const BuildOptions&)> preBuildActions;
    std::function<void(const BuildOptions&)> postBuildActions;
};

// Build step information
struct BuildStep {
    std::string name;
    std::string command;
    std::vector<std::string> arguments;
    std::chrono::milliseconds estimatedDuration;
    bool isCritical;
    std::function<bool(const BuildOptions&)> skipCondition;
    std::function<void()> preStepActions;
    std::function<void()> postStepActions;
};

// Build statistics
struct BuildStats {
    double totalTime;
    size_t stepsCompleted;
    size_t stepsFailed;
    size_t totalFilesGenerated;
    size_t totalFilesModified;
    std::unordered_map<std::string, double> stepTimes;
    std::unordered_map<std::string, std::string> generatedFiles;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
};

// Build system interface
class IBuildSystem {
public:
    virtual ~IBuildSystem() = default;
    
    // Build configuration
    virtual bool configure(const BuildOptions& options) = 0;
    virtual BuildOptions getConfiguration() const = 0;
    
    // Build execution
    virtual bool build(const std::vector<BuildTarget>& targets) = 0;
    virtual bool build(BuildTarget target) = 0;
    virtual bool clean() = 0;
    virtual bool rebuild() = 0;
    
    // Build queries
    virtual bool isBuilt(BuildTarget target) const = 0;
    virtual std::vector<BuildTarget> getBuiltTargets() const = 0;
    virtual std::vector<BuildTarget> getAvailableTargets() const = 0;
    
    // Build statistics
    virtual BuildStats getBuildStats() const = 0;
    virtual std::string getBuildLog() const = 0;
    
    // Dependency management
    virtual bool addDependency(const std::string& target, const std::string& dependency) = 0;
    virtual bool removeDependency(const std::string& target, const std::string& dependency) = 0;
    virtual std::vector<std::string> getDependencies(BuildTarget target) const = 0;
    
    // Environment setup
    virtual bool setupEnvironment() = 0;
    virtual bool validateEnvironment() const = 0;
};

// CMake-based build system
class CMakeBuildSystem : public IBuildSystem {
public:
    CMakeBuildSystem();
    ~CMakeBuildSystem() override = default;
    
    bool configure(const BuildOptions& options) override;
    BuildOptions getConfiguration() const override;
    
    bool build(const std::vector<BuildTarget>& targets) override;
    bool build(BuildTarget target) override;
    bool clean() override;
    bool rebuild() override;
    
    bool isBuilt(BuildTarget target) const override;
    std::vector<BuildTarget> getBuiltTargets() const override;
    std::vector<BuildTarget> getAvailableTargets() const override;
    
    BuildStats getBuildStats() const override;
    std::string getBuildLog() const override;
    
    bool addDependency(const std::string& target, const std::string& dependency) override;
    bool removeDependency(const std::string& target, const std::string& dependency) override;
    std::vector<std::string> getDependencies(BuildTarget target) const override;
    
    bool setupEnvironment() override;
    bool validateEnvironment() const override;
    
    // CMake-specific functionality
    bool generateCMakeLists(const BuildOptions& options);
    bool configureCMake(const BuildOptions& options);
    bool runCMakeBuild(const std::vector<BuildTarget>& targets);
    bool installBuild(const BuildOptions& options);
    
    // Component generation
    bool generatePluginCMake(const std::string& pluginName,
                             const BuildOptions::PluginOptions& pluginOptions);
    bool generateFactoryCMake(const std::string& factoryName,
                              const BuildOptions::FactoryOptions& factoryOptions);
    bool generateTestCMake(const std::string& testName,
                           const BuildOptions::TestOptions& testOptions);
    
    // Platform-specific builds
    bool buildForPlatform(const std::string& platform,
                          const BuildOptions& options);
    bool crossCompile(const std::string& targetArch,
                      const BuildOptions& options);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Makefile-based build system
class MakefileBuildSystem : public IBuildSystem {
public:
    MakefileBuildSystem();
    ~MakefileBuildSystem() override = default;
    
    bool configure(const BuildOptions& options) override;
    BuildOptions getConfiguration() const override;
    
    bool build(const std::vector<BuildTarget>& targets) override;
    bool build(BuildTarget target) override;
    bool clean() override;
    bool rebuild() override;
    
    bool isBuilt(BuildTarget target) const override;
    std::vector<BuildTarget> getBuiltTargets() const override;
    std::vector<BuildTarget> getAvailableTargets() const override;
    
    BuildStats getBuildStats() const override;
    std::string getBuildLog() const override;
    
    bool addDependency(const std::string& target, const std::string& dependency) override;
    bool removeDependency(const std::string& target, const std::string& dependency) override;
    std::vector<std::string> getDependencies(BuildTarget target) const override;
    
    bool setupEnvironment() override;
    bool validateEnvironment() const override;
    
    // Makefile-specific functionality
    bool generateMakefiles(const BuildOptions& options);
    bool generateComponentMakefiles(const BuildOptions& options);
    bool generateTestMakefiles(const BuildOptions& options);
    
    // Build step execution
    bool executeBuildStep(const BuildStep& step, const BuildOptions& options);
    bool executeParallelBuildSteps(const std::vector<BuildStep>& steps,
                                   const BuildOptions& options);
    
    // Dependency resolution
    std::vector<BuildStep> resolveBuildSteps(BuildTarget target);
    std::vector<BuildStep> resolveDependencies(BuildTarget target);
    
    // Platform-specific build scripts
    std::string generateBuildScript(const BuildOptions& options);
    bool executeBuildScript(const std::string& scriptPath);
    
    // Installation and packaging
    bool createInstallScripts(const BuildOptions& options);
    bool createPackageFiles(const BuildOptions& options);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Ninja build system
class NinjaBuildSystem : public IBuildSystem {
public:
    NinjaBuildSystem();
    ~NinjaBuildSystem() override = default;
    
    bool configure(const BuildOptions& options) override;
    BuildOptions getConfiguration() const override;
    
    bool build(const std::vector<BuildTarget>& targets) override;
    bool build(BuildTarget target) override;
    bool clean() override;
    bool rebuild() override;
    
    bool isBuilt(BuildTarget target) const override;
    std::vector<BuildTarget> getBuiltTargets() const override;
    std::vector<BuildTarget> getAvailableTargets() const override;
    
    BuildStats getBuildStats() const override;
    std::string getBuildLog() const override;
    
    bool addDependency(const std::string& target, const std::string& dependency) override;
    bool removeDependency(const std::string& target, const std::string& dependency) override;
    std::vector<std::string> getDependencies(BuildTarget target) const override;
    
    bool setupEnvironment() override;
    bool validateEnvironment() const override;
    
    // Ninja-specific functionality
    bool generateNinjaBuild(const BuildOptions& options);
    bool generateNinjaRules(const BuildOptions& options);
    bool generateNinjaDeps(const BuildOptions& options);
    
    // Fast build optimization
    bool optimizeBuild(const BuildOptions& options);
    bool generateBuildManifest(const BuildOptions& options);
    
    // Parallel build
    bool executeParallelBuild(const std::vector<BuildStep>& steps,
                              const BuildOptions& options);
    bool generateNinjaParallelCommands(const std::vector<BuildStep>& steps);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Build system factory
class BuildSystemFactory {
public:
    static BuildSystemFactory& getInstance();
    
    // Build system creation
    std::unique_ptr<IBuildSystem> createBuildSystem(const std::string& type);
    
    // Available build systems
    std::vector<std::string> getAvailableBuildSystems() const;
    std::string getDefaultBuildSystem() const;
    
    // Configuration templates
    BuildOptions getBuildOptions(BuildConfiguration config);
    BuildOptions getPluginBuildOptions(const BuildOptions::PluginOptions& pluginOptions);
    BuildOptions getFactoryBuildOptions(const BuildOptions::FactoryOptions& factoryOptions);
    BuildOptions getTestBuildOptions(const BuildOptions::TestOptions& testOptions);
    
    // Component build options
    BuildOptions getNeuronModelBuildOptions(const std::string& modelType);
    BuildOptions getPlasticityRuleBuildOptions(const std::string& ruleType);
    BuildOptions getLearningAlgorithmBuildOptions(const std::string& algorithmType);
    
    // Platform-specific configurations
    BuildOptions getPlatformBuildOptions(const std::string& platform);
    BuildOptions getCrossCompileOptions(const std::string& targetArch);
    
    // Performance optimization
    BuildOptions getPerformanceOptimizedOptions(BuildConfiguration config);
    BuildOptions getMemoryOptimizedOptions(BuildConfiguration config);
    BuildOptions getSizeOptimizedOptions(BuildConfiguration config);
    
private:
    BuildSystemFactory() = default;
    ~BuildSystemFactory() = default;
    
    // Prevent copying
    BuildSystemFactory(const BuildSystemFactory&) = delete;
    BuildSystemFactory& operator=(const BuildSystemFactory&) = delete;
};

// Build utilities
namespace utils {
    // Build configuration utilities
    std::string buildConfigurationToString(BuildConfiguration config);
    BuildConfiguration stringToBuildConfiguration(const std::string& str);
    std::string buildTargetToString(BuildTarget target);
    BuildTarget stringToBuildTarget(const std::string& str);
    
    // Build system detection
    std::string detectBuildSystem(const std::string& directory);
    bool isCMakeAvailable();
    bool isNinjaAvailable();
    bool isMakeAvailable();
    
    // Platform detection
    std::string detectPlatform();
    std::string detectArchitecture();
    std::string getCompilerVersion();
    
    // Build step utilities
    std::vector<BuildStep> parseBuildCommands(const std::string& makefile);
    std::string generateBuildCommand(const BuildStep& step);
    bool validateBuildStep(const BuildStep& step);
    
    // Dependency utilities
    bool checkDependencies(const BuildOptions& options);
    bool installDependencies(const BuildOptions& options);
    std::vector<std::string> getSystemDependencies();
    
    // Thread-safe build operations
    template<typename Func>
    auto threadSafeBuildOperation(Func&& func) -> decltype(func()) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        return func();
    }
    
    // Build log and statistics utilities
    void logBuildMessage(const std::string& message, const std::string& level = "INFO");
    void logBuildError(const std::string& error);
    void logBuildWarning(const std::string& warning);
    
    std::string formatBuildStats(const BuildStats& stats);
    std::string generateBuildReport(const BuildStats& stats);
}

// Build cache system
class BuildCache {
public:
    BuildCache();
    ~BuildCache();
    
    // Cache management
    bool addToCache(const std::string& key, const std::string& value);
    bool getFromCache(const std::string& key, std::string& value) const;
    bool removeFromCache(const std::string& key);
    bool clearCache();
    
    // Dependency cache
    bool cacheDependencies(const std::vector<std::string>& dependencies);
    std::vector<std::string> getCachedDependencies() const;
    bool clearDependenciesCache();
    
    // Build result cache
    bool cacheBuildResult(BuildTarget target, const std::string& result);
    std::string getCachedBuildResult(BuildTarget target) const;
    bool isBuildCached(BuildTarget target) const;
    
    // Performance cache
    bool cachePerformanceMetrics(const std::string& operation, double time);
    double getCachedPerformanceMetrics(const std::string& operation) const;
    
    // Configuration cache
    bool cacheConfiguration(const BuildOptions& config);
    BuildOptions getCachedConfiguration() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Build logger
class BuildLogger {
public:
    BuildLogger();
    ~BuildLogger();
    
    // Logging interface
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    void logSuccess(const std::string& message);
    
    // Level-based logging
    void logMessage(const std::string& level, const std::string& message);
    
    // File output
    bool enableFileLogging(const std::string& filename);
    bool disableFileLogging();
    bool isFileLoggingEnabled() const;
    
    // Console output
    bool enableConsoleOutput();
    bool disableConsoleOutput();
    bool isConsoleOutputEnabled() const;
    
    // Formatting
    void setLogFormat(const std::string& format);
    void setTimestampFormat(const std::string& format);
    void setLogLevel(const std::string& level);
    
    // Log manipulation
    void clearLog();
    std::string getLog() const;
    size_t getLogSize() const;
    
    // Statistics
    size_t getInfoCount() const;
    size_t getWarningCount() const;
    size_t getErrorCount() const;
    size_t getSuccessCount() const;
    
    // Export
    bool exportLog(const std::string& filename);
    bool exportStatistics(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Build analyzer
class BuildAnalyzer {
public:
    BuildAnalyzer();
    ~BuildAnalyzer();
    
    // Build analysis
    bool analyzeBuild(const BuildOptions& options);
    bool analyzePerformance(const BuildOptions& options);
    bool analyzeDependencies(const BuildOptions& options);
    
    // Dependency analysis
    std::vector<std::string> findCircularDependencies() const;
    std::vector<std::string> findMissingDependencies() const;
    std::vector<std::string> findUnusedTargets() const;
    
    // Performance analysis
    double calculateBuildTimeEstimate(const BuildOptions& options) const;
    size_t calculateBuildSizeEstimate(const BuildOptions& options) const;
    std::string calculateBuildComplexity(const BuildOptions& options) const;
    
    // Quality analysis
    bool checkCodeQuality(BuildTarget target) const;
    bool checkCompilationErrors(BuildTarget target) const;
    bool checkLinkErrors(BuildTarget target) const;
    
    // Optimization analysis
    std::vector<std::string> suggestOptimizations() const;
    std::vector<std::string> suggestParallelizations() const;
    std::vector<std::string> suggestCaching() const;
    
    // Statistics
    std::unordered_map<std::string, double> getBuildStatistics() const;
    std::vector<std::string> getBuildIssues() const;
    std::vector<std::string> getBuildRecommendations() const;
    
    // Export analysis
    bool exportAnalysis(const std::string& filename);
    bool importAnalysis(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Build commander
class BuildCommander {
public:
    BuildCommander();
    ~BuildCommander();
    
    // Build orchestration
    bool orchestrateBuild(const std::vector<BuildTarget>& targets);
    bool orchestrateBuild(const std::string& targetList);
    
    // Component orchestration
    bool orchestrateComponentBuild(BuildTarget component);
    bool orchestratePluginBuild(const std::string& pluginName);
    bool orchestrateFactoryBuild(const std::string& factoryName);
    bool orchestrateTestBuild(const std::string& testName);
    
    // Cross-platform orchestration
    bool orchestrateCrossPlatformBuild(const std::string& platform);
    bool orchestrateMultiArchitectureBuild(const std::vector<std::string>& architectures);
    
    // Distributed build orchestration
    bool orchestrateDistributedBuild(const std::vector<std::string>& nodes);
    bool orchestrateCloudBuild(const std::string& provider);
    
    // Build monitoring
    bool monitorBuildProgress(const std::vector<BuildTarget>& targets);
    bool monitorBuildResources(const BuildOptions& options);
    bool monitorBuildDependencies(const BuildOptions& options);
    
    // Build control
    bool pauseBuild();
    bool resumeBuild();
    bool stopBuild();
    bool skipBuildTarget(BuildTarget target);
    
    // Build information
    std::vector<BuildTarget> getCurrentBuildTargets() const;
    BuildStats getCurrentBuildStats() const;
    std::chrono::milliseconds getEstimatedBuildTime() const;
    
    // Export/import
    bool exportBuildConfiguration(const std::string& filename);
    bool importBuildConfiguration(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Build system integration
namespace integration {
    // Build system registration
    bool registerBuildSystem(const std::string& name, std::unique_ptr<IBuildSystem> system);
    std::unique_ptr<IBuildSystem> getBuildSystem(const std::string& name);
    std::vector<std::string> getRegisteredBuildSystems();
    
    // Build system utilities
    bool initializeBuildSystem(const std::string& name, const BuildOptions& options);
    bool shutdownBuildSystem(const std::string& name);
    bool isBuildSystemAvailable(const std::string& name);
    
    // Cross-component build integration
    bool integratePluginBuilds(const std::vector<std::string>& pluginNames);
    bool integrateFactoryBuilds(const std::vector<std::string>& factoryNames);
    bool integrateTestBuilds(const std::vector<std::string>& testNames);
    
    // Performance integration
    bool optimizeBuildIntegration(const BuildOptions& options);
    bool balanceBuildResources(const BuildOptions& options);
    bool scheduleBuildTasks(const BuildOptions& options);
    
    // Logging and monitoring integration
    void integrateBuildLogging(std::shared_ptr<BuildLogger> logger);
    void integrateBuildAnalytics(std::shared_ptr<BuildAnalyzer> analyzer);
    void integrateBuildCommanding(std::shared_ptr<BuildCommander> commander);
}

// Build system command line interface
class BuildCLI {
public:
    BuildCLI();
    ~BuildCLI();
    
    // Command line interface
    bool parseCommandLine(int argc, char** argv);
    bool executeCommandLine(int argc, char** argv);
    
    // Command handlers
    bool handleBuildCommand(const std::vector<std::string>& args);
    bool handleCleanCommand(const std::vector<std::string>& args);
    bool handleReBuildCommand(const std::vector<std::string>& args);
    bool handleConfigureCommand(const std::vector<std::string>& args);
    bool handleInstallCommand(const std::vector<std::string>& args);
    bool handleTestCommand(const std::vector<std::string>& args);
    bool handleBenchmarkCommand(const std::vector<std::string>& args);
    
    // Command line options
    bool addBuildOption(const std::string& option);
    bool addBuildTarget(const std::string& target);
    bool setBuildConfiguration(const std::string& config);
    bool setBuildSystem(const std::string& system);
    
    // Command line queries
    std::vector<std::string> getAvailableCommands() const;
    std::vector<std::string> getCurrentBuildTargets() const;
    BuildOptions getCurrentBuildOptions() const;
    BuildStats getCurrentBuildStats() const;
    
    // Help and documentation
    bool showHelp(const std::string& command = "") const;
    bool showVersion() const;
    bool showConfiguration() const;
    bool showAvailableTargets() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Build system services
namespace services {
    // Build system service registration
    bool registerBuildService(const std::string& name, std::function<bool(const BuildOptions&)> service);
    bool unregisterBuildService(const std::string& name);
    std::vector<std::string> getRegisteredBuildServices();
    
    // Build service execution
    bool executeBuildService(const std::string& name, const BuildOptions& options);
    bool executeAllBuildServices(const BuildOptions& options);
    
    // Build service configuration
    bool configureBuildService(const std::string& name, const std::unordered_map<std::string, std::any>& config);
    std::unordered_map<std::string, std::any> getBuildServiceConfiguration(const std::string& name);
    
    // Build service monitoring
    bool monitorBuildService(const std::string& name);
    bool monitorAllBuildServices();
    std::unordered_map<std::string, std::any> getBuildServiceStatus(const std::string& name);
}

// Build system administration
namespace administration {
    // Build administration
    bool validateBuildConfiguration(const BuildOptions& options);
    bool optimizeBuildConfiguration(BuildOptions& options);
    bool generateBuildDocumentation(const BuildOptions& options);
    bool createBuildArchive(const BuildOptions& options, const std::string& archiveName);
    
    // Build maintenance
    bool cleanupBuildArtifacts(const BuildOptions& options);
    bool compressBuildArtifacts(const BuildOptions& options);
    bool verifyBuildIntegrity(const BuildOptions& options);
    
    // Build security
    bool secureBuildConfiguration(BuildOptions& options);
    bool validateBuildSecurity(const BuildOptions& options);
    bool auditBuildOperations(const BuildOptions& options);
    
    // Build compliance
    bool checkBuildCompliance(const BuildOptions& options);
    bool generateBuildReport(const BuildOptions& options, const std::string& reportName);
    bool exportBuildConfiguration(const BuildOptions& options, const std::string& exportFormat);
}

// Build system metrics
namespace metrics {
    // Build metrics
    struct BuildMetrics {
        double totalBuildTime;
        size_t stepsExecuted;
        size_t stepsSkipped;
        size_t stepsFailed;
        double cpuUtilization;
        double memoryUtilization;
        double diskIO;
        std::vector<std::string> warningMessages;
        std::vector<std::string> errorMessages;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
    };
    
    // Metrics collection
    bool collectBuildMetrics(BuildMetrics& metrics);
    bool collectPerformanceMetrics(BuildMetrics& metrics);
    bool collectDependencyMetrics(BuildMetrics& metrics);
    
    // Metrics analysis
    double calculateBuildEfficiency(const BuildMetrics& metrics);
    double calculateBuildQuality(const BuildMetrics& metrics);
    std::vector<std::string> getPerformanceRecommendations(const BuildMetrics& metrics);
    std::vector<std::string> getOptimizationSuggestions(const BuildMetrics& metrics);
    
    // Metrics export
    bool exportMetrics(const BuildMetrics& metrics, const std::string& filename);
    bool importMetrics(BuildMetrics& metrics, const std::string& filename);
    bool generateMetricsReport(const BuildMetrics& metrics, const std::string& reportName);
    
    // Real-time monitoring
    bool startMetricsMonitoring();
    bool stopMetricsMonitoring();
    BuildMetrics getCurrentMetrics();
    bool updateMetrics();
}

// Build system factory functions
namespace factory {
    // Build system creation
    std::unique_ptr<IBuildSystem> createCMakeBuildSystem();
    std::unique_ptr<IBuildSystem> createMakefileBuildSystem();
    std::unique_ptr<IBuildSystem> createNinjaBuildSystem();
    
    // Build system configuration
    BuildOptions createDebugBuildOptions();
    BuildOptions createReleaseBuildOptions();
    BuildOptions createMinSizeRelBuildOptions();
    BuildOptions createRelWithDebInfoBuildOptions();
    
    // Component build configurations
    BuildOptions createPluginBuildOptions(const std::vector<std::string>& plugins);
    BuildOptions createFactoryBuildOptions(const std::vector<std::string>& factories);
    BuildOptions createTestBuildOptions(const std::vector<std::string>& tests);
    
    // Platform-specific configurations
    BuildOptions createLinuxBuildOptions();
    BuildOptions createWindowsBuildOptions();
    BuildOptions createMacOSBuildOptions();
    BuildOptions createAndroidBuildOptions();
    
    // Performance configurations
    BuildOptions createPerformanceBuildOptions();
    BuildOptions createMemoryOptimizedBuildOptions();
    BuildOptions createSizeOptimizedBuildOptions();
}

// Build system version and information
namespace version {
    const std::string& getVersion();
    const std::string& getBuildSystemVersion();
    const std::string& getCMakeVersion();
    const std::string& getNinjaVersion();
    const std::string& getMakeVersion();
    
    bool isVersionCompatible(const std::string& version);
    std::string getSupportedPlatforms();
    std::string getSupportedArchitectures();
    std::string getSupportedCompilers();
    
    bool checkDependencies();
    bool checkPlatformSupport();
    bool checkToolchainSupport();
}

// Build system exceptions
namespace exceptions {
    class BuildException : public std::runtime_error {
    public:
        BuildException(const std::string& message) : std::runtime_error(message) {}
    };
    
    class ConfigurationException : public BuildException {
    public:
        ConfigurationException(const std::string& message) : BuildException(message) {}
    };
    
    class BuildStepException : public BuildException {
    public:
        BuildStepException(const std::string& message) : BuildException(message) {}
    };
    
    class DependencyException : public BuildException {
    public:
        DependencyException(const std::string& message) : BuildException(message) {}
    };
    
    class PlatformException : public BuildException {
    public:
        PlatformException(const std::string& message) : BuildException(message) {}
    };
}

// Build system namespace aliases
namespace bs = build;
namespace bsi = build::integration;
namespace bso = build::options;
namespace bst = build::target;

// Build system main functions
namespace main {
    bool runBuildSystem(int argc, char** argv);
    bool initializeBuildSystem(const BuildOptions& options);
    bool shutdownBuildSystem();
    bool getBuildSystemStatus();
    bool getBuildSystemConfiguration();
    bool getBuildSystemStatistics();
}

// Build system convenience functions
namespace convenience {
    // Quick build commands
    bool buildAll();
    bool buildPlugins();
    bool buildFactories();
    bool buildTests();
    bool buildExamples();
    
    // Clean commands
    bool cleanAll();
    bool cleanBuild();
    bool cleanArtifacts();
    
    // Re-build commands
    bool rebuildAll();
    bool rebuildChanged();
    
    // Configure commands
    bool configureDebug();
    bool configureRelease();
    bool configureCustom(const std::unordered_map<std::string, std::any>& options);
    
    // Install commands
    bool installAll();
    bool installPlugin(const std::string& pluginName);
    bool installFactory(const std::string& factoryName);
    
    // Test commands
    bool runTests();
    bool runBenchmarks();
    bool runPerformanceTests();
    
    // Information commands
    bool showBuildInfo();
    bool showAvailableTargets();
    bool showBuildStatistics();
    bool showSystemStatus();
}

} // namespace build
} // namespace nlm

// Build system command line interface entry point
extern "C" bool runBuildCLI(int argc, char** argv);

// Build system initialization and shutdown
namespace nlm {
    namespace build {
        bool initializeBuildSystem();
        bool shutdownBuildSystem();
        bool isBuildSystemInitialized();
        std::string getBuildSystemVersion();
    }
}

// Build system utility functions
namespace nlm {
    namespace build {
        bool executeBuildCommand(const std::string& command);
        bool executeBuildTargets(const std::vector<std::string>& targets);
        bool executeBuildConfiguration(const std::string& configName);
        bool executeBuildInstallation(const std::string& target);
        
        bool getBuildStatus(const std::string& target);
        std::vector<std::string> getBuildStatusAll();
        
        bool generateBuildDocumentation();
        bool generateBuildReport();
        bool generateBuildArchive();
        
        bool validateBuildEnvironment();
        bool setupBuildEnvironment();
        bool configureBuildEnvironment();
    }
}