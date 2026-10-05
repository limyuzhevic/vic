// NLM Testing Framework Implementation
// Provides complete implementation of testing and benchmarking features

#include "framework/TestingFramework.hpp"
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>

namespace nlm {
    
    // TestCase implementation
    namespace TestCase {
        // LambdaTestCase implementation
        LambdaTestCase::LambdaTestCase(const std::string& name, const std::string& description,
                                      std::function<TestResult(const std::shared_ptr<Config>&)> testFunction)
            : name_(name), description_(description), testFunction_(testFunction) {}
        
        std::string LambdaTestCase::getName() const {
            return name_;
        }
        
        std::string LambdaTestCase::getDescription() const {
            return description_;
        }
        
        TestResult LambdaTestCase::run(const std::shared_ptr<Config>& config) {
            TestResult result;
            result.testName = name_;
            result.description = description_;
            result.startTime = std::chrono::system_clock::now();
            
            try {
                result = testFunction_(config);
                result.calculateExecutionTime();
                result.passed = !result.errorMessage.empty(); // If there's an error, test failed
            } catch (const std::exception& e) {
                result.passed = false;
                result.errorMessage = std::string(e.what());
                result.calculateExecutionTime();
            }
            
            return result;
        }
    }
    
    // BenchmarkCase implementation
    namespace BenchmarkCase {
        // LambdaBenchmarkCase implementation
        LambdaBenchmarkCase::LambdaBenchmarkCase(const std::string& name, const std::string& description,
                                               std::function<BenchmarkResult(const std::shared_ptr<Config>&)> benchmarkFunction)
            : name_(name), description_(description), benchmarkFunction_(benchmarkFunction) {}
        
        std::string LambdaBenchmarkCase::getName() const {
            return name_;
        }
        
        std::string LambdaBenchmarkCase::getDescription() const {
            return description_;
        }
        
        BenchmarkResult LambdaBenchmarkCase::run(const std::shared_ptr<Config>& config) {
            BenchmarkResult result;
            result.benchmarkName = name_;
            result.description = description_;
            
            try {
                result = benchmarkFunction_(config);
                result.calculateExecutionTime();
            } catch (const std::exception& e) {
                result.benchmarkName = name_ + " (ERROR)";
                result.errorMessage = std::string(e.what());
                result.calculateExecutionTime();
            }
            
            return result;
        }
    }
    
    // TestSuite implementation
    namespace TestSuite {
        TestResults TestSuite::runAll(const std::shared_ptr<Config>& config) {
            TestResults results;
            
            NLM_LOG_INFO("Running test suite: " + name_);
            
            for (auto& testCase : testCases_) {
                NLM_LOG_INFO("Running test: " + testCase->getName());
                
                TestResult result = testCase->run(config);
                results.testResults.push_back(std::move(result));
            }
            
            return results;
        }
        
        TestResults TestSuite::runAllWithSweeps(const std::shared_ptr<Config>& config) {
            TestResults results;
            
            // Generate parameter combinations
            std::vector<std::map<std::string, float>> parameterCombinations;
            generateParameterCombinations(parameterCombinations);
            
            NLM_LOG_INFO("Running test suite: " + name_ + " with parameter sweeps");
            NLM_LOG_INFO("Generated " + std::to_string(parameterCombinations.size()) + " parameter combinations");
            
            for (size_t i = 0; i < parameterCombinations.size(); ++i) {
                const auto& params = parameterCombinations[i];
                
                NLM_LOG_INFO("Running test case " + std::to_string(i + 1) + "/" + 
                            std::to_string(parameterCombinations.size()) + 
                            " with parameters: " + formatParameters(params));
                
                for (auto& testCase : testCases_) {
                    TestResult result = testCase->runWithParameters(params);
                    result.testName += " (" + formatParameters(params) + ")";
                    results.testResults.push_back(std::move(result));
                }
            }
            
            return results;
        }
        
        BenchmarkResults TestSuite::runAllBenchmarks(const std::shared_ptr<Config>& config) {
            BenchmarkResults results;
            
            NLM_LOG_INFO("Running benchmark suite: " + name_);
            
            for (auto& testCase : testCases_) {
                // Cast to benchmark case if possible
                auto* benchmarkCase = dynamic_cast<BenchmarkCase*>(testCase.get());
                if (benchmarkCase) {
                    NLM_LOG_INFO("Running benchmark: " + benchmarkCase->getName());
                    
                    BenchmarkResult result = benchmarkCase->run(config);
                    results.benchmarkResults.push_back(std::move(result));
                }
            }
            
            return results;
        }
    }
    
    // BenchmarkSuite implementation
    namespace BenchmarkSuite {
        BenchmarkResults BenchmarkSuite::runAll(const std::shared_ptr<Config>& config) {
            BenchmarkResults results;
            
            NLM_LOG_INFO("Running benchmark suite: " + name_);
            
            for (auto& benchmarkCase : benchmarkCases_) {
                NLM_LOG_INFO("Running benchmark: " + benchmarkCase->getName());
                
                BenchmarkResult result = benchmarkCase->run(config);
                results.benchmarkResults.push_back(std::move(result));
            }
            
            return results;
        }
        
        BenchmarkResults BenchmarkSuite::runWithParameterSweeps(const std::shared_ptr<Config>& config) {
            BenchmarkResults results;
            
            // Generate parameter combinations
            std::vector<std::map<std::string, float>> parameterCombinations;
            generateParameterCombinations(parameterCombinations);
            
            NLM_LOG_INFO("Running benchmark suite: " + name_ + " with parameter sweeps");
            NLM_LOG_INFO("Generated " + std::to_string(parameterCombinations.size()) + " parameter combinations");
            
            for (size_t i = 0; i < parameterCombinations.size(); ++i) {
                const auto& params = parameterCombinations[i];
                
                NLM_LOG_INFO("Running benchmark case " + std::to_string(i + 1) + "/" + 
                            std::to_string(parameterCombinations.size()) + 
                            " with parameters: " + formatParameters(params));
                
                for (auto& benchmarkCase : benchmarkCases_) {
                    BenchmarkResult result = benchmarkCase->runWithParameters(params);
                    result.benchmarkName += " (" + formatParameters(params) + ")";
                    results.benchmarkResults.push_back(std::move(result));
                }
            }
            
            return results;
        }
        
        BenchmarkResults BenchmarkSuite::runWithMultipleIterations(const std::shared_ptr<Config>& config, 
                                                                  int iterations) {
            BenchmarkResults results;
            
            NLM_LOG_INFO("Running benchmark suite: " + name_ + " with " + 
                        std::to_string(iterations) + " iterations per benchmark");
            
            for (auto& benchmarkCase : benchmarkCases_) {
                NLM_LOG_INFO("Running benchmark: " + benchmarkCase->getName() + 
                            " for " + std::to_string(iterations) + " iterations");
                
                double totalTimeMs = 0.0;
                size_t totalMemoryKB = 0;
                double totalSpikesPerSec = 0.0;
                double totalSynapsesPerMs = 0.0;
                
                for (int i = 0; i < iterations; ++i) {
                    NLM_LOG_INFO("  Iteration " + std::to_string(i + 1) + "/" + std::to_string(iterations));
                    
                    BenchmarkResult result = benchmarkCase->run(config);
                    
                    totalTimeMs += result.executionTimeMs;
                    totalMemoryKB += result.memoryUsageKB;
                    totalSpikesPerSec += result.spikesPerSecond;
                    totalSynapsesPerMs += result.synapsesPerMs;
                    
                    // Store individual iteration result
                    results.benchmarkResults.push_back(result);
                }
                
                // Calculate averages
                BenchmarkResult avgResult;
                avgResult.benchmarkName = benchmarkCase->getName() + " (average)";
                avgResult.description = benchmarkCase->getDescription();
                avgResult.executionTimeMs = totalTimeMs / iterations;
                avgResult.memoryUsageKB = totalMemoryKB / iterations;
                avgResult.spikesPerSecond = totalSpikesPerSec / iterations;
                avgResult.synapsesPerMs = totalSynapsesPerMs / iterations;
                
                results.benchmarkResults.push_back(avgResult);
            }
            
            return results;
        }
    }
    
    // TestRunner implementation
    namespace TestRunner {
        TestResults TestRunner::runTestSuite(TestSuite& testSuite, const std::shared_ptr<Config>& config) {
            NLM_LOG_INFO("Starting test suite: " + testSuite.getName());
            
            TestResults results;
            
            // Setup phase
            for (auto& testCase : testSuite.getTestCases()) {
                testCase->setup(config);
            }
            
            try {
                // Run tests
                if (hasParameterSweeps(testSuite)) {
                    results = testSuite.runAllWithSweeps(config);
                } else {
                    results = testSuite.runAll(config);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Test suite execution failed: " + std::string(e.what()));
            }
            
            // Teardown phase
            for (auto& testCase : testSuite.getTestCases()) {
                testCase->teardown(config);
            }
            
            NLM_LOG_INFO("Test suite completed: " + testSuite.getName());
            NLM_LOG_INFO("Tests passed: " + std::to_string(countPassedTests(results)));
            NLM_LOG_INFO("Tests failed: " + std::to_string(countFailedTests(results)));
            
            return results;
        }
        
        BenchmarkResults TestRunner::runBenchmarkSuite(BenchmarkSuite& benchmarkSuite, 
                                                      const std::shared_ptr<Config>& config) {
            NLM_LOG_INFO("Starting benchmark suite: " + benchmarkSuite.getName());
            
            BenchmarkResults results;
            
            // Setup phase
            for (auto& benchmarkCase : benchmarkSuite.getBenchmarkCases()) {
                benchmarkCase->setup(config);
            }
            
            try {
                // Run benchmarks
                if (hasParameterSweeps(benchmarkSuite)) {
                    results = benchmarkSuite.runWithParameterSweeps(config);
                } else {
                    results = benchmarkSuite.runAll(config);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Benchmark suite execution failed: " + std::string(e.what()));
            }
            
            // Teardown phase
            for (auto& benchmarkCase : benchmarkSuite.getBenchmarkCases()) {
                benchmarkCase->teardown(config);
            }
            
            NLM_LOG_INFO("Benchmark suite completed: " + benchmarkSuite.getName());
            
            return results;
        }
        
        TestResults TestRunner::runTestMultipleIterations(TestCase& testCase, 
                                                         const std::shared_ptr<Config>& config,
                                                         int iterations) {
            TestResults results;
            
            NLM_LOG_INFO("Running test " + testCase.getName() + " for " + 
                        std::to_string(iterations) + " iterations");
            
            // Setup
            testCase.setup(config);
            
            try {
                for (int i = 0; i < iterations; ++i) {
                    NLM_LOG_INFO("  Iteration " + std::to_string(i + 1) + "/" + std::to_string(iterations));
                    
                    TestResult result = testCase.run(config);
                    result.testName += " (iteration " + std::to_string(i + 1) + ")";
                    results.testResults.push_back(std::move(result));
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Test execution failed: " + std::string(e.what()));
            }
            
            // Teardown
            testCase.teardown(config);
            
            return results;
        }
        
        BenchmarkResults TestRunner::runBenchmarkMultipleIterations(BenchmarkCase& benchmarkCase,
                                                                   const std::shared_ptr<Config>& config,
                                                                   int iterations) {
            BenchmarkResults results;
            
            NLM_LOG_INFO("Running benchmark " + benchmarkCase.getName() + " for " + 
                        std::to_string(iterations) + " iterations");
            
            // Setup
            benchmarkCase.setup(config);
            
            try {
                for (int i = 0; i < iterations; ++i) {
                    NLM_LOG_INFO("  Iteration " + std::to_string(i + 1) + "/" + std::to_string(iterations));
                    
                    BenchmarkResult result = benchmarkCase.run(config);
                    result.benchmarkName += " (iteration " + std::to_string(i + 1) + ")";
                    results.benchmarkResults.push_back(std::move(result));
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Benchmark execution failed: " + std::string(e.what()));
            }
            
            // Teardown
            benchmarkCase.teardown(config);
            
            return results;
        }
        
        TestAnalysis TestRunner::analyzeTestResults(const TestResults& results) {
            TestAnalysis analysis;
            
            analysis.totalTests = results.testResults.size();
            analysis.passedTests = countPassedTests(results);
            analysis.failedTests = countFailedTests(results);
            
            if (analysis.totalTests > 0) {
                analysis.passRate = static_cast<double>(analysis.passedTests) / analysis.totalTests * 100.0;
            }
            
            // Calculate execution time statistics
            double totalTime = 0.0;
            double maxTime = 0.0;
            for (const auto& test : results.testResults) {
                totalTime += test.executionTimeMs;
                maxTime = std::max(maxTime, test.executionTimeMs);
            }
            
            analysis.avgExecutionTime = totalTime / results.testResults.size();
            analysis.maxExecutionTime = maxTime;
            
            // Calculate metric statistics
            for (const auto& test : results.testResults) {
                for (const auto& metric : test.metrics) {
                    analysis.metricStats[metric.first].total += metric.second;
                    analysis.metricStats[metric.first].count++;
                    analysis.metricStats[metric.first].average = 
                        analysis.metricStats[metric.first].total / analysis.metricStats[metric.first].count;
                }
            }
            
            return analysis;
        }
        
        BenchmarkAnalysis TestRunner::analyzeBenchmarkResults(const BenchmarkResults& results) {
            BenchmarkAnalysis analysis;
            
            analysis.totalBenchmarks = results.benchmarkResults.size();
            
            // Calculate execution time statistics
            double totalTime = 0.0;
            double maxTime = 0.0;
            size_t totalMemory = 0;
            double totalSpikesPerSec = 0.0;
            double totalSynapsesPerMs = 0.0;
            
            for (const auto& benchmark : results.benchmarkResults) {
                totalTime += benchmark.executionTimeMs;
                maxTime = std::max(maxTime, benchmark.executionTimeMs);
                totalMemory += benchmark.memoryUsageKB;
                totalSpikesPerSec += benchmark.spikesPerSecond;
                totalSynapsesPerMs += benchmark.synapsesPerMs;
            }
            
            if (analysis.totalBenchmarks > 0) {
                analysis.avgExecutionTime = totalTime / analysis.totalBenchmarks;
                analysis.avgMemoryUsage = static_cast<double>(totalMemory) / analysis.totalBenchmarks;
                analysis.avgSpikesPerSecond = totalSpikesPerSec / analysis.totalBenchmarks;
                analysis.avgSynapsesPerMs = totalSynapsesPerMs / analysis.totalBenchmarks;
            }
            
            analysis.maxExecutionTime = maxTime;
            
            return analysis;
        }
        
        bool TestRunner::exportTestResults(const std::string& filename, const TestResults& results) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open test results file: " + filename);
                return false;
            }
            
            file << "NLM Test Results" << std::endl;
            file << "===============" << std::endl;
            file << std::endl;
            
            file << "Total Tests: " << results.testResults.size() << std::endl;
            file << "Passed: " << countPassedTests(results) << std::endl;
            file << "Failed: " << countFailedTests(results) << std::endl;
            file << std::endl;
            
            for (const auto& test : results.testResults) {
                file << "Test: " << test.testName << std::endl;
                file << "Description: " << test.description << std::endl;
                file << "Status: " << (test.passed ? "PASSED" : "FAILED") << std::endl;
                file << "Execution Time: " << test.getDurationString() << std::endl;
                if (!test.errorMessage.empty()) {
                    file << "Error: " << test.errorMessage << std::endl;
                }
                file << "Metrics:" << std::endl;
                for (const auto& metric : test.metrics) {
                    file << "  " << metric.first << ": " << metric.second << std::endl;
                }
                file << std::endl;
            }
            
            file.close();
            NLM_LOG_INFO("Test results exported to: " + filename);
            return true;
        }
        
        bool TestRunner::exportBenchmarkResults(const std::string& filename, const BenchmarkResults& results) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open benchmark results file: " + filename);
                return false;
            }
            
            file << "NLM Benchmark Results" << std::endl;
            file << "========================" << std::endl;
            file << std::endl;
            
            file << "Total Benchmarks: " << results.benchmarkResults.size() << std::endl;
            file << std::endl;
            
            for (const auto& benchmark : results.benchmarkResults) {
                file << "Benchmark: " << benchmark.benchmarkName << std::endl;
                file << "Description: " << benchmark.description << std::endl;
                file << "Execution Time: " << benchmark.getDurationString() << std::endl;
                file << "Memory Usage: " << benchmark.memoryUsageKB << " KB" << std::endl;
                file << "Spikes/Second: " << benchmark.spikesPerSecond << std::endl;
                file << "Synapses/ms: " << benchmark.synapsesPerMs << std::endl;
                file << "CPU Usage: " << benchmark.cpuUsagePercent << "%" << std::endl;
                file << "Threads: " << benchmark.threadCount << std::endl;
                file << "Metrics:" << std::endl;
                for (const auto& metric : benchmark.metrics) {
                    file << "  " << metric.first << ": " << metric.second << std::endl;
                }
                file << std::endl;
            }
            
            file.close();
            NLM_LOG_INFO("Benchmark results exported to: " + filename);
            return true;
        }
        
        bool TestRunner::hasParameterSweeps(const TestSuite& suite) const {
            for (const auto& testCase : suite.getTestCases()) {
                if (!testCase->getParameterSweeps().empty()) {
                    return true;
                }
            }
            return false;
        }
        
        bool TestRunner::hasParameterSweeps(const BenchmarkSuite& suite) const {
            for (const auto& benchmarkCase : suite.getBenchmarkCases()) {
                if (!benchmarkCase->getParameterSweeps().empty()) {
                    return true;
                }
            }
            return false;
        }
        
        int TestRunner::countPassedTests(const TestResults& results) const {
            int count = 0;
            for (const auto& test : results.testResults) {
                if (test.passed) {
                    count++;
                }
            }
            return count;
        }
        
        int TestRunner::countFailedTests(const TestResults& results) const {
            int count = 0;
            for (const auto& test : results.testResults) {
                if (!test.passed) {
                    count++;
                }
            }
            return count;
        }
    }
    
} // namespace nlm
