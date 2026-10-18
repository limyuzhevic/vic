// performance_monitor.cpp - Implementation of PerformanceMonitor
#include "performance_monitor.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("PerformanceMonitor initialized");
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::startTimer(const std::string& name) {
    pImpl->startTimer(name);
}

void PerformanceMonitor::stopTimer(const std::string& name) {
    pImpl->stopTimer(name);
}

double PerformanceMonitor::getElapsedTime(const std::string& name) const {
    return pImpl->getElapsedTime(name);
}

void PerformanceMonitor::resetTimer(const std::string& name) {
    pImpl->resetTimer(name);
}

bool PerformanceMonitor::isTimerRunning(const std::string& name) const {
    return pImpl->isTimerRunning(name);
}

void PerformanceMonitor::trackMemoryAllocation(size_t size) {
    pImpl->trackMemoryAllocation(size);
}

void PerformanceMonitor::trackMemoryDeallocation(size_t size) {
    pImpl->trackMemoryDeallocation(size);
}

size_t PerformanceMonitor::getCurrentMemoryUsage() const {
    return pImpl->getCurrentMemoryUsage();
}

size_t PerformanceMonitor::getPeakMemoryUsage() const {
    return pImpl->getPeakMemoryUsage();
}

void PerformanceMonitor::resetMemoryTracking() {
    pImpl->resetMemoryTracking();
}

void PerformanceMonitor::startCPUMeasurement() {
    pImpl->startCPUMeasurement();
}

void PerformanceMonitor::stopCPUMeasurement() {
    pImpl->stopCPUMeasurement();
}

double PerformanceMonitor::getCPUUtilization() const {
    return pImpl->getCPUUtilization();
}

void PerformanceMonitor::trackRead(size_t bytes) {
    pImpl->trackRead(bytes);
}

void PerformanceMonitor::trackWrite(size_t bytes) {
    pImpl->trackWrite(bytes);
}

size_t PerformanceMonitor::getTotalBytesRead() const {
    return pImpl->getTotalBytesRead();
}

size_t PerformanceMonitor::getTotalBytesWritten() const {
    return pImpl->getTotalBytesWritten();
}

void PerformanceMonitor::recordEvent(const std::string& eventName) {
    pImpl->recordEvent(eventName);
}

size_t PerformanceMonitor::getEventCount(const std::string& eventName) const {
    return pImpl->getEventCount(eventName);
}

void PerformanceMonitor::generatePerformanceReport(const std::string& outputFile) const {
    pImpl->generatePerformanceReport(outputFile);
}

void PerformanceMonitor::generateBenchmarkReport(const std::string& outputFile) const {
    pImpl->generateBenchmarkReport(outputFile);
}

void PerformanceMonitor::optimizePerformance() {
    pImpl->optimizePerformance();
}

double PerformanceMonitor::getSystemCPUUsage() const {
    return pImpl->getSystemCPUUsage();
}

size_t PerformanceMonitor::getSystemMemoryUsage() const {
    return pImpl->getSystemMemoryUsage();
}

size_t PerformanceMonitor::getSystemMemoryPeak() const {
    return pImpl->getSystemMemoryPeak();
}

double PerformanceMonitor::getSystemDiskUsage() const {
    return pImpl->getSystemDiskUsage();
}

std::string PerformanceMonitor::getPerformanceStatistics() const {
    return pImpl->getPerformanceStatistics();
}

std::vector<std::string> PerformanceMonitor::getSlowTimers(double thresholdSeconds) const {
    return pImpl->getSlowTimers(thresholdSeconds);
}

std::vector<std::string> PerformanceMonitor::getMemoryLeaks() const {
    return pImpl->getMemoryLeaks();
}

void PerformanceMonitor::setSamplingInterval(double interval) {
    pImpl->setSamplingInterval(interval);
}

void PerformanceMonitor::enableRealTimeMonitoring(bool enable) {
    pImpl->enableRealTimeMonitoring(enable);
}

void PerformanceMonitor::setProfilingLevel(int level) {
    pImpl->setProfilingLevel(level);
}

void PerformanceMonitor::setThreadSafe(bool enable) {
    pImpl->setThreadSafe(enable);
}

} // namespace nlm
