#pragma once

#include "CommandLineInterface.hpp"

namespace nlm {

class InteractiveCommand : public CommandInterface {
public:
    InteractiveCommand();
    ~InteractiveCommand() override;
    void execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) override;
    std::string getName() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class RunCommand : public CommandInterface {
public:
    RunCommand();
    ~RunCommand() override;
    void execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) override;
    std::string getName() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class BenchmarkCommand : public CommandInterface {
public:
    BenchmarkCommand();
    ~BenchmarkCommand() override;
    void execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) override;
    std::string getName() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class TrainCommand : public CommandInterface {
public:
    TrainCommand();
    ~TrainCommand() override;
    void execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) override;
    std::string getName() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
