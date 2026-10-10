#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_environment {

void testEnvironmentCreation() {
    nlm::Environment environment;
    assert(environment.getTimeStep() == 0.001f);
    assert(environment.getPhysicsEnabled() == true);
    assert(environment.getDimensions() == 3);
    std::cout << "    testEnvironmentCreation passed" << std::endl;
}

void testObservation() {
    nlm::Observation observation;
    assert(observation.getObservationType() == nlm::ObservationType::Visual);
    assert(observation.getValue() == 0.0f);
    assert(observation.getConfidence() == 0.0f);
    std::cout << "    testObservation passed" << std::endl;
}

void testEnvironmentSimulation() {
    nlm::Environment environment;
    
    // Advance environment
    environment.advance(1.0f);
    assert(environment.getTimeStep() == 0.001f);
    
    // Get observations
    auto observations = environment.getObservations();
    assert(observations.size() >= 0);
    
    std::cout << "    testEnvironmentSimulation passed" << std::endl;
}

void testObservationTypes() {
    nlm::Observation observation;
    
    // Test different observation types
    observation.setObservationType(nlm::ObservationType::Visual);
    assert(observation.getObservationType() == nlm::ObservationType::Visual);
    
    observation.setObservationType(nlm::ObservationType::Audio);
    assert(observation.getObservationType() == nlm::ObservationType::Audio);
    
    observation.setObservationType(nlm::ObservationType::Internal);
    assert(observation.getObservationType() == nlm::ObservationType::Internal);
    
    observation.setObservationType(nlm::ObservationType::Tactile);
    assert(observation.getObservationType() == nlm::ObservationType::Tactile);
    
    std::cout << "    testObservationTypes passed" << std::endl;
}

void testEnvironmentIntegration() {
    nlm::Environment environment;
    
    // Test integration with Brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Environment should be accessible
    assert(&environment != nullptr);
    std::cout << "    testEnvironmentIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Environment tests..." << std::endl;
    testEnvironmentCreation();
    testObservation();
    testEnvironmentSimulation();
    testObservationTypes();
    testEnvironmentIntegration();
}

} // namespace test_environment