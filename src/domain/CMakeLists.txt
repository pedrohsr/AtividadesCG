foreach(EXEC ${EXECS})
    target_sources(${EXEC}
                   PRIVATE
                   ${CMAKE_CURRENT_SOURCE_DIR}/Obj.hpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/Shader.hpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/Obj.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp
                   )
endforeach()