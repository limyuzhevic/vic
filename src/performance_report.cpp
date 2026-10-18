// performance_report.cpp - Implementation of PerformanceReportGenerator
#include "performance_report.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

PerformanceReportGenerator::PerformanceReportGenerator() {
    NLM_LOG_INFO("PerformanceReportGenerator initialized");
}

PerformanceReportGenerator::~PerformanceReportGenerator() = default;

void PerformanceReportGenerator::addMetric(const std::string& name, double value, const std::string& unit,
                                          const std::string& category, const std::string& operation) {
    pImpl->addMetric(name, value, unit, category, operation);
}

void PerformanceReportGenerator::addTimeMeasurement(const std::string& operation, double time) {
    pImpl->addTimeMeasurement(operation, time);
}

void PerformanceReportGenerator::addMemoryMeasurement(const std::string& operation, size_t memory) {
    pImpl->addMemoryMeasurement(operation, memory);
}

void PerformanceReportGenerator::addCPUMeasurement(const std::string& operation, double cpuTime) {
    pImpl->addCPUMeasurement(operation, cpuTime);
}

void PerformanceReportGenerator::addEventCount(const std::string& eventName, size_t count) {
    pImpl->addEventCount(eventName, count);
}

double PerformanceReportGenerator::calculateMean(const std::string& metricName) const {
    return pImpl->calculateMean(metricName);
}

double PerformanceReportGenerator::calculateStandardDeviation(const std::string& metricName) const {
    return pImpl->calculateStandardDeviation(metricName);
}

double PerformanceReportGenerator::calculateMedian(const std::string& metricName) const {
    return pImpl->calculateMedian(metricName);
}

double PerformanceReportGenerator::calculatePercentile(const std::string& metricName, double percentile) const {
    return pImpl->calculatePercentile(metricName, percentile);
}

void PerformanceReportGenerator::generateTextReport(std::ostream& output) const {
    pImpl->generateTextReport(output);
}

void PerformanceReportGenerator::generateJSONReport(std::ostream& output) const {
    pImpl->generateJSONReport(output);
}

void PerformanceReportGenerator::generateHTMLReport(std::ostream& output) const {
    pImpl->generateHTMLReport(output);
}

void PerformanceReportGenerator::generateCSVReport(std::ostream& output) const {
    pImpl->generateCSVReport(output);
}

bool PerformanceReportGenerator::generateTextReport(const std::string& filename) const {
    return pImpl->generateTextReport(filename);
}

bool PerformanceReportGenerator::generateJSONReport(const std::string& filename) const {
    return pImpl->generateJSONReport(filename);
}

bool PerformanceReportGenerator::generateHTMLReport(const std::string& filename) const {
    return pImpl->generateHTMLReport(filename);
}

bool PerformanceReportGenerator::generateCSVReport(const std::string& filename) const {
    return pImpl->generateCSVReport(filename);
}

std::vector<std::string> PerformanceReportGenerator::identifyPerformanceTrends() const {
    return pImpl->identifyPerformanceTrends();
}

std::vector<std::string> PerformanceReportGenerator::detectAnomalies() const {
    return pImpl->detectAnomalies();
}

std::vector<std::string> PerformanceReportGenerator::generateOptimizationRecommendations() const {
    return pImpl->generateOptimizationRecommendations();
}

std::string PerformanceReportGenerator::getPerformanceSummary() const {
    return pImpl->getPerformanceSummary();
}

void PerformanceReportGenerator::setTitle(const std::string& title) {
    pImpl->title = title;
}

void PerformanceReportGenerator::setDescription(const std::string& description) {
    pImpl->description = description;
}

} // namespace nlm
