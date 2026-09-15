#include "../sensory/SensoryInput.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class AgentBrain : public Brain {
public:
    AgentBrain(std::shared_ptr<Config> config);
    
    // Override sensory input processing
    void receiveSensoryInput(const SensoryInput& input) override;
    
    // Override action production
    std::unique_ptr<Action> produceAction() override;
    
    // Log integrated sensory status
    void logStatus() const override;
    
private:
    // Real sensory processors with neural computation
    std::unique_ptr<VisionProcessor> visionProcessor;
    std::unique_ptr<AudioProcessor> audioProcessor;
    std::unique_ptr<InternalSignalsProcessor> internalSignalsProcessor;
};

} // namespace nlm