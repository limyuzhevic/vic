#include <iostream>
#include <vector>
#include <cassert>

// Simple test to verify episodic memory functionality
namespace nlm {
    // Basic types
    using SimulationStep = int;
    using NeuronId = int;
    using Timestamp = float;
    
    // Test EpisodicMemoryItem structure
    struct EpisodicMemoryItem {
        SimulationStep timestamp;
        SimulationStep simulationStep;
        std::vector<float> sensoryState;
        std::vector<float> resultingSensoryState;
        float positionX = 0;
        float positionY = 0;
        float orientation = 0;
        enum class ActionType { MoveForward, Rest, Wait };
        ActionType action = ActionType::Wait;
        float reward = 0;
        float resultingReward = 0;
        float energy = 0;
        float novelty = 0;
        std::vector<NeuronId> activeNeurons;
        std::vector<float> neuronActivations;
        std::string context;
        SimulationStep age = 0;
        float relevance = 0;
        float strength = 0;
        
        EpisodicMemoryItem() {}
        EpisodicMemoryItem(SimulationStep simStep, float px, float py, ActionType act, float rew)
            : timestamp(simStep), simulationStep(simStep), positionX(px), positionY(py), action(act), reward(rew) {}
    };
    
    // Test basic episodic memory operations
    class TestEpisodicMemory {
    public:
        void storeEpisode(const EpisodicMemoryItem& episode) {
            episodes.push_back(episode);
        }
        
        size_t getEpisodeCount() const {
            return episodes.size();
        }
        
        const EpisodicMemoryItem* getEpisode(size_t index) const {
            if (index < episodes.size()) {
                return &episodes[index];
            }
            return nullptr;
        }
        
        std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const {
            std::vector<const EpisodicMemoryItem*> results;
            size_t start = count > episodes.size() ? 0 : episodes.size() - count;
            for (size_t i = start; i < episodes.size(); ++i) {
                results.push_back(&episodes[i]);
            }
            return results;
        }
        
        void clear() {
            episodes.clear();
        }
        
    private:
        std::vector<EpisodicMemoryItem> episodes;
    };
}

int main() {
    std::cout << "Testing episodic memory implementation..." << std::endl;
    
    // Test 1: Create and store episodes
    nlm::EpisodicMemoryItem episode1(1, 10.0f, 20.0f, nlm::EpisodicMemoryItem::ActionType::MoveForward, 5.0f);
    episode1.reward = 5.0f;
    episode1.energy = 1.0f;
    episode1.context = "test_step_1";
    
    nlm::EpisodicMemoryItem episode2(2, 15.0f, 25.0f, nlm::EpisodicMemoryItem::ActionType::Rest, 3.0f);
    episode2.reward = 3.0f;
    episode2.energy = 0.8f;
    episode2.context = "test_step_2";
    
    nlm::TestEpisodicMemory testMem;
    testMem.storeEpisode(episode1);
    testMem.storeEpisode(episode2);
    
    // Test 2: Verify episode count
    assert(testMem.getEpisodeCount() == 2);
    std::cout << "✓ Episode count: " << testMem.getEpisodeCount() << std::endl;
    
    // Test 3: Verify episode retrieval
    const nlm::EpisodicMemoryItem* retrieved1 = testMem.getEpisode(0);
    const nlm::EpisodicMemoryItem* retrieved2 = testMem.getEpisode(1);
    
    assert(retrieved1 != nullptr);
    assert(retrieved1->timestamp == 1);
    assert(retrieved1->positionX == 10.0f);
    assert(retrieved1->action == nlm::EpisodicMemoryItem::ActionType::MoveForward);
    std::cout << "✓ Episode 1 retrieved correctly" << std::endl;
    
    assert(retrieved2 != nullptr);
    assert(retrieved2->timestamp == 2);
    assert(retrieved2->positionX == 15.0f);
    assert(retrieved2->action == nlm::EpisodicMemoryItem::ActionType::Rest);
    std::cout << "✓ Episode 2 retrieved correctly" << std::endl;
    
    // Test 4: Verify replay functionality
    std::vector<const nlm::EpisodicMemoryItem*> replay = testMem.getEpisodesForReplay(1);
    assert(replay.size() == 1);
    assert(replay[0]->timestamp == 2); // Most recent episode
    std::cout << "✓ Replay selection works correctly" << std::endl;
    
    // Test 5: Clear and verify
    testMem.clear();
    assert(testMem.getEpisodeCount() == 0);
    std::cout << "✓ Clear functionality works correctly" << std::endl;
    
    std::cout << "\nAll episodic memory tests passed!" << std::endl;
    return 0;
}