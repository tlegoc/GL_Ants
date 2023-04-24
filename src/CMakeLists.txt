add_executable(AntsGL 
main.cpp
config.h
Simulation.h
Simulation.cpp
)

target_link_libraries(AntsGL ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})