#ifndef DRAGONBONES_FACTORIES_BASE_FACTORY_H
#define DRAGONBONES_FACTORIES_BASE_FACTORY_H

#include "DragonBones.h"
#include "2d/CCNode.h"

NAME_SPACE_DRAGON_BONES_BEGIN

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
    
    virtual cocos2d::Node* getTextureDisplay(const std::string &textureName, const std::string &textureAtlasName = "", const DisplayData *displayData = nullptr) const;

protected:
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
