#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/Vision.hpp"
#include "../motor/MotorSystem.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>
#include <map>
#include <algorithm>
#include <termios.h>
#include <unistd.h>

namespace nlm {

class VisualizationDashboard {
private:
    struct VisualizationData {
        std::chrono::high_resolution_clock::time_point timestamp;
        float brainActivity;
        float firingRate;
        float novelty;
        float curiosity;
        float neuromodulation;
        float energy;
        std::vector<float> visionData;
        std::vector<float> motorOutput;
        std::string action;
        float reward;
        
        VisualizationData() : brainActivity(0.0f), firingRate(0.0f), novelty(0.0f),
                            curiosity(0.0f), neuromodulation(0.0f), energy(0.0f),
                            reward(0.0f) {}
    };
    
    std::vector<VisualizationData> dataHistory;
    std::shared_ptr<Config> config;
    bool running;
    bool paused;
    bool saveToFile;
    std::string outputFile;
    int refreshRate;
    int dataPoints;
    std::ofstream dataOutputFile;
    
    // Terminal controls
    struct termios originalTermios;
    
    // Dashboard controls
    void setupTerminal();
    void restoreTerminal();
    bool kbhit();
    char getch();
    
    // Display functions
    void clearScreen();
    void displayHeader();
    void displaySystemInfo();
    void displayBrainActivity();
    void displayNeuromodulation();
    void displaySensoryData();
    void displayMotorOutput();
    void displayMetrics();
    void displayControls();
    void displayHelp();
    
    // Data collection and processing
    void collectData(std::shared_ptr<Brain> brain, std::shared_ptr<Vision> vision,
                   std::shared_ptr<MotorSystem> motorSystem, float reward);
    void processData();
    void saveData();
    
    // Configuration
    void setupDashboardConfig();
    
    // Main loop
    void runDashboard(int argc, char* argv[]);
    void dashboardLoop(std::shared_ptr<Brain> brain, std::shared_ptr<Vision> vision,
                      std::shared_ptr<MotorSystem> motorSystem);
    
public:
    VisualizationDashboard();
    ~VisualizationDashboard();
    
    void run(int argc, char* argv[]);
    void printBanner();
    void showHelp();
};

} // namespace nlm
