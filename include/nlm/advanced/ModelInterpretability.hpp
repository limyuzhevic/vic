#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Config/Config.hpp"

namespace nlm {

class AdvancedModelInterpretability {
public:
    AdvancedModelInterpretability(const std::string& id, const Config& config);
    ~AdvancedModelInterpretability();
    
    void calculateFeatureImportance(const std::string& datasetId);
    
    void generatePartialDependenceExplanations(const std::string& featureId);
    
    std::string generateSHAPExplanations(const std::string& instanceId);
    
    std::string generateAnchorExplanations(const std::string& instanceId);
    
    std::string getTopKFeatures(int k) const;
    
    std::string getFeatureImportance(const std::string& featureId) const;
    
    std::string getAdvancedModelInterpretabilityStatus() const;
    
    void saveInterpretabilityResults(const Config& config) const;
    
    void loadInterpretabilityResults(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
