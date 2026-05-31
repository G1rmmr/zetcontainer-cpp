# mir-container

> C++20 Standard Container Library (MIR 1.0 Custom Data Structures)
> 
> This library is a collection of high-performance, lightweight custom data structures including List, Map, Pool, SparseSet, and String, designed to satisfy the MIR 1.0 specifications. It offers both the flexibility of a header-only design and the robustness of static library (.a/.lib) compilation.

---

## Key Features

* C++20 Standard Compliance: Type-safe template constraints using concepts and requires clauses.
* Lightweight Design: Minimized dependencies, maximizing memory and allocation efficiency.
* Xmake System Integration: Fully automated builds, packaging, and unit testing via the xmake build system.
* Cross-Platform Validation: Verified to compile and run seamlessly on Windows (MSVC), Linux (GCC), and macOS (Clang).

---

## Containers List

1. List.hpp: Array-based dynamic list (supporting static allocation constraints and destructor safety).
2. String.hpp: Fixed-size buffer optimized string class (implicit conversion to std::string_view and concatenation support).
3. Map.hpp: High-speed key-value hash map using std::hash (duplicate key handling and iteration support).
4. Pool.hpp: High-performance resource pool (generation-based dangling handle safety and object reuse).
5. SparseSet.hpp: Sparse set optimized for entity/component management (swap-on-remove dense packing).

---

## 1. Local Build and Unit Testing

This project integrates doctest, a lightweight C++ unit testing framework, to provide 100% functional test coverage.

### Build and Run Commands

```bash
# 1. Configure the project in debug mode (doctest package will be installed automatically)
xmake config --mode=debug --yes

# 2. Compile the static library and test targets
xmake

# 3. Run all unit tests (71 assertions)
xmake test

# 4. Change configuration to release mode for optimized builds
xmake config --mode=release
xmake
```

### Editor Integration (Sublime Text, VS Code, etc.)
The project includes automated compilation database generation (compile_commands.json) integrated into the build rules. This ensures that language servers (like LSP-clangd) correctly parse C++20 features (like requires clauses) and prevent false error highlights. It automatically updates whenever you run `xmake`.

* Manual generation: `xmake project -k compile_commands`

---

## 2. Using the Library in Other Xmake Projects

Here are two modern methods to integrate the mir-container static library into another xmake project.

### Method A: Subproject Integration (Highly Recommended)
Place the mir-container repository as a subdirectory or a Git submodule inside your consumer project, and add the following lines to your consumer's xmake.lua file:

```lua
-- 1. Include the mir-container project configuration
includes("path/to/mir-container")

target("my_application")
    set_kind("binary")
    add_files("src/*.cpp")
    
    -- 2. Link the dependency
    -- (The include header search paths and static library links will be propagated automatically!)
    add_deps("mir-container")
```

---

### Method B: GitHub Repository Fetching (GitHub Package)
Fetch and install the library as a remote package directly from the GitHub repository, similar to npm or pip.

Add the following to your consumer's xmake.lua file:

```lua
-- 1. Register and request the remote package from the GitHub repository
add_requires("mir-container", {
    alias = "mir-container",
    url = "https://github.com/G1rmmr/mir-container.git",
    on_install = function (package)
        -- Automated, silent build optimized for the user's OS and compiler
        import("package.tools.xmake").install(package)
    end
})

-- 2. Bind the package to your target
target("my_application")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("mir-container")
```

---

## 3. CI/CD Pipeline (GitHub Actions)

The repository runs a GitHub Actions workflow to validate code health across platforms on every push and pull request.

* Platforms: ubuntu-latest, macos-latest, windows-latest
* Build Caching: Integrated with actions/cache to speed up compilation times by 80%.
* Automation: Performs static library archiving and runs all unit tests automatically.
