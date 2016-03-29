/* 
 *  Copyright (c) 2011,  广州江南科友union  
 *  All rights reserved. 
 *  文件名称： mangerCmd.h
 *  摘 要：    应用加密机和密钥管理指令接口的定义  
 *  当前版本： 1.0,  编写者: 杨武，修改时间: 2011-11-22 修改内容: 创建
 *  
 */
extern "C"
{
	//和应用加密机创建socket连接
	//输入参数：hsmIp 加密机IP地址
	//输入参数：hsmPort 加密机对外服务端口
	//输入参数：isCfg 0:不从配置文件读取hsmIp、hsmPort启用输入参数中的这些信息 1：从配置文件中读取上述信息
	//返回值： <0：失败；>0 成功，所创建的套接字句柄
	_declspec(dllexport) int union_syj1001_connect(char * hsmIp,int hsmPort,int isCfg);
	//断开和应用加密机的socket连接
	//输入参数：clientSocket socket套接字句柄
	//返回值： <0：失败；0：成功
	_declspec(dllexport) int union_syj1001_disConnect(int clientSocket);
	//透传接口，将组好的报文转发给应用加密机
	//输入参数：msg 报文
	//输入参数：hsmIp 加密机IP地址
	//输入参数：hsmPort 加密机对外服务端口
	//输入参数：timeOut socket超时等待时间 ms单位
	//输入参数：isCfg 0:不从配置文件读取lenOfMsg、hsmIp、hsmPort、timeOut、hsmLenOfMsgHeader、isLenOfHsmMsg 启用输入参数中的这些信息 1：从配置文件中读取上述信息
	//输入参数：clientSocket -1:采用的是短连接方式，需要在函数体完成socket的创建和释放 >0:采用长连接方式，复用该套接字句柄
	//输入参数：hsmLenOfMsgHeader 消息头长度
	//输入参数：isLenOfHsmMsg 报文长度（两字节）
	//输出参数：revMsg 返回的报文相应
	//返回值： <0：失败；>0：输出的报文长度
	_declspec(dllexport) int union_syj1001_direct_sendMsg(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char* msg, char *revMsg);
	//获取加密机基本信息 命令码 0000
	//输入参数：hsmIp 加密机IP地址
	//输入参数：hsmPort 加密机对外服务端口
	//输入参数：timeOut socket超时等待时间 ms单位
	//输入参数：isCfg 0:不从配置文件读取lenOfMsg、hsmIp、hsmPort、timeOut、hsmLenOfMsgHeader、isLenOfHsmMsg 启用输入参数中的这些信息 1：从配置文件中读取上述信息
	//输入参数：clientSocket -1:采用的是短连接方式，需要在函数体完成socket的创建和释放 >0:采用长连接方式，复用该套接字句柄
	//输入参数：hsmLenOfMsgHeader 消息头长度
	//输入参数：isLenOfHsmMsg 报文长度（两字节）
	//输出参数：revMsg 返回的报文相应
	//返回值： <0：失败；=0：revMsg中包含主密钥校验值，和加密机基本信息；=13 加密机基本信息
	_declspec(dllexport) int union_syj1001_getHsmBaseInfo(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket, char *revMsg);
	//给加密机授权 命令码 0002
	//输入参数：hsmIp 加密机IP地址
	//输入参数：hsmPort 加密机对外服务端口
	//输入参数：timeOut socket超时等待时间 ms单位
	//输入参数：isCfg 0:不从配置文件读取lenOfMsg、hsmIp、hsmPort、timeOut、hsmLenOfMsgHeader、isLenOfHsmMsg 启用输入参数中的这些信息 1：从配置文件中读取上述信息
	//输入参数：clientSocket -1:采用的是短连接方式，需要在函数体完成socket的创建和释放 >0:采用长连接方式，复用该套接字句柄
	//输入参数：hsmLenOfMsgHeader 消息头长度
	//输入参数：isLenOfHsmMsg 报文长度（两字节）
	//返回值： <0：失败；=0成功
	_declspec(dllexport) int union_syj1001_setAuthApply(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket);
	//取消加密机授权 命令码 0004
	//输入参数：hsmIp 加密机IP地址
	//输入参数：hsmPort 加密机对外服务端口
	//输入参数：timeOut socket超时等待时间 ms单位
	//输入参数：isCfg 0:不从配置文件读取lenOfMsg、hsmIp、hsmPort、timeOut、hsmLenOfMsgHeader、isLenOfHsmMsg 启用输入参数中的这些信息 1：从配置文件中读取上述信息
	//输入参数：clientSocket -1:采用的是短连接方式，需要在函数体完成socket的创建和释放 >0:采用长连接方式，复用该套接字句柄
	//输入参数：hsmLenOfMsgHeader 消息头长度
	//输入参数：isLenOfHsmMsg 报文长度（两字节）
	//返回值： <0：失败；=0成功
	_declspec(dllexport) int union_syj1001_cancelAuthApply(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket);
	 
};
