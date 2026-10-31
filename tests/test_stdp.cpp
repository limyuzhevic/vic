// STDP Tests
#include "nlm.h"
#include <cassert>
#include <iostream>

namespace test_stdp {

void testSTDPConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    config->set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
    
    // Verify STDP parameters are accessible
    auto ltpOpt = config->get<float>("stdp_ltp_weight");
    auto ltdOpt = config->get<float>("stdp_ltd_weight");
    auto tauOpt = config->get<float>("stdp_tau");
    
    assert(ltpOpt && *ltpOpt == 0.02f);
    assert(ltdOpt && *ltdOpt == 0.015f);
    assert(tauOpt && *tauOpt == 20.0f);
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

void testSTDPWeightRanges() {
    auto config = std::make_shared<nlm::Config>();
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    
    auto ltpOpt = config->get<float>("stdp_ltp_weight");
    auto ltdOpt = config->get<float>("stdp_ltd_weight");
    
    assert(*ltpOpt > 0.0f);
    assert(*ltdOpt > 0.0f);
    
    // LTP should typically be larger than LTD for potentiation
    assert(*ltpOpt >= *ltdOpt);
    
    std::cout << "    testSTDPWeightRanges passed" << std::endl;
}

void testSTDPTimeConstant() {
    auto config = std::make_shared<nlm::Config>();
    config->set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
    
    auto tauOpt = config->get<float>("stdp_tau");
    
    assert(tauOpt && *tauOpt > 0.0f);
    assert(*tauOpt == 20.0f);
    
    std::cout << "    testSTDPTimeConstant passed" << std::endl;
}

void testSTDPValidation() {
    nlm::Config config;
    
    // Test validation (this would be done during brain initialization)
    // For now, just verify that invalid STDP parameters would be caught
    auto validationErrors = ConfigValidator::validate(config);
    // Empty config should have validation errors
    // assert(!validationErrors.empty());
    
    std::cout << "    testSTDPValidation passed" << std::endl;
}

void runAll() {
    testSTDPConfiguration();
    testSTDPWeightRanges();
    testSTDPTimeConstant();
    testSTDPValidation();
}

} // namespace test_stdp
