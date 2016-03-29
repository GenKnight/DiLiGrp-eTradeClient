/* 
 *  Copyright (c) 2011,  广州江南科友union  
 *  All rights reserved. 
 *  文件名称： commonCmd.h
 *  摘 要：    应用加密机通用指令接口的定义  
 *  当前版本： 1.0,  编写者: 杨武，修改时间: 2011-11-22 修改内容: 创建
 *  
 */
extern "C" 
{
	
	// 装载打印格式 0062
	//输入参数：hsmIp 加密机IP地址
	//输入参数：hsmPort 加密机对外服务端口
	//输入参数：timeout socket超时等待时间 ms单位
	//输入参数：isCfg 0:不从配置文件读取lenOfMsg、hsmIp、hsmPort、timeOut、hsmLenOfMsgHeader、isLenOfHsmMsg 启用输入参数中的这些信息 1：从配置文件中读取上述信息
	//输入参数：clientSocket -1:采用的是短连接方式，需要在函数体完成socket的创建和释放 >0:采用长连接方式，复用该套接字句柄
	//输入参数：hsmLenOfMsgHeader 消息头长度
	//输入参数：isLenOfHsmMsg 报文长度（两字节）
	//输入参数：typeOfFormat 格式类型 P:打印PIN格式 K:打印密钥成分格式
	//输入参数：lenOfFormat 格式长度 小于512
	//输入参数：formatInfo 格式内容
	//返回值： <0：失败；=0成功
	_declspec(dllexport) int union_syj1001_storePrintFormat(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char typeOfFormat,int lenOfFormat,char * formatInfo);
	//用指定密钥对指定数据进行加密 A050
	//输入参数：typeOfAlgo 算法类型
	//输入参数：indexOfKey 加密密钥索引
	//输入参数：keyValByMK 加密密钥值
	//输入参数：iniVec 初始向量
	//输入参数：lenOfSData 数据块长度
	//输入参数：SData 数据块
	//输出参数：DData 加密后数据块
	//返回值： <0：失败；>0成功 返回数据长度
	_declspec(dllexport) int union_syj1001_enctyptData(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char * typeOfAlgo,char* indexOfKey,char *keyValByMK,char*iniVec,int lenOfSData,char *SData,char* DData);
	//用指定密钥对指定数据进行加密 S050
	//输入参数：typeOfAlgo 算法类型
	//输入参数：indexOfKey 加密密钥索引
	//输入参数：keyValByMK 加密密钥值
	//输入参数：iniVec 初始向量
	//输入参数：lenOfSData 数据块长度
	//输入参数：SData 数据块
	//输出参数：DData 加密后数据块
	//返回值： <0：失败；>0成功 返回数据长度
	_declspec(dllexport) int union_syj1001_enctyptData_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char * typeOfAlgo,char* indexOfKey,char *keyValByMK,char*iniVec,int lenOfSData,char *SData,char* DData);
	//用指定密钥对指定数据进行解密 A050
	//输入参数：typeOfAlgo 算法类型
	//输入参数：indexOfKey 解密密钥索引
	//输入参数：keyValByMK 解密密钥值
	//输入参数：iniVec 初始向量
	//输入参数：lenOfSData 数据块长度
	//输入参数：SData 数据块
	//输出参数：DData 解密后数据块
	//返回值： <0：失败；>0成功 返回数据长度
	_declspec(dllexport) int union_syj1001_dectyptData(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char * typeOfAlgo,char* indexOfKey,char *keyValByMK,char*iniVec,int lenOfSData,char *SData,char* DData);
	//用指定密钥对指定数据进行解密 S050
	//输入参数：typeOfAlgo 算法类型
	//输入参数：indexOfKey 解密密钥索引
	//输入参数：keyValByMK 解密密钥值
	//输入参数：iniVec 初始向量
	//输入参数：lenOfSData 数据块长度
	//输入参数：SData 数据块
	//输出参数：DData 解密后数据块
	//返回值： <0：失败；>0成功 返回数据长度
	_declspec(dllexport) int union_syj1001_dectyptData_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char * typeOfAlgo,char* indexOfKey,char *keyValByMK,char*iniVec,int lenOfSData,char *SData,char* DData);
	//对指定数据生成MAC  0030
	//输入参数：typeOfAlgo 算法类型
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：iniVec     初始向量
	//输入参数：lenOfMacData  macData数据长度
	//输入参数：macData 
	//输出参数：mac
	//返回值： <0：失败；>0成功返回的mac数据长度 
	_declspec(dllexport) int union_syj1001_genMac(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char *typeOfAlgo,char * indexOfKey,char*keyValByMK,char*iniVec,int lenOfMacData,char*macData,char*mac);
	//对指定数据生成MAC  S030
	//输入参数：typeOfAlgo 算法类型
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：iniVec     初始向量
	//输入参数：lenOfMacData  macData数据长度
	//输入参数：macData 
	//输出参数：mac
	//返回值： <0：失败；>0成功返回的mac数据长度 
	_declspec(dllexport) int union_syj1001_genMac_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char *typeOfAlgo,char * indexOfKey,char*keyValByMK,char*iniVec,int lenOfMacData,char*macData,char*mac);
	//对指定数据验证MAC 0032
	//输入参数：typeOfAlgo 算法类型
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：iniVec     初始向量
	//输入参数：lenOfMac   mac长度 MAC值中需要进行验证的字节数；最小值为4，最大值16
	//输入参数：mac
	//输入参数：lenOfMacData  macData数据长度
	//输入参数：macData 
	//返回值： <0：失败；=0成功
	_declspec(dllexport) int union_syj1001_verifyMac(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*typeOfAlgo,char*indexOfKey,char*keyValByMK,char*iniVec,int lenOfMac,char*mac,int lenOfMacData,char *macData);
	//对指定数据验证MAC S032
	//输入参数：typeOfAlgo 算法类型
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：iniVec     初始向量
	//输入参数：lenOfMac   mac长度 MAC值中需要进行验证的字节数；最小值为4，最大值16
	//输入参数：mac
	//输入参数：lenOfMacData  macData数据长度
	//输入参数：macData 
	//返回值： <0：失败；=0成功
	_declspec(dllexport) int union_syj1001_verifyMac_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*typeOfAlgo,char*indexOfKey,char*keyValByMK,char*iniVec,int lenOfMac,char*mac,int lenOfMacData,char *macData);
	//用指定密钥对明文PIN进行加密 0020
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：formatOfPinBlock PIN块的格式
	//输入参数：clearPin PIN块的明文
	//输入参数：accNo  账号
	//输出参数：pinByKEK pin密文
	//返回值： <0：失败；>0成功 输出数据长度
	_declspec(dllexport)  int union_syj1001_enctyptPin(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfKey,char*keyValByMK,char*formatOfPinBlock, char*clearPin,char*accNo,char*pinByKEK);
	//用指定密钥对明文PIN进行加密 S020
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：formatOfPinBlock PIN块的格式
	//输入参数：clearPin PIN块的明文
	//输入参数：accNo  账号
	//输出参数：pinByKEK pin密文
	//返回值： <0：失败；>0成功 输出数据长度
	_declspec(dllexport)  int union_syj1001_enctyptPin_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfKey,char*keyValByMK,char*formatOfPinBlock, char*clearPin,char*accNo,char*pinByKEK);
	//用指定密钥对密文PIN进行解密 0024
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：formatOfPinBlock PIN块的格式
	//输入参数：pinByKEK pin密文
	//输入参数：accNo  账号
	//输出参数：clearPin PIN块的明文
	//返回值： <0：失败；=0成功
	_declspec(dllexport)  int union_syj1001_dectyptPin(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfKey,char*keyValByMK,char*formatOfPinBlock, char*pinByKEK,char*accNo,char*clearPin);
	//用指定密钥对密文PIN进行解密 S024
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：formatOfPinBlock PIN块的格式
	//输入参数：pinByKEK pin密文
	//输入参数：accNo  账号
	//输出参数：clearPin PIN块的明文
	//返回值： <0：失败；=0成功
	_declspec(dllexport)  int union_syj1001_dectyptPin_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfKey,char*keyValByMK,char*formatOfPinBlock, char*pinByKEK,char*accNo,char*clearPin);
	//打印PIN/PIN请求数据 00PE
	//输入参数：typeOfFile 文档类型
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：accNo  账号
	//输入参数：pinBlock pin密文
	//输入参数：formatOfPinBlock PIN块的格式
	//输入参数：printFieldNo 在打印格域个数，最多16个
	//输入参数：... 在打印格式定义中作为“Print Field s”定义的打印域
	//返回值： <0：失败；=0成功
	_declspec(dllexport)  int union_syj1001_printPin(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char typeOfFile,char* indexOfKey,char*keyValByMK,char*accNo,char*pinBlock,char*formatOfPinBlock,int printFieldNo,...);
	//打印PIN/PIN请求数据 S0PE
	//输入参数：typeOfFile 文档类型
	//输入参数：indexOfKey 密钥索引
	//输入参数：keyValByMK 密钥值
	//输入参数：accNo  账号
	//输入参数：pinBlock pin密文
	//输入参数：formatOfPinBlock PIN块的格式
	//输入参数：printFieldNo 在打印格域个数，最多16个
	//输入参数：... 在打印格式定义中作为“Print Field s”定义的打印域
	//返回值： <0：失败；=0成功
	_declspec(dllexport)  int union_syj1001_printPin_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char typeOfFile,char* indexOfKey,char*keyValByMK,char*accNo,char*pinBlock,char*formatOfPinBlock,int printFieldNo,...);
	//用指定密钥对离散数据衍生密钥 0064
	//输入参数：indexOfRKey 根密钥索引
	//输入参数：rKeyValByMK 根密钥值
	//输入参数：disperseCount 离散次数
	//输出参数：newKeyVal MK加密下的过程密钥/子密钥密文
	//输出参数：chkVal 过程密钥/子密钥的校验值
	//输入参数：... 离散数据
	//返回值： <0：失败；>0成功 密钥长度
	_declspec(dllexport)  int union_syj1001_disperseKey(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfRKey,char* rKeyValByMK,char*newKeyVal,char*chkVal,int disperseCount,...);
	//用指定密钥对离散数据衍生密钥 S064
	//输入参数：indexOfRKey 根密钥索引
	//输入参数：rKeyValByMK 根密钥值
	//输入参数：disperseCount 离散次数
	//输出参数：newKeyVal MK加密下的过程密钥/子密钥密文
	//输出参数：chkVal 过程密钥/子密钥的校验值
	//输入参数：... 离散数据
	//返回值： <0：失败；>0成功 密钥长度
	_declspec(dllexport)  int union_syj1001_disperseKey_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfRKey,char* rKeyValByMK,char*newKeyVal,char*chkVal,int disperseCount,...);
};