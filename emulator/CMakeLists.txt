set(SOURCES
	"Config.cpp"
	"ControlUnit.cpp"
	"CPU.cpp"
	"Cx16.cpp"
	"Display.cpp"
	"Executor.cpp"
	"Gfx.cpp"
	"HDD.cpp"
	"main.cpp"
	"PIT.cpp"
	"PMI.cpp"
)

find_package(SDL)
find_package(SDL_image)

add_executable(cx16-emulator ${SOURCES})
target_link_libraries(cx16-emulator pthread SDL SDLmain SDL_image SDL_gfx)
