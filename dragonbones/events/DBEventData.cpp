//
//  DBEventData.cpp
//  dragonbones
//
//  Created by Relvin on 16/3/23.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBEventData.h"

NAME_SPACE_DRAGON_BONES_BEGIN

const std::string DBEventData::Z_ORDER_UPDATED = "zorderUpdate";
const std::string DBEventData::ANIMATION_FRAME_EVENT = "animationFrameEvent";
const std::string DBEventData::BONE_FRAME_EVENT = "boneFrameEvent";
const std::string DBEventData::SOUND = "sound";
const std::string DBEventData::FADE_IN = "fadeIn";
const std::string DBEventData::FADE_OUT = "fadeOut";
const std::string DBEventData::START = "start";
const std::string DBEventData::COMPLETE = "complete";
const std::string DBEventData::LOOP_COMPLETE = "loopComplete";
const std::string DBEventData::FADE_IN_COMPLETE = "fadeInComplete";
const std::string DBEventData::FADE_OUT_COMPLETE = "fadeOutComplete";

const std::string DBEventData::_ERROR = "error";

const std::string& DBEventData::typeToString(DBEventData::EventType eventType)
{
    switch (eventType)
    {
        case EventType::Z_ORDER_UPDATED:
        return Z_ORDER_UPDATED;
        
        case EventType::ANIMATION_FRAME_EVENT:
        return ANIMATION_FRAME_EVENT;
        
        case EventType::BONE_FRAME_EVENT:
        return BONE_FRAME_EVENT;
        
        case EventType::SOUND:
        return SOUND;
        
        case EventType::FADE_IN:
        return FADE_IN;
        
        case EventType::FADE_OUT:
        return FADE_OUT;
        
        case EventType::START:
        return START;
        
        case EventType::COMPLETE:
        return COMPLETE;
        
        case EventType::LOOP_COMPLETE:
        return LOOP_COMPLETE;
        
        case EventType::FADE_IN_COMPLETE:
        return FADE_IN_COMPLETE;
        
        case EventType::FADE_OUT_COMPLETE:
        return FADE_OUT_COMPLETE;
        
        default:
        break;
    }
    
    // throw
    return _ERROR;
}

DBEventDataMgr::DBEventDataMgr()
{
    
}

DBEventDataMgr::~DBEventDataMgr()
{
    
}

DBEventDataMgr* DBEventDataMgr::create()
{
    DBEventDataMgr *eventDataMgr = new (std::nothrow) DBEventDataMgr();
    if (eventDataMgr && eventDataMgr->init())
    {
        eventDataMgr->autorelease();
        return eventDataMgr;
    }
    CC_SAFE_DELETE(eventDataMgr);
    return nullptr;
}

bool DBEventDataMgr::init()
{
    if (!MultData<DBEventData>::init())
    {
        return false;
    }
    return true;
}

DBEventData* DBEventDataMgr::getUnusedEventData()
{
    auto eventData = MultData<DBEventData>::getUnusedData();
    eventData->resetEventData();
    return eventData;
}

void DBEventDataMgr::removeEventData(DBEventData* eventData)
{
    if (!eventData)
    {
        return;
    }
    eventData->resetEventData();
    MultData<DBEventData>::removeDataFromUsed(eventData);
}

void DBEventDataMgr::removeAllEventData()
{
    
    for (auto iter = getUsedVectorBegin(); iter != getUsedVectorEnd();++iter )
    {
        (*iter)->resetEventData();
    }
    MultData<DBEventData>::removeAllDataFromUsed();
}


DBEventData::EventType DBEventData::getType() const
{
    return _type;
}

const std::string& DBEventData::getStringType() const
{
    return typeToString(_type);
}

DBEventData* DBEventData::create()
{
    DBEventData* eventData = new (std::nothrow) DBEventData();
    if (eventData && eventData->init())
    {
        eventData->autorelease();
        return eventData;
    }
    CC_SAFE_DELETE(eventData);
    return nullptr;
}

bool DBEventData::init()
{
    if (!this->initWithTypeAndArmature(EventType::_ERROR, nullptr))
    {
        return false;
    }
    return true;
}

DBEventData* DBEventData::create(dragonBones::DBEventData::EventType type, dragonBones::DBArmature *armatureTarget)
{
    DBEventData* eventData = new (std::nothrow) DBEventData();
    if (eventData && eventData->initWithTypeAndArmature(type, armatureTarget))
    {
        eventData->autorelease();
        return eventData;
    }
    CC_SAFE_DELETE(eventData);
    return nullptr;
}

bool DBEventData::initWithTypeAndArmature(dragonBones::DBEventData::EventType type, dragonBones::DBArmature *armatureTarget)
{
    _type = type;
    _armature = armatureTarget;
    _bone = nullptr;
    _animationState = nullptr;
    _frame = nullptr;
    _frameLabel.clear();
    _sound.clear();
    
    return true;
}



DBEventData::DBEventData()
{
    _type = EventType::_ERROR;
    _armature = nullptr;
    _bone = nullptr;
    _animationState = nullptr;
    _frame = nullptr;
    _frameLabel.clear();
    _sound.clear();
}

DBEventData::~DBEventData()
{
    resetEventData();
}

void DBEventData::resetEventData()
{
    _armature = nullptr;
    _bone = nullptr;
    _animationState = nullptr;
    _frame = nullptr;
    _type = EventType::_ERROR;
    
    _frameLabel.clear();
    _sound.clear();
}

void DBEventData::setType(dragonBones::DBEventData::EventType type)
{
    this->_type = type;
}

const DBArmature* DBEventData::getArmature() const
{
    return this->_armature;
}

void DBEventData::setArmature(dragonBones::DBArmature *armature)
{
    this->_armature = armature;
}

const DBBone* DBEventData::getBone() const
{
    return this->_bone;
}

void DBEventData::setBone(dragonBones::DBBone *bone)
{
    this->_bone = bone;
}

const Frame* DBEventData::getFrame() const
{
    return this->_frame;
}

void DBEventData::setFrame(dragonBones::Frame *frame)
{
    this->_frame = frame;
}

const DBAnimationState* DBEventData::getAnimationState() const
{
    return this->_animationState;
}

void DBEventData::setAnimationState(dragonBones::DBAnimationState *animationState)
{
    this->_animationState = animationState;
}

const std::string& DBEventData::getFrameLabel() const
{
    return this->_frameLabel;
}

void DBEventData::setFrameLabel(const std::string &frameLabel)
{
    this->_frameLabel = frameLabel;
}

const std::string& DBEventData::getSound() const
{
    return this->_sound;
}

void DBEventData::setSound(const std::string &sound)
{
    this->_sound = sound;
}

NAME_SPACE_DRAGON_BONES_END