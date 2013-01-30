//
//  MathUtils.h
//  gameoneB
//
//  Created by Martin Raue on 1/14/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_MathUtils_h
#define gameoneB_MathUtils_h

#include "math.h"

#define MAX(a, b) (a > b ? a : b)

#define IW_DEG_TO_RAD M_PI / 180.0
#define IW_RAD_TO_DEG 180.0 / M_PI

#define IW_FRAND ((float)rand() / (float)RAND_MAX)
#define IW_RAND_UNIFORM(min, max) (min + (float)rand() / (float)RAND_MAX * (max - min))
#define IW_RAND_SIGN ((float)rand() / (float)RAND_MAX < 0.5 ? -1.0 : 1.0)

#endif
