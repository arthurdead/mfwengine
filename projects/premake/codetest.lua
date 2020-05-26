kind "ConsoleApp"

--depends "core"
linksanddepends "core"
links{"X11","Xrandr","dl","xcb"}

filter "configurations:Debug"
	links "iberty"
