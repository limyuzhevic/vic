#pragma once

namespace nlm {

class Brain;

class TestRunner {
public:
    static void runBasicConnectivityTest(const std::shared_ptr<Brain>& brain);
    static void runPlasticityExperiment(const std::shared_ptr<Brain>& brain);
    static void runStdpVerification(const std::shared_ptr<Brain>& brain);
};

} // namespace nlm
