#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <cmath>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>

namespace nlm {

// Integration types
enum class ExternalTool {
    MATLAB,
    Python,
    R,
    Julia,
    CSharp,
    Java,
    NodeJS,
    Go,
    Rust,
    Custom
};

enum class IntegrationType {
    Native,
    Foreign,
    WebBased,
    Cloud,
    Distributed,
    Plugin,
    Extension
};

// Plugin information
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    IntegrationType type;
    std::vector<std::string> dependencies;
    bool enabled;
    std::map<std::string, std::string> metadata;
    
    PluginInfo(const std::string& n = "Plugin", const std::string& v = "1.0")
        : name(n), version(v), description(""), author(""), type(IntegrationType::Plugin), enabled(true) {}
};

// External tool interface
class ExternalToolInterface {
public:
    virtual ~ExternalToolInterface() = default;
    
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isReady() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual ExternalTool getToolType() const = 0;
    
    virtual std::vector<ConfigValue> execute(const std::string& command, const std::vector<ConfigValue>& args) = 0;
    virtual bool loadScript(const std::string& scriptPath) = 0;
    virtual bool runScript(const std::string& scriptName, const std::vector<ConfigValue>& arguments) = 0;
    virtual std::map<std::string, ConfigValue> getSystemInfo() = 0;
};

// Plugin manager interface
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool isEnabled() const = 0;
    virtual bool enable() = 0;
    virtual bool disable() = 0;
    
    virtual std::vector<ConfigValue> execute(const std::string& function, const std::vector<ConfigValue>& args) = 0;
    virtual bool registerFunction(const std::string& name, std::function<ConfigValue(const std::vector<ConfigValue>&)> func) = 0;
    virtual bool unregisterFunction(const std::string& name) = 0;
    virtual std::map<std::string, std::function<ConfigValue(const std::vector<ConfigValue>&)>> getAvailableFunctions() const = 0;
};

// Cloud computing interface
class CloudInterface {
public:
    virtual ~CloudInterface() = default;
    
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isAvailable() const = 0;
    virtual std::string getProviderName() const = 0;
    virtual std::string getRegion() const = 0;
    
    virtual std::string submitJob(const std::string& jobName, const std::vector<ConfigValue>& jobData) = 0;
    virtual bool waitForJob(const std::string& jobId, float timeout = 300.0f) = 0;
    virtual std::map<std::string, ConfigValue> getJobStatus(const std::string& jobId) = 0;
    virtual bool cancelJob(const std::string& jobId) = 0;
    virtual std::vector<std::string> listJobs() = 0;
    
    virtual std::string createStorageBucket(const std::string& bucketName) = 0;
    virtual bool uploadToStorage(const std::string& bucketName, const std::string& key, const ConfigValue& data) = 0;
    virtual std::map<std::string, ConfigValue> downloadFromStorage(const std::string& bucketName, const std::string& key) = 0;
    virtual bool deleteFromStorage(const std::string& bucketName, const std::string& key) = 0;
    virtual std::vector<std::string> listStorageContents(const std::string& bucketName) = 0;
    
    virtual std::string createDatabase(const std::string& dbName) = 0;
    virtual bool executeQuery(const std::string& dbName, const std::string& query, std::vector<std::map<std::string, ConfigValue>>& results) = 0;
    virtual bool backupDatabase(const std::string& dbName, const std::string& backupPath) = 0;
    virtual bool restoreDatabase(const std::string& dbName, const std::string& backupPath) = 0;
};

// Integration manager
class IntegrationManager {
public:
    IntegrationManager();
    ~IntegrationManager();
    
    // External tool management
    bool registerExternalTool(const std::string& name, ExternalTool toolType, const std::map<std::string, std::string>& config);
    bool unregisterExternalTool(const std::string& name);
    ExternalToolInterface* getExternalTool(const std::string& name);
    std::vector<std::string> getRegisteredTools() const;
    std::vector<ExternalToolInterface*> getToolsByType(ExternalTool toolType);
    
    // Plugin management
    bool registerPlugin(const std::string& name, const PluginInfo& pluginInfo);
    bool unregisterPlugin(const std::string& name);
    PluginInterface* getPlugin(const std::string& name);
    std::vector<std::string> getRegisteredPlugins() const;
    bool loadPlugin(const std::string& name);
    bool unloadPlugin(const std::string& name);
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    
    // Cloud integration
    bool connectToCloud(const std::string& provider, const std::map<std::string, std::string>& config);
    bool disconnectFromCloud();
    CloudInterface* getCloudInterface();
    bool isConnectedToCloud() const { return cloudInterface != nullptr; }
    
    // Integration execution
    std::vector<ConfigValue> executeWithExternalTool(const std::string& toolName, const std::string& command, const std::vector<ConfigValue>& args);
    std::vector<ConfigValue> executeWithPlugin(const std::string& pluginName, const std::string& function, const std::vector<ConfigValue>& args);
    std::string executeWithCloud(const std::string& jobName, const std::vector<ConfigValue>& jobData);
    
    // Configuration integration
    bool integrateWithConfig(const Config& config);
    bool extractConfiguration(const std::string& configSource, Config& config);
    bool validateConfigurationCompatibility(const Config& config) const;
    
    // Monitoring and management
    std::map<std::string, bool> getIntegrationStatus() const;
    std::map<std::string, std::string> getIntegrationDiagnostics() const;
    bool testIntegrationConnections();
    void setIntegrationCallback(std::function<void(const std::string&, const std::string&, bool)> callback);
    
    // Export/import
    bool exportIntegrationConfig(const std::string& filepath);
    bool importIntegrationConfig(const std::string& filepath);
    std::string exportIntegrationState(const std::string& format = "json") const;
    
    // Advanced features
    bool enableDistributedComputing(const std::vector<std::string>& workerNodes);
    bool createLoadBalancedExecution(const std::vector<std::string>& workerNodes);
    bool enableAutoScaling(const std::string& policyName);
    bool configureFaultTolerance(const std::string& strategy);
    
    // Plugin system features
    bool createPluginFramework(const std::string& frameworkName, const std::string& language);
    bool registerPluginWithFramework(const std::string& frameworkName, const PluginInfo& pluginInfo);
    bool loadPluginFromFramework(const std::string& frameworkName, const std::string& pluginName);
    bool enablePluginFramework(const std::string& frameworkName);
    bool disablePluginFramework(const std::string& frameworkName);
    
    // Tool compatibility
    bool checkToolCompatibility(const std::string& toolName, const std::vector<std::string>& requiredFeatures);
    bool resolveToolDependencies(const std::string& toolName);
    bool updateToolVersion(const std::string& toolName, const std::string& version);
    bool installToolDependencies(const std::string& toolName);
    
    // Advanced integration
    bool createIntegrationPipeline(const std::vector<std::string>& integrationSteps);
    bool executeIntegrationPipeline(const std::string& pipelineName, const std::map<std::string, ConfigValue>& config);
    bool configureIntegrationMonitoring(const std::string& monitoringType);
    bool generateIntegrationReport(const std::string& reportType);
    
protected:
    void updateIntegrationStatus();
    bool validateIntegrationConfiguration() const;
    void cleanupUnusedResources();
    
    std::map<std::string, std::unique_ptr<ExternalToolInterface>> externalTools;
    std::map<std::string, std::unique_ptr<PluginInterface>> plugins;
    std::unique_ptr<CloudInterface> cloudInterface;
    
    std::function<void(const std::string&, const std::string&, bool)> integrationCallback;
    
    bool monitoringEnabled;
    float monitoringInterval;
    std::chrono::system_clock::time_point lastUpdate;
};

// Plugin manager
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin lifecycle
    bool loadPlugin(const std::string& pluginName, const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    bool reloadPlugin(const std::string& pluginName);
    
    // Plugin information
    PluginInfo getPluginInfo(const std::string& pluginName) const;
    std::vector<PluginInfo> getAllPlugins() const;
    std::vector<PluginInfo> getEnabledPlugins() const;
    std::vector<PluginInfo> getDisabledPlugins() const;
    std::map<std::string, PluginInfo> getPluginsByType(IntegrationType type);
    
    // Function management
    bool registerFunction(const std::string& pluginName, const std::string& functionName,
                         std::function<ConfigValue(const std::vector<ConfigValue>&)> func);
    bool unregisterFunction(const std::string& pluginName, const std::string& functionName);
    ConfigValue callFunction(const std::string& pluginName, const std::string& functionName,
                            const std::vector<ConfigValue>& args);
    bool broadcastFunction(const std::string& functionName, const std::vector<ConfigValue>& args,
                          std::vector<ConfigValue>& results);
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& pluginPath);
    bool detectPluginDependencies(const std::string& pluginName);
    bool resolvePluginDependencies(const std::string& pluginName);
    bool checkPluginCompatibility(const PluginInfo& pluginInfo);
    
    // Plugin configuration
    bool configurePlugin(const std::string& pluginName, const std::map<std::string, ConfigValue>& config);
    std::map<std::string, ConfigValue> getPluginConfig(const std::string& pluginName) const;
    bool updatePluginConfig(const std::string& pluginName, const std::map<std::string, ConfigValue>& config);
    bool resetPluginConfig(const std::string& pluginName);
    
    // Plugin security
    bool validatePluginSignature(const std::string& pluginPath);
    bool checkPluginPermissions(const std::string& pluginName);
    bool sandboxPlugin(const std::string& pluginName);
    bool verifyPluginIntegrity(const std::string& pluginName);
    
    // Plugin development
    bool createPluginTemplate(const std::string& templateName, const std::string& language);
    bool generatePluginCode(const std::string& templateName, const std::map<std::string, std::string>& params);
    bool compilePlugin(const std::string& pluginPath);
    bool testPlugin(const std::string& pluginName, const std::map<std::string, ConfigValue>& testData);
    
    // Plugin deployment
    bool deployPlugin(const std::string& pluginName, const std::string& targetEnvironment);
    bool undeployPlugin(const std::string& pluginName);
    bool rollbackPlugin(const std::string& pluginName);
    bool updatePlugin(const std::string& pluginName, const std::string& newVersion);
    
    // Plugin analysis
    std::map<std::string, float> analyzePluginPerformance(const std::string& pluginName);
    std::map<std::string, float> analyzePluginResourceUsage(const std::string& pluginName);
    bool profilePlugin(const std::string& pluginName, const std::string& profilingType);
    std::string getPluginDiagnostics(const std::string& pluginName) const;
    bool checkPluginHealth(const std::string& pluginName);
    
    // Plugin export/import
    bool exportPlugin(const std::string& pluginName, const std::string& exportPath);
    bool importPlugin(const std::string& importPath);
    std::string exportPluginMetadata(const std::string& pluginName) const;
    bool importPluginMetadata(const std::string& metadata, std::string& pluginName);
    
    // Advanced plugin features
    bool createPluginNetwork(const std::vector<std::string>& pluginNames);
    bool enablePluginCommunication(const std::string& pluginName, const std::string& communicationProtocol);
    bool configurePluginLoadBalancing(const std::string& pluginName, const std::string& loadBalancingStrategy);
    bool enablePluginCaching(const std::string& pluginName, const std::string& cachingStrategy);
    bool configurePluginMonitoring(const std::string& pluginName, const std::string& monitoringConfig);
    
protected:
    void initializePluginSystem();
    bool validatePluginName(const std::string& pluginName) const;
    bool checkPluginAvailability(const std::string& pluginName) const;
    void cleanupPluginResources(const std::string& pluginName);
    
    std::map<std::string, std::unique_ptr<PluginInterface>> plugins;
    std::map<std::string, PluginInfo> pluginInfos;
    std::map<std::string, std::string> pluginPaths;
    std::map<std::string, std::vector<std::string>> pluginDependencies;
    std::map<std::string, std::map<std::string, ConfigValue>> pluginConfigs;
    
    bool systemInitialized;
    float lastScanTime;
    std::string pluginDirectory;
};

// Cloud manager
class CloudManager {
public:
    CloudManager();
    ~CloudManager();
    
    // Cloud provider management
    bool registerCloudProvider(const std::string& name, const std::string& providerType);
    bool unregisterCloudProvider(const std::string& name);
    CloudInterface* getCloudProvider(const std::string& name);
    std::vector<std::string> getRegisteredProviders() const;
    bool switchCloudProvider(const std::string& name);
    bool isCurrentProviderAvailable() const;
    
    // Cloud resource management
    std::string createComputeResource(const std::string& resourceType, const std::map<std::string, ConfigValue>& config);
    bool deleteComputeResource(const std::string& resourceId);
    std::map<std::string, ConfigValue> getComputeResourceInfo(const std::string& resourceId);
    std::vector<std::map<std::string, ConfigValue>> listComputeResources(const std::string& resourceType);
    
    // Cloud storage management
    std::string createStorageBucket(const std::string& bucketName, const std::map<std::string, ConfigValue>& config);
    bool deleteStorageBucket(const std::string& bucketName);
    std::map<std::string, ConfigValue> getStorageBucketInfo(const std::string& bucketName);
    std::vector<std::string> listStorageBuckets();
    
    // Cloud database management
    std::string createDatabase(const std::string& dbName, const std::map<std::string, ConfigValue>& config);
    bool deleteDatabase(const std::string& dbName);
    std::map<std::string, ConfigValue> getDatabaseInfo(const std::string& dbName);
    std::vector<std::string> listDatabases();
    
    // Cloud job management
    std::string submitJob(const std::string& jobName, const std::map<std::string, ConfigValue>& jobConfig);
    bool waitForJobCompletion(const std::string& jobId, float timeout = 300.0f);
    std::map<std::string, ConfigValue> getJobStatus(const std::string& jobId);
    bool cancelJob(const std::string& jobId);
    bool killJob(const std::string& jobId);
    std::vector<std::map<std::string, ConfigValue>> listJobs();
    
    // Cloud monitoring
    std::map<std::string, float> getCloudResourceUsage(const std::string& resourceId);
    std::map<std::string, float> getCloudPerformanceMetrics();
    std::vector<std::map<std::string, ConfigValue>> getCloudLogs(const std::string& resourceId, const std::string& logType);
    bool monitorCloudResource(const std::string& resourceId, const std::string& monitoringConfig);
    
    // Cloud backup and recovery
    bool createBackup(const std::string& resourceId, const std::string& backupPath);
    bool restoreBackup(const std::string& resourceId, const std::string& backupPath);
    std::vector<std::string> listBackups(const std::string& resourceId);
    bool deleteBackup(const std::string& backupId);
    
    // Cloud integration
    bool integrateWithLocalSystem(const std::map<std::string, ConfigValue>& localConfig);
    bool synchronizeCloudData(const std::string& syncType);
    bool configureCloudSync(const std::string& syncConfig);
    bool validateCloudConnectivity();
    
    // Cloud analytics
    std::map<std::string, float> analyzeCloudUsage(const std::string& resourceId);
    std::vector<std::map<std::string, ConfigValue>> queryCloudLogs(const std::string& query);
    bool generateCloudReport(const std::string& reportType, const std::string& outputPath);
    bool exportCloudMetrics(const std::string& metricType, const std::string& outputFormat);
    
    // Advanced cloud features
    bool enableAutoScaling(const std::string& resourceId, const std::string& scalingPolicy);
    bool configureLoadBalancing(const std::string& loadBalancerId, const std::string& algorithm);
    bool setupHighAvailability(const std::string& resourceId, const std::string& haConfig);
    bool configureSecurityGroups(const std::string& resourceId, const std::vector<std::string>& securityGroupIds);
    bool configureNetworkSettings(const std::string& resourceId, const std::map<std::string, ConfigValue>& networkConfig);
    
    // Cloud security
    bool configureSecurity(const std::string& securityConfig);
    bool configureEncryption(const std::string& encryptionAlgorithm, const std::string& keyId);
    bool configureAccessControl(const std::string& accessControlPolicy);
    bool configureAuditLogging(const std::string& auditConfig);
    bool configureCompliance(const std::string& complianceStandard);
    
    // Cloud optimization
    bool optimizeCloudResources(const std::string& optimizationType);
    bool enableCostOptimization(const std::string& optimizationStrategy);
    bool configureResourceAllocation(const std::map<std::string, float>& allocationRatios);
    bool setupResourceReservations(const std::vector<std::string>& reservationIds);
    bool configureWorkloadBalancing(const std::string& balancingAlgorithm);
    
protected:
    void initializeCloudSystem();
    bool validateCloudConfiguration(const std::map<std::string, std::string>& config) const;
    bool checkCloudProviderAvailability(const std::string& providerName) const;
    void cleanupCloudResources(const std::string& providerName);
    
    std::map<std::string, std::unique_ptr<CloudInterface>> cloudProviders;
    std::string currentProvider;
    
    bool cloudInitialized;
    float lastHealthCheck;
    std::map<std::string, std::chrono::system_clock::time_point> resourceLastUsed;
};

} // namespace nlm
