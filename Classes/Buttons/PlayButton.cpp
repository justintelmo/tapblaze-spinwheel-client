//
// Created by psyba on 11/23/2022.
//

#include <MainScene.h>
#include "../Wheel.h"
#include "PlayButton.h"
#include "cocos/ui/CocosGUI.h"

const int PLAY_BUTTON_TAG = 4;

using namespace cocos2d::ui;
PlayButton::~PlayButton()
= default;

PlayButton* PlayButton::create(const std::string& imageName, const std::string& buttonText)
{
    auto* playBtn = new (std::nothrow) PlayButton();
    if (playBtn && playBtn->init(imageName, "", "", Widget::TextureResType::PLIST)) {
        playBtn->_buttonText = buttonText;
        playBtn->setConfiguration();
        playBtn->setupEventListener();
        playBtn->autorelease();
        return playBtn;
    }

    CC_SAFE_DELETE(playBtn);
    return nullptr;
}

void PlayButton::setConfiguration()
{
    Size winSize = Director::getInstance()->getWinSize();
    this->setScale9Enabled(false);
    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    this->setPosition(Vec2(winSize.width * 0.5f, winSize.height * 0.1f));
    this->setPressedActionEnabled(false);
    this->setTag(PLAY_BUTTON_TAG);

    _textField = Text::create(_buttonText, "fonts/arial.ttf", 60);
    this->addChild(_textField);
    _textField->setColor(Color3B::WHITE);
    _textField->enableOutline(Color4B::BLACK, 2);
    _textField->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _textField->setPosition(this->getContentSize() * 0.5f);
}

bool PlayButton::onTouchBegan(Touch* touch, Event *event)
{
    event->getCurrentTarget()->setScale(1.05f);
    return true;
}

void PlayButton::onTouchMoved(Touch* touch, Event* event)
{
    event->getCurrentTarget()->setScale(1.05f);
}

void PlayButton::onTouchEnded(Touch* touch, Event* event)
{
    event->getCurrentTarget()->setScale(1.0f);
    ScaleTo *scaleTo = ScaleTo::create(0.3f, 0.0f);
    event->getCurrentTarget()->runAction(EaseIn::create(scaleTo, 0.7f));

    onClickBtn();
}

void PlayButton::onTouchCancelled(Touch* touch, Event* event)
{
    event->getCurrentTarget()->setScale(1.0f);
}

void PlayButton::setupEventListener()
{
    auto btnListener = EventListenerTouchOneByOne::create();
    btnListener->setSwallowTouches(false);
    btnListener->onTouchBegan = CC_CALLBACK_2(PlayButton::onTouchBegan, this);
    btnListener->onTouchMoved = CC_CALLBACK_2(PlayButton::onTouchMoved, this);
    btnListener->onTouchEnded = CC_CALLBACK_2(PlayButton::onTouchEnded, this);
    btnListener->onTouchCancelled = CC_CALLBACK_2(PlayButton::onTouchCancelled, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(btnListener, this);
}

void PlayButton::onClickBtn()
{
    this->setTouchEnabled(false);
    Director* director = Director::getInstance();
    Scene* scene = director->getRunningScene();
    MainScene* mainScene = dynamic_cast<MainScene*>(scene);
    Wheel* wheel = scene->getChildByName<Wheel*>("wheel");
    if (wheel)
    {
        if (wheel->getIsSpinning())
        {
            return;
        }

        HttpRequest* request = new (std::nothrow) HttpRequest();
        std::string numSpinsString = "spins=" + std::to_string(1);
        request->setUrl("http://justintelmo-tapblaze-spinwheel.herokuapp.com/api/spins");
        request->setRequestType(HttpRequest::Type::POST);
        request->setRequestData(numSpinsString.c_str(), numSpinsString.size());
        request->setResponseCallback(MainScene::getSpinResult);
        HttpClient::getInstance()->send(request);
        request->release();
        // Start spinning animation until response comes back from server
        wheel->playSpinAnimation(CC_CALLBACK_0(MainScene::onEndWheelSpinAnimation, mainScene));
    }
}