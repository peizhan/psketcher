cmake_minimum_required(VERSION 2.6)

PROJECT (psketcher)

set (PSKETCHER_VERSION 0.1)
ADD_DEFINITIONS (-DVERSION="\\"${PSKETCHER_VERSION}\\"")

add_subdirectory (src/Interface)
add_subdirectory (src/NumOptimization)
add_subdirectory (src/mmcMatrix)
add_subdirectory (src/ConstraintSolver)
add_subdirectory (src/QtBinding)
#add_subdirectory (src/PythonBinding)
add_subdirectory (src/InteractiveConstructors)
add_subdirectory (src/sqlite3)

