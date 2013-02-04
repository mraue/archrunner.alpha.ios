//
//  IWUIDefinitions.h
//  theBr1dge
//
//  Created by Martin Raue on 2/3/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWUIDefinitions_h
#define theBr1dge_IWUIDefinitions_h

#define IWUI_COLOR_BLUE(a) IWVector4Make(0.4, 0.4, 1.0, a)
#define IWUI_COLOR_DARK_BLUE(a) IWVector4Make(48.0 / 255.0, 43.0 / 255.0, 140.0 / 255.0, a)
#define IWUI_COLOR_LIGHT_BLUE(a) IWVector4Make(139.0 / 255.0, 134.0 / 255.0, 255.0 / 255.0, a)
#define IWUI_COLOR_GOLD(a) IWVector4Make(255.0 / 255.0, 219.0 / 255.0, 93.0 / 255.0, a)
#define IWUI_COLOR_DARK_GOLD(a) IWVector4Make(132.0 / 255.0, 106.0 / 255.0, 25.0 / 255.0, a)
#define IWUI_COLOR_PURPLE(a) IWVector4Make(181.0 / 255.0, 63.0 / 255.0, 244.0 / 255.0, a)
#define IWUI_COLOR_DARK_PURPLE(a) IWVector4Make(93.0 / 255.0, 17.0 / 255.0, 136.0 / 255.0, a)

typedef enum {
    IWUI_ORIENTATION_HORIZONTAL,
    IWUI_ORIENTATION_VERTICAL
} IWUI_ORIENTATION;

typedef enum {
    IWUI_DIRECTION_NORMAL,
    IWUI_DIRECTION_REVERSE
} IWUI_DIRECTION;

#endif
