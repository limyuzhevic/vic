#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Multi-objective solution
struct MultiObjectiveSolution {
    std::string id;
    std::vector<double> objectives;  // Objective values to optimize
    std::vector<double> decisionVariables;  // Decision variables
    std::map<std::string, double> constraintValues;
    double crowdingDistance;
    double fitness;  // Combined fitness value
    bool dominated;
    
    MultiObjectiveSolution(const std::string& solutionId,
                          const std::vector<double>& obj,
                          const std::vector<double>& vars)
        : id(solutionId), objectives(obj), decisionVariables(vars),
          crowdingDistance(0.0), fitness(0.0), dominated(false) {}
};

// Pareto dominance checker
class ParetoDominance {
public:
    static bool dominates(const MultiObjectiveSolution& a,
                         const MultiObjectiveSolution& b) {
        bool betterInOneObjective = false;
        
        for (size_t i = 0; i < a.objectives.size() && i < b.objectives.size(); ++i) {
            if (a.objectives[i] < b.objectives[i]) {
                betterInOneObjective = true;
            } else if (a.objectives[i] > b.objectives[i]) {
                return false;  // b is better in this objective
            }
        }
        
        return betterInOneObjective;
    }
};

// Non-dominated sorting
class NonDominatedSorting {
private:
    std::vector<std::vector<MultiObjectiveSolution>> fronts;
    std::vector<int> dominationCounts;
    
public:
    void sort(std::vector<MultiObjectiveSolution>& solutions) {
        fronts.clear();
        
        // Initialize domination counts
        dominationCounts.assign(solutions.size(), 0);
        
        // Find domination relationships
        for (size_t i = 0; i < solutions.size(); ++i) {
            for (size_t j = 0; j < solutions.size(); ++j) {
                if (i == j) continue;
                
                if (ParetoDominance::dominates(solutions[j], solutions[i])) {
                    dominationCounts[i]++;
                }
            }
        }
        
        // Create first front
        std::vector<MultiObjectiveSolution> front1;
        for (size_t i = 0; i < solutions.size(); ++i) {
            if (dominationCounts[i] == 0) {
                solutions[i].dominated = false;
                front1.push_back(solutions[i]);
            } else {
                solutions[i].dominated = true;
            }
        }
        
        fronts.push_back(front1);
        
        // Create subsequent fronts
        std::vector<MultiObjectiveSolution> remaining = solutions;
        remaining.erase(remaining.end() - front1.size(), remaining.end());
        
        while (!remaining.empty()) {
            std::vector<MultiObjectiveSolution> currentFront;
            
            for (size_t i = 0; i < remaining.size(); ++i) {
                bool allDominatorsFound = true;
                
                for (size_t j = 0; j < fronts.size(); ++j) {
                    for (const auto& frontSolution : fronts[j]) {
                        if (ParetoDominance::dominates(frontSolution, remaining[i])) {
                            allDominatorsFound = false;
                            break;
                        }
                    }
                    if (!allDominatorsFound) break;
                }
                
                if (allDominatorsFound) {
                    remaining[i].dominated = false;
                    currentFront.push_back(remaining[i]);
                }
            }
            
            // Remove front solutions from remaining
            std::vector<MultiObjectiveSolution> newRemaining;
            for (size_t i = 0; i < remaining.size(); ++i) {
                bool isInCurrentFront = false;
                for (const auto& frontSolution : currentFront) {
                    if (frontSolution.id == remaining[i].id) {
                        isInCurrentFront = true;
                        break;
                    }
                }
                if (!isInCurrentFront) {
                    newRemaining.push_back(remaining[i]);
                }
            }
            
            fronts.push_back(currentFront);
            remaining = newRemaining;
        }
        
        // Update solutions in original vector
        size_t index = 0;
        for (const auto& front : fronts) {
            for (const auto& solution : front) {
                if (index < solutions.size()) {
                    solutions[index] = solution;
                    index++;
                }
            }
        }
        solutions.resize(index);
    }
    
    const std::vector<std::vector<MultiObjectiveSolution>>& getFronts() const {
        return fronts;
    }
};

// Crowding distance calculator
class CrowdingDistanceCalculator {
public:
    static void calculateCrowdingDistance(std::vector<MultiObjectiveSolution>& front,
                                         const std::vector<double>& minBounds,
                                         const std::vector<double>& maxBounds) {
        if (front.size() <= 2) {
            for (auto& solution : front) {
                solution.crowdingDistance = std::numeric_limits<double>::max();
            }
            return;
        }
        
        // Initialize crowding distances
        for (auto& solution : front) {
            solution.crowdingDistance = 0.0;
        }
        
        // Calculate crowding distance for each objective
        for (size_t obj = 0; obj < front[0].objectives.size(); ++obj) {
            // Sort solutions by objective value
            std::sort(front.begin(), front.end(),
                     [obj](const MultiObjectiveSolution& a, const MultiObjectiveSolution& b) {
                         return a.objectives[obj] < b.objectives[obj];
                     });
            
            // Set boundary solutions to max distance
            front[0].crowdingDistance = std::numeric_limits<double>::max();
            front.back().crowdingDistance = std::numeric_limits<double>::max();
            
            if (maxBounds[obj] > minBounds[obj]) {
                double range = maxBounds[obj] - minBounds[obj];
                for (size_t i = 1; i < front.size() - 1; ++i) {
                    if (i > 0 && i < front.size() - 1) {
                        double prevDiff = front[i].objectives[obj] - front[i-1].objectives[obj];
                        double nextDiff = front[i+1].objectives[obj] - front[i].objectives[obj];
                        
                        front[i].crowdingDistance += (prevDiff + nextDiff) / range;
                    }
                }
            }
        }
    }
};

// Evolution strategy operator
class EvolutionStrategy {
private:
    std::mt19937 rng;
    double mutationRate;
    double crossoverRate;
    double sigma;
    
public:
    EvolutionStrategy(double mutationRate = 0.01, double crossoverRate = 0.8,
                     double sigma = 0.1)
        : mutationRate(mutationRate), crossoverRate(crossoverRate), sigma(sigma),
          rng(std::random_device{}()) {}
    
    std::vector<double> mutate(const std::vector<double>& solution,
                               const std::vector<double>& bounds) {
        std::vector<double> mutated = solution;
        std::normal_distribution<double> distribution(0.0, sigma);
        
        for (size_t i = 0; i < mutated.size(); ++i) {
            if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) < mutationRate) {
                double perturbation = distribution(rng);
                mutated[i] += perturbation;
                
                // Apply bounds
                if (mutated[i] < bounds[i]) {
                    mutated[i] = bounds[i] + (bounds[i+1] - bounds[i]) * std::uniform_real_distribution<double>(0.0, 1.0)(rng);
                } else if (mutated[i] > bounds[i + 1]) {
                    mutated[i] = bounds[i] + (bounds[i+1] - bounds[i]) * std::uniform_real_distribution<double>(0.0, 1.0)(rng);
                }
            }
        }
        
        return mutated;
    }
    
    std::vector<double> crossover(const std::vector<double>& parent1,
                                  const std::vector<double>& parent2) {
        if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) > crossoverRate) {
            return parent1;  // Return first parent
        }
        
        std::vector<double> child = parent1;
        for (size_t i = 0; i < parent1.size(); ++i) {
            if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) < 0.5) {
                child[i] = parent2[i];
            }
        }
        
        return child;
    }
};

// Advanced multi-objective optimization
class AdvancedMultiObjectiveOptimization {
private:
    std::vector<MultiObjectiveSolution> population;
    std::vector<MultiObjectiveSolution> paretoFront;
    std::unique_ptr<NonDominatedSorting> sorter;
    std::unique_ptr<CrowdingDistanceCalculator> crowdingCalculator;
    std::unique_ptr<EvolutionStrategy> evolutionStrategy;
    std::vector<double> objectiveBounds;
    std::vector<double> variableBounds;
    double convergenceThreshold;
    double populationSize;
    int generationCount;
    
public:
    AdvancedMultiObjectiveOptimization(const Config& config) {
        sorter = std::make_unique<NonDominatedSorting>();
        crowdingCalculator = std::make_unique<CrowdingDistanceCalculator>();
        
        populationSize = config.getOr("moo.population_size", 100.0);
        convergenceThreshold = config.getOr("moo.convergence_threshold", 0.95);
        generationCount = config.getOr("moo.max_generations", 1000);
        
        double mutationRate = config.getOr("moo.mutation_rate", 0.01);
        double crossoverRate = config.getOr("moo.crossover_rate", 0.8);
        double sigma = config.getOr("moo.sigma", 0.1);
        
        evolutionStrategy = std::make_unique<EvolutionStrategy>(
            mutationRate, crossoverRate, sigma);
        
        // Initialize bounds (in real implementation, would be configured)
        objectiveBounds = {0.0, 100.0, 50.0};  // min, max for each objective
        variableBounds.resize(populationSize * 2 + 1, 0.0);  // Dummy bounds
        for (size_t i = 0; i < variableBounds.size(); i += 2) {
            variableBounds[i] = 0.0;
            variableBounds[i + 1] = 1.0;
        }
    }
    
    void initializePopulation() {
        population.clear();
        
        // Create initial random population
        for (int i = 0; i < populationSize; ++i) {
            std::string id = "solution_" + std::to_string(i);
            
            // Generate random decision variables
            std::vector<double> decisionVars;
            for (size_t j = 0; j < variableBounds.size(); j += 2) {
                double var = variableBounds[j] + 
                           std::uniform_real_distribution<double>(0.0, 1.0)(evolutionStrategy->rng) *
                           (variableBounds[j + 1] - variableBounds[j]);
                decisionVars.push_back(var);
            }
            
            // Generate random objective values based on decision variables
            std::vector<double> objectives;
            for (size_t j = 0; j < objectiveBounds.size(); j += 2) {
                double obj = variableBounds[j] +
                           std::uniform_real_distribution<double>(0.0, 1.0)(evolutionStrategy->rng) *
                           (variableBounds[j + 1] - variableBounds[j]);
                objectives.push_back(obj);
            }
            
            population.emplace_back(id, objectives, decisionVars);
        }
    }
    
    void runOptimization(const Brain& brain, const Config& config) {
        initializePopulation();
        
        int maxGenerations = config.getOr("moo.max_generations", 1000);
        double convergenceRate = config.getOr("moo.convergence_rate", 0.01);
        
        for (generationCount = 0; generationCount < maxGenerations; ++generationCount) {
            // Sort population using non-dominated sorting
            sorter->sort(population);
            
            // Select first front as current Pareto front
            paretoFront.clear();
            const auto& fronts = sorter->getFronts();
            if (!fronts.empty()) {
                for (const auto& front : fronts) {
                    paretoFront.insert(paretoFront.end(), front.begin(), front.end());
                }
            }
            
            // Calculate crowding distance for current front
            if (!paretoFront.empty()) {
                crowdingCalculator->calculateCrowdingDistance(paretoFront, objectiveBounds, variableBounds);
            }
            
            // Check convergence
            double hyperVolume = calculateHyperVolume(paretoFront);
            if (hyperVolume > convergenceThreshold) {
                break;
            }
            
            // Create next generation
            std::vector<MultiObjectiveSolution> newPopulation;
            
            // Elitism: keep best solutions from current front
            size_t eliteCount = std::min(static_cast<size_t>(populationSize * 0.2), paretoFront.size());
            for (size_t i = 0; i < eliteCount; ++i) {
                newPopulation.push_back(paretoFront[i]);
            }
            
            // Fill remaining slots with offspring
            while (newPopulation.size() < populationSize) {
                // Selection (tournament selection)
                MultiObjectiveSolution parent1 = selectParent(paretoFront);
                MultiObjectiveSolution parent2 = selectParent(paretoFront);
                
                // Crossover
                std::vector<double> childVars = evolutionStrategy->crossover(
                    parent1.decisionVariables, parent2.decisionVariables);
                
                // Mutation
                childVars = evolutionStrategy->mutate(childVars, variableBounds);
                
                // Evaluate child
                std::vector<double> childObjectives = evaluateObjectives(childVars, brain);
                
                newPopulation.emplace_back("child_" + std::to_string(newPopulation.size()),
                                         childObjectives, childVars);
            }
            
            population = std::move(newPopulation);
        }
    }
    
    std::vector<double> evaluateObjectives(const std::vector<double>& vars,
                                           const Brain& brain) {
        std::vector<double> objectives;
        
        // Evaluate objectives based on brain state and variables
        double excitation = brain.getGlobalExcitation();
        double inhibition = brain.getGlobalInhibition();
        
        // Simple objective functions
        objectives.push_back(vars.size());  // Minimization objective
        objectives.push_back(1.0 - excitation / 100.0);  // Maximization objective
        objectives.push_back(vars.empty() ? 0.0 : vars[0]);  // Middle objective
        
        return objectives;
    }
    
    MultiObjectiveSolution selectParent(const std::vector<MultiObjectiveSolution>& paretoFront) {
        if (paretoFront.empty()) {
            throw std::runtime_error("Cannot select parent from empty Pareto front");
        }
        
        // Tournament selection with crowding distance bias
        std::uniform_int_distribution<int> dist(0, paretoFront.size() - 1);
        int idx1 = dist(evolutionStrategy->rng);
        int idx2 = dist(evolutionStrategy->rng);
        
        if (paretoFront[idx1].crowdingDistance > paretoFront[idx2].crowdingDistance) {
            return paretoFront[idx1];
        } else {
            return paretoFront[idx2];
        }
    }
    
    double calculateHyperVolume(const std::vector<MultiObjectiveSolution>& front) {
        if (front.empty()) return 0.0;
        
        // Simple hypervolume calculation (in real implementation, would be more sophisticated)
        double volume = 0.0;
        for (const auto& solution : front) {
            for (size_t i = 0; i < solution.objectives.size(); ++i) {
                volume += solution.objectives[i];
            }
        }
        
        return volume / front.size() / front[0].objectives.size();
    }
    
    std::string getOptimizationSummary() const {
        std::stringstream ss;
        ss << "Multi-Objective Optimization Summary:\n";
        ss << "  Generations: " << generationCount << "\n";
        ss << "  Population Size: " << population.size() << "\n";
        ss << "  Pareto Front Size: " << paretoFront.size() << "\n";
        ss << "  Hypervolume: " << calculateHyperVolume(paretoFront) << "\n";
        ss << "  Convergence Rate: " << (generationCount / static_cast<double>(generationCount)) << "\n";
        return ss.str();
    }
    
    std::vector<std::vector<double>> getParetoOptimalSolutions() const {
        std::vector<std::vector<double>> solutions;
        for (const auto& solution : paretoFront) {
            solutions.push_back(solution.objectives);
        }
        return solutions;
    }
    
    void saveResults(const Config& config) const {
        std::string filename = config.getOr("moo.results_file", "moo_results.json");
        // Implementation for saving results
    }
    
    void loadResults(const Config& config) {
        std::string filename = config.getOr("moo.results_file", "moo_results.json");
        // Implementation for loading results
    }
};

} // namespace nlm
