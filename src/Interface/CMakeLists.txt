# The fllowing forces cmake to show the compiler command line arguements, usefull for debugging
SET (CMAKE_VERBOSE_MAKEFILE TRUE)

# Add local link paths
LINK_DIRECTORIES ("../mmcMatrix")
LINK_DIRECTORIES ("../NumOptimization")
LINK_DIRECTORIES ("../ConstraintSolver")
LINK_DIRECTORIES ("../QtBinding")
# LINK_DIRECTORIES ("../PythonBinding")
LINK_DIRECTORIES ("../InteractiveConstructors")

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
set(psketcher_SRCS
    main.cpp
    pSketcherWindow.cpp
    pSketcherWidget.cpp
    Point2DEditDialog.cpp
)
set(psketcher_MOC_SRCS
    ./pSketcherWindow.h
    ./pSketcherWidget.h
	./Point2DEditDialog.h
)
set(psketcher_RCCS
    pSketcher.qrc
)
qt4_wrap_cpp(psketcher_MOCS ${psketcher_MOC_SRCS})
qt4_add_resources(psketcher_RCC_SRCS ${psketcher_RCCS})
add_definitions(-DQT_NO_DEBUG)

# LINK_LIBRARIES(boost_python)

#OpenGL
FIND_PACKAGE (OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

#Glut
#FIND_PACKAGE (GLUT REQUIRED)

# The following module is included so that the pkg_check_modules macro can be used below
find_package(PkgConfig)

#Boost
find_package( Boost 1.43 COMPONENTS filesystem system)
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )

#Python
# FIND_PACKAGE(PythonLibs)
# INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

add_executable(psketcher
    ${psketcher_SRCS}
    ${psketcher_MOCS}
    ${psketcher_RCC_SRCS})

TARGET_LINK_LIBRARIES (psketcher ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES (psketcher ${CMAKE_DL_LIBS})
#TARGET_LINK_LIBRARIES(psketcher ${OPENGL_LIBRARIES})
#TARGET_LINK_LIBRARIES(psketcher ${GLUT_Xmu_LIBRARY})
target_link_libraries(psketcher ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(psketcher dime)
#TARGET_LINK_LIBRARIES (psketcher psketcher_module)
TARGET_LINK_LIBRARIES (psketcher Ark3dQtBinding)
TARGET_LINK_LIBRARIES (psketcher Ark3d)
TARGET_LINK_LIBRARIES (psketcher mmcMatrix)
TARGET_LINK_LIBRARIES (psketcher InteractiveConstructors)
TARGET_LINK_LIBRARIES (psketcher bfgs)
TARGET_LINK_LIBRARIES (psketcher sqlite3)
#TARGET_LINK_LIBRARIES (psketcher ${PYTHON_LIBRARIES})

