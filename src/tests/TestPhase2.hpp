// Test Headers for Phase 2: Real Neural Computation Tests
// These headers provide clean interfaces for the main test functions

#pragma once

// Basic connectivity test
void runBasicConnectivityTest(std::shared_ptr<nlm::Brain> brain);

// Plasticity experiment test
void runPlasticityExperimentTest(std::shared_ptr<nlm::Brain> brain);

// STDP verification test
void runStdpVerificationTest(std::shared_ptr<nlm::Brain> brain);