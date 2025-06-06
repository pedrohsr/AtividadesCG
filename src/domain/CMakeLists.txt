set(VIEWERS
    TexturedViewer
    modelLoader
    )

foreach(EXEC ${VIEWERS})
    target_sources(${EXEC}
                   PRIVATE
                   ${CMAKE_CURRENT_SOURCE_DIR}/Obj.hpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/Shader.hpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/TexturedObj.hpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/Obj.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/TexturedObj.cpp
                   )
endforeach()