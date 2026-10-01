// DemoRunner.hpp
// Demo mode implementations for NLM

#ifndef NLM_DEMORUNNER_HPP
#define NLM_DEMORUNNER_HPP

#include <string>

namespace nlm {

class DemoRunner {
public:
    static void runPhase2Demo();
    static void runPhase4Demo();
    static void runPhase6Demo();
    static void runBenchmarks();
    static void runIntegrationTests();
    static void runCustomExperiment();
};

} // namespace nlm

#endif // NLM_DEMORUNNER_HPP