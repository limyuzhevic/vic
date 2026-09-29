#ifndef NLMSCRIPTGENERATOR_HPP
#define NLMSCRIPTGENERATOR_HPP

#include <string>
#include <map>
#include <vector>

namespace nlm {

class NLMScriptGenerator {
public:
    NLMScriptGenerator();
    ~NLMScriptGenerator();
    
    /**
     * Generate shell script from template
     * 
     * @param templateName Template name
     * @param outputFile Output file path
     * @param parameters Additional parameters
     * @return 0 on success, non-zero on error
     */
    int generateScript(const std::string& templateName, 
                      const std::string& outputFile,
                      const std::map<std::string, std::string>& parameters = {});
    
    /**
     * Generate Docker file
     * 
     * @param configFile Configuration file
     * @param outputFile Output file path
     * @param parameters Additional parameters
     * @return 0 on success, non-zero on error
     */
    int generateDocker(const std::string& configFile, 
                      const std::string& outputFile,
                      const std::map<std::string, std::string>& parameters = {});
    
    /**
     * Generate SLURM job script
     * 
     * @param configFile Configuration file
     * @param outputFile Output file path
     * @param parameters Additional parameters
     * @return 0 on success, non-zero on error
     */
    int generateSLURM(const std::string& configFile, 
                     const std::string& outputFile,
                     const std::map<std::string, std::string>& parameters = {});
    
    /**
     * Generate Kubernetes manifest
     * 
     * @param configFile Configuration file
     * @param outputFile Output file path
     * @param parameters Additional parameters
     * @return 0 on success, non-zero on error
     */
    int generateKubernetes(const std::string& configFile, 
                          const std::string& outputFile,
                          const std::map<std::string, std::string>& parameters = {});
    
    /**
     * Get available script templates
     * 
     * @return Vector of template names
     */
    std::vector<std::string> getAvailableTemplates() const;

private:
    std::string generateBashScript(const std::string& templateName,
                                  const std::map<std::string, std::string>& parameters) const;
    std::string generateDockerFile(const std::string& templateName,
                                  const std::map<std::string, std::string>& parameters) const;
    std::string generateSLURMScript(const std::string& templateName,
                                   const std::map<std::string, std::string>& parameters) const;
    std::string generateKubernetesManifest(const std::string& templateName,
                                          const std::map<std::string, std::string>& parameters) const;
    void writeScript(const std::string& content, const std::string& outputFile) const;
};

} // namespace nlm

#endif // NLMSCRIPTGENERATOR_HPP
