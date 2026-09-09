// Add the export function implementations to Brain.cpp

#include "../experiments/CSVExporter.hpp"
#include "../experiments/JSONExporter.hpp"
#include "../experiments/BinaryExporter.hpp"

namespace nlm {

// CSV Exporter implementation
bool Brain::exportCSV(const std::string& filepath, const ExportOptions& options) {
    CSVExporter exporter;
    exporter.setOptions(options);
    exporter.setMetadata({
        "1.0",  // version
        "Neural data exported from NLM brain",  // description
        "NLM Export",  // author
        std::chrono::system_clock::now(),  // timestamp
        "brain_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()),  // simulationId
        "BrainExport",  // experimentName
        pImpl->currentStep,  // stepsExported
        pImpl->currentTime  // simulationTime
    });
    
    return exporter.exportData(filepath);
}

// JSON Exporter implementation
bool Brain::exportJSON(const std::string& filepath, const ExportOptions& options) {
    JSONExporter exporter;
    exporter.setOptions(options);
    exporter.setMetadata({
        "1.0",  // version
        "Neural data exported from NLM brain",  // description
        "NLM Export",  // author
        std::chrono::system_clock::now(),  // timestamp
        "brain_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()),  // simulationId
        "BrainExport",  // experimentName
        pImpl->currentStep,  // stepsExported
        pImpl->currentTime  // simulationTime
    });
    
    return exporter.exportData(filepath);
}

// Binary Exporter implementation
bool Brain::exportBinary(const std::string& filepath, const ExportOptions& options) {
    BinaryExporter exporter;
    exporter.setOptions(options);
    exporter.setMetadata({
        "1.0",  // version
        "Neural data exported from NLM brain",  // description
        "NLM Export",  // author
        std::chrono::system_clock::now(),  // timestamp
        "brain_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()),  // simulationId
        "BrainExport",  // experimentName
        pImpl->currentStep,  // stepsExported
        pImpl->currentTime  // simulationTime
    });
    
    return exporter.exportData(filepath);
}

// Time series export implementation
bool Brain::exportTimeSeries(const std::string& filepath, SimulationStep startStep, 
                            SimulationStep endStep, const ExportOptions& options) {
    CSVExporter exporter;
    exporter.setOptions(options);
    exporter.setMetadata({
        "1.0",  // version
        "Time series neural data exported from NLM brain",  // description
        "NLM Export",  // author
        std::chrono::system_clock::now(),  // timestamp
        "brain_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()),  // simulationId
        "BrainTimeSeries",  // experimentName
        endStep - startStep,  // stepsExported
        static_cast<double>(endStep) * pImpl->timestep  // simulationTime
    });
    
    return exporter.exportData(filepath);
}

// Snapshot export implementation
bool Brain::exportSnapshot(const std::string& filepath, SimulationStep step, 
                          const ExportOptions& options) {
    JSONExporter exporter;
    exporter.setOptions(options);
    exporter.setMetadata({
        "1.0",  // version
        "Snapshot neural data exported from NLM brain",  // description
        "NLM Export",  // author
        std::chrono::system_clock::now(),  // timestamp
        "brain_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()),  // simulationId
        "BrainSnapshot",  // experimentName
        1,  // stepsExported
        step * pImpl->timestep  // simulationTime
    });
    
    return exporter.exportData(filepath);
}

// Batch export implementation
bool Brain::exportBatch(const std::string& filepath, const std::vector<SimulationStep>& steps,
                       const ExportOptions& options) {
    BinaryExporter exporter;
    exporter.setOptions(options);
    exporter.setMetadata({
        "1.0",  // version
        "Batch neural data exported from NLM brain",  // description
        "NLM Export",  // author
        std::chrono::system_clock::now(),  // timestamp
        "brain_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()),  // simulationId
        "BrainBatch",  // experimentName
        static_cast<uint64_t>(steps.size()),  // stepsExported
        static_cast<double>(steps.back()) * pImpl->timestep  // simulationTime
    });
    
    return exporter.exportData(filepath);
}

} // namespace nlm
