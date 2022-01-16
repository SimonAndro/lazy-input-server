add_library(
    server 
    src/server.c
)

target_include_directories (server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# target_link_libraries(server network simulation)
