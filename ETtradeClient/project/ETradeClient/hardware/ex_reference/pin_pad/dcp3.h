#ifndef DCP3_H_
#define DCP3_H_

#ifdef __cplusplus
extern "C" {
#endif
	
	//des slgorythm.
	__int16 __stdcall P3_IC_Encrypt( char *key, char *ptrSource, unsigned __int16 msgLen, char *ptrDest);
	__int16 __stdcall P3_IC_Decrypt( char *key, char *ptrSource, unsigned __int16 msgLen, char *ptrDest);
	__int16 __stdcall P3_IC_Encrypt_Hex( char *key, char *ptrSource, unsigned __int16 msgLen, char *ptrDest);
	__int16 __stdcall P3_IC_Decrypt_Hex( char *key, char *ptrSource, unsigned __int16 msgLen, char *ptrDest);
	__int16 __stdcall P3_IC_Encrypt_Neu( char *key, char *ptrSource, unsigned __int16 msgLen, char *ptrDest);
	__int16 __stdcall P3_IC_Decrypt_Neu( char *key, char *ptrSource, unsigned __int16 msgLen, char *ptrDest);
	__int16 __stdcall P3_IC_Status_1(HANDLE idComDev);
	//
	HANDLE __stdcall P3_IC_InitComm(__int16 port);
	HANDLE __stdcall P3_IC_InitCommAdvanced(__int16 port);
	__int16 __stdcall P3_IC_ExitComm(HANDLE idComDev);
	__int16 __stdcall P3_IC_Status(HANDLE idComDev);
	__int16 __stdcall P3_IC_Down(HANDLE idComDev);
	
	__int16 __stdcall P3_IC_Reset(HANDLE idComDev);
	
	__int16 __stdcall P3_IC_ReadVer(HANDLE idComDev, unsigned char *Ver);
	__int16 __stdcall P3_IC_DevBeep(HANDLE idComDev, unsigned char beeptime);
	__int16 __stdcall P3_IC_Beep(HANDLE idComDev, unsigned char delaytime, unsigned char beeptime);
	__int16 __stdcall P3_IC_ReadDevice(HANDLE idComDev, __int16 offset, __int16 len, unsigned char * databuffer);
	__int16 __stdcall P3_IC_WriteDevice(HANDLE idComDev, __int16 offset, __int16 len, unsigned char * writebuffer);
	__int16 __stdcall P3_IC_ReadDevice_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char * databuffer);
	__int16 __stdcall P3_IC_WriteDevice_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char * writebuffer);
	
	//
	__int16 __stdcall P3_IC_CpuApduSourceEXT(HANDLE idComDev, __int16 slen, unsigned char * sendbuffer, __int16 *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_IC_CpuApduSourceEXT_Hex(HANDLE idComDev, __int16 slen, unsigned char * sendbuffer, __int16 *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_IC_CpuApduEXT(HANDLE idComDev, __int16 slen, unsigned char * sendbuffer, __int16 *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_IC_CpuApduEXT_Hex(HANDLE idComDev, __int16 slen, unsigned char * sendbuffer, __int16 *rlen, unsigned char * databuffer);
	
	__int16 __stdcall P3_IC_CpuReset(HANDLE idComDev, unsigned char *rlen, unsigned char *databuffer);
	__int16 __stdcall P3_IC_CpuApdu(HANDLE idComDev, unsigned char slen, unsigned char * sendbuffer, unsigned char *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_IC_CpuApduSource(HANDLE idComDev, unsigned char slen, unsigned char * sendbuffer, unsigned char *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_IC_CpuReset_Hex(HANDLE idComDev, unsigned char *rlen, unsigned char *databuffer);
	__int16 __stdcall P3_IC_CpuApdu_Hex(HANDLE idComDev, unsigned char slen, unsigned char * sendbuffer, unsigned char *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_IC_CpuApduSource_Hex(HANDLE idComDev, unsigned char slen, unsigned char * sendbuffer, unsigned char *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_IC_CpuGetProtocol(HANDLE idComDev);
	__int16 __stdcall P3_IC_CpuApduRespon(HANDLE idComDev, unsigned char slen, unsigned char * sendbuffer, unsigned char *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_IC_CpuApduRespon_Hex(HANDLE idComDev, unsigned char slen, unsigned char * sendbuffer, unsigned char *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_IC_CpuSetProtocol(HANDLE idComDev, unsigned char cardp);
	
	
	//
	__int16 __stdcall P3_IC_Pushout(HANDLE idComDev);
	__int16 __stdcall P3_IC_InitType (HANDLE idComDev, __int16 type);
	__int16 __stdcall P3_IC_Read(HANDLE idComDev, __int16 offset, __int16 len, unsigned char * databuffer);
	__int16 __stdcall P3_IC_Read_Float(HANDLE idComDev, __int16 offset, float *fdata);
	__int16 __stdcall P3_IC_Read_Int(HANDLE idComDev, __int16 offset, long *fdata);
	__int16 __stdcall P3_IC_Write_Int(HANDLE idComDev, __int16 offset, long fdata);
	__int16 __stdcall P3_IC_ReadProtection(HANDLE idComDev, __int16 offset, __int16 len, unsigned char* protbuffer);
	__int16 __stdcall P3_IC_ReadProtection_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char* protbuffer);
	__int16 __stdcall P3_IC_ReadWithProtection(HANDLE idComDev, __int16 offset, __int16 len, unsigned char* protbuffer);
	__int16 __stdcall P3_IC_ReadWithProtection_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char* protbuffer);
	__int16 __stdcall P3_IC_Write(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_Write_Float(HANDLE idComDev, __int16 offset, float fdata);
	__int16 __stdcall P3_IC_WriteProtection(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *protbuffer);
	__int16 __stdcall P3_IC_WriteWithProtection(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_WriteProtection_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *protbuffer);
	__int16 __stdcall P3_IC_WriteWithProtection_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char * writebuffer);
	
	__int16 __stdcall P3_IC_ReadPass_SLE4442(HANDLE idComDev, unsigned char*password);
	__int16 __stdcall P3_IC_ReadCount_SLE4442(HANDLE idComDev);
	__int16 __stdcall P3_IC_CheckPass_SLE4442(HANDLE idComDev, unsigned char* password);
	__int16 __stdcall P3_IC_ChangePass_SLE4442(HANDLE idComDev, unsigned char*password);
	//
	__int16 __stdcall P3_IC_CheckPass_4442hex(HANDLE idComDev, unsigned char*password);
	__int16 __stdcall P3_IC_ChangePass_4442hex(HANDLE idComDev, unsigned char*password);
	__int16 __stdcall P3_IC_ReadPass_4442hex(HANDLE idComDev, unsigned char*password);
	//
	__int16 __stdcall P3_IC_ReadCount_SLE4428(HANDLE idComDev);
	__int16 __stdcall P3_IC_CheckPass_SLE4428(HANDLE idComDev, unsigned char* password);
	__int16 __stdcall P3_IC_ChangePass_SLE4428(HANDLE idComDev, unsigned char* password);
	//
	__int16 __stdcall P3_IC_CheckPass_4428hex(HANDLE idComDev, unsigned char* password);
	__int16 __stdcall P3_IC_ChangePass_4428hex(HANDLE idComDev, unsigned char* password);
	//
	__int16 __stdcall P3_IC_Erase(HANDLE idComDev, __int16 offset, __int16 len);
	__int16 __stdcall P3_IC_Erase_102(HANDLE idComDev, __int16 offset, __int16 len);
	__int16 __stdcall P3_IC_Fuse_102(HANDLE idComDev);
	__int16 __stdcall P3_IC_ReadCount_102(HANDLE idComDev);
	
	__int16 __stdcall P3_IC_CheckPass_102(HANDLE idComDev, unsigned char*password);
	__int16 __stdcall P3_IC_ChangePass_102(HANDLE idComDev, unsigned char*password);
	__int16 __stdcall P3_IC_CheckAZPass_102(HANDLE idComDev, __int16 zone, unsigned char*password);
	__int16 __stdcall P3_IC_ChangeAZPass_102(HANDLE idComDev, __int16 zone, unsigned char*password);
	//
	__int16 __stdcall P3_IC_CheckPass_102hex(HANDLE idComDev, unsigned char*password);
	__int16 __stdcall P3_IC_ChangePass_102hex(HANDLE idComDev, unsigned char*password);
	__int16 __stdcall P3_IC_CheckAZPass_102hex(HANDLE idComDev, __int16 zone, unsigned char*password);
	__int16 __stdcall P3_IC_ChangeAZPass_102hex(HANDLE idComDev, __int16 zone, unsigned char*password);
	
	__int16 __stdcall P3_IC_Fuse_1604(HANDLE idComDev);
	__int16 __stdcall P3_IC_ReadCount_1604(HANDLE idComDev, __int16 area);
	__int16 __stdcall P3_IC_CheckPass_1604(HANDLE idComDev, __int16 area, unsigned char*password);
	__int16 __stdcall P3_IC_ChangePass_1604(HANDLE idComDev, __int16 area, unsigned char*password);
	//
	__int16 __stdcall P3_IC_CheckPass_1604hex(HANDLE idComDev, __int16 area, unsigned char*password);
	__int16 __stdcall P3_IC_ChangePass_1604hex(HANDLE idComDev, __int16 area, unsigned char*password);
	//4404
	__int16 __stdcall P3_IC_Fuse_4404(HANDLE icdev);
	__int16 __stdcall P3_IC_ReadCount_4404(HANDLE icdev);
	__int16 __stdcall P3_IC_CheckPass_4404(HANDLE idComDev, unsigned char *password);
	__int16 __stdcall P3_IC_ChangePass_4404(HANDLE idComDev, unsigned char *password);
	__int16 __stdcall P3_IC_CheckAZPass_4404(HANDLE idComDev, unsigned char *password);
	__int16 __stdcall P3_IC_ChangeAZPass_4404(HANDLE idComDev, unsigned char*password);
	__int16 __stdcall P3_IC_CheckPass_4404hex(HANDLE idComDev, unsigned char *password);
	__int16 __stdcall P3_IC_ChangePass_4404hex(HANDLE idComDev, unsigned char *password);
	__int16 __stdcall P3_IC_CheckAZPass_4404hex(HANDLE idComDev, unsigned char *password);
	__int16 __stdcall P3_IC_ChangeAZPass_4404hex(HANDLE idComDev, unsigned char*password);
	
	//4406
	__int16 __stdcall P3_IC_RValue(HANDLE idComDev);
	__int16 __stdcall P3_IC_DEValue(HANDLE idComDev, __int16 num);
	__int16 __stdcall P3_IC_Fuse_4406(HANDLE icdev, __int16 value);
	__int16 __stdcall P3_IC_ReadCount_4406(HANDLE icdev);
	__int16 __stdcall P3_IC_CheckPass_4406(HANDLE idComDev, unsigned char *password);
	
	__int16 __stdcall P3_IC_Erase_4406(HANDLE idComDev, unsigned char offset);
	__int16 __stdcall P3_IC_CheckPass_4406hex(HANDLE idComDev, unsigned char *password);
	__int16 __stdcall P3_IC_CheckPass_4406userhex(HANDLE idComDev, unsigned char *password);
	__int16 __stdcall P3_IC_CheckPass_4406user(HANDLE idComDev, unsigned char *password);
	
	//
	__int16 __stdcall P3_IC_Write24(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_Write64(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	//hex read and write.
	__int16 __stdcall P3_IC_Write_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_Read_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char * databuffer);
	__int16 __stdcall P3_IC_Write24_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_Write64_Hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	//
	__int16 __stdcall P3_IC_DirectRead(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *buff);
	__int16 __stdcall P3_IC_WriteByBuffer(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *buff);
	__int16 __stdcall P3_IC_DirectWrite(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *buff);
	//
	__int16 __stdcall P3_IC_DirectRead_Hex(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *buff);
	__int16 __stdcall P3_IC_DirectWrite_Hex(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_WriteByBuffer_Hex(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *writebuffer);
	//
	__int16 __stdcall P3_IC_Fuse_1604B(HANDLE idComDev);
	__int16 __stdcall P3_IC_ReadCount_1604B(HANDLE idComDev, __int16 area);
	__int16 __stdcall P3_IC_ChangePass_1604B(HANDLE idComDev, __int16 area, unsigned char*password);
	__int16 __stdcall P3_IC_CheckPass_1604B(HANDLE idComDev, __int16 area, unsigned char*password);
	__int16 __stdcall P3_IC_ChangePass_1604Bhex(HANDLE idComDev, __int16 area, unsigned char*password);
	__int16 __stdcall P3_IC_CheckPass_1604Bhex(HANDLE idComDev, __int16 area, unsigned char*password);
	//
	__int16 __stdcall P3_IC_Read_1101(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *buff);
	__int16 __stdcall P3_IC_Write_1101(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *buff);
	__int16 __stdcall P3_IC_Read_1101hex(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *buff);
	__int16 __stdcall P3_IC_Write_1101hex(HANDLE idComDev, __int16 page, __int16 offset, __int16 bytes, unsigned char *buff);
	
	__int16 __stdcall P3_IC_GotoUpdate (HANDLE idComDev);       //for download function
	//AT88SC1608 卡的专用函数
	
	__int16 __stdcall P3_IC_InitAuth_Hex(HANDLE idComDev, unsigned char *Q0);
	__int16 __stdcall P3_IC_CheckAuth_Hex(HANDLE idComDev, unsigned char *Q1);
	__int16 __stdcall P3_IC_InitAuth(HANDLE idComDev, unsigned char *Q0);
	__int16 __stdcall P3_IC_CheckAuth(HANDLE idComDev, unsigned char *Q1);
	__int16 __stdcall P3_IC_CheckRPassword_Hex(HANDLE idComDev, unsigned char zone, unsigned char *Pin);
	__int16 __stdcall P3_IC_CheckWPassword_Hex(HANDLE idComDev, unsigned char zone, unsigned char *Pin);
	__int16 __stdcall P3_IC_CheckRPassword(HANDLE idComDev, unsigned char zone, unsigned char *Pin);
	__int16 __stdcall P3_IC_CheckWPassword(HANDLE idComDev, unsigned char zone, unsigned char *Pin);
	__int16 __stdcall P3_IC_ChangeRPassword(HANDLE idComDev, unsigned char zone, unsigned char *Pin);
	__int16 __stdcall P3_IC_ChangeWPassword(HANDLE idComDev, unsigned char zone, unsigned char *Pin);
	__int16 __stdcall P3_IC_ChangeRPassword_Hex(HANDLE idComDev, unsigned char zone, unsigned char *Pin);
	__int16 __stdcall P3_IC_ChangeWPassword_Hex(HANDLE idComDev, unsigned char zone, unsigned char *Pin);
	__int16 __stdcall P3_IC_WriteFuse(HANDLE idComDev, unsigned char fusetype);
	__int16 __stdcall P3_IC_ReadFuse(HANDLE idComDev);
	__int16 __stdcall P3_IC_WriteConfigZone(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *WDataBuff);
	__int16 __stdcall P3_IC_WriteUserZone(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *WDataBuff);
	__int16 __stdcall P3_IC_ReadConfigZone(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *RDataBuff);
	__int16 __stdcall P3_IC_ReadUserZone(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *RDataBuff);
	__int16 __stdcall P3_IC_ReadWPasswordCount(HANDLE idComDev, unsigned char zone);
	__int16 __stdcall P3_IC_ReadRPasswordCount(HANDLE idComDev, unsigned char zone);
	
	__int16 __stdcall P3_IC_ReadAuthCount(HANDLE idComDev);
	__int16 __stdcall P3_IC_WriteConfigZone_Hex(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *WDataBuff);
	__int16 __stdcall P3_IC_WriteUserZone_Hex(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *WDataBuff);
	__int16 __stdcall P3_IC_ReadConfigZone_Hex(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *RDataBuff);
	__int16 __stdcall P3_IC_ReadUserZone_Hex(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *RDataBuff);
	
	__int16 __stdcall IC_SetUserZone(HANDLE idComDev, unsigned char Zone);
	
	
	//AT88SC1608的专用算法程序，资料中介绍为F2算法。
	__int16 __stdcall P3_SetInit(unsigned char *Ci, unsigned char *Gc, unsigned char *Q0);
	__int16 __stdcall P3_Authenticate(unsigned char *Q1, unsigned char *Q2);
	//可以使用16进制的数据传输
	__int16 __stdcall P3_SetInit_Hex(unsigned char *Ci, unsigned char *Gc, unsigned char *Q0);
	__int16 __stdcall P3_Authenticate_Hex(unsigned char *Q1, unsigned char *Q2);
	
	/*
	认证的使用过程：
	1。产生Q0为一个随机数。
	2。使用IC_InitAuth（Q0）将Q0送入卡中。
	3。从卡中读出Ci用于下一步的密钥初始化。
	4。使用SetInit(Ci，Gc，Q0), 其中Gc为用户密钥，由用于掌握，必须和卡中的Gc相同，Q0在第一步中已经产生。
	5。使用Authenticate（Q1，Q2）产生Q1，Q2，用于下一步的认证过程
	6。使用IC_CheckAuth（Q1）将Q1送入卡中认证。
	7。从卡中读出Ci和Q2比较，如果相同则认证通过。
	
	  注意：在此过程中Gc为用户已知密钥
	*/
	//2个工具函数，用于字符串格式的转换
	__int16 __stdcall P3_hex2asc(unsigned char *strhex, unsigned char *strasc, __int16 length);
	__int16 __stdcall P3_asc2hex(unsigned char *strasc, unsigned char *strhex, __int16 length);
	
	__int16 __stdcall P3_SendData(HANDLE idComDev, __int16 len, unsigned char * databuffer);
	__int16 __stdcall P3_ReceiveData(HANDLE idComDev, __int16 len, unsigned char *databuffer);
	
	
	unsigned __int16 __stdcall P3_ICC_Internal_Auth(HANDLE idComDev, unsigned char kid, unsigned char *randifd, unsigned char retlen, unsigned char *encrand);
	unsigned __int16 __stdcall P3_ICC_External_Auth(HANDLE idComDev, unsigned char kid, unsigned char *encrand);
	unsigned __int16 __stdcall P3_ICC_Verify(HANDLE idComDev, unsigned char kid, unsigned char pin_len, unsigned char *pin);
	unsigned __int16 __stdcall P3_ICC_Write_Bin(HANDLE idComDev, unsigned __int16 offset, unsigned __int16 len, unsigned char *data);
	unsigned __int16 __stdcall P3_ICC_Read_Bin(HANDLE idComDev, unsigned __int16 offset, unsigned __int16 len, unsigned char *resp);
	unsigned __int16 __stdcall P3_ICC_Select_File(HANDLE idComDev, unsigned char sflag, unsigned __int16 fid);
	unsigned __int16 __stdcall P3_ICC_Get_Challenge(HANDLE idComDev, unsigned char len, unsigned char *rand);
	
	__int16 __stdcall P3_IC_ReadUsbSnr(HANDLE idComDev, unsigned char *snrdata);
	
	//BOOL __stdcall RegisterCallback(LPRD102_CALLBACK_FUNC lpfnCallback);
	__int16 __stdcall P3_IC_Control(HANDLE idComDev, unsigned char ctype, unsigned char delaytime);
	
	
	
	__int16 __stdcall P3_IC_CheckCard(HANDLE idComDev);
	
	__int16 __stdcall P3_IC_Check_4442(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_4428(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_102(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_1604(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_1604B(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_24C01(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_24C02(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_24C04(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_24C08(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_24C16(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_24C64(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_45DB041(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_1101(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_CPU(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_153(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_1608(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_4404(HANDLE idComDev);
	__int16 __stdcall P3_IC_Check_4406(HANDLE idComDev);
	
	HANDLE __stdcall P3_IC_InitComm_Baud(__int16 port, unsigned long combaud);
	__int16 __stdcall P3_IC_SetCommTimeout(DWORD lTime_ms, DWORD sTime_ms);
	__int16 __stdcall P3_IC_Write_102(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_Write_102hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_Write_1604(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_Write_1604hex(HANDLE idComDev, __int16 offset, __int16 len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_SetCpuPara(HANDLE idComDev, unsigned char cputype, unsigned char cpupro, unsigned char cpuetu);
	__int16 __stdcall P3_IC_CpuColdReset(HANDLE idComDev, unsigned char *rlen, unsigned char *databuffer);
	__int16 __stdcall P3_IC_CpuColdReset_Hex(HANDLE idComDev, unsigned char *rlen, unsigned char *databuffer);
	__int16 __stdcall P3_IC_CpuHotReset(HANDLE idComDev, unsigned char *rlen, unsigned char *databuffer);
	__int16 __stdcall P3_IC_CpuHotReset_Hex(HANDLE idComDev, unsigned char *rlen, unsigned char *databuffer);
	
	__int16 __stdcall P3_IC_SwitchPcsc(HANDLE idComDev, unsigned char flag);
	__int16 __stdcall P3_DEV_CommandMcu(HANDLE idComDev, unsigned char ctimeout, unsigned char slen, unsigned char * sendbuffer, unsigned char *rlen, unsigned char * databuffer);
	__int16 __stdcall P3_DEV_CommandMcu_Hex(HANDLE idComDev, unsigned char ctimeout, unsigned char slen, unsigned char * sendbuffer, unsigned char *rlen, unsigned char * databuffer);
	
	__int16 __stdcall P3_IC_DispLcd(HANDLE idComDev, unsigned char flag);
	__int16 __stdcall P3_IC_GetInputPass(HANDLE idComDev, unsigned char ctime, unsigned char *rlen, unsigned char * cpass);
	__int16 __stdcall P3_IC_ReadMagCard(HANDLE idComDev, unsigned char ctime, unsigned char *pTrack2Data, unsigned long *pTrack2Len, unsigned char *pTrack3Data, unsigned long *pTrack3Len);
	__int16 __stdcall P3_IC_ReadMagCardAll(HANDLE idComDev, unsigned char ctime, unsigned char *pTrack1Data, unsigned long *pTrack1Len, unsigned char *pTrack2Data, unsigned long *pTrack2Len, unsigned char *pTrack3Data, unsigned long *pTrack3Len);
	__int16 __stdcall P3_IC_TestDeviceComm(HANDLE idComDev);
	__int16 __stdcall P3_IC_DispMainMenu(HANDLE idComDev);
	__int16 __stdcall P3_IC_SetDeviceTime(HANDLE idComDev, 
		unsigned char year, 
		unsigned char month, 
		unsigned char date, 
		unsigned char hour, 
		unsigned char minute, 
		unsigned char second);
	__int16 __stdcall P3_IC_GetDeviceTime(HANDLE idComDev, 
		unsigned char *year, 
		unsigned char *month, 
		unsigned char *date, 
		unsigned char *hour, 
		unsigned char *minute, 
		unsigned char *second);
	__int16 __stdcall P3_IC_DispInfoEn(HANDLE idComDev, unsigned char line, unsigned char offset, char *data);
	__int16 __stdcall P3_IC_DispInfo(HANDLE idComDev, unsigned char line, unsigned char offset = 0x00, char *data = NULL);
	__int16 __stdcall P3_IC_DispMainInfo(HANDLE idComDev, unsigned char offset, char *data);
	__int16 __stdcall P3_IC_PosBeep(HANDLE idComDev, unsigned char beeptime);
	
	__int16 __stdcall P3_IC_WriteDeviceEn(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *writebuffer, unsigned char *pass);
	__int16 __stdcall P3_IC_ReadDeviceEn(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *readbuffer, unsigned char *pass);
	
	__int16 __stdcall P3_IC_WriteDeviceEn_Hex(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *writebuffer, unsigned char *pass);
	__int16 __stdcall P3_IC_ReadDeviceEn_Hex(HANDLE idComDev, __int16 Offset, __int16 len, unsigned char *readbuffer, unsigned char *pass);
	__int16 __stdcall P3_DEV_SetControl(HANDLE idComDev, unsigned char Controlp);
	
	__int16 __stdcall P3_IC_ReadDevSnr(HANDLE idComDev, unsigned char *snr);
	__int16 __stdcall P3_IC_ReadDevSnr_Hex(HANDLE idComDev, unsigned char *snr);
	
	__int16 __stdcall P3_IC_SetUsbTimeout(unsigned char ntimes);
	
	__int16 __stdcall P3_IC_CtlBackLight(HANDLE idComDev, unsigned char cOpenFlag);
	__int16 __stdcall P3_IC_CtlLed(HANDLE idComDev, unsigned char cLed, unsigned char cOpenFlag);
	__int16 __stdcall P3_IC_LcdClrScrn(HANDLE idComDev, unsigned char cLine);
	
	__int16 __stdcall P3_IC_SetReaderTime(HANDLE idComDev, 
		unsigned char year, 
		unsigned char month, 
		unsigned char date, 
		unsigned char hour, 
		unsigned char minute, 
		unsigned char second);
	
	__int16 __stdcall P3_IC_GetReaderTime(HANDLE idComDev, 
		unsigned char *year, 
		unsigned char *month, 
		unsigned char *date, 
		unsigned char *hour, 
		unsigned char *minute, 
		unsigned char *second);
	
	__int16 __stdcall P3_IC_PassIn(HANDLE idComDev, unsigned char ctime);
	__int16 __stdcall P3_IC_PassGet(HANDLE idComDev, unsigned char *rlen, unsigned char * cpass);
	__int16 __stdcall P3_IC_PassCancel(HANDLE idComDev);
	__int16 __stdcall P3_IC_GetInputKey(HANDLE idComDev, unsigned char disptype, unsigned char line, 
		unsigned char ctime, unsigned char *rlen, unsigned char * ckeydata);
	
	
	__int16 __stdcall P3_IC_Request(HANDLE idComDev, BYTE _bMode, __int16 *_wTagType);
	__int16 __stdcall P3_IC_Anticoll(HANDLE idComDev, BYTE _bBcnt, DWORD *_dwSnr);
	__int16 __stdcall P3_IC_Select(HANDLE idComDev, DWORD _dwSnr, BYTE *_bSize);
	__int16 __stdcall P3_IC_Authentication(HANDLE idComDev, BYTE _bMode, BYTE _bSecNr);
	__int16 __stdcall P3_IC_Halt(HANDLE idComDev);
	__int16 __stdcall P3_IC_ReadMifare(HANDLE idComDev, BYTE _bAdr, BYTE *_bData);
	__int16 __stdcall P3_IC_ReadMifare_Hex(HANDLE idComDev, BYTE _bAdr, BYTE *_bData);
	__int16 __stdcall P3_IC_WriteMifare(HANDLE idComDev, BYTE _bAdr, BYTE *_bData);
	__int16 __stdcall P3_IC_WriteMifare_Hex(HANDLE idComDev, BYTE _bAdr, BYTE *_bData);
	__int16 __stdcall P3_IC_Transfer(HANDLE idComDev, BYTE _bAdr);
	__int16 __stdcall P3_IC_Restore(HANDLE idComDev, BYTE _bAdr);
	__int16 __stdcall P3_IC_Increment(HANDLE idComDev, BYTE _bAdr, DWORD _dwValue);
	__int16 __stdcall P3_IC_Decrement(HANDLE idComDev, BYTE _bAdr, DWORD _dwValue);
	__int16 __stdcall P3_IC_Load_Key(HANDLE idComDev, BYTE _bMode, BYTE _bSecNr, unsigned char *_bNKey);
	__int16 __stdcall P3_IC_Load_Keyhex(HANDLE idComDev, BYTE _bMode, BYTE _bSecNr, unsigned char *_bNKey);
	__int16 __stdcall P3_IC_ResetMifare(HANDLE idComDev, __int16 _wMsec);
	__int16 __stdcall P3_IC_Init_Value(HANDLE idComDev, BYTE _bAdr);
	__int16 __stdcall P3_IC_Read_Value(HANDLE idComDev, BYTE _bAdr, long *_lValue);
	__int16 __stdcall P3_IC_Card(HANDLE icdev, unsigned char _Mode, unsigned long *_Snr);
	__int16 __stdcall P3_IC_Card_Hex(HANDLE icdev, unsigned char _Mode, unsigned char *snrstr);
	__int16 __stdcall P3_IC_Authentication_Passaddr(HANDLE icdev, unsigned char _Mode, 
		unsigned char _Addr, unsigned char *passbuff);
	__int16 __stdcall P3_IC_Authentication_Passaddrhex(HANDLE icdev, unsigned char _Mode, 
		unsigned char _Addr, unsigned char *passbuff);												
	
	__int16 __stdcall P3_IC_ReadReaderSnr(HANDLE idComDev, unsigned char *snr);
	__int16 __stdcall P3_IC_ResetDevice(HANDLE idComDev);
	
	__int16 __stdcall P3_IC_TypeA_Cpu_Reset_Hex(HANDLE icdev, unsigned char _Mode, unsigned char *rlen, unsigned char *receive_data);
	__int16 __stdcall P3_IC_TypeA_Cpu_Reset(HANDLE icdev, unsigned char _Mode, unsigned char *rlen, unsigned char *receive_data);
	
	__int16 __stdcall P3_IC_Pro_Reset(HANDLE icdev, unsigned char *rlen, unsigned char *receive_data);
	__int16 __stdcall P3_IC_Pro_Commandsource(HANDLE idComDev, unsigned char slen, 
		unsigned char * sendbuffer, unsigned char *rlen, 
		unsigned char * databuffer, unsigned char timeout);
	__int16 __stdcall P3_IC_Config_Card(HANDLE icdev, unsigned char _Mode);
	__int16 __stdcall P3_IC_Request_B(HANDLE icdev, unsigned char _Mode, 
		unsigned char AFI, 
		unsigned char N, 
		unsigned char *ATQB);
	__int16 __stdcall P3_IC_Attrib(HANDLE icdev, unsigned char *PUPI, unsigned char CID);
	__int16 __stdcall P3_IC_Anticoll2(HANDLE idComDev, BYTE _bBcnt, DWORD *_dwSnr);
	__int16 __stdcall P3_IC_Select2(HANDLE idComDev, DWORD _dwSnr, BYTE *_bSize);
	__int16 __stdcall P3_IC_SetUldPpsMode(HANDLE idComDev, unsigned char bs);
	
	
	//-----------------------------以下为特殊用途的定制函数，可以根据需要选用或者删除-------------------
	//----------------------------------------------------------------------------------------------
	//山东社保神思定制读写器函数两条。 SD_SendToPos 
	__int16 __stdcall P3_SD_SendToPos(HANDLE idComDev, unsigned char pplen, unsigned char * ppdata);
	__int16 __stdcall P3_SD_GetFromPos(HANDLE idComDev, unsigned char * rlen, unsigned char * ppdata, unsigned char timeout);
	
	//
	__int16 __stdcall P3_IC_GetUsbID(unsigned int *VendorID, unsigned int *ProductID);
	
	__int16 __stdcall P3_IC_Pro_Resethex(HANDLE icdev, unsigned char *rlen, unsigned char *receive_data);
	__int16 __stdcall P3_IC_Pro_Commandsourcehex(HANDLE idComDev, unsigned char slen, 
		unsigned char * sendbuffer, unsigned char *rlen, 
		unsigned char * databuffer, unsigned char timeout);
	__int16 __stdcall P3_IC_Request_Bhex(HANDLE icdev, unsigned char _Mode, 
		unsigned char AFI, 
		unsigned char N, 
		unsigned char *ATQB);
	__int16 __stdcall P3_IC_Attrib_Hex(HANDLE icdev, unsigned char *PUPI, unsigned char CID);
	__int16 __stdcall P3_IC_Pro_Commandlink(HANDLE idComDev, unsigned int slen, 
		unsigned char * sendbuffer, unsigned int *rlen, 
		unsigned char * databuffer, unsigned char timeout, 
		unsigned char FG);
	__int16 __stdcall P3_IC_Pro_Commandlinkhex(HANDLE idComDev, unsigned int slen, 
		unsigned char * sendbuffer, unsigned int *rlen, 
		unsigned char * databuffer, unsigned char timeout, 
		unsigned char FG);
	__int16 __stdcall P3_IC_InstallKbKey(HANDLE idComDev,unsigned char flag,unsigned char keylen=0, 
								unsigned char * keydata=NULL);
	__int16 __stdcall P3_IC_GetEnPass(HANDLE idComDev, unsigned char ctime, unsigned char panlen, unsigned char * pandata, 
		unsigned char *rlen, unsigned char * cpass);
	__int16 __stdcall P3_IC_CreatMacData(HANDLE idComDev, unsigned short DataLen, unsigned char *SourceData, unsigned char *MacData);
	__int16 __stdcall P3_IC_SetKbKeyID(HANDLE idComDev, unsigned char keyID);
	
	__int16 __stdcall P3_DEV_SendCommandMcu(HANDLE idComDev, unsigned char ctimeout, unsigned char slen, unsigned char * sendbuffer);
	__int16 __stdcall P3_DEV_SendCommandMcu_Hex(HANDLE idComDev, unsigned char ctimeout, unsigned char slen, unsigned char * sendbuffer);
	
	__int16 __stdcall P3_IC_Write1024(HANDLE idComDev, unsigned long offset, unsigned long len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_Read1024(HANDLE idComDev, unsigned long offset, unsigned long len, unsigned char * databuffer);
	__int16 __stdcall P3_IC_Read1024_Hex(HANDLE idComDev, unsigned long offset, unsigned long len, unsigned char * databuffer);
	__int16 __stdcall P3_IC_Write1024_Hex(HANDLE idComDev, unsigned long offset, unsigned long len, unsigned char *writebuffer);
	__int16 __stdcall P3_IC_PlayVoice(HANDLE idComDev, unsigned char _modeVoice);
	__int16 __stdcall P3_IC_CreatDesData(HANDLE idComDev,unsigned char keytype,unsigned char enmode,
								unsigned char * sourcedata,unsigned char * retdata);
	__int16 __stdcall P3_IC_SetKeyboardLcdShowMode(HANDLE idComDev, unsigned char _modeshowKeyboard);//设置键盘输入显示模式
	__int16 __stdcall P3_IC_SetInPutKeyLength(HANDLE idComDev, unsigned char keylength);//设置用户输入密码长度
	__int16 __stdcall P3_IC_SetKeyLocation(HANDLE idComDev, unsigned char line, unsigned char offset);//设置用户输入密码位置
	__int16 P3_IC_GetMainKey(HANDLE idComDev, unsigned char *EnMainKey,int *EnMainKeyLength);
#ifdef __cplusplus
}
#endif

#endif // DCIC32_H_
