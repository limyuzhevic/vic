#ifndef NLMRESULTEXPORTER_HPP
#define NLMRESULTEXPORTER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

class NLMResultExporter {
public:
    NLMResultExporter();
    ~NLMResultExporter();
    
    /**
     * Export results to different formats
     * 
     * @param sourceDirectory Source directory containing results
     * @param format Export format
     * @param outputFile Output file path
     * @return 0 on success, non-zero on error
     */
    int exportResults(const std::string& sourceDirectory, 
                      const std::string& format,
                      const std::string& outputFile);
    
    /**
     * Export results to JSON format
     * 
     * @param sourceDirectory Source directory containing results
     * @param outputFile Output file path
     * @return 0 on success, non-zero on error
     */
    int exportToJSON(const std::string& sourceDirectory, const std::string& outputFile);
    
    /**
     * Export results to YAML format
     * 
     * @param sourceDirectory Source directory containing results
     * @param outputFile Output file path
     * @return 0 on success, non-zero on error
     */
    int exportToYAML(const std::string& sourceDirectory, const std::string& outputFile);
    
    /**
     * Export results to CSV format
     * 
     * @param sourceDirectory Source directory containing results
     * @param outputFile Output file path
     * @return 0 on success, non-zero on error
     */
    int exportToCSV(const std::string& sourceDirectory, const std::string& outputFile);
    
    /**
     * Export results to SQLite format
     * 
     * @param sourceDirectory Source directory containing results
     * @param outputFile Output file path
     * @return 0 on success, non-zero on error
     */
    int exportToSQLite(const std::string& sourceDirectory, const std::string& outputFile);
    
    /**
     * Export results to HDF5 format
     * 
     * @param sourceDirectory Source directory containing results
     * @param outputFile Output file path
     * @return 0 on success, non-zero on error
     */
    int exportToHDF5(const std::string& sourceDirectory, const std::string& outputFile);
    
    /**
     * Analyze and export results
     * 
     * @param sourceDirectory Source directory containing results
     * @param analysisType Type of analysis
     * @param outputFile Output file path
     * @return 0 on success, non-zero on error
     */
    int analyzeAndExport(const std::string& sourceDirectory,
                        const std::string& analysisType,
                        const std::string& outputFile);
    
    /**
     * Get supported export formats
     * 
     * @return Vector of supported formats
     */
    std::vector<std::string> getSupportedFormats() const;

private:
    std::map<std::string, std::string> parseResults(const std::string& sourceDirectory) const;
    void writeJSON(const std::map<std::string, std::string>& results, const std::string& outputFile) const;
    void writeYAML(const std::map<std::string, std::string>& results, const std::string& outputFile) const;
    void writeCSV(const std::map<std::string, std::string>& results, const std::string& outputFile) const;
    void writeSQLite(const std::map<std::string, std::string>& results, const std::string& outputFile) const;
    void writeHDF5(const std::map<std::string, std::string>& results, const std::string& outputFile) const;
};

} // namespace nlm

#endif // NLMRESULTEXPORTER_HPP
