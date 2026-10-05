// Output formatting infrastructure for NLM
// Supports JSON, CSV, and human-readable output formats

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "brain/Brain.hpp"
#include "core/Config/AdvancedConfigManager.hpp"

namespace nlm {

// Base output formatter interface
class OutputFormatter {
public:
    virtual ~OutputFormatter() = default;
    
    virtual void setBrain(std::shared_ptr<Brain> brain) = 0;
    virtual void setConfig(std::shared_ptr<AdvancedConfigManager> config) = 0;
    virtual void setProfiler(std::shared_ptr<PerformanceProfiler> profiler) = 0;
    
    virtual void setTitle(const std::string& title) = 0;
    virtual void setTimestamp(const std::chrono::system_clock::time_point& timestamp) = 0;
    
    virtual std::string format() const = 0;
    virtual bool saveToFile(const std::string& filepath) const = 0;
    
    virtual void addSection(const std::string& name, const std::string& content) = 0;
    virtual void addMetric(const std::string& name, double value, const std::string& unit = "") = 0;
    virtual void addPerformanceResult(const PerformanceResult& result) = 0;
    virtual void addExperimentResult(const std::string& name, bool success, const std::string& details = "") = 0;
    
    virtual void clear() = 0;
};

// Human-readable output formatter
class HumanReadableFormatter : public OutputFormatter {
public:
    HumanReadableFormatter();
    ~HumanReadableFormatter() = default;
    
    void setBrain(std::shared_ptr<Brain> brain) override;
    void setConfig(std::shared_ptr<AdvancedConfigManager> config) override;
    void setProfiler(std::shared_ptr<PerformanceProfiler> profiler) override;
    
    void setTitle(const std::string& title) override;
    void setTimestamp(const std::chrono::system_clock::time_point& timestamp) override;
    
    std::string format() const override;
    bool saveToFile(const std::string& filepath) const override;
    
    void addSection(const std::string& name, const std::string& content) override;
    void addMetric(const std::string& name, double value, const std::string& unit = "") override;
    void addPerformanceResult(const PerformanceResult& result) override;
    void addExperimentResult(const std::string& name, bool success, const std::string& details = "") override;
    
    void clear() override;
    
    // Human-readable specific methods
    std::string generateBrainStatus() const;
    std::string generateConfigurationSummary() const;
    std::string generatePerformanceSummary() const;
    std::string generateExperimentSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// JSON output formatter
class JsonFormatter : public OutputFormatter {
public:
    JsonFormatter();
    ~JsonFormatter() = default;
    
    void setBrain(std::shared_ptr<Brain> brain) override;
    void setConfig(std::shared_ptr<AdvancedConfigManager> config) override;
    void setProfiler(std::shared_ptr<PerformanceProfiler> profiler) override;
    
    void setTitle(const std::string& title) override;
    void setTimestamp(const std::chrono::system_clock::time_point& timestamp) override;
    
    std::string format() const override;
    bool saveToFile(const std::string& filepath) const override;
    
    void addSection(const std::string& name, const std::string& content) override;
    void addMetric(const std::string& name, double value, const std::string& unit = "") override;
    void addPerformanceResult(const PerformanceResult& result) override;
    void addExperimentResult(const std::string& name, bool success, const std::string& details = "") override;
    
    void clear() override;
    
    // JSON specific methods
    nlohmann::json generateBrainData() const;
    nlohmann::json generateConfigurationData() const;
    nlohmann::json generatePerformanceData() const;
    nlohmann::json generateExperimentData() const;
    nlohmann::json generateMetadata() const;
    
    void exportToFile(const std::string& filepath) const;
};

// CSV output formatter
class CsvFormatter : public OutputFormatter {
public:
    CsvFormatter();
    ~CsvFormatter() = default;
    
    void setBrain(std::shared_ptr<Brain> brain) override;
    void setConfig(std::shared_ptr<AdvancedConfigManager> config) override;
    void setProfiler(std::shared_ptr<PerformanceProfiler> profiler) override;
    
    void setTitle(const std::string& title) override;
    void setTimestamp(const std::chrono::system_clock::time_point& timestamp) override;
    
    std::string format() const override;
    bool saveToFile(const std::string& filepath) const override;
    
    void addSection(const std::string& name, const std::string& content) override;
    void addMetric(const std::string& name, double value, const std::string& unit = "") override;
    void addPerformanceResult(const PerformanceResult& result) override;
    void addExperimentResult(const std::string& name, bool success, const std::string& details = "") override;
    
    void clear() override;
    
    // CSV specific methods
    std::string generateBrainCsv() const;
    std::string generateConfigurationCsv() const;
    std::string generatePerformanceCsv() const;
    std::string generateExperimentCsv() const;
    
    void exportToFile(const std::string& filepath) const;
};

// Machine-readable output formatter (for automation)
class MachineReadableFormatter : public OutputFormatter {
public:
    MachineReadableFormatter();
    ~MachineReadableFormatter() = default;
    
    void setBrain(std::shared_ptr<Brain> brain) override;
    void setConfig(std::shared_ptr<AdvancedConfigManager> config) override;
    void setProfiler(std::shared_ptr<PerformanceProfiler> profiler) override;
    
    void setTitle(const std::string& title) override;
    void setTimestamp(const std::chrono::system_clock::time_point& timestamp) override;
    
    std::string format() const override;
    bool saveToFile(const std::string& filepath) const override;
    
    void addSection(const std::string& name, const std::string& content) override;
    void addMetric(const std::string& name, double value, const std::string& unit = "") override;
    void addPerformanceResult(const PerformanceResult& result) override;
    void addExperimentResult(const std::string& name, bool success, const std::string& details = "") override;
    
    void clear() override;
    
    // Machine-readable specific methods
    std::string generateBrainMachineData() const;
    std::string generateConfigurationMachineData() const;
    std::string generatePerformanceMachineData() const;
    std::string generateExperimentMachineData() const;
    
    std::string generateJsonOutput() const;
    std::string generateYamlOutput() const;
    std::string generateProtobufOutput() const; // Placeholder
    
    void exportToFile(const std::string& filepath) const;
};

// Output formatter factory
class OutputFormatterFactory {
public:
    static std::unique_ptr<OutputFormatter> createFormatter(const std::string& format);
    
    static bool saveFormattedOutput(const std::string& format, const std::string& filepath,
                                    std::shared_ptr<Brain> brain,
                                    std::shared_ptr<AdvancedConfigManager> config,
                                    std::shared_ptr<PerformanceProfiler> profiler);
    
    static std::string getSupportedFormats();
    
    static bool validateFormat(const std::string& format);
    
private:
    static const std::vector<std::string> supportedFormats;
};

} // namespace nlm
