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
#include "GameSceneData.h"

class Airplane;

class Field : public ExSprite3D
{
public:
    // Life Cycle
    static Field* createById(int i, bool collisionMesh = false, bool subField = false);
    static Field* createWithModelPath(const std::string& modelPath, bool collisionMesh = false);
    static Field* createWithData(const FieldData& data, bool collisionMesh = false, bool subField = false);
    void setAirplaneToField(Airplane* airplane);
    cocos2d::Vec3 getAirplanePosition() const;

    void step(float dt);

    void setupSpheres();
    void shareSphereList(std::vector<Sprite3DBatchNode*> sphereBatchList);
    int checkSphereCollision();
    int getSphereCount() const;
    const std::vector<AchievedSphereInfo>& getAchievedSphereInfoList() const;

    // パラメータ
    int getFieldId() const;
    const std::string& getFieldName() const;

protected:
    Airplane* airplane;

    void setupShaders(const FieldData& data);

    std::vector<Field*> subFieldList;
    std::vector<Sprite3DBatchNode*> sphereBatchList;
    std::vector<AchievedSphereInfo> achievedSphereList;

    // パラメータ
    int fieldId;
    std::string fieldName;
};

#endif /* defined(__Flight__Field__) */