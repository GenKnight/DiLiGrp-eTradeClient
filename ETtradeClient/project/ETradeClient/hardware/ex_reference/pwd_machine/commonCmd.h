/* 
 *  Copyright (c) 2011,  ���ݽ��Ͽ���union  
 *  All rights reserved. 
 *  �ļ����ƣ� commonCmd.h
 *  ժ Ҫ��    Ӧ�ü��ܻ�ͨ��ָ��ӿڵĶ���  
 *  ��ǰ�汾�� 1.0,  ��д��: ���䣬�޸�ʱ��: 2011-11-22 �޸�����: ����
 *  
 */
extern "C" 
{
	
	// װ�ش�ӡ��ʽ 0062
	//���������hsmIp ���ܻ�IP��ַ
	//���������hsmPort ���ܻ��������˿�
	//���������timeout socket��ʱ�ȴ�ʱ�� ms��λ
	//���������isCfg 0:���������ļ���ȡlenOfMsg��hsmIp��hsmPort��timeOut��hsmLenOfMsgHeader��isLenOfHsmMsg ������������е���Щ��Ϣ 1���������ļ��ж�ȡ������Ϣ
	//���������clientSocket -1:���õ��Ƕ����ӷ�ʽ����Ҫ�ں��������socket�Ĵ������ͷ� >0:���ó����ӷ�ʽ�����ø��׽��־��
	//���������hsmLenOfMsgHeader ��Ϣͷ����
	//���������isLenOfHsmMsg ���ĳ��ȣ����ֽڣ�
	//���������typeOfFormat ��ʽ���� P:��ӡPIN��ʽ K:��ӡ��Կ�ɷָ�ʽ
	//���������lenOfFormat ��ʽ���� С��512
	//���������formatInfo ��ʽ����
	//����ֵ�� <0��ʧ�ܣ�=0�ɹ�
	_declspec(dllexport) int union_syj1001_storePrintFormat(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char typeOfFormat,int lenOfFormat,char * formatInfo);
	//��ָ����Կ��ָ�����ݽ��м��� A050
	//���������typeOfAlgo �㷨����
	//���������indexOfKey ������Կ����
	//���������keyValByMK ������Կֵ
	//���������iniVec ��ʼ����
	//���������lenOfSData ���ݿ鳤��
	//���������SData ���ݿ�
	//���������DData ���ܺ����ݿ�
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ� �������ݳ���
	_declspec(dllexport) int union_syj1001_enctyptData(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char * typeOfAlgo,char* indexOfKey,char *keyValByMK,char*iniVec,int lenOfSData,char *SData,char* DData);
	//��ָ����Կ��ָ�����ݽ��м��� S050
	//���������typeOfAlgo �㷨����
	//���������indexOfKey ������Կ����
	//���������keyValByMK ������Կֵ
	//���������iniVec ��ʼ����
	//���������lenOfSData ���ݿ鳤��
	//���������SData ���ݿ�
	//���������DData ���ܺ����ݿ�
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ� �������ݳ���
	_declspec(dllexport) int union_syj1001_enctyptData_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char * typeOfAlgo,char* indexOfKey,char *keyValByMK,char*iniVec,int lenOfSData,char *SData,char* DData);
	//��ָ����Կ��ָ�����ݽ��н��� A050
	//���������typeOfAlgo �㷨����
	//���������indexOfKey ������Կ����
	//���������keyValByMK ������Կֵ
	//���������iniVec ��ʼ����
	//���������lenOfSData ���ݿ鳤��
	//���������SData ���ݿ�
	//���������DData ���ܺ����ݿ�
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ� �������ݳ���
	_declspec(dllexport) int union_syj1001_dectyptData(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char * typeOfAlgo,char* indexOfKey,char *keyValByMK,char*iniVec,int lenOfSData,char *SData,char* DData);
	//��ָ����Կ��ָ�����ݽ��н��� S050
	//���������typeOfAlgo �㷨����
	//���������indexOfKey ������Կ����
	//���������keyValByMK ������Կֵ
	//���������iniVec ��ʼ����
	//���������lenOfSData ���ݿ鳤��
	//���������SData ���ݿ�
	//���������DData ���ܺ����ݿ�
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ� �������ݳ���
	_declspec(dllexport) int union_syj1001_dectyptData_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char * typeOfAlgo,char* indexOfKey,char *keyValByMK,char*iniVec,int lenOfSData,char *SData,char* DData);
	//��ָ����������MAC  0030
	//���������typeOfAlgo �㷨����
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������iniVec     ��ʼ����
	//���������lenOfMacData  macData���ݳ���
	//���������macData 
	//���������mac
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ����ص�mac���ݳ��� 
	_declspec(dllexport) int union_syj1001_genMac(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char *typeOfAlgo,char * indexOfKey,char*keyValByMK,char*iniVec,int lenOfMacData,char*macData,char*mac);
	//��ָ����������MAC  S030
	//���������typeOfAlgo �㷨����
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������iniVec     ��ʼ����
	//���������lenOfMacData  macData���ݳ���
	//���������macData 
	//���������mac
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ����ص�mac���ݳ��� 
	_declspec(dllexport) int union_syj1001_genMac_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char *typeOfAlgo,char * indexOfKey,char*keyValByMK,char*iniVec,int lenOfMacData,char*macData,char*mac);
	//��ָ��������֤MAC 0032
	//���������typeOfAlgo �㷨����
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������iniVec     ��ʼ����
	//���������lenOfMac   mac���� MACֵ����Ҫ������֤���ֽ�������СֵΪ4�����ֵ16
	//���������mac
	//���������lenOfMacData  macData���ݳ���
	//���������macData 
	//����ֵ�� <0��ʧ�ܣ�=0�ɹ�
	_declspec(dllexport) int union_syj1001_verifyMac(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*typeOfAlgo,char*indexOfKey,char*keyValByMK,char*iniVec,int lenOfMac,char*mac,int lenOfMacData,char *macData);
	//��ָ��������֤MAC S032
	//���������typeOfAlgo �㷨����
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������iniVec     ��ʼ����
	//���������lenOfMac   mac���� MACֵ����Ҫ������֤���ֽ�������СֵΪ4�����ֵ16
	//���������mac
	//���������lenOfMacData  macData���ݳ���
	//���������macData 
	//����ֵ�� <0��ʧ�ܣ�=0�ɹ�
	_declspec(dllexport) int union_syj1001_verifyMac_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*typeOfAlgo,char*indexOfKey,char*keyValByMK,char*iniVec,int lenOfMac,char*mac,int lenOfMacData,char *macData);
	//��ָ����Կ������PIN���м��� 0020
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������formatOfPinBlock PIN��ĸ�ʽ
	//���������clearPin PIN�������
	//���������accNo  �˺�
	//���������pinByKEK pin����
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ� ������ݳ���
	_declspec(dllexport)  int union_syj1001_enctyptPin(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfKey,char*keyValByMK,char*formatOfPinBlock, char*clearPin,char*accNo,char*pinByKEK);
	//��ָ����Կ������PIN���м��� S020
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������formatOfPinBlock PIN��ĸ�ʽ
	//���������clearPin PIN�������
	//���������accNo  �˺�
	//���������pinByKEK pin����
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ� ������ݳ���
	_declspec(dllexport)  int union_syj1001_enctyptPin_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfKey,char*keyValByMK,char*formatOfPinBlock, char*clearPin,char*accNo,char*pinByKEK);
	//��ָ����Կ������PIN���н��� 0024
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������formatOfPinBlock PIN��ĸ�ʽ
	//���������pinByKEK pin����
	//���������accNo  �˺�
	//���������clearPin PIN�������
	//����ֵ�� <0��ʧ�ܣ�=0�ɹ�
	_declspec(dllexport)  int union_syj1001_dectyptPin(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfKey,char*keyValByMK,char*formatOfPinBlock, char*pinByKEK,char*accNo,char*clearPin);
	//��ָ����Կ������PIN���н��� S024
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������formatOfPinBlock PIN��ĸ�ʽ
	//���������pinByKEK pin����
	//���������accNo  �˺�
	//���������clearPin PIN�������
	//����ֵ�� <0��ʧ�ܣ�=0�ɹ�
	_declspec(dllexport)  int union_syj1001_dectyptPin_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfKey,char*keyValByMK,char*formatOfPinBlock, char*pinByKEK,char*accNo,char*clearPin);
	//��ӡPIN/PIN�������� 00PE
	//���������typeOfFile �ĵ�����
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������accNo  �˺�
	//���������pinBlock pin����
	//���������formatOfPinBlock PIN��ĸ�ʽ
	//���������printFieldNo �ڴ�ӡ������������16��
	//���������... �ڴ�ӡ��ʽ��������Ϊ��Print Field s������Ĵ�ӡ��
	//����ֵ�� <0��ʧ�ܣ�=0�ɹ�
	_declspec(dllexport)  int union_syj1001_printPin(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char typeOfFile,char* indexOfKey,char*keyValByMK,char*accNo,char*pinBlock,char*formatOfPinBlock,int printFieldNo,...);
	//��ӡPIN/PIN�������� S0PE
	//���������typeOfFile �ĵ�����
	//���������indexOfKey ��Կ����
	//���������keyValByMK ��Կֵ
	//���������accNo  �˺�
	//���������pinBlock pin����
	//���������formatOfPinBlock PIN��ĸ�ʽ
	//���������printFieldNo �ڴ�ӡ������������16��
	//���������... �ڴ�ӡ��ʽ��������Ϊ��Print Field s������Ĵ�ӡ��
	//����ֵ�� <0��ʧ�ܣ�=0�ɹ�
	_declspec(dllexport)  int union_syj1001_printPin_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char typeOfFile,char* indexOfKey,char*keyValByMK,char*accNo,char*pinBlock,char*formatOfPinBlock,int printFieldNo,...);
	//��ָ����Կ����ɢ����������Կ 0064
	//���������indexOfRKey ����Կ����
	//���������rKeyValByMK ����Կֵ
	//���������disperseCount ��ɢ����
	//���������newKeyVal MK�����µĹ�����Կ/����Կ����
	//���������chkVal ������Կ/����Կ��У��ֵ
	//���������... ��ɢ����
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ� ��Կ����
	_declspec(dllexport)  int union_syj1001_disperseKey(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfRKey,char* rKeyValByMK,char*newKeyVal,char*chkVal,int disperseCount,...);
	//��ָ����Կ����ɢ����������Կ S064
	//���������indexOfRKey ����Կ����
	//���������rKeyValByMK ����Կֵ
	//���������disperseCount ��ɢ����
	//���������newKeyVal MK�����µĹ�����Կ/����Կ����
	//���������chkVal ������Կ/����Կ��У��ֵ
	//���������... ��ɢ����
	//����ֵ�� <0��ʧ�ܣ�>0�ɹ� ��Կ����
	_declspec(dllexport)  int union_syj1001_disperseKey_SM1(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char*indexOfRKey,char* rKeyValByMK,char*newKeyVal,char*chkVal,int disperseCount,...);
};