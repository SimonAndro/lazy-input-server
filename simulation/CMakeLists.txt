add_library(
    simulation 
    src/simulation.c
)

target_include_directories (simulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# target_link_libraries(simulation ws2_32)