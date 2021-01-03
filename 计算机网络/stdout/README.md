# CSU_network
计网课设

0.1.0版本依旧有着很多bug（由于多线程的问题）

# ZOOM

## User

### 用途

在服务器端存储用户信息，保证通信正常。

### 成员变量

- `name`： 用户名
- `socket`：socket通信
- `friends`：好友列表
- `group`：群组列表
- `pw`：socket的输出流
- `br`：输入流

### 方法

各种`getter` `setter`

## Server

### 用途

服务器，每当一个用户连接上之后，创建一个`ServerThread`

### 方法

- `startServer`：开始服务器，接收到输入则创建一个进程
- `Login`：处理登陆情况
- `Judge`：判断用户名密码（目前使用的就是`equal`判断，后期可改用SQL）



## ServerThread

### 用途

用于保持客户端和服务器端的通信，交互

采用指令驱动交互的方式，例如`/say` `/add`等

### 方法

- `run`：持续接收用户指令

- `dealMsg`：处理接收到的指令
- 其他：用于处理指令



## Client && LoginGUI

### 用途

登陆时和服务器的交互



## ClientThread

### 用途

接收服务器端数据，给出相应的显示



## ClientGUI

### 用途

登陆完成后客户端界面

显示好友列表，群组列表

提供加好友，删好友，加群，退群方法（单方面）



### TalkGUI && GroupGUI

### 用途

聊天窗口界面（不同时打开就聊不了的那种，之后可以改进，但是没必要）







