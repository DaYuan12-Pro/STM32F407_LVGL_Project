cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(STM32F407_LVGL_Project C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ============================================================
# STM32F407 Cortex-M4F
# ============================================================

# MCU architecture 
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

# ============================================================
# Source files
# ============================================================

# Source files   
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    Core/Src/*.c

    Drivers/SYSTEM/*.c

    Drivers/BSP/LED/*.c
    Drivers/BSP/KEY/*.c
    Drivers/BSP/SRAM/*.c
    Drivers/BSP/LCD/*.c
    Drivers/BSP/TOUCH/*.c
    Drivers/BSP/IIC/*.c
    Drivers/BSP/24CXX/*.c

    Drivers/STM32F4xx_HAL_Driver/Src/*.c

    Middlewares/LVGL/GUI/lvgl/src/*.c
    Middlewares/ATK_MD0430/*.c

    Startup/*.s
)
# lcd.c 已通过 #include 包含 lcd_ex.c，不能重复编译
list(FILTER SOURCES EXCLUDE REGEX "/lcd_ex\\.c$")
# ============================================================
# FreeRTOS
# ============================================================

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

# ============================================================
# Exclude unnecessary HAL files
# ============================================================

# 不编译 HAL 官方提供的示例模板和旧版兼容源码
list(FILTER SOURCES EXCLUDE REGEX ".*/Legacy/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*_template\\.c$")

# ============================================================
# LVGL Port
# ============================================================

list(APPEND SOURCES
    ${CMAKE_SOURCE_DIR}/Middlewares/LVGL/GUI/lvgl/examples/porting/lv_port_disp_template.c
    ${CMAKE_SOURCE_DIR}/Middlewares/LVGL/GUI/lvgl/examples/porting/lv_port_indev_template.c
)


# ============================================================
# LVGL Stress Demo
# ============================================================

file(GLOB_RECURSE LVGL_STRESS_SOURCES
    ${CMAKE_SOURCE_DIR}/Middlewares/LVGL/GUI_APP/demos/stress/*.c
)

list(APPEND SOURCES
    ${LVGL_STRESS_SOURCES}
)

# ============================================================
# ELF
# ============================================================

# 添加内存管理源文件
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/MALLOC/malloc.c
)

# 创建程序
add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE m)

set_target_properties(${PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
)


# ============================================================
# Include directories
# ============================================================

target_include_directories(${PROJECT_NAME} PRIVATE
    Core/Inc
    Drivers/STM32F4xx_HAL_Driver/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F4xx/Include

    # 支持原例程中的 "./BSP/..." 和 "./SYSTEM/..." 写法
    Drivers

    # 支持直接写 #include "sys.h" 等
    Drivers/SYSTEM/sys
    Drivers/SYSTEM/delay
    Drivers/SYSTEM/usart

    Drivers/BSP/LED
    Drivers/BSP/KEY
    Drivers/BSP/SRAM
    Drivers/BSP/LCD
    Drivers/BSP/TOUCH
    Drivers/BSP/IIC
    Drivers/BSP/24CXX

    # FreeRTOS
    Middlewares/FreeRTOS/include
    Middlewares/FreeRTOS/portable/GCC/ARM_CM4F

    # LVGL
    Middlewares
    Middlewares/LVGL/GUI/lvgl
    Middlewares/LVGL/GUI/lvgl/src
    Middlewares/LVGL/GUI/lvgl/examples/porting
    Middlewares/LVGL/GUI_APP/demos
    Middlewares/LVGL/GUI_APP/demos/music
    Middlewares/LVGL/GUI_APP/demos/stress
    Middlewares/LVGL/GUI_APP/demos/widgets
    Middlewares/LVGL/GUI_APP/demos/benchmark
    Middlewares/LVGL/GUI_APP/demos/keypad_encoder

    # ATK_MD0430
    Middlewares/ATK_MD0430

    # SRAM
    Middlewares/MALLOC
)


# ============================================================
# STM32 Define
# ============================================================

target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32F407xx
    USE_HAL_DRIVER
)


# ============================================================
# Linker
# ============================================================

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/Linker/STM32F407XX_FLASH.ld
    --specs=nosys.specs
)


# ============================================================
# Flash / RAM usage
# ============================================================

# 编译完成后显示程序 Flash / RAM 占用
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-size $<TARGET_FILE:${PROJECT_NAME}>
)


# ============================================================
# HEX
# ============================================================

# 生成 HEX 文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex
            $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
)


# ============================================================
# BIN
# ============================================================


# 生成 BIN 文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary
            $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
)