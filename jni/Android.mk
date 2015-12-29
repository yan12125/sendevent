LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= sendevent.cpp screen.cpp

LOCAL_MODULE := sendevent

LOCAL_CFLAGS := -Wall -Wextra

include $(BUILD_EXECUTABLE)
