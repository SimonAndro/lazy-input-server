add_library(
    network 
    src/network.c
)

target_include_directories (network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(network ws2_32)