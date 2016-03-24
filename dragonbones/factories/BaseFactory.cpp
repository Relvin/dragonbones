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

cocos2d::Node* BaseFactory::getTextureDisplay(const std::string &textureName, const std::string &textureAtlasName, const DisplayData *displayData) const
{
    ITextureAtlas *textureAtlas = nullptr;
    TextureData *textureData = nullptr;
    
    if (!textureAtlasName.empty())
    {
        auto iterator = _textureAtlasMap.find(textureAtlasName);
        
        if (iterator != _textureAtlasMap.end())
        {
            textureAtlas = iterator->second;
            textureData = textureAtlas->textureAtlasData->getTextureData(textureName);
        }
    }
    
    if (!textureData)
    {
        AutoSearchType searchType = (textureAtlasName.empty() ? AutoSearchType::AST_ALL : (autoSearchTexture ? AutoSearchType::AST_AUTO : AutoSearchType::AST_NONE));
        
        if (searchType != AutoSearchType::AST_NONE)
        {
            for (auto iterator = _textureAtlasMap.begin(); iterator != _textureAtlasMap.end(); ++iterator)
            {
                textureAtlas = iterator->second;
                
                if (searchType == AutoSearchType::AST_ALL || textureAtlas->textureAtlasData->autoSearch)
                {
                    textureData = textureAtlas->textureAtlasData->getTextureData(textureName);
                    
                    if (textureData)
                    {
                        break;
                    }
                }
            }
        }
    }
    
    if (!textureData)
    {
        return nullptr;
    }
    
    if (!displayData)
    {
        auto iterator = _dragonBonesDataMap.find(textureAtlas->textureAtlasData->name);
        
        if (iterator != _dragonBonesDataMap.end())
        {
            DragonBonesData *dragonBonesData = iterator->second;
            
            for (size_t i = 0, l = dragonBonesData->armatureDataList.size(); i < l; ++i)
            {
                for (size_t j = 0, l = dragonBonesData->armatureDataList[i]->skinDataList.size(); j < l; ++j)
                {
                    for (size_t k = 0, l = dragonBonesData->armatureDataList[i]->skinDataList[j]->slotDataList.size(); k < l; ++k)
                    {
                        for (size_t m = 0, l = dragonBonesData->armatureDataList[i]->skinDataList[j]->slotDataList[k]->displayDataList.size(); m < l; ++m)
                        {
                            displayData = dragonBonesData->armatureDataList[i]->skinDataList[j]->slotDataList[k]->displayDataList[m];
                            
                            if (displayData->name != textureName)
                            {
                                displayData = nullptr;
                            }
                            else
                            {
                                break;
                            }
                        }
                        
                        if (displayData)
                        {
                            break;
                        }
                    }
                    
                    if (displayData)
                    {
                        break;
                    }
                }
                
        if (displayData)
                {
                    break;
                }
            }
        }
    }
    
    return generateDisplay(textureAtlas, textureData, displayData);
}

NAME_SPACE_DRAGON_BONES_END
