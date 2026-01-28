cmake_minimum_required(VERSION 3.15)
include(pico_sdk_import.cmake)

# Definir el nombre del proyecto
project(GasDetection
	LANGUAGES CXX C ASM
)

# Inicializar el SDK de Pico
pico_sdk_init()

# Añadir el ejecutable para la pantalla SSD1306
add_executable(ssd1306
    ssd1306.c
    font.h
)


# Enlazar las bibliotecas necesarias
target_link_libraries(ssd1306
    pico_stdlib    # Librería estándar de Pico
    hardware_i2c   # Para trabajar con I2C
)

target_include_directories(ssd1306 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

# Habilitar/Deshabilitar la salida por USB y UART
pico_enable_stdio_usb(ssd1306 1)
pico_enable_stdio_uart(ssd1306 0)

# Crear archivos adicionales (binario, map, hex, uf2)
pico_add_extra_outputs(ssd1306)