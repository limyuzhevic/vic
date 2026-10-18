// benchmark.cpp - Implementation of BenchmarkSuite
#include "benchmark.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

BenchmarkSuite::BenchmarkSuite() {
    NLM_LOG_INFO("BenchmarkSuite initialized");
}

BenchmarkSuite::~BenchmarkSuite() = default;

void BenchmarkSuite::addBenchmark(const Benchmark& benchmark) {
    pImpl->addBenchmark(benchmark);
}

void BenchmarkSuite::removeBenchmark(const std::string& name) {
    pImpl->removeBenchmark(name);
}

void BenchmarkSuite::clearBenchmarks() {
    pImpl->clearBenchmarks();
}

bool BenchmarkSuite::hasBenchmark(const std::string& name) const {
    return pImpl->hasBenchmark(name);
}

void BenchmarkSuite::setIterations(size_t iterations) {
    pImpl->setIterations(iterations);
}

void BenchmarkSuite::setTimeout(double timeout) {
    pImpl->setTimeout(timeout);
}

void BenchmarkSuite::enableWarmup(bool enable) {
    pImpl->enableWarmup(enable);
}

void BenchmarkSuite::setWarmupIterations(size_t iterations) {
    pImpl->setWarmupIterations(iterations);
}

BenchmarkResult BenchmarkSuite::runBenchmark(const std::string& name) {
    return pImpl->runBenchmark(name);
}

std::vector<BenchmarkResult> BenchmarkSuite::runAllBenchmarks() {
    return pImpl->runAllBenchmarks();
}

double BenchmarkSuite::getAverageTime(const std::string& name) const {
    return pImpl->getAverageTime(name);
}

double BenchmarkSuite::getMinTime(const std::string& name) const {
    return pImpl->getMinTime(name);
}

double BenchmarkSuite::getMaxTime(const std::string& name) const {
    return pImpl->getMaxTime(name);
}

double BenchmarkSuite::getStandardDeviation(const std::string& name) const {
    return pImpl->getStandardDeviation(name);
}

size_t BenchmarkSuite::getTotalOperations(const std::string& name) const {
    return pImpl->getTotalOperations(name);
}

double BenchmarkSuite::getOperationsPerSecond(const std::string& name) const {
    return pImpl->getOperationsPerSecond(name);
}

void BenchmarkSuite::generateBenchmarkReport(const std::string& outputFile) const {
    pImpl->generateBenchmarkReport(outputFile);
}

void BenchmarkSuite::generateComparisonReport(const std::vector<BenchmarkResult>& results, const std::string& outputFile) const {
    pImpl->generateComparisonReport(results, outputFile);
}

void BenchmarkSuite::addDefaultBenchmarks() {
    pImpl->addDefaultBenchmarks();
}

void BenchmarkSuite::addSimulationBenchmarks() {
    pImpl->addSimulationBenchmarks();
}

void BenchmarkSuite::addMemoryBenchmarks() {
    pImpl->addMemoryBenchmarks();
}

void BenchmarkSuite::addCPUBenchmarks() {
    pImpl->addCPUBenchmarks();
}

bool BenchmarkSuite::loadBenchmarksFromFile(const std::string& filename) {
    return pImpl->loadBenchmarksFromFile(filename);
}

bool BenchmarkSuite::saveBenchmarksToFile(const std::string& filename) const {
    return pImpl->saveBenchmarksToFile(filename);
}

} // namespace nlm
