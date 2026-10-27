cmake_minimum_required(VERSION 4.0)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)

project(List C)
# add_executable(tree-rb tree-rb.c tree-rb.h)
add_library(tree-rb SHARED tree-rb.c tree-rb.h)

install(
	TARGETS tree-rb
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
)

install(
	FILES tree-rb.h
	DESTINATION include
)

# Ajout d'un fichier de configuration de type pkgconfig. Copie le 1er argument vers le 2ème.
# @ONLY = restreint le remplacement de variable dans tree-rb.pc.in
# à celles qui ont le format @<var>@ pour éviter les conflits avec la syntaxe CMake ${<var>}.
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/tree-rb.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/tree-rb.pc
	@ONLY
)
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/tree-rb.pc
	DESTINATION share/pkgconfig
	COMPONENT "PkgConfig"
)

#  Ajout d'un fichier de configuration de type cmake
include(CMakePackageConfigHelpers)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/TreeRBConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/TreeRBConfig.cmake
	INSTALL_DESTINATION cmake
)
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/TreeRBConfig.cmake
	DESTINATION cmake
)

# Ajout d'un exécutable dépendant également de tree-rb.h
add_executable(test-tree-rb test-tree-rb.c tree-rb.h)
# Précision de l'ordre de construction: le programme de test doit se faire
# après la librairie
add_dependencies(test-tree-rb tree-rb)
# Aggrégation du programme de test avec la librairie tree-rb
target_link_libraries(test-tree-rb tree-rb)

# Activation des tests
enable_testing()
# Ajout d'un test
add_test(test-tree-rb ./test-tree-rb)
