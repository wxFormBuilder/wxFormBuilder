if(WIN32)
  if(MINGW)
    find_package(BFD REQUIRED)
    find_package(Iberty REQUIRED)
    # Non-MSYS2-MinGW builds seem not to require Intl but only ZLIB
    if(MSYS)
      find_package(Intl REQUIRED)
    else()
      find_package(Intl QUIET)
    endif()
    find_package(ZLIB REQUIRED)
  endif()
endif()

add_library(stack_trace_stack OBJECT)
add_library(stack_trace::stack ALIAS stack_trace_stack)

target_sources(stack_trace_stack
  PRIVATE
    stack_trace/stack.cpp
    stack_trace/stack.hpp
)

target_include_directories(stack_trace_stack
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

if(WIN32)
  if(MINGW)
    target_link_libraries(stack_trace_stack
      PRIVATE
        BFD::BFD
        Iberty::Iberty
        $<TARGET_NAME_IF_EXISTS:Intl::Intl>
        ZLIB::ZLIB
        psapi # TODO: Only required for systems older than Windows 7
    )
  endif()
  target_link_libraries(stack_trace_stack
    PRIVATE
      imagehlp
  )
else()
  target_link_libraries(stack_trace_stack
    PRIVATE
      dl
  )
endif()
