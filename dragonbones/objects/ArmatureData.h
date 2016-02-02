#ifndef DRAGONBONES_OBJECTS_ARMATURE_DATA_H
#define DRAGONBONES_OBJECTS_ARMATURE_DATA_H

#include "DragonBones.h"
#include "BoneData.h"
#include "SkinData.h"
#include "AnimationData.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class ArmatureData
{    
public:
    ArmatureData() {}
    ArmatureData(const ArmatureData &copyData)
    {
        operator=(copyData);
    }
    ArmatureData& operator=(const ArmatureData &copyData)
    {
        dispose();
        name = copyData.name;
        
        boneDataList.reserve(copyData.boneDataList.size());
        for (size_t i = 0, l = boneDataList.size(); i < l; ++i)
        {
            boneDataList.push_back(new BoneData());
            *(boneDataList[i]) = *(copyData.boneDataList[i]);
        }
        
        skinDataList.reserve(copyData.skinDataList.size());
        for (size_t i = 0, l = skinDataList.size(); i < l; ++i)
        {
            skinDataList.push_back(new SkinData());
            *(skinDataList[i]) = *(copyData.skinDataList[i]);
        }
        
        animationDataList.reserve(copyData.animationDataList.size());
        for (size_t i = 0, l = animationDataList.size(); i < l; ++i)
        {
            animationDataList.push_back(new AnimationData());
            *(animationDataList[i]) = *(copyData.animationDataList[i]);
        }
        
        return *this;
    }

    virtual ~ArmatureData()
    {
        dispose();
    }
    void dispose()
    {        
        for (size_t i = 0, l = boneDataList.size(); i < l; ++i)
        {
            boneDataList[i]->dispose();
            delete boneDataList[i];
        }
        for (size_t i = 0, l = skinDataList.size(); i < l; ++i)
        {
            skinDataList[i]->dispose();
            delete skinDataList[i];
        }
		for (size_t i = 0, l = slotDataList.size(); i < l; ++i)
		{
			slotDataList[i]->dispose();
			delete slotDataList[i];
		}
        for (size_t i = 0, l = animationDataList.size(); i < l; ++i)
        {
            animationDataList[i]->dispose();
            delete animationDataList[i];
        }
        
        boneDataList.clear();
        skinDataList.clear();
		slotDataList.clear();
        animationDataList.clear();
    }
    
    BoneData* getBoneData(const std::string &boneName) const
    {
        for (size_t i = 0, l = boneDataList.size(); i < l; ++i)
        {
            if (boneDataList[i]->name == boneName)
            {
                return boneDataList[i];
            }
        }
        return nullptr;
    }

    SkinData* getDefaultSkinData() const
    {
        if (skinDataList.empty())
        {
            return nullptr;
        }

        for (size_t i = 0, l = skinDataList.size(); i < l; ++i)
        {
            const std::string &skinDataName = skinDataList[i]->name;
            if (
                skinDataName.empty() || 
                skinDataName == "default"
                )
            {
                return skinDataList[i];
            }
        }
        return skinDataList.front();
    }
    
    SkinData* getSkinData(const std::string &skinName) const
    {
        if (skinDataList.empty())
        {
            return nullptr;
        }
        
        if (skinName.empty())
        {
            return getDefaultSkinData();
        }
        
        for (size_t i = 0, l = skinDataList.size(); i < l; ++i)
        {
            if (skinDataList[i]->name == skinName)
            {
                return skinDataList[i];
            }
        }
        
        return nullptr;
    }

	void setSkinData(const std::string& skinName)
	{
		for (int i = 0, len = slotDataList.size(); i < len; i++)
		{
			slotDataList[i]->dispose();
		}
		SkinData *skinData;
		if (skinName.empty() && skinDataList.size() > 0)
		{
			skinData = skinDataList[0];
		}
		else
		{
			for (int i = 0, len = skinDataList.size(); i < len; i++)
			{
				if (skinDataList[i]->name == skinName)
				{
					skinData = skinDataList[i];
					break;
				}
			}
		}

		if (skinData)
		{
			SlotData *slotData;
			for (int i = 0, len = skinData->slotDataList.size(); i < len; i++)
			{
				slotData = getSlotData(skinData->slotDataList[i]->name);
				if (slotData)
				{
					for (int j = 0, jLen = skinData->slotDataList[i]->displayDataList.size(); j < jLen; j++)
					{
						slotData->displayDataList.push_back(skinData->slotDataList[i]->displayDataList[j]);
					}
				}
			}
		}
	}

	SlotData* getSlotData(const std::string& slotName)
	{
		if (slotName.empty() && slotDataList.size() > 0)
		{
			return slotDataList[0];
		}
		int i = slotDataList.size();
		while (i --)
		{
			if (slotDataList[i]->name == slotName)
			{
				return slotDataList[i];
			}
		}
		return nullptr;
	}
    
    AnimationData* getAnimationData(const std::string &animationName) const
    {
        for (size_t i = 0, l = animationDataList.size(); i < l; ++i)
        {
            if (animationDataList[i]->name == animationName)
            {
                return animationDataList[i];
            }
        }
        
        return nullptr;
    }
    
    void sortBoneDataList()
    {
        if (boneDataList.empty())
        {
            return;
        }
        
        std::vector<std::pair<int , BoneData*>> sortedList;
        for (size_t i = 0, l = boneDataList.size(); i < l; ++i)
        {
            BoneData *boneData = boneDataList[i];
            BoneData *parentData = boneData;
            int level = 0;
            
            while (parentData)
            {
                parentData = getBoneData(parentData->parent);
                level ++;
            }
            sortedList.push_back(std::make_pair(level , boneData));
        }
        
        std::sort(sortedList.begin() , sortedList.end() , sortBone);        
        for (size_t i = 0, l = sortedList.size(); i < l; ++i)
        {
            boneDataList[i] = sortedList[i].second;
        }
    }

private:
	static bool sortBone(const std::pair<int, BoneData*> &a, const std::pair<int, BoneData*> &b)
	{
		return a.first < b.first;
	}

public:
	std::string name;
	std::vector<BoneData*> boneDataList;
	std::vector<SkinData*> skinDataList;
	std::vector<SlotData*> slotDataList;
	std::vector<AnimationData*> animationDataList;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_OBJECTS_ARMATURE_DATA_H
