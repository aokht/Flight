//
//  SelectScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/25.
//
//

#ifndef __Flight__SelectScene__
#define __Flight__SelectScene__

#include "cocos2d.h"
#include "cocosGUI.h"
#include "Field.h"
#include "Airplane.h"

class SelectScene : public cocos2d::Layer
{
public:
    CREATE_FUNC(SelectScene);
    static cocos2d::Scene* createScene();

private:
    cocos2d::Node* rootNode;
    cocos2d::Node* stageNode;
    cocos2d::Node* airplaneNode;
    cocos2d::ui::Text* stageNameLabel;
    cocos2d::ui::Text* airplaneNameLabel;
    cocos2d::ui::Button* nextButton;
    cocos2d::ui::Button* backButton;
    cocos2d::ui::Button* stageLeftButton;
    cocos2d::ui::Button* stageRightButton;
    cocos2d::ui::Button* airplaneLeftButton;
    cocos2d::ui::Button* airplaneRightButton;

    bool fieldLoading;
    std::vector<int> fieldIdList;
    cocos2d::Map<int, Field*> fieldList;
    int currentField;

    bool airplaneLoading;
    std::vector<int> airplaneIdList;
    cocos2d::Map<int, Airplane*> airplaneList;
    int currentAirplane;

    bool init();
    void onEnter() override;
    void setupUI();
    void grabElements();
    void loadStages();
    void showStage(int index);
    void loadAirplanes();
    void showAirplane(int index);
    bool canSelectStage() const;
    void checkNextButtonEnable();
};

#endif /* defined(__Flight__SelectScene__) */
