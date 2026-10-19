// User Experience Enhancement - Test Utilities and Support Tools

#ifndef NLM_USER_EXPERIENCE_H
#define NLM_USER_EXPERIENCE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <map>
#include <algorithm>
#include <cmath>

namespace nlm {

// User Experience Utilities
class UXUtilities {
public:
    // Progress reporting and status monitoring
    static void showProgress(const std::string& task, float progress, int width = 50);
    static void showSpinner(const std::string& message, bool& stopSpinner);
    static void showStatus(const std::string& component, const std::string& status, 
                          const std::string& color = "");
    
    // User interaction and input handling
    static bool confirm(const std::string& question, bool defaultAnswer = true);
    static std::string prompt(const std::string& question, const std::string& defaultValue = "");
    static int promptInt(const std::string& question, int min = 0, int max = 100);
    static float promptFloat(const std::string& question, float min = 0.0f, float max = 1.0f);
    
    // Report generation and formatting
    static void generateDetailedReport(const std::string& filename, 
                                      const std::string& sessionData);
    static void printFormattedSection(const std::string& title, const std::string& content);
    static void createMarkdownReport(const std::string& filename, 
                                   const std::map<std::string, std::string>& sections);
    
    // Configuration and preferences
    static class UserPreferences {
    public:
        bool enableVerboseOutput = false;
        bool enableProgressIndicators = true;
        bool enableConfirmationPrompts = true;
        bool enableDetailedReports = true;
        std::string outputFormat = "text"; // "text", "json", "markdown"
        std::string logLevel = "info"; // "debug", "info", "warning", "error"
        
        UserPreferences() {}
        
        void loadFromFile(const std::string& filename);
        void saveToFile(const std::string& filename) const;
        void printSettings() const;
    } preferences;
    
    // Session management
    class SessionManager {
    public:
        struct SessionInfo {
            std::string sessionId;
            std::chrono::system_clock::time_point startTime;
            std::chrono::system_clock::time_point endTime;
            std::string configFile;
            uint64_t totalSteps;
            float totalReward;
            size_t neuronsUsed;
            bool successful;
        };
        
        static std::string createSessionId();
        static SessionInfo startSession(const std::string& configFile);
        static void endSession(const SessionInfo& sessionInfo, bool success = true);
        static void saveSessionHistory(const std::string& filename);
        static std::vector<SessionInfo> loadSessionHistory(const std::string& filename);
        
        static std::vector<SessionInfo> sessionHistory;
    };
};

// Enhanced Test Framework
class EnhancedTestFramework {
public:
    struct TestResult {
        std::string testName;
        bool passed;
        std::string message;
        std::chrono::duration<double> executionTime;
        std::map<std::string, std::string> metadata;
        
        TestResult(const std::string& name) : testName(name), passed(false) {}
    };
    
    // Test organization and reporting
    static void addTest(const std::string& testName, std::function<bool()> testFunc);
    static void runAllTests();
    static void runPerformanceTests();
    static void runIntegrationTests();
    static void runRegressionTests();
    
    // Test utilities
    static bool testFileExists(const std::string& path);
    static bool testDirectoryExists(const std::string& path);
    static bool compareFiles(const std::string& file1, const std::string& file2);
    static std::string generateTestData(size_t size);
    static bool validateTestData(const std::string& data);
    
    // Performance testing utilities
    static class PerformanceTester {
    public:
        static void benchmarkOperation(const std::string& name, std::function<void()> operation, 
                                      size_t iterations = 100);
        static void profileMemoryUsage(const std::string& name, std::function<void()> operation);
        static void profileCpuUsage(const std::string& name, std::function<void()> operation);
        static void compareImplementations(const std::string& name, 
                                          std::function<void()> impl1,
                                          std::function<void()> impl2);
    };
    
    // Test reporting
    static void generateTestReport(const std::string& filename, const std::vector<TestResult>& results);
    static void generatePerformanceReport(const std::string& filename);
    static void generateComparisonReport(const std::string& filename);
    
    static std::vector<TestResult> testResults;
    static bool testsRunning;
    static bool reportsEnabled;
};

// Interactive User Interface
class InteractiveUI {
public:
    // Brain exploration interface
    static void displayBrainExplorer(std::shared_ptr<Brain> brain);
    static void displayMemoryVisualizer(std::shared_ptr<Brain> brain);
    static void displayNeuromodulationDashboard(std::shared_ptr<Brain> brain);
    static void displayDevelopmentTracker(std::shared_ptr<Brain> brain);
    
    // Command interface
    class CommandProcessor {
    public:
        struct Command {
            std::string name;
            std::string description;
            std::function<bool(const std::vector<std::string>&)> execute;
            std::vector<std::string> parameters;
            bool requiresBrain = false;
        };
        
        static void registerCommand(const Command& cmd);
        static bool processCommand(const std::string& input, std::shared_ptr<Brain> brain = nullptr);
        static void printHelp();
        static void printBrainCommands(std::shared_ptr<Brain> brain);
        static void printAnalysisCommands(std::shared_ptr<Brain> brain);
        
        static std::map<std::string, Command> commands;
    };
    
    // Visualization helpers
    static void createAsciiVisualization(const std::string& title, 
                                         const std::vector<std::vector<std::string>>& grid);
    static void createHeatMapVisualization(const std::string& title,
                                          const std::vector<std::vector<float>>& data);
    static void createTimelineVisualization(const std::string& title,
                                           const std::vector<std::pair<double, std::string>>& events);
    
    // Input validation and help
    static bool validateInput(const std::string& input, const std::string& pattern);
    static std::string getAutocompletion(const std::string& partialInput);
    static void showContextHelp(const std::string& context);
};

// Utility Classes
class StringUtils {
public:
    static std::string toLowerCase(const std::string& str);
    static std::string toUpperCase(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::string formatNumber(size_t number, char thousandsSeparator = ',');
    static std::string formatFloat(float value, int precision = 2);
    static std::string colorize(const std::string& text, const std::string& color);
};

class FileUtils {
public:
    static bool ensureDirectoryExists(const std::string& path);
    static std::string getFileExtension(const std::string& filename);
    static std::string getBaseFilename(const std::string& filename);
    static std::string getTimestampString();
    static std::string createUniqueFilename(const std::string& base, const std::string& extension = "");
    static bool safeCopy(const std::string& source, const std::string& destination);
    static bool safeMove(const std::string& source, const std::string& destination);
};

class TimeUtils {
public:
    static std::string formatDuration(std::chrono::duration<double> duration);
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& timePoint);
    static std::chrono::system_clock::time_point parseTimestamp(const std::string& timestamp);
    static double getCurrentTimestamp();
};

// Main user experience setup
void initializeUserExperience();
void setupDefaultPreferences();
void setupDefaultCommands();
void showWelcomeMessage();
void showHelpForNewUsers();

} // namespace nlm

#endif // NLM_USER_EXPERIENCE_H
