if (TARGET tinyusb_device)
    add_executable(faceDriver main.cpp sources/ProtoComms.cpp sources/Logger.cpp sources/Max7219Driver.cpp sources/BoopSensor.cpp sources/Animation.cpp sources/Emotion.cpp sources/FanController.cpp sources/TinyLED.cpp sources/Gamma.cpp sources/SK6812/Pixel.cpp sources/SK6812/PixelString.cpp sources/SK6812/PixelMatrix.cpp sources/CheekFinAnimator.cpp sources/APDS9960.cpp Animations/BlinkAnim.cpp Animations/GlitchAnim.cpp)
    pico_generate_pio_header(faceDriver ${CMAKE_CURRENT_LIST_DIR}/sk6812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR})

    # pull in common dependencies
    target_link_libraries(faceDriver pico_stdlib hardware_spi hardware_pwm hardware_pio hardware_i2c hardware_dma hardware_uart hardware_gpio pico_time pico_sync pico_multicore)

    pico_enable_stdio_usb(faceDriver 1)
    pico_enable_stdio_uart(faceDriver 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(faceDriver)

    add_compile_options(-Wall -Werror -Wextra -Wpedantic -Wno-format -Wno-unused-function -Wno-maybe-uninitialized)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building faceDriver because TinyUSB submodule is not initialized in the SDK")
endif()