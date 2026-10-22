#ifndef NLM_OJA_LEARNING_H
#define NLM_OJA_LEARNING_H

#include "src/plasticity/PlasticityRule.hpp"
#include "src/brain/Synapse.hpp"
#include <cmath>

namespace nlm {

class OjaLearning : public PlasticityRule {
public:
    OjaLearning(float learningRate = 0.01f, float weightLimit = 1.0f);
    virtual ~OjaLearning() = default;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override;
    
    void setLearningRate(float rate) override;
    void setMaxWeight(float max) override;
    void setMinWeight(float min) override;
    
    float getLearningRate() const override;
    float getMaxWeight() const override;
    float getMinWeight() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_OJA_LEARNING_H