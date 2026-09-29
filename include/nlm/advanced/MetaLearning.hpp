#pragma once

#include <string>
#include <vector>
#include "../core/Config/Config.hpp"

namespace nlm {

class AdvancedMetaLearning {
public:
    AdvancedMetaLearning(const Config& config);
    ~AdvancedMetaLearning();
    
    void addMetaTask(const std::string& id, const std::string& description,
                    const std::string& sourceDomain, const std::string& targetDomain);
    
    void recordSourcePerformance(const std::string& domain, const std::vector<double>& performance);
    
    void recordTargetPerformance(const std::string& domain, const std::vector<double>& performance);
    
    std::vector<double> adaptToNewDomain(const std::string& targetDomain,
                                         const std::vector<double>& sourceParams,
                                         const Config& config);
    
    void recordAdaptation(const std::string& taskId, double speed);
    
    double getOverallTransferEffectiveness() const;
    
    std::string getMetaLearningStatus() const;
    
    void saveState(const Config& config) const;
    
    void loadState(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
