#ifndef DRAGONBONES_RENDERER_DBCC_FACTORY_H
#define DRAGONBONES_RENDERER_DBCC_FACTORY_H

#include "DragonBonesHeaders.h"
#include "DBCCRenderHeaders.h"
#include "cocos2d.h"
#include "factories/BaseFactory.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class DBCCFactory : public BaseFactory
{
public:
	static DBCCFactory* getInstance();
	static void destroyInstance();

    DBCCFactory();
    virtual ~DBCCFactory();

    virtual DragonBonesData* loadDragonBonesData(const std::string &dragonBonesFile, const std::string &name = "");
    virtual ITextureAtlas* loadTextureAtlas(const std::string &textureAtlasFile, const std::string &name = "");
    virtual void refreshTextureAtlasTexture(const std::string &name);
    virtual void refreshAllTextureAtlasTexture();
    virtual bool hasDragonBones(const std::string &skeletonName, const std::string &armatureName = "", const std::string &animationName = "");
    void removeUnusedDragonBonesData() ;
    TextureAtlasData* loadTextureWithPlist(const std::string &path,const char* filedata, int filesize);
protected:

private:
	static DBCCFactory *_instance;

private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBCCFactory);
};
NAME_SPACE_DRAGON_BONES_END

#endif  // DRAGONBONES_RENDERER_DBCC_FACTORY_H
