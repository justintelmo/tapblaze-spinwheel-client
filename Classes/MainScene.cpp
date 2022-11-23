#include "MainScene.h"
#include "AudioEngine.h"
#include "cocos/ui/CocosGUI.h"
#include "Wheel.h"
#include "RewardItem.h"
#include "ItemData.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpClient.h"
#include <map>
#include <fstream>
#include <iostream>
#include "spine/Json.h"
#include "CCDirector.h"
#include "json/rapidjson.h"
#include "external/json/document.h"
#include "WheelItem.h"
#include "json/pointer.h"
#include <filesystem>
#include "Buttons/PlayButton.h"
#include "Buttons/ClaimButton.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <direct.h>
#define GetCurrentDir _getcwd
#endif

USING_NS_CC;

#define WINDOWS 1
#define TEST_MODE 1
#define TAG_REWARD_ITEM 1
using namespace cocos2d::ui;
using namespace cocos2d::network;

const float WHEEL_SECTION_ROTATION_OFFSET = -22.5f;
const float ROTATION_PER_ITEM_SLOTS = 45.0f;
const float WHEEL_SCALE = 1.15f;
const float WHEEL_OFFSET_HEIGHT = 25.0f;
const int WHEEL_SPIN_ANIMATION_TAG = 3;
const int PLAY_BUTTON_TAG = 4;
const int CLAIM_BUTTON_TAG = 5;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

bool MainScene::init()
{

    if (!Scene::init())
    {
        return false;
    }

    SpriteFrameCache* instance = SpriteFrameCache::getInstance();
    instance->addSpriteFramesWithFile("ui_elements.plist");

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    Size winSize = Director::getInstance()->getWinSize();

    sendItemDataRequest();
    setupButtons();
    setupWheel();

    EventListenerKeyboard* listener = EventListenerKeyboard::create();
    listener->onKeyPressed = [=](cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* e)
    {
#if TEST_MODE
        if (keycode == EventKeyboard::KeyCode::KEY_T)
        {

            this->testResult();
        }
#endif
    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void MainScene::sendItemDataRequest()
{
    HttpRequest* request = new (std::nothrow) HttpRequest();
    request->setUrl("http://justintelmo-tapblaze-spinwheel.herokuapp.com/api/wheel_items");
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(ItemData::getItemDataFromServer);
    HttpClient::getInstance()->send(request);
    request->release();
}

void MainScene::setupButtons()
{
    PlayButton* btnPlay = PlayButton::create("spin_button.png", "Play On");
    this->addChild(btnPlay);

    ClaimButton* btnClaim = ClaimButton::create("spin_button.png", "Claim");
    this->addChild(btnClaim);
}

void MainScene::setupWheel()
{
    Size winSize = Director::getInstance()->getWinSize();
    _wheel = Wheel::create();
    this->addChild(_wheel);
    _wheel->setName("wheel");
    _wheel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _wheel->setScale(WHEEL_SCALE);
    _wheel->setPosition(Vec2(winSize.width * 0.5f, winSize.height * 0.5f + WHEEL_OFFSET_HEIGHT));
}

void MainScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void MainScene::getSpinResult(HttpClient* request, HttpResponse* response)
{
    // Get spin result from server, stop current animation and animate from current to where spin result is
    if (!response)
    {
        return;
    }

    std::vector<char>* buffer = response->getResponseData();
    std::string responseBody(buffer->begin(), buffer->end());
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(responseBody.c_str());
    const rapidjson::Value& responseData = jsonDoc["data"];
    assert(responseData.IsArray());
    Director* director = Director::getInstance();
    Scene* scene = director->getRunningScene();
    auto* mainScene = dynamic_cast<MainScene*>(scene);
    rapidjson::SizeType responseIdx = 0;
    int spinResult = responseData[responseIdx]["result"].GetInt();
    mainScene->setLastRewardItemIndex(spinResult);


    Wheel* targetWheel = dynamic_cast<Wheel*>(mainScene->getChildByName("wheel"));
    Node* wheelSection = targetWheel->getChildByName("wheel_section");
    float rotationDuration = RandomHelper::random_real(2.0f, 2.5f);
    RotateBy* rotateBy = RotateBy::create(rotationDuration, 720.0f);
    RotateTo* rotateTo = RotateTo::create(rotationDuration, (-ROTATION_PER_ITEM_SLOTS * ((float) spinResult + 0.5f) + 180.0f));
    EaseOut* easeOutRotateTo = EaseOut::create(rotateTo, rotationDuration);
    DelayTime* briefPause = DelayTime::create(0.5f);
    wheelSection->stopActionByTag(WHEEL_SPIN_ANIMATION_TAG);
    wheelSection->runAction(Sequence::create(rotateBy, easeOutRotateTo, briefPause, CallFunc::create([=]() {
        if (targetWheel->getCallbackEndSpinAnimation())
        {
            targetWheel->fireCallbackEndSpinAnimation();
        }
    }), nullptr));
}

void MainScene::setLastRewardItemIndex(int lastRewardItemIndex)
{
    this->_lastRewardItemIndex = lastRewardItemIndex;
}


MainScene* MainScene::getCurrentScene()
{
    auto* director = Director::getInstance();
    Scene* scene = director->getRunningScene();
    return dynamic_cast<MainScene*>(scene);
}

PlayButton* MainScene::getPlayButton()
{
    MainScene* mainScene = getCurrentScene();
    return dynamic_cast<PlayButton*>(mainScene->getChildByTag(PLAY_BUTTON_TAG));
}

ClaimButton* MainScene::getClaimButton()
{
    MainScene* mainScene = getCurrentScene();
    return dynamic_cast<ClaimButton*>(mainScene->getChildByTag(CLAIM_BUTTON_TAG));
}

void MainScene::onClickBtnClaim()
{
    if (_wheel->getIsSpinning())
    {
        return;
    }

    PlayButton* playButton = getPlayButton();
    ClaimButton* claimButton = getClaimButton();
    _wheel->runAction(EaseBackOut::create(ScaleTo::create(0.25f, 1.0f)));
    _wheel->resetPosition();
    playButton->runAction(EaseBackOut::create(ScaleTo::create(0.25f, 1.0f)));
    playButton->setTouchEnabled(true);
    claimButton->setTouchEnabled(false);

    Node* childTarget = this->getChildByTag(TAG_REWARD_ITEM);
    if (childTarget)
    {
        childTarget->removeFromParent();
        childTarget->release();
    }
}

void MainScene::onEndWheelSpinAnimation()
{
    float scaleSmallDuration = 0.2f;
    _wheel->runAction(ScaleTo::create(scaleSmallDuration, 0.0f));

    if (_lastRewardItemIndex < _wheel->getItemData().size())
    {
        ItemData* data = _wheel->getItemData().at(_lastRewardItemIndex);
        RewardItem* item = RewardItem::createWith(data->type, data->value);
        this->addChild(item);
        item->setTag(TAG_REWARD_ITEM);
        item->retain();
        Size winSize = Director::getInstance()->getWinSize();
        item->setPosition(Vec2(winSize.width * 0.5f, winSize.height * 0.5f));
        float effectDuration = 1.2f;
        MoveTo* moveTo = MoveTo::create(effectDuration, winSize * 0.5f);
        item->setScale(0.2f);
        ScaleTo* scaleTo = ScaleTo::create(effectDuration, 1.3f);
        item->runAction(Sequence::create(DelayTime::create(scaleSmallDuration), Spawn::create(EaseBackOut::create(moveTo), EaseBackOut::create(scaleTo), nullptr), nullptr));
    }

    ClaimButton* claimButton = getClaimButton();
    claimButton->setVisible(true);
    claimButton->setTouchEnabled(true);

    claimButton->setScale(0.0f);
    claimButton->runAction(EaseBackOut::create(ScaleTo::create(0.25f, 1.0f)));

    _wheel->setIsSpinning(false);
}

Wheel* MainScene::getCurrentWheel()
{
    return _wheel;
}

Size MainScene::getWinSize()
{
    return Director::getInstance()->getWinSize();
}

#if TEST_MODE

void MainScene::onTestRequestCompleted(HttpClient *request, HttpResponse *response)
{
    if (!response)
    {
        return;
    }

    // Create vector to store results in
    std::vector<ItemData*> testResults;
    std::string results;
    std::vector<int> resultFrequency;

    std::vector<char>* buffer = response->getResponseData();
    std::string responseBody(buffer->begin(), buffer->end());
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(responseBody.c_str());

    // Create vector of size n where n = number of total prizes
    for (unsigned int i = 0; i < getCurrentWheel()->getItemData().size(); ++i)
    {
        resultFrequency.push_back(0);
    }

    rapidjson::Value& responseData = jsonDoc["data"];
    assert(responseData.IsArray());
    std::vector<ItemData*> wheelItems = getCurrentWheel()->getItemData();
    for (rapidjson::SizeType i = 0; i < responseData.Size(); i++)
    {
        ItemData* testResult = new ItemData;
        int id = responseData[i]["result"].GetInt();
        ItemData* currentResult = wheelItems.at(id);

        testResult->id = currentResult->id;
        testResult->value = currentResult->value;
        testResult->type = (ItemType)currentResult->type;
        testResult->weight = currentResult->weight;

        testResults.push_back(testResult);
    }

    int currentTurn = 0;
    // Format string output based on results
    for (ItemData* item : testResults)
    {
        currentTurn++;
        int itemType = static_cast<int>(item->id);
        int value = item->value;
        results += cocos2d::StringUtils::format("Turn %d, Prize = %s\n", currentTurn, getItemDescription(itemType, value));
        resultFrequency.at(itemType)++;
    }

    int itemIndex = 0;
    for (int itemFrequency : resultFrequency)
    {
        results += cocos2d::StringUtils::format("Prize %d occurred %d times.\n", itemIndex, itemFrequency);
        itemIndex++;
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    char buffer[256];

    strcpy(buffer, getenv("HOME"));
    strcat(buffer, "/testoutput.txt");
    std::string current_working_dir(buffer);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    std::string current_working_dir = "/data/data";
#endif

    std::ofstream ofs(current_working_dir + "/testoutput.txt", std::ofstream::binary);
    if (!ofs.fail())
    {
        ofs.write(results.c_str(), results.length());
        ofs.close();
    }
}

void MainScene::testResult()
{
    int numTestSpins = 1000;
    auto request = new (std::nothrow) HttpRequest();
    std::string numSpinsString = "spins=" + std::to_string(numTestSpins);
    request->setUrl("http://justintelmo-tapblaze-spinwheel.herokuapp.com/api/spins");
    request->setRequestType(HttpRequest::Type::POST);
    request->setRequestData(numSpinsString.c_str(), numSpinsString.size());
    request->setResponseCallback(CC_CALLBACK_2(MainScene::onTestRequestCompleted, this));

    HttpClient::getInstance()->send(request);
    request->release();
}

const char* MainScene::getItemDescription(int index, int value)
{
    std::string valueString = std::to_string(value);
    std::string typeString = "";
    switch (index)
    {
        case 0:
            typeString = " Min. Unlimited Lives";
            break;
        case 1:
        case 5:
            typeString = "x Brush";
            break;
        case 2:
        case 6:
            typeString = "x Gem";
            break;
        case 3:
        case 7:
            typeString = "x Hammer";
            break;
        case 4:
            typeString = "x Coin";
            break;
        default:
            typeString = "x Unrecognized Type";
            break;
    }

    std::string resultString = valueString + typeString;
    char result [1024];
    strcpy(result, resultString.c_str());
    return result;
}
#endif