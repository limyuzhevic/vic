// Infrastructure benchmarking implementations

#include "InfrastructureBenchmarks.hpp"
#include "../benchmarks/Benchmark.hpp"

namespace nlm {
namespace benchmark {
namespace infrastructure {

// Flamegraph utilities implementation

namespace flamegraph {
    FlameGraphNode buildFlameGraph(const std::vector<ProfileEvent>& events) {
        FlameGraphNode root;
        root.name = "root";
        root.selfTime = 0.0;
        root.totalTime = 0.0;
        root.percentage = 0.0;
        
        double totalTime = 0.0;
        for (const auto& event : events) {
            totalTime += std::chrono::duration<double>(event.endTime - event.startTime).count();
        }
        
        // Build hierarchy
        for (const auto& event : events) {
            double eventTime = std::chrono::duration<double>(event.endTime - event.startTime).count();
            
            FlameGraphNode* current = &root;
            
            // Parse function name to create hierarchy
            size_t pos = 0;
            std::string remaining = event.functionName;
            std::string currentPath = "";
            
            while (!remaining.empty()) {
                size_t separator = remaining.find_first_of(" .:");
                std::string segment;
                
                if (separator == std::string::npos) {
                    segment = remaining;
                    remaining.clear();
                } else {
                    segment = remaining.substr(0, separator);
                    remaining = remaining.substr(separator + 1);
                }
                
                if (currentPath.empty()) {
                    currentPath = segment;
                } else {
                    currentPath += "." + segment;
                }
                
                // Find or create child node
                auto it = std::find_if(current->children.begin(), current->children.end(),
                    [&segment](const FlameGraphNode& node) { return node.name == segment; });
                
                if (it == current->children.end()) {
                    current->children.push_back(FlameGraphNode{segment, 0.0, 0.0, 0.0, std::vector<FlameGraphNode>(), event.threadId, "", 0});
                    it = std::prev(current->children.end());
                }
                
                current = &(*it);
                current->totalTime += eventTime;
            }
        }
        
        // Calculate percentages and self times
        auto calculateStats = [&totalTime](FlameGraphNode& node) {
            node.percentage = totalTime > 0 ? (node.totalTime / totalTime) * 100.0 : 0.0;
            
            for (auto& child : node.children) {
                calculateStats(child);
                node.selfTime += child.totalTime;
            }
            
            node.selfTime = node.totalTime - node.selfTime;
        };
        
        calculateStats(root);
        
        return root;
    }
    
    void exportFlameGraph(const FlameGraphNode& root, const std::string& outputPath) {
        // This would normally write the flame graph data to a file
        // For now, just log that we're exporting
        std::cout << "Exporting flame graph to: " << outputPath << std::endl;
    }
    
    void generateSVG(const FlameGraphNode& root, const std::string& outputPath) {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << outputPath << std::endl;
            return;
        }
        
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        file << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100%\" height=\"100%\">\n";
        file << "  <style>\n";
        file << "    .node { stroke: #ccc; stroke-width: 0.5; fill: #fff; }\n";
        file << "    .node:hover { stroke: #f00; stroke-width: 2; }\n";
        file << "    .label { font-family: monospace; font-size: 12px; text-anchor: middle; }\n";
        file << "  </style>\n";
        
        // Generate SVG content for flame graph (simplified)
        file << "  <g transform=\"translate(0, 0)\">\n";
        // TODO: Implement actual flame graph visualization
        file << "  </g>\n";
        file << "</svg>\n";
        
        file.close();
        std::cout << "Generated SVG flame graph: " << outputPath << std::endl;
    }
    
    std::string generateHTMLFlameGraph(const FlameGraphNode& root) {
        std::stringstream html;
        html << "<!DOCTYPE html>\n";
        html << "<html>\n";
        html << "<head>\n";
        html << "  <title>Flame Graph</title>\n";
        html << "  <style>\n";
        html << "    body { font-family: sans-serif; margin: 20px; }\n";
        html << "    .node { stroke: #ccc; stroke-width: 0.5; fill: #fff; cursor: pointer; }\n";
        html << "    .node:hover { stroke: #f00; stroke-width: 2; }\n";
        html << "    .label { font-family: monospace; font-size: 12px; text-anchor: middle; }\n";
        html << "  </style>\n";
        html << "</head>\n";
        html << "<body>\n";
        html << "  <h1>Flame Graph Profile</h1>\n";
        html << "  <div id=\"flamegraph\"></div>\n";
        html << "  <script src=\"https://d3js.org/d3.v6.min.js\"></script>\n";
        html << "  <script>\n";
        html << "    // TODO: Implement flame graph visualization with D3.js\n";
        html << "    // This would normally load the flame graph data and render it\n";
        html << "  </script>\n";
        html << "</body>\n";
        html << "</html>\n";
        
        return html.str();
    }
}

// MemoryUsageTracker implementation

void MemoryUsageTracker::startTracking() {
    tracking = true;
    samples.clear();
    
    trackingThread = std::thread([this]() {
        while (tracking) {
            recordSample();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void MemoryUsageTracker::stopTracking() {
    tracking = false;
    if (trackingThread.joinable()) {
        trackingThread.join();
    }
}

void MemoryUsageTracker::recordSample() {
    std::lock_guard<std::mutex> lock(mutex);
    MemorySample sample;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    
    // TODO: Get actual system memory metrics
    sample.residentSet = 0;
    sample.virtualMemory = 0;
    sample.sharedMemory = 0;
    sample.textSegment = 0;
    sample.dataSegment = 0;
    sample.stack = 0;
    sample.cpuUtilization = 0.0;
    
    samples.push_back(sample);
}

std::vector<MemoryUsageTracker::MemorySample> MemoryUsageTracker::getMemoryProfile() const {
    return samples;
}

double MemoryUsageTracker::calculatePeakMemoryUsage() const {
    double peak = 0.0;
    for (const auto& sample : samples) {
        peak = std::max(peak, static_cast<double>(sample.residentSet));
    }
    return peak;
}

size_t MemoryUsageTracker::getCurrentMemoryUsage() const {
    if (samples.empty()) return 0;
    return samples.back().residentSet;
}

double MemoryUsageTracker::analyzeMemoryLeak(const std::vector<MemorySample>& samples) const {
    if (samples.size() < 10) return 0.0;
    
    // Simple leak detection: check if memory usage grows over time
    size_t firstHalf = samples.size() / 2;
    size_t secondHalf = samples.size() - firstHalf;
    
    double firstMean = 0.0, secondMean = 0.0;
    for (size_t i = 0; i < firstHalf; ++i) {
        firstMean += samples[i].residentSet;
    }
    firstMean /= firstHalf;
    
    for (size_t i = firstHalf; i < samples.size(); ++i) {
        secondMean += samples[i].residentSet;
    }
    secondMean /= secondHalf;
    
    return (secondMean - firstMean) / firstMean;
}

void MemoryUsageTracker::generateMemoryReport(const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << outputPath << std::endl;
        return;
    }
    
    file << "Memory Usage Report\n";
    file << "===================\n\n";
    file << "Total Samples: " << samples.size() << "\n";
    file << "Peak Memory Usage: " << calculatePeakMemoryUsage() << " bytes\n";
    file << "Current Memory Usage: " << getCurrentMemoryUsage() << " bytes\n";
    double leakRate = analyzeMemoryLeak(samples);
    file << "Memory Leak Rate: " << (leakRate * 100) << "%\n\n";
    
    file << "| Timestamp | RSS (bytes) | Virtual (bytes) | CPU (%) |\n";
    file << "|-----------|-------------|------------------|---------|\n";
    
    for (const auto& sample : samples) {
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(sample.timestamp.time_since_epoch());
        file << "| " << time.count() << " | "
             << sample.residentSet << " | "
             << sample.virtualMemory << " | "
             << std::fixed << std::setprecision(2) << sample.cpuUtilization << " |" << std::endl;
    }
    
    file.close();
    std::cout << "Generated memory report: " << outputPath << std::endl;
}

// CacheEfficiencyAnalyzer implementation

CacheEfficiencyAnalyzer::CacheMetrics CacheEfficiencyAnalyzer::measureCacheEfficiency(const std::string& benchmarkName) {
    CacheMetrics metrics;
    
    // TODO: Use OS-specific methods to get cache statistics
    // For now, return default values
    
    metrics.L1CacheHits = 1000000;
    metrics.L1CacheMisses = 10000;
    metrics.L2CacheHits = 2000000;
    metrics.L2CacheMisses = 20000;
    metrics.L3CacheHits = 5000000;
    metrics.L3CacheMisses = 50000;
    metrics.totalInstructions = 10000000;
    metrics.totalBranches = 2000000;
    metrics.branchMisses = 20000;
    metrics.memoryOperations = 3000000;
    metrics.IPC = metrics.totalInstructions / 1000000; // Assuming 1M cycles
    metrics.L1MissRate = static_cast<double>(metrics.L1CacheMisses) / (metrics.L1CacheHits + metrics.L1CacheMisses);
    metrics.L2MissRate = static_cast<double>(metrics.L2CacheMisses) / (metrics.L2CacheHits + metrics.L2CacheMisses);
    metrics.L3MissRate = static_cast<double>(metrics.L3CacheMisses) / (metrics.L3CacheHits + metrics.L3CacheMisses);
    metrics.branchMissRate = static_cast<double>(metrics.branchMisses) / metrics.totalBranches;
    metrics.cacheHitRatio = (metrics.L1CacheHits + metrics.L2CacheHits + metrics.L3CacheHits) / 
                           (metrics.L1CacheHits + metrics.L1CacheMisses + metrics.L2CacheHits + metrics.L2CacheMisses + metrics.L3CacheHits + metrics.L3CacheMisses);
    metrics.cacheEfficiency = metrics.cacheHitRatio; // Simplified efficiency calculation
    
    return metrics;
}

double CacheEfficiencyAnalyzer::calculateCacheEfficiency(const CacheMetrics& metrics) {
    double totalAccesses = metrics.L1CacheHits + metrics.L1CacheMisses +
                          metrics.L2CacheHits + metrics.L2CacheMisses +
                          metrics.L3CacheHits + metrics.L3CacheMisses;
    if (totalAccesses == 0) return 0.0;
    
    double weightedHitRatio = (metrics.L1CacheHits * 1.0 + metrics.L2CacheHits * 0.1 + metrics.L3CacheHits * 0.01) /
                             totalAccesses;
    
    return weightedHitRatio;
}

std::string CacheEfficiencyAnalyzer::analyzeCacheBottlenecks(const CacheMetrics& metrics) {
    std::stringstream bottlenecks;
    
    if (metrics.L1MissRate > 0.1) {
        bottlenecks << "L1 cache miss rate is high (" << metrics.L1MissRate * 100 << "%)" << std::endl;
    }
    
    if (metrics.L2MissRate > 0.05) {
        bottlenecks << "L2 cache miss rate is high (" << metrics.L2MissRate * 100 << "%)" << std::endl;
    }
    
    if (metrics.L3MissRate > 0.02) {
        bottlenecks << "L3 cache miss rate is high (" << metrics.L3MissRate * 100 << "%)" << std::endl;
    }
    
    if (metrics.branchMissRate > 0.05) {
        bottlenecks << "Branch misprediction rate is high (" << metrics.branchMissRate * 100 << "%)" << std::endl;
    }
    
    if (bottlenecks.str().empty()) {
        bottlenecks << "No significant cache bottlenecks detected." << std::endl;
    }
    
    return bottlenecks.str();
}

void CacheEfficiencyAnalyzer::generateCacheReport(const CacheMetrics& metrics, const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << outputPath << std::endl;
        return;
    }
    
    file << "Cache Efficiency Report\n";
    file << "======================\n\n";
    file << "Cache Hit/Miss Ratios:\n";
    file << "  L1: " << metrics.L1CacheHits << " hits, " << metrics.L1CacheMisses << " misses (" << metrics.L1MissRate * 100 << "% miss rate)\n";
    file << "  L2: " << metrics.L2CacheHits << " hits, " << metrics.L2CacheMisses << " misses (" << metrics.L2MissRate * 100 << "% miss rate)\n";
    file << "  L3: " << metrics.L3CacheHits << " hits, " << metrics.L3CacheMisses << " misses (" << metrics.L3MissRate * 100 << "% miss rate)\n\n";
    
    file << "Performance Metrics:\n";
    file << "  Instructions Per Cycle (IPC): " << metrics.IPC << "\n";
    file << "  Branch Prediction Rate: " << (1.0 - metrics.branchMissRate) * 100 << "%\n";
    file << "  Memory Operations: " << metrics.memoryOperations << "\n\n";
    
    file << "Efficiency Calculations:\n";
    file << "  Cache Hit Ratio: " << metrics.cacheHitRatio * 100 << "%\n";
    file << "  Cache Efficiency: " << metrics.cacheEfficiency * 100 << "%\n\n";
    
    file << "Bottlenecks:\n" << analyzeCacheBottlenecks(metrics) << std::endl;
    
    file.close();
    std::cout << "Generated cache report: " << outputPath << std::endl;
}

double CacheEfficiencyAnalyzer::analyzeSpatialLocality(const std::vector<uint64_t>& addresses) {
    if (addresses.empty()) return 0.0;
    
    // Calculate spatial locality by checking distance between consecutive accesses
    size_t consecutiveSameCacheLine = 0;
    size_t totalAccesses = addresses.size();
    size_t cacheLineSize = 64; // Typical cache line size
    
    for (size_t i = 1; i < addresses.size(); ++i) {
        uint64_t addr1 = addresses[i-1];
        uint64_t addr2 = addresses[i];
        
        uint64_t cacheLine1 = addr1 / cacheLineSize;
        uint64_t cacheLine2 = addr2 / cacheLineSize;
        
        if (cacheLine1 == cacheLine2) {
            consecutiveSameCacheLine++;
        }
    }
    
    return static_cast<double>(consecutiveSameCacheLine) / totalAccesses;
}

double CacheEfficiencyAnalyzer::analyzeTemporalLocality(const std::vector<uint64_t>& accessTimestamps) {
    // Calculate temporal locality by checking if same addresses are accessed again soon
    // This is a simplified implementation
    return 0.0; // TODO: Implement proper temporal locality analysis
}

double CacheEfficiencyAnalyzer::analyzeWorkingSetSize(const std::vector<uint64_t>& addresses) {
    // Estimate working set size by counting unique cache lines accessed
    size_t cacheLineSize = 64;
    std::unordered_set<uint64_t> cacheLines;
    
    for (uint64_t addr : addresses) {
        cacheLines.insert(addr / cacheLineSize);
    }
    
    return static_cast<double>(cacheLines.size()) * cacheLineSize;
}

double CacheEfficiencyAnalyzer::analyzeMemoryLevelDistribution(const std::vector<uint64_t>& addresses) {
    // Estimate distribution of memory accesses across cache levels
    // This is a simplified implementation based on access patterns
    return 0.0; // TODO: Implement proper memory level distribution analysis
}

// AdvancedStatistics implementation

AdvancedStatistics::StatisticalSummary AdvancedStatistics::computeSummary(const std::vector<double>& data) {
    StatisticalSummary summary;
    
    if (data.empty()) {
        return summary;
    }
    
    // Basic statistics
    summary.mean = mean(data);
    summary.median = median(data);
    summary.variance = variance(data);
    summary.standardDeviation = standardDeviation(data);
    summary.minValue = *std::min_element(data.begin(), data.end());
    summary.maxValue = *std::max_element(data.begin(), data.end());
    summary.range = summary.maxValue - summary.minValue;
    
    // Mode calculation (simplified)
    std::map<double, int> frequency;
    for (double value : data) {
        frequency[value]++;
    }
    if (!frequency.empty()) {
        double maxFreq = 0.0;
        for (const auto& pair : frequency) {
            if (pair.second > maxFreq) {
                maxFreq = pair.second;
                summary.mode = pair.first;
            }
        }
    }
    
    // Percentiles
    std::vector<double> percentiles = {25.0, 50.0, 75.0, 90.0, 95.0, 99.0};
    summary.percentiles = calculatePercentiles(data, percentiles);
    
    // Skewness and kurtosis
    summary.skewness = skewness(data);
    summary.kurtosis = kurtosis(data);
    
    // Coefficient of variation
    summary.coefficientOfVariation = (summary.mean != 0.0) ? (summary.standardDeviation / summary.mean) : 0.0;
    
    // Confidence interval
    summary.confidenceInterval95 = confidenceInterval(data, 0.95);
    
    // Sample size
    summary.sampleSize = data.size();
    
    // Interquartile range
    if (summary.percentiles.size() >= 2) {
        summary.iqr = summary.percentiles[2] - summary.percentiles[0]; // Q3 - Q1
    }
    
    return summary;
}

std::vector<double> AdvancedStatistics::bootstrap(const std::vector<double>& data, size_t iterations) {
    std::vector<double> bootstrapSamples;
    bootstrapSamples.reserve(iterations);
    
    size_t n = data.size();
    if (n == 0) return bootstrapSamples;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, n-1);
    
    for (size_t i = 0; i < iterations; ++i) {
        std::vector<double> sample;
        sample.reserve(n);
        
        for (size_t j = 0; j < n; ++j) {
            sample.push_back(data[dist(gen)]);
        }
        
        bootstrapSamples.push_back(mean(sample));
    }
    
    return bootstrapSamples;
}

double AdvancedStatistics::bootstrapConfidenceInterval(const std::vector<double>& data, double confidence) {
    std::vector<double> bootstrapSamples = bootstrap(data);
    
    if (bootstrapSamples.empty()) return 0.0;
    
    std::sort(bootstrapSamples.begin(), bootstrapSamples.end());
    
    size_t lowerIndex = static_cast<size_t>((1.0 - confidence) / 2.0 * bootstrapSamples.size());
    size_t upperIndex = bootstrapSamples.size() - 1 - lowerIndex;
    
    if (lowerIndex >= bootstrapSamples.size() || upperIndex >= bootstrapSamples.size()) {
        return 0.0;
    }
    
    return bootstrapSamples[upperIndex] - bootstrapSamples[lowerIndex];
}

double AdvancedStatistics::coefficientOfVariation(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    double mean = Statistics::mean(data);
    if (mean == 0.0) return 0.0;
    return standardDeviation(data) / mean;
}

double AdvancedStatistics::skewness(const std::vector<double>& data) {
    if (data.size() < 3) return 0.0;
    
    double mean = Statistics::mean(data);
    double stddev = standardDeviation(data);
    
    if (stddev == 0.0) return 0.0;
    
    double sum = 0.0;
    for (double value : data) {
        sum += std::pow((value - mean) / stddev, 3);
    }
    
    return sum / (data.size() - 2) / (data.size() - 3);
}

double AdvancedStatistics::kurtosis(const std::vector<double>& data) {
    if (data.size() < 4) return 0.0;
    
    double mean = Statistics::mean(data);
    double stddev = standardDeviation(data);
    
    if (stddev == 0.0) return 0.0;
    
    double sum = 0.0;
    for (double value : data) {
        sum += std::pow((value - mean) / stddev, 4);
    }
    
    return (sum / (data.size() - 3)) - 3.0;
}

std::vector<double> AdvancedStatistics::calculatePercentiles(const std::vector<double>& data, const std::vector<double>& percentiles) {
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    std::vector<double> results;
    for (double percentile : percentiles) {
        results.push_back(percentile(sorted, percentile));
    }
    
    return results;
}

std::pair<double, double> AdvancedStatistics::linearRegression(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.empty()) {
        return {0.0, 0.0};
    }
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    size_t n = x.size();
    
    for (size_t i = 0; i < n; ++i) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }
    
    double denominator = n * sumX2 - sumX * sumX;
    if (denominator == 0.0) return {0.0, 0.0};
    
    double slope = (n * sumXY - sumX * sumY) / denominator;
    double intercept = (sumY - slope * sumX) / n;
    
    return {slope, intercept};
}

double AdvancedStatistics::rSquared(const std::vector<double>& x, const std::vector<double>& y, const std::pair<double, double>& regression) {
    if (x.size() != y.size() || x.empty()) return 0.0;
    
    double ssTot = 0.0, ssRes = 0.0;
    double meanY = Statistics::mean(y);
    
    for (size_t i = 0; i < x.size(); ++i) {
        double predicted = regression.first * x[i] + regression.second;
        ssRes += std::pow(y[i] - predicted, 2);
        ssTot += std::pow(y[i] - meanY, 2);
    }
    
    if (ssTot == 0.0) return 1.0;
    
    return 1.0 - ssRes / ssTot;
}

std::vector<double> AdvancedStatistics::movingAverage(const std::vector<double>& data, size_t windowSize) {
    return Statistics::movingAverage(data, windowSize);
}

std::vector<double> AdvancedStatistics::exponentialMovingAverage(const std::vector<double>& data, double alpha) {
    std::vector<double> ema;
    if (data.empty()) return ema;
    
    ema.push_back(data[0]);
    for (size_t i = 1; i < data.size(); ++i) {
        ema.push_back(alpha * data[i] + (1.0 - alpha) * ema[i-1]);
    }
    
    return ema;
}

double AdvancedStatistics::analyzeTimeSeriesTrend(const std::vector<double>& data) {
    if (data.size() < 3) return 0.0;
    
    // Calculate trend using linear regression slope
    auto regression = linearRegression(
        std::vector<double>(data.size(), 0.0), // x = 0, 1, 2, ...
        data
    );
    
    return regression.first;
}

bool AdvancedStatistics::isOutlier(double value, const std::vector<double>& data, double zThreshold) {
    if (data.empty()) return false;
    
    double mean = Statistics::mean(data);
    double stddev = standardDeviation(data);
    
    if (stddev == 0.0) return false;
    
    double zScore = std::abs(value - mean) / stddev;
    return zScore > zThreshold;
}

std::vector<size_t> AdvancedStatistics::findOutliers(const std::vector<double>& data, double zThreshold) {
    std::vector<size_t> outliers;
    
    if (data.empty()) return outliers;
    
    double mean = Statistics::mean(data);
    double stddev = standardDeviation(data);
    
    if (stddev == 0.0) return outliers;
    
    for (size_t i = 0; i < data.size(); ++i) {
        double zScore = std::abs(data[i] - mean) / stddev;
        if (zScore > zThreshold) {
            outliers.push_back(i);
        }
    }
    
    return outliers;
}

double AdvancedStatistics::entropy(const std::vector<size_t>& frequencies) {
    double total = 0.0;
    for (size_t freq : frequencies) {
        total += freq;
    }
    
    if (total == 0.0) return 0.0;
    
    double entropy = 0.0;
    for (size_t freq : frequencies) {
        if (freq > 0) {
            double p = static_cast<double>(freq) / total;
            entropy -= p * std::log2(p);
        }
    }
    
    return entropy;
}

double AdvancedStatistics::mutualInformation(const std::vector<size_t>& x, const std::vector<size_t>& y) {
    if (x.size() != y.size() || x.empty()) return 0.0;
    
    // Calculate joint probability distribution
    std::map<std::pair<size_t, size_t>, double> jointProb;
    size_t total = x.size();
    
    for (size_t i = 0; i < x.size(); ++i) {
        jointProb[std::make_pair(x[i], y[i])] += 1.0;
    }
    
    for (auto& pair : jointProb) {
        pair.second /= total;
    }
    
    // Calculate marginal distributions
    std::map<size_t, double> px, py;
    for (const auto& pair : jointProb) {
        px[pair.first.first] += pair.second;
        py[pair.first.second] += pair.second;
    }
    
    // Calculate mutual information
    double mi = 0.0;
    for (const auto& pair : jointProb) {
        size_t xi = pair.first.first;
        size_t yi = pair.first.second;
        double pxy = pair.second;
        
        if (pxy > 0.0 && px[xi] > 0.0 && py[yi] > 0.0) {
            double logTerm = std::log2(pxy / (px[xi] * py[yi]));
            mi += pxy * logTerm;
        }
    }
    
    return mi;
}

std::vector<double> AdvancedStatistics::normalize(const std::vector<double>& data) {
    std::vector<double> normalized;
    if (data.empty()) return normalized;
    
    double min = *std::min_element(data.begin(), data.end());
    double max = *std::max_element(data.begin(), data.end());
    double range = max - min;
    
    if (range == 0.0) {
        normalized = data;
    } else {
        for (double value : data) {
            normalized.push_back((value - min) / range);
        }
    }
    
    return normalized;
}

std::vector<double> AdvancedStatistics::standardize(const std::vector<double>& data) {
    std::vector<double> standardized;
    if (data.empty()) return standardized;
    
    double mean = Statistics::mean(data);
    double stddev = standardDeviation(data);
    
    if (stddev == 0.0) {
        standardized = data;
    } else {
        for (double value : data) {
            standardized.push_back((value - mean) / stddev);
        }
    }
    
    return standardized;
}

double AdvancedStatistics::mahalanobisDistance(const std::vector<double>& point, const std::vector<std::vector<double>>& data) {
    if (data.empty() || data[0].size() != point.size()) return 0.0;
    
    // Calculate mean and covariance matrix
    std::vector<double> mean(point.size(), 0.0);
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            mean[i] += row[i];
        }
    }
    for (size_t i = 0; i < mean.size(); ++i) {
        mean[i] /= data.size();
    }
    
    // Calculate covariance matrix (simplified)
    std::vector<std::vector<double>> cov(point.size(), std::vector<double>(point.size(), 0.0));
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            for (size_t j = 0; j < row.size(); ++j) {
                cov[i][j] += (row[i] - mean[i]) * (row[j] - mean[j]);
            }
        }
    }
    for (size_t i = 0; i < cov.size(); ++i) {
        for (size_t j = 0; j < cov[i].size(); ++j) {
            cov[i][j] /= data.size();
        }
    }
    
    // Calculate Mahalanobis distance
    double distance = 0.0;
    for (size_t i = 0; i < point.size(); ++i) {
        double diff = point[i] - mean[i];
        for (size_t j = 0; j < point.size(); ++j) {
            distance += diff * diff * cov[i][j];
        }
    }
    
    return std::sqrt(distance);
}

} // namespace infrastructure
} // namespace benchmark
} // namespace nlm

#include "InfrastructureBenchmarks.inl"