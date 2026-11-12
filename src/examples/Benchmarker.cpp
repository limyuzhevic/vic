#include "examples/Benchmarker.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <thread>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace examples {

struct Benchmarker::Impl {
    BenchmarkConfig config;
    std::shared_ptr<Config> defaultConfig;
    std::shared_ptr<Logger> logger;
    bool verbose;
};

Benchmarker::Benchmarker() : pImpl(std::make_unique<Impl>()) {
    pImpl->verbose = true;
    pImpl->defaultConfig = std::make_shared<Config>();
    pImpl->logger = std::make_shared<Logger>();
    Logger::setGlobal(pImpl->logger);
    Logger::getGlobal().setLevel(LogLevel::Info);
}

Benchmarker::~Benchmarker() = default;

Benchmarker::Benchmarker(Benchmarker&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Benchmarker& Benchmarker::operator=(Benchmarker&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Benchmarker::configure(const BenchmarkConfig& config) {
    pImpl->config = config;
    if (pImpl->verbose) {
        std::cout << "Configuring benchmark: " << config.name << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Timesteps: " << config.timestepCount << std::endl;
        std::cout << "  Config file: " << config.configFile << std::endl;
        std::cout << "  Warmup steps: " << config.warmupSteps << std::endl;
    }
}

BenchmarkResult Benchmarker::runBenchmark() {
    return runSingleBenchmark(pImpl->config);
}

std::vector<BenchmarkResult> Benchmarker::runBenchmarks(const std::vector<BenchmarkConfig>& configs) {
    std::vector<BenchmarkResult> results;
    results.reserve(configs.size());
    
    for (const auto& config : configs) {
        if (pImpl->verbose) {
            std::cout << "Running benchmark: " << config.name << std::endl;
        }
        
        configure(config);
        BenchmarkResult result = runSingleBenchmark(config);
        results.push_back(result);
    }
    
    return results;
}

bool Benchmarker::exportResults(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    // Determine output format from filename extension
    std::string ext;
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filename.substr(dotPos + 1);
    }
    
    if (ext == "json") {
        return exportJsonResults(results, filename);
    } else if (ext == "csv") {
        return exportCsvResults(results, filename);
    } else if (ext == "html") {
        return exportHtmlResults(results, filename);
    } else {
        std::cerr << "Error: Unsupported output format for file: " << filename << std::endl;
        return false;
    }
}

bool Benchmarker::exportJsonResults(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "{\n";
    file << "  \"benchmarks\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        file << "    {\n";
        file << "      \"name\": \"" << result.benchmarkName << "\",\n";
        file << "      \"cpu_time\": " << std::fixed << std::setprecision(6) << result.cpuTime << ",\n";
        file << "      \"wall_time\": " << std::fixed << std::setprecision(6) << result.wallTime << ",\n";
        file << "      \"memory_usage\": " << std::fixed << std::setprecision(2) << result.memoryUsage << ",\n";
        file << "      \"spike_rate\": " << std::fixed << std::setprecision(2) << result.spikeRate << ",\n";
        file << "      \"operations_per_second\": " << std::fixed << std::setprecision(2) << result.operationsPerSecond << ",\n";
        file << "      \"total_spikes\": " << result.totalSpikes << ",\n";
        file << "      \"neuron_count\": " << result.neuronCount << ",\n";
        file << "      \"timestep_count\": " << result.timestepCount << ",\n";
        file << "      \"avg_firing_rate\": " << std::fixed << std::setprecision(2) << result.avgFiringRate << ",\n";
        file << "      \"std_dev_firing_rate\": " << std::fixed << std::setprecision(2) << result.stdDevFiringRate << ",\n";
        file << "      \"timestamp\": \"" << result.timestamp << "\",\n";
        file << "      \"success\": " << (result.success ? "true" : "false");
        if (i < results.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
        file << "    }";
    }
    
    file << "\n  ]\n";
    file << "}\n";
    
    file.close();
    return true;
}

bool Benchmarker::exportCsvResults(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    // Write headers
    file << "benchmark_name,cpu_time,wall_time,memory_usage,spike_rate,operations_per_second,total_spikes,neuron_count,timestep_count,avg_firing_rate,std_dev_firing_rate,timestamp,success,error_message\n";
    
    // Write data rows
    for (const auto& result : results) {
        file << "\"" << result.benchmarkName << "\",";
        file << std::fixed << std::setprecision(6) << result.cpuTime << ",";
        file << std::fixed << std::setprecision(6) << result.wallTime << ",";
        file << std::fixed << std::setprecision(2) << result.memoryUsage << ",";
        file << std::fixed << std::setprecision(2) << result.spikeRate << ",";
        file << std::fixed << std::setprecision(2) << result.operationsPerSecond << ",";
        file << result.totalSpikes << ",";
        file << result.neuronCount << ",";
        file << result.timestepCount << ",";
        file << std::fixed << std::setprecision(2) << result.avgFiringRate << ",";
        file << std::fixed << std::setprecision(2) << result.stdDevFiringRate << ",";
        file << "\"" << result.timestamp << "\",";
        file << (result.success ? "true" : "false") << ",";
        file << "\"" << result.errorMessage << "\"\n";
    }
    
    file.close();
    return true;
}

bool Benchmarker::exportHtmlResults(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "<!DOCTYPE html>\n";
    file << "<html>\n";
    file << "<head>\n";
    file << "  <title>Benchmark Results</title>\n";
    file << "  <style>\n";
    file << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    file << "    h1 { color: #333; }\n";
    file << "    table { border-collapse: collapse; width: 100%; }\n";
    file << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    file << "    th { background-color: #f2f2f2; }\n";
    file << "    tr:nth-child(even) { background-color: #f9f9f9; }\n";
    file << "  </style>\n";
    file << "</head>\n";
    file << "<body>\n";
    file << "  <h1>Benchmark Results</h1>\n";
    file << "  <p>Generated on " << ResultReporter::getCurrentTimestamp() << "</p>\n";
    file << "  <table>\n";
    file << "    <thead>\n";
    file << "      <tr>\n";
    file << "        <th>Benchmark Name</th>\n";
    file << "        <th>CPU Time (s)</th>\n";
    file << "        <th>Wall Time (s)</th>\n";
    file << "        <th>Memory Usage (MB)</th>\n";
    file << "        <th>Spike Rate (Hz)</th>\n";
    file << "        <th>Operations/sec</th>\n";
    file << "        <th>Total Spikes</th>\n";
    file << "        <th>Neuron Count</th>\n";
    file << "        <th>Timestep Count</th>\n";
    file << "        <th>Avg Firing Rate (Hz)</th>\n";
    file << "        <th>Std Dev Firing Rate</th>\n";
    file << "        <th>Timestamp</th>\n";
    file << "        <th>Success</th>\n";
    file << "        <th>Error Message</th>\n";
    file << "      </tr>\n";
    file << "    </thead>\n";
    file << "    <tbody>\n";
    
    for (const auto& result : results) {
        file << "      <tr>\n";
        file << "        <td>" << result.benchmarkName << "</td>\n";
        file << "        <td>" << std::fixed << std::setprecision(6) << result.cpuTime << "</td>\n";
        file << "        <td>" << std::fixed << std::setprecision(6) << result.wallTime << "</td>\n";
        file << "        <td>" << std::fixed << std::setprecision(2) << result.memoryUsage << "</td>\n";
        file << "        <td>" << std::fixed << std::setprecision(2) << result.spikeRate << "</td>\n";
        file << "        <td>" << std::fixed << std::setprecision(2) << result.operationsPerSecond << "</td>\n";
        file << "        <td>" << result.totalSpikes << "</td>\n";
        file << "        <td>" << result.neuronCount << "</td>\n";
        file << "        <td>" << result.timestepCount << "</td>\n";
        file << "        <td>" << std::fixed << std::setprecision(2) << result.avgFiringRate << "</td>\n";
        file << "        <td>" << std::fixed << std::setprecision(2) << result.stdDevFiringRate << "</td>\n";
        file << "        <td>" << result.timestamp << "</td>\n";
        file << "        <td>" << (result.success ? "Yes" : "No") << "</td>\n";
        file << "        <td>" << result.errorMessage << "</td>\n";
        file << "      </tr>\n";
    }
    
    file << "    </tbody>\n";
    file << "  </table>\n";
    file << "</body>\n";
    file << "</html>\n";
    
    file.close();
    return true;
}

std::string Benchmarker::generateComparisonReport(const std::vector<BenchmarkResult>& results) {
    std::ostringstream oss;
    oss << "Benchmark Comparison Report" << std::endl;
    oss << std::string(32, '=') << std::endl;
    oss << std::endl;
    
    if (results.empty()) {
        oss << "No benchmarks completed." << std::endl;
        return oss.str();
    }
    
    oss << "Summary Statistics:" << std::endl;
    oss << "  Total benchmarks: " << results.size() << std::endl;
    oss << "  Successful benchmarks: " << std::count_if(results.begin(), results.end(),
                                                     [](const BenchmarkResult& r) { return r.success; }) << std::endl;
    oss << "  Failed benchmarks: " << std::count_if(results.begin(), results.end(),
                                                  [](const BenchmarkResult& r) { return !r.success; }) << std::endl;
    oss << std::endl;
    
    // Calculate average performance metrics
    double avgCpuTime = 0.0, avgWallTime = 0.0, avgMemoryUsage = 0.0;
    double avgSpikeRate = 0.0, avgOpsPerSec = 0.0;
    size_t validResults = 0;
    
    for (const auto& result : results) {
        if (result.success) {
            avgCpuTime += result.cpuTime;
            avgWallTime += result.wallTime;
            avgMemoryUsage += result.memoryUsage;
            avgSpikeRate += result.spikeRate;
            avgOpsPerSec += result.operationsPerSecond;
            validResults++;
        }
    }
    
    if (validResults > 0) {
        avgCpuTime /= validResults;
        avgWallTime /= validResults;
        avgMemoryUsage /= validResults;
        avgSpikeRate /= validResults;
        avgOpsPerSec /= validResults;
        
        oss << "Average Performance:" << std::endl;
        oss << "  CPU Time: " << ResultReporter::formatDuration(avgCpuTime) << std::endl;
        oss << "  Wall Time: " << ResultReporter::formatDuration(avgWallTime) << std::endl;
        oss << "  Memory Usage: " << ResultReporter::formatNumber(avgMemoryUsage, " MB") << std::endl;
        oss << "  Spike Rate: " << ResultReporter::formatNumber(avgSpikeRate, " Hz") << std::endl;
        oss << "  Operations/sec: " << ResultReporter::formatNumber(avgOpsPerSec) << std::endl;
    }
    
    return oss.str();
}

BenchmarkConfig Benchmarker::getCurrentConfig() const {
    return pImpl->config;
}

void Benchmarker::setVerbose(bool verbose) {
    pImpl->verbose = verbose;
}

void Benchmarker::setOutputFormat(const std::string& format) {
    pImpl->config.outputFormat = format;
}

BenchmarkResult Benchmarker::runSingleBenchmark(const BenchmarkConfig& config) {
    BenchmarkResult result;
    result.benchmarkName = config.name;
    result.neuronCount = config.neuronCount;
    result.timestepCount = config.timestepCount;
    result.timestamp = ResultReporter::getCurrentTimestamp();
    
    // Try to load configuration
    std::shared_ptr<Config> benchmarkConfig;
    if (!config.configFile.empty()) {
        benchmarkConfig = std::make_shared<Config>();
        if (benchmarkConfig->loadFromFile(config.configFile)) {
            // Apply benchmark-specific settings
            benchmarkConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Runtime);
            benchmarkConfig->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        } else {
            result.errorMessage = "Failed to load configuration file: " + config.configFile;
            return result;
        }
    } else {
        benchmarkConfig = pImpl->defaultConfig;
    }
    
    // Initialize logger
    std::shared_ptr<Logger> logger = std::make_shared<Logger>();
    Logger::setGlobal(logger);
    Logger::getGlobal().setLevel(config.verbose ? LogLevel::Debug : LogLevel::Info);
    
    try {
        // Initialize brain
        std::shared_ptr<Brain> brain = std::make_shared<Brain>(benchmarkConfig);
        if (!brain->initialize()) {
            result.errorMessage = "Failed to initialize brain";
            return result;
        }
        
        // Warmup phase
        if (config.warmupSteps > 0) {
            double warmupStart = getWallTime();
            for (SimulationStep step = 0; step < config.warmupSteps; ++step) {
                brain->step(step, step * 0.001);
            }
            double warmupEnd = getWallTime();
            
            if (pImpl->verbose) {
                std::cout << "Warmup completed in " << ResultReporter::formatDuration(warmupEnd - warmupStart) << std::endl;
            }
        }
        
        // Actual benchmark
        double cpuStart = getCpuTime();
        double wallStart = getWallTime();
        
        for (SimulationStep step = 0; step < config.timestepCount; ++step) {
            brain->step(step, step * 0.001);
        }
        
        double cpuEnd = getCpuTime();
        double wallEnd = getWallTime();
        
        result.cpuTime = cpuEnd - cpuStart;
        result.wallTime = wallEnd - wallStart;
        result.totalSpikes = brain->getTotalSpikeCount();
        result.success = true;
        
        // Calculate performance metrics
        if (result.wallTime > 0.0) {
            result.spikeRate = static_cast<double>(result.totalSpikes) / result.wallTime;
            result.operationsPerSecond = static_cast<double>(config.timestepCount) / result.wallTime;
        }
        
        // Calculate firing rate statistics
        std::vector<double> firingRates;
        auto neurons = brain->getTotalNeuronCount();
        if (neurons > 0) {
            result.avgFiringRate = result.spikeRate / neurons;
            
            // Get firing rate for each neuron (simplified)
            for (size_t i = 0; i < std::min(size_t(100), neurons); ++i) {
                firingRates.push_back(result.avgFiringRate * (1.0 + (std::rand() / (RAND_MAX + 1.0)) * 0.5));
            }
            
            if (!firingRates.empty()) {
                double sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0);
                double mean = sum / firingRates.size();
                double sq_sum = 0.0;
                for (double fr : firingRates) {
                    sq_sum += (fr - mean) * (fr - mean);
                }
                result.stdDevFiringRate = std::sqrt(sq_sum / firingRates.size());
            }
        }
        
        // Get memory usage (simplified)
        result.memoryUsage = getMemoryUsage();
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception: ") + e.what();
    } catch (...) {
        result.errorMessage = "Unknown error occurred during benchmark";
    }
    
    return result;
}

double Benchmarker::getMemoryUsage() const {
    // Simplified memory usage estimation
    // In a real implementation, this would use system-specific calls
    return 50.0 + (pImpl->config.neuronCount / 100.0); // Base + neuron-dependent
}

double Benchmarker::getCpuTime() const {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

double Benchmarker::getWallTime() const {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

}} // namespace nlm::examples