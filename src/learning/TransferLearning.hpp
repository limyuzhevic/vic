#ifndef TRANSFER_LEARNING_HPP
#define TRANSFER_LEARNING_HPP

#include <vector>
#include <string>

namespace nlm {

class Brain;

class TransferLearning {
public:
    TransferLearning();
    ~TransferLearning();
    
    void initialize(Brain* brain);
    void transferSkills(const std::string& sourceTask,
                       const std::vector<float>& sourceSkills,
                       const std::string& targetTask,
                       std::vector<float>& targetSkills);
    void adaptToDomain(const std::vector<std::vector<float>>& sourceData,
                      const std::vector<float>& targetDomain,
                      std::vector<std::vector<float>>& adaptedData);
    void updateMetaLearning(const std::vector<std::vector<float>>& taskHistories,
                           std::vector<float>& learnedPrior,
                           const std::vector<float>& currentLearningRates,
                           const std::vector<float>& taskParameters);
    void updateContinualLearning(const std::vector<float>& currentKnowledge,
                                const std::vector<float>& newKnowledge,
                                std::vector<float>& retainedKnowledge);
    
    // Configuration methods
    void setFastTransferEnabled(bool enabled);
    void setDomainAdaptationEnabled(bool enabled);
    void setMetaLearningEnabled(bool enabled);
    void setContinualLearningEnabled(bool enabled);
    
    // Status and control
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration flags
    bool fastTransferEnabled_;
    bool domainAdaptationEnabled_;
    bool metaLearningEnabled_;
    bool continualLearningEnabled_;
};

// Internal model classes (for implementation)
class SkillTransfer;
class DomainAdaptation;
class MetaLearning;
class ContinualLearning;

} // namespace nlm

#endif // TRANSFER_LEARNING_HPP
