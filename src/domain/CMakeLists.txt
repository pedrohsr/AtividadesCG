add_library(domain
    Obj.hpp
    Obj.cpp
    Shader.hpp
    Shader.cpp
    TexturedObj.hpp
    TexturedObj.cpp
)

target_include_directories(domain PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/glad 
    ${glm_SOURCE_DIR} 
    ${stb_image_SOURCE_DIR}
)

target_link_libraries(domain PUBLIC glfw ${OPENGL_LIBS})