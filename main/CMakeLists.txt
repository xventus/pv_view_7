# Define the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the component sources
set(COMPONENT_SRCS
    "main.cpp"              # Main application entry point
       # Custom display driver implementation
    "application.cpp"	
    "rptask.cpp"
    "rptimer.cpp"
    "web_task.cpp"
    "wifi_task.cpp"
    "reset_task.cpp"
    "dspl_task.cpp"
    "mqtt_task.cpp"
    "time_task.cpp"
    "CH422G.cpp"
    "sngl_ch422.cpp"
    "setting_screen.cpp"
    "noway_screen.cpp"
    "main_screen.cpp"
    "ui/display_driver.cpp"
    "ui/screen_manager.cpp"
)

# Define the include directories
set(COMPONENT_ADD_INCLUDEDIRS ".")

# Use idf_component_register to register the component and its properties
idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS}
)

# Set compilation options for C++ files
target_compile_options(${COMPONENT_LIB} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++20>     # Use C++20 for C++ source files
)

# Set compilation options for C files if needed
target_compile_options(${COMPONENT_LIB} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-std=c99>         # Use C99 for C source files
)
