# ----------------------------
# Program Options
# ----------------------------

NAME		 ?= GOL
ICON		 ?= src/gfx/iconc.png
DESCRIPTION  ?= "Game of Life"
COMPRESSED   ?= YES
ARCHIVED	 ?= NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

include $(shell cedev-config --makefile)