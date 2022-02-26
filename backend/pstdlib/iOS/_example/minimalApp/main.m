#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

// ---------------- VIEW CONTROLLER ------------
@interface ViewController : UIViewController
@end
@implementation ViewController
- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Hello, World!";
    self.view.backgroundColor = [UIColor whiteColor];
}
@end
// ---------------- VIEW CONTROLLER ------------

// ---------------- SCENE DELEGATE ------------
@interface SceneDelegate : NSObject <UIWindowSceneDelegate>
@property (strong, nonatomic) UIWindow * window;
@end
@implementation SceneDelegate
- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session 
options:(UISceneConnectionOptions *)connectionOptions 
{
    UIWindowScene *windowScene = (UIWindowScene *)scene;
    self.window = [[UIWindow alloc] initWithWindowScene: windowScene];
    ViewController *vc = [[ViewController alloc] init];
    UINavigationController *navController = [[UINavigationController alloc] 
        initWithRootViewController: vc];
    self.window.rootViewController = navController;
    [self.window makeKeyAndVisible];
    self.window.windowScene = windowScene; 
}
@end
// ---------------- SCENE DELEGATE ------------

// ---------------- SCENE CLASS -------------
@interface MyScene : UIWindowScene
    /*
        A UIWindowScene object manages one instance of your app’s UI, 
        including one or more windows that you display from that scene. 
        The scene object manages the display of your windows on the user’s device, 
        and the life cycle of that scene as the user interacts with it. 
    */
@end
@implementation MyScene
@end
// ---------------- SCENE CLASS -------------


// ---------------- APP DELEGATE ------------
@interface AppDelegate : NSObject <UIApplicationDelegate>
@end
@implementation AppDelegate
- (UISceneConfiguration *)application:(UIApplication *)application 
configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession 
                              options:(UISceneConnectionOptions *)options
{
    UISceneConfiguration *sceneConfig = [[UISceneConfiguration alloc] 
        initWithName:@"My Scene Configuration" sessionRole:connectingSceneSession.role];
    sceneConfig.delegateClass = [SceneDelegate class];
    sceneConfig.sceneClass = [MyScene class];
    return sceneConfig;
}
@end
// ---------------- APP DELEGATE ------------

int main(int argc, char * argv[]) {
    NSString *delegateClassName;
    delegateClassName = NSStringFromClass([AppDelegate class]);
    UIApplicationMain(argc, argv, 
        nil, delegateClassName);
}