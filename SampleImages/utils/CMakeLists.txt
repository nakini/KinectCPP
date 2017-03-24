# Add new variable for src utils lib
SET(UTLIS_LIB_SRC
    PlyFunctions.cpp
    Transformation.cpp 
)

# Create our new library
add_library(Utils ${UTLIS_LIB_SRC})
# Make sure the compiler can find include files for our library
target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
