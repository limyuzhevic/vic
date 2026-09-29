#ifndef EXTENDED_DEVELOPMENT_HPP
#define EXTENDED_DEVELOPMENT_HPP

#include <vector>
#include <string>

namespace nlm {

class Brain;

class ExtendedDevelopment {
public:
    ExtendedDevelopment();
    ~ExtendedDevelopment();
    
    void initialize(Brain* brain);
    void updateDevelopment(const std::vector<float>& initialState,
                          const std::vector<std::vector<float>>& experiences,
                          const std::vector<float>& environmentalFactors,
                          std::vector<float>& developedState,
                          float timeElapsed);
    void updateSensitivePeriod(float timeElapsed);
    void updateEpigeneticState(const std::vector<float>& environmentalFactors,
                              float timeElapsed);
    void updatePlasticity(const std::vector<float>& experienceQuality,
                         float timeElapsed);
    void applyDevelopmentalChanges(const std::vector<float>& currentState,
                                   std::vector<float>& targetState,
                                   float timeElapsed);
    
    // Configuration methods
    void setSensitivePeriodExtension(float extension);
    void setEpigeneticInfluence(float influence);
    void setPlasticityLevel(float level);
    
    // Status and control
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Development state
    float sensitivePeriodExtension_;
    float epigeneticInfluence_;
    float plasticityLevel_;
    float experienceQuality_;
};

// Internal model classes (for implementation)
class ExperienceDependentDevelopment;
class SensitivePeriodExtender;
class EpigeneticInfluencer;
class DevelopmentalPlasticity;

} // namespace nlm

#endif // EXTENDED_DEVELOPMENT_HPP
