# 解决问题：

解决iOS客户端使用HTTPDNS时的痛点，完美处理了sni，web场景使用nsnsurlsession不能处理的问题。一键接入，全面体验方便高效的HTTPDNS带来的防劫持，调度以及网络访问性能的提升。

## 导入HTTPDNS库

### cocoapods依赖:（推荐使用）

添加source:
source 'https://github.com/cosmos33/MMSpecs'

pod 'PhotonHTTPDNS', '~>1.0.0'

PhotonHTTPDNS依赖openssl，需要引入

pod 'openssl-lib', '~>1.0.1'

如果工程中已经有引入依赖openssl库，可先尝试使用已引入的，如编译或者运行时存在问题，在依赖使用 pod 'openssl-lib','~>1.0.1'。


在[Build Phases]->[Link Binary With Libraries]中添加libc++。

## HTTPDNS库代码接入

HTTPDNS库的接入分为两种：

* 一种是单实例模式。这种适用的维度整个APP，HTTPDNS的生命周期和整个app的声明周期一致。支持两种使用方式。直接获取解析ip的使用的方式和一键式的使用的方式。具体请看单实例代码接入。

* 一种是可创建多实例的使用方式。这种适用的维度是APP中依赖的单个SDK，SDK内部自己使用HTTPDNS功能。创建的HTTPDNS实例的声明周期和SDK的声明周期一致。仅支持直接获取解析ip的使用方式。具体请看多实例代码接入。

##  单实例代码接入。

### 1.初始化
> 实现`MDDNSConfigProtocol`协议方法，配置好`MDDNSConfig`代码如下

```
MDDNSConfig.h
@interface PhotonHTTPDNSConfig : NSObject<PhotonHTTPDNSConfigProtocol>

@end

MDDNSConfig.m
@implementation MDDNSConfig
// appid
- (NSString *)getAppid{
    return @"2bd1a15c553de0a9df6dcede9af22962";
}

- (NSString *)getAppVersion{
    return @"1438";
}

- (NSString *)getOsType{
    return @"iOS";
}

- (NSString *)getUseragent{
    return @"MomoChat/9.0 ios/1471 (iPhone 8; iOS 12.1.2; zh_CN; iPhone10,1; S1)";
}
// 获取全局的域名配置，有则设置
- (NSString *)getHttpDNSGlobalConfigs{
    return @"";
}
// 获取当前的用户id
- (NSString *)getUserid{
    return @"12345";
}

// 获取当前的用户id
- (double)getLng{
    return 0.0f;
}

// 获取当前的用户id
- (double)getLat{
    return 32.43f;
}

// 指定预先解析的域名
- (nonnull NSArray<NSString *> *)getPreResolveHosts {
    return @[@"immomo.com"];
}

@end

// 获取当前的用户id
- (double)getLng{
    return 23.43f;
}

// 获取当前的用户id
- (double)getLat{
    return 32.43f;
}
@end

```

**使用PhotonHTTPDNSConfig初始化SDK**
* 1 单实例初始化：
```
 PhotonHTTPDNSConfig *config = [[PhotonHTTPDNSConfig alloc] init];
 [PhotonHTTPDNSClient initHTTPDNSWithConfig:config];

 // 打开底层log 日志，用于排查问题，默认是关闭
#ifdef DEBUG
[PhotonHTTPDNSClient shouldConsolLog:YES];
#else
[PhotonHTTPDNSClient shouldConsolLog:NO];

```

### 2.使用方式

#### 2.1 直接式的使用方式

>注: 此种使用方式需要接入方处理的事务比较多，比如(SNI场景和wkwebview)的接入，但是灵活性比较高

* 在请求之前调用 `[PhotonHTTPDNSClient getIPbyHost:host]`获取host解析后得到的ip。

* 请求成功调用 `[PhotonHTTPDNSClient requestSucceedForDomain:host andSucceedDomain:usedIp andSuccessedPort:0];
        }`

* 请求失败调用 `[PhotonHTTPDNSClient requestFailedForDomain:host andFailedDomain:usedIp andFailedPort:0]`

示例代码如下:

```
    // 业务层最原始的请求
    NSString *originUrl = _textfield.text;
    // 通过HTTPDNS获取IP成功，进行URL替换和HOST头设置
    NSURLComponents *com = [[NSURLComponents alloc] initWithString:originUrl];
    NSString *host = com.host;
    NSString *usedIp = [PhotonHTTPDNSClient getIPbyHost:host];
    com.host = usedIp;
    // 使用解析后的ip生成新的请求使用的url
    NSURL *newURL = com.URL;
    
    // 模拟发起请求(业务层针对自己的请求模块)
    NSURLSessionConfiguration * config = [NSURLSessionConfiguration defaultSessionConfiguration];
    NSURLSession *session = [NSURLSession sessionWithConfiguration:config delegate:nil delegateQueue:[NSOperationQueue mainQueue]];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:newURL];
    request.timeoutInterval = 10;
    
    // 设置Hosts请求头
    if (![usedIp isEqualToString:host]) {
        [request setValue:host forHTTPHeaderField:@"Host"];
    }
    NSURLSessionDataTask * dataTask =  [session dataTaskWithRequest:request completionHandler:^(NSData * __nullable data, NSURLResponse * __nullable response, NSError * __nullable error) {
        if (error) {
            // 请求失败。告知httpdns
            [PhotonHTTPDNSClient requestFailedForDomain:host andFailedDomain:usedIp andFailedPort:0];
        }else{
	     // 请求成功。告知httpdns
            [PhotonHTTPDNSClient requestSucceedForDomain:host andSucceedDomain:usedIp andSuccessedPort:0];
        }
        
    }];
    [dataTask resume];

```

> 直接调用httpdns模块获取ip的方式，业务成需要单独处理两种主要的场景:SNI（单ip多证书）场景和webview场景。

* SNI（单IP多HTTPS证书）场景下，iOS上层网络库 NSURLConnection/NSURLSession 没有提供接口进行 SNI 字段 配置，因此需要 Socket 层级的底层网络库例如 CFNetwork（不支持HTTP2.0），来实现 IP 直连网络请求适配方案。处理的一般思路是通过自定义的NSURULProtocol拦截请求后通过CFNetwork底层网络进行转发,处理过程中需要开发者处理数据的收发、重定向、解码、缓存等问题（CFNetwork是非常底层的网络实现）。也可使用libcurl或者Cronet

* webview场景下。可以使用NSURULProtocol拦截请求，在自定义的NSURULProtocol调用httdns模块处理域名的切换。其中存在的WKWebView 无法使用 NSURLProtocol 拦截请求的问题，可以使用私用的api进行处理

```
    //注册自己的protocol
    [NSURLProtocol registerClass:[CustomProtocol class]];
    //创建WKWebview
    WKWebViewConfiguration * config = [[WKWebViewConfiguration alloc] init];
    WKWebView * wkWebView = [[WKWebView alloc] initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height) configuration:config];
    [wkWebView loadRequest:webViewReq];
    [self.view addSubview:wkWebView];
    //注册scheme
    Class cls = NSClassFromString(@"WKBrowsingContextController");
    SEL sel = NSSelectorFromString(@"registerSchemeForCustomProtocol:");
    if ([cls respondsToSelector:sel]) {
        // 通过http和https的请求，同理可通过其他的Scheme 但是要满足ULR Loading System
        [cls performSelector:sel withObject:@"http"];
        [cls performSelector:sel withObject:@"https"];
    }
	
```

> 注:以上则为简单试的接入方式，接入方需要单独的处理SNI（单ip多证书）场景和webview场景存在的问题。

### 2.2 一键式的接入方式
* 调用 ` [PhotonHTTPDNSClient startEnableAutoRequest] `开启httpdns模块的urlportocol请求的拦截操作处理接入逻辑。
>注:此方式的接入，只适用于基于iOS SDK自带的网库库(NSURLSession NSURLConnection)，其他的网络库需要使用第一种方式

接入代码如下:

```
 // 可配置那些域名支持自动接入Httpdns的方式，其调用在startEnableAutoRequest之前
 NSSet *filter = [NSSet setWithObjects:@"img.momoc.com", nil];
 [PhotonHTTPDNSClient setRequestDomainFilter:filter];
 // 可配置那些域名支持自动接入Httpdns的方式，其调用在startEnableAutoRequest之前
 [PhotonHTTPDNSClient supportWKWebview];
 
 [PhotonHTTPDNSClient startEnableAutoRequest];

```
仅以上代码部分，不需再接入其他任何额外的代码



##  多实例代码接入

### 1.初始化
> 实现`MDDNSConfigProtocol`协议方法，配置好`MDDNSConfig`代码如下

```
MDDNSConfig.h
@interface PhotonHTTPDNSConfig : NSObject<PhotonHTTPDNSConfigProtocol>

@end

MDDNSConfig.m
@implementation MDDNSConfig
// appid
- (NSString *)getAppid{
    return @"2bd1a15c553de0a9df6dcede9af22962";
}

- (NSString *)getAppVersion{
    return @"1438";
}

- (NSString *)getOsType{
    return @"iOS";
}

- (NSString *)getUseragent{
    return @"MomoChat/9.0 ios/1471 (iPhone 8; iOS 12.1.2; zh_CN; iPhone10,1; S1)";
}
// 获取全局的域名配置，有则设置
- (NSString *)getHttpDNSGlobalConfigs{
    return @"";
}
// 获取当前的用户id
- (NSString *)getUserid{
    return @"12345";
}

// 获取当前的用户id
- (double)getLng{
    return 0.0f;
}

// 获取当前的用户id
- (double)getLat{
    return 32.43f;
}

// 指定预先解析的域名
- (nonnull NSArray<NSString *> *)getPreResolveHosts {
    return @[@"immomo.com"];
}

@end

// 获取当前的用户id
- (double)getLng{
    return 23.43f;
}

// 获取当前的用户id
- (double)getLat{
    return 32.43f;
}
@end

```

**使用PhotonHTTPDNSConfig初始化SDK**

* 初始化：

```
 PhotonHTTPDNSConfig *config = [[PhotonHTTPDNSConfig alloc] init];
PhotonHTTPDNSClient *client = [[PhotonHTTPDNSClient alloc] initHTTPDNSWithConfig:config];
 // 打开底层log 日志，用于排查问题，默认是关闭
#ifdef DEBUG
[client shouldConsolLog:YES];
#else
[client shouldConsolLog:NO];

```

### 2.使用方式

>注: 此种使用方式需要接入方处理的事务比较多，比如(SNI场景和wkwebview)的接入，但是灵活性比较高

* 在请求之前调用 `[client getIPbyHost:host]`获取host解析后得到的ip。

* 请求成功调用 `[client requestSucceedForDomain:host andSucceedDomain:usedIp andSuccessedPort:0];
        }`

* 请求失败调用 `[client requestFailedForDomain:host andFailedDomain:usedIp andFailedPort:0]`

示例代码如下:

```

 MDDNSConfig *config = [[MDDNSConfig alloc] init];
 _client = [[PhotonHTTPDNSClient alloc] initHTTPDNSWithConfig:config];

```

```
   NSString *originUrl = @"https://example.immomo.com/example";
    NSURLComponents *com = [[NSURLComponents alloc] initWithString:originUrl];
    NSString *host = com.host;
    NSString *usedIp = [_client getIPbyHost:host];
    com.host = usedIp;
    NSURL *newURL = [NSURL URLWithString:originUrl];
    NSURLSession *session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]
     delegate:self delegateQueue:nil];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:newURL];
    request.timeoutInterval = 30;
    if ([usedIp isEqualToString:host]) {
        [request addValue:host forHTTPHeaderField:@"Host"];
    }
    
    NSURLSessionDataTask * dataTask =  [session dataTaskWithRequest:request completionHandler:^(NSData * __nullable data, NSURLResponse * __nullable response, NSError * __nullable error) {
        if (error) {
            [self.client requestFailedForDomain:host andFailedDomain:usedIp andFailedPort:0];
        }else{
            [self.client requestSucceedForDomain:host andSucceedDomain:usedIp andSuccessedPort:0];
        }

    }];
    [dataTask resume];

```






 










 










 
