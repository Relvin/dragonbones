LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

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
                   ../../renderer/DBArmature.cpp \
                   ../../renderer/DBBone.cpp \
                   ../../renderer/DBCCFactory.cpp \
                   ../../renderer/DBCCUtils.cpp \
                   ../../renderer/DBIKConstraint.cpp \
                   ../../renderer/DBSkin.cpp \
                   ../../renderer/DBSlot.cpp


LOCAL_C_INCLUDES :=     $(LOCAL_PATH)/../.. \
                        $(LOCAL_PATH)/../../renderer \
                        $(LOCAL_PATH)/../../parsers \
                        $(LOCAL_PATH)/../../objects \
                        $(LOCAL_PATH)/../../factories \
                        $(LOCAL_PATH)/../../core \
                        $(LOCAL_PATH)/../../events 

LOCAL_EXPORT_C_INCLUDES :=    $(LOCAL_PATH)/../.. \
                              $(LOCAL_PATH)/../../renderer \
                              $(LOCAL_PATH)/../../parsers \
                              $(LOCAL_PATH)/../../objects \
                              $(LOCAL_PATH)/../../factories \
                              $(LOCAL_PATH)/../../core \
                              $(LOCAL_PATH)/../../events 

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static

LOCAL_CFLAGS += -Wno-psabi
LOCAL_EXPORT_CFLAGS += -Wno-psabi

include $(BUILD_STATIC_LIBRARY)

$(call import-module,.)
