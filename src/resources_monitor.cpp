// resources_monitor.cpp - Implementation of ResourceMonitor
#include "resources_monitor.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

ResourceMonitor::ResourceMonitor() : isMonitoring_(false) {
    NLM_LOG_INFO("ResourceMonitor initialized");
}

ResourceMonitor::~ResourceMonitor() {
    stopMonitoring();
}

void ResourceMonitor::startMonitoring() {
    pImpl->startMonitoring();
}

void ResourceMonitor::stopMonitoring() {
    pImpl->stopMonitoring();
}

ResourceMonitor::ResourceUsage ResourceMonitor::getCurrentResourceUsage() const {
    return pImpl->getCurrentResourceUsage();
}

std::vector<ResourceMonitor::ResourceUsage> ResourceMonitor::getResourceUsageHistory() const {
    return pImpl->getResourceUsageHistory();
}

ResourceMonitor::ResourceUsage ResourceMonitor::getAverageResourceUsage() const {
    return pImpl->getAverageResourceUsage();
}

double ResourceMonitor::getCPUUsage() const {
    return pImpl->getCPUUsage();
}

size_t ResourceMonitor::getMemoryUsage() const {
    return pImpl->getMemoryUsage();
}

size_t ResourceMonitor::getMemoryPeak() const {
    return pImpl->getMemoryPeak();
}

double ResourceMonitor::getDiskUsage() const {
    return pImpl->getDiskUsage();
}

double ResourceMonitor::getNetworkUsage() const {
    return pImpl->getNetworkUsage();
}

double ResourceMonitor::getTemperature() const {
    return pImpl->getTemperature();
}

void ResourceMonitor::clearHistory() {
    pImpl->clearHistory();
}

void ResourceMonitor::setHistorySize(size_t size) {
    pImpl->setHistorySize(size);
}

void ResourceMonitor::setSamplingInterval(double interval) {
    pImpl->setSamplingInterval(interval);
}

void ResourceMonitor::enableTemperatureMonitoring(bool enable) {
    pImpl->enableTemperatureMonitoring(enable);
}

void ResourceMonitor::setCPUMonitorEnabled(bool enable) {
    pImpl->setCPUMonitorEnabled(enable);
}

void ResourceMonitor::setMemoryMonitorEnabled(bool enable) {
    pImpl->setMemoryMonitorEnabled(enable);
}

void ResourceMonitor::setDiskMonitorEnabled(bool enable) {
    pImpl->setDiskMonitorEnabled(enable);
}

void ResourceMonitor::setNetworkMonitorEnabled(bool enable) {
    pImpl->setNetworkMonitorEnabled(enable);
}

void ResourceMonitor::generateResourceReport(const std::string& outputFile) const {
    pImpl->generateResourceReport(outputFile);
}

void ResourceMonitor::generateResourceHistoryReport(const std::string& outputFile) const {
    pImpl->generateResourceHistoryReport(outputFile);
}

std::string ResourceMonitor::getResourceSummary() const {
    return pImpl->getResourceSummary();
}

double ResourceMonitor::calculateCPUUsageAverage() const {
    return pImpl->calculateCPUUsageAverage();
}

size_t ResourceMonitor::calculateMemoryUsageAverage() const {
    return pImpl->calculateMemoryUsageAverage();
}

size_t ResourceMonitor::calculateMemoryUsagePeak() const {
    return pImpl->calculateMemoryUsagePeak();
}

std::vector<std::string> ResourceMonitor::identifyResourceBottlenecks() const {
    return pImpl->identifyResourceBottlenecks();
}

} // namespace nlm
