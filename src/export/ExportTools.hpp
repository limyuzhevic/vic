#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>
#include "../core/Logger/Logger.hpp"
#include "../visualization/VisualizationData.hpp"
#include "../brain/Brain.hpp"
#include "../debugging/DebugInterface.hpp"

namespace nlm {

/**
 * @file DataExporter.hpp
 * 
 * Data export system for NLM.
 * 
 * This file provides the DataExporter class, which offers comprehensive
 * data export capabilities for NLM applications. It exports neural data,
 * simulation results, and analysis data in various formats for external
 * analysis and integration.
 * 
 * Key features:
 * - Neural data export (spike trains, voltage traces, synaptic data)
 * - Simulation result export
 * - Analysis result export
 * - Visualization data export
 * - Multiple export formats (JSON, CSV, XML, HDF5)
 * - Export configuration management
 * - Export validation
 * - Export compression
 * - Export encryption
 * - Remote export support
 * - Batch export
 * - Incremental export
 * - Export monitoring
 * - Export error handling
 * - Export progress tracking
 * 
 * @note Integrates with NLM's data systems
 * @see VisualizationData - Data to export
 * @see DebugInterface - For integration
 * @see DataAnalyzer - For analysis integration
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class DataExporter {
public:
    DataExporter();
    ~DataExporter();
    
    // Disable copying - PImpl idiom
    DataExporter(const DataExporter&) = delete;
    DataExporter& operator=(const DataExporter&) = delete;
    DataExporter(DataExporter&&) noexcept;
    DataExporter& operator=(DataExporter&&) noexcept;
    
    /**
     * @brief Initialize the data exporter
     * 
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Export neural data
     * 
     * @param data Neural data to export
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportNeuralData(const VisualizationData& data, const std::string& format, const std::string& filepath);
    
    /**
     * @brief Export simulation results
     * 
     * @param brain Reference to brain system
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportSimulationResults(const Brain& brain, const std::string& format, const std::string& filepath);
    
    /**
     * @brief Export analysis results
     * 
     * @param results Analysis results
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportAnalysisResults(const std::unordered_map<std::string, double>& results, const std::string& format, const std::string& filepath);
    
    /**
     * @brief Export visualization data
     * 
     * @param data Visualization data
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportVisualizationData(const VisualizationData& data, const std::string& format, const std::string& filepath);
    
    /**
     * @brief Export debug session
     * 
     * @param debugInterface Reference to debug interface
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportDebugSession(const DebugInterface& debugInterface, const std::string& format, const std::string& filepath);
    
    /**
     * @brief Set export callback
     * 
     * @param callback Function to call on export completion
     */
    void setExportCallback(std::function<void(const std::string&, bool)> callback);
    
    /**
     * @brief Get available export formats
     * 
     * @return Available export formats
     */
    std::vector<std::string> getAvailableExportFormats() const;
    
    /**
     * @brief Check if export format is supported
     * 
     * @param format Export format
     * @return true if format is supported
     */
    bool isExportFormatSupported(const std::string& format) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file ResultArchiver.hpp
 * 
 * Result archiving system for NLM.
 * 
 * This file provides the ResultArchiver class, which offers
 * comprehensive result archiving capabilities. It archives experiment
 * results, analysis results, and simulation results for long-term
 * storage and retrieval.
 * 
 * Key features:
 * - Experiment result archiving
 * - Analysis result archiving
 * - Simulation result archiving
 * - Result compression
 * - Result encryption
 * - Archive management
 * - Archive retrieval
 * - Archive cleanup
 * - Archive indexing
 * - Archive validation
 * - Archive integrity checking
 * - Remote archiving
 * - Local archiving
 * - Hierarchical archiving
 * - Version control archiving
 * - Incremental archiving
 * - Archive monitoring
 * - Archive error handling
 * 
 * @note Integrates with NLM's storage systems
 * @see DataExporter - For export integration
 * @see ExperimentManager - For integration
 * @see InteractiveVisualizer - For integration
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class ResultArchiver {
public:
    ResultArchiver();
    ~ResultArchiver();
    
    // Disable copying - PImpl idiom
    ResultArchiver(const ResultArchiver&) = delete;
    ResultArchiver& operator=(const ResultArchiver&) = delete;
    ResultArchiver(ResultArchiver&&) noexcept;
    ResultArchiver& operator=(ResultArchiver&&) noexcept;
    
    /**
     * @brief Initialize the result archiver
     * 
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Archive experiment results
     * 
     * @param experimentName Name of experiment
     * @param results Experiment results
     * @return true if results archived
     */
    bool archiveExperimentResults(const std::string& experimentName, const std::unordered_map<std::string, double>& results);
    
    /**
     * @brief Archive analysis results
     * 
     * @param analysisType Type of analysis
     * @param results Analysis results
     * @return true if results archived
     */
    bool archiveAnalysisResults(const std::string& analysisType, const std::unordered_map<std::string, double>& results);
    
    /**
     * @brief Archive simulation results
     * 
     * @param brain Reference to brain system
     * @param simulationName Name of simulation
     * @return true if results archived
     */
    bool archiveSimulationResults(const Brain& brain, const std::string& simulationName);
    
    /**
     * @brief Retrieve archived results
     * 
     * @param archiveId Archive identifier
     * @return Archived results
     */
    std::unordered_map<std::string, double> retrieveArchivedResults(const std::string& archiveId) const;
    
    /**
     * @brief List archived results
     * 
     * @param archiveType Type of archive
     * @return List of archived results
     */
    std::vector<std::string> listArchivedResults(const std::string& archiveType) const;
    
    /**
     * @brief Clean up old archives
     * 
     * @param archiveType Type of archive to clean
     * @param maxAge Maximum age in days
     * @return Number of archives cleaned
     */
    size_t cleanupOldArchives(const std::string& archiveType, int maxAge);
    
    /**
     * @brief Set archiving callback
     * 
     * @param callback Function to call on archiving completion
     */
    void setArchiveCallback(std::function<void(const std::string&, bool)> callback);
    
    /**
     * @brief Check archive integrity
     * 
     * @param archiveId Archive identifier
     * @return true if archive is intact
     */
    bool checkArchiveIntegrity(const std::string& archiveId) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm