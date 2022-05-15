#=============================================================================
# SPDX-FileCopyrightText: 2022 Chris Roberts
#
# SPDX-License-Identifier: MIT
#=============================================================================

#
# - Sphinx module for CMake
#

include(FindPackageHandleStandardArgs)

find_program(SPHINX_BUILD_EXE NAMES sphinx-build)

mark_as_advanced(SPHINX_BUILD_EXE)

find_package_handle_standard_args(Sphinx DEFAULT_MSG SPHINX_BUILD_EXE)
