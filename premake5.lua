workspace("IPchan")
    configurations({"Debug", "Release"})

project("IPchan")
    kind("ConsoleApp")
    language("C++")
    cppdialect("C++11")
    targetdir("bin/%{cfg.buildcfg}")

    --buildoptions({""})

    --libdirs({"/usr/local/lib"})
    links({"dl", "pthread", "crypto"})

    files({
        "**.cpp",
        "**.c",
        "**.h"
    })


    filter "configurations:Debug"
        defines({"DEBUG"})
        symbols("On")

    filter "configurations:Release"
        defines({"NDEBUG"})
        optimize("On")
