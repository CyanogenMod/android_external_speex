LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES:= libspeex/fftwrap.c \
	libspeex/filterbank.c \
	libspeex/mdf.c \
	libspeex/preprocess.c \
	libspeex/smallft.c

LOCAL_MODULE:= libspeex

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS+= -DFLOATING_POINT -DEXPORT="" -DUSE_SMALLFT
LOCAL_CFLAGS+= -O3 -fstrict-aliasing -fprefetch-loop-arrays 

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include

include $(BUILD_STATIC_LIBRARY)
