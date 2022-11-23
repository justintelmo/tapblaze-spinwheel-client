/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "AppDelegate.h"
#include "MainScene.h"
// #define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#endif

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1080, 1920);
static cocos2d::Size smallResolutionSize = cocos2d::Size(384, 512);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(768, 1024);
static cocos2d::Size largeResolutionSize = cocos2d::Size(1080, 1920);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}

void AppDelegate::initMultiResolution()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();

    glview->setDesignResolutionSize(
        designResolutionSize.width,
        designResolutionSize.height,
        ResolutionPolicy::NO_BORDER
    );

    std::vector<std::string> searchPaths;
    float scaleFactor = 1.0f;
    Size frameSize = glview->getFrameSize();

    if (frameSize.height > mediumResolutionSize.height)
    {
        searchPaths.push_back("sprites/HD");
        scaleFactor = largeResolutionSize.height / designResolutionSize.height;
    }
    else if (frameSize.height > smallResolutionSize.height)
    {
        searchPaths.push_back("sprites/HDR");
        scaleFactor = mediumResolutionSize.height / designResolutionSize.height;
    }
    else
    {
        searchPaths.push_back("sprites/SD");
        scaleFactor = smallResolutionSize.height / designResolutionSize.height;
    }

    director->setContentScaleFactor(scaleFactor);
    FileUtils::getInstance()->setSearchPaths(searchPaths);
}

void AppDelegate::initOpenGL()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();

    if (!glview)
    {
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("TapBlaze Wheel Test",
            Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
        #else
        glview = GLViewImpl::createWithRect("TapBlaze Wheel Test", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
        #endif
        director->setOpenGLView(glview);
    }
}

void AppDelegate::initDirector()
{
    auto director = Director::getInstance();
    director->setAnimationInterval(1.0f / 60.0f);
    director->setDisplayStats(true);
}

void AppDelegate::createAndRunScene()
{
    auto scene = MainScene::createScene();
    Director::getInstance()->runWithScene(scene);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    initOpenGL();
    initDirector();
    initMultiResolution();
    createAndRunScene();
    return true;
}