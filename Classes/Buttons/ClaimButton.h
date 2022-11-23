//
// Created by psyba on 11/23/2022.
//

#ifndef PROJ_ANDROID_CLAIMBUTTON_H
#define PROJ_ANDROID_CLAIMBUTTON_H

#include <string>
#include <ui/CocosGUI.h>

using namespace cocos2d;
using namespace cocos2d::ui;

class ClaimButton : public Button {
public:
    ~ClaimButton();
    static ClaimButton* create(const std::string& imageName, const std::string& buttonText);

private:
    std::string _buttonText;
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
    void onTouchCancelled(Touch* touch, Event* event);
    void onClickBtn();
    void setConfiguration();
    void setupEventListeners();
};


#endif //PROJ_ANDROID_CLAIMBUTTON_H
