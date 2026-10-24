// Test runner class for executing NLM Phase 6 tests
class TestRunner {
public:
    TestRunner();
    ~TestRunner() = default;
    
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    
    void addTest(std::unique_ptr<TestBase> test);
    
    std::vector<TestResult> runAllTests(const TestConfig& baseConfig);
    
    void printSummary(const std::vector<TestResult>& results);
    
    // Legacy compatibility function
    std::shared_ptr<Brain> createTestBrain(const TestConfig& config);
    
private:
    std::vector<std::unique_ptr<TestBase>> tests_;
    LogLevel logLevel_;
};

// Global test runner instance accessor
TestRunner& getTestRunner();
