//
//  PhotonHTTPDNSConfigProtocol.h
//  MDDNS
//
//  Created by Bruce on 2019/5/7.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN
@protocol PhotonHTTPDNSConfigProtocol <NSObject>
@required
// appid
- (NSString *)getAppid;

- (NSString *)getAppVersion;

- (NSString *)getOsType;

- (NSString *)getUseragent;

// 获取全局的域名配置，有则设置
- (NSString *)getHttpDNSGlobalConfigs;

- (NSArray<NSString *> *)getPreResolveHosts;
// 获取当前的用户id
- (NSString *)getUserid;

// 获取当前的精度
- (double)getLng;

// 获取当前的维度
- (double)getLat;

@end

NS_ASSUME_NONNULL_END
