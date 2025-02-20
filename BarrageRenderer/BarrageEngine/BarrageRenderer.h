// Part of BarrageRenderer. Created by UnAsh.
// Blog: http://blog.exbye.com
// Github: https://github.com/unash/BarrageRenderer

// This code is distributed under the terms and conditions of the MIT license.

// Copyright (c) 2015年 UnAsh.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#import <Foundation/Foundation.h>

#import "BarrageHeader.h"

@class BarrageDescriptor;
@class BarrageRenderer;

typedef NS_ENUM(NSInteger, BarrageSpriteStage) {
    BarrageSpriteStageBegin = 1, // 弹幕进入屏幕阶段
    BarrageSpriteStageEnd   = 2  // 弹幕退出屏幕阶段
};

@protocol BarrageRendererDelegate <NSObject>

@optional
/// 通过外部渠道获取当前时间,用于内部时间系统; 当依附的视频具有快进快退功能时,必须实现这个函数,并返回时间轴上的当前时刻,即已经播放的时间.
- (NSTimeInterval)timeForBarrageRenderer:(BarrageRenderer *)renderer;

/// 弹幕生命周期行为，实验特性; 可试用, 亦可以通过继承 BarrageRenderer 来实现相同功能
- (void)barrageRenderer:(BarrageRenderer *)renderer spriteStage:(BarrageSpriteStage)stage spriteParams:(NSDictionary *)params;

@end

/// 弹幕渲染器
@interface BarrageRenderer : NSObject

#pragma mark - life cycle

#pragma mark - control

/// 启动弹幕, 内部时钟从0开始;
/// 若是stop之后的start,则start函数内部会清空records;
/// 视频开始的时候需要同时运行此方法.
- (void)start;

/// 暂停, 已经渲染上去的保持不变, 此时发送弹幕无效, 内部时钟暂停;
/// 视频暂停的时候需要同时运行此方法.
- (void)pause;

/// 停止弹幕渲染, 会清空所有; 再发弹幕就无效了; 一切都会停止;
/// 此方法在不再需要弹幕的时候必须调用,否则可能造成内存泄露.
- (void)stop;

/// 接收弹幕消息, 如果尚未start, 则调用无效.
- (void)receive:(BarrageDescriptor *)descriptor;

#pragma mark - config

/// 画布的边距
@property(nonatomic,assign)UIEdgeInsets canvasMargin;
/// 和 canvasMargin 类似，但数值为 superview.bounds.size 的百分比。例如，左0.5右0.5，width 就会为0。该值优先于 canvasMargin。
@property(nonatomic, assign)UIEdgeInsets canvasPercentMargin;
/// 画布是否拦截事件
@property(nonatomic,assign)BOOL masked;

/// 是否开启平滑; 对于突发弹幕，开启平滑可以降低瞬间的掉帧，默认关闭
/// 对于 CPU 性能不错的手机(比如>=iphone7)，推荐开启此功能
/// 设置时间平滑, 应对弹幕激增;
/// 范围为[0,1],当为0时，无平滑; 否则越大，越平滑;
/// 高平滑值在大量弹幕(一般100+)的时候，可能造成弹幕丢失
@property(nonatomic,assign)CGFloat smoothness;

/// 调整弹幕整体速度, 需要>0, 否则会被抛弃.
@property(nonatomic,assign)CGFloat speed;

/// 如果时光倒流, 弹幕能不能重新显示. 若设置为YES, 则当执行后退8s->5s时,会显示6s时刻上的弹幕.
@property(nonatomic,assign)BOOL redisplay;

/// 获取外部时间的代理,若需将弹幕固定到时间点上,则需设置此代理
@property(nonatomic,weak)id<BarrageRendererDelegate> delegate;

#pragma mark - output

/// 返回给外部的view
@property(nonatomic,weak)UIView * view;

/// 获取当前屏幕弹幕数量,spriteName表示弹幕类名,如果传入nil,则计算屏幕显示出的所有弹幕数量.
- (NSInteger)spritesNumberWithName:(NSString *)spriteName;

/// 移除当前屏幕上的弹幕; 可在转屏幕的时候调用，以应对位置错乱的情况
/// spriteName表示弹幕类名,如果传入nil,则计算屏幕显示出的所有弹幕数量.
- (void)removePresentSpritesWithName:(NSString *)spriteName;

/// 移除标识符为 identifier 的弹幕
- (void)removeSpriteWithIdentifier:(NSString *)identifier;

/// 逻辑时间,露出参考.
@property(nonatomic,assign,readonly)NSTimeInterval time;

#pragma mark - z-index

/// 是否开启z-index功能,开启之后,性能会稍有降低,绘图会按照z_index进行,值越大,越靠上;默认关闭.
@property(nonatomic,assign)BOOL zIndex;

#pragma mark - record

/// 如需要记录,需要在运行start之后立即运行此函数,内部会通过时间差计算delay;
/// 记录弹幕,可能会序列化到本地; 默认为NO.
@property(nonatomic,assign)BOOL recording;

/// 加载已经存在的弹幕,如果已经start, 会立刻被调用receive; 否则, 会等到start的时候再调用receive.
/// 在 v2.1 之后, 使用 load 不会重新调整 descriptor 的 delay; 而在之前的版本, 会调整 descriptor 的 delay
- (void)load:(NSArray *)descriptors;

/// 弹幕记录数组.
- (NSArray *)records;

@end
