
add_executable(${ProjectName}
    main.cpp
    WS2812.cpp
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    hardware_adc
    hardware_pio
    FreeRTOS-Kernel-Heap4 
    )

pico_enable_stdio_usb(${ProjectName} 1)
pico_enable_stdio_uart(${ProjectName} 0)


pico_add_extra_outputs(${ProjectName})
pico_generate_pio_header(${ProjectName} ${CMAKE_CURRENT_LIST_DIR}/WS2812.pio)
