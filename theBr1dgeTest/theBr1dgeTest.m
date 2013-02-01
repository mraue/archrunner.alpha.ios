//
//  theBr1dgeTest.m
//  theBr1dgeTest
//
//  Created by Martin Raue on 2/1/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import "theBr1dgeTest.h"

#import "IWMathUtils.h"

#import "IWIndexList.h"

@implementation theBr1dgeTest

- (void)setUp
{
    [super setUp];
    
    // Set-up code here.
}

- (void)tearDown
{
    // Tear-down code here.
    
    [super tearDown];
}

- (void)testIWIndexList
{
    // Setup index list
    IWIndexListData indexListData = IWIndexListMake(10);
    for (int i = 0; i < 10; i++) {
        indexListData.map[i] = i;
        indexListData.reverseMap[i] = i;
    }
    IWIndexListReplaceWithLast(&indexListData, 0);
    STAssertTrue(indexListData.map[0] == 9, @"%u", indexListData.map[0]);
    STAssertTrue(indexListData.reverseMap[9] == 0, @"%u", indexListData.reverseMap[0]);
IW_RAND_RANDOMIZE_TIMER;
    unsigned int l = IWIndexListRemoveRandom(&indexListData);
printf("%u\n", l);
STAssertTrue(indexListData.map[indexListData.reverseMap[l]] == 8, @"%u", indexListData.map[indexListData.reverseMap[l]]);
    IWIndexListReplaceWithLast(&indexListData, 7);
for (int i = 0; i < 10; i++) {
    printf("%i %u %u\n", i, indexListData.map[i], indexListData.reverseMap[i]);
}
STAssertTrue(indexListData.nEntries == 7, @"");
}

@end
