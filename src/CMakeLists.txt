if (TARGET tinyusb_device)
    add_executable(faceDriver main.cpp sources/Logger.cpp sources/Max7219Driver.cpp sources/Animation.cpp sources/Emotion.cpp sources/BoopCode.cpp sources/PolledBoopCode.cpp)
    # pico_generate_pio_header(faceDriver ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR})

    # pull in common dependencies
    target_link_libraries(faceDriver pico_stdlib hardware_spi)

    pico_enable_stdio_usb(faceDriver 1)
    pico_enable_stdio_uart(faceDriver 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(faceDriver)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building faceDriver because TinyUSB submodule is not initialized in the SDK")
endif()