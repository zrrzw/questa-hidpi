include(FindTCL)

add_library(questa-hidpi-patch SHARED questa-hidpi-patch.c)
target_link_libraries(questa-hidpi-patch PRIVATE frida-gum)
target_include_directories(questa-hidpi-patch PRIVATE ${TCL_INCLUDE_PATH} ${TK_INCLUDE_PATH})