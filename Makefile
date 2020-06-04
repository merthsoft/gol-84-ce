# ----------------------------
# Program Options
# ----------------------------

NAME		 ?= GOL
ICON		 ?= assets/icon.png
DESCRIPTION  ?= "Game of Life"
COMPRESSED   ?= YES
ARCHIVED	 ?= NO
OUTPUT_MAP   ?= NO

.DEFAULT_GOAL := debug
include $(CEDEV)/include/.makefile