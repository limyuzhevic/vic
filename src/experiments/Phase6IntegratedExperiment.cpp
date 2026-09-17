#include "Phase6IntegratedExperiment.hpp"
#include "Phase6ConfigManager.hpp"
#include "ExperimentFactory.hpp"
#include "SystemCreators.hpp"
#include "ExperimentRunnerImpl.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <chrono>
#include <cmath>

namespace nlm {

Phase6IntegratedExperiment::Phase6IntegratedExperiment()
    : configManager_(std::make_unique<Phase6ConfigManager>()),
      systemCreator_(std::make_unique<SystemCreators>()),
      experimentRunner_(std::make_unique<ExperimentRunnerImpl>()) {}

Phase6IntegratedExperiment::~Phase6IntegratedExperiment() = default;

Phase6IntegrationResult Phase6IntegratedExperiment::run(const Phase6Config& config) {
    return experimentRunner_->run(config);
}

bool Phase6IntegratedExperiment::verifyIntegration() {
    return experimentRunner_->verifyIntegration();
}

bool Phase6IntegratedExperiment::testMemoryIntegration() {
    return experimentRunner_->testMemoryIntegration();
}

bool Phase6IntegratedExperiment::testNeuromodulationIntegration() {
    return experimentRunner_->testNeuromodulationIntegration();
}

bool Phase6IntegratedExperiment::testCheckpointing() {
    return experimentRunner_->testCheckpointing();
}

bool Phase6IntegratedExperiment::testReplay() {
    return experimentRunner_->testReplay();
}

} // namespace nlm
