#ifndef DRAGONBONES_FACTORIES_BASE_FACTORY_H
#define DRAGONBONES_FACTORIES_BASE_FACTORY_H

#include "DragonBones.h"
#include "2d/CCNode.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class Slot;
class Bone;
class Armature;
class ArmatureData;
class ITextureAtlas;
class DragonBonesData;
class DisplayData;
class SkinData;
class SlotData;
class TextureData;

class BaseFactory
{
    enum class AutoSearchType {AST_ALL, AST_AUTO, AST_NONE};
    
public:
    BaseFactory();
    virtual ~BaseFactory();
    virtual void dispose(bool disposeData = true);
    
	const std::map<std::string, DragonBonesData*>& getDragonBonesDataMap() const;
	const std::map<std::string, ITextureAtlas*>& getTextureAtlasMap() const;

    virtual DragonBonesData* getDragonBonesData(const std::string &name) const;
    virtual void addDragonBonesData(DragonBonesData *data, const std::string &name = "");
    virtual void removeDragonBonesData(const std::string &name, bool disposeData = true);
    
    virtual ITextureAtlas* getTextureAtlas(const std::string &name) const;
    virtual void addTextureAtlas(ITextureAtlas *textureAtlas, const std::string &name = "");
    virtual void removeTextureAtlas(const std::string &name, bool disposeData = true);
    
    virtual Armature* buildArmature(const std::string &armatureName) const;
    virtual Armature* buildArmature(const std::string &armatureName, const std::string &dragonBonesName) const;
    virtual Armature* buildArmature(const std::string &armatureName, const std::string &skinName, const std::string &animationName, const std::string &dragonBonesName, const std::string &textureAtlasName) const;
    virtual cocos2d::Node* getTextureDisplay(const std::string &textureName, const std::string &textureAtlasName = "", const DisplayData *displayData = nullptr) const;
    
protected:
    virtual void buildBones(Armature *armature, const ArmatureData *armatureData) const;
    virtual void buildSlots(Armature *armature, const ArmatureData *armatureData, const SkinData *skinData, const SkinData *skinDataCopy) const;
    
    virtual Armature* generateArmature(const ArmatureData *armatureData) const = 0;
    virtual Slot* generateSlot(const SlotData *slotData) const = 0;
    virtual cocos2d::Node* generateDisplay(const ITextureAtlas *textureAtlas, const TextureData *textureData, const DisplayData *displayData) const = 0;
    
public:
	bool autoSearchDragonBonesData;
	bool autoSearchTexture;

protected:
	mutable std::string _currentDragonBonesDataName;
	mutable std::string _currentTextureAtlasName;
	std::map<std::string, DragonBonesData*> _dragonBonesDataMap;
	std::map<std::string, ITextureAtlas*> _textureAtlasMap;

private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(BaseFactory);
};
NAME_SPACE_DRAGON_BONES_END
#endif // DRAGONBONES_FACTORIES_BASE_FACTORY_H
