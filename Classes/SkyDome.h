//
//  SkyDome.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/13.
//
//

#ifndef __Flight__SkyDome__
#define __Flight__SkyDome__

#include "cocos2d.h"
#include "ExSprite3D.h"

class SkyDome : public ExSprite3D
{
public:
    static SkyDome* create(const std::string& modelPath);

protected:
    bool initWithFile(const std::string& modelPath);
    void setupShader();
};


#endif /* defined(__Flight__SkyDome__) */
