#include "HelloWorldScene.h"
#include "renderer/DBCCFactory.h"

#include "cocostudio/CocoStudio.h"

#include "renderer/DBArmature.h"
#include "events/DBEventData.h"

USING_NS_CC;

//using namespace dragonBones;

const std::string actArr[] = {"walk","fall","stand","jump"};

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "res/CloseNormal.png",
                                           "res/CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
//    auto label = Label::createWithTTF("Hello World", "res/fonts/Marker Felt.ttf", 24);
//    
//    // position the label on the center of the screen
//    label->setPosition(Vec2(origin.x + visibleSize.width/2,
//                            origin.y + visibleSize.height - label->getContentSize().height));
//
//    // add the label as a child to this layer
//    this->addChild(label, 1);
//
    // add "HelloWorld" splash screen"
//    auto sprite = Sprite::create("res/HelloWorld.png");
//
//    // position the sprite on the center of the screen
//    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
//
//    // add the sprite as a child to this layer
//    this->addChild(sprite, 0);
    
    
    
//
//    this->addChild(armNode);
    
    
//    DBCCFactory::getInstance()->loadDragonBonesData("res/Dragon.xml","Dragon");
//    DBCCFactory::getInstance()->loadTextureAtlas("res/texture.xml","Dragon");
//    DBCCArmatureNode* armNode = DBCCFactory::getInstance()->buildArmatureNode("Dragon","Dragon");
//
    
//    this->addDragonBones();
    this->addUbbie();
    
    return true;
}


void HelloWorld::addUbbie()
{
    Size winSize = Director::getInstance()->getWinSize();
    dragonBones::DBCCFactory::getInstance()->loadDragonBonesData("res/Ubbie/Ubbie.xml","Ubbie");
    dragonBones::DBCCFactory::getInstance()->loadTextureAtlas("res/Ubbie/texture.xml","Ubbie");
    dragonBones::DBArmature* armature = dragonBones::DBArmature::create("Ubbie");
    this->addChild(armature);
    armature->setScale(0.5);
    armature->setPosition(winSize / 2);
    armature->getAnimation()->gotoAndPlay("turn face");
//    armature->setSpeed(0.2);
    armature->setName("Ubbie");
    armature->setEventCallback(CC_CALLBACK_1(HelloWorld::animationEventCallback,this));
    
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void HelloWorld::animationEventCallback(const dragonBones::DBEventData* eventData)
{
    CCLOG("animationEventCallback ---> %s",eventData->getStringType().c_str());
}



