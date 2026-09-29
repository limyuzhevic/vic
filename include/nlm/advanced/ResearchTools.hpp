#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Config/Config.hpp"

namespace nlm {

struct ExperimentConfig {
    std::string experimentId;
    std::string description;
    std::map<std::string, std::string> parameters;
    bool isReproducible;
    double reproducibilityScore;
};

class AdvancedResearchTools {
public:
    AdvancedResearchTools(const std::string& id, const Config& config);
    ~AdvancedResearchTools();
    
    ExperimentConfig createExperiment(const std::string& id, const std::string& description);
    
    void addExperimentParameter(ExperimentConfig& experiment, const std::string& param, const std::string& value);
    
    bool executeExperiment(const std::string& experimentId);
    
    std::string generateResearchReport() const;
    
    std::string getAdvancedResearchToolsStatus() const;
    
    void saveResearchState(const Config& config) const;
    
    void loadResearchState(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
