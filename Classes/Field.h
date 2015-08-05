//
//  Field.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/17.
//
//

#ifndef __Flight__Field__
#define __Flight__Field__

#include "cocos2d.h"
#include "ExSprite3D.h"
#include "FieldDataSource.h"
#include "Sprite3DBatchNode.h"
#include "SceneData.h"

class Airplane;

class Field : public ExSprite3D
{
public:
    // Life Cycle
    static Field* createById(int i, bool collisionMesh = false, bool subField = false);
    static Field* createWithModelPath(const std::string& modelPath, bool collisionMesh = false);
    static Field* createWithData(const FieldData& data, bool collisionMesh = false, bool subField = false);
    static void createByIdAsync(int i, const std::function<void(Field*, void*)>& callback, void* callbackparam, bool collisionMesh = false, bool subField = false);
    static void createWithModelPathAsync(const std::string& modelPath, const std::function<void(Field*, void*)>& callback, void* callbackparam, bool collisionMesh = false);
    static void createWithDataAsync(const FieldData& data, const std::function<void(Field*, void*)>& callback, void* callbackparam, bool collisionMesh = false, bool subField = false);

    void setAirplaneToField(Airplane* airplane);
    cocos2d::Vec3 getAirplanePosition() const;

    void step(float dt);

    void setupSpheres();
    void shareSphereList(std::vector<Sprite3DBatchNode*> sphereBatchList);
    void checkSphereCollision(std::vector<AchievedSphereInfo>* achievedSphereInfoListPerFrame);
    int getSphereCount() const;
    const std::vector<AchievedSphereInfo>& getAchievedSphereInfoList() const;
    int getRemainingSphereCount() const;

    void setOtherAirplane(int peerId, Airplane* airplane);
    void setOtherAirplaneInfo(int peerId, const cocos2d::Vec3& position, const cocos2d::Vec3& rotation);
    void setOtherAirplaneAchievedSphere(int peerId, int sphereCount, const AchievedSphereInfo* sphereInfoList);
    const std::map<int, Airplane*>& getOtherAirplaneList() const;
    const std::map<int, std::vector<AchievedSphereInfo>>& getOtherAirplaneAchievedSphereList() const;


    // パラメータ
    int getFieldId() const;
    const std::string& getFieldName() const;

protected:
    Airplane* airplane;
    std::map<int, Airplane*> otherAirplaneList;

    void setupShaders(const FieldData& data);
    void setupSubFields();

    std::vector<Field*> subFieldList;
    std::vector<Sprite3DBatchNode*> sphereBatchList;
    std::vector<AchievedSphereInfo> achievedSphereList;
    std::map<int, std::vector<AchievedSphereInfo>> otherAirplaneAchievedSphereList;

    // パラメータ
    int fieldId;
    std::string fieldName;
};

#endif /* defined(__Flight__Field__) */