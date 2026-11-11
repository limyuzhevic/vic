// Phase3Demo.hpp
// Declares the Phase3Demo class for world interaction demonstration

#pragma once

#include "DemoBase.hpp"

namespace nlm {

class Phase3Demo : public DemoBase {
public:
    Phase3Demo();
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) override;
    
    void addDemoOptions(int& argc, char** argv, int& currentArg) override;
    void printHelp() override;
    
private:
    void configureEnvironment(std::shared_ptr<Config> config) override;
    void configureBrain(std::shared_ptr<Config> config) override;
    
    const char* motorCommandToString(MotorCommand cmd);
    const char* stageToString(DevelopmentalStage stage);
};

} // namespace nlm