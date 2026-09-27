// Simple test to verify AgentBrain compilation
#include "src/agent/AgentBrain.hpp"
#include <memory>

// Mock classes for testing
class MockBrain {
public:
    class MockRegion {
    public:
        class MockSynapse {
        public:
            float getEligibilityTrace() const { return 0.0f; }
            void addToWeight(float) {}
            void decayEligibilityTrace(float) {}
        };
        std::vector<MockSynapse*> getSynapses() { return {}; }
    };
    std::vector<MockRegion*> getRegions() { return {}; }
    class MockSTDP {
    public:
        void setLTPWeight(float) {}
        void setLTDWeight(float) {}
    };
    MockSTDP* getSTDP() { return nullptr; }
    class MockPredictionError {
    public:
        void initialize(class MockBrain*) {}
        void computeError(float, float) {}
        void update(float) {}
    };
    MockPredictionError* getPredictionErrorSignal() { return nullptr; }
    class MockRandomGenerator {
    public:
        float uniformReal(float, float) { return 0.5f; }
        int uniformInt(int, int) { return 0; }
    };
    MockRandomGenerator* getRandomGenerator() { return nullptr; }
    void setDevelopmentalStage(int) {}
    int getDevelopmentalStage() { return 0; }
    class MockStructuralPlasticity {
    public:
        void setSynaptogenesisRate(float) {}
        void setPruningRate(float) {}
    };
    MockStructuralPlasticity* getStructuralPlasticity() { return nullptr; }
    class MockPredictionSystem {
    public:
        void recordSensoryState(const std::vector<float>&, int) {}
        std::vector<float> generatePrediction(int) { return {}; }
        float getPredictionConfidence() { return 0.0f; }
        float updateWithObservation(const std::vector<float>&, int) { return 0.0f; }
        std::unique_ptr<class SensoryInput> predictNextState(const SensoryInput&) { return nullptr; }
        void updatePredictions(const SensoryInput&, const SensoryInput&) {}
        void train(const SensoryInput&) {}
    };
    MockPredictionSystem* getPredictionSystem() { return nullptr; }
};

int main() {
    auto brain = std::make_shared<MockBrain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Test initialization
    agentBrain.initialize(SimpleWorld());
    
    // Test prediction error computation
    float error = agentBrain.computePredictionError(
        std::vector<float>{0.5f, 0.6f, 0.7f},
        std::vector<float>{0.4f, 0.7f, 0.8f}
    );
    
    // Test get methods
    float predError = agentBrain.getPredictionError();
    float curiosityThreshold = agentBrain.getCuriosityThreshold();
    float curiosityLevel = agentBrain.getCuriosityLevel();
    float noveltyLevel = agentBrain.getNoveltyLevel();
    float neuromodulationLevel = agentBrain.getNeuromodulationLevel();
    
    // Test applyRewardModulation
    agentBrain.applyRewardModulation(1.0f, 0.8f);
    
    return 0;
}
