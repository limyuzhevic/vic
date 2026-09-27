#pragma once

#include "ProfileExporter.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <cmath>

namespace nlm {

class ProfileExporter {
public:
    struct ExportOptions {
        bool includeCPU{true};
        bool includeMemory{true};
        bool includeCallStack{true};
        bool includeBottlenecks{true};
        bool includeMemoryLeaks{true};
        std::string format{"json"}; // json, csv, text, xml
        std::string outputFile;
        bool compress{false};
        double timestampThreshold{0.0}; // in seconds
    };

    ProfileExporter();
    ~ProfileExporter();

    // Disable copying, enable moving
    ProfileExporter(const ProfileExporter&) = delete;
    ProfileExporter& operator=(const ProfileExporter&) = delete;
    ProfileExporter(ProfileExporter&&) noexcept;
    ProfileExporter& operator=(ProfileExporter&&) noexcept;

    // Export functionality
    bool exportProfiles(const ExportOptions& options);
    bool exportToFile(const std::string& filename);
    bool exportToMemory(std::vector<char>& output, const ExportOptions& options);

    // Format-specific exports
    bool exportAsJSON(const ExportOptions& options);
    bool exportAsCSV(const ExportOptions& options);
    bool exportAsText(const ExportOptions& options);
    bool exportAsXML(const ExportOptions& options);

    // Integration
    void setCallProfiler(CallProfiler* callProfiler);
    void setMemoryProfiler(MemoryProfiler* memoryProfiler);
    void setCPProfiler(CPProfiler* cpProfiler);
    void setPerformanceMonitor(PerformanceMonitor* monitor);
    void setBenchmarkSuite(BenchmarkSuite* benchmarkSuite);

    // Statistics
    std::string getStatistics() const;
    std::string generateReport() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
