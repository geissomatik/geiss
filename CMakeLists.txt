# This file is part of: Geiss screensaver and Winamp music visualization plug-in
#                           ___ ___ ___ ___ ___
#                          / __| __|_ _/ __/ __|
#                         | (_ | _| | |\__ \__ \
#                          \___|___|___|___/___/
# 3-Clause BSD License
#
# Copyright (c) 2022-2022 Sebastian Pipping <sebastian@pipping.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
cmake_minimum_required(VERSION 3.15.0)  # for property MSVC_RUNTIME_LIBRARY

project(geiss)

set(_GEISS_SOURCES
#   AFXRES.H
#   CPU_MHZ.H
#   CPU_TYPE.H
#   CYRIX.H
#   DEFINES.H
#   Effects.h
#   GETDXVER.H
    helper.cpp
#   helper.h
    main.cpp
#   outsound.cpp
#   outsound.h
    proc_map.cpp
#   Proc_map.h
    Resource.rc
#   resource.h
#   SOUND.CPP
#   SOUND.H
#   Sysstuff.h
#   video.h
#   VIS.H
)

add_executable(geiss_scr WIN32 ${_GEISS_SOURCES})
target_compile_definitions(geiss_scr PRIVATE SAVER)

# NOTE: Only a single "s" due to 8.3 naming limitations back then
add_library(vis_geis MODULE ${_GEISS_SOURCES})
target_compile_definitions(vis_geis PRIVATE PLUGIN)

foreach(_target geiss_scr vis_geis)
    target_link_libraries(${_target} comctl32.lib ddraw.lib dsound.lib dxguid.lib winmm.lib)
    # NOTE: Wine doesn't come with debug(!) DLL "VCRUNTIME140D.dll" so we use the non-DLL CRT version:
    set_target_properties(${_target} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endforeach()
