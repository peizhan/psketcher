LINK_DIRECTORIES ("../mmcMatrix")
LINK_DIRECTORIES ("../NumOptimization")


# create the psketcher library
ADD_LIBRARY (Ark3d STATIC ConstraintSolver.cpp Sketch.cpp DOF.cpp IndependentDOF.cpp DependentDOF.cpp PrimitiveBase.cpp pSketcherModel.cpp Point.cpp Vector.cpp SketchPlane.cpp Primitive2DBase.cpp Point2D.cpp Edge2DBase.cpp Line.cpp Line2D.cpp ConstraintEquationBase.cpp SolverFunctions.cpp SolverFunctionsBase.cpp DistancePoint2D.cpp  ParallelLine2D.cpp HoriVertLine2D.cpp TangentEdge2D.cpp AngleLine2D.cpp Arc2D.cpp Circle2D.cpp EdgeLoop2D.cpp DistancePointLine2D.cpp)

# The following module is included so that the pkg_check_modules macro can be used below
find_package(PkgConfig)

#Boost
find_package( Boost 1.34 COMPONENTS filesystem )
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )

