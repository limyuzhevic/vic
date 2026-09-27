#pragma once

// Command declarations for CLI

namespace nlm {

// Command declarations - these will be implemented in CLI.cpp
class LoadConfigCommand;
class SaveConfigCommand;
class ExportConfigCommand;
class ValidateConfigCommand;
class SetParamCommand;
class GetParamCommand;
class ListParamsCommand;

class RunCommand;
class ResetCommand;
class StatusCommand;

class InspectBrainCommand;
class CheckNeuralCommand;
class AnalyzePlasticityCommand;

class QuietCommand;
class VerboseCommand;
class JSONOutputCommand;
class CSVOutputCommand;

class BatchCommand;
class ScenarioCommand;
class ProfileCommand;

class VersionCommand;
class HelpCommand;
class BenchmarkCommand;

// Function to register all commands with the CLI
void registerAllCommands(CLI& cli);

} // namespace nlm