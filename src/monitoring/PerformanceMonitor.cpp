// NLM Performance Monitoring System Implementation
// Real-time performance monitoring and metrics collection for the NLM artificial brain

#include "PerformanceMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <sys/resource.h>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>

namespace nlm {

// SystemResourceMonitor implementation
SystemResourceMonitor::SystemResourceMonitor()
    : monitoringInterval_(1.0), // Update every second
      monitorCPU_(true)
    , monitorMemory_(true)
    , monitorGPU_(false)
    , currentMetrics_() {
    currentMetrics_.lastUpdate = std::chrono::steady_clock::now();
}

SystemResourceMonitor::~SystemResourceMonitor() {
}

void SystemResourceMonitor::update() {
    updateCPUUsage();
    updateMemoryUsage();
    updateGPUUsage();
    updateThreadCount();
    
    currentMetrics_.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - currentMetrics_.lastUpdate).count();
    currentMetrics_.lastUpdate = std::chrono::steady_clock::now();
}

void SystemResourceMonitor::updateCPUUsage() {
    if (!monitorCPU_) return;
    
    // Simple CPU usage estimation using getrusage
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    // This is a rough approximation - actual CPU usage would require
    // more sophisticated monitoring
    currentMetrics_.cpuUsage = std::min(100.0, usage.ru_utime.tv_sec * 1000.0 + 
                                                           usage.ru_utime.tv_usec / 1000.0);
}

void SystemResourceMonitor::updateMemoryUsage() {
    if (!monitorMemory_) return;
    
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    // Estimate memory usage from ru_maxrss (in KB on Linux, bytes on macOS)
    currentMetrics_.memoryUsage = std::min(100.0, usage.ru_maxrss / 1024.0);
}

void SystemResourceMonitor::updateGPUUsage() {
    if (!monitorGPU_) return;
    
    // GPU monitoring would require NVIDIA Management Library (NVML) or
    // AMD GPU monitoring API - placeholder for now
    currentMetrics_.gpuUsage = 0.0;
}

void SystemResourceMonitor::updateThreadCount() {
    // Get current thread count
    currentMetrics_.threadCount = std::thread::hardware_concurrency();
}

// BrainPerformanceMonitor implementation
BrainPerformanceMonitor::BrainPerformanceMonitor()
    : brain_(nullptr)
    , realTimeMode_(true)
    , metricInterval_(0.1) // 100ms default
    , currentMetrics_()
    , lastMetricUpdate_(std::chrono::steady_clock::now()) {
    currentMetrics_.lastUpdate = std::chrono::steady_clock::now();
}

BrainPerformanceMonitor::~BrainPerformanceMonitor() {
}

void BrainPerformanceMonitor::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
}

void BrainPerformanceMonitor::update(double timestep, size_t step) {
    if (!brain_) return;
    
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    currentMetrics_.simulationStep = step;
    currentMetrics_.timestep = timestep;
    currentMetrics_.lastUpdate = std::chrono::steady_clock::now();
    
    // Collect basic brain metrics
    currentMetrics_.totalNeurons = brain_->getTotalNeuronCount();
    currentMetrics_.totalSynapses = brain_->getTotalSynapseCount();
    currentMetrics_.firingNeuronCount = brain_->getFiringNeuronCount();
    currentMetrics_.averageFiringRate = brain_->getAverageFiringRate();
    
    // Calculate derived metrics
    if (currentMetrics_.totalNeurons > 0) {
        currentMetrics_.synapsesPerNeuron = static_cast<double>(currentMetrics_.totalSynapses) / 
                                           currentMetrics_.totalNeurons;
    }
    
    // Update cognitive metrics
    updateCognitiveMetrics();
    
    // Check for bottlenecks
    if (realTimeMode_) {
        detectPerformanceBottlenecks();
    }
}

void BrainPerformanceMonitor::updateCognitiveMetrics() {
    if (!brain_) return;
    
    // Memory systems
    checkMemorySystems();
    
    // Neuromodulation
    checkNeuromodulation();
    
    // Prediction system
    checkPredictionSystem();
    
    // Development system
    checkDevelopmentSystem();
}

void BrainPerformanceMonitor::checkMemorySystems() {
    if (!brain_) return;
    
    auto* workingMemory = brain_->getWorkingMemory();
    auto* episodicMemory = brain_->getEpisodicMemory();
    
    if (workingMemory) {
        currentMetrics_.workingMemoryUsage = workingMemory->getActiveTraces();
    }
    
    if (episodicMemory) {
        currentMetrics_.episodicMemoryUsage = episodicMemory->getEpisodeCount();
    }
}

void BrainPerformanceMonitor::checkNeuromodulation() {
    if (!brain_) return;
    
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        currentMetrics_.dopamineLevel = dopamine->getCurrentLevel();
    }
    
    // Get other neuromodulators if available
    auto* curiosity = brain_->getCuriosity();
    if (curiosity) {
        currentMetrics_.curiosityLevel = curiosity->getCurrentLevel();
    }
    
    auto* novelty = brain_->getNovelty();
    if (novelty) {
        currentMetrics_.noveltyLevel = novelty->getCurrentLevel();
    }
}

void BrainPerformanceMonitor::checkPredictionSystem() {
    if (!brain_) return;
    
    auto* prediction = brain_->getPredictionSystem();
    if (prediction) {
        currentMetrics_.predictionAccuracy = prediction->getPredictionAccuracy();
    }
}

void BrainPerformanceMonitor::checkDevelopmentSystem() {
    if (!brain_) return;
    
    auto* development = brain_->getDevelopmentSystem();
    if (development) {
        // Get developmental stage or plasticity rate
        currentMetrics_.memoryEfficiency = development->getPlasticityRate();
    }
}

void BrainPerformanceMonitor::detectPerformanceBottlenecks() {
    currentMetrics_.detectedBottlenecks.clear();
    
    // Check memory usage
    if (currentMetrics_.brainMemoryUsage > 1024.0) { // > 1GB
        currentMetrics_.detectedBottlenecks.push_back("High memory usage (>1GB)");
        currentMetrics_.hasPerformanceIssues = true;
    }
    
    // Check synapse density
    if (currentMetrics_.synapsesPerNeuron > 100.0) {
        currentMetrics_.detectedBottlenecks.push_back("High synapse density (>100/neuron)");
        currentMetrics_.hasPerformanceIssues = true;
    }
    
    // Check firing rate
    if (currentMetrics_.averageFiringRate > 50.0) { // > 50 Hz
        currentMetrics_.detectedBottlenecks.push_back("High firing rate (>50Hz)");
        currentMetrics_.hasPerformanceIssues = true;
    }
    
    // Check memory system load
    if (currentMetrics_.workingMemoryUsage > 1000.0) {
        currentMetrics_.detectedBottlenecks.push_back("High working memory load (>1000 traces)");
        currentMetrics_.hasPerformanceIssues = true;
    }
}

void BrainPerformanceMonitor::analyzeMemoryUsage() {
    // Analyze memory usage patterns
    if (metricHistory_.size() >= 5) {
        double avgMemory = 0.0;
        for (size_t i = metricHistory_.size() - 5; i < metricHistory_.size(); ++i) {
            avgMemory += metricHistory_[i].brainMemoryUsage;
        }
        avgMemory /= 5.0;
        
        if (currentMetrics_.brainMemoryUsage > avgMemory * 1.5) {
            currentMetrics_.detectedBottlenecks.push_back("Memory usage spike detected");
        }
    }
}

void BrainPerformanceMonitor::analyzeConnectivity() {
    // Analyze connectivity patterns
    if (metricHistory_.size() >= 3) {
        double avgSynapsesPerNeuron = 0.0;
        for (size_t i = metricHistory_.size() - 3; i < metricHistory_.size(); ++i) {
            avgSynapsesPerNeuron += metricHistory_[i].synapsesPerNeuron;
        }
        avgSynapsesPerNeuron /= 3.0;
        
        if (currentMetrics_.synapsesPerNeuron > avgSynapsesPerNeuron * 1.5) {
            currentMetrics_.detectedBottlenecks.push_back("Connectivity spike detected");
        }
    }
}

void BrainPerformanceMonitor::addMetricCollector(const std::string& name, std::function<double()> collector) {
    metricCollectors_[name] = collector;
}

// ExperimentProgressTracker implementation
ExperimentProgressTracker::ExperimentProgressTracker()
    : isRunning_(false)
    , progress_(0.0)
    , currentStage_("Initialized")
    , experimentName_("Unknown") {
}

ExperimentProgressTracker::~ExperimentProgressTracker() {
}

void ExperimentProgressTracker::startExperiment(const std::string& name) {
    std::lock_guard<std::mutex> lock(progressMutex_);
    experimentName_ = name;
    isRunning_ = true;
    progress_ = 0.0;
    currentStage_ = "Initializing";
    startTime_ = std::chrono::steady_clock::now();
    metrics_.clear();
    
    NLM_LOG_INFO("Experiment started: " + name);
}

void ExperimentProgressTracker::updateProgress(double progress, const std::string& status) {
    std::lock_guard<std::mutex> lock(progressMutex_);
    progress_ = std::min(100.0, std::max(0.0, progress));
    if (!status.empty()) {
        currentStatus_ = status;
    }
    
    double elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime_).count();
    
    NLM_LOG_INFO("Experiment " + experimentName_ + ": " + std::to_string(progress_) + "% - " + currentStatus_ +
                 " (elapsed: " + std::to_string(elapsed) + "s)");
}

void ExperimentProgressTracker::setCurrentStage(const std::string& stage) {
    std::lock_guard<std::mutex> lock(progressMutex_);
    currentStage_ = stage;
}

void ExperimentProgressTracker::recordMetric(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(progressMutex_);
    metrics_.record(name, value);
}

// PerformanceAlertSystem implementation
PerformanceAlertSystem::PerformanceAlertSystem()
    : alertLog_("performance_alerts.log", std::ios::app) {
}

PerformanceAlertSystem::~PerformanceAlertSystem() {
    if (alertLog_.is_open()) {
        alertLog_.close();
    }
}

void PerformanceAlertSystem::configure(const std::string& configFile) {
    // TODO: Implement configuration file parsing
}

void PerformanceAlertSystem::checkAlerts(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(alertsMutex_);
    
    for (auto& alert : alerts_) {
        if (alert.enabled && alert.condition(metrics)) {
            logAlert(alert.name, metrics);
            alert.lastTriggered = std::chrono::steady_clock::now();
            alert.triggerCount++;
        }
    }
}

void PerformanceAlertSystem::addAlert(const std::string& name, double threshold,
                                     const std::string& severity, const std::string& message,
                                     std::function<bool(const PerformanceMetrics&)> condition) {
    Alert alert;
    alert.name = name;
    alert.threshold = threshold;
    alert.severity = severity;
    alert.message = message;
    alert.enabled = true;
    alert.condition = condition;
    alert.lastTriggered = std::chrono::steady_clock::time_point();
    alert.triggerCount = 0;
    
    alerts_.push_back(alert);
}

void PerformanceAlertSystem::enableAlert(const std::string& name, bool enable) {
    std::lock_guard<std::mutex> lock(alertsMutex_);
    
    for (auto& alert : alerts_) {
        if (alert.name == name) {
            alert.enabled = enable;
            break;
        }
    }
}

void PerformanceAlertSystem::logAlert(const std::string& name, const PerformanceMetrics& metrics) {
    std::time_t now = std::time(nullptr);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    
    std::ostringstream oss;
    oss << "[" << timestamp << "] " << metrics.timestamp << "ms "
        << "ALERT: " << name << " - " << metrics.brainMemoryUsage << "MB "
        << message_ << std::endl;
    
    alertLog_ << oss.str();
    alertLog_.flush();
}

std::vector<std::string> PerformanceAlertSystem::getActiveAlerts() const {
    std::vector<std::string> active;
    
    std::lock_guard<std::mutex> lock(alertsMutex_);
    
    for (const auto& alert : alerts_) {
        if (alert.enabled && alert.triggerCount > 0) {
            active.push_back(alert.name + " (" + alert.severity + ")");
        }
    }
    
    return active;
}

// PerformanceAnalysisEngine implementation
PerformanceAnalysisEngine::PerformanceAnalysisEngine() {
}

PerformanceAnalysisEngine::~PerformanceAnalysisEngine() {
}

void PerformanceAnalysisEngine::analyze(const PerformanceMetrics& current, const std::vector<PerformanceMetrics>& history) {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    trendAnalysis_.clear();
    bottleneckAnalysis_.clear();
    regressionAnalysis_.clear();
    
    if (history.size() < 2) return;
    
    // Analyze trends
    analyzeTrends(history);
    
    // Analyze bottlenecks
    analyzeBottlenecks(history);
    
    // Analyze regressions
    analyzeRegressions(history);
}

void PerformanceAnalysisEngine::analyzeTrends(const std::vector<PerformanceMetrics>& history) {
    // Simple trend analysis for key metrics
    std::vector<std::string> metricsToAnalyze = {
        "brainMemoryUsage", "simulationSpeed", "averageFiringRate", "workingMemoryUsage"
    };
    
    for (const auto& metricName : metricsToAnalyze) {
        std::vector<double> values;
        for (const auto& metrics : history) {
            // Use reflection to get metric values
            if (metricName == "brainMemoryUsage") {
                values.push_back(metrics.brainMemoryUsage);
            } else if (metricName == "simulationSpeed") {
                values.push_back(metrics.simulationSpeed);
            } else if (metricName == "averageFiringRate") {
                values.push_back(metrics.averageFiringRate);
            } else if (metricName == "workingMemoryUsage") {
                values.push_back(metrics.workingMemoryUsage);
            }
        }
        
        if (values.size() >= 3) {
            TrendAnalysis trend;
            trend.metricName = metricName;
            trend.currentValue = values.back();
            
            // Calculate linear trend
            double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
            for (size_t i = 0; i < values.size(); ++i) {
                double x = static_cast<double>(i);
                sumX += x;
                sumY += values[i];
                sumXY += x * values[i];
                sumX2 += x * x;
            }
            
            double n = static_cast<double>(values.size());
            double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
            double intercept = (sumY - slope * sumX) / n;
            
            trend.trend = slope;
            trend.confidence = std::min(1.0, values.size() / 10.0); // Simple confidence
            
            if (std::abs(slope) < 0.01) {
                trend.direction = "stable";
                trend.recommendation = "No action needed";
            } else if (slope > 0) {
                trend.direction = "increasing";
                if (metricName == "brainMemoryUsage") {
                    trend.recommendation = "Monitor memory usage";
                } else {
                    trend.recommendation = "Normal growth detected";
                }
            } else {
                trend.direction = "decreasing";
                trend.recommendation = "Performance improving";
            }
            
            trendAnalysis_.push_back(trend);
        }
    }
}

void PerformanceAnalysisEngine::analyzeBottlenecks(const std::vector<PerformanceMetrics>& history) {
    // Analyze historical bottlenecks
    for (const auto& metrics : history) {
        if (metrics.hasPerformanceIssues) {
            BottleneckAnalysis bottleneck;
            bottleneck.severity = 0.7; // Default severity
            
            for (const auto& bottleneckMsg : metrics.detectedBottlenecks) {
                if (bottleneckMsg.find("memory") != std::string::npos) {
                    bottleneck.component = "Memory System";
                    bottleneck.bottleneckType = "Resource Constraints";
                    bottleneck.impactScore = 0.8;
                    bottleneck.suggestedFix = "Increase memory pool size or optimize memory usage";
                    break;
                } else if (bottleneckMsg.find("firing") != std::string::npos) {
                    bottleneck.component = "Neural Processing";
                    bottleneck.bottleneckType = "Computational Load";
                    bottleneck.impactScore = 0.6;
                    bottleneck.suggestedFix = "Optimize spike processing or increase parallelism";
                    break;
                } else if (bottleneckMsg.find("connectivity") != std::string::npos) {
                    bottleneck.component = "Connectivity";
                    bottleneck.bottleneckType = "Network Structure";
                    bottleneck.impactScore = 0.5;
                    bottleneck.suggestedFix = "Review connectivity patterns or use sparse representation";
                    break;
                }
            }
            
            if (!bottleneck.component.empty()) {
                bottleneckAnalysis_.push_back(bottleneck);
            }
        }
    }
}

void PerformanceAnalysisEngine::analyzeRegressions(const std::vector<PerformanceMetrics>& history) {
    // Simple regression analysis - detect significant performance drops
    if (history.size() < 3) return;
    
    for (size_t i = 1; i < history.size(); ++i) {
        const auto& prev = history[i-1];
        const auto& curr = history[i];
        
        // Check for significant simulation speed regression
        double speedChange = (curr.simulationSpeed - prev.simulationSpeed) / prev.simulationSpeed;
        if (speedChange < -0.2) { // > 20% degradation
            RegressionAnalysis regression;
            regression.regressionType = "Performance Regression";
            regression.pValue = 0.01; // Placeholder
            regression.isSignificant = true;
            regression.effectSize = std::abs(speedChange);
            regression.description = "Simulation speed degraded by " + 
                                     std::to_string(speedChange * 100) + "%";
            regressionAnalysis_.push_back(regression);
        }
    }
}

void PerformanceAnalysisEngine::generateReport(const std::string& outputFile) {
    // Generate analysis report
    std::string report = "NLM Performance Analysis Report\n";
    report += "===============================\n\n";
    
    report += "Trend Analysis:\n";
    for (const auto& trend : trendAnalysis_) {
        report += "  " + trend.metricName + ": " + trend.direction + 
                  " (trend: " + std::to_string(trend.trend) + ")\n";
    }
    
    report += "\nBottleneck Analysis:\n";
    for (const auto& bottleneck : bottleneckAnalysis_) {
        report += "  " + bottleneck.component + ": " + bottleneck.bottleneckType +
                  " (severity: " + std::to_string(bottleneck.severity) + ")\n";
    }
    
    report += "\nRegression Analysis:\n";
    for (const auto& regression : regressionAnalysis_) {
        report += "  " + regression.description + " (p-value: " + 
                  std::to_string(regression.pValue) + ")\n";
    }
    
    if (!outputFile.empty()) {
        std::ofstream outFile(outputFile);
        if (outFile.is_open()) {
            outFile << report;
            outFile.close();
            NLM_LOG_INFO("Performance analysis report written to: " + outputFile);
        }
    }
}

void PerformanceAnalysisEngine::compareWithBaseline(const std::vector<PerformanceMetrics>& baseline) {
    // Compare current performance with baseline
    if (baseline.empty()) return;
    
    // Simple comparison
    double baselineAvgSpeed = 0.0;
    double currentSpeed = 0.0; // This would need to be passed in
    
    for (const auto& metrics : baseline) {
        baselineAvgSpeed += metrics.simulationSpeed;
    }
    baselineAvgSpeed /= baseline.size();
    
    double speedImprovement = (currentSpeed - baselineAvgSpeed) / baselineAvgSpeed * 100.0;
    
    if (speedImprovement < -5.0) {
        NLM_LOG_WARNING("Performance regression detected: " + 
                       std::to_string(speedImprovement) + "% slower than baseline");
    } else if (speedImprovement > 5.0) {
        NLM_LOG_INFO("Performance improvement: " + std::to_string(speedImprovement) + "% faster than baseline");
    } else {
        NLM_LOG_INFO("Performance within baseline range: " + std::to_string(speedImprovement) + "% change");
    }
}

// PerformanceMonitor implementation
PerformanceMonitor::PerformanceMonitor()
    : currentMetrics_()
    , monitoringActive_(false) {
}

PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
}

void PerformanceMonitor::initialize(std::shared_ptr<Config> config) {
    config_ = config;
    
    systemMonitor_ = std::make_unique<SystemResourceMonitor>();
    brainMonitor_ = std::make_unique<BrainPerformanceMonitor>();
    progressTracker_ = std::make_unique<ExperimentProgressTracker>();
    alertSystem_ = std::make_unique<PerformanceAlertSystem>();
    analysisEngine_ = std::make_unique<PerformanceAnalysisEngine>();
    
    // Configure alert system with default alerts
    alertSystem_->addAlert("memory_usage", 1024.0, "WARNING",
        "High memory usage detected", [this](const PerformanceMetrics& m) { return m.brainMemoryUsage > 1024.0; });
    
    alertSystem_->addAlert("simulation_speed", 100.0, "ERROR",
        "Slow simulation speed detected", [this](const PerformanceMetrics& m) { return m.simulationSpeed < 100.0; });
    
    alertSystem_->addAlert("memory_pressure", 80.0, "WARNING",
        "High memory pressure", [this](const PerformanceMetrics& m) { return m.memoryUsage > 80.0; });
}

void PerformanceMonitor::startMonitoring() {
    if (monitoringActive_) return;
    
    monitoringActive_ = true;
    monitoringThread_ = std::thread(&PerformanceMonitor::monitoringLoop, this);
    
    NLM_LOG_INFO("Performance monitoring started");
}

void PerformanceMonitor::stopMonitoring() {
    if (!monitoringActive_) return;
    
    monitoringActive_ = false;
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    
    NLM_LOG_INFO("Performance monitoring stopped");
}

void PerformanceMonitor::updateBrainMetrics(std::shared_ptr<Brain> brain, double timestep, size_t step) {
    brainMonitor_->update(timestep, step);
    currentMetrics_ = brainMonitor_->getCurrentMetrics();
}

void PerformanceMonitor::updateSystemMetrics() {
    systemMonitor_->update();
}

void PerformanceMonitor::startExperiment(const std::string& name) {
    progressTracker_->startExperiment(name);
}

void PerformanceMonitor::updateExperimentProgress(double progress, const std::string& status) {
    progressTracker_->updateProgress(progress, status);
}

void PerformanceMonitor::checkAlerts() {
    alertSystem_->checkAlerts(currentMetrics_);
}

void PerformanceMonitor::performAnalysis() {
    // Store current metrics in history
    // TODO: Add current metrics to analysis engine history
    analysisEngine_->analyze(currentMetrics_, {}); // TODO: Pass history
}

void PerformanceMonitor::generateReports(const std::string& outputDirectory) {
    std::filesystem::create_directories(outputDirectory);
    
    std::string metricsFile = outputDirectory + "/performance_metrics.txt";
    std::string analysisFile = outputDirectory + "/performance_analysis.txt";
    std::string alertsFile = outputDirectory + "/performance_alerts.txt";
    
    // Write metrics report
    std::ofstream metricsOut(metricsFile);
    if (metricsOut.is_open()) {
        metricsOut << "NLM Performance Metrics\n";
        metricsOut << "======================\n\n";
        metricsOut << "Simulation Step: " << currentMetrics_.simulationStep << "\n";
        metricsOut << "Wall Clock Time: " << currentMetrics_.wallClockTime << "s\n";
        metricsOut << "Brain Memory Usage: " << currentMetrics_.brainMemoryUsage << " MB\n";
        metricsOut << "Total Neurons: " << currentMetrics_.totalNeurons << "\n";
        metricsOut << "Total Synapses: " << currentMetrics_.totalSynapses << "\n";
        metricsOut << "Firing Neurons: " << currentMetrics_.firingNeuronCount << "\n";
        metricsOut << "Average Firing Rate: " << currentMetrics_.averageFiringRate << " Hz\n";
        metricsOut << "Simulation Speed: " << currentMetrics_.simulationSpeed << " steps/sec\n";
        metricsOut << "Memory Usage: " << currentMetrics_.memoryUsage << "%\n";
        metricsOut << "CPU Usage: " << currentMetrics_.cpuUsage << "%\n";
        metricsOut << "Working Memory Usage: " << currentMetrics_.workingMemoryUsage << "\n";
        metricsOut << "Episodic Memory Usage: " << currentMetrics_.episodicMemoryUsage << "\n";
        metricsOut << "Dopamine Level: " << currentMetrics_.dopamineLevel << "\n";
        metricsOut << "Novelty Level: " << currentMetrics_.noveltyLevel << "\n";
        metricsOut << "Curiosity Level: " << currentMetrics_.curiosityLevel << "\n";
        metricsOut << "Prediction Accuracy: " << currentMetrics_.predictionAccuracy << "\n";
        
        metricsOut.close();
    }
    
    // Generate analysis report
    analysisEngine_->generateReport(analysisFile);
    
    // Write alerts
    std::ofstream alertsOut(alertsFile);
    if (alertsOut.is_open()) {
        auto activeAlerts = alertSystem_->getActiveAlerts();
        alertsOut << "Active Performance Alerts:\n";
        alertsOut << "===========================\n\n";
        for (const auto& alert : activeAlerts) {
            alertsOut << "- " << alert << "\n";
        }
        alertsOut.close();
    }
    
    NLM_LOG_INFO("Performance reports generated in: " + outputDirectory);
}

void PerformanceMonitor::configure(const std::string& configFile) {
    alertSystem_->configure(configFile);
}

void PerformanceMonitor::monitoringLoop() {
    while (monitoringActive_) {
        std::unique_lock<std::mutex> lock(monitorMutex_);
        
        updateSystemMetrics();
        checkAlerts();
        performAnalysis();
        
        lock.unlock();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(metricInterval_ * 1000)));
    }
}

} // namespace nlm
