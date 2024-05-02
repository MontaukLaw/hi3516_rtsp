include ../Makefile.param

SMP_SRCS += $(wildcard $(PWD)/*.c)
SMP_INC := -I$(PWD)/

MPI_LIBS += $(SDK_PATH)/mpp/lib/libXD_Stream.a

TARGET := cool_rtsp_demo

TARGET_PATH := $(PWD)/

include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak