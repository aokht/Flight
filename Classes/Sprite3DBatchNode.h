//
//  Sprite3DBatchNode.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/01.
//
//

#ifndef __Flight__Sprite3DBatchNode__
#define __Flight__Sprite3DBatchNode__

#include "cocos2d.h"
#include "ExSprite3D.h"

class Sprite3DBatchNode : public ExSprite3D
{
public:
    static Sprite3DBatchNode* create(const std::string& modelPath);

    void add(cocos2d::Vec3 position);

    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

protected:
    int nodeCount;
    std::vector<float> positionOffsetList;

    bool isBuilt;
    void build();

    Sprite3DBatchNode();
    ~Sprite3DBatchNode();
    bool initWithFile(const std::string& modelPath);
    void setupShaders();

    cocos2d::CustomCommand command;
};

#endif /* defined(__Flight__Sprite3DBatchNode__) */