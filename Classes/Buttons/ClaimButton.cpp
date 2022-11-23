//
// Created by psyba on 11/23/2022.
//

#include "ClaimButton.h"
#include "cocos2d.h"
#include <string>
#include <MainScene.h>

using namespace cocos2d;

const int CLAIM_BUTTON_TAG = 5;
ClaimButton::~ClaimButton()
= default;

ClaimButton* ClaimButton::create(const std::string& imageName, const std::string& buttonText)
{
    auto* claimBtn = new (std::nothrow) ClaimButton();
    if (claimBtn && claimBtn->init(imageName, "", "", Widget::TextureResType::PLIST)) {
        claimBtn->_buttonText = buttonText;
        claimBtn->setConfiguration();
        claimBtn->setupEventListeners();
        claimBtn->autorelease();
        return claimBtn;
    }

    CC_SAFE_DELETE(claimBtn);
    return nullptr;
}

void ClaimButton::setConfiguration()
{
    Size winSize = Director::getInstance()->getWinSize();
    this->setVisible(false);
    this->setScale9Enabled(false);
    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    this->setPosition(Vec2(winSize.width * 0.5f, winSize.height * 0.1f));
    this->setPressedActionEnabled(false);
    this->setTag(CLAIM_BUTTON_TAG);

    Text* txtClaim = Text::create(_buttonText, "fonts/arial.ttf", 60);
    this->addChild(txtClaim);
    txtClaim->setColor(Color3B::WHITE);
    txtClaim->enableOutline(Color4B::BLACK, 1);
    txtClaim->setTextHorizontalAlignment(TextHAlignment::CENTER);
    txtClaim->setPosition(this->getContentSize() * 0.5f);
}

void ClaimButton::setupEventListeners()
{

}

bool ClaimButton::onTouchBegan(Touch* touch, Event* event)
{
    event->getCurrentTarget()->setScale(1.05f);
    return true;
}

void ClaimButton::onTouchMoved(Touch* touch, Event* event)
{
    event->getCurrentTarget()->setScale(1.05f);
}

void ClaimButton::onTouchEnded(Touch* touch, Event* event)
{
    event->getCurrentTarget()->setScale(1.0f);
    ScaleTo *scaleTo = ScaleTo::create(0.3f, 0.0f);
    event->getCurrentTarget()->runAction(EaseIn::create(scaleTo, 0.7f));

    onClickBtn();
}

void ClaimButton::onTouchCancelled(Touch* touch, Event* event)
{
    event->getCurrentTarget()->setScale(1.0f);
}

void ClaimButton::onClickBtn()
{
    auto* director = Director::getInstance();
    Scene* scene = director->getRunningScene();
    MainScene* mainScene = dynamic_cast<MainScene*>(scene);
    mainScene->onClickBtnClaim();
}


