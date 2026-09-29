#pragma once

#include <string>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class AdvancedCurriculumLearning {
public:
    AdvancedCurriculumLearning(const Config& config);
    ~AdvancedCurriculumLearning();
    
    void addLearningTask(const std::string& id, const std::string& description,
                        const std::string& type, double difficulty, double complexity);
    
    void updatePerformance(const std::string& taskId, double performance);
    
    std::string getCurrentTask(const Brain& brain, const Config& config);
    
    void completeTask(const std::string& taskId, const Brain& brain,
                     const Config& config);
    
    double getConvergenceRate() const;
    
    std::string getProgressSummary() const;
    
    void saveToConfig(Config& config) const;
    
    void loadFromConfig(Config& config);
    
    bool isCompleted() const;
    
    double getProgress() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
