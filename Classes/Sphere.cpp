//
//  Sphere.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/11.
//
//

#include <string>
#include <sstream>
#include <iostream>
#include "Sphere.h"
#include "SceneManager.h"
#include "GameSceneManager.h"
#include "Global.h"

using namespace std;
using namespace cocos2d;


Sphere* Sphere::createWithType(Sphere::Type type)
{
    Sphere* sphere = new Sphere(type);

    if (sphere && sphere->init()) {
        sphere->autorelease();
        return sphere;
    }

    CC_SAFE_DELETE(sphere);
    return nullptr;
}

Sphere* Sphere::createOne(Sphere::Type type)
{
    Sphere* sphere = new Sphere(type);

    if (sphere && sphere->initOne()) {
        sphere->autorelease();
        return sphere;
    }

    CC_SAFE_DELETE(sphere);
    return nullptr;
}

Sphere* Sphere::create(const Sphere& src)
{
    return createWithType(src.type);
}

bool Sphere::init()
{
    if (! Sprite3DBatchNode::initWithFileAndShaders("objects/diamond.obj", "SphereShader.vert", "SphereShader.frag")) {
        return false;
    }

    this->setupShaders();

    return true;
}

bool Sphere::initOne()
{
    if (! Sprite3DBatchNode::initWithFileAndShaders("objects/diamond.obj", "SphereShaderForTitleScreen.vert", "SphereShaderForTitleScreen.frag")) {
        return false;
    }

    this->setupShaders();
    this->add(Vec3(0.f, 0.f, 0.f));

    return true;
}

void Sphere::setupShaders()
{
    GLProgramState* state = this->getGLProgramState();

    Color3B c;
    switch (this->type) {
        case Type::BLUE:
            c = Color3B::BLUE;
            break;
        case Type::YELLOW:
            c = Color3B::YELLOW;
            break;
        case Type::RED:
            c = Color3B::RED;
            break;
        default:
            c = Color3B::WHITE;
            break;
    }
    state->setUniformVec4("u_sphereColor", Vec4(c.r / 255.f, c.g / 255.f, c.b / 255.f, 1.f));

    state->setUniformCallback("u_cameraPosition", [](GLProgram* program, Uniform* uniform) {
        Vec3 p;
        if (SceneManager::getInstance()->isInGameScene()) {
            p = GameSceneManager::getInstance()->getCameraPosition();
        } else {
            p = Camera::getDefaultCamera()->getPosition3D();
        }
        program->setUniformLocationWith3f(uniform->location, p.x, p.y, p.z);
    });

    state->setUniformCallback("u_cameraEye", [](GLProgram* program, Uniform* uniform) {
        Vec3 p;
        if (SceneManager::getInstance()->isInGameScene()) {
            p = GameSceneManager::getInstance()->getCameraEye();
        } else {
            p = Camera::getDefaultCamera()->getPosition3D() + Vec3(0.f, 0.f, -1.f);
        }
        program->setUniformLocationWith3f(uniform->location, p.x, p.y, p.z);
    });
}


Sphere::Sphere(Sphere::Type type) :
    type(type)
{
}

Sphere::~Sphere()
{
}


vector<vector<Sphere::SphereGroupInfo>> Sphere::getSphereGroupInfoList(const string& path)
{
    stringstream sphereGroupListStringStream(FileUtils::getInstance()->getStringFromFile(path));
    std::string line;
    map<int, vector<SphereGroupInfo>> sphereGroupListMap;
    while (getline(sphereGroupListStringStream, line)) {
        string param;
        stringstream lineStream(line);
        int index = 0;
        int groupNumber, positionNumber;
        Type colorType;
        Vec3 coordinate;
        int prefixLength = (int)string("SphereGroup").size();
        while (getline(lineStream, param, ',')) {
            switch (index) {
                case 0:
                {
                    string groupString(param.begin() + prefixLength, param.end());
                    stringstream groupStream(groupString);
                    string groupStr, numberStr, colorStr;
                    getline(groupStream, groupStr, '_');
                    getline(groupStream, numberStr, '_');
                    getline(groupStream, colorStr, '_');
                    if (colorStr.compare("Blue") == 0) {
                        colorType = Type::BLUE;
                    } else if (colorStr.compare("Yellow") == 0) {
                        colorType = Type::YELLOW;
                    } else if (colorStr.compare("Red") == 0) {
                        colorType = Type::RED;
                    } else {
                        colorType = Type::NONE;
                    }

                    groupNumber = atoi(groupStr.data());
                    positionNumber = atoi(numberStr.data());
                }
                    break;
                case 1:
                    coordinate.x = stof(param);
                    break;
                case 2:
                    coordinate.y = stof(param);
                    break;
                case 3:
                    coordinate.z = stof(param);
                    break;
                default:
                    break;
            }
            index++;
        }
        if (sphereGroupListMap.find(groupNumber) == sphereGroupListMap.end()) {
            sphereGroupListMap[groupNumber] = vector<SphereGroupInfo>(3); // TODO 可変
        }
        sphereGroupListMap[groupNumber][positionNumber - 1] = { colorType, coordinate };
    }

    vector<vector<SphereGroupInfo>> sphereGroupList;
    for (auto sphereGroup : sphereGroupListMap) {
        sphereGroupList.push_back(sphereGroup.second);
    }

    return sphereGroupList;
}