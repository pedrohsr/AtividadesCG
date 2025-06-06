set(EXECS
    Hello3D
    CubeViewer
    TriangleTex
    SpherePhong
)

set(DOMAIN_EXECS
    TexturedViewer
    PhongViewer
    ModelLoader
)

foreach(EXEC ${EXECS})
    add_executable(${EXEC} ${EXEC}.cpp ${GLAD_C_FILE})
    target_include_directories(${EXEC} PRIVATE ${CMAKE_SOURCE_DIR}/include/glad ${glm_SOURCE_DIR} ${stb_image_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${EXEC} glfw ${OPENGL_LIBS})
endforeach()

foreach(EXEC ${DOMAIN_EXECS})
    add_executable(${EXEC} ${EXEC}.cpp ${GLAD_C_FILE})
    target_include_directories(${EXEC} PRIVATE ${CMAKE_SOURCE_DIR}/include/glad ${glm_SOURCE_DIR} ${stb_image_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${EXEC} glfw ${OPENGL_LIBS} domain)
endforeach()

add_subdirectory(domain)