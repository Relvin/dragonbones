LOCAL_PATH := $(call my-dir)

LOCAL_MODULE := dragonbones_static

LOCAL_MODULE_FILENAME := libdragonbones


LOCAL_SRC_FILES := ../../3rd/tinyxml2/dbtinyxml2.cpp \
                   ../../animation/DBAnimation.cpp \
                   ../../animation/DBAnimationState.cpp \
                   ../../animation/DBFFDTimelineState.cpp \
                   ../../animation/DBSlotTimelineState.cpp \
                   ../../animation/DBTimelineState.cpp \
                   ../../core/DBBase.cpp \
                   ../../events/DBEventData.cpp \
                   ../../factories/BaseFactory.cpp \
                   ../../objects/FFDFrame.cpp \
                   ../../objects/FFDTimeline.cpp \
                   ../../objects/IKData.cpp \
                   ../../objects/MeshData.cpp \
                   ../../parsers/BaseDataParser.cpp \
                   ../../parsers/BinaryParser.cpp \
                   ../../parsers/ConstValues.cpp \
                   ../../parsers/JSONDataParser.cpp \
                   ../../parsers/XMLDataParser.cpp \
                   ../DBAnimation.cpp \
                   ../DBArmature.cpp \
                   ../DBBone.cpp \
                   ../DBCCFactory.cpp \
                   ../DBCCLuaUtils.cpp \
                   ../DBCCUtils.cpp \
                   ../DBIKConstraint.cpp \
                   ../DBSkin.cpp \
                   ../DBSlot.cpp


LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/.. \
$(LOCAL_PATH)/../../.. 

LOCAL_EXPORT_C_INCLUDES := \
$(LOCAL_PATH)/.. \
$(LOCAL_PATH)/../../.. 

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static

LOCAL_CFLAGS += -Wno-psabi
LOCAL_EXPORT_CFLAGS += -Wno-psabi

include $(BUILD_STATIC_LIBRARY)

$(call import-module,.)
