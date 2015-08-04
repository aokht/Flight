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

vector<vector<Vec3>> Sphere::getSphereGroupPositionList(const string& path)
{
    stringstream sphereGroupListStringStream(FileUtils::getInstance()->getStringFromFile(path));
    std::string line;
    map<int, vector<Vec3>> sphereGroupListMap;
    while (getline(sphereGroupListStringStream, line)) {
        string param;
        stringstream lineStream(line);
        int index = 0;
        int groupNumber, positionNumber;
        Vec3 coordinate;
        int prefixLength = (int)string("SphereGroup").size();
        while (getline(lineStream, param, ',')) {
            switch (index) {
                case 0:
                {
                    string groupString(param.begin() + prefixLength, param.end());
                    stringstream groupStream(groupString);
                    string groupStr, numberStr;
                    getline(groupStream, groupStr, '_');
                    getline(groupStream, numberStr, '_');

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
            sphereGroupListMap[groupNumber] = vector<Vec3>(3); // TODO 可変
        }
        sphereGroupListMap[groupNumber][positionNumber - 1] = coordinate;
    }

    vector<vector<Vec3>> sphereGroupList;
    for (auto sphereGroup : sphereGroupListMap) {
        sphereGroupList.push_back(sphereGroup.second);
    }

    return sphereGroupList;
}