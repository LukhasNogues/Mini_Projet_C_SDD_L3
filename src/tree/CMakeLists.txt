cmake_minimum_required(VERSION 4.0)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)

project(List C)
# add_executable(tree tree.c tree.h)
add_library(tree SHARED tree.c tree.h)

install(
	TARGETS tree
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
)

install(
	FILES tree.h
	DESTINATION include
)

# Ajout d'un fichier de configuration de type pkgconfig. Copie le 1er argument vers le 2ème. @ONLY = restreint le remplacement de variable dans tree.pc.in
# à celles qui ont le format @<var>@ pour éviter les conflits avec la syntaxe CMake ${<var>}.
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/tree.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/tree.pc
	@ONLY
)
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/tree.pc
	DESTINATION share/pkgconfig
	COMPONENT "PkgConfig"
)

#  Ajout d'un fichier de configuration de type cmake
include(CMakePackageConfigHelpers)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/TreeConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/TreeConfig.cmake
	INSTALL_DESTINATION cmake
)
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/TreeConfig.cmake
	DESTINATION cmake
)

# Ajout d'un exécutable dépendant également de tree.h
add_executable(test-tree test-tree.c tree.h)
# Précision de l'ordre de construction: le programme de test doit se faire
# après la librairie
add_dependencies(test-tree tree)
# Aggrégation du programme de test avec la librairie tree
target_link_libraries(test-tree tree)

# Activation des tests
enable_testing()
# Ajout d'un test
add_test(test-tree ./test-tree)
