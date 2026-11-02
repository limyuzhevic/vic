# NLM Development Environment Configuration
# Provides IDE templates and debugging setups for optimal development experience

## Visual Studio Code (VSCode) Configuration

### Required Extensions

Install the following VSCode extensions for optimal NLM development:

```json
{
    "extensions": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "ms-vscode.cpptools-extension-pack",
        "vertexclojure.vscode-clojure",
        "maruspr.static-analysis-essentials",
        "eamodio.gitlens",
        "knisterpeter.vscode-class-leak-detector",
        "redhat.vscode-yaml",
        "docker.docker",
        "streetsidesoftware.code-spell-checker",
        "editorconfig.editorconfig",
        "mikestead.toml"
    ]
}
```

### VSCode Settings

Create `.vscode/settings.json`:

```json
{
    "cpptools.enableUpdateChannelNotification": false,
    "files.associations": {
        "*.cfg": "ini",
        "*.md": "markdown",
        "CMakeLists.txt": "cmake",
        "Dockerfile": "dockerfile"
    },
    "editor.formatOnSave": true,
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.detectIndentation": false,
    "files.trimTrailingWhitespace": true,
    "search.exclude": {
        "**/.git": true,
        "**/.hg": true,
        "**/.svn": true,
        "**/.gitignore": true,
        "**/.gitignore_global": true,
        "**/.dockerignore": true,
        "**/build": true,
        "**/.vscode": true,
        "**/.idea": true,
        "**/__pycache__": true,
        "**/*.pyc": true,
        "**/*.so": true,
        "**/*.dylib": true,
        "**/*.dll": true,
        "**/*.a": true,
        "**/*.lib": true
    },
    "cmake.sourceDirectory": "${workspaceFolder}",
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.configureSettings": {
        "CMAKE_BUILD_TYPE": "Release",
        "NLM_BUILD_PYTHON_BINDINGS": "OFF"
    },
    "cmake.generateDoNotBuildFromCMake": true,
    "cmake.autoGenerateCommands": "onFirstOpen",
    "cmake.alwaysShowLog": false,
    "cmake.formatOnSave": true,
    "C_Cpp.default.cppStandard": "20",
    "C_Cpp.intelliSenseEngine": "default",
    "C_Cpp.SuggestSnippets": false,
    "C_Cpp.RustAnalyzer.enabled": false,
    "C_Cpp.ClangTidy.enabled": true,
    "C_Cpp.ClangTidy.path": "${workspaceFolder}/build/clang-tidy",
    "C_Cpp.clangd.path": "${workspaceFolder}/build/clangd",
    "C_Cpp.errorSquiggles": "Enabled",
    "C_Cpp.suggestCpp11Features": false,
    "C_Cpp.suggestCpp14Features": false,
    "C_Cpp.suggestCpp17Features": false,
    "C_Cpp.suggestCpp20Features": true,
    "C_Cpp.codeAnalysis.showDiagnostics": "First Error",
    "C_Cpp.codeAnalysis.severity": "Warning",
    "C_Cpp.codeAnalysis.provider": "ClangTidy",
    "C_Cpp.codeAnalysis.includePath": [
        "${workspaceFolder}/src",
        "${workspaceFolder}/src/brain",
        "${workspaceFolder}/src/core",
        "${workspaceFolder}/src/experiments"
    ],
    "C_Cpp.codeAnalysis.defines": [
        "NLM_VERSION_MAJOR=0",
        "NLM_VERSION_MINOR=1",
        "NLM_VERSION_PATCH=0",
        "NLM_BUILD_TYPE=\"Release\""
    ],
    "C_Cpp.codeAnalysis.macros": {
        "DEBUG": "0",
        "NDEBUG": "1",
        "HAVE_CONFIG_H": "1"
    },
    "C_Cpp.intelliSenseEngineFallback": "Enabled",
    "C_Cpp.useCppCommentCompletions": true,
    "C_Cpp.useVivideComments": false,
    "C_Cpp.suggestLocalSymbols": true,
    "C_Cpp.suggestGlobalSymbols": true,
    "C_Cpp.allowSingleClickFolding": true,
    "C_Cpp.showMissingIncludeSupport": true,
    "C_Cpp.errorValidation": "Enabled",
    "C_Cpp.enableConfigurationTracking": false,
    "C_Cpp.enableCpp11Intellisense": true,
    "C_Cpp.enhancedIntelliSense": true,
    "C_Cpp.cacheIntelliSense": true,
    "C_Cpp.cacheLocation": "${workspaceFolder}/.cache/caches",
    "C_Cpp.disableAutoDimComments": false,
    "C_Cpp.preferredSeparator": "newline",
    "C_Cpp.styleOverridePath": "${workspaceFolder}/.vscode/cppstyle.json",
    "C_Cpp.browse.filter": {
        "kind": [
            "namespace",
            "class",
            "enum",
            "function",
            "typedef",
            "variable"
        ],
        "access": [
            "public",
            "protected",
            "private"
        ]
    },
    "C_Cpp.experimental.enableClangd": true,
    "C_Cpp.experimental.useCMakeDependencyParsing": true,
    "C_Cpp.experimental.useCacheJsonCompilation": true,
    "C_Cpp.experimental.useHeaderAnalysis": true,
    "C_Cpp.experimental.useIntelliSenseFileCache": true,
    "C_Cpp.experimental.useModuleCompilation": true,
    "C_Cpp.experimental.usePerFileCache": true,
    "C_Cpp.experimental.useResolveSymlinks": true,
    "C_Cpp.experimental.useUnmergedIncludes": true,
    "C_Cpp.experimental.useWorkspaceSymbols": true,
    "C_Cpp.experimental.useWorkspaceSymbolsSource": "compile_commands",
    "C_Cpp.experimental.useCMakeCache": true,
    "C_Cpp.experimental.useEnvironmentVariables": true,
    "C_Cpp.experimental.useExecutablePaths": true,
    "C_Cpp.experimental.useIncludePathEntries": true,
    "C_Cpp.experimental.useMacroDefinitions": true,
    "C_Cpp.experimental.usePragmas": true,
    "C_Cpp.experimental.usePrecompiledHeaders": true,
    "C_Cpp.experimental.useProgressReporting": true,
    "C_Cpp.experimental.useProjectIncludes": true,
    "C_Cpp.experimental.useSearchDirectories": true,
    "C_Cpp.experimental.useSourceFileComments": true,
    "C_Cpp.experimental.useStandardHeaders": true,
    "C_Cpp.experimental.useSystemIncludes": true,
    "C_Cpp.experimental.useTargetDefines": true,
    "C_Cpp.experimental.useTargetIncludes": true,
    "C_Cpp.experimental.useTargetLinkLibraries": true,
    "C_Cpp.experimental.useTargetCompileDefinitions": true,
    "C_Cpp.experimental.useTargetCompileOptions": true,
    "C_Cpp.experimental.useTargetCompileFeatures": true,
    "C_Cpp.experimental.useTargetSourceProperties": true,
    "C_Cpp.experimental.useTargetSourceGroups": true,
    "C_Cpp.experimental.useTargetExcludeFromAll": true,
    "C_Cpp.experimental.useTargetObjectLibraries": true,
    "C_Cpp.experimental.useTargetModule": true,
    "C_Cpp.experimental.useTargetImportedTargets": true,
    "C_Cpp.experimental.useTargetAliasTargets": true,
    "C_Cpp.experimental.useTargetInterfaceLibraries": true,
    "C_Cpp.experimental.useTargetGenerateExportHeader": true,
    "C_Cpp.experimental.useTargetCreateGeneratedSources": true,
    "C_Cpp.experimental.useTargetAddDependencies": true,
    "C_Cpp.experimental.useTargetAddCustomCommand": true,
    "C_Cpp.experimental.useTargetAddCustomTarget": true,
    "C_Cpp.experimental.useTargetAddInstallTargets": true,
    "C_Cpp.experimental.useTargetAddTest": true,
    "C_Cpp.experimental.useTargetAddExamples": true,
    "C_Cpp.experimental.useTargetAddBenchmarks": true,
    "C_Cpp.experimental.useTargetAddExperimental": true,
    "C_Cpp.experimental.useTargetAddDocs": true,
    "C_Cpp.experimental.useTargetAddPackageConfig": true,
    "C_Cpp.experimental.useTargetAddPkgConfig": true,
    "C_Cpp.experimental.useTargetAddFindPackage": true,
    "C_Cpp.experimental.useTargetFindDependency": true,
    "C_Cpp.experimental.useTargetFindPackagePath": true,
    "C_Cpp.experimental.useTargetFindModule": true,
    "C_Cpp.experimental.useTargetFindLibrary": true,
    "C_Cpp.experimental.useTargetFindPath": true,
    "C_Cpp.experimental.useTargetFindFile": true,
    "C_Cpp.experimental.useTargetFindResources": true,
    "C_Cpp.experimental.useTargetFindHeaders": true,
    "C_Cpp.experimental.useTargetFindSources": true,
    "C_Cpp.experimental.useTargetFindCMake": true,
    "C_Cpp.experimental.useTargetFindMakefile": true,
    "C_Cpp.experimental.useTargetFindScript": true,
    "C_Cpp.experimental.useTargetFindPackage": true,
    "C_Cpp.experimental.useTargetFindLibrary": true,
    "C_Cpp.experimental.useTargetFindPath": true,
    "C_Cpp.experimental.useTargetFindFile": true,
    "C_Cpp.experimental.useTargetFindResources": true,
    "C_Cpp.experimental.useTargetFindHeaders": true,
    "C_Cpp.experimental.useTargetFindSources": true,
    "C_Cpp.experimental.useTargetFindCMake": true,
    "C_Cpp.experimental.useTargetFindMakefile": true,
    "C_Cpp.experimental.useTargetFindScript": true
    },
    "C_Cpp.experimental.useAdvancedClangd": true,
    "C_Cpp.experimental.useCodeNavigation": true,
    "C_Cpp.experimental.useCrossFileSymbolInformation": true,
    "C_Cpp.experimental.useDataFlowAnalysis": true,
    "C_Cpp.experimental.useDeclarativeCompletions": true,
    "C_Cpp.experimental.useDiagnoser": true,
    "C_Cpp.experimental.useEditorServices": true,
    "C_Cpp.experimental.useEditingCapabilities": true,
    "C_Cpp.experimental.useFileSystemWatcher": true,
    "C_Cpp.experimental.useGotoDefinition": true,
    "C_Cpp.experimental.useHover": true,
    "C_Cpp.experimental.useInlayHints": true,
    "C_Cpp.experimental.useInlineCompletions": true,
    "C_Cpp.experimental.useLanguageServer": true,
    "C_Cpp.experimental.useLocalization": true,
    "C_Cpp.experimental.useNavigation": true,
    "C_Cpp.experimental.useParameterHints": true,
    "C_Cpp.experimental.useQuickInfo": true,
    "C_Cpp.experimental.useRefactorings": true,
    "C_Cpp.experimental.useSemanticHighlighting": true,
    "C_Cpp.experimental.useSignatureHelp": true,
    "C_Cpp.experimental.useSymbolInformation": true,
    "C_Cpp.experimental.useTextDocumentSync": true,
    "C_Cpp.experimental.useUri": true,
    "C_Cpp.experimental.useWorkspace": true,
    "C_Cpp.experimental.useWorkspaceFolders": true,
    "C_Cpp.experimental.useWorkspaceState": true
}
```

### CMake Integration

The CMake configuration supports modern development features:

```json
// cmake-settings.json
{
    "cmake.configureOnOpen": false,
    "cmake.configureSettings": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON",
        "CMAKE_CXX_STANDARD": "20",
        "CMAKE_CXX_STANDARD_REQUIRED": true,
        "CMAKE_CXX_EXTENSIONS": false,
        "NLM_BUILD_PYTHON_BINDINGS": "OFF",
        "NLM_ENABLE_TESTS": "ON"
    },
    "cmake.configureArgs": [
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        "-DCMAKE_CXX_FLAGS=-Wall -Wextra -Wpedantic"
    ],
    "cmake.configureEnvironmentVariables": {
        "CC": "gcc",
        "CXX": "g++"
    },
    "cmake.debugConfig": "gdb",
    "cmake.cmakePaths": ["${workspaceFolder}"],
    "cmake.additionalSettings": [
        {
            "name": "NLM Build Type",
            "type": "choice",
            "default": "Release",
            "options": ["Debug", "Release", "RelWithDebInfo", "MinSizeRel"]
        },
        {
            "name": "Build Python Bindings",
            "type": "boolean",
            "default": false
        },
        {
            "name": "Enable Tests",
            "type": "boolean",
            "default": true
        }
    ]
}
```

## JetBrains CLion Configuration

### CLion Settings

Create `.idea/` directory with the following files:

#### Code Style Configuration (`codeStyleSettings.xml`)

```xml
<code_styles_settings version="173">
    <option name="COGNAC">
        <indentOptions>
            <option name="INDENT_SIZE" value="4" />
            <option name="CONTINUE_INDENT_SIZE" value="4" />
            <option name="CASE_INDENT_SIZE" value="4" />
        </indentOptions>
        <emptyLineBeforeAccessModifier value="1" />
        <emptyLineBeforeFunctionBody value="1" />
        <mySqlIndentOptions>
            <option name="CALL_PARAMETERS_WRAP_ALWAYS" value="3" />
            <option name="CALL_PARAMETERS_WRAP_IF_LONG" value="3" />
            <option name="ELABORATION_PARAMETERS_WRAP_ALWAYS" value="3" />
            <option name="ELABORATION_PARAMETERS_WRAP_IF_LONG" value="3" />
            <option name="METHOD_CHAIN_WRAP_ALWAYS" value="3" />
            <option name="METHOD_CHAIN_WRAP_IF_LONG" value="3" />
            <option name="METHOD_CALL_CHAIN_WRAP_ALWAYS" value="3" />
            <option name="METHOD_CALL_CHAIN_WRAP_IF_LONG" value="3" />
            <option name="ARRAY_INITIALIZER_WRAP_ALWAYS" value="3" />
            <option name="ARRAY_INITIALIZER_WRAP_IF_LONG" value="3" />
            <option name="FOR_STATEMENT_WRAP_ALWAYS" value="3" />
            <option name="FOR_STATEMENT_WRAP_IF_LONG" value="3" />
            <option name="WHILE_STATEMENT_WRAP_ALWAYS" value="3" />
            <option name="WHILE_STATEMENT_WRAP_IF_LONG" value="3" />
            <option name="TRY_CATCH_WRAP_ALWAYS" value="3" />
            <option name="TRY_CATCH_WRAP_IF_LONG" value="3" />
            <option name="IF_STATEMENT_WRAP_ALWAYS" value="3" />
            <option name="IF_STATEMENT_WRAP_IF_LONG" value="3" />
            <option name="ELSE_IF_BRANCH_WRAP_ALWAYS" value="3" />
            <option name="ELSE_IF_BRANCH_WRAP_IF_LONG" value="3" />
            <option name="ELSE_BRANCH_WRAP_ALWAYS" value="3" />
            <option name="ELSE_BRANCH_WRAP_IF_LONG" value="3" />
            <option name="TERNARY_OPERATOR_WRAP_ALWAYS" value="3" />
            <option name="TERNARY_OPERATOR_WRAP_IF_LONG" value="3" />
            <option name="CATCH_BINARY_OPERATOR_WRAP_ALWAYS" value="3" />
            <option name="CATCH_BINARY_OPERATOR_WRAP_IF_LONG" value="3" />
            <option name="BINARY_OPERATOR_WRAP_ALWAYS" value="3" />
            <option name="BINARY_OPERATOR_WRAP_IF_LONG" value="3" />
            <option name="UNARY_OPERATOR_WRAP_ALWAYS" value="3" />
            <option name="UNARY_OPERATOR_WRAP_IF_LONG" value="3" />
            <option name="RETURN_STATEMENT_WRAP_ALWAYS" value="3" />
            <option name="RETURN_STATEMENT_WRAP_IF_LONG" value="3" />
            <option name="THROW_EXPRESSION_WRAP_ALWAYS" value="3" />
            <option name="THROW_EXPRESSION_WRAP_IF_LONG" value="3" />
            <option name="ASSIGNMENT_OPERATOR_WRAP_ALWAYS" value="3" />
            <option name="ASSIGNMENT_OPERATOR_WRAP_IF_LONG" value="3" />
            <option name="ATTRIBUTE_LIST_WRAP_ALWAYS" value="3" />
            <option name="ATTRIBUTE_LIST_WRAP_IF_LONG" value="3" />
            <option name="PARENTHESIZED_EXPRESSION_WRAP_ALWAYS" value="3" />
            <option name="PARENTHESIZED_EXPRESSION_WRAP_IF_LONG" value="3" />
            <option name="BRACED_LIST_WRAP_ALWAYS" value="3" />
            <option name="BRACED_LIST_WRAP_IF_LONG" value="3" />
            <option name="TYPE_CONSTRAINT_WRAP_ALWAYS" value="3" />
            <option name="TYPE_CONSTRAINT_WRAP_IF_LONG" value="3" />
            <option name="INIT_CAPTURE_WRAP_ALWAYS" value="3" />
            <option name="INIT_CAPTURE_WRAP_IF_LONG" value="3" />
            <option name="SINGLE_STATEMENT_TEMPLATE_PARAMETERS_WRAP_ALWAYS" value="3" />
            <option name="SINGLE_STATEMENT_TEMPLATE_PARAMETERS_WRAP_IF_LONG" value="3" />
            <option name="CLASS_TEMPLATE_PARAMETERS_WRAP_ALWAYS" value="3" />
            <option name="CLASS_TEMPLATE_PARAMETERS_WRAP_IF_LONG" value="3" />
            <option name="FUNCTION_TEMPLATE_PARAMETERS_WRAP_ALWAYS" value="3" />
            <option name="FUNCTION_TEMPLATE_PARAMETERS_WRAP_IF_LONG" value="3" />
            <option name="TEMPLATE_TEMPLATE_PARAMETERS_WRAP_ALWAYS" value="3" />
            <option name="TEMPLATE_TEMPLATE_PARAMETERS_WRAP_IF_LONG" value="3" />
            <option name="TEMPLATE_TYPE_PARAMETERS_WRAP_ALWAYS" value="3" />
            <option name="TEMPLATE_TYPE_PARAMETERS_WRAP_IF_LONG" value="3" />
            <option name="TEMPLATE_DEFAULT_ARGUMENT_WRAP_ALWAYS" value="3" />
            <option name="TEMPLATE_DEFAULT_ARGUMENT_WRAP_IF_LONG" value="3" />
            <option name="TEMPLATE_TYPE_TEMPLATE_ARGUMENT_LIST_WRAP_ALWAYS" value="3" />
            <option name="TEMPLATE_TYPE_TEMPLATE_ARGUMENT_LIST_WRAP_IF_LONG" value="3" />
            <option name="TEMPLATE_TEMPLATE_TEMPLATE_ARGUMENT_LIST_WRAP_ALWAYS" value="3" />
            <option name="TEMPLATE_TEMPLATE_TEMPLATE_ARGUMENT_LIST_WRAP_IF_LONG" value="3" />
        </mySqlIndentOptions>
    </option>
</code_styles_settings>
```

#### Inspector Settings (`inspectors.xml`)

```xml
<inspect_profile version="173">
    <option name="show_global_issues" value="false" />
    <option name="show_imported_modules" value="true" />
    <option name="show_definitions" value="false" />
    <option name="show_declaration" value="false" />
    <option name="show_references" value="false" />
    <option name="show_type_resolution" value="false" />
    <option name="show_static_initializers" value="false" />
    <option name="show_member_overrides" value="true" />
    <option name="show_implicit_move_constructors" value="true" />
    <option name="show_implicit_copy_constructors" value="true" />
    <option name="show_rvo_constructors" value="true" />
    <option name="show_implicit_copy_assignments" value="true" />
    <option name="show_implicit_move_assignments" value="true" />
    <option name="show_destructor_deleted" value="true" />
    <option name="show_default_constructor_deleted" value="true" />
    <option name="show_copy_constructor_deleted" value="true" />
    <option name="show_move_constructor_deleted" value="true" />
    <option name="show_copy_assignment_deleted" value="true" />
    <option name="show_move_assignment_deleted" value="true" />
    <option name="show_noexcept_constructor" value="false" />
    "option name=\"show_noexcept_assignment\" value="false" />
    <option name="show_override_final" value="true" />
    <option name="show_explicit_copy" value="false" />
    <option name="show_explicit_move" value="false" />
    <option name="show_definitions_with_body" value="true" />
    <option name="show_member_names" value="true" />
    <option name="show_nested_names" value="false" />
    <option name="show_static_initializers" value="false" />
    <option name="show_destructor_overrides" value="false" />
    <option name="show_const_methods_last" value="false" />
    <option name="show_internal_access_only" value="false" />
    <option name="show_pure_virtuals" value="false" />
    <option name="show_static_only" value="false" />
    <option name="show_overloadable_once" value="false" />
    <option name="show_overloadable_macros" value="false" />
    <option name="show_using_namespace" value="false" />
    <option name="show_namespace_alias" value="false" />
    <option name="show_type_alias" value="false" />
    <option name="show_implicit_default_constructor" value="true" />
    <option name="show_referenced_methods" value="true" />
    <option name="show_implicit_construction" value="true" />
    <option name="show_global_definitions" value="true" />
</inspect_profile>
```

### CLion Project-Wide Settings

Create `.ide/settings.xml`:

```xml
<project version="4">
  <component name="CMakeWorkspace">
    <option name="trackFilesMode" value="Standard" />
    <option name="generateImportStatements" value="false" />
    <option name="skipConfiguratorsList" value="false" />
    <option name="autoUpdateSources" value="false" />
    <option name="manualSources" value="true" />
    <option name="registerCommonCompilerDefines" value="true" />
    <option name="perModuleCompilerSettings" value="true" />
    <option name="skipBuildAfterChange" value="false" />
    <option name="showToolchainOptions" value="false" />
    <option name="useClangCompleter" value="true" />
    <option name="useClangdForCodeCompletion" value="true" />
    <option name="useClangdForNavigation" value="true" />
    <option name="useClangdForSemanticHighlighting" value="true" />
    <option name="useClangdForDiagnostics" value="true" />
    <option name="useClangdForCodeStyle" value="true" />
    <option name="useClangdForRefactoring" value="true" />
    <option name="useClangdForQuickFixes" value="true" />
    <option name="useClangdForInlayHints" value="true" />
    <option name="useClangdForCompletion" value="true" />
    <option name="useClangdForCallHierarchy" value="true" />
    <option name="useClangdForTypedefs" value="true" />
    <option name="useClangdForMacroExpansion" value="true" />
    <option name="useClangdForProjectSummary" value="true" />
    <option name="useClangdForVirtualFileSystem" value="true" />
    <option name="useClangdForFilesystemWatchers" value="true" />
    <option name="useClangdForWorkspaceIndex" value="true" />
    <option name="useClangdForExternalIncludes" value="true" />
    <option name="useClangdForIdeFeatures" value="true" />
    <option name="useClangdForDocumentation" value="true" />
    <option name="useClangdForFormatting" value="true" />
    <option name="useClangdForCodeActions" value="true" />
    <option name="useClangdForRename" value="true" />
    <option name="useClangdForFindUsages" value="true" />
    <option name="useClangdForGoToDeclaration" value="true" />
    <option name="useClangdForHover" value="true" />
    <option name="useClangdForSignatureHelp" value="true" />
    <option name="useClangdForCompletionResolve" value="true" />
    <option name="useClangdForCodeActionResolve" value="true" />
    <option name="useClangdForExecuteCommand" value="true" />
    <option name="useClangdForFoldingRanges" value="true" />
    <option name="useClangdForSelectionRanges" value="true" />
    <option name="useClangdForSemanticTokens" value="true" />
    <option name="useClangdForInlayHints" value="true" />
    <option name="useClangdForMoniker" value="true" />
    <option name="useClangdForLinkDefinition" value="true" />
    <option name="useClangdForCallSignatureHelp" value="true" />
    <option name="useClangdForFormat" value="true" />
    <option name="useClangdForDiagnostic" value="true" />
    <option name="useClangdForProgress" value="true" />
    <option name="useClangdForConfiguration" value="true" />
    <option name="useClangdForSettings" value="true" />
    <option name="useClangdForWorkspaceFolders" value="true" />
    <option name="useClangdForDiagnostics" value="true" />
    <option name="useClangdForFormat" value="true" />
    <option name="useClangdForNavigation" value="true" />
    <option name="useClangdForCodeCompletion" value="true" />
    <option name="useClangdForSemanticHighlighting" value="true" />
    <option name="useClangdForDocumentHighlight" value="true" />
    <option name="useClangdForDocumentLink" value="true" />
    <option name="useClangdForDocumentRangeFormatting" value="true" />
    <option name="useClangdForOnTypeFormatting" value="true" />
    <option name="useClangdForCompletionResolve" value="true" />
    <option name="useClangdForCodeActionResolve" value="true" />
    <option name="useClangdForExecuteCommand" value="true" />
    <option name="useClangdForFoldingRanges" value="true" />
    <option name="useClangdForSelectionRanges" value="true" />
    <option name="useClangdForSemanticTokens" value="true" />
    <option name="useClangdForInlayHints" value="true" />
    <option name="useClangdForMoniker" value="true" />
    <option name="useClangdForLinkDefinition" value="true" />
    <option name="useClangdForCallSignatureHelp" value="true" />
    <option name="useClangdForFormat" value="true" />
    <option name="useClangdForDiagnostic" value="true" />
    <option name="useClangdForProgress" value="true" />
    <option name="useClangdForConfiguration" value="true" />
    <option name="useClangdForSettings" value="true" />
    <option name="useClangdForWorkspaceFolders" value="true" />
  </component>
</project>
```

## Common Development Tasks

### Quick Build and Run

```bash
# Configure and build in one command
./setup.sh build

# Quick test build
./setup.sh test

# Install in development mode
./setup.sh install

# Run the unified CLI
./nlm --command="brain status"
```

### Code Formatting

```bash
# Format all code with clang-format
./setup.sh format

# Format only modified files
./setup.sh format-modified
```

### Static Analysis

```bash
# Run clang-tidy
./setup.sh lint

# Run cppcheck
./setup.sh cppcheck

# Run all static analysis
./setup.sh analyze
```

### Testing

```bash
# Run unit tests
./setup.sh test

# Run performance benchmarks
./setup.sh benchmark

# Run integration tests
./setup.sh integration

# Run all tests
./setup.sh test-all
```

## Development Workflow

### 1. Initial Setup

```bash
# Clone the repository
https://github.com/nlm-research/nlm.git

# Navigate to project directory
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install dependencies (if needed)
./setup.sh install-deps
```

### 2. Regular Development

```bash
# Check project status
cd /workspace/app
./setup.sh status

# Build changes
./setup.sh build

# Run tests
./setup.sh test

# Format code
./setup.sh format

# Static analysis
./setup.sh lint

# Check performance
./setup.sh benchmark
```

### 3. Debugging

```bash
# Launch debugger
./setup.sh debug

# Attach to running process
gdb -p <pid>

# Generate core dump analysis
./setup.sh analyze-crash

# Check memory usage
./setup.sh memory-check
```

### 4. Documentation

```bash
# Generate documentation
./setup.sh docs

# Update API documentation
./setup.sh doxygen

# Generate examples
./setup.sh examples
```

## Support and Resources

### Getting Help

- **Documentation**: See `docs/` directory for comprehensive guides
- **Examples**: Review `examples/` directory for usage examples
- **API Reference**: Generated documentation in `docs/doxygen/`
- **Community**: GitHub repository and issue tracker

### Troubleshooting

#### Common Issues

1. **Build Failures**
   ```bash
   # Check for common issues
cmake --version
gcc --version
g++ --version
   ```

2. **IDE Integration**
   ```bash
   # Re-import CMake project
cmd + Shift + B
   # or Ctrl + Shift + B (Linux)
   ```

3. **Performance Issues**
   ```bash
   # Check system resources
   free -h
   df -h
   ```

4. **Memory Issues**
   ```bash
   # Monitor memory usage
   valgrind --tool=massif ./nlm --command="brain status"
   ```

### Development Tips

1. **Use CMake Cache**
   ```bash
   # Save compilation database
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
   ```

2. **Parallel Builds**
   ```bash
   # Optimize for your system
   make -j$(nproc)
   # or for Windows
   cmake --build . --config Release --parallel
   ```

3. **Code Review**
   ```bash
   # Generate diff for review
git diff HEAD~1
   ```

## Configuration Templates

### Sample Development Configuration

Create `config/dev.cfg`:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.0001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "enable_checkpointing": true,
    "enable_development": true,
    "enable_replay": true,
    "enable_prediction": true,
    "enable_neuromodulation": true,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "maturation_rate": 0.001,
    "initial_development_stage": 0,
    "dopamine_sensitivity": 1.0,
    "curiosity_strength": 0.5,
    "log_level": "debug",
    "performance_mode": "high"
}
```

### Sample Performance Configuration

Create `config/performance.cfg`:

```json
{
    "random_seed": 12345,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.1,
    "enable_checkpointing": true,
    "enable_development": false,
    "enable_replay": false,
    "enable_prediction": true,
    "enable_neuromodulation": true,
    "stdp_ltp_weight": 0.015,
    "stdp_ltd_weight": 0.01,
    "stdp_tau": 15.0,
    "synaptogenesis_rate": 0.00005,
    "pruning_rate": 0.000005,
    "maturation_rate": 0.0005,
    "initial_development_stage": 0,
    "dopamine_sensitivity": 0.8,
    "curiosity_strength": 0.3,
    "log_level": "info",
    "performance_mode": "fast"
}
```

## Advanced Development Features

### Custom Build Types

The project supports several custom CMake build types:

1. **Debug**: Maximum debugging with assertions
2. **Release**: Optimized for speed
3. **RelWithDebInfo**: Release with debugging symbols
4. **MinSizeRel**: Optimized for size
5. **NLM-Debug**: Custom debug with neuroscience-specific enhancements

### Performance Monitoring

The project includes built-in performance monitoring:

```bash
# Run with performance profiling
./nlm --command="benchmark"

# Monitor resource usage
gtop
htop

# Generate performance reports
./setup.sh generate-report
```

### Code Analysis Tools

The project supports comprehensive code analysis:

1. **Clang-Tidy**: Static analysis and code style
2. **CppCheck**: Memory and thread analysis
3. **Valgrind**: Memory leak detection
4. **AddressSanitizer**: Runtime memory error detection

## Migration Guide

### From Previous Versions

If migrating from previous versions of NLM:

1. **Command Line Interface**: The unified CLI replaces separate demo executables
2. **Configuration**: Configuration file format has been extended
3. **API**: New API methods are available in CommandLineInterface
4. **Documentation**: Comprehensive new documentation is available

### Breaking Changes

- The unified CLI is the new recommended interface
- Some configuration keys have been added for advanced features
- Backward compatibility is maintained through preserved demo executables

## Future Development

### Planned Improvements

1. **Enhanced IDE Support**: Additional IDE templates and integrations
2. **Development Workflows**: Automated testing and deployment pipelines
3. **Performance Tools**: Advanced profiling and optimization tools
4. **Documentation Systems**: Automated documentation generation
5. **Development Environments**: Container-based development setups
6. **Code Quality**: Continuous integration and quality assurance

### Contribution Guidelines

1. **Code Style**: Follow existing C++20 conventions
2. **Documentation**: Update documentation for new features
3. **Testing**: Add unit tests for new functionality
4. **Performance**: Ensure no performance regressions
5. **Compatibility**: Maintain backward compatibility

This document provides comprehensive development environment configuration for the NLM project, ensuring a productive and efficient development experience for all contributors.