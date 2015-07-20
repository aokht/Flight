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

class Field : public ExSprite3D
{
public:
    CREATE_FUNC(Field);

protected:
    bool init() override;
};

#endif /* defined(__Flight__Field__) */
