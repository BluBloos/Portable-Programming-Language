//
//  myClass.h
//  myFirstIOSApp
//
//  Created by Noah Cabral on 2022-02-07.
//

#ifndef myClass_h
#define myClass_h

@interface myClass : NSObject

// Readonly means the setter is not automatically generated.
@property (readonly) int myProp;
@property NSString *name;
-(instancetype) initWithName: (NSString *) defaultName;

-(void) getInfo;

// defining methods here.
-(float) weightInKg: (float) weightInPounds;


@end


#endif /* myClass_h */
