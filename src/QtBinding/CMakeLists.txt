find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
set(qtbinding_SRCS
QtPrimitiveBase.cpp QtPoint.cpp QtPoint2D.cpp QtLine.cpp QtLine2D.cpp QtArc2D.cpp QtCircle2D.cpp QtParallelLine2D.cpp QtHoriVertLine2D.cpp QtDistancePoint2D.cpp QtDistancePointLine2D.cpp QtAngleLine2D.cpp QtTangentEdge2D.cpp QtSketch.cpp DisplayProperties.cpp
)
set(qtbinding_MOC_SRCS
QtPrimitiveBase.h QtPoint.h QtPoint2D.h QtLine.h QtLine2D.h QtArc2D.h QtCircle2D.h QtParallelLine2D.h QtHoriVertLine2D.h QtDistancePoint2D.h QtDistancePointLine2D.h QtAngleLine2D.h QtTangentEdge2D.h QtSketch.h
)

qt4_wrap_cpp(qtbinding_MOCS ${qtbinding_MOC_SRCS})
add_definitions(-DQT_NO_DEBUG)

add_library(Ark3dQtBinding STATIC
    ${qtbinding_SRCS}
    ${qtbinding_MOCS})
