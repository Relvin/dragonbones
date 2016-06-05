//
//  DBSlot.hpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBSlot_hpp
#define DBSlot_hpp

#include "DragonBones.h"
#include "2d/CCNode.h"
#include "objects/SlotData.h"
#include "renderer/DBSkin.h"
#include "core/DBBase.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class DBSlotTimelineState;
class DBAnimationState;
class DBArmature;
class DBSkin;
class MeshData;
class DBSlot
: public cocos2d::Node
, public DBBase
{
public:
    static DBSlot* create(SlotData* slotData,const std::string & textureName);
    bool init();
    bool initWithSlotData(SlotData* slotData,const std::string & textureName);
    
    DBSlot();
    virtual ~DBSlot();
    
    virtual void update(float delta);
    void updateDisplayTransform();
    
    void updateGlobal(Transform &transform, Matrix &matrix);
    void calculateRelativeParentTransform();
    
    void arriveAtFrame( Frame *frame, const DBSlotTimelineState *timelineState, DBAnimationState *animationState, bool isCross );
    
    void changeDisplay(int displayIndex);
    void updateDisplayVisible(bool visible);
    
    void updateDisplayColor(int aOffset, int rOffset, int gOffset, int bOffset, float aMultiplier, float rMultiplier, float gMultiplier, float bMultiplier, bool colorChanged);
    bool isColorChanged() const { return _isColorChanged; }
    void updateSkinDisplay();
    
    void setTweenZOrder(float zOrder);
    
    MeshData* getMeshData(const std::string& name);
    
    void resetToOrigin();
    
private:
    DBSkin* _display;
    SlotData* _slotData;
    bool _needUpdate;
    float _tweenZOrder;
    ColorTransform _colorTransform;
    bool _isColorChanged;
    int _displayIndex;
    float _originZOrder;
    DBArmature* _childArmature;
    std::string _textureAtlasName;
    int _originDisplayIndex ;
    
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBSlot_h */
