# FPGA-based data migration of network access

## Introduction & Scenario

将KV存储卸载到FPGA网卡，客户端可使用定制FPGA和RDMA直接访问服务器内存资源。旨在为分布式KV存储提供硬件协同辅助，通过FPGA加速远端内存访问，降低管理开销和访存延迟。

## Architecture

<p align="center">
<img src ="./arch.png">
</p>
<p align = "center">
<i>架构图</i>
</p>

由图可知，FPGA作为网络和Host主机之间的中间件，直接处理特定的KV操作网络包，减少存储过程中CPU的使用。

## Design

1. 数据放置：将KV的Index存储在FPGA片上内存中，同时开辟少量高速缓存空间存储热数据。
2. 索引查询：解析引擎对网络指令解析并完成索引查询
3. 热度检测：检测引擎对访存过程进行检测，统计访存频率确认热点数据。

##### 功能：
* 解析接收的网络包。
* 根据网络包请求进行访存热数据检测。
* 热数据迁移至高速缓存。

## Implementation

由于环境和时间限制，使用模拟的方式进行测试，仅实现FPGA模块内的上述功能，实际不与网络和主机交互。

## Plan

第一&二周：利用HLS实现上述设计
第三周：   通过模拟测试验证上述功能


