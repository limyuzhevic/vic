// Performance Profiling Tools Implementation
// Comprehensive performance profiling tools for NLM

#include "PerformanceProfiler.hpp"
#include <fstream>
#include <sstream>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>

namespace nlm {
namespace plugins {

// ScriptExecutionProfiler implementation
ScriptExecutionProfiler::ScriptExecutionProfiler() : pImpl(std::make_unique<Impl>()) {
    // Initialize execution profiler
}

ScriptExecutionProfiler::~ScriptExecutionProfiler() {
    // Cleanup
}

bool ScriptExecutionProfiler::startProfiling(const std::string& scriptId) {
    return pImpl->startProfiling(scriptId);
}

bool ScriptExecutionProfiler::stopProfiling(const std::string& scriptId) {
    return pImpl->stopProfiling(scriptId);
}

bool ScriptExecutionProfiler::pauseProfiling(const std::string& scriptId) {
    return pImpl->pauseProfiling(scriptId);
}

bool ScriptExecutionProfiler::resumeProfiling(const std::string& scriptId) {
    return pImpl->resumeProfiling(scriptId);
}

json ScriptExecutionProfiler::getProfilingData(const std::string& scriptId) const {
    return pImpl->getProfilingData(scriptId);
}

std::vector<json> ScriptExecutionProfiler::getProfilingHistory(const std::string& scriptId) const {
    return pImpl->getProfilingHistory(scriptId);
}

bool ScriptExecutionProfiler::clearProfilingData(const std::string& scriptId) {
    return pImpl->clearProfilingData(scriptId);
}

PerformanceAnalysis ScriptExecutionProfiler::analyzePerformance(const std::string& scriptId) const {
    return pImpl->analyzePerformance(scriptId);
}

json ScriptExecutionProfiler::getPerformanceMetrics(const std::string& scriptId) const {
    return pImpl->getPerformanceMetrics(scriptId);
}

std::vector<json> ScriptExecutionProfiler::getPerformanceHistory(const std::string& scriptId) const {
    return pImpl->getPerformanceHistory(scriptId);
}

std::vector<PerformanceAlert> ScriptExecutionProfiler::getPerformanceAlerts(const std::string& scriptId) const {
    return pImpl->getPerformanceAlerts(scriptId);
}

bool ScriptExecutionProfiler::resolvePerformanceAlert(const std::string& scriptId, const std::string& alertId) {
    return pImpl->resolvePerformanceAlert(scriptId, alertId);
}

bool ScriptExecutionProfiler::createPerformanceAlert(const PerformanceAlert& alert) {
    return pImpl->createPerformanceAlert(alert);
}

std::vector<PerformanceTrend> ScriptExecutionProfiler::analyzePerformanceTrends(const std::string& scriptId) const {
    return pImpl->analyzePerformanceTrends(scriptId);
}

std::vector<PerformancePrediction> ScriptExecutionProfiler::predictPerformance(const std::string& scriptId) const {
    return pImpl->predictPerformance(scriptId);
}

std::vector<std::string> ScriptExecutionProfiler::getOptimizationRecommendations(const std::string& scriptId) const {
    return pImpl->getOptimizationRecommendations(scriptId);
}

bool ScriptExecutionProfiler::applyOptimization(const std::string& scriptId,
                                               const std::string& optimization) {
    return pImpl->applyOptimization(scriptId, optimization);
}

bool ScriptExecutionProfiler::revertOptimization(const std::string& scriptId,
                                                 const std::string& optimization) {
    return pImpl->revertOptimization(scriptId, optimization);
}

bool ScriptExecutionProfiler::setProfilingMode(ProfilingMode mode) {
    return pImpl->setProfilingMode(mode);
}

ProfilingMode ScriptExecutionProfiler::getProfilingMode() const {
    return pImpl->getProfilingMode();
}

bool ScriptExecutionProfiler::setSamplingRate(double rate) {
    return pImpl->setSamplingRate(rate);
}

double ScriptExecutionProfiler::getSamplingRate() const {
    return pImpl->getSamplingRate();
}

bool ScriptExecutionProfiler::setMaxMemoryUsage(size_t maxMemory) {
    return pImpl->setMaxMemoryUsage(maxMemory);
}

size_t ScriptExecutionProfiler::getMaxMemoryUsage() const {
    return pImpl->getMaxMemoryUsage();
}

bool ScriptExecutionProfiler::setAlertThresholds(const std::map<PerformanceMetric, double>& thresholds) {
    return pImpl->setAlertThresholds(thresholds);
}

std::map<PerformanceMetric, double> ScriptExecutionProfiler::getAlertThresholds() const {
    return pImpl->getAlertThresholds();
}

bool ScriptExecutionProfiler::configure(const json& config) {
    return pImpl->configure(config);
}

json ScriptExecutionProfiler::getConfiguration() const {
    return pImpl->getConfiguration();
}

bool ScriptExecutionProfiler::start() {
    return pImpl->start();
}

bool ScriptExecutionProfiler::stop() {
    return pImpl->stop();
}

bool ScriptExecutionProfiler::isRunning() const {
    return pImpl->isRunning();
}

bool ScriptExecutionProfiler::profileExecution(const std::string& scriptId,
                                               const std::function<json()>& scriptFunction) {
    return pImpl->profileExecution(scriptId, scriptFunction);
}

bool ScriptExecutionProfiler::profileScriptFile(const std::string& scriptId,
                                                const std::string& filepath) {
    return pImpl->profileScriptFile(scriptId, filepath);
}

json ScriptExecutionProfiler::getExecutionProfile(const std::string& scriptId) const {
    return pImpl->getExecutionProfile(scriptId);
}

bool ScriptExecutionProfiler::enableMemoryProfiling(bool enabled) {
    return pImpl->enableMemoryProfiling(enabled);
}

bool ScriptExecutionProfiler::isMemoryProfilingEnabled() const {
    return pImpl->isMemoryProfilingEnabled();
}

json ScriptExecutionProfiler::getMemoryProfile(const std::string& scriptId) const {
    return pImpl->getMemoryProfile(scriptId);
}

bool ScriptExecutionProfiler::enableCpuProfiling(bool enabled) {
    return pImpl->enableCpuProfiling(enabled);
}

bool ScriptExecutionProfiler::isCpuProfilingEnabled() const {
    return pImpl->isCpuProfilingEnabled();
}

json ScriptExecutionProfiler::getCpuProfile(const std::string& scriptId) const {
    return pImpl->getCpuProfile(scriptId);
}

bool ScriptExecutionProfiler::enableIoProfiling(bool enabled) {
    return pImpl->enableIoProfiling(enabled);
}

bool ScriptExecutionProfiler::isIoProfilingEnabled() const {
    return pImpl->isIoProfilingEnabled();
}

json ScriptExecutionProfiler::getIoProfile(const std::string& scriptId) const {
    return pImpl->getIoProfile(scriptId);
}

bool ScriptExecutionProfiler::enableNetworkProfiling(bool enabled) {
    return pImpl->enableNetworkProfiling(enabled);
}

bool ScriptExecutionProfiler::isNetworkProfilingEnabled() const {
    return pImpl->isNetworkProfilingEnabled();
}

json ScriptExecutionProfiler::getNetworkProfile(const std::string& scriptId) const {
    return pImpl->getNetworkProfile(scriptId);
}

bool ScriptExecutionProfiler::enableThreadProfiling(bool enabled) {
    return pImpl->enableThreadProfiling(enabled);
}

bool ScriptExecutionProfiler::isThreadProfilingEnabled() const {
    return pImpl->isThreadProfilingEnabled();
}

json ScriptExecutionProfiler::getThreadProfile(const std::string& scriptId) const {
    return pImpl->getThreadProfile(scriptId);
}

bool ScriptExecutionProfiler::enableErrorProfiling(bool enabled) {
    return pImpl->enableErrorProfiling(enabled);
}

bool ScriptExecutionProfiler::isErrorProfilingEnabled() const {
    return pImpl->isErrorProfilingEnabled();
}

json ScriptExecutionProfiler::getErrorProfile(const std::string& scriptId) const {
    return pImpl->getErrorProfile(scriptId);
}

// SystemPerformanceProfiler implementation
SystemPerformanceProfiler::SystemPerformanceProfiler() : pImpl(std::make_unique<Impl>()) {
    // Initialize system profiler
}

SystemPerformanceProfiler::~SystemPerformanceProfiler() {
    // Cleanup
}

bool SystemPerformanceProfiler::startProfiling(const std::string& scriptId) {
    return pImpl->startProfiling(scriptId);
}

bool SystemPerformanceProfiler::stopProfiling(const std::string& scriptId) {
    return pImpl->stopProfiling(scriptId);
}

bool SystemPerformanceProfiler::pauseProfiling(const std::string& scriptId) {
    return pImpl->pauseProfiling(scriptId);
}

bool SystemPerformanceProfiler::resumeProfiling(const std::string& scriptId) {
    return pImpl->resumeProfiling(scriptId);
}

json SystemPerformanceProfiler::getProfilingData(const std::string& scriptId) const {
    return pImpl->getProfilingData(scriptId);
}

std::vector<json> SystemPerformanceProfiler::getProfilingHistory(const std::string& scriptId) const {
    return pImpl->getProfilingHistory(scriptId);
}

bool SystemPerformanceProfiler::clearProfilingData(const std::string& scriptId) {
    return pImpl->clearProfilingData(scriptId);
}

PerformanceAnalysis SystemPerformanceProfiler::analyzePerformance(const std::string& scriptId) const {
    return pImpl->analyzePerformance(scriptId);
}

json SystemPerformanceProfiler::getPerformanceMetrics(const std::string& scriptId) const {
    return pImpl->getPerformanceMetrics(scriptId);
}

std::vector<json> SystemPerformanceProfiler::getPerformanceHistory(const std::string& scriptId) const {
    return pImpl->getPerformanceHistory(scriptId);
}

std::vector<PerformanceAlert> SystemPerformanceProfiler::getPerformanceAlerts(const std::string& scriptId) const {
    return pImpl->getPerformanceAlerts(scriptId);
}

bool SystemPerformanceProfiler::resolvePerformanceAlert(const std::string& scriptId, const std::string& alertId) {
    return pImpl->resolvePerformanceAlert(scriptId, alertId);
}

bool SystemPerformanceProfiler::createPerformanceAlert(const PerformanceAlert& alert) {
    return pImpl->createPerformanceAlert(alert);
}

std::vector<PerformanceTrend> SystemPerformanceProfiler::analyzePerformanceTrends(const std::string& scriptId) const {
    return pImpl->analyzePerformanceTrends(scriptId);
}

std::vector<PerformancePrediction> SystemPerformanceProfiler::predictPerformance(const std::string& scriptId) const {
    return pImpl->predictPerformance(scriptId);
}

std::vector<std::string> SystemPerformanceProfiler::getOptimizationRecommendations(const std::string& scriptId) const {
    return pImpl->getOptimizationRecommendations(scriptId);
}

bool SystemPerformanceProfiler::applyOptimization(const std::string& scriptId,
                                                 const std::string& optimization) {
    return pImpl->applyOptimization(scriptId, optimization);
}

bool SystemPerformanceProfiler::revertOptimization(const std::string& scriptId,
                                                   const std::string& optimization) {
    return pImpl->revertOptimization(scriptId, optimization);
}

bool SystemPerformanceProfiler::setProfilingMode(ProfilingMode mode) {
    return pImpl->setProfilingMode(mode);
}

ProfilingMode SystemPerformanceProfiler::getProfilingMode() const {
    return pImpl->getProfilingMode();
}

bool SystemPerformanceProfiler::setSamplingRate(double rate) {
    return pImpl->setSamplingRate(rate);
}

double SystemPerformanceProfiler::getSamplingRate() const {
    return pImpl->getSamplingRate();
}

bool SystemPerformanceProfiler::setMaxMemoryUsage(size_t maxMemory) {
    return pImpl->setMaxMemoryUsage(maxMemory);
}

size_t SystemPerformanceProfiler::getMaxMemoryUsage() const {
    return pImpl->getMaxMemoryUsage();
}

bool SystemPerformanceProfiler::setAlertThresholds(const std::map<PerformanceMetric, double>& thresholds) {
    return pImpl->setAlertThresholds(thresholds);
}

std::map<PerformanceMetric, double> SystemPerformanceProfiler::getAlertThresholds() const {
    return pImpl->getAlertThresholds();
}

bool SystemPerformanceProfiler::configure(const json& config) {
    return pImpl->configure(config);
}

json SystemPerformanceProfiler::getConfiguration() const {
    return pImpl->getConfiguration();
}

bool SystemPerformanceProfiler::start() {
    return pImpl->start();
}

bool SystemPerformanceProfiler::stop() {
    return pImpl->stop();
}

bool SystemPerformanceProfiler::isRunning() const {
    return pImpl->isRunning();
}

json SystemPerformanceProfiler::profileSystem(const json& parameters) const {
    return pImpl->profileSystem(parameters);
}

json SystemPerformanceProfiler::getSystemMetrics() const {
    return pImpl->getSystemMetrics();
}

json SystemPerformanceProfiler::profileNetworkUsage() const {
    return pImpl->profileNetworkUsage();
}

json SystemPerformanceProfiler::profileDiskUsage() const {
    return pImpl->profileDiskUsage();
}

json SystemPerformanceProfiler::profileMemoryUsage() const {
    return pImpl->profileMemoryUsage();
}

json SystemPerformanceProfiler::profileCpuUsage() const {
    return pImpl->profileCpuUsage();
}

bool SystemPerformanceProfiler::enableResourceMonitoring(bool enabled) {
    return pImpl->enableResourceMonitoring(enabled);
}

bool SystemPerformanceProfiler::isResourceMonitoringEnabled() const {
    return pImpl->isResourceMonitoringEnabled();
}

json SystemPerformanceProfiler::getResourceUsage() const {
    return pImpl->getResourceUsage();
}

json SystemPerformanceProfiler::getResourceHistory() const {
    return pImpl->getResourceHistory();
}

json SystemPerformanceProfiler::analyzeSystemPerformance() const {
    return pImpl->analyzeSystemPerformance();
}

json SystemPerformanceProfiler::detectPerformanceBottlenecks() const {
    return pImpl->detectPerformanceBottlenecks();
}

json SystemPerformanceProfiler::getSystemRecommendations() const {
    return pImpl->getSystemRecommendations();
}

// ComparativePerformanceProfiler implementation
ComparativePerformanceProfiler::ComparativePerformanceProfiler(std::shared_ptr<IScriptProfiler> baseProfiler)
    : baseProfiler_(baseProfiler), pImpl(std::make_unique<Impl>()) {
    // Initialize comparative profiler
}

ComparativePerformanceProfiler::~ComparativePerformanceProfiler() {
    // Cleanup
}

bool ComparativePerformanceProfiler::startProfiling(const std::string& scriptId) {
    return pImpl->startProfiling(scriptId);
}

bool ComparativePerformanceProfiler::stopProfiling(const std::string& scriptId) {
    return pImpl->stopProfiling(scriptId);
}

bool ComparativePerformanceProfiler::pauseProfiling(const std::string& scriptId) {
    return pImpl->pauseProfiling(scriptId);
}

bool ComparativePerformanceProfiler::resumeProfiling(const std::string& scriptId) {
    return pImpl->resumeProfiling(scriptId);
}

json ComparativePerformanceProfiler::getProfilingData(const std::string& scriptId) const {
    return pImpl->getProfilingData(scriptId);
}

std::vector<json> ComparativePerformanceProfiler::getProfilingHistory(const std::string& scriptId) const {
    return pImpl->getProfilingHistory(scriptId);
}

bool ComparativePerformanceProfiler::clearProfilingData(const std::string& scriptId) {
    return pImpl->clearProfilingData(scriptId);
}

PerformanceAnalysis ComparativePerformanceProfiler::analyzePerformance(const std::string& scriptId) const {
    return pImpl->analyzePerformance(scriptId);
}

json ComparativePerformanceProfiler::getPerformanceMetrics(const std::string& scriptId) const {
    return pImpl->getPerformanceMetrics(scriptId);
}

std::vector<json> ComparativePerformanceProfiler::getPerformanceHistory(const std::string& scriptId) const {
    return pImpl->getPerformanceHistory(scriptId);
}

std::vector<PerformanceAlert> ComparativePerformanceProfiler::getPerformanceAlerts(const std::string& scriptId) const {
    return pImpl->getPerformanceAlerts(scriptId);
}

bool ComparativePerformanceProfiler::resolvePerformanceAlert(const std::string& scriptId, const std::string& alertId) {
    return pImpl->resolvePerformanceAlert(scriptId, alertId);
}

bool ComparativePerformanceProfiler::createPerformanceAlert(const PerformanceAlert& alert) {
    return pImpl->createPerformanceAlert(alert);
}

std::vector<PerformanceTrend> ComparativePerformanceProfiler::analyzePerformanceTrends(const std::string& scriptId) const {
    return pImpl->analyzePerformanceTrends(scriptId);
}

std::vector<PerformancePrediction> ComparativePerformanceProfiler::predictPerformance(const std::string& scriptId) const {
    return pImpl->predictPerformance(scriptId);
}

std::vector<std::string> ComparativePerformanceProfiler::getOptimizationRecommendations(const std::string& scriptId) const {
    return pImpl->getOptimizationRecommendations(scriptId);
}

bool ComparativePerformanceProfiler::applyOptimization(const std::string& scriptId,
                                                     const std::string& optimization) {
    return pImpl->applyOptimization(scriptId, optimization);
}

bool ComparativePerformanceProfiler::revertOptimization(const std::string& scriptId,
                                                       const std::string& optimization) {
    return pImpl->revertOptimization(scriptId, optimization);
}

bool ComparativePerformanceProfiler::setProfilingMode(ProfilingMode mode) {
    return pImpl->setProfilingMode(mode);
}

ProfilingMode ComparativePerformanceProfiler::getProfilingMode() const {
    return pImpl->getProfilingMode();
}

bool ComparativePerformanceProfiler::setSamplingRate(double rate) {
    return pImpl->setSamplingRate(rate);
}

double ComparativePerformanceProfiler::getSamplingRate() const {
    return pImpl->getSamplingRate();
}

bool ComparativePerformanceProfiler::setMaxMemoryUsage(size_t maxMemory) {
    return pImpl->setMaxMemoryUsage(maxMemory);
}

size_t ComparativePerformanceProfiler::getMaxMemoryUsage() const {
    return pImpl->getMaxMemoryUsage();
}

bool ComparativePerformanceProfiler::setAlertThresholds(const std::map<PerformanceMetric, double>& thresholds) {
    return pImpl->setAlertThresholds(thresholds);
}

std::map<PerformanceMetric, double> ComparativePerformanceProfiler::getAlertThresholds() const {
    return pImpl->getAlertThresholds();
}

bool ComparativePerformanceProfiler::configure(const json& config) {
    return pImpl->configure(config);
}

json ComparativePerformanceProfiler::getConfiguration() const {
    return pImpl->getConfiguration();
}

bool ComparativePerformanceProfiler::start() {
    return pImpl->start();
}

bool ComparativePerformanceProfiler::stop() {
    return pImpl->stop();
}

bool ComparativePerformanceProfiler::isRunning() const {
    return pImpl->isRunning();
}

bool ComparativePerformanceProfiler::addBaseline(const std::string& baselineId,
                                               const std::string& baselineScriptId) {
    return pImpl->addBaseline(baselineId, baselineScriptId);
}

bool ComparativePerformanceProfiler::removeBaseline(const std::string& baselineId) {
    return pImpl->removeBaseline(baselineId);
}

bool ComparativePerformanceProfiler::setBaseline(const std::string& baselineId) {
    return pImpl->setBaseline(baselineId);
}

std::string ComparativePerformanceProfiler::getBaseline() const {
    return pImpl->getBaseline();
}

json ComparativePerformanceProfiler::compareWithBaseline(const std::string& scriptId) const {
    return pImpl->compareWithBaseline(scriptId);
}

json ComparativePerformanceProfiler::compareWithMultipleBaselines(const std::string& scriptId) const {
    return pImpl->compareWithMultipleBaselines(scriptId);
}

json ComparativePerformanceProfiler::getPerformanceImprovement(const std::string& scriptId,
                                                           const std::string& baselineId) const {
    return pImpl->getPerformanceImprovement(scriptId, baselineId);
}

bool ComparativePerformanceProfiler::addBenchmark(const std::string& benchmarkId,
                                                 const json& benchmarkConfig) {
    return pImpl->addBenchmark(benchmarkId, benchmarkConfig);
}

bool ComparativePerformanceProfiler::removeBenchmark(const std::string& benchmarkId) {
    return pImpl->removeBenchmark(benchmarkId);
}

json ComparativePerformanceProfiler::runBenchmark(const std::string& benchmarkId) const {
    return pImpl->runBenchmark(benchmarkId);
}

json ComparativePerformanceProfiler::getBenchmarkResults(const std::string& benchmarkId) const {
    return pImpl->getBenchmarkResults(benchmarkId);
}

json ComparativePerformanceProfiler::analyzePerformanceComparison(const std::string& scriptId) const {
    return pImpl->analyzePerformanceComparison(scriptId);
}

json ComparativePerformanceProfiler::getPerformanceRanking(const std::string& scriptId) const {
    return pImpl->getPerformanceRanking(scriptId);
}

json ComparativePerformanceProfiler::generatePerformanceReport(const std::string& scriptId) const {
    return pImpl->generatePerformanceReport(scriptId);
}

// PerformanceProfilerFactory implementation
std::unordered_map<std::string, std::function<std::shared_ptr<IScriptProfiler>()>> PerformanceProfilerFactory::profilerCreators_;
std::mutex PerformanceProfilerFactory::profilerMutex_;

std::shared_ptr<IScriptProfiler> PerformanceProfilerFactory::createProfiler(const std::string& profilerType) {
    std::lock_guard<std::mutex> lock(profilerMutex_);
    
    auto it = profilerCreators_.find(profilerType);
    if (it != profilerCreators_.end()) {
        return it->second();
    }
    
    return nullptr;
}

bool PerformanceProfilerFactory::registerProfiler(const std::string& profilerType,
                                                 std::function<std::shared_ptr<IScriptProfiler>()> creator) {
    std::lock_guard<std::mutex> lock(profilerMutex_);
    profilerCreators_[profilerType] = creator;
    return true;
}

bool PerformanceProfilerFactory::unregisterProfiler(const std::string& profilerType) {
    std::lock_guard<std::mutex> lock(profilerMutex_);
    
    auto it = profilerCreators_.find(profilerType);
    if (it != profilerCreators_.end()) {
        profilerCreators_.erase(it);
        return true;
    }
    return false;
}

std::vector<std::string> PerformanceProfilerFactory::getRegisteredProfilers() {
    std::lock_guard<std::mutex> lock(profilerMutex_);
    
    std::vector<std::string> result;
    for (const auto& pair : profilerCreators_) {
        result.push_back(pair.first);
    }
    return result;
}

std::string PerformanceProfilerFactory::getProfilerName(const std::string& profilerType) {
    if (profilerType == "execution") return "Script Execution Profiler";
    if (profilerType == "system") return "System Performance Profiler";
    if (profilerType == "comparative") return "Comparative Performance Profiler";
    return "Unknown";
}

std::string PerformanceProfilerFactory::getProfilerVersion(const std::string& profilerType) {
    return "1.0.0";
}

std::vector<std::string> PerformanceProfilerFactory::getProfilerCapabilities(const std::string& profilerType) {
    // In real implementation, would return profiler capabilities
    return std::vector<std::string>{"Profiling", "Analysis", "Reporting"};
}

bool PerformanceProfilerFactory::profilerSupportsFeature(const std::string& profilerType, const std::string& feature) {
    // In real implementation, would check if profiler supports the feature
    return true;
}

bool PerformanceProfilerFactory::configureProfiler(const std::string& profilerType, const json& config) {
    // In real implementation, would configure the profiler
    return true;
}

json PerformanceProfilerFactory::getProfilerConfig(const std::string& profilerType) {
    // In real implementation, would return profiler configuration
    return json::object();
}

bool PerformanceProfilerFactory::initializeAllProfilers(const json& config) {
    bool allSuccess = true;
    for (const auto& profilerType : getRegisteredProfilers()) {
        if (!configureProfiler(profilerType, config)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

bool PerformanceProfilerFactory::shutdownAllProfilers() {
    bool allSuccess = true;
    for (const auto& profilerType : getRegisteredProfilers()) {
        auto profiler = createProfiler(profilerType);
        if (profiler && !profiler->shutdown()) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

bool PerformanceProfilerFactory::reconfigureAllProfilers(const json& config) {
    bool allSuccess = true;
    for (const auto& profilerType : getRegisteredProfilers()) {
        if (!configureProfiler(profilerType, config)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

std::vector<json> PerformanceProfilerFactory::getAllProfilerStatuses() const {
    std::vector<json> result;
    for (const auto& profilerType : getRegisteredProfilers()) {
        result.push_back(getProfilerStatus(profilerType));
    }
    return result;
}

json PerformanceProfilerFactory::getProfilerStatus(const std::string& profilerType) const {
    json j;
    j["profilerType"] = profilerType;
    j["name"] = getProfilerName(profilerType);
    j["version"] = getProfilerVersion(profilerType);
    j["active"] = true;
    j["capabilities"] = getProfilerCapabilities(profilerType);
    return j;
}

} // namespace plugins
} // namespace nlm