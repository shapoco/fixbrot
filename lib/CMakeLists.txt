# file(GLOB CPP_FILES
#     src/*.cpp
# )
add_library(libfixbrot STATIC
#  ${CPP_FILES}
)
target_include_directories(libfixbrot PUBLIC
  include
)
