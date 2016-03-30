#include "DBCCFactory.h"
#include "DBCCTextureAtlas.h"
#include "cocos2d.h"

NAME_SPACE_DRAGON_BONES_BEGIN

namespace
{
	std::string getFilePosfix(const std::string &filename)
	{
		std::string reDir = filename;
		std::string file_extension = "";
		size_t pos = reDir.find_last_of('.');
		if (pos != std::string::npos)
		{
			file_extension = reDir.substr(pos, reDir.length());
			std::transform(file_extension.begin(),file_extension.end(), file_extension.begin(), (int(*)(int))toupper);
		}
		return file_extension;
	}
}

DBCCFactory* DBCCFactory::_instance = nullptr;

DBCCFactory* DBCCFactory::getInstance()
{
    if (!_instance)
    {
        _instance = new DBCCFactory();
    }
    return _instance;
}

void DBCCFactory::destroyInstance()
{
    if (_instance)
    {
       CC_SAFE_DELETE(_instance);
    }
}

DBCCFactory::DBCCFactory() {}
DBCCFactory::~DBCCFactory() {}

DragonBonesData* DBCCFactory::loadDragonBonesData(const std::string &dragonBonesFilePath, const std::string &name)
{
    DragonBonesData *existDragonBonesData = getDragonBonesData(name);

    if (existDragonBonesData)
    {
        return existDragonBonesData;
    }
	cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(dragonBonesFilePath);
    if (data.getSize() == 0)
    {
		CCLOG("read file [%s] error!", dragonBonesFilePath.c_str());
        return nullptr;
    }
	DragonBonesData *dragonBonesData = nullptr;
	const std::string filePosfix = getFilePosfix(dragonBonesFilePath);
	if (".JSON" == filePosfix)
	{
		dragonBonesData = JSONDataParser::parseDragonBonesData(reinterpret_cast<char*>(data.getBytes()));
	}
	else if (".XML" == filePosfix)
	{

#ifdef COCOS2D_DEBUG
        timeval t_beg,t_end;
        gettimeofday(&t_beg, NULL);
#endif
        float scale = cocos2d::Director::getInstance()->getContentScaleFactor();
		// load skeleton.xml using XML parser.
		dragonBones::XMLDocument doc;
		doc.Parse(reinterpret_cast<char*>(data.getBytes()), data.getSize());
		// paser dragonbones skeleton data.
		dragonBones::XMLDataParser parser;
		dragonBonesData = parser.parseDragonBonesData(doc.RootElement(),scale);
#ifdef COCOS2D_DEBUG
        size_t pos = dragonBonesFilePath.rfind('/');
        std::string fileName = dragonBonesFilePath;
        if (0 < pos) {
            fileName = fileName.substr(pos + 1, std::string::npos);
        }
        gettimeofday(&t_end, NULL);
        CCLOG("%s load waste time = %f s",fileName.c_str(),(t_end.tv_usec - t_beg.tv_usec) / 1000000.f);
#endif
	}
    else if (".XMLB" == filePosfix)
    {
#ifdef COCOS2D_DEBUG
        timeval t_beg,t_end;
        gettimeofday(&t_beg, NULL);
#endif
        float scale = cocos2d::Director::getInstance()->getContentScaleFactor();
        dragonBones::BinaryParser parser;
        dragonBonesData = parser.parseDragonBonesData(data.getBytes(),scale);
#ifdef COCOS2D_DEBUG
        size_t pos = dragonBonesFilePath.rfind('/');
        std::string fileName = dragonBonesFilePath;
        if (0 < pos) {
            fileName = fileName.substr(pos + 1, std::string::npos);
        }
        gettimeofday(&t_end, NULL);
        CCLOG("%s load waste time = %f s",fileName.c_str(),(t_end.tv_usec - t_beg.tv_usec) / 1000000.f);
#endif
    }
	else
	{
		CCLOG("read file [%s] error!", dragonBonesFilePath.c_str());
		return nullptr;
	}

    addDragonBonesData(dragonBonesData, name);
    return dragonBonesData;
}

ITextureAtlas* DBCCFactory::loadTextureAtlas(const std::string &textureAtlasFile, const std::string &name)
{
    ITextureAtlas *existTextureAtlas = getTextureAtlas(name);

    if (existTextureAtlas)
    {
        refreshTextureAtlasTexture(name.empty() ? existTextureAtlas->textureAtlasData->name : name);
        return existTextureAtlas;
    }

	const std::string data = cocos2d::FileUtils::getInstance()->getStringFromFile(textureAtlasFile);
	if (data.empty())
	{
		CCLOG("read file [%s] error!", textureAtlasFile.c_str());
		return nullptr;
	}

	DBCCTextureAtlas *textureAtlas = new DBCCTextureAtlas();
	const std::string filePosfix = getFilePosfix(textureAtlasFile);
	if (".JSON" == filePosfix)
	{
		textureAtlas->textureAtlasData = JSONDataParser::parseTextureAtlasData(data.c_str());
	}
	else if (".XML" == filePosfix)
	{
		// textureAtlas scale
		float scale = cocos2d::Director::getInstance()->getContentScaleFactor();
		dragonBones::XMLDocument doc;
		doc.Parse(data.c_str(), data.size());
		dragonBones::XMLDataParser parser;
		textureAtlas->textureAtlasData = parser.parseTextureAtlasData(doc.RootElement(), scale);
	}
	else
	{
		CCLOG("read file [%s] error!", textureAtlasFile.c_str());
		return nullptr;
	}

    int pos = textureAtlasFile.find_last_of("/");
    if (std::string::npos != pos)
    {
        std::string base_path = textureAtlasFile.substr(0, pos + 1);
        textureAtlas->textureAtlasData->imagePath = base_path + textureAtlas->textureAtlasData->imagePath;
    }

    //
    addTextureAtlas(textureAtlas, name);
    refreshTextureAtlasTexture(name.empty() ? textureAtlas->textureAtlasData->name : name);
    return textureAtlas;
}

void DBCCFactory::refreshTextureAtlasTexture(const std::string &name)
{
    for (auto iterator = _textureAtlasMap.begin(); iterator != _textureAtlasMap.end(); ++iterator)
    {
        DBCCTextureAtlas *textureAtlas = static_cast<DBCCTextureAtlas*>(iterator->second);
        const TextureAtlasData *textureAtlasData = textureAtlas->textureAtlasData;

        if (iterator->first == name)
        {
            textureAtlas->reloadTexture();
        }
    }
}

void DBCCFactory::refreshAllTextureAtlasTexture()
{
    for (auto iterator = _textureAtlasMap.begin(); iterator != _textureAtlasMap.end(); ++iterator)
    {
        DBCCTextureAtlas *textureAtlas = static_cast<DBCCTextureAtlas*>(iterator->second);
        const TextureAtlasData *textureAtlasData = textureAtlas->textureAtlasData;
        textureAtlas->reloadTexture();
    }
}

bool DBCCFactory::hasDragonBones(const std::string &skeletonName, const std::string &armatureName, const std::string &animationName)
{
    auto dragonbonesData = getDragonBonesData(skeletonName);

    if (!dragonbonesData) { return false; }

    if (!armatureName.empty())
    {
        auto armatureData = dragonbonesData->getArmatureData(armatureName);

        if (!armatureData) { return false; }

        if (!animationName.empty())
        {
            auto animationData = armatureData->getAnimationData(animationName);
            return animationData != nullptr;
        }
    }

    return true;
}

void DBCCFactory::removeUnusedDragonBonesData()
{
    auto iterator = _dragonBonesDataMap.begin();
    
    while (iterator != _dragonBonesDataMap.end())
    {
        if (iterator->second->getReferenceCount() == 1)
        {
            iterator->second->dispose();
            delete iterator->second;
            iterator = _dragonBonesDataMap.erase(iterator);
        }
        else
        {
            iterator++;
        }
        
    }
#ifdef COCOS2D_DEBUG
    if (_dragonBonesDataMap.size() != 0)
    {
        CCLOG("%s,%d dragonBonesDataCache size is %lu",__FUNCTION__,__LINE__,_dragonBonesDataMap.size());
    }
#endif
}


NAME_SPACE_DRAGON_BONES_END
