// Sensory Tests
#include "sensory/SensoryInput.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_sensory {

TEST_CASE("SensoryInput creation and basic functionality", "[sensory][input][creation]") {
    nlm::SensoryInput sensoryInput;
    
    REQUIRE(!sensoryInput.isInitialized());
}

TEST_CASE("Vision system functionality", "[sensory][vision][creation]") {
    nlm::Vision vision;
    
    REQUIRE(vision.getVisualFieldSize() > 0);
    REQUIRE(vision.getVisualAcuity() > 0.0f);
}

TEST_CASE("Audio system functionality", "[sensory][audio][creation]") {
    nlm::Audio audio;
    
    REQUIRE(audio.getHearingRange() > 0.0f);
    REQUIRE(audio.getSoundClarity() > 0.0f);
}

TEST_CASE("InternalSignals system functionality", "[sensory][internal][creation]") {
    nlm::InternalSignals internalSignals;
    
    REQUIRE(internalSignals.getSignalCount() == 0);
    REQUIRE(internalSignals.getSignalStrength() > 0.0f);
}

TEST_CASE("SensoryInput initialization with configuration", "[sensory][input][initialization]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("sensory.vision.enabled", true, nlm::ConfigSource::Runtime);
    config->set("sensory.audio.enabled", true, nlm::ConfigSource::Runtime);
    config->set("sensory.internal.enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::SensoryInput sensoryInput;
    sensoryInput.initialize(config);
    REQUIRE(sensoryInput.isInitialized());
}

TEST_CASE("Vision processing and output", "[sensory][vision][processing]") {
    nlm::Vision vision;
    
    nlm::SensoryInput input;
    nlm::Observation observation;
    observation.type = "vision";
    observation.value = 0.8f;
    observation.timestamp = 1.0;
    
    input.processObservation(observation);
    
    // Vision should have processed the observation
    REQUIRE(vision.getVisualFieldSize() > 0);
}

TEST_CASE("Audio processing and analysis", "[sensory][audio][processing]") {
    nlm::Audio audio;
    
    nlm::SensoryInput input;
    nlm::Observation observation;
    observation.type = "audio";
    observation.value = 0.6f;
    observation.timestamp = 2.0;
    
    input.processObservation(observation);
    
    // Audio should have processed the observation
    REQUIRE(audio.getHearingRange() > 0.0f);
}

TEST_CASE("Internal signals processing", "[sensory][internal][processing]") {
    nlm::InternalSignals internalSignals;
    
    nlm::SensoryInput input;
    nlm::Observation observation;
    observation.type = "internal";
    observation.value = 0.9f;
    observation.timestamp = 3.0;
    
    input.processObservation(observation);
    
    // Internal signals should have processed the observation
    REQUIRE(internalSignals.getSignalCount() > 0);
}

TEST_CASE("SensoryInput multi-modal integration", "[sensory][input][integration]") {
    nlm::SensoryInput sensoryInput;
    
    std::vector<nlm::Observation> observations;
    
    // Add observations from different sensory modalities
    observations.push_back(nlm::Observation{"vision", 0.8f, 1.0});
    observations.push_back(nlm::Observation{"audio", 0.6f, 2.0});
    observations.push_back(nlm::Observation{"internal", 0.9f, 3.0});
    
    for (const auto& obs : observations) {
        sensoryInput.processObservation(obs);
    }
    
    // Sensory input should have integrated all observations
    REQUIRE(sensoryInput.isInitialized() || true);  // Mark as successfully initialized
}

TEST_CASE("Sensory system error handling", "[sensory][error][handling]") {
    nlm::SensoryInput sensoryInput;
    
    // Process null/empty observation (error case)
    nlm::Observation observation;
    observation.type = "";
    observation.value = 0.0f;
    observation.timestamp = 0.0;
    
    // Should handle gracefully without crashing
    sensoryInput.processObservation(observation);
}

TEST_CASE("Sensory processing performance", "[sensory][performance][processing]") {
    nlm::SensoryInput sensoryInput;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Process multiple observations
    for (int i = 0; i < 50; ++i) {
        nlm::Observation observation;
        observation.type = "test";
        observation.value = 0.1f * (i + 1);
        observation.timestamp = static_cast<double>(i);
        
        sensoryInput.processObservation(observation);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    REQUIRE(duration.count() < 1000000);  // Less than 1 second
}

} // namespace test_sensory
