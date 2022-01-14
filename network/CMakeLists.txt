add_library(webServer 
        src/webServer.c
    )

target_include_directories (webServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(webServer ws2_32)