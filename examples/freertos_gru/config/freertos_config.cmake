cmake_minimum_required(VERSION 3.15)

add_library(freertos_gru_config STATIC)

target_sources(freertos_gru_config PUBLIC   
        ${CMAKE_CURRENT_LIST_DIR}/IdleMemory.c
    )
target_include_directories(freertos_gru_config PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	) # The config file directory\n"
target_compile_definitions(freertos_gru_config
	PUBLIC  projCOVERAGE_TEST=0
	)
target_link_libraries(freertos_gru_config
	FreeRTOS-Kernel
	pico_stdlib
	)
