#pragma once

/**
 * Performance Documentation
 * 
 * This module provides comprehensive performance documentation for NLM,
 * including performance documentation, performance examples,
 * performance tutorials, and performance best practices.
 * 
 * Key Features:
 * - Performance documentation generation
 * - Performance examples generation
 * - Performance tutorials generation
 * - Performance best practices documentation
 * - Performance API documentation
 * - Performance configuration documentation
 * - Performance setup documentation
 * - Performance troubleshooting documentation
 * 
 * Architecture:
 * - DocumentationBase: Abstract base class for documentation
 * - PerformanceDocumentation: Main documentation generator
 * - PerformanceExamples: Documentation for examples
 * - PerformanceTutorials: Documentation for tutorials
 * - PerformanceBestPractices: Documentation for best practices
 * - PerformanceAPIDoc: Documentation for API
 * - PerformanceConfigDoc: Documentation for configuration
 * - PerformanceSetupDoc: Documentation for setup
 * - PerformanceTroubleshootingDoc: Documentation for troubleshooting
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <functional>
#include <cmath>
#include <set>
#include <filesystem>

namespace nlm {
namespace documentation {

// Documentation data structures
struct DocumentationSection {
    std::string section_id;
    std::string title;
    std::string description;
    std::string content;
    std::vector<std::string> examples;
    std::vector<std::string> code_samples;
    std::vector<std::string> references;
    std::string category;  // api, configuration, tutorial, example, best_practice, troubleshooting
    int order;
    std::map<std::string, std::string> metadata;
};

struct DocumentationPage {
    std::string page_id;
    std::string title;
    std::string description;
    std::vector<DocumentationSection> sections;
    std::vector<std::string> related_pages;
    std::string last_updated;
    std::string author;
    std::string category;  // api, configuration, tutorial, example, best_practice, troubleshooting
    std::map<std::string, std::string> page_metadata;
};

struct DocumentationExample {
    std::string example_id;
    std::string title;
    std::string description;
    std::string code;
    std::string output;
    std::string language;  // c++, python, etc.
    std::string difficulty;  // beginner, intermediate, advanced
    std::vector<std::string> prerequisites;
    std::vector<std::string> learning_objectives;
    std::map<std::string, std::string> example_metadata;
};

struct DocumentationTutorial {
    std::string tutorial_id;
    std::string title;
    std::string description;
    std::vector<std::string> objectives;
    std::vector<std::string> steps;
    std::vector<std::string> prerequisites;
    std::string estimated_duration;
    std::string difficulty;  // beginner, intermediate, advanced
    std::vector<DocumentationExample> examples;
    std::map<std::string, std::string> tutorial_metadata;
};

struct DocumentationBestPractice {
    std::string practice_id;
    std::string title;
    std::string description;
    std::string category;  // profiling, benchmarking, monitoring, optimization, analysis
    std::string impact;  // performance, memory, cpu, network
    std::string implementation_difficulty;
    std::string expected_improvement;
    std::string code_example;
    std::string reference_documentation;
    std::vector<std::string> steps_to_implement;
    std::map<std::string, std::string> practice_metadata;
};

// Abstract base class for documentation
class DocumentationBase {
protected:
    std::string name_;
    bool enabled_;
    bool verbose_;
    std::mutex documentation_mutex_;
    
public:
    DocumentationBase(const std::string& name) : name_(name), enabled_(false), verbose_(false) {}
    virtual ~DocumentationBase() = default;
    
    virtual std::string generate_documentation() = 0;
    virtual std::string generate_api_documentation() = 0;
    virtual std::string generate_configuration_documentation() = 0;
    virtual std::string generate_tutorial_documentation() = 0;
    virtual std::string generate_example_documentation() = 0;
    virtual std::string generate_best_practice_documentation() = 0;
    virtual std::string generate_setup_documentation() = 0;
    virtual std::string generate_troubleshooting_documentation() = 0;
    
    bool is_enabled() const { return enabled_; }
    void set_enabled(bool enabled) { enabled_ = enabled; }
    bool is_verbose() const { return verbose_; }
    void set_verbose(bool verbose) { verbose_ = verbose; }
    const std::string& get_name() const { return name_; }
};

// Performance Documentation Generator
class PerformanceDocumentation : public DocumentationBase {
private:
    std::vector<DocumentationPage> pages_;
    std::vector<DocumentationExample> examples_;
    std::vector<DocumentationTutorial> tutorials_;
    std::vector<DocumentationBestPractice> best_practices_;
    std::string output_directory_;
    
public:
    PerformanceDocumentation(const std::string& name, const std::string& output_directory = ".")
        : DocumentationBase(name), output_directory_(output_directory) {
        generate_default_content();
    }
    
    void add_page(const DocumentationPage& page) {
        std::lock_guard<std::mutex> lock(documentation_mutex_);
        pages_.push_back(page);
    }
    
    void add_example(const DocumentationExample& example) {
        std::lock_guard<std::mutex> lock(documentation_mutex_);
        examples_.push_back(example);
    }
    
    void add_tutorial(const DocumentationTutorial& tutorial) {
        std::lock_guard<std::mutex> lock(documentation_mutex_);
        tutorials_.push_back(tutorial);
    }
    
    void add_best_practice(const DocumentationBestPractice& practice) {
        std::lock_guard<std::mutex> lock(documentation_mutex_);
        best_practices_.push_back(practice);
    }
    
    std::string generate_documentation() override {
        std::stringstream ss;
        ss << generate_overview() << std::endl;
        ss << generate_table_of_contents() << std::endl;
        ss << generate_api_documentation() << std::endl;
        ss << generate_configuration_documentation() << std::endl;
        ss << generate_tutorial_documentation() << std::endl;
        ss << generate_example_documentation() << std::endl;
        ss << generate_best_practice_documentation() << std::endl;
        ss << generate_setup_documentation() << std::endl;
        ss << generate_troubleshooting_documentation() << std::endl;
        
        return ss.str();
    }
    
    std::string generate_api_documentation() override {
        std::stringstream ss;
        ss << "=== API Documentation ===" << std::endl;
        ss << std::endl;
        
        for (const auto& page : pages_) {
            if (page.category == "api") {
                ss << "## " << page.title << std::endl;
                ss << page.description << std::endl;
                ss << std::endl;
                
                for (const auto& section : page.sections) {
                    ss << "### " << section.title << std::endl;
                    ss << section.content << std::endl;
                    ss << std::endl;
                    
                    if (!section.code_samples.empty()) {
                        ss << "#### Code Examples:" << std::endl;
                        for (const auto& code : section.code_samples) {
                            ss << "```cpp" << std::endl;
                            ss << code << std::endl;
                            ss << "```" << std::endl;
                            ss << std::endl;
                        }
                    }
                }
            }
        }
        
        return ss.str();
    }
    
    std::string generate_configuration_documentation() override {
        std::stringstream ss;
        ss << "=== Configuration Documentation ===" << std::endl;
        ss << std::endl;
        
        for (const auto& page : pages_) {
            if (page.category == "configuration") {
                ss << "## " << page.title << std::endl;
                ss << page.description << std::endl;
                ss << std::endl;
                
                for (const auto& section : page.sections) {
                    ss << "### " << section.title << std::endl;
                    ss << section.content << std::endl;
                    ss << std::endl;
                    
                    if (!section.examples.empty()) {
                        ss << "#### Configuration Examples:" << std::endl;
                        for (const auto& example : section.examples) {
                            ss << "```" << example << "```" << std::endl;
                            ss << std::endl;
                        }
                    }
                }
            }
        }
        
        return ss.str();
    }
    
    std::string generate_tutorial_documentation() override {
        std::stringstream ss;
        ss << "=== Tutorial Documentation ===" << std::endl;
        ss << std::endl;
        
        for (const auto& tutorial : tutorials_) {
            ss << "## " << tutorial.title << std::endl;
            ss << tutorial.description << std::endl;
            ss << std::endl;
            
            ss << "### Objectives:" << std::endl;
            for (const auto& objective : tutorial.objectives) {
                ss << "- " << objective << std::endl;
            }
            ss << std::endl;
            
            ss << "### Prerequisites:" << std::endl;
            for (const auto& prerequisite : tutorial.prerequisites) {
                ss << "- " << prerequisite << std::endl;
            }
            ss << std::endl;
            
            ss << "### Steps:" << std::endl;
            for (size_t i = 0; i < tutorial.steps.size(); ++i) {
                ss << i + 1 << ". " << tutorial.steps[i] << std::endl;
            }
            ss << std::endl;
            
            ss << "### Estimated Duration: " << tutorial.estimated_duration << std::endl;
            ss << "### Difficulty: " << tutorial.difficulty << std::endl;
            ss << std::endl;
            
            if (!tutorial.examples.empty()) {
                ss << "### Examples:" << std::endl;
                for (const auto& example : tutorial.examples) {
                    ss << "#### " << example.title << std::endl;
                    ss << example.description << std::endl;
                    ss << "**Language:** " << example.language << std::endl;
                    ss << "**Difficulty:** " << example.difficulty << std::endl;
                    ss << std::endl;
                    ss << "```" << example.language << std::endl;
                    ss << example.code << std::endl;
                    ss << "```" << std::endl;
                    if (!example.output.empty()) {
                        ss << "**Output:** " << example.output << std::endl;
                    }
                    ss << std::endl;
                }
            }
        }
        
        return ss.str();
    }
    
    std::string generate_example_documentation() override {
        std::stringstream ss;
        ss << "=== Example Documentation ===" << std::endl;
        ss << std::endl;
        
        for (const auto& example : examples_) {
            ss << "## " << example.title << std::endl;
            ss << example.description << std::endl;
            ss << std::endl;
            
            ss << "**Language:** " << example.language << std::endl;
            ss << "**Difficulty:** " << example.difficulty << std::endl;
            ss << std::endl;
            
            ss << "### Prerequisites:" << std::endl;
            for (const auto& prerequisite : example.prerequisites) {
                ss << "- " << prerequisite << std::endl;
            }
            ss << std::endl;
            
            ss << "### Learning Objectives:" << std::endl;
            for (const auto& objective : example.learning_objectives) {
                ss << "- " << objective << std::endl;
            }
            ss << std::endl;
            
            ss << "```" << example.language << std::endl;
            ss << example.code << std::endl;
            ss << "```" << std::endl;
            if (!example.output.empty()) {
                ss << "### Output:" << std::endl;
                ss << "```" << std::endl;
                ss << example.output << std::endl;
                ss << "```" << std::endl;
                ss << std::endl;
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    std::string generate_best_practice_documentation() override {
        std::stringstream ss;
        ss << "=== Best Practice Documentation ===" << std::endl;
        ss << std::endl;
        
        for (const auto& practice : best_practices_) {
            ss << "## " << practice.title << std::endl;
            ss << practice.description << std::endl;
            ss << std::endl;
            
            ss << "**Category:** " << practice.category << std::endl;
            ss << "**Impact:** " << practice.impact << std::endl;
            ss << "**Implementation Difficulty:** " << practice.implementation_difficulty << std::endl;
            ss << "**Expected Improvement:** " << practice.expected_improvement << std::endl;
            ss << std::endl;
            
            ss << "### Steps to Implement:" << std::endl;
            for (const auto& step : practice.steps_to_implement) {
                ss << "1. " << step << std::endl;
            }
            ss << std::endl;
            
            ss << "### Code Example:" << std::endl;
            ss << "```cpp" << std::endl;
            ss << practice.code_example << std::endl;
            ss << "```" << std::endl;
            ss << std::endl;
            
            ss << "**Reference Documentation:** " << practice.reference_documentation << std::endl;
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    std::string generate_setup_documentation() override {
        std::stringstream ss;
        ss << "=== Setup Documentation ===" << std::endl;
        ss << std::endl;
        
        ss << "## Getting Started with NLM Performance Tools" << std::endl;
        ss << std::endl;
        ss << "This document provides setup instructions for using NLM performance profiling, benchmarking, monitoring, optimization, and analysis tools." << std::endl;
        ss << std::endl;
        
        ss << "### Prerequisites" << std::endl;
        ss << "- NLM framework (Phase 5 or later)" << std::endl;
        ss << "- C++ compiler with C++17 support" << std::endl;
        ss << "- CMake 3.10 or higher" << std::endl;
        ss << "- Build system (Make, Ninja, etc.)" << std::endl;
        ss << std::endl;
        
        ss << "### Installation Steps" << std::endl;
        ss << "1. Clone the NLM repository" << std::endl;
        ss << "2. Navigate to the project directory" << std::endl;
        ss << "3. Create build directory: `mkdir build && cd build`" << std::endl;
        ss << "4. Run CMake: `cmake -DCMAKE_BUILD_TYPE=Release ..`" << std::endl;
        ss << "5. Build the project: `make -j$(nproc)`" << std::endl;
        ss << std::endl;
        
        ss << "### Performance Tools Installation" << std::endl;
        ss << "The performance tools are built as part of the main NLM build. Enable performance features with:" << std::endl;
        ss << "```bash" << std::endl;
        ss << "cmake -DNLM_PERFORMANCE_ENABLED=ON .." << std::endl;
        ss << "```" << std::endl;
        ss << std::endl;
        
        return ss.str();
    }
    
    std::string generate_troubleshooting_documentation() override {
        std::stringstream ss;
        ss << "=== Troubleshooting Documentation ===" << std::endl;
        ss << std::endl;
        
        ss << "## Common Performance Issues and Solutions" << std::endl;
        ss << std::endl;
        
        ss << "### Issue: Poor Performance in Simulation" << std::endl;
        ss << "**Symptoms:** Slow simulation, high CPU usage, memory bottlenecks" << std::endl;
        ss << "**Solutions:**" << std::endl;
        ss << "1. Enable performance optimizations in CMake: `-DNLM_PERFORMANCE_ENABLED=ON`" << std::endl;
        ss << "2. Use SIMD vectorization: `-DNLM_SIMD_AVX512=ON`" << std::endl;
        ss << "3. Enable multithreading: `-DNLM_THREAD_COUNT=8`" << std::endl;
        ss << "4. Optimize memory pools and cache locality" << std::endl;
        ss << "5. Profile the simulation to identify bottlenecks" << std::endl;
        ss << std::endl;
        
        ss << "### Issue: Memory Leaks" << std::endl;
        ss << "**Symptoms:** Memory usage grows over time, crash after long simulations" << std::endl;
        ss << "**Solutions:**" << std::endl;
        ss << "1. Use memory profiler to identify leaks" << std::endl;
        ss << "2. Enable memory tracking: `-DNLM_MEMORY_LEAK_DETECTION=ON`" << std::endl;
        ss << "3. Use memory pools instead of dynamic allocation" << std::endl;
        ss << "4. Check for dangling pointers and double free" << std::endl;
        ss << std::endl;
        
        ss << "### Issue: Benchmarking Results Inconsistent" << std::endl;
        ss << "**Symptoms:** Different results when running benchmarks multiple times" << std::endl;
        ss << "**Solutions:**" << std::endl;
        ss << "1. Warm up the system before running benchmarks" << std::endl;
        ss << "2. Use consistent benchmark parameters" << std::endl;
        ss << "3. Run benchmarks multiple times and use average results" << std::endl;
        ss << "4. Check for external system load affecting results" << std::endl;
        ss << std::endl;
        
        ss << "### Issue: Performance Profiling Data Not Generated" << std::endl;
        ss << "**Symptoms:** Profiling tools enabled but no data collected" << std::endl;
        ss << "**Solutions:**" << std::endl;
        ss << "1. Check if profiling is properly initialized" << std::endl;
        ss << "2. Verify profiling configuration is correct" << std::endl;
        ss << "3. Check for profiling file permissions" << std::endl;
        ss << "4. Restart the application with profiling enabled" << std::endl;
        ss << std::endl;
        
        ss << "### Issue: Monitoring System Not Collecting Data" << std::endl;
        ss << "**Symptoms:** Monitoring enabled but no metrics collected" << std::endl;
        ss << "**Solutions:**" << std::endl;
        ss << "1. Check if monitoring is properly configured" << std::endl;
        ss << "2. Verify monitoring data collection interval" << std::endl;
        ss << "3. Check for network connectivity issues" << std::endl;
        ss << "4. Restart monitoring system" << std::endl;
        ss << std::endl;
        
        ss << "### Issue: Optimization Results Not Applied" << std::endl;
        ss << "**Symptoms:** Optimization attempted but performance didn't improve" << std::endl;
        ss << "**Solutions:**" << std::endl;
        ss << "1. Check if optimization is actually applied" << std::endl;
        ss << "2. Verify optimization configuration" << std::endl;
        ss << "3. Check for optimization validation failures" << std::endl;
        ss << "4. Re-run optimization after system restart" << std::endl;
        ss << std::endl;
        
        ss << "### Issue: Analysis Results Not Generated" << std::endl;
        ss << "**Symptoms:** Analysis requested but no results generated" << std::endl;
        ss << "**Solutions:**" << std::endl;
        ss << "1. Check if analysis data is available" << std::endl;
        ss << "2. Verify analysis configuration" << std::endl;
        ss << "3. Check for analysis tool dependencies" << std::endl;
        ss << "4. Restart analysis system" << std::endl;
        ss << std::endl;
        
        return ss.str();
    }
    
private:
    std::string generate_overview() {
        std::stringstream ss;
        ss << "# NLM Performance Documentation" << std::endl;
        ss << std::endl;
        ss << "This documentation covers comprehensive performance profiling, benchmarking, monitoring, optimization, and analysis tools for the NLM (Neural Learning Machine) framework." << std::endl;
        ss << std::endl;
        ss << "## Key Features" << std::endl;
        ss << std::endl;
        ss << "### Performance Profiling Tools" << std::endl;
        ss << "- CPU profiler for performance bottleneck identification" << std::endl;
        ss << "- Memory profiler for memory usage analysis" << std::endl;
        ss << "- Network profiler for network I/O monitoring" << std::endl;
        ss << "- I/O profiler for file and device I/O tracking" << std::endl;
        ss << "- GPU profiler for GPU operations profiling" << std::endl;
        ss << "- Timeline profiler for event timeline tracking" << std::endl;
        ss << "- Event profiler for system and application event profiling" << std::endl;
        ss << std::endl;
        ss << "### Benchmarking Tools" << std::endl;
        ss << "- Benchmarking framework for performance testing" << std::endl;
        ss << "- Performance benchmark suite for system evaluation" << std::endl;
        ss << "- System benchmark for overall system performance" << std::endl;
        ss << "- Component benchmark for individual component performance" << std::endl;
        ss << "- Integration benchmark for system integration testing" << std::endl;
        ss << "- End-to-end benchmark for complete workflow testing" << std::endl;
        ss << std::endl;
        ss << "### Performance Monitoring Tools" << std::endl;
        ss << "- Performance monitoring system for real-time monitoring" << std::endl;
        ss << "- Real-time performance monitoring with alerts" << std::endl;
        ss << "- Performance alerting system with configurable thresholds" << std::endl;
        ss << "- Performance reporting system with multiple output formats" << std::endl;
        ss << "- Performance analysis system with trend detection" << std::endl;
        ss << std::endl;
        ss << "### Performance Optimization Tools" << std::endl;
        ss << "- Performance optimization framework with configurable strategies" << std::endl;
        ss << "- Performance profiling-based optimization" << std::endl;
        ss << "- Performance analysis-based optimization" << std::endl;
        ss << "- Performance benchmarking-based optimization" << std::endl;
        ss << "- Performance testing-based optimization" << std::endl;
        ss << "- Performance monitoring-based optimization" << std::endl;
        ss << std::endl;
        ss << "### Performance Analysis Tools" << std::endl;
        ss << "- Performance analysis framework for comprehensive analysis" << std::endl;
        ss << "- Performance data analysis for raw data processing" << std::endl;
        ss << "- Performance trend analysis for time-series analysis" << std::endl;
        ss << "- Performance correlation analysis for relationship analysis" << std::endl;
        ss << "- Performance prediction for forecasting" << std::endl;
        ss << "- Performance optimization recommendation for actionable insights" << std::endl;
        ss << std::endl;
        ss << "## Integration" << std::endl;
        ss << std::endl;
        ss << "All performance tools integrate with the existing NLM framework, providing comprehensive performance management capabilities while maintaining backward compatibility." << std::endl;
        ss << std::endl;
        ss << "## Usage" << std::endl;
        ss << std::endl;
        ss << "Refer to the individual tool documentation for specific usage instructions, configuration options, and examples." << std::endl;
        ss << std::endl;
        
        return ss.str();
    }
    
    std::string generate_table_of_contents() {
        std::stringstream ss;
        ss << "## Table of Contents" << std::endl;
        ss << std::endl;
        ss << "1. [API Documentation](#api-documentation)" << std::endl;
        ss << "2. [Configuration Documentation](#configuration-documentation)" << std::endl;
        ss << "3. [Tutorial Documentation](#tutorial-documentation)" << std::endl;
        ss << "4. [Example Documentation](#example-documentation)" << std::endl;
        ss << "5. [Best Practice Documentation](#best-practice-documentation)" << std::endl;
        ss << "6. [Setup Documentation](#setup-documentation)" << std::endl;
        ss << "7. [Troubleshooting Documentation](#troubleshooting-documentation)" << std::endl;
        ss << std::endl;
        
        return ss.str();
    }
    
    void generate_default_content() {
        // Generate default documentation content
        
        // Add API pages
        DocumentationPage api_page;
        api_page.page_id = "api_overview";
        api_page.title = "API Overview";
        api_page.description = "Overview of performance profiling API";
        api_page.category = "api";
        api_page.order = 1;
        
        DocumentationSection api_section;
        api_section.section_id = "profiler_api";
        api_section.title = "Profiler API";
        api_section.description = "API for performance profiling tools";
        api_section.content = "The performance profiling API provides comprehensive profiling capabilities for NLM simulations.";
        api_section.category = "api";
        api_section.order = 1;
        api_page.sections.push_back(api_section);
        
        pages_.push_back(api_page);
        
        // Add configuration pages
        DocumentationPage config_page;
        config_page.page_id = "configuration_overview";
        config_page.title = "Configuration Overview";
        config_page.description = "Configuration options for performance tools";
        config_page.category = "configuration";
        config_page.order = 2;
        
        DocumentationSection config_section;
        config_section.section_id = "profiler_configuration";
        config_section.title = "Profiler Configuration";
        config_section.description = "Configuration options for profilers";
        config_section.content = "Profilers can be configured with various options including sampling intervals, thresholds, and output formats.";
        config_section.category = "configuration";
        config_section.order = 1;
        config_page.sections.push_back(config_section);
        
        pages_.push_back(config_page);
        
        // Add example
        DocumentationExample example;
        example.example_id = "basic_profiling_example";
        example.title = "Basic Profiling Example";
        example.description = "Example of basic profiling usage";
        example.code = "nlm::profiling::start_performance_profiling();\n// ... run simulation ...\nnlm::profiling::stop_performance_profiling();\nnlm::profiling::print_performance_summary();";
        example.language = "cpp";
        example.difficulty = "beginner";
        example.prerequisites = {"Basic C++ knowledge", "NLM framework installed"};
        example.learning_objectives = {"Understand profiling API", "Collect performance data", "Analyze profiling results"};
        examples_.push_back(example);
        
        // Add tutorial
        DocumentationTutorial tutorial;
        tutorial.tutorial_id = "profiling_tutorial";
        tutorial.title = "NLM Performance Profiling Tutorial";
        tutorial.description = "A comprehensive tutorial on using NLM performance profiling tools";
        tutorial.objectives = {"Set up performance profiling", "Collect profiling data", "Analyze profiling results", "Optimize performance based on profiling data"};
        tutorial.prerequisites = {"NLM framework installed", "Build with performance features enabled"};
        tutorial.estimated_duration = "30 minutes";
        tutorial.difficulty = "intermediate";
        tutorial.steps = {
            "Initialize performance profiler",
            "Configure profiling options",
            "Run simulation with profiling enabled",
            "Analyze profiling results",
            "Apply performance optimizations"
        };
        tutorials_.push_back(tutorial);
        
        // Add best practices
        DocumentationBestPractice best_practice;
        best_practice.practice_id = "profile_before_optimize";
        best_practice.title = "Profile Before Optimizing";
        best_practice.description = "Always profile your application before attempting optimizations to identify bottlenecks";
        best_practice.category = "profiling";
        best_practice.impact = "performance";
        best_practice.implementation_difficulty = "easy";
        best_practice.expected_improvement = "10-50%";
        best_practice.code_example = "// Profile the application\nnlm::profiling::start_performance_profiling();\n// Run application\nnlm::profiling::stop_performance_profiling();\n// Analyze results\nnlm::profiling::print_performance_summary();";
        best_practice.reference_documentation = "NLM Performance Documentation";
        best_practice.steps_to_implement = {
            "Enable performance profiling in build configuration",
            "Start performance profiler before application execution",
            "Stop performance profiler after application completion",
            "Analyze profiling results to identify bottlenecks",
            "Apply optimizations to identified bottlenecks"
        };
        best_practices_.push_back(best_practice);
    }
};

// Global documentation instance
static std::unique_ptr<PerformanceDocumentation> global_documentation;

// Convenience functions
namespace nlm {
namespace documentation {

inline PerformanceDocumentation& getGlobalDocumentation() {
    if (!global_documentation) {
        global_documentation = std::make_unique<PerformanceDocumentation>("performance_documentation");
    }
    return *global_documentation;
}

inline std::string generate_full_documentation() {
    return getGlobalDocumentation().generate_documentation();
}

inline std::string generate_api_documentation() {
    return getGlobalDocumentation().generate_api_documentation();
}

inline std::string generate_configuration_documentation() {
    return getGlobalDocumentation().generate_configuration_documentation();
}

inline std::string generate_tutorial_documentation() {
    return getGlobalDocumentation().generate_tutorial_documentation();
}

inline std::string generate_example_documentation() {
    return getGlobalDocumentation().generate_example_documentation();
}

inline std::string generate_best_practice_documentation() {
    return getGlobalDocumentation().generate_best_practice_documentation();
}

inline std::string generate_setup_documentation() {
    return getGlobalDocumentation().generate_setup_documentation();
}

inline std::string generate_troubleshooting_documentation() {
    return getGlobalDocumentation().generate_troubleshooting_documentation();
}

inline void enable_documentation(bool enabled) {
    getGlobalDocumentation().set_enabled(enabled);
}

inline void enable_verbose_documentation(bool verbose) {
    getGlobalDocumentation().set_verbose(verbose);
}

} // namespace documentation
} // namespace nlm

// Example usage and demonstration
void demonstrate_documentation() {
    std::cout << "=== NLM Performance Documentation Demonstration ===" << std::endl;
    
    // Generate documentation
    std::cout << "Generating full documentation..." << std::endl;
    std::string documentation = nlm::documentation::generate_full_documentation();
    
    // Print to file
    std::ofstream file("performance_documentation.md");
    if (file.is_open()) {
        file << documentation;
        file.close();
        std::cout << "Documentation saved to performance_documentation.md" << std::endl;
    }
    
    // Generate specific sections
    std::cout << std::endl << "Generating API documentation..." << std::endl;
    std::string api_doc = nlm::documentation::generate_api_documentation();
    std::cout << "API documentation generated (first 500 chars):" << std::endl;
    std::cout << api_doc.substr(0, 500) << "..." << std::endl;
    
    std::cout << std::endl << "Generating tutorial documentation..." << std::endl;
    std::string tutorial_doc = nlm::documentation::generate_tutorial_documentation();
    std::cout << "Tutorial documentation generated (first 500 chars):" << std::endl;
    std::cout << tutorial_doc.substr(0, 500) << "..." << std::endl;
}

// Command-line interface for documentation
void documentation_cli(int argc, char* argv[]) {
    std::cout << "NLM Documentation CLI" << std::endl;
    std::cout << "Usage: nlm_documentation [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --demo                    Run documentation demonstration" << std::endl;
    std::cout << "  --generate                Generate full documentation" << std::endl;
    std::cout << "  --api                     Generate API documentation" << std::endl;
    std::cout << "  --configuration           Generate configuration documentation" << std::endl;
    std::cout << "  --tutorial                Generate tutorial documentation" << std::endl;
    std::cout << "  --examples                Generate example documentation" << std::endl;
    std::cout << "  --best-practices          Generate best practice documentation" << std::endl;
    std::cout << "  --setup                  Generate setup documentation" << std::endl;
    std::cout << "  --troubleshooting         Generate troubleshooting documentation" << std::endl;
    
    // Simple argument parsing (in real implementation would use proper parsing)
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--demo") {
            demonstrate_documentation();
        } else if (arg == "--generate") {
            std::cout << nlm::documentation::generate_full_documentation();
        } else if (arg == "--api") {
            std::cout << nlm::documentation::generate_api_documentation();
        } else if (arg == "--configuration") {
            std::cout << nlm::documentation::generate_configuration_documentation();
        } else if (arg == "--tutorial") {
            std::cout << nlm::documentation::generate_tutorial_documentation();
        } else if (arg == "--examples") {
            std::cout << nlm::documentation::generate_example_documentation();
        } else if (arg == "--best-practices") {
            std::cout << nlm::documentation::generate_best_practice_documentation();
        } else if (arg == "--setup") {
            std::cout << nlm::documentation::generate_setup_documentation();
        } else if (arg == "--troubleshooting") {
            std::cout << nlm::documentation::generate_troubleshooting_documentation();
        }
    }
}