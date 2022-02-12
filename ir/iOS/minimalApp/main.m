#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface AppDelegate : NSObject <UIApplicationDelegate>
@end

@implementation AppDelegate
@end

@interface ViewController : UIViewController
@end

@implementation ViewController
@end

// https://developer.apple.com/documentation/uikit/view_controllers/managing_content_in_your_app_s_windows?language=objc
// One ViewController per page. Many views per page.
// There exists ContainerViewControllers that manage other types of ViewControllers.

int main(int argc, char * argv[]) {
    // autorelease pool.
    // once we hit the end of the autorelease pool, for all objects that have received the autorelease
    // message, they will then be sent a release message. This decrements their reference counter.
    // should the ref counter go to zero, the objects are sent a dealloc message.
    //
    // sending an object an autorelease message is basically adding that object to the autorelease pool.
    // if we send a retain message, this will increase the ref count by 1.
    //
    // storing some ref in an array will increase the ref count, but copying the pointer to another local var
    // will not...
    //
    // an NSConstantString will return a reference count of 0xffffffff because these actually have no reference count.
    // i.e. you cannot release these.
    // 
    // if you intitialize and immutalbe string by a constant string object, this too will have a reference count
    // of 0xffffffff

    // question. What if there is no autorelease pool and I send an object the autorelease message?
    //     answer: the program should throw an error.
    
    // TODO(Noah): Is there an assert?
    // NSLog (@"delegateClassName retain count: %lu", (unsigned long)[delegateClassName retainCount]);

    // The folks over at Apple have some rules about memory management.
    // "When you no longer need it, you must relinquish ownership of an object you own"
    // this means either sending it a release message or an autorelease message.
    //
    // using release will immediately dealloc something that you have created.
    // so you would like to use an autorelease.
    //
    // one of the other rules is that "I do not own objects returned by reference".

    // NOTE(Noah): I am CONVINCED that the autorelease pool is not such a good idea.
    // NSStringFromClass will have retain count of 0xffffffff, and thererfore autorelease pool does 
    // nothing. 

    NSString *delegateClassName;
    delegateClassName = NSStringFromClass([AppDelegate class]);
    UIApplicationMain(argc, argv, 
        nil, delegateClassName);
}