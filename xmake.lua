add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

-- Add doctest package
add_requires("doctest")

-- Static Library Target
target("mir-container")
    set_kind("static")
    set_languages("c++20")
    -- Export current directory so dependency targets can automatically include our headers
    add_includedirs(".", {public = true})
    add_headerfiles("*.hpp")
    add_files("src/*.cpp")

-- Unit Test Target
target("test_suite")
    set_kind("binary")
    set_languages("c++20")
    add_packages("doctest")
    add_files("tests/*.cpp")
    add_deps("mir-container")
    
    -- Register this binary as a test suite so xmake test can execute it
    add_tests("default")
