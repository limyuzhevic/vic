// profiler.cpp - Implementation of Profiler
#include "profiler.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

Profiler::Profiler() : isProfiling_(false) {
    NLM_LOG_INFO("Profiler initialized");
}

Profiler::Profiler(const ProfilerConfig& config) : isProfiling_(false) {
    configure(config);
    NLM_LOG_INFO("Profiler initialized with configuration: " + config.name);
}

Profiler::~Profiler() = default;

void Profiler::configure(const ProfilerConfig& cfg) {
    pImpl->configure(cfg);
}

void Profiler::startProfiling() {
    pImpl->startProfiling();
}

void Profiler::stopProfiling() {
    pImpl->stopProfiling();
}

void Profiler::profileStep(int step, const PerformanceMetrics& metrics) {
    pImpl->profileStep(step, metrics);
}

void Profiler::profileEvent(const std::string& eventName, const std::chrono::duration<double>& duration) {
    pImpl->profileEvent(eventName, duration);
}

void Profiler::profileMemory(size_t size, bool allocation) {
    pImpl->profileMemory(size, allocation);
}

void Profiler::profileCPU(double time) {
    pImpl->profileCPU(time);
}

void Profiler::profileIO(size_t bytes, bool isRead) {
    pImpl->profileIO(bytes, isRead);
}

Profiler::ProfileData Profiler::getProfileData() const {
    return pImpl->getProfileData();
}

Profiler::PerformanceMetrics Profiler::getLastMetric() const {
    return pImpl->getLastMetric();
}

double Profiler::getAverageStepTime() const {
    return pImpl->getAverageStepTime();
}

double Profiler::getMedianStepTime() const {
    return pImpl->getMedianStepTime();
}

std::vector<std::string> Profiler::identifySlowOperations(double threshold) const {
    return pImpl->identifySlowOperations(threshold);
}

std::vector<std::string> Profiler::identifyMemoryLeaks() const {
    return pImpl->identifyMemoryLeaks();
}

std::vector<std::string> Profiler::getPerformanceBottlenecks() const {
    return pImpl->getPerformanceBottlenecks();
}

std::string Profiler::generatePerformanceAnalysis() const {
    return pImpl->generatePerformanceAnalysis();
}

void Profiler::exportProfile(const std::string& filename) const {
    pImpl->exportProfile(filename);
}

void Profiler::generateTextReport(std::ostream& output) const {
    pImpl->generateTextReport(output);
}

void Profiler::generateJSONReport(std::ostream& output) const {
    pImpl->generateJSONReport(output);
}

void Profiler::generateCSVReport(std::ostream& output) const {
    pImpl->generateCSVReport(output);
}

void Profiler::generateHTMLReport(std::ostream& output) const {
    pImpl->generateHTMLReport(output);
}

} // namespace nlm
