#include "dbccMacro.h"
#include "DBCCArmatureNode.h"
#include "DBCCFactory.h"


NAME_SPACE_DRAGON_BONES_BEGIN

DBCCArmatureNode* DBCCArmatureNode::createWithName(const std::string& name)
{
    DBCCArmatureNode* ret = new (std::nothrow) DBCCArmatureNode();
    if (ret && ret->initWithName(name))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}




bool DBCCArmatureNode::initWithName(const std::string& name)
{
//    dragonBones::DBCCArmatureNode* armNode = dragonBones::DBCCFactory::getInstance()->buildArmatureNode(name,name);

    return true;
}

#if 0
DBCCArmatureNode* DBCCFactory::buildArmatureNode(const std::string &armatureName) const
{
    auto armature = buildArmature(armatureName);
    return DBCCArmatureNode::create(armature);
}

DBCCArmatureNode* DBCCFactory::buildArmatureNode(const std::string &armatureName, const std::string &dragonBonesName) const
{
    auto armature = buildArmature(armatureName, dragonBonesName);
    return DBCCArmatureNode::create(armature);
}

DBCCArmatureNode* DBCCFactory::buildArmatureNode(const std::string &armatureName, const std::string &skinName, const std::string &animationName,
                                                 const std::string &dragonBonesName, const std::string &textureAtlasName) const
{
    auto armature = buildArmature(armatureName, skinName, animationName, dragonBonesName, textureAtlasName);
    return DBCCArmatureNode::create(armature);
}
#endif







DBCCArmatureNode* DBCCArmatureNode::createWithArmature(DBCCArmature *armature)
{
    DBCCArmatureNode *ret = new DBCCArmatureNode();
    
    if (ret && ret->initWithArmature(armature))
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    
    return ret;
}

bool DBCCArmatureNode::initWithArmature(DBCCArmature *armature)
{
    if (armature != nullptr)
    {
        _armature = armature;
        _armature->setArmatureNode(this);
        
        {
            scheduleUpdate();
        }
        addChild(armature->getCCDisplay());
        setCascadeOpacityEnabled(true);
        setCascadeColorEnabled(true);
        return true;
    }

	return false;
}


DBCCArmatureNode::DBCCArmatureNode()
    :_armature(nullptr)
{
}
DBCCArmatureNode::~DBCCArmatureNode()
{
    
    {
        unscheduleUpdate();
    }

    if (_armature)
    {
        delete _armature;
        _armature = nullptr;
    }
}

cocos2d::Rect DBCCArmatureNode::getBoundingBox() const
{
    auto rect = _armature->getCCBoundingBox();
    return cocos2d::RectApplyTransform(rect, getNodeToParentTransform());
}

cocos2d::Rect DBCCArmatureNode::getInnerBoundingBox() const
{
    return _armature->getCCBoundingBox();
}

void DBCCArmatureNode::update(float dt)
{
    retain();
    _armature->advanceTime(dt);
    release();
}

///////////////////////////////////////////////////////////////////////
/////////////// for lua
///////////////////////////////////////////////////////////////////////

#if (DRAGON_BONES_ENABLE_LUA == 1)

void DBCCArmatureNode::registerFrameEventHandler(cocos2d::LUA_FUNCTION func)
{
	_armature->registerFrameEventHandler(func);
}

void DBCCArmatureNode::registerAnimationEventHandler(cocos2d::LUA_FUNCTION func)
{
	_armature->registerAnimationEventHandler(func);
}

void DBCCArmatureNode::unregisterFrameEventHandler()
{
	_armature->unregisterFrameEventHandler();
}

void DBCCArmatureNode::unregisterAnimationEventHandler()
{
	_armature->unregisterAnimationEventHandler();
}

#endif // !DRAGON_BONES_ENABLE_LUA


NAME_SPACE_DRAGON_BONES_END
