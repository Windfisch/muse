#=============================================================================
#  MusE
#  Linux Music Editor
#  $Id:$
#
#  Copyright (C) 1999-2011 by Werner Schweer and others
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#=============================================================================

##
## List of source files to compile
##
file (GLOB soundtouch_bpmdetect_source_files
      BPMDetect.cpp
      FIFOSampleBuffer.cpp
      PeakFinder.cpp
      )

##
## Define target
##
add_library ( soundtouch_bpmdetect ${MODULES_BUILD} 
      ${soundtouch_bpmdetect_source_files}
      )



##
## Compilation flags and target name
##
set_target_properties( widgets_functiondialogs
      # PROPERTIES COMPILE_FLAGS "-include ${PROJECT_BINARY_DIR}/all.h ${MUSECXXFLAGS} -I../ -I${PROJECT_SOURCE_DIR}/synti "
      PROPERTIES COMPILE_FLAGS "${MUSECXXFLAGS} " 
      OUTPUT_NAME muse_soundtouch_bpmdetect
      )

##
## Linkage
##
target_link_libraries ( widgets_functiondialogs
      )


##
## Install location
##
if ( ${MODULES_BUILD} STREQUAL SHARED )
      install(TARGETS soundtouch_bpmdetect
            DESTINATION ${MusE_MODULES_DIR}
            )
endif ( ${MODULES_BUILD} STREQUAL SHARED )

