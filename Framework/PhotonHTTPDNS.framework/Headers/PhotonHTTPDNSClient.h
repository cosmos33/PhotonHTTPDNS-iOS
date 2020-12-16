//
//  PhotonHTTPDNS.h
//  MMDNSDemo
//
//  Created by Bruce on 2018/9/20.
//  Copyright © 2018年 Bruce. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "PhotonHTTPDNSConfigProtocol.h"
NS_ASSUME_NONNULL_BEGIN
@interface PhotonHTTPDNSClient : NSObject

/**
设置HTTPDNS,在startAutoRequest之前调用，整个app声明周期内仅调用一次
*/
+ (void)initHTTPDNSWithConfig:(id<PhotonHTTPDNSConfigProtocol>)config;

/**
 在initHTTPDNSWithConfig方法之后 startAutoRequest之前调用
 */
+ (void)shouldConsolLog:(BOOL)en;

/**
那些请求走自动的httpdns接入,在initHTTPDNSWithConfig方法之后 startAutoRequest之前调用，整个app声明周期内仅调用一次
@param domainList 设置支持走自动处理httpdns的请求，如果不设置，默认所有的api支持自动处理
*/
+ (void)setRequestDomainFilter:(nullable NSSet *)domainList;


/**
 获取可用的连接host

 @param host 原请求的host
 @return 真实请求使用的ip
 */
+ (NSString *)getIPbyHost:(nullable NSString *)host;

/**
 获取可用的连接host
 
 @param host 原请求的host
 @return 真实请求使用的ip列表
 */
+ (NSArray *)getIPsbyHost:(nullable NSString *)host;

/**
 连接成功后调用，通知DNS模块
 @param mainDomain 最初请求的域名
 @param succeedDomain 在请求成功时使用的域名
 @param successedPort 请求成功时使用的port
 */
+ (void)requestSucceedForDomain:(nullable NSString *)mainDomain andSucceedDomain:(nullable NSString *)succeedDomain andSuccessedPort:(NSInteger)successedPort;

/**
 连接失败后调用，通知DNS模块
 @param mainDomain 最初请求的域名
 @param failedDomain 在请求失败时使用的域名
 @param failedPort 请求失败时使用的port
 */
+ (void)requestFailedForDomain:(nullable NSString *)mainDomain andFailedDomain:(nullable NSString *)failedDomain andFailedPort:(NSInteger)failedPort;




#pragma mark ====== 创建多实例的HTTPDNS,此不支持处理sni，webview等场景。仅支持获取解析到的ip ============
/**
设置HTTPDNS,在startAutoRequest之前调用，整个app声明周期内仅调用一次
*/
- (instancetype)initHTTPDNSWithConfig:(id<PhotonHTTPDNSConfigProtocol>)config;

/**
 在initHTTPDNSWithConfig方法之后 startAutoRequest之前调用
 */
- (void)shouldConsolLog:(BOOL)en;

/**
 获取可用的连接host

 @param host 原请求的host
 @return 真实请求使用的ip
 */
- (NSString *)getIPbyHost:(nullable NSString *)host;

/**
 获取可用的连接host
 
 @param host 原请求的host
 @return 真实请求使用的ip列表
 */
- (NSArray *)getIPsbyHost:(nullable NSString *)host;

/**
 连接成功后调用，通知DNS模块
 @param mainDomain 最初请求的域名
 @param succeedDomain 在请求成功时使用的域名
 @param successedPort 请求成功时使用的port
 */
- (void)requestSucceedForDomain:(nullable NSString *)mainDomain andSucceedDomain:(nullable NSString *)succeedDomain andSuccessedPort:(NSInteger)successedPort;

/**
 连接失败后调用，通知DNS模块
 @param mainDomain 最初请求的域名
 @param failedDomain 在请求失败时使用的域名
 @param failedPort 请求失败时使用的port
 */
- (void)requestFailedForDomain:(nullable NSString *)mainDomain andFailedDomain:(nullable NSString *)failedDomain andFailedPort:(NSInteger)failedPort;


@end
NS_ASSUME_NONNULL_END
