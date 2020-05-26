kind "SharedLib"

defines "MFW_APPLICATION_MAIN_DEFINED"

removefiles(path.join(sourcefolder, "private/mfw/renderer/vulkan/**"))

filter "system:not Windows"
	links{"X11","Xrandr"}
