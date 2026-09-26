// Performance Profiling CLI Integration for NLM
// Extends AdvancedCLI to add comprehensive performance profiling and benchmarking commands

#include "PerformanceProfiler.hpp"
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

/**
 * Performance profiling CLI integration
 * Provides commands for real-time monitoring, benchmarking, and performance analysis
 */
class PerformanceProfilerCLI {
private:
    std::shared_ptr<Logger> logger;
    std::shared_ptr<SimulationClock> clock;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<PerformanceProfiler> profiler;
    
    // Command registration
    struct Command {
        std::string name;
        std::string description;
        std::function<void(const std::vector<std::string>&)> handler;
        std::string usage;
    };
    
    std::vector<Command> commands;
    std::string output_directory = "performance_data";
    
public:
    PerformanceProfilerCLI(
        std::shared_ptr<Logger> logger,
        std::shared_ptr<SimulationClock> clock,
        std::shared_ptr<Brain> brain
    ) : logger(logger), clock(clock), brain(brain) {
        profiler = std::make_shared<PerformanceProfiler>(logger, clock, brain, output_directory);
        registerCommands();
    }
    
    void registerCommands() {
        // Real-time Monitoring Commands
        commands.push_back({
            "profile-start",
            "Start real-time performance profiling",
            [this](const std::vector<std::string>& args) { handleProfileStart(args); },
            "profile-start [interval] [output_dir]"
        });
        
        commands.push_back({
            "profile-stop",
            "Stop real-time performance profiling",
            [this](const std::vector<std::string>& args) { handleProfileStop(args); },
            "profile-stop"
        });
        
        commands.push_back({
            "profile-status",
            "Get current profiling status and metrics",
            [this](const std::vector<std::string>& args) { handleProfileStatus(args); },
            "profile-status"
        });
        
        commands.push_back({
            "monitor-live",
            "Start live performance monitoring with real-time display",
            [this](const std::vector<std::string>& args) { handleLiveMonitor(args); },
            "monitor-live [refresh_interval]"
        });
        
        // Component Profiling Commands
        commands.push_back({
            "profile-neurons",
            "Profile neuron operations",
            [this](const std::vector<std::string>& args) { handleNeuronProfiling(args); },
            "profile-neurons [iterations]"
        });
        
        commands.push_back({
            "profile-synapses",
            "Profile synapse operations",
            [this](const std::vector<std::string>& args) { handleSynapseProfiling(args); },
            "profile-synapses [iterations]"
        });
        
        commands.push_back({
            "profile-plasticity",
            "Profile plasticity rule operations",
            [this](const std::vector<std::string>& args) { handlePlasticityProfiling(args); },
            "profile-plasticity [iterations]"
        });
        
        commands.push_back({
            "profile-memory",
            "Profile memory system operations",
            [this](const std::vector<std::string>& args) { handleMemoryProfiling(args); },
            "profile-memory [iterations]"
        });
        
        // Benchmarking Commands
        commands.push_back({
            "benchmark-network-size",
            "Benchmark neural network scalability with different sizes",
            [this](const std::vector<std::string>& args) { handleNetworkSizeBenchmark(args); },
            "benchmark-network-size [sizes]"
        });
        
        commands.push_back({
            "benchmark-simulation-speed",
            "Benchmark simulation speed with different configurations",
            [this](const std::vector<std::string>& args) { handleSimulationSpeedBenchmark(args); },
            "benchmark-simulation-speed [durations]"
        });
        
        commands.push_back({
            "benchmark-plasticity-rules",
            "Benchmark plasticity rule performance",
            [this](const std::vector<std::string>& args) { handlePlasticityRuleBenchmark(args); },
            "benchmark-plasticity-rules [rules]"
        });
        
        commands.push_back({
            "benchmark-memory-efficiency",
            "Benchmark memory efficiency",
            [this](const std::vector<std::string>& args) { handleMemoryEfficiencyBenchmark(args); },
            "benchmark-memory-efficiency [sizes]"
        });
        
        // Analysis and Reporting Commands
        commands.push_back({
            "report-generate",
            "Generate comprehensive performance report",
            [this](const std::vector<std::string>& args) { handleGenerateReport(args); },
            "report-generate [format] [output_file]"
        });
        
        commands.push_back({
            "report-compare",
            "Compare performance between runs",
            [this](const std::vector<std::string>& args) { handleCompareReports(args); },
            "report-compare [baseline] [test]"
        });
        
        commands.push_back({
            "report-analysis",
            "Analyze performance data and generate insights",
            [this](const std::vector<std::string>& args) { handleAnalysisReport(args); },
            "report-analysis [output_file]"
        });
        
        // Utility Commands
        commands.push_back({
            "profile-reset",
            "Reset profiler data and statistics",
            [this](const std::vector<std::string>& args) { handleReset(args); },
            "profile-reset"
        });
        
        commands.push_back({
            "profile-save",
            "Save current profiling data to files",
            [this](const std::vector<std::string>& args) { handleSaveData(args); },
            "profile-save [output_dir]"
        });
        
        commands.push_back({
            "profile-load",
            "Load profiling data from files",
            [this](const std::vector<std::string>& args) { handleLoadData(args); },
            "profile-load [input_dir]"
        });
        
        commands.push_back({
            "help-performance",
            "Show all performance profiling commands",
            [this](const std::vector<std::string>& args) { handleHelp(args); },
            "help-performance"
        });
    }
    
    void executeCommand(const std::string& command_name, const std::vector<std::string>& args) {
        for (const auto& cmd : commands) {
            if (cmd.name == command_name) {
                try {
                    cmd.handler(args);
                    logCommand("[PERFORMANCE] " + command_name);
                } catch (const std::exception& e) {
                    logger->log(ERROR, "Performance CLI command failed: " + std::string(e.what()));
                }
                return;
            }
        }
        
        logger->log(WARNING, "Unknown performance CLI command: " + command_name);
    }
    
    void printHelp() const {
        std::cout << "=== NLM Performance Profiling Commands ===" << std::endl << std::endl;
        
        std::cout << "Real-time Monitoring:" << std::endl;
        std::cout << "  profile-start     - Start real-time performance profiling" << std::endl;
        std::cout << "  profile-stop      - Stop real-time performance profiling" << std::endl;
        std::cout << "  profile-status    - Get current profiling status and metrics" << std::endl;
        std::cout << "  monitor-live      - Start live performance monitoring" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Component Profiling:" << std::endl;
        std::cout << "  profile-neurons   - Profile neuron operations" << std::endl;
        std::cout << "  profile-synapses  - Profile synapse operations" << std::endl;
        std::cout << "  profile-plasticity - Profile plasticity rule operations" << std::endl;
        std::cout << "  profile-memory    - Profile memory system operations" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Benchmarking:" << std::endl;
        std::cout << "  benchmark-network-size     - Benchmark neural network scalability" << std::endl;
        std::cout << "  benchmark-simulation-speed - Benchmark simulation speed" << std::endl;
        std::cout << "  benchmark-plasticity-rules - Benchmark plasticity rule performance" << std::endl;
        std::cout << "  benchmark-memory-efficiency - Benchmark memory efficiency" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Analysis and Reporting:" << std::endl;
        std::cout << "  report-generate  - Generate comprehensive performance report" << std::endl;
        std::cout << "  report-compare   - Compare performance between runs" << std::endl;
        std::cout << "  report-analysis  - Analyze performance data and generate insights" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Utility:" << std::endl;
        std::cout << "  profile-reset    - Reset profiler data and statistics" << std::endl;
        std::cout << "  profile-save     - Save current profiling data to files" << std::endl;
        std::cout << "  profile-load     - Load profiling data from files" << std::endl;
        std::cout << "  help-performance - Show all performance profiling commands" << std::endl;
    }
    
private:
    // Command handlers
    void handleProfileStart(const std::vector<std::string>& args) {
        double interval = args.size() > 0 ? std::stod(args[0]) : 0.1;
        std::string output_dir = args.size() > 1 ? args[1] : output_directory;
        
        output_directory = output_dir;
        profiler->configure(interval, true, true);
        profiler->startProfiling();
        
        std::cout << "[PERFORMANCE] Started profiling with interval " << interval << "s" << std::endl;
        std::cout << "[PERFORMANCE] Output directory: " << output_dir << std::endl;
    }
    
    void handleProfileStop(const std::vector<std::string>& args) {
        profiler->stopProfiling();
        std::cout << "[PERFORMANCE] Profiling stopped" << std::endl;
    }
    
    void handleProfileStatus(const std::vector<std::string>& args) {
        auto metrics = profiler->getCurrentMetrics();
        auto history = profiler->getProfilingHistory();
        
        std::cout << "[PERFORMANCE] Status Report:" << std::endl;
        std::cout << "  Profiling Enabled: " << (profiler->getBenchmarkResults().size() > 0 ? "Yes" : "No") << std::endl;
        std::cout << "  Total Metrics Recorded: " << history.size() << std::endl;
        std::cout << "  Active Benchmarks: " << profiler->getBenchmarkResults().size() << std::endl;
        std::cout << "  Current CPU Time: " << std::fixed << std::setprecision(3) << metrics.cpu_time << "s" << std::endl;
        std::cout << "  Current Memory Usage: " << metrics.memory_used << " bytes" << std::endl;
        std::cout << "  Current CPU Utilization: " << std::fixed << std::setprecision(1) << metrics.cpu_utilization << "%" << std::endl;
        
        if (!history.empty()) {
            std::cout << "  Last 5 Metrics Samples:" << std::endl;
            size_t start_idx = history.size() > 5 ? history.size() - 5 : 0;
            for (size_t i = start_idx; i < history.size(); ++i) {
                const auto& m = history[i];
                std::cout << "    " << i << ": Time=" << std::fixed << std::setprecision(3) << m.wall_time
                          << "s, Memory=" << m.memory_used << "b, CPU=" << std::fixed << std::setprecision(1) << m.cpu_utilization << "%" << std::endl;
            }
        }
    }
    
    void handleLiveMonitor(const std::vector<std::string>& args) {
        double refresh_interval = args.size() > 0 ? std::stod(args[0]) : 1.0;
        
        std::cout << "[PERFORMANCE] Starting live monitoring (refresh: " << refresh_interval << "s)" << std::endl;
        std::cout << "Press Ctrl+C to stop monitoring" << std::endl << std::endl;
        
        profiler->startProfiling();
        
        try {
            while (true) {
                auto metrics = profiler->getCurrentMetrics();
                auto history = profiler->getProfilingHistory();
                
                // Clear screen (simplified)
                std::cout << "\033[2J\033[1;1H";
                
                std::cout << "=== NLM Real-time Performance Monitor ===" << std::endl;
                std::cout << "Time: " << std::fixed << std::setprecision(1) << metrics.timestamp << std::endl;
                std::cout << "CPU Utilization: " << std::fixed << std::setprecision(1) << metrics.cpu_utilization << "%" << std::endl;
                std::cout << "Memory Usage: " << metrics.memory_used << " bytes (" 
                          << std::fixed << std::setprecision(1) << (metrics.memory_utilization) << "%)" << std::endl;
                std::cout << "Wall Time: " << std::fixed << std::setprecision(3) << metrics.wall_time << "s" << std::endl;
                std::cout << "CPU Time: " << std::fixed << std::setprecision(3) << metrics.cpu_time << "s" << std::endl;
                std::cout << std::endl;
                
                std::cout << "System Stats:" << std::endl;
                std::cout << "  Neurons: " << (brain ? brain->getTotalNeuronCount() : 0) << std::endl;
                std::cout << "  Synapses: " << (brain ? brain->getTotalSynapseCount() : 0) << std::endl;
                std::cout << "  Spikes: " << (brain ? brain->getTotalSpikeCount() : 0) << std::endl;
                std::cout << std::endl;
                
                std::cout << "Active Profiles:" << std::endl;
                for (const auto& pair : profiler->getBenchmarkResults()) {
                    std::cout << "  " << pair.benchmark_name << ": " << std::fixed << std::setprecision(3) << pair.total_time << "s" << std::endl;
                }
                
                std::cout << std::endl;
                std::cout << "Metrics History (last 5):" << std::endl;
                size_t start_idx = history.size() > 5 ? history.size() - 5 : 0;
                for (size_t i = start_idx; i < history.size(); ++i) {
                    const auto& m = history[i];
                    std::cout << "  " << i << ": " << std::fixed << std::setprecision(1) << m.timestamp 
                              << ", CPU=" << std::fixed << std::setprecision(1) << m.cpu_utilization << "%, "
                              << "Memory=" << m.memory_used << "b" << std::endl;
                }
                
                std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(refresh_interval)));
            }
        } catch (const std::exception& e) {
            std::cout << "\n[PERFORMANCE] Live monitoring stopped: " << e.what() << std::endl;
        }
        
        profiler->stopProfiling();
    }
    
    void handleNeuronProfiling(const std::vector<std::string>& args) {
        int iterations = args.size() > 0 ? std::stoi(args[0]) : 1000;
        
        std::cout << "[PERFORMANCE] Profiling neuron operations for " << iterations << " iterations..." << std::endl;
        profiler->startComponentProfile("neuron_operations", "neuron");
        
        for (int i = 0; i < iterations; ++i) {
            profiler->profileNeuronOperations();
        }
        
        profiler->stopProfiling();
        std::cout << "[PERFORMANCE] Neuron profiling completed" << std::endl;
    }
    
    void handleSynapseProfiling(const std::vector<std::string>& args) {
        int iterations = args.size() > 0 ? std::stoi(args[0]) : 1000;
        
        std::cout << "[PERFORMANCE] Profiling synapse operations for " << iterations << " iterations..." << std::endl;
        profiler->startComponentProfile("synapse_operations", "synapse");
        
        for (int i = 0; i < iterations; ++i) {
            profiler->profileSynapseOperations();
        }
        
        profiler->stopProfiling();
        std::cout << "[PERFORMANCE] Synapse profiling completed" << std::endl;
    }
    
    void handlePlasticityProfiling(const std::vector<std::string>& args) {
        int iterations = args.size() > 0 ? std::stoi(args[0]) : 1000;
        
        std::cout << "[PERFORMANCE] Profiling plasticity operations for " << iterations << " iterations..." << std::endl;
        profiler->startComponentProfile("plasticity_operations", "plasticity");
        
        for (int i = 0; i < iterations; ++i) {
            profiler->profilePlasticityOperations();
        }
        
        profiler->stopProfiling();
        std::cout << "[PERFORMANCE] Plasticity profiling completed" << std::endl;
    }
    
    void handleMemoryProfiling(const std::vector<std::string>& args) {
        int iterations = args.size() > 0 ? std::stoi(args[0]) : 1000;
        
        std::cout << "[PERFORMANCE] Profiling memory systems for " << iterations << " iterations..." << std::endl;
        profiler->startComponentProfile("memory_systems", "memory");
        
        for (int i = 0; i < iterations; ++i) {
            profiler->profileMemorySystems();
        }
        
        profiler->stopProfiling();
        std::cout << "[PERFORMANCE] Memory profiling completed" << std::endl;
    }
    
    void handleNetworkSizeBenchmark(const std::vector<std::string>& args) {
        std::vector<size_t> neuron_counts;
        if (args.size() > 0) {
            std::stringstream ss(args[0]);
            size_t size;
            while (ss >> size) {
                neuron_counts.push_back(size);
                if (ss.peek() == ',') ss.ignore();
            }
        }
        
        if (neuron_counts.empty()) {
            neuron_counts = {100, 500, 1000, 5000, 10000};
        }
        
        std::cout << "[PERFORMANCE] Running neural network size benchmark for sizes: ";
        for (size_t size : neuron_counts) std::cout << size << " ";
        std::cout << std::endl;
        
        profiler->runNeuralNetworkSizeBenchmark(neuron_counts);
        
        std::cout << "[PERFORMANCE] Network size benchmark completed" << std::endl;
    }
    
    void handleSimulationSpeedBenchmark(const std::vector<std::string>& args) {
        std::vector<double> durations;
        if (args.size() > 0) {
            std::stringstream ss(args[0]);
            double duration;
            while (ss >> duration) {
                durations.push_back(duration);
                if (ss.peek() == ',') ss.ignore();
            }
        }
        
        if (durations.empty()) {
            durations = {1.0, 5.0, 10.0, 30.0};
        }
        
        std::cout << "[PERFORMANCE] Running simulation speed benchmark for durations: ";
        for (double dur : durations) std::cout << dur << "s ";
        std::cout << std::endl;
        
        profiler->runSimulationSpeedBenchmark(durations);
        
        std::cout << "[PERFORMANCE] Simulation speed benchmark completed" << std::endl;
    }
    
    void handlePlasticityRuleBenchmark(const std::vector<std::string>& args) {
        std::vector<std::string> rule_names;
        if (args.size() > 0) {
            std::stringstream ss(args[0]);
            std::string rule;
            while (ss >> rule) {
                rule_names.push_back(rule);
                if (ss.peek() == ',') ss.ignore();
            }
        }
        
        if (rule_names.empty()) {
            rule_names = {"STDP", "BCM", "Oja", "Hebbian", "RewardModulated"};
        }
        
        std::cout << "[PERFORMANCE] Running plasticity rule benchmark for rules: ";
        for (const auto& rule : rule_names) std::cout << rule << " ";
        std::cout << std::endl;
        
        profiler->runPlasticityRuleBenchmark(rule_names);
        
        std::cout << "[PERFORMANCE] Plasticity rule benchmark completed" << std::endl;
    }
    
    void handleMemoryEfficiencyBenchmark(const std::vector<std::string>& args) {
        std::vector<size_t> memory_sizes;
        if (args.size() > 0) {
            std::stringstream ss(args[0]);
            size_t size;
            while (ss >> size) {
                memory_sizes.push_back(size);
                if (ss.peek() == ',') ss.ignore();
            }
        }
        
        if (memory_sizes.empty()) {
            memory_sizes = {1024, 4096, 16384, 65536, 262144};
        }
        
        std::cout << "[PERFORMANCE] Running memory efficiency benchmark for sizes: ";
        for (size_t size : memory_sizes) std::cout << size << "b ";
        std::cout << std::endl;
        
        profiler->runMemoryEfficiencyBenchmark(memory_sizes);
        
        std::cout << "[PERFORMANCE] Memory efficiency benchmark completed" << std::endl;
    }
    
    void handleGenerateReport(const std::vector<std::string>& args) {
        std::string format = args.size() > 0 ? args[0] : "html";
        std::string output_file = args.size() > 1 ? args[1] : "performance_report.html";
        
        std::cout << "[PERFORMANCE] Generating performance report in " << format << " format..." << std::endl;
        
        if (format == "html") {
            profiler->generatePerformanceReport(output_file);
            profiler->generateBenchmarkReport(output_file.replace(output_file.find('.'), 4, "_benchmark"));
            profiler->generateDetailedReport(output_file.replace(output_file.find('.'), 4, "_detailed"));
        } else if (format == "json") {
            // Generate JSON reports
            std::string json_file = output_file.substr(0, output_file.find_last_of('.')) + ".json";
            std::ofstream file(json_file);
            if (file.is_open()) {
                nlohmann::json j = {
                    {"benchmarks", profiler->getBenchmarkResults()},
                    {"metrics", profiler->getProfilingHistory()}
                };
                file << j.dump(2);
                file.close();
            }
        }
        
        std::cout << "[PERFORMANCE] Performance reports generated: " << output_file << std::endl;
    }
    
    void handleCompareReports(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "[PERFORMANCE] Usage: report-compare <baseline> <test>" << std::endl;
            return;
        }
        
        std::string baseline = args[0];
        std::string test = args[1];
        
        std::cout << "[PERFORMANCE] Comparing performance between " << baseline << " and " << test << std::endl;
        profiler->compareBenchmarks(baseline, test);
        
        std::cout << "[PERFORMANCE] Comparison completed" << std::endl;
    }
    
    void handleAnalysisReport(const std::vector<std::string>& args) {
        std::string output_file = args.size() > 0 ? args[0] : "performance_analysis.html";
        
        std::cout << "[PERFORMANCE] Analyzing performance data..." << std::endl;
        
        // Generate analysis report
        std::ofstream file(output_file);
        if (file.is_open()) {
            file << "<!DOCTYPE html>\n<html>\n<head>\n<title>NLM Performance Analysis Report</title>\n<style>\n"
                 << "body { font-family: Arial, sans-serif; margin: 20px; }\n"
                 << "h1 { color: #333; }\n"
                 << "h2 { color: #555; }\n"
                 << "table { border-collapse: collapse; width: 100%; margin: 20px 0; }\n"
                 << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
                 << "th { background-color: #f2f2f2; }\n"
                 << "tr:nth-child(even) { background-color: #f9f9f9; }\n"
                 << ".insight { background-color: #e8f5e8; padding: 10px; margin: 10px 0; border-radius: 5px; }\n"
                 << "</style>\n</head>\n<body>\n"
                 << "<h1>NLM Performance Analysis Report</h1>\n"
                 << "<p>Generated: " << std::chrono::system_clock::now() << "</p>\n";
            
            // Add performance insights
            file << "<h2>Performance Insights</h2>\n";
            
            auto results = profiler->getBenchmarkResults();
            if (!results.empty()) {
                file << "<div class='insight'>";
                file << "<h3>Benchmark Analysis</h3>";
                file << "<p>Total benchmarks executed: " << results.size() << "</p>";
                file << "<p>Average benchmark time: ";
                double total_time = std::accumulate(results.begin(), results.end(), 0.0,
                    [](double sum, const BenchmarkResult& r) { return sum + r.total_time; });
                double avg_time = total_time / results.size();
                file << std::fixed << std::setprecision(3) << avg_time << "s</p>";
                file << "</div>\n";
            }
            
            auto metrics = profiler->getProfilingHistory();
            if (!metrics.empty()) {
                file << "<div class='insight'>";
                file << "<h3>System Performance Analysis</h3>";
                file << "<p>Total metrics samples: " << metrics.size() << "</p>";
                file << "<p>Average CPU utilization: ";
                double avg_cpu = std::accumulate(metrics.begin(), metrics.end(), 0.0,
                    [](double sum, const ProfileMetrics& m) { return sum + m.cpu_utilization; }) / metrics.size();
                file << std::fixed << std::setprecision(1) << avg_cpu << "%</p>";
                file << "<p>Peak memory usage: " << metrics.back().memory_used << " bytes</p>";
                file << "</div>\n";
            }
            
            file << "</body>\n</html>";
            file.close();
        }
        
        std::cout << "[PERFORMANCE] Performance analysis report generated: " << output_file << std::endl;
    }
    
    void handleReset(const std::vector<std::string>& args) {
        std::cout << "[PERFORMANCE] Resetting profiler data..." << std::endl;
        profiler->reset();
        std::cout << "[PERFORMANCE] Profiler data reset" << std::endl;
    }
    
    void handleSaveData(const std::vector<std::string>& args) {
        std::string output_dir = args.size() > 0 ? args[0] : output_directory;
        output_directory = output_dir;
        
        std::cout << "[PERFORMANCE] Saving profiling data to: " << output_dir << std::endl;
        profiler->saveDataToFiles();
        std::cout << "[PERFORMANCE] Data saved successfully" << std::endl;
    }
    
    void handleLoadData(const std::vector<std::string>& args) {
        std::string input_dir = args.size() > 0 ? args[0] : output_directory;
        output_directory = input_dir;
        
        std::cout << "[PERFORMANCE] Loading profiling data from: " << input_dir << std::endl;
        profiler->loadDataFromFiles();
        std::cout << "[PERFORMANCE] Data loaded successfully" << std::endl;
    }
    
    void handleHelp(const std::vector<std::string>& args) {
        printHelp();
    }
    
    void logCommand(const std::string& command) {
        logger->log(DEBUG, "[PERFORMANCE CLI] " + command);
    }
};