#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <random>
#include <map>
#include <chrono>
#include <cmath>

namespace nlm {

class ParameterTuner {
private:
    struct ParameterRange {
        std::string name;
        double minValue;
        double maxValue;
        double stepSize;
        std::string distribution; // "uniform", "normal"
        double normalMean;
        double normalStddev;
        
        ParameterRange(const std::string& n, double minV, double maxV, 
                      double step = 0.1, const std::string& dist = "uniform",
                      double mean = 0.0, double stddev = 1.0)
            : name(n), minValue(minV), maxValue(maxV), stepSize(step),
              distribution(dist), normalMean(mean), normalStddev(stddev) {}
    };
    
    struct ExperimentResult {
        std::vector<double> parameters;
        double fitnessScore;
        std::chrono::high_resolution_clock::duration executionTime;
        bool converged;
        
        ExperimentResult(const std::vector<double>& params, double fitness,
                        const std::chrono::high_resolution_clock::duration& time,
                        bool conv = false)
            : parameters(params), fitnessScore(fitness), executionTime(time),
              converged(conv) {}
    };
    
    std::shared_ptr<Config> config;
    std::vector<ParameterRange> parameterRanges;
    int populationSize;
    int maxGenerations;
    double mutationRate;
    double crossoverRate;
    bool useTournamentSelection;
    int tournamentSize;
    std::string optimizationGoal; // "maximize", "minimize"
    bool verbose;
    bool saveResults;
    
    // Random number generation
    std::mt19937 rng;
    
    // Evolution algorithm
    std::vector<std::vector<double>> initializePopulation();
    double evaluateFitness(const std::vector<double>& parameters);
    std::vector<double> tournamentSelection(const std::vector<std::vector<double>>& population,
                                           const std::vector<double>& fitnesses);
    std::vector<double> crossover(const std::vector<double>& parent1,
                                const std::vector<double>& parent2);
    std::vector<double> mutate(const std::vector<double>& individual);
    std::vector<double> generateRandomParameters();
    
    // Configuration and setup
    void setupParameterRanges();
    void setupEvolutionConfig();
    void setupTunerConfig();
    
    // Main algorithm
    ExperimentResult runGeneticAlgorithm();
    std::vector<ExperimentResult> runParameterSearch();
    
    // Output and reporting
    void printResults(const ExperimentResult& result);
    void printPopulationStats(const std::vector<ExperimentResult>& results);
    void saveResultsToFile(const ExperimentResult& result, const std::string& filename);
    void exportBestParametersToConfig(const std::vector<double>& bestParameters);
    
    // Command processing
    void runParameterTuner(int argc, char* argv[]);
    void showHelp();
    void showCurrentConfig();
    
public:
    ParameterTuner();
    ~ParameterTuner();
    
    void run(int argc, char* argv[]);
    void printBanner();
};

} // namespace nlm
