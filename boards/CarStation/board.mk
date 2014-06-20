# List of all the board related files.
BOARDSRC = ./boards/CarStation/board.c

# Required include directories
BOARDINC = ${CHIBIOS}/boards/CarStation

# Below for ugfx
GFXINC  += ./boards/CarStation
GFXSRC  +=
GFXDEFS += -DGFX_USE_OS_CHIBIOS=TRUE
include $(GFXLIB)/drivers/gdisp/SSD1306/gdisp_lld.mk

