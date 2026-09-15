#pragma once

namespace nlm {

// Export data utilities
class ExportUtilities {
public:
    ExportUtilities();
    ~ExportUtilities();
    
    // Export spike data
    static bool exportSpikeData(const std::vector<DetailedSpikeEvent>& spikes,
                               const std::string& filepath,
                               ExportFormat format);
    
    // Export weight matrix data
    static bool exportWeightData(const std::vector<std::vector<float>>& matrix,
                                const std::string& filepath,
                                ExportFormat format,
                                const std::vector<std::string>& rowLabels,
                                const std::vector<std::string>& colLabels);
    
    // Export activity data
    static bool exportActivityData(const std::vector<double>& firingRates,
                                  const std::vector<double>& membranePotentials,
                                  const std::string& filepath,
                                  ExportFormat format);
    
    // Export network data
    static bool exportNetworkData(const class Brain& brain,
                                 const std::string& filepath,
                                 ExportFormat format);
    
    // Export development data
    static bool exportDevelopmentData(const VisualizationSystem::DevelopmentStats& stats,
                                     const std::string& filepath,
                                     ExportFormat format);
    
    // Create visualization configuration file
    static bool exportConfig(const std::map<std::string, std::string>& config,
                            const std::string& filepath);
    
    // Import configuration file
    static bool importConfig(const std::string& filepath,
                            std::map<std::string, std::string>& config);
    
private:
    static bool writeText(const std::string& content, const std::string& filepath);
    static bool writeJSON(const std::string& content, const std::string& filepath);
    static bool writeCSV(const std::string& content, const std::string& filepath);
    static bool writeBinary(const std::vector<char>& data, const std::string& filepath);
};

} // namespace nlm
