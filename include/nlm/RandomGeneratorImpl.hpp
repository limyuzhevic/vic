#pragma once

#include "RandomGenerator.hpp"
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace nlm {

RandomGenerator::RandomGenerator() : RandomGenerator(std::chrono::system_clock::now().time_since_epoch().count()) {}

RandomGenerator::RandomGenerator(uint32_t seed) : generator(seed) {
    // Initialize distributions with default parameters
    uniformDist = std::uniform_real_distribution<double>(0.0, 1.0);
    normalDist = std::normal_distribution<double>(0.0, 1.0);
    exponentialDist = std::exponential_distribution<double>(1.0);
    gammaDist = std::gamma_distribution<double>(1.0, 1.0);
    betaDist = std::beta_distribution<double>(1.0, 1.0);
    chiSquareDist = std::chi_squared_distribution<double>(1.0);
    studentTDist = std::student_t_distribution<double>(1.0);
    weibullDist = std::weibull_distribution<double>(1.0, 1.0);
    extremeValueDist = std::extreme_value_distribution<double>(0.0, 1.0);
    cauchyDist = std::cauchy_distribution<double>(0.0, 1.0);
    logisticDist = std::logistic_distribution<double>(0.0, 1.0);
    paretoDist = std::pareto_distribution<double>(1.0, 1.0);
    geometricDist = std::geometric_distribution<int>(0.5);
    binomialDist = std::binomial_distribution<int>(10, 0.5);
    poissonDist = std::poisson_distribution<int>(5);
    negativeBinomialDist = std::negative_binomial_distribution<int>(10, 0.5);
    uniformIntDist = std::uniform_int_distribution<int>(0, 100);
}

double RandomGenerator::next() {
    return uniformDist(generator);
}

int RandomGenerator::nextInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator);
}

float RandomGenerator::nextFloat(float min, float max) {
    return min + static_cast<float>(next()) * (max - min);
}

double RandomGenerator::nextDouble(double min, double max) {
    return min + next() * (max - min);
}

double RandomGenerator::nextNormal() {
    return normalDist(generator);
}

double RandomGenerator::nextNormal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(generator);
}

double RandomGenerator::nextExponential(double lambda) {
    std::exponential_distribution<double> dist(lambda);
    return dist(generator);
}

double RandomGenerator::nextGamma(double shape, double scale) {
    std::gamma_distribution<double> dist(shape, scale);
    return dist(generator);
}

double RandomGenerator::nextUniform() {
    return uniformDist(generator);
}

double RandomGenerator::nextUniform(double min, double max) {
    return min + uniformDist(generator) * (max - min);
}

double RandomGenerator::nextBeta(double alpha, double beta) {
    std::beta_distribution<double> dist(alpha, beta);
    return dist(generator);
}

int RandomGenerator::nextBinomial(int n, double p) {
    std::binomial_distribution<int> dist(n, p);
    return dist(generator);
}

int RandomGenerator::nextPoisson(double lambda) {
    std::poisson_distribution<int> dist(lambda);
    return dist(generator);
}

double RandomGenerator::nextChiSquare(double degreesOfFreedom) {
    std::chi_squared_distribution<double> dist(degreesOfFreedom);
    return dist(generator);
}

double RandomGenerator::nextF(double df1, double df2) {
    std::fish_f_distribution<double> dist(df1, df2);
    return dist(generator);
}

double RandomGenerator::nextStudentT(double degreesOfFreedom) {
    std::student_t_distribution<double> dist(degreesOfFreedom);
    return dist(generator);
}

double RandomGenerator::nextBetaPrime(double alpha, double beta) {
    double u = nextBeta(alpha, beta);
    double v = nextBeta(alpha, beta);
    return u / v;
}

double RandomGenerator::nextLaplace(double mu, double b) {
    std::laplace_distribution<double> dist(mu, b);
    return dist(generator);
}

double RandomGenerator::nextLogistic(double mu, double s) {
    std::logistic_distribution<double> dist(mu, s);
    return dist(generator);
}

double RandomGenerator::nextWeibull(double shape, double scale) {
    std::weibull_distribution<double> dist(shape, scale);
    return dist(generator);
}

double RandomGenerator::nextCauchy(double location, double scale) {
    std::cauchy_distribution<double> dist(location, scale);
    return dist(generator);
}

double RandomGenerator::nextDoubleExponential(double lambda1, double lambda2) {
    if (next() < 0.5) {
        return nextExponential(lambda1);
    } else {
        return -nextExponential(lambda2);
    }
}

double RandomGenerator::nextBeta(double alpha, double beta) {
    std::beta_distribution<double> dist(alpha, beta);
    return dist(generator);
}

double RandomGenerator::nextGamma(double shape, double scale) {
    std::gamma_distribution<double> dist(shape, scale);
    return dist(generator);
}

double RandomGenerator::nextChiSquare(double degreesOfFreedom) {
    std::chi_squared_distribution<double> dist(degreesOfFreedom);
    return dist(generator);
}

double RandomGenerator::nextF(double df1, double df2) {
    std::fish_f_distribution<double> dist(df1, df2);
    return dist(generator);
}

double RandomGenerator::nextStudentT(double degreesOfFreedom) {
    std::student_t_distribution<double> dist(degreesOfFreedom);
    return dist(generator);
}

double RandomGenerator::nextBetaPrime(double alpha, double beta) {
    double u = nextBeta(alpha, beta);
    double v = nextBeta(alpha, beta);
    return u / v;
}

double RandomGenerator::nextWeibull(double shape, double scale) {
    std::weibull_distribution<double> dist(shape, scale);
    return dist(generator);
}

double RandomGenerator::nextCauchy(double location, double scale) {
    std::cauchy_distribution<double> dist(location, scale);
    return dist(generator);
}

double RandomGenerator::nextLaplace(double mu, double b) {
    std::laplace_distribution<double> dist(mu, b);
    return dist(generator);
}

double RandomGenerator::nextLogistic(double mu, double s) {
    std::logistic_distribution<double> dist(mu, s);
    return dist(generator);
}

double RandomGenerator::nextDoubleExponential(double lambda1, double lambda2) {
    if (next() < 0.5) {
        return nextExponential(lambda1);
    } else {
        return -nextExponential(lambda2);
    }
}

double RandomGenerator::nextMixture(const std::vector<double>& weights,
                                   const std::vector<std::function<double()>>& generators,
                                   const std::vector<std::vector<double>>& params) {
    if (weights.size() != generators.size()) {
        throw std::invalid_argument("Weights and generators must have the same size");
    }
    if (params.size() != generators.size()) {
        throw std::invalid_argument("Params must have the same size as generators");
    }
    
    double u = nextUniform();
    double cumulative = 0.0;
    size_t selected = 0;
    
    for (size_t i = 0; i < weights.size(); i++) {
        cumulative += weights[i];
        if (u <= cumulative) {
            selected = i;
            break;
        }
    }
    
    // Apply parameters if needed
    // In a complete implementation, this would use the params[selected] vector
    return generators[selected]();
}

std::mt19937 RandomGenerator::createGenerator(uint32_t seed) {
    return std::mt19937(seed);
}
}