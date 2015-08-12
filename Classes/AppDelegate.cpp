#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "Global.h"

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("Flight");
        director->setOpenGLView(glview);
    }

    director->getOpenGLView()->setDesignResolutionSize(1334, 750, ResolutionPolicy::FIXED_WIDTH);

    // turn on display FPS
#if COCOS2D_DEBUG
    director->setDisplayStats(true);
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    register_all_packages();

    FileUtils::getInstance()->addSearchPath("res");
    FileUtils::getInstance()->addSearchPath("plists");
    FileUtils::getInstance()->addSearchPath("scenes");
    FileUtils::getInstance()->addSearchPath("fonts");
    FileUtils::getInstance()->addSearchPath("BGM");
    FileUtils::getInstance()->addSearchPath("SE");

    SimpleAudioEngine* audio = SimpleAudioEngine::getInstance();
    for (int i = 0; i < BGM_LAST; ++i) {
        audio->preloadBackgroundMusic(BGM_LIST[i]);
    }
    for (int i = 0; i < SE_LAST; ++i) {
        audio->preloadEffect(SE_LIST[i]);
    }

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
