cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(STM32F407_LVGL_Project C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# MCU architecture  //加 STM32F103C8T6 对应的 GCC 编译参数
set(MCU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

add_compile_options(
    ${MCU_FLAGS}

    -Wall
    -ffunction-sections
    -fdata-sections

    -finput-charset=UTF-8
    -fexec-charset=UTF-8
)

add_link_options(
    ${MCU_FLAGS}
    -Wl,--gc-sections
)

# Source files   
file(GLOB_RECURSE SOURCES
    Core/Src/*.c
    Drivers/STM32F4xx_HAL_Driver/Src/*.c
    Middlewares/LVGL/src/*.c
    Middlewares/ATK_MD0430/*.c
    Startup/*.s
)

# FreeRTOS Kernel
list(APPEND SOURCES
    ${CMAKE_SOURCE_DIR}/Middlewares/tasks.c
    ${CMAKE_SOURCE_DIR}/Middlewares/queue.c
    ${CMAKE_SOURCE_DIR}/Middlewares/list.c
    ${CMAKE_SOURCE_DIR}/Middlewares/timers.c
    ${CMAKE_SOURCE_DIR}/Middlewares/event_groups.c
    ${CMAKE_SOURCE_DIR}/Middlewares/stream_buffer.c

    # FreeRTOS GCC Cortex-M3 Port
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/portable/GCC/ARM_CM4F/port.c

    # FreeRTOS Heap
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/portable/MemMang/heap_4.c
)

# 不编译 HAL 官方提供的示例模板和旧版兼容源码
list(FILTER SOURCES EXCLUDE REGEX ".*/Legacy/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*_template\\.c$")

add_executable(${PROJECT_NAME}
    ${SOURCES}
)
target_link_libraries(${PROJECT_NAME} PRIVATE m)

set_target_properties(${PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    Core/Inc
    Drivers/STM32F4xx_HAL_Driver/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F4xx/Include

    # FreeRTOS
    Middlewares/FreeRTOS/include
    Middlewares/FreeRTOS/portable/GCC/ARM_CM4F
    # LVGL
    Middlewares/LVGL
    # ATK_MD0430
    Middlewares/ATK_MD0430
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32F407xx
    USE_HAL_DRIVER
)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/Linker/STM32F407XX_FLASH.ld
    --specs=nosys.specs
)

# 编译完成后显示程序 Flash / RAM 占用
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-size $<TARGET_FILE:${PROJECT_NAME}>
)

# 生成 HEX 文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex
            $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
)

# 生成 BIN 文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary
            $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
)