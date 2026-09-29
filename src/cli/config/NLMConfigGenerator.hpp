#ifndef NLMCONFIGGENERATOR_HPP
#define NLMCONFIGGENERATOR_HPP

#include <string>
#include <map>
#include <vector>

namespace nlm {

class NLMConfigGenerator {
public:
    NLMConfigGenerator();
    ~NLMConfigGenerator();
    
    /**
     * Generate configuration from template
     * 
     * @param templateName Template name
     * @param outputFile Output file path
     * @param parameters Additional parameters
     * @return 0 on success, non-zero on error
     */
    int generateFromTemplate(const std::string& templateName, 
                             const std::string& outputFile,
                             const std::map<std::string, std::string>& parameters = {});
    
    /**
     * Get available templates
     * 
     * @return Vector of template names
     */
    std::vector<std::string> getAvailableTemplates() const;
    
    /**
     * Validate configuration
     * 
     * @param configFile Configuration file to validate
     * @return 0 if valid, non-zero if invalid
     */
    int validateConfig(const std::string& configFile) const;
    
    /**
     * Merge configuration files
     * 
     * @param file1 First configuration file
     * @param file2 Second configuration file
     * @param outputFile Output file path
     * @return 0 on success, non-zero on error
     */
    int mergeConfigs(const std::string& file1, const std::string& file2, const std::string& outputFile);
    
    /**
     * Export configuration to different formats
     * 
     * @param configFile Input configuration file
     * @param format Output format
     * @param outputFile Output file path
     * @return 0 on success, non-zero on error
     */
    int exportConfig(const std::string& configFile, const std::string& format, const std::string& outputFile);
    
    /**
     * Import configuration from different formats
     * 
     * @param inputFile Input file path
     * @param format Input format
     * @param outputFile Output configuration file
     * @return 0 on success, non-zero on error
     */
    int importConfig(const std::string& inputFile, const std::string& format, const std::string& outputFile);
    
    /**
     * Show configuration values
     * 
     * @param key Key to show (empty for all)
     * @param configFile Configuration file
     * @return 0 on success, non-zero on error
     */
    int showConfig(const std::string& key, const std::string& configFile);

private:
    std::vector<std::string> loadTemplates() const;
    std::string processTemplate(const std::string& templateName, 
                               const std::map<std::string, std::string>& parameters) const;
    std::map<std::string, std::string> parseConfigFile(const std::string& configFile) const;
    void writeConfig(const std::map<std::string, std::string>& config, const std::string& outputFile) const;
};

} // namespace nlm

#endif // NLMCONFIGGENERATOR_HPP
