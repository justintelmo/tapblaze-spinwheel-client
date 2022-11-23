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
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
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
const int WHEEL_SPIN_ANIMATION_TAG = 3;

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

	_btnPlay = Button::create("spin_button.png", "", "", Widget::TextureResType::PLIST);
	this->addChild(_btnPlay);
	_btnPlay->setScale9Enabled(false);
	_btnPlay->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_btnPlay->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.1f));
	_btnPlay->addTouchEventListener(CC_CALLBACK_2(MainScene::onTouchBtnPlay, this));
	_btnPlay->setPressedActionEnabled(false);

	Text* txtPlay = Text::create("Play More", "fonts/arial.ttf", 15);
	_btnPlay->addChild(txtPlay);
	txtPlay->setColor(Color3B::WHITE);
	txtPlay->enableOutline(Color4B::BLACK, 2);
	txtPlay->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	txtPlay->setPosition(_btnPlay->getContentSize() * 0.5f);

	_btnClaim = Button::create("spin_button.png", "", "", Widget::TextureResType::PLIST);
	this->addChild(_btnClaim);
	_btnClaim->setVisible(false);
	_btnClaim->setScale9Enabled(false);
	_btnClaim->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_btnClaim->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.1f));
	_btnClaim->addTouchEventListener(CC_CALLBACK_2(MainScene::onTouchBtnClaim, this));
	_btnClaim->setPressedActionEnabled(false);

	Text* txtClaim = Text::create("Claim", "fonts/arial.ttf", 35);
	_btnClaim->addChild(txtClaim);
	txtClaim->setColor(Color3B::WHITE);
	txtClaim->enableOutline(Color4B::BLACK, 1);
	txtClaim->setTextHorizontalAlignment(TextHAlignment::CENTER);
	txtClaim->setPosition(_btnClaim->getContentSize() * 0.5f);

	_wheel = Wheel::create();
	this->addChild(_wheel);
	_wheel->setName("wheel");
	_wheel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_wheel->setScale(1.15f);
	_wheel->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f + 25.0f));

	ItemData* data = new ItemData();
	HttpRequest* request = new (std::nothrow) HttpRequest();
	request->setUrl("http://justintelmo-tapblaze-spinwheel.herokuapp.com/api/wheel_items");
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(ItemData::getItemDataFromServer);
	HttpClient::getInstance()->send(request);
	request->release();

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
	MainScene* mainScene = dynamic_cast<MainScene*>(scene);
	rapidjson::SizeType responseIdx = 0;
	int spinResult = responseData[responseIdx]["result"].GetInt();
	mainScene->_lastRewardItemIndex = spinResult;
	

	Wheel* targetWheel = dynamic_cast<Wheel*>(mainScene->getChildByName("wheel"));
	Node* wheelSection = targetWheel->getChildByName("wheel_section");
	WheelItem* targetWheelItem = dynamic_cast<WheelItem*>(wheelSection->getChildByName("Wheel_Item_" + std::to_string(spinResult)));
	float targetItemRotation = targetWheelItem->getRotation();
	float rotationDuration = RandomHelper::random_real(2.0f, 2.5f);
	RotateBy* rotateBy = RotateBy::create(rotationDuration, 720.0f);
	RotateTo* rotateTo = RotateTo::create(rotationDuration, (-ROTATION_PER_ITEM_SLOTS * (spinResult + 0.5f) + 180.0f));
	//float compensationAngle = (360.0f * 1.5f) - (WHEEL_SECTION_ROTATION_OFFSET * spinResult + ROTATION_PER_ITEM_SLOTS);
	//RotateBy* rotateToTarget = RotateBy::create(RandomHelper::random_real(2.0f, 2.5f), compensationAngle);
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

void MainScene::onClickBtnPlay()
{
	_btnPlay->setTouchEnabled(false);
	if (_wheel)
	{
		HttpRequest* request = new (std::nothrow) HttpRequest();
		std::string numSpinsString = "spins=" + std::to_string(1);
		request->setUrl("http://justintelmo-tapblaze-spinwheel.herokuapp.com/api/spins");
		request->setRequestType(HttpRequest::Type::POST);
		request->setRequestData(numSpinsString.c_str(), numSpinsString.size());
		request->setResponseCallback(MainScene::getSpinResult);
		HttpClient::getInstance()->send(request);
		request->release();
		// Start spinning animation until response comes back from server
		_wheel->playSpinAnimation(CC_CALLBACK_0(MainScene::onEndWheelSpinAnimation, this));
	}
}

void MainScene::onTouchBtnPlay(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		_btnPlay->setScale(1.05f);
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		_btnPlay->setScale(1.0f);
		ScaleTo* scaleTo = ScaleTo::create(0.3f, 0.0f);
		_btnPlay->runAction(EaseIn::create(scaleTo, 0.7f));
		onClickBtnPlay();
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		_btnPlay->setScale(1.0f);
		break;
	}
}

void MainScene::onTouchBtnClaim(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		_btnClaim->setScale(1.05f);
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		_btnClaim->setScale(1.0f);
		ScaleTo* scaleTo = ScaleTo::create(0.3f, 0.0f);
		_btnClaim->runAction(EaseIn::create(scaleTo, 0.7f));

		onClickBtnClaim();
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		_btnClaim->setScale(1.0f);
		break;
	}
}

void MainScene::onClickBtnClaim()
{
	_wheel->runAction(EaseBackOut::create(ScaleTo::create(0.25f, 1.0f)));
	_wheel->resetPosition();
	_btnPlay->runAction(EaseBackOut::create(ScaleTo::create(0.25f, 1.0f)));
	_btnPlay->setTouchEnabled(true);
	_btnClaim->setTouchEnabled(false);

	Node* childTarget = this->getChildByTag(TAG_REWARD_ITEM);
	if (childTarget) 
	{
		childTarget->removeFromParent();
		childTarget->release();
	}
}


int MainScene::getRandomItemReward()
{
	// TODO:: Replace with call to server so that we can fake.. animation?
	int sumOfWeight = _wheel->getSumOfWeight();
	int random = cocos2d::RandomHelper::random_int(1, sumOfWeight);
	for (unsigned int i = 0; i < _wheel->getItemData().size(); i++) {
		if (random < _wheel->getItemData().at(i)->weight) {
			return i;
		}
		random -= _wheel->getItemData().at(i)->weight;
	}

	return 0;
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
		item->setPosition(Vec2(winSize.width * 0.5f, winSize.height));
		float effectDuration = 1.2f;
		MoveTo* moveTo = MoveTo::create(effectDuration, winSize * 0.5f);
		item->setScale(0.2f);
		ScaleTo* scaleTo = ScaleTo::create(effectDuration, 1.3f);
		item->runAction(Sequence::create(DelayTime::create(scaleSmallDuration), Spawn::create(EaseBackOut::create(moveTo), EaseBackOut::create(scaleTo), nullptr), nullptr));
	}

	_btnClaim->setVisible(true);
	_btnClaim->setTouchEnabled(true);

	_btnClaim->setScale(0.0f);
	_btnClaim->runAction(EaseBackOut::create(ScaleTo::create(0.25f, 1.0f)));
}

int MainScene::getSumOfWeight()
{
	return _sumOfWeight;
}

Wheel* MainScene::getCurrentWheel()
{
	return _wheel;
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
	for (int i = 0; i < _wheel->getItemData().size(); ++i)
	{
		resultFrequency.push_back(0);
	}

	rapidjson::Value& responseData = jsonDoc["data"];
	assert(responseData.IsArray());
	std::vector<ItemData*> wheelItems = _wheel->getItemData();
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
		int itemType = static_cast<int>(item->type);
		results += cocos2d::StringUtils::format("Turn %d, Prize = %s\n", currentTurn, getItemDescription(itemType));
		resultFrequency.at(itemType)++;
	}

	int itemIndex = 0;
	for (int itemFrequency : resultFrequency)
	{
		results += cocos2d::StringUtils::format("Prize %d occurred %d times.\n", itemIndex, itemFrequency);
		itemIndex++;
	}

	// Write output to C:/testoutput.txt
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);

	std::ofstream ofs(current_working_dir + "/testoutput.txt", std::ofstream::binary);
	if (ofs.fail() == false)
	{
		ofs.write(results.c_str(), results.length());
		ofs.close();
	}
}

void MainScene::testResult()
{
	int numTestSpins = 1000;
	HttpRequest* request = new (std::nothrow) HttpRequest();
	std::string numSpinsString = "spins=" + std::to_string(numTestSpins);
	request->setUrl("http://justintelmo-tapblaze-spinwheel.herokuapp.com/api/spins");
	request->setRequestType(HttpRequest::Type::POST);
	request->setRequestData(numSpinsString.c_str(), numSpinsString.size());
	request->setResponseCallback(CC_CALLBACK_2(MainScene::onTestRequestCompleted, this));

	HttpClient::getInstance()->send(request);
	request->release();
}

const char* MainScene::getItemDescription(int index)
{
	switch (index)
	{
		case 0: return "30 Min. Unlimited Lives";
		case 1: return "3x Brush";
		case 2: return "35x Gem";
		case 3: return "3x Hammer";
		case 4: return "750x Coin";
		case 5: return "1x Brush";
		case 6: return "75x Gem";
		case 7: return "1x Hammer";
		default:
			break;
	}

	return "";
}
#endif