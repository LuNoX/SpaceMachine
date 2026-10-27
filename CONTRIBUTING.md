# Contributing

SpaceMachine welcomes contributions!

## Requirements

- [CMake](https://cmake.org/) 3.25 or newer
- [Ninja](https://ninja-build.org/), or another [Makefile](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#makefile-generators) or [Ninja](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#ninja-generators) generator that writes `compile_commands.json`, set in your user presets (see [Setup](#setup))
- [clang-cl](https://clang.llvm.org/docs/UsersManual.html#clang-cl), or another compiler, set in your user presets (see [Setup](#setup))
- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) and [clangd](https://clangd.llvm.org/), both part of [LLVM](https://llvm.org/)
- [cppcheck](https://cppcheck.sourceforge.io/)
- [uv](https://docs.astral.sh/uv/)

## Setup

1. Create your CMake user presets from the template:

   ```sh
   cp CMakeUserPresets.template.json CMakeUserPresets.json
   ```

   `CMakeUserPresets.json` is ignored by git and holds the buildable `debug`, `relwithdebinfo` and `release` presets.

2. Put any machine specific settings and [CMake options](#cmake-options) into its `user-overrides` preset. For example, to generate with `Unix Makefiles` instead of the default `Ninja`, build with `clang++` instead of the default `clang-cl` and use a `clang-tidy` that's not on `PATH`:

   ```json
   {
       "name": "user-overrides",
       "hidden": true,
       "generator": "Unix Makefiles",
       "cacheVariables": {
           "CMAKE_CXX_COMPILER": "clang++",
           "SPACEMACHINE_CLANG_TIDY_EXE": "path/to/my/clang-tidy"
       }
   }
   ```

   Note that when changing the generator of an existing build directory, configure once with `--fresh`.

3. Configure and build:

   ```sh
   cmake --preset debug
   cmake --build --preset debug
   ```

You should now have `.venv` and `build` directories in your tree and are ready to start developing!

## Presets

All presets default to Ninja and clang-cl, and build into `build/<preset>`.

| Preset | Build type | `SPACEMACHINE_DEV_MODE` | `SPACEMACHINE_REQUIRE_DEV_TOOLS` |
| --- | --- | --- | --- |
| `debug` | Debug | `ON` | `ON` |
| `relwithdebinfo` | RelWithDebInfo | `ON` | `ON` |
| `release` | Release | `OFF` | `OFF` |

## CMake options

Set these in your `user-overrides` preset's `cacheVariables`, or by passing `-D <Option>=<Value>` to cmake when configuring on the command line.

| Option | Default | Effect |
| --- | --- | --- |
| `SPACEMACHINE_DEV_MODE` | `OFF` | Adds the [demo](dev/demo/main.cpp) and the [checks](#checks). Only takes effect when SpaceMachine is the top-level project. |
| `SPACEMACHINE_REQUIRE_DEV_TOOLS` | `OFF` | Fails configuration when clang-tidy, clangd, cppcheck or uv is missing. When `OFF`, checks that need a missing tool are skipped with a message instead. |
| `SPACEMACHINE_CLANG_TIDY_EXE` | found on `PATH` | Path to the clang-tidy executable. |
| `SPACEMACHINE_CLANGD_EXE` | found on `PATH` | Path to the clangd executable. |
| `SPACEMACHINE_CPPCHECK_EXE` | found on `PATH` | Path to the cppcheck executable. |
| `SPACEMACHINE_UV_EXE` | found on `PATH` | Path to the uv executable. |

## Checks

Compiler warnings, clang-tidy findings and cppcheck findings are errors.

Additionally, building the dev tree runs these checks:

| Target | Checks that |
| --- | --- |
| `SpaceMachineCheckIndividualHeaderCompilation` | every header compiles on its own |
| `SpaceMachineAssertOneDefinitionRule` | every header can be included in two translation units of one program |
| `SpaceMachineCheckIncludes` | every header includes exactly what it uses, using clangd through [tools/lint_headers.py](tools/lint_headers.py) |
| `SpaceMachineDemo` | the [demo](dev/demo/main.cpp) builds cleanly under clang-tidy and cppcheck |
| `SpaceMachineCheckPolyfillHeaderCompilation` | every header compiles with all polyfill fallbacks forced |
| `SpaceMachineCheckPolyfillDemo` | the demo builds with all polyfill fallbacks forced |

## Code style

C++ is formatted with [clang-format](https://clang.llvm.org/docs/ClangFormat.html) using [.clang-format](.clang-format), and CMake with [cmake-format](https://github.com/cheshirekow/cmake_format) using [.cmake-format.yaml](.cmake-format.yaml). The build doesn't check formatting, so run `clang-format -i` and `cmake-format -i` on the files you changed before committing, either manually or by configuring your IDE to do it automatically.

Lint rules for clang-tidy are in [.clang-tidy](.clang-tidy). [.clangd](.clangd) sets clangd's include diagnostics to match clang-tidy's. The cppcheck options are in [cmake/SpaceMachineAnalyzers.cmake](cmake/SpaceMachineAnalyzers.cmake). All three run as part of the [checks](#checks).

In [VS Code](https://code.visualstudio.com/), install the recommended extensions from [.vscode/extensions.json](.vscode/extensions.json). The workspace [settings](.vscode/settings.json) turn off Microsoft's C++ IntelliSense in favor of clangd, and format C++ through clangd and CMake through cmake-format. clangd finds the compile flags through `build/compile_commands.json`, which the dev build links there.
