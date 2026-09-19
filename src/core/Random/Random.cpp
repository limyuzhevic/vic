bool RandomGenerator::bernoulli(double p) {
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("p must be in range [0,1] for bernoulli distribution");
    }
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda < 0.0) {
        throw std::invalid_argument("lambda must be >= 0 for poisson distribution");
    }
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be > 0 for exponential distribution");
    }
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->gen);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    if (!data && size > 0) {
        throw std::invalid_argument("data pointer cannot be null for fillBytes");
    }
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
    }
}