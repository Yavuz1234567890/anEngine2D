workspace "anEngine2D"
	location ""
	architecture "x86_64"
	startproject "anEngine2DEditor"

	configurations
	{
		"Debug",
		"Release"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"thirdparty/glfw/include",
		"thirdparty/glew/include",
		"thirdparty/freetype/include",
		"thirdparty/fmod/include",
		"thirdparty/stb",
		"thirdparty/glm",
		"thirdparty/imgui",
		"thirdparty/entt/include",
		"thirdparty/tinyxml2",
		"thirdparty/tinyfiledialogs",
		"thirdparty/sol2/include",
		"thirdparty/lua",
		"thirdparty/ImGuiColorTextEdit",
		"src/anEngine2D"
	}

	filter "system:windows"
		defines
		{
			"PLATFORM_WINDOWS"
		}

	project "ImGui"
		location "thirdparty/imgui"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir "bin/%{cfg.buildcfg}"
		objdir "obj/%{cfg.buildcfg}"

		files
		{
			"thirdparty/imgui/**.cpp",
			"thirdparty/imgui/**.h"
		}

		filter "system:windows"
			systemversion "latest"

		filter "configurations:Debug"
			defines "C_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "C_RELEASE"
			runtime "Release"
			optimize "on"

	project "Lua"
		location "thirdparty/lua"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir "bin/%{cfg.buildcfg}"
		objdir "obj/%{cfg.buildcfg}"

		files
		{
			"thirdparty/sol2/include/**.hpp",
			"thirdparty/sol2/include/**.h",

			"thirdparty/lua/**.c",
			"thirdparty/lua/**.h"
		}

		filter "system:windows"
			systemversion "latest"

		filter "configurations:Debug"
			defines "C_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "C_RELEASE"
			runtime "Release"
			optimize "on"

	project "anEngine2D"
		location "src/anEngine2D"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir "bin/%{cfg.buildcfg}"
		objdir "obj/%{cfg.buildcfg}"

		files
		{
			"src/anEngine2D/**.cpp",
			"src/anEngine2D/**.h",
			
			"thirdparty/stb/**.cpp",
			"thirdparty/stb/**.h",

			"thirdparty/glm/**.cpp",
			"thirdparty/glm/**.hpp",
			"thirdparty/glm/**.h",

			"thirdparty/tinyxml2/tinyxml2.h",
			"thirdparty/tinyxml2/tinyxml2.cpp",

			"thirdparty/tinyfiledialogs/tinyfiledialogs.h",
			"thirdparty/tinyfiledialogs/tinyfiledialogs.c",

			"thirdparty/ImGuiColorTextEdit/ImGuiColorTextEdit/TextEditor.cpp",
			"thirdparty/ImGuiColorTextEdit/ImGuiColorTextEdit/TextEditor.h"
		}

		links
		{
			"ImGui",
			"Lua"
		}

		filter "system:windows"
			systemversion "latest"

			libdirs
			{
				"thirdparty/glew/lib-x64",
				"thirdparty/glfw/lib-x64",
				"thirdparty/freetype/lib-x64",
				"thirdparty/fmod/lib-x64"
			}

			links
			{
				"opengl32.lib",
				"glew32.lib",
				"glfw3.lib",
				"freetype.lib",
				"fmod_vc.lib"
			}

		filter "configurations:Debug"
			defines "C_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "C_RELEASE"
			runtime "Release"
			optimize "on"

	project "Application"
		location "src/Application"
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir "bin/%{cfg.buildcfg}"
		objdir "obj/%{cfg.buildcfg}"

		files
		{
			"src/Application/**.cpp",
			"src/Application/**.h"
		}

		links
		{
			"anEngine2D"
		}

		filter "system:windows"
			systemversion "latest"

		filter "configurations:Debug"
			defines "C_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "C_RELEASE"
			runtime "Release"
			optimize "on"

	project "anEngine2DEditor"
		location "src/anEngine2DEditor"
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir "bin/%{cfg.buildcfg}"
		objdir "obj/%{cfg.buildcfg}"

		files
		{
			"src/anEngine2DEditor/**.cpp",
			"src/anEngine2DEditor/**.h"
		}

		links
		{
			"anEngine2D"
		}

		filter "system:windows"
			systemversion "latest"

		filter "configurations:Debug"
			defines "C_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "C_RELEASE"
			runtime "Release"
			optimize "on"

	project "anEngine2DApplication"
		location "src/anEngine2DApplication"
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir "bin/%{cfg.buildcfg}"
		objdir "obj/%{cfg.buildcfg}"

		files
		{
			"src/anEngine2DApplication/**.cpp",
			"src/anEngine2DApplication/**.h"
		}

		links
		{
			"anEngine2D"
		}

		filter "system:windows"
			systemversion "latest"

		filter "configurations:Debug"
			defines "C_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "C_RELEASE"
			runtime "Release"
			optimize "on"
