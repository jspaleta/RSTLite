# Makefile for RPosMag
# ====================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/superdarn 
IGNVER=1
OBJS = RPosMag.o
OUTPUT = RPosMag
LIBS=-lrpos.1 -lradar.1 -lrtime.1 -lrcnv.1 -laacgm.1 -ligrf.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
