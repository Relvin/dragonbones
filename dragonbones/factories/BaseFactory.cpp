#include "BaseFactory.h"
#include "objects/ArmatureData.h"
#include "objects/DragonBonesData.h"
#include "textures/ITextureAtlas.h"

NAME_SPACE_DRAGON_BONES_BEGIN

const std::map<std::string, DragonBonesData*>& BaseFactory::getDragonBonesDataMap() const
{
    return _dragonBonesDataMap;
}

const std::map<std::string, ITextureAtlas*>& BaseFactory::getTextureAtlasMap() const
{
    return _textureAtlasMap;
}

BaseFactory::BaseFactory()
{
    autoSearchDragonBonesData = false;
    autoSearchTexture = false;
}

BaseFactory::~BaseFactory()
{
    dispose();
}

DragonBonesData* BaseFactory::getDragonBonesData(const std::string &name) const
{
    auto iterator = _dragonBonesDataMap.find(name);
    
    if (iterator != _dragonBonesDataMap.end())
    {
        return iterator->second;
    }
    else
    {
        return nullptr;
    }
}

void BaseFactory::addDragonBonesData(DragonBonesData *data, const std::string &name)
{
    DBASSERT(data, "Invalid data.");
    
    const std::string &key = name.empty() ? data->name : name;

    DBASSERT(!key.empty(), "Name is empty.");
    
    DBASSERT(_dragonBonesDataMap.find(key) == _dragonBonesDataMap.end(), "Data has been added.");

    _dragonBonesDataMap[key] = data;
}

void BaseFactory::removeDragonBonesData(const std::string &name, bool disposeData)
{
    auto iterator = _dragonBonesDataMap.find(name);
    
    if (iterator != _dragonBonesDataMap.end())
    {
        if (disposeData)
        {
            iterator->second->dispose();
            delete iterator->second;
        }
        
        _dragonBonesDataMap.erase(iterator);
    }
}

ITextureAtlas* BaseFactory::getTextureAtlas(const std::string &name) const
{
    auto iterator = _textureAtlasMap.find(name);
    
    if (iterator != _textureAtlasMap.end())
    {
        return iterator->second;
    }
    else
    {
        return nullptr;
    }
}
void BaseFactory::addTextureAtlas(ITextureAtlas *textureAtlas, const std::string &name)
{
    DBASSERT(textureAtlas, "Invalid textureAtlas.");
    
    const std::string &key = name.empty() ? textureAtlas->textureAtlasData->name : name;
    
    DBASSERT(!key.empty(), "Name is empty.");
    
    DBASSERT(_textureAtlasMap.find(key) == _textureAtlasMap.end(), "Data has been added.");
    
    _textureAtlasMap[key] = textureAtlas;
}

void BaseFactory::removeTextureAtlas(const std::string &name, bool disposeData)
{
    auto iterator = _textureAtlasMap.find(name);
    
    if (iterator != _textureAtlasMap.end())
    {
        if (disposeData)
        {
            iterator->second->dispose();
            delete iterator->second;
        }
        
        _textureAtlasMap.erase(iterator);
    }
}

void BaseFactory::dispose(bool disposeData)
{
    if (disposeData)
    {
        for (auto iterator = _dragonBonesDataMap.begin(); iterator != _dragonBonesDataMap.end(); ++iterator)
        {
            iterator->second->dispose();
            delete iterator->second;
        }
        
        for (auto iterator = _textureAtlasMap.begin(); iterator != _textureAtlasMap.end(); ++iterator)
        {
            iterator->second->dispose();
            delete iterator->second;
        }
    }
    
    _dragonBonesDataMap.clear();
    _textureAtlasMap.clear();
}

NAME_SPACE_DRAGON_BONES_END
