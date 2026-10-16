#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <random>

namespace nlm {

// Statistical analysis utilities
class Statistics {
public:
    struct Stats {
        double mean = 0.0;
        double variance = 0.0;
        double stddev = 0.0;
        double median = 0.0;
        double min = 0.0;
        double max = 0.0;
        double range = 0.0;
        size_t count = 0;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "mean=" << std::fixed << std::setprecision(4) << mean
               << " variance=" << std::fixed << std::setprecision(6) << variance
               << " stddev=" << std::fixed << std::setprecision(4) << stddev
               << " median=" << std::fixed << std::setprecision(4) << median
               << " min=" << min << " max=" << max
               << " range=" << range << " count=" << count;
            return ss.str();
        }
    };
    
    static Stats calculate(const std::vector<double>& values) {
        Stats stats;
        stats.count = values.size();
        if (stats.count == 0) return stats;
        
        stats.min = *std::min_element(values.begin(), values.end());
        stats.max = *std::max_element(values.begin(), values.end());
        stats.range = stats.max - stats.min;
        
        stats.mean = std::accumulate(values.begin(), values.end(), 0.0) / stats.count;
        
        double sumSquares = 0.0;
        for (double value : values) {
            sumSquares += (value - stats.mean) * (value - stats.mean);
        }
        stats.variance = stats.count > 1 ? sumSquares / (stats.count - 1) : 0.0;
        stats.stddev = std::sqrt(stats.variance);
        
        // Calculate median
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        if (stats.count % 2 == 0) {
            stats.median = (sorted[stats.count/2 - 1] + sorted[stats.count/2]) / 2.0;
        } else {
            stats.median = sorted[stats.count/2];
        }
        
        return stats;
    }
    
    static double correlation(const std::vector<double>& x, const std::vector<double>& y) {
        if (x.size() != y.size() || x.size() == 0) return 0.0;
        
        Stats statsX = calculate(x);
        Stats statsY = calculate(y);
        
        double covariance = 0.0;
        for (size_t i = 0; i < x.size(); ++i) {
            covariance += (x[i] - statsX.mean) * (y[i] - statsY.mean);
        }
        
        if (statsX.stddev == 0.0 || statsY.stddev == 0.0) return 0.0;
        
        return covariance / ((x.size() - 1) * statsX.stddev * statsY.stddev);
    }
};

// Neural activity analyzer
class NeuralAnalyzer {
public:
    struct NeuralActivity {
        size_t neuronId = 0;
        double firingRate = 0.0;
        size_t spikeCount = 0;
        double averageInterSpikeInterval = 0.0;
        double burstFrequency = 0.0;
        double synchronyIndex = 0.0;
        double coherence = 0.0;
        std::vector<double> frequencySpectrum;
        std::string activityPattern;
    };
    
    struct NetworkStatistics {
        size_t totalNeurons = 0;
        size_t activeNeurons = 0;
        size_t firingNeurons = 0;
        double overallFiringRate = 0.0;
        double excitationInhibitionRatio = 0.0;
        double networkSynchrony = 0.0;
        double averageCoherence = 0.0;
        std::vector<double> firingRateDistribution;
        std::vector<NeuralActivity> neuronActivities;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "Network Statistics:" << std::endl;
            ss << "  Total Neurons: " << totalNeurons << std::endl;
            ss << "  Active Neurons: " << activeNeurons << std::endl;
            ss << "  Firing Neurons: " << firingNeurons << std::endl;
            ss << "  Overall Firing Rate: " << std::fixed << std::setprecision(4) << overallFiringRate << std::endl;
            ss << "  E/I Ratio: " << std::fixed << std::setprecision(4) << excitationInhibitionRatio << std::endl;
            ss << "  Network Synchrony: " << std::fixed << std::setprecision(4) << networkSynchrony << std::endl;
            ss << "  Average Coherence: " << std::fixed << std::setprecision(4) << averageCoherence << std::endl;
            ss << "  Total Neurons Analyzed: " << neuronActivities.size() << std::endl;
            return ss.str();
        }
    };
    
    NeuralAnalyzer() = default;
    ~NeuralAnalyzer() = default;
    
    // Analyze neural activity from spike trains
    NeuralActivity analyzeNeuronActivity(const std::vector<double>& spikeTimes, 
                                        double samplingRate = 1000.0) {
        NeuralActivity activity;
        
        if (spikeTimes.empty()) {
            return activity;
        }
        
        activity.spikeCount = spikeTimes.size();
        activity.firingRate = (spikeTimes.size() / (spikeTimes.back() - spikeTimes.front())) * samplingRate;
        
        if (spikeTimes.size() > 1) {
            std::vector<double> interSpikeIntervals;
            for (size_t i = 1; i < spikeTimes.size(); ++i) {
                interSpikeIntervals.push_back(spikeTimes[i] - spikeTimes[i-1]);
            }
            
            activity.averageInterSpikeInterval = std::accumulate(interSpikeIntervals.begin(), 
                                                              interSpikeIntervals.end(), 0.0) / interSpikeIntervals.size();
            
            // Calculate burst frequency (simplified)
            double maxISI = *std::max_element(interSpikeIntervals.begin(), interSpikeIntervals.end());
            activity.burstFrequency = 1.0 / maxISI;
        }
        
        // Generate frequency spectrum (simplified FFT)
        activity.frequencySpectrum = computeFrequencySpectrum(spikeTimes, samplingRate);
        
        // Determine activity pattern
        activity.activityPattern = determineActivityPattern(spikeTimes);
        
        return activity;
    }
    
    // Analyze network statistics
    NetworkStatistics analyzeNetwork(const std::vector<NeuralActivity>& neuronActivities) {
        NetworkStatistics stats;
        
        stats.totalNeurons = neuronActivities.size();
        for (const auto& activity : neuronActivities) {
            stats.firingNeurons += (activity.spikeCount > 0) ? 1 : 0;
            stats.activeNeurons += (activity.firingRate > 0.0) ? 1 : 0;
            stats.overallFiringRate += activity.firingRate;
        }
        
        if (stats.totalNeurons > 0) {
            stats.overallFiringRate /= stats.totalNeurons;
        }
        
        // Calculate firing rate distribution
        for (const auto& activity : neuronActivities) {
            stats.firingRateDistribution.push_back(activity.firingRate);
        }
        
        // Calculate synchrony and coherence (simplified)
        stats.networkSynchrony = computeNetworkSynchrony(neuronActivities);
        stats.averageCoherence = computeAverageCoherence(neuronActivities);
        
        stats.neuronActivities = neuronActivities;
        
        return stats;
    }
    
    // Detect pattern changes
    std::vector<std::string> detectPatternChanges(const std::vector<NeuralActivity>& activities,
                                                 double threshold = 0.1) {
        std::vector<std::string> patterns;
        
        if (activities.empty()) return patterns;
        
        // Analyze firing rate distribution
        double meanFR = 0.0;
        for (const auto& activity : activities) {
            meanFR += activity.firingRate;
        }
        meanFR /= activities.size();
        
        double variance = 0.0;
        for (const auto& activity : activities) {
            variance += (activity.firingRate - meanFR) * (activity.firingRate - meanFR);
        }
        variance /= activities.size();
        
        double stddev = std::sqrt(variance);
        
        // Classify patterns
        for (const auto& activity : activities) {
            double zScore = (activity.firingRate - meanFR) / (stddev > 0 ? stddev : 1.0);
            
            if (std::abs(zScore) > 2.0) {
                if (activity.firingRate > meanFR) {
                    patterns.push_back("high_firing_neuron");
                } else {
                    patterns.push_back("low_firing_neuron");
                }
            } else {
                patterns.push_back("normal_firing_neuron");
            }
        }
        
        return patterns;
    }
    
    // Generate neural activity report
    std::string generateNeuralReport(const NetworkStatistics& stats) {
        std::stringstream ss;
        ss << "=== Neural Activity Analysis Report ===" << std::endl;
        ss << stats.toString() << std::endl;
        
        // Add statistics for firing rate distribution
        if (!stats.firingRateDistribution.empty()) {
            Statistics::Stats frStats = Statistics::calculate(stats.firingRateDistribution);
            ss << "Firing Rate Distribution Statistics:" << std::endl;
            ss << "  Mean: " << std::fixed << std::setprecision(4) << frStats.mean << std::endl;
            ss << "  Std Dev: " << std::fixed << std::setprecision(4) << frStats.stddev << std::endl;
            ss << "  Range: [" << frStats.min << ", " << frStats.max << "]" << std::endl;
        }
        
        return ss.str();
    }
    
private:
    std::vector<double> computeFrequencySpectrum(const std::vector<double>& spikeTimes, 
                                                 double samplingRate) {
        std::vector<double> spectrum;
        // Simplified spectrum computation
        spectrum.resize(64, 0.0); // 64 frequency bins
        return spectrum;
    }
    
    std::string determineActivityPattern(const std::vector<double>& spikeTimes) {
        if (spikeTimes.size() < 3) return "regular";
        
        // Calculate coefficient of variation of ISIs
        std::vector<double> isis;
        for (size_t i = 1; i < spikeTimes.size(); ++i) {
            isis.push_back(spikeTimes[i] - spikeTimes[i-1]);
        }
        
        double meanISI = std::accumulate(isis.begin(), isis.end(), 0.0) / isis.size();
        double variance = 0.0;
        for (double isi : isis) {
            variance += (isi - meanISI) * (isi - meanISI);
        }
        variance /= isis.size();
        double cv = std::sqrt(variance) / meanISI;
        
        if (cv < 0.1) return "regular";
        else if (cv < 0.3) return "bursting";
        else if (cv < 0.5) return "irregular";
        else return "complex";
    }
    
    double computeNetworkSynchrony(const std::vector<NeuralActivity>& activities) {
        if (activities.size() < 2) return 0.0;
        
        double totalSynchrony = 0.0;
        size_t pairs = 0;
        
        for (size_t i = 0; i < activities.size(); ++i) {
            for (size_t j = i + 1; j < activities.size(); ++j) {
                double synchrony = std::abs(activities[i].synchronyIndex - activities[j].synchronyIndex);
                totalSynchrony += synchrony;
                pairs++;
            }
        }
        
        return pairs > 0 ? totalSynchrony / pairs : 0.0;
    }
    
    double computeAverageCoherence(const std::vector<NeuralActivity>& activities) {
        if (activities.empty()) return 0.0;
        
        double totalCoherence = 0.0;
        for (const auto& activity : activities) {
            totalCoherence += activity.coherence;
        }
        
        return totalCoherence / activities.size();
    }
};

// Connectivity analyzer
class ConnectivityAnalyzer {
public:
    struct ConnectionInfo {
        size_t sourceId = 0;
        size_t targetId = 0;
        double weight = 0.0;
        double delay = 0.0;
        std::string connectionType;
        double impactStrength = 0.0;
        bool isFunctional = false;
    };
    
    struct NetworkConnectivity {
        size_t totalConnections = 0;
        double averageWeight = 0.0;
        double averageDelay = 0.0;
        double connectionDensity = 0.0;
        std::vector<ConnectionInfo> strongestConnections;
        std::vector<ConnectionInfo> weakestConnections;
        std::map<size_t, size_t> degreeDistribution; // neuronId -> degree
        std::unordered_map<size_t, std::vector<size_t>> adjacencyList;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "Network Connectivity:" << std::endl;
            ss << "  Total Connections: " << totalConnections << std::endl;
            ss << "  Average Weight: " << std::fixed << std::setprecision(4) << averageWeight << std::endl;
            ss << "  Average Delay: " << std::fixed << std::setprecision(4) << averageDelay << std::endl;
            ss << "  Connection Density: " << std::fixed << std::setprecision(4) << connectionDensity << std::endl;
            ss << "  Strongest Connection: " << (strongestConnections.empty() ? "N/A" : 
                            std::to_string(strongestConnections[0].weight) + " weight") << std::endl;
            ss << "  Weakest Connection: " << (weakestConnections.empty() ? "N/A" : 
                            std::to_string(weakestConnections[0].weight) + " weight") << std::endl;
            return ss.str();
        }
    };
    
    ConnectivityAnalyzer() = default;
    ~ConnectivityAnalyzer() = default;
    
    // Analyze connectivity from adjacency matrix
    NetworkConnectivity analyzeConnectivity(const std::vector<std::vector<double>>& adjacencyMatrix) {
        NetworkConnectivity connectivity;
        
        size_t n = adjacencyMatrix.size();
        connectivity.totalConnections = 0;
        double totalWeight = 0.0;
        double totalDelay = 0.0;
        
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (adjacencyMatrix[i][j] > 0.0) {
                    connectivity.totalConnections++;
                    totalWeight += adjacencyMatrix[i][j];
                    connectivity.degreeDistribution[i]++;
                    connectivity.degreeDistribution[j]++;
                    
                    // Add to adjacency list
                    connectivity.adjacencyList[i].push_back(j);
                    
                    // Create connection info
                    ConnectionInfo conn;
                    conn.sourceId = i;
                    conn.targetId = j;
                    conn.weight = adjacencyMatrix[i][j];
                    conn.connectionType = "excitation";
                    conn.isFunctional = true;
                    
                    // Estimate delay based on weight
                    conn.delay = 1.0 / (conn.weight + 0.01); // Simplified formula
                    
                    connectivity.strongestConnections.push_back(conn);
                }
            }
        }
        
        // Calculate averages
        if (connectivity.totalConnections > 0) {
            connectivity.averageWeight = totalWeight / connectivity.totalConnections;
            connectivity.connectionDensity = static_cast<double>(connectivity.totalConnections) / (n * n);
        }
        
        // Sort connections by weight
        std::sort(connectivity.strongestConnections.begin(), connectivity.strongestConnections.end(),
                 [](const ConnectionInfo& a, const ConnectionInfo& b) {
                     return a.weight > b.weight;
                 });
        
        // Keep only top 10 strongest connections
        if (connectivity.strongestConnections.size() > 10) {
            connectivity.strongestConnections.resize(10);
        }
        
        // Find weakest connections (reverse sorted)
        connectivity.weakestConnections = connectivity.strongestConnections;
        std::reverse(connectivity.weakestConnections.begin(), connectivity.weakestConnections.end());
        if (connectivity.weakestConnections.size() > 10) {
            connectivity.weakestConnections.resize(10);
        }
        
        return connectivity;
    }
    
    // Find shortest path in network
    std::vector<size_t> findShortestPath(const std::vector<std::vector<double>>& adjacencyMatrix,
                                         size_t start, size_t end) {
        size_t n = adjacencyMatrix.size();
        std::vector<double> distances(n, std::numeric_limits<double>::max());
        std::vector<size_t> predecessors(n, -1);
        std::vector<bool> visited(n, false);
        
        distances[start] = 0.0;
        
        for (size_t i = 0; i < n; ++i) {
            size_t current = -1;
            double minDist = std::numeric_limits<double>::max();
            
            for (size_t j = 0; j < n; ++j) {
                if (!visited[j] && distances[j] < minDist) {
                    minDist = distances[j];
                    current = j;
                }
            }
            
            if (current == -1 || current == end) break;
            
            visited[current] = true;
            
            for (size_t j = 0; j < n; ++j) {
                if (!visited[j] && adjacencyMatrix[current][j] > 0.0) {
                    double newDist = distances[current] + adjacencyMatrix[current][j];
                    if (newDist < distances[j]) {
                        distances[j] = newDist;
                        predecessors[j] = current;
                    }
                }
            }
        }
        
        // Reconstruct path
        std::vector<size_t> path;
        if (predecessors[end] != -1 || start == end) {
            for (size_t current = end; current != -1; current = predecessors[current]) {
                path.insert(path.begin(), current);
            }
        }
        
        return path;
    }
    
    // Calculate network centrality measures
    std::map<size_t, double> calculateCentrality(const std::vector<std::vector<double>>& adjacencyMatrix) {
        std::map<size_t, double> centralities;
        size_t n = adjacencyMatrix.size();
        
        // Calculate degree centrality
        for (size_t i = 0; i < n; ++i) {
            double degree = 0.0;
            for (size_t j = 0; j < n; ++j) {
                if (adjacencyMatrix[i][j] > 0.0) {
                    degree++;
                }
            }
            centralities[i] = degree;
        }
        
        // Normalize by max degree
        double maxDegree = 0.0;
        for (const auto& pair : centralities) {
            if (pair.second > maxDegree) maxDegree = pair.second;
        }
        
        if (maxDegree > 0.0) {
            for (auto& pair : centralities) {
                pair.second /= maxDegree;
            }
        }
        
        return centralities;
    }
    
    // Generate connectivity report
    std::string generateConnectivityReport(const NetworkConnectivity& connectivity) {
        std::stringstream ss;
        ss << "=== Connectivity Analysis Report ===" << std::endl;
        ss << connectivity.toString() << std::endl;
        
        ss << "Degree Distribution:" << std::endl;
        for (const auto& pair : connectivity.degreeDistribution) {
            ss << "  Neuron " << pair.first << ": " << pair.second << " connections" << std::endl;
        }
        
        ss << "\nStrongest Connections:" << std::endl;
        for (size_t i = 0; i < connectivity.strongestConnections.size() && i < 5; ++i) {
            const auto& conn = connectivity.strongestConnections[i];
            ss << "  " << conn.sourceId << " -> " << conn.targetId
               << " (weight: " << std::fixed << std::setprecision(4) << conn.weight << ")" << std::endl;
        }
        
        return ss.str();
    }
};

// Performance analyzer
class PerformanceAnalyzer {
public:
    struct PerformanceMetrics {
        double cpuUsage = 0.0;
        double memoryUsageMB = 0.0;
        double networkIO = 0.0;
        double simulationSpeed = 0.0;
        double efficiency = 0.0;
        size_t activeNeurons = 0;
        size_t totalNeurons = 0;
        double averageFiringRate = 0.0;
        double simulationTime = 0.0;
        std::string timestamp;
        std::vector<double> resourceUsageOverTime;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "Performance Metrics:" << std::endl;
            ss << "  CPU Usage: " << std::fixed << std::setprecision(2) << cpuUsage << "%" << std::endl;
            ss << "  Memory Usage: " << std::fixed << std::setprecision(2) << memoryUsageMB << " MB" << std::endl;
            ss << "  Network I/O: " << std::fixed << std::setprecision(2) << networkIO << " MB/s" << std::endl;
            ss << "  Simulation Speed: " << std::fixed << std::setprecision(2) << simulationSpeed << " steps/s" << std::endl;
            ss << "  Efficiency: " << std::fixed << std::setprecision(4) << efficiency << " %" << std::endl;
            ss << "  Active Neurons: " << activeNeurons << std::endl;
            ss << "  Total Neurons: " << totalNeurons << std::endl;
            ss << "  Average Firing Rate: " << std::fixed << std::setprecision(4) << averageFiringRate << std::endl;
            ss << "  Simulation Time: " << std::fixed << std::setprecision(2) << simulationTime << " s" << std::endl;
            return ss.str();
        }
    };
    
    PerformanceAnalyzer() = default;
    ~PerformanceAnalyzer() = default;
    
    // Analyze simulation performance
    PerformanceMetrics analyzePerformance(double cpuUsage,
                                          double memoryUsageMB,
                                          double networkIO,
                                          double simulationSpeed,
                                          size_t activeNeurons,
                                          size_t totalNeurons,
                                          double averageFiringRate,
                                          double simulationTime) {
        PerformanceMetrics metrics;
        
        metrics.cpuUsage = cpuUsage;
        metrics.memoryUsageMB = memoryUsageMB;
        metrics.networkIO = networkIO;
        metrics.simulationSpeed = simulationSpeed;
        metrics.activeNeurons = activeNeurons;
        metrics.totalNeurons = totalNeurons;
        metrics.averageFiringRate = averageFiringRate;
        metrics.simulationTime = simulationTime;
        
        // Calculate efficiency
        if (simulationSpeed > 0.0) {
            metrics.efficiency = (activeNeurons / totalNeurons) * (simulationSpeed / 100.0) * 100.0;
        }
        
        // Generate timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        metrics.timestamp = ss.str();
        
        return metrics;
    }
    
    // Compare performance between benchmarks
    std::vector<std::string> comparePerformance(const std::vector<PerformanceMetrics>& metricsList) {
        std::vector<std::string> comparisons;
        
        if (metricsList.size() < 2) {
            comparisons.push_back("Not enough data for comparison.");
            return comparisons;
        }
        
        // Find best performer
        const PerformanceMetrics* best = &metricsList[0];
        for (const auto& metrics : metricsList) {
            if (metrics.efficiency > best->efficiency) {
                best = &metrics;
            }
        }
        
        comparisons.push_back("Best Performance: " + best->timestamp + " (" +
                             std::to_string(best->efficiency) + "% efficiency)");
        
        // Compare memory usage
        double minMemory = metricsList[0].memoryUsageMB;
        double maxMemory = metricsList[0].memoryUsageMB;
        for (const auto& metrics : metricsList) {
            minMemory = std::min(minMemory, metrics.memoryUsageMB);
            maxMemory = std::max(maxMemory, metrics.memoryUsageMB);
        }
        
        comparisons.push_back("Memory Usage Range: " + std::to_string(minMemory) + " - " +
                             std::to_string(maxMemory) + " MB");
        
        // Compare simulation speed
        double minSpeed = metricsList[0].simulationSpeed;
        double maxSpeed = metricsList[0].simulationSpeed;
        for (const auto& metrics : metricsList) {
            minSpeed = std::min(minSpeed, metrics.simulationSpeed);
            maxSpeed = std::max(maxSpeed, metrics.simulationSpeed);
        }
        
        comparisons.push_back("Simulation Speed Range: " + std::to_string(minSpeed) + " - " +
                             std::to_string(maxSpeed) + " steps/s");
        
        return comparisons;
    }
    
    // Generate performance analysis report
    std::string generatePerformanceReport(const PerformanceMetrics& metrics,
                                           const std::string& benchmarkName) {
        std::stringstream ss;
        ss << "=== Performance Analysis Report ===" << std::endl;
        ss << "Benchmark: " << benchmarkName << std::endl;
        ss << "Timestamp: " << metrics.timestamp << std::endl;
        ss << std::endl;
        ss << metrics.toString() << std::endl;
        
        // Resource usage trend (simplified)
        ss << "Resource Usage Trend:" << std::endl;
        ss << "  CPU: " << std::fixed << std::setprecision(2) << metrics.cpuUsage << "%" << std::endl;
        ss << "  Memory: " << std::fixed << std::setprecision(2) << metrics.memoryUsageMB << " MB" << std::endl;
        
        return ss.str();
    }
};

// Pattern analyzer
class PatternAnalyzer {
public:
    struct PatternResult {
        std::string patternType;
        double confidence = 0.0;
        double frequency = 0.0;
        std::vector<double> parameters;
        std::string description;
        
        std::string toString() const {
            std::stringstream ss;
            ss << patternType << " (confidence: " << std::fixed << std::setprecision(4) << confidence << ")";
            if (!description.empty()) {
                ss << " - " << description;
            }
            return ss.str();
        }
    };
    
    PatternAnalyzer() = default;
    ~PatternAnalyzer() = default;
    
    // Detect patterns in time series data
    std::vector<PatternResult> detectPatterns(const std::vector<double>& timeSeries,
                                              double samplingRate = 1000.0) {
        std::vector<PatternResult> patterns;
        
        if (timeSeries.size() < 10) {
            return patterns;
        }
        
        // Simple pattern detection (would use more sophisticated algorithms in production)
        
        // Check for sinusoidal pattern
        PatternResult sinusoid = detectSinusoidalPattern(timeSeries, samplingRate);
        if (sinusoid.confidence > 0.1) {
            patterns.push_back(sinusoid);
        }
        
        // Check for exponential growth/decay
        PatternResult exponential = detectExponentialPattern(timeSeries);
        if (exponential.confidence > 0.1) {
            patterns.push_back(exponential);
        }
        
        // Check for periodic bursts
        PatternResult bursts = detectBurstPattern(timeSeries, samplingRate);
        if (bursts.confidence > 0.1) {
            patterns.push_back(bursts);
        }
        
        // Check for linear trend
        PatternResult linear = detectLinearPattern(timeSeries);
        if (linear.confidence > 0.1) {
            patterns.push_back(linear);
        }
        
        // Sort by confidence
        std::sort(patterns.begin(), patterns.end(),
                 [](const PatternResult& a, const PatternResult& b) {
                     return a.confidence > b.confidence;
                 });
        
        return patterns;
    }
    
    // Analyze neural firing patterns
    std::vector<PatternResult> analyzeNeuralPatterns(const std::vector<std::vector<double>>& spikeTrains,
                                                     double samplingRate = 1000.0) {
        std::vector<PatternResult> patterns;
        
        if (spikeTrains.empty()) return patterns;
        
        // Analyze each neuron
        for (size_t i = 0; i < spikeTrains.size(); ++i) {
            PatternResult pattern = analyzeSingleNeuronPattern(spikeTrains[i], samplingRate);
            if (pattern.confidence > 0.1) {
                patterns.push_back(pattern);
            }
        }
        
        // Analyze network-level patterns
        PatternResult networkPattern = analyzeNetworkPattern(spikeTrains);
        if (networkPattern.confidence > 0.1) {
            patterns.push_back(networkPattern);
        }
        
        return patterns;
    }
    
    // Generate pattern analysis report
    std::string generatePatternReport(const std::vector<PatternResult>& patterns) {
        std::stringstream ss;
        ss << "=== Pattern Analysis Report ===" << std::endl;
        ss << std::endl;
        
        if (patterns.empty()) {
            ss << "No significant patterns detected." << std::endl;
            return ss.str();
        }
        
        ss << "Top " << std::min(5, static_cast<int>(patterns.size())) << " Patterns:" << std::endl;
        
        for (size_t i = 0; i < patterns.size() && i < 5; ++i) {
            ss << "  " << (i + 1) << ". " << patterns[i].toString() << std::endl;
        }
        
        // Summary statistics
        double totalConfidence = 0.0;
        for (const auto& pattern : patterns) {
            totalConfidence += pattern.confidence;
        }
        
        ss << std::endl << "Pattern Summary:" << std::endl;
        ss << "  Total Patterns Detected: " << patterns.size() << std::endl;
        ss << "  Average Confidence: " << std::fixed << std::setprecision(4) << (totalConfidence / patterns.size()) << std::endl;
        
        // Pattern distribution
        std::map<std::string, int> patternCounts;
        for (const auto& pattern : patterns) {
            patternCounts[pattern.patternType]++;
        }
        
        ss << "  Pattern Distribution:" << std::endl;
        for (const auto& pair : patternCounts) {
            ss << "    " << pair.first << ": " << pair.second << std::endl;
        }
        
        return ss.str();
    }
    
private:
    PatternResult detectSinusoidalPattern(const std::vector<double>& timeSeries, double samplingRate) {
        PatternResult result;
        result.patternType = "sinusoidal";
        
        // Simplified sinusoidal detection
        size_t n = timeSeries.size();
        if (n < 4) {
            return result;
        }
        
        // Calculate autocorrelation
        std::vector<double> acf(n, 0.0);
        double mean = std::accumulate(timeSeries.begin(), timeSeries.end(), 0.0) / n;
        
        for (size_t lag = 0; lag < n; ++lag) {
            double sum = 0.0;
            for (size_t i = 0; i < n - lag; ++i) {
                sum += (timeSeries[i] - mean) * (timeSeries[i + lag] - mean);
            }
            acf[lag] = sum / (n - lag);
        }
        
        // Find peaks in autocorrelation (indicating periodicity)
        for (size_t i = 1; i < n - 1; ++i) {
            if (acf[i] > acf[i - 1] && acf[i] > acf[i + 1] && acf[i] > 0.5) {
                result.confidence = acf[i];
                result.frequency = samplingRate / i;
                result.parameters.push_back(2.0 * 3.14159 / i); // angular frequency
                result.description = "Periodic oscillation detected";
                break;
            }
        }
        
        return result;
    }
    
    PatternResult detectExponentialPattern(const std::vector<double>& timeSeries) {
        PatternResult result;
        result.patternType = "exponential";
        
        // Simple exponential detection
        size_t n = timeSeries.size();
        if (n < 3) {
            return result;
        }
        
        // Check if data follows y = a * exp(b * x)
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
        
        for (size_t i = 0; i < n; ++i) {
            double x = static_cast<double>(i);
            double y = timeSeries[i];
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
            sumY2 += y * y;
        }
        
        double numerator = (n * sumXY - sumX * sumY);
        double denominator = std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
        
        if (denominator > 1e-10) {
            result.confidence = std::abs(numerator / denominator);
            result.description = "Exponential growth/decay detected";
        }
        
        return result;
    }
    
    PatternResult detectBurstPattern(const std::vector<double>& timeSeries, double samplingRate) {
        PatternResult result;
        result.patternType = "bursts";
        
        // Burst detection
        std::vector<double> isis;
        for (size_t i = 1; i < timeSeries.size(); ++i) {
            double diff = std::abs(timeSeries[i] - timeSeries[i - 1]);
            if (diff > 0.0) {
                isis.push_back(diff);
            }
        }
        
        if (isis.empty()) return result;
        
        // Calculate coefficient of variation
        double meanISI = std::accumulate(isis.begin(), isis.end(), 0.0) / isis.size();
        double variance = 0.0;
        for (double isi : isis) {
            variance += (isi - meanISI) * (isi - meanISI);
        }
        variance /= isis.size();
        double cv = std::sqrt(variance) / (meanISI > 0 ? meanISI : 1.0);
        
        result.confidence = std::min(cv, 1.0);
        result.description = "Bursting pattern detected";
        
        return result;
    }
    
    PatternResult detectLinearPattern(const std::vector<double>& timeSeries) {
        PatternResult result;
        result.patternType = "linear";
        
        // Simple linear regression
        size_t n = timeSeries.size();
        if (n < 2) return result;
        
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
        
        for (size_t i = 0; i < n; ++i) {
            double x = static_cast<double>(i);
            double y = timeSeries[i];
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }
        
        double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        double intercept = (sumY - slope * sumX) / n;
        
        // Calculate R-squared
        double ssTotal = 0.0, ssResidual = 0.0;
        double yMean = sumY / n;
        
        for (size_t i = 0; i < n; ++i) {
            double x = static_cast<double>(i);
            double y = timeSeries[i];
            ssTotal += (y - yMean) * (y - yMean);
            ssResidual += (y - (slope * x + intercept)) * (y - (slope * x + intercept));
        }
        
        double rSquared = 1.0 - (ssResidual / (ssTotal > 0 ? ssTotal : 1.0));
        
        result.confidence = std::abs(rSquared);
        result.parameters.push_back(slope);
        result.parameters.push_back(intercept);
        result.description = "Linear trend detected";
        
        return result;
    }
    
    PatternResult analyzeSingleNeuronPattern(const std::vector<double>& spikeTimes, double samplingRate) {
        PatternResult result;
        
        if (spikeTimes.size() < 2) {
            result.patternType = "irregular";
            result.confidence = 0.0;
            return result;
        }
        
        // Calculate firing statistics
        std::vector<double> isis;
        for (size_t i = 1; i < spikeTimes.size(); ++i) {
            isis.push_back(spikeTimes[i] - spikeTimes[i - 1]);
        }
        
        double meanISI = std::accumulate(isis.begin(), isis.end(), 0.0) / isis.size();
        double variance = 0.0;
        for (double isi : isis) {
            variance += (isi - meanISI) * (isi - meanISI);
        }
        variance /= isis.size();
        double cv = std::sqrt(variance) / (meanISI > 0 ? meanISI : 1.0);
        
        // Determine pattern type based on CV
        if (cv < 0.1) {
            result.patternType = "regular";
            result.description = "Regular spiking pattern";
        } else if (cv < 0.3) {
            result.patternType = "bursting";
            result.description = "Bursting pattern with irregular intervals";
        } else if (cv < 0.5) {
            result.patternType = "irregular";
            result.description = "Irregular spiking pattern";
        } else {
            result.patternType = "complex";
            result.description = "Complex spiking pattern";
        }
        
        result.confidence = std::min(cv, 1.0);
        result.frequency = (spikeTimes.size() - 1) / spikeTimes.back();
        
        return result;
    }
    
    PatternResult analyzeNetworkPattern(const std::vector<std::vector<double>>& spikeTrains) {
        PatternResult result;
        result.patternType = "network_coordination";
        
        if (spikeTrains.empty()) {
            return result;
        }
        
        // Simple network coordination analysis
        size_t n = spikeTrains.size();
        size_t synchronizedSpikes = 0;
        
        // Check for synchronized firing (simplified)
        for (size_t i = 0; i < n; ++i) {
            if (!spikeTrains[i].empty()) {
                synchronizedSpikes++;
            }
        }
        
        result.confidence = static_cast<double>(synchronizedSpikes) / n;
        result.description = "Network coordination detected";
        
        return result;
    }
};

} // namespace nlm