#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include "TestFramework.hpp"
#include "TestReporting.hpp"
#include "IntegrationTestRunner.hpp"
#include "PerformanceTestSuite.hpp"
#include "PhaseIntegrationTestRunner.hpp"
#include "NLMComponentTestSuite.hpp"

namespace nlm {

class AutomatedTestRunner {
public:
    enum class TestRunMode {
        CONTINUOUS,
        SCHEDULED,
        TRIGGERED,
        MANUAL,
        ON_DEMAND
    };
    
    enum class CIEnvironment {
        JENKINS,
        GITHUB_ACTIONS,
        GITLAB_CI,
        AZURE_PIPELINES,
        BITBUCKET_PIPELINES,
        CUSTOM
    };
    
    enum class ReportFormat {
        CONSOLE,
        XML,
        JSON,
        CSV,
        HTML,
        JUNIT_XML,
        COBERTURA,
        CUCUMBERY
    };
    
    struct TestRunConfiguration {
        std::string name;
        TestRunMode mode;
        CIEnvironment ciEnvironment;
        ReportFormat reportFormat;
        std::string outputDirectory;
        std::string reportTitle;
        std::vector<std::string> testTags;
        std::vector<std::string> excludedTags;
        std::chrono::system_clock::time_point scheduledTime;
        std::chrono::duration<double> timeout;
        bool enablePerformanceTests;
        bool enableMemoryTests;
        bool enableIntegrationTests;
        bool enableRegressionTests;
        std::map<std::string, std::string> environmentVariables;
        std::string gitCommitHash;
        std::string gitBranch;
        std::string buildNumber;
        
        TestRunConfiguration(const std::string& n) : name(n), mode(TestRunMode::MANUAL),
                                                  ciEnvironment(CIEnvironment::CUSTOM),
                                                  reportFormat(ReportFormat::CONSOLE),
                                                  timeout(3600.0), // 1 hour default
                                                  enablePerformanceTests(true),
                                                  enableMemoryTests(true),
                                                  enableIntegrationTests(true),
                                                  enableRegressionTests(true) {}
    };
    
    struct TestRunResult {
        std::string runId;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::chrono::duration<double> duration;
        std::string status; // SUCCESS, FAILURE, ERROR, TIMEOUT, ABORTED
        std::string buildNumber;
        std::string gitCommit;
        std::string gitBranch;
        std::vector<TestFramework::TestResult> testResults;
        std::vector<PerformanceBenchmark::BenchmarkResult> benchmarkResults;
        std::vector<MemoryAllocationTracker::AllocationInfo> memoryAllocations;
        std::map<std::string, double> metrics;
        std::map<std::string, std::string> environmentInfo;
        std::string summaryReport;
        std::string detailedReport;
        
        TestRunResult(const std::string& id) : runId(id), startTime(std::chrono::system_clock::now()),
                                            endTime(), duration(0.0) {}
    };
    
    struct PipelineStage {
        std::string name;
        int order;
        bool required;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::string status;
        std::string output;
        std::string errorMessage;
        
        PipelineStage(const std::string& n, int o) : name(n), order(o), required(true),
                                                   startTime(), endTime(), status("PENDING") {}
    };
    
    AutomatedTestRunner();
    ~AutomatedTestRunner() = default;
    
    static AutomatedTestRunner& getInstance() {
        static AutomatedTestRunner instance;
        return instance;
    }
    
    // Configuration management
    void loadConfiguration(const std::string& filename);
    void saveConfiguration(const std::string& filename) const;
    void setConfiguration(const TestRunConfiguration& config);
    const TestRunConfiguration& getConfiguration() const { return config; }
    
    // CI/CD integration
    void initializeCIEnvironment();
    void setupBuildArtifacts();
    void cleanupBuildArtifacts();
    bool isCIEnvironmentActive() const;
    
    // Test execution
    TestRunResult runTests(const TestRunConfiguration& config);
    TestRunResult runTests(const std::string& configName);
    TestRunResult runUnitTests();
    TestRunResult runIntegrationTests();
    TestRunResult runPerformanceTests();
    TestRunResult runMemoryTests();
    TestRunResult runRegressionTests();
    TestRunResult runAllTests();
    
    // Pipeline execution
    void startPipeline(const std::string& pipelineName);
    void stopPipeline(const std::string& pipelineName);
    void addPipelineStage(const PipelineStage& stage);
    void executePipelineStage(const std::string& stageName);
    const PipelineStage& getPipelineStage(const std::string& stageName) const;
    std::vector<PipelineStage> getPipelineStages() const;
    
    // Continuous integration
    void enableContinuousIntegration();
    void disableContinuousIntegration();
    bool isContinuousIntegrationEnabled() const;
    void setContinuousIntegrationSchedule(const std::string& cronExpression);
    
    // Reporting
    void generateReports(const TestRunResult& result);
    void generateHTMLReport(const TestRunResult& result);
    void generateJSONReport(const TestRunResult& result);
    void generateXMLReport(const TestRunResult& result);
    void generateCSVReport(const TestRunResult& result);
    void generateJUnitXMLReport(const TestRunResult& result);
    void generateCoBERTuraReport(const TestRunResult& result);
    void generateCucumberReport(const TestRunResult& result);
    
    // Results management
    void storeTestRunResult(const TestRunResult& result);
    TestRunResult getTestRunResult(const std::string& runId) const;
    std::vector<TestRunResult> getTestRunHistory() const;
    void clearTestRunHistory();
    
    // Analysis and trending
    std::string analyzeTestTrends() const;
    std::string analyzePerformanceTrends() const;
    std::string analyzeRegressionTrends() const;
    std::map<std::string, double> calculateTestMetrics(const TestRunResult& result) const;
    
    // Notification and integration
    void sendNotification(const std::string& message, const std::string& channel = "default");
    void integrateWithIssueTracker(const TestRunResult& result);
    void integrateWithVersionControl(const TestRunResult& result);
    void integrateWithMonitoringSystem(const TestRunResult& result);
    
    // Automation utilities
    std::string generateTestRunId() const;
    std::string getBuildIdentifier() const;
    std::string getEnvironmentIdentifier() const;
    std::chrono::system_clock::time_point getCurrentTime() const;
    
    // Performance optimization
    void optimizeTestExecution() const;
    void enableTestCaching() const;
    void disableTestCaching() const;
    bool isTestCachingEnabled() const;
    
    // Error handling and recovery
    void handleTestFailure(const std::string& testName, const std::exception& error);
    void handleTestTimeout(const std::string& testName);
    void handleSystemFailure(const std::string& failureType, const std::string& errorMessage);
    void recoverFromFailure();
    
    // Monitoring
    void enableMonitoring() const;
    void disableMonitoring() const;
    bool isMonitoringEnabled() const;
    void recordMetric(const std::string& name, double value);
    std::map<std::string, double> getMetrics() const;
    
protected:
    TestRunConfiguration config;
    std::vector<TestRunResult> testRunHistory;
    std::vector<PipelineStage> pipelineStages;
    std::map<std::string, PipelineStage> stageMap;
    std::mutex historyMutex;
    
    bool ciActive;
    bool ciEnabled;
    bool monitoringEnabled;
    bool testCachingEnabled;
    std::string currentBuildId;
    std::map<std::string, double> metrics;
    
    virtual void setupTestEnvironment();
    virtual void teardownTestEnvironment();
    virtual TestRunResult executeTestPhase(const std::string& phaseName);
    virtual void validateTestResults(const TestRunResult& result);
    virtual void generateBuildArtifacts(const TestRunResult& result);
    virtual void publishResults(const TestRunResult& result);
    
    virtual std::string getEnvironmentInfo(const std::string& key) const;
    virtual bool validateEnvironment() const;
    virtual void cleanup() const;
};

class JenkinsTestRunner : public AutomatedTestRunner {
public:
    JenkinsTestRunner();
    ~JenkinsTestRunner() override;
    
    void setupJenkinsBuild() override;
    void publishJUnitResults() const override;
    void publishTestReports() const override;
    void setBuildStatus(const std::string& status) const override;
    void setBuildDescription(const std::string& description) const override;
    
protected:
    std::string jenkinsUrl;
    std::string jenkinsJobName;
    std::string jenkinsToken;
};

class GitHubActionsTestRunner : public AutomatedTestRunner {
public:
    GitHubActionsTestRunner();
    ~GitHubActionsTestRunner() override;
    
    void setupGitHubActions() override;
    void publishWorkflowResults() const override;
    void setWorkflowStatus(const std::string& status) const override;
    
protected:
    std::string githubToken;
    std::string workflowFile;
};

class GitLabCITestRunner : public AutomatedTestRunner {
public:
    GitLabCITestRunner();
    ~GitLabCITestRunner() override;
    
    void setupGitLabCI() override;
    void publishGitLabResults() const override;
    void setGitLabStatus(const std::string& status) const override;
    
protected:
    std::string gitlabToken;
    std::string gitlabProject;
};

class AzurePipelinesTestRunner : public AutomatedTestRunner {
public:
    AzurePipelinesTestRunner();
    ~AzurePipelinesTestRunner() override;
    
    void setupAzurePipelines() override;
    void publishAzureResults() const override;
    void setAzureStatus(const std::string& status) const override;
    
protected:
    std::string azureToken;
    std::string azureSubscription;
};

class CustomCIConfigurationTestRunner : public AutomatedTestRunner {
public:
    CustomCIConfigurationTestRunner();
    ~CustomCIConfigurationTestRunner() override;
    
    void setupCustomCI() override;
    void publishCustomResults() const override;
    void setCustomStatus(const std::string& status) const override;
    
protected:
    std::string customEndpoint;
    std::string customToken;
    std::string customConfiguration;
};

// Test automation utilities
namespace automation {
    
    // Configuration management
    class ConfigurationManager {
    public:
        static ConfigurationManager& getInstance() {
            static ConfigurationManager instance;
            return instance;
        }
        
        void loadConfiguration(const std::string& filename);
        void saveConfiguration(const std::string& filename) const;
        const TestRunConfiguration& getTestConfiguration() const { return config; }
        void setTestConfiguration(const TestRunConfiguration& config) { this->config = config; }
        
        // CI/CD configuration
        void loadCIDefinition(const std::string& filename);
        const std::map<std::string, PipelineStage>& getPipelineStages() const { return pipelineStages; }
        void setPipelineStages(const std::map<std::string, PipelineStage>& stages) { pipelineStages = stages; }
        
        // Environment configuration
        void loadEnvironmentConfig(const std::string& filename);
        const std::map<std::string, std::string>& getEnvironmentVariables() const { return environmentVariables; }
        void setEnvironmentVariable(const std::string& key, const std::string& value) { environmentVariables[key] = value; }
        
    private:
        TestRunConfiguration config;
        std::map<std::string, PipelineStage> pipelineStages;
        std::map<std::string, std::string> environmentVariables;
        std::mutex configMutex;
    };
    
    // Test result analysis
    class ResultAnalyzer {
    public:
        static ResultAnalyzer& getInstance() {
            static ResultAnalyzer instance;
            return instance;
        }
        
        double calculateSuccessRate(const std::vector<TestFramework::TestResult>& results) const;
        double calculateAverageExecutionTime(const std::vector<TestFramework::TestResult>& results) const;
        size_t countFailedTests(const std::vector<TestFramework::TestResult>& results) const;
        size_t countSkippedTests(const std::vector<TestFramework::TestResult>& results) const;
        double calculateTestCoverage(const std::vector<TestFramework::TestResult>& results) const;
        std::string generateSummaryReport(const std::vector<TestFramework::TestResult>& results) const;
        std::string generateDetailedReport(const std::vector<TestFramework::TestResult>& results) const;
        
        // Performance analysis
        double calculatePerformanceRegression(const std::vector<PerformanceBenchmark::BenchmarkResult>& baseline,
                                             const std::vector<PerformanceBenchmark::BenchmarkResult>& current) const;
        std::string analyzePerformanceTrends(const std::vector<PerformanceBenchmark::BenchmarkResult>& results) const;
        
        // Memory analysis
        bool detectMemoryLeaks(const std::vector<MemoryAllocationTracker::AllocationInfo>& allocations) const;
        size_t calculateTotalMemoryUsage(const std::vector<MemoryAllocationTracker::AllocationInfo>& allocations) const;
        std::string generateMemoryReport(const std::vector<MemoryAllocationTracker::AllocationInfo>& allocations) const;
        
    private:
        std::mutex analyzerMutex;
    };
    
    // Report generation
    class ReportGenerator {
    public:
        static ReportGenerator& getInstance() {
            static ReportGenerator instance;
            return instance;
        }
        
        std::string generateHTMLReport(const TestRunResult& result) const;
        std::string generateJSONReport(const TestRunResult& result) const;
        std::string generateXMLReport(const TestRunResult& result) const;
        std::string generateCSVReport(const TestRunResult& result) const;
        std::string generateJUnitXMLReport(const TestRunResult& result) const;
        std::string generateCoBERTuraReport(const TestRunResult& result) const;
        std::string generateCucumberReport(const TestRunResult& result) const;
        
        // Specialized reports
        std::string generatePerformanceReport(const TestRunResult& result) const;
        std::string generateMemoryReport(const TestRunResult& result) const;
        std::string generateIntegrationReport(const TestRunResult& result) const;
        std::string generateRegressionReport(const TestRunResult& result) const;
        
    private:
        std::mutex generatorMutex;
    };
    
    // Notification system
    class NotificationSystem {
    public:
        static NotificationSystem& getInstance() {
            static NotificationSystem instance;
            return instance;
        }
        
        void sendEmailNotification(const std::string& subject, const std::string& body,
                                 const std::vector<std::string>& recipients = {}) const;
        void sendSlackNotification(const std::string& message, const std::string& channel = "#test-reports") const;
        void sendMicrosoftTeamsNotification(const std::string& message) const;
        void sendDiscordNotification(const std::string& message) const;
        void sendWebhookNotification(const std::string& webhookUrl, const std::string& payload) const;
        
        void sendTestRunNotification(const TestRunResult& result) const;
        void sendFailureNotification(const TestRunResult& result) const;
        void sendSuccessNotification(const TestRunResult& result) const;
        
    private:
        std::mutex notificationMutex;
    };
    
    // Build artifact management
    class BuildArtifactManager {
    public:
        static BuildArtifactManager& getInstance() {
            static BuildArtifactManager instance;
            return instance;
        }
        
        void storeTestResults(const TestRunResult& result, const std::string& directory);
        void storeTestReports(const TestRunResult& result, const std::string& directory);
        void storePerformanceReports(const TestRunResult& result, const std::string& directory);
        void storeMemoryReports(const TestRunResult& result, const std::string& directory);
        
        TestRunResult loadTestResults(const std::string& runId, const std::string& directory);
        std::vector<TestRunResult> loadAllTestResults(const std::string& directory);
        
        void cleanupOldArtifacts(const std::string& directory, int keepDays = 30);
        std::vector<std::string> getAvailableArtifacts(const std::string& directory) const;
        
        // Artifact compression and archival
        void compressArtifacts(const std::string& sourceDir, const std::string& archiveFile);
        void extractArtifacts(const std::string& archiveFile, const std::string& extractDir);
        
    private:
        std::mutex artifactMutex;
    };
    
} // namespace automation

} // namespace nlm
