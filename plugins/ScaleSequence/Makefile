#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = ScaleSequence

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	ScaleSequence.cpp
	
FILES_UI = \
	ScaleSequenceUI.cpp \
	../../dpf-widgets/opengl/DearImGui.cpp
	
# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk
BUILD_CXX_FLAGS += -I../../tuning-library/include
BUILD_CXX_FLAGS += -I../../MTS-ESP/Master
BUILD_CXX_FLAGS += -I../../dpf-widgets/generic
BUILD_CXX_FLAGS += -I../../dpf-widgets/opengl

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS += jack
TARGETS += clap
TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3

all: $(TARGETS)

# --------------------------------------------------------------
