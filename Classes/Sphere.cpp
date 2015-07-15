//
//  Sphere.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/11.
//
//

#include <string>
#include <sstream>
#include "Sphere.h"

using namespace std;
using namespace cocos2d;

bool Sphere::init()
{
    if (! Sprite3D::initWithFile("spheres/sphere.obj")) {
        return false;
    }

    return true;
}

vector<Vec3> Sphere::getSpherePositionList()
{
    stringstream sphereListStringStream(FileUtils::getInstance()->getStringFromFile("spheres/sphere_list.dat"));
    std::string line;
    vector<Vec3> sphereList;
    while (getline(sphereListStringStream, line)) {
        string param;
        stringstream lineStream(line);
        int index = 0;
        Vec3 coordinate;
        while (getline(lineStream, param, ',')) {
            switch (index) {
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
        sphereList.push_back(coordinate);
    }

    return sphereList;
}