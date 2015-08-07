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
    static Sprite3DBatchNode* create(const std::string& modelPath, const std::string& vertShader, const std::string& fragShader);
    static Sprite3DBatchNode* create(const Sprite3DBatchNode& src);
    static void makeBatchNodeShared(const Sprite3DBatchNode& src, Sprite3DBatchNode* dst);

    void add(cocos2d::Vec3 position);
    int getNodeCount() const;

    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

    struct NodeStatus {
        bool isVisible;
        cocos2d::Vec3 position;
    };
    const std::vector<NodeStatus>& getNodeStatusList() const;

    void setNodeVisible(int index, bool visible);
    int getRemainingNodeCount() const;

protected:
    int nodeCount;
    std::vector<float>* positionOffsetList;
    std::vector<float>* visibleList;
    std::vector<NodeStatus>* statusList;
    bool isShared;

    bool isBuilt;
    void build();

    Sprite3DBatchNode();
    ~Sprite3DBatchNode();
    bool initWithFileAndShaders(const std::string& modelPath, const std::string& vertShader, const std::string& fragShader);
    void setupShaders(const std::string& vertShader, const std::string& fragShader);

    cocos2d::CustomCommand command;

    std::string modelPath;
    std::string shaderPathVert;
    std::string shaderPathFrag;
};

#endif /* defined(__Flight__Sprite3DBatchNode__) */