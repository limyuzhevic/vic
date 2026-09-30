// NLM Advanced Benchmark Types - Header
// AdvancedBenchmarkTypes.hpp

#pragma once

#include "BenchmarkBase.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {
namespace benchmarking {

// Forward declarations
class EnhancedBenchmarkManager;
class ConsoleRealTimeMonitor;

// Advanced benchmark implementations
class StandardBenchmark : public BenchmarkBase {
public:
    BenchmarkType getType() const override { return BenchmarkType::STANDARD; }
    std::string getName() const override { return "Standard Performance Benchmark"; }
    BenchmarkResult run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) override;
    bool validateConfig(const BenchmarkConfig& config) const override;
    std::vector<std::string> getRequiredSystemFeatures() const override;
};

class MemoryBenchmark : public BenchmarkBase {
public:
    BenchmarkType getType() const override { return BenchmarkType::MEMORY; }
    std::string getName() const override { return "Memory Usage Benchmark"; }
    BenchmarkResult run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) override;
    bool validateConfig(const BenchmarkConfig& config) const override;
    std::vector<std::string> getRequiredSystemFeatures() const override;
protected:
    double calculateCoefficientOfVariation(const std::vector<double>& values);
};

class PlasticityBenchmark : public BenchmarkBase {
public:
    BenchmarkType getType() const override { return BenchmarkType::PLASTICITY; }
    std::string getName() const override { return "Plasticity Learning Benchmark"; }
    BenchmarkResult run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) override;
    bool validateConfig(const BenchmarkConfig& config) const override;
    std::vector<std::string> getRequiredSystemFeatures() const override;
};

class SpikeBenchmark : public BenchmarkBase {
public:
    BenchmarkType getType() const override { return BenchmarkType::SPIKE; }
    std::string getName() const override { return "Spike Generation Benchmark"; }
    BenchmarkResult run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) override;
    bool validateConfig(const BenchmarkConfig& config) const override;
    std::vector<std::string> getRequiredSystemFeatures() const override;
protected:
    double calculateCoefficientOfVariation(const std::vector<double>& values);
};

class IntegrationBenchmark : public BenchmarkBase {
public:
    BenchmarkType getType() const override { return BenchmarkType::INTEGRATION; }
    std::string getName() const override { return "System Integration Benchmark"; }
    BenchmarkResult run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) override;
    bool validateConfig(const BenchmarkConfig& config) const override;
    std::vector<std::string> getRequiredSystemFeatures() const override;
};

class ScalabilityBenchmark : public BenchmarkBase {
public:
    BenchmarkType getType() const override { return BenchmarkType::SCALABILITY; }
    std::string getName() const override { return "Scalability Benchmark"; }
    BenchmarkResult run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) override;
    bool validateConfig(const BenchmarkConfig& config) const override;
    std::vector<std::string> getRequiredSystemFeatures() const override;
protected:
    double calculateCoefficientOfVariation(const std::vector<double>& values);
};

}}} // namespace nlm::benchmarking
