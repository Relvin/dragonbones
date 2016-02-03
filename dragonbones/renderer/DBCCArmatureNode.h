#ifndef DRAGONBONES_RENDERER_DBCC_ARMATURE_NODE_H
#define DRAGONBONES_RENDERER_DBCC_ARMATURE_NODE_H

#include "dbccMacro.h"
#include "DragonBonesHeaders.h"
#include "cocos2d.h"
#include "DBCCSlot.h"
#include "DBCCArmature.h"

#if (DRAGON_BONES_ENABLE_LUA == 1)
#include "CCLuaValue.h"
#endif // DRAGON_BONES_ENABLE_LUA


NAME_SPACE_DRAGON_BONES_BEGIN
class DBCCArmatureNode : public cocos2d::Node
{
public:
    static DBCCArmatureNode* createWithFile(const std::string& name);
    static DBCCArmatureNode* createWithFile(const std::string& dragonBonesFileName,const std::string& textureFileName);

    bool initWithFile(const std::string& name);
    bool initWithFile(const std::string& dragonBonesFileName,const std::string& textureFileName);
    static DBCCArmatureNode* createWithName(const std::string& name);
    static DBCCArmatureNode* createWithName(const std::string& armatureName,const std::string& dragonBonesName);
    bool initWithName(const std::string& name);
    bool initWithName(const std::string& armatureName,const std::string& dragonName);
    bool initWithName(const std::string &armatureName, const std::string &skinName, const std::string &animationName,
                      const std::string &dragonBonesName, const std::string &textureAtlasName);
    
    DBCCSlot* getCCSlot(const std::string &slotName) const { return _armature->getCCSlot(slotName); };
	cocos2d::Node* getCCDisplay() const { return _armature->getCCDisplay(); };
	cocos2d::EventDispatcher* getCCEventDispatcher() const { return _armature->getCCEventDispatcher(); };
	cocos2d::Rect getBoundingBox() const override;
    cocos2d::Rect getInnerBoundingBox() const;

#if (DRAGON_BONES_ENABLE_LUA == 1)
	virtual void registerFrameEventHandler(cocos2d::LUA_FUNCTION func);
	virtual void registerAnimationEventHandler(cocos2d::LUA_FUNCTION func);
	virtual void unregisterFrameEventHandler();
	virtual void unregisterAnimationEventHandler();
#endif // DRAGON_BONES_ENABLE_LUA


public:
    // create DBCCArmatureNode without WorldClock
    static DBCCArmatureNode* createWithArmature(DBCCArmature *armature);
    /**
     * create DDCCArmatureNode with WorldClock
     * @param armature
     * @param clock if null, use WorldClock::getInstance()
     * @return 
     */
    virtual bool initWithArmature(DBCCArmature *armature);
    
    DBCCArmatureNode();
    virtual ~DBCCArmatureNode();

	DBCCArmature* getArmature() const { return _armature; };
	//Animation* getAnimation() const { return _armature->getAnimation(); };
	Animation* getAnimation() { return _armature->getAnimation(); };

	virtual void update(float dt) override;

protected:
    DBCCArmature *_armature;
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBCCArmatureNode);
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_RENDERER_DBCC_ARMATURE_NODE_H
