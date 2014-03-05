# The following module is included so that the pkg_check_modules macro can be used below
find_package(PkgConfig)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# create the psketcher library
ADD_LIBRARY (InteractiveConstructors STATIC QtEventProperties.cpp  InteractiveConstructorBase.cpp  Line2DConstructor.cpp  Point2DConstructor.cpp Distance2DConstructor.cpp AngleLine2DConstructor.cpp ParallelLine2DConstructor.cpp HoriVertLine2DConstructor.cpp Arc2DConstructor.cpp Circle2DConstructor.cpp TangentEdge2DConstructor.cpp)


