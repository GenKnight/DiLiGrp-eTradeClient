/* 
 *  Copyright (c) 2011,  ���ݽ��Ͽ���union  
 *  All rights reserved. 
 *  �ļ����ƣ� mangerCmd.h
 *  ժ Ҫ��    Ӧ�ü��ܻ�����Կ����ָ��ӿڵĶ���  
 *  ��ǰ�汾�� 1.0,  ��д��: ���䣬�޸�ʱ��: 2011-11-22 �޸�����: ����
 *  
 */
extern "C"
{
	//��Ӧ�ü��ܻ�����socket����
	//���������hsmIp ���ܻ�IP��ַ
	//���������hsmPort ���ܻ��������˿�
	//���������isCfg 0:���������ļ���ȡhsmIp��hsmPort������������е���Щ��Ϣ 1���������ļ��ж�ȡ������Ϣ
	//����ֵ�� <0��ʧ�ܣ�>0 �ɹ������������׽��־��
	_declspec(dllexport) int union_syj1001_connect(char * hsmIp,int hsmPort,int isCfg);
	//�Ͽ���Ӧ�ü��ܻ���socket����
	//���������clientSocket socket�׽��־��
	//����ֵ�� <0��ʧ�ܣ�0���ɹ�
	_declspec(dllexport) int union_syj1001_disConnect(int clientSocket);
	//͸���ӿڣ�����õı���ת����Ӧ�ü��ܻ�
	//���������msg ����
	//���������hsmIp ���ܻ�IP��ַ
	//���������hsmPort ���ܻ��������˿�
	//���������timeOut socket��ʱ�ȴ�ʱ�� ms��λ
	//���������isCfg 0:���������ļ���ȡlenOfMsg��hsmIp��hsmPort��timeOut��hsmLenOfMsgHeader��isLenOfHsmMsg ������������е���Щ��Ϣ 1���������ļ��ж�ȡ������Ϣ
	//���������clientSocket -1:���õ��Ƕ����ӷ�ʽ����Ҫ�ں��������socket�Ĵ������ͷ� >0:���ó����ӷ�ʽ�����ø��׽��־��
	//���������hsmLenOfMsgHeader ��Ϣͷ����
	//���������isLenOfHsmMsg ���ĳ��ȣ����ֽڣ�
	//���������revMsg ���صı�����Ӧ
	//����ֵ�� <0��ʧ�ܣ�>0������ı��ĳ���
	_declspec(dllexport) int union_syj1001_direct_sendMsg(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket,char* msg, char *revMsg);
	//��ȡ���ܻ�������Ϣ ������ 0000
	//���������hsmIp ���ܻ�IP��ַ
	//���������hsmPort ���ܻ��������˿�
	//���������timeOut socket��ʱ�ȴ�ʱ�� ms��λ
	//���������isCfg 0:���������ļ���ȡlenOfMsg��hsmIp��hsmPort��timeOut��hsmLenOfMsgHeader��isLenOfHsmMsg ������������е���Щ��Ϣ 1���������ļ��ж�ȡ������Ϣ
	//���������clientSocket -1:���õ��Ƕ����ӷ�ʽ����Ҫ�ں��������socket�Ĵ������ͷ� >0:���ó����ӷ�ʽ�����ø��׽��־��
	//���������hsmLenOfMsgHeader ��Ϣͷ����
	//���������isLenOfHsmMsg ���ĳ��ȣ����ֽڣ�
	//���������revMsg ���صı�����Ӧ
	//����ֵ�� <0��ʧ�ܣ�=0��revMsg�а�������ԿУ��ֵ���ͼ��ܻ�������Ϣ��=13 ���ܻ�������Ϣ
	_declspec(dllexport) int union_syj1001_getHsmBaseInfo(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket, char *revMsg);
	//�����ܻ���Ȩ ������ 0002
	//���������hsmIp ���ܻ�IP��ַ
	//���������hsmPort ���ܻ��������˿�
	//���������timeOut socket��ʱ�ȴ�ʱ�� ms��λ
	//���������isCfg 0:���������ļ���ȡlenOfMsg��hsmIp��hsmPort��timeOut��hsmLenOfMsgHeader��isLenOfHsmMsg ������������е���Щ��Ϣ 1���������ļ��ж�ȡ������Ϣ
	//���������clientSocket -1:���õ��Ƕ����ӷ�ʽ����Ҫ�ں��������socket�Ĵ������ͷ� >0:���ó����ӷ�ʽ�����ø��׽��־��
	//���������hsmLenOfMsgHeader ��Ϣͷ����
	//���������isLenOfHsmMsg ���ĳ��ȣ����ֽڣ�
	//����ֵ�� <0��ʧ�ܣ�=0�ɹ�
	_declspec(dllexport) int union_syj1001_setAuthApply(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket);
	//ȡ�����ܻ���Ȩ ������ 0004
	//���������hsmIp ���ܻ�IP��ַ
	//���������hsmPort ���ܻ��������˿�
	//���������timeOut socket��ʱ�ȴ�ʱ�� ms��λ
	//���������isCfg 0:���������ļ���ȡlenOfMsg��hsmIp��hsmPort��timeOut��hsmLenOfMsgHeader��isLenOfHsmMsg ������������е���Щ��Ϣ 1���������ļ��ж�ȡ������Ϣ
	//���������clientSocket -1:���õ��Ƕ����ӷ�ʽ����Ҫ�ں��������socket�Ĵ������ͷ� >0:���ó����ӷ�ʽ�����ø��׽��־��
	//���������hsmLenOfMsgHeader ��Ϣͷ����
	//���������isLenOfHsmMsg ���ĳ��ȣ����ֽڣ�
	//����ֵ�� <0��ʧ�ܣ�=0�ɹ�
	_declspec(dllexport) int union_syj1001_cancelAuthApply(char* hsmIp,int hsmPort,int timeout,int isCfg,int hsmLenOfMsgHeader,int isLenOfHsmMsg,int clientSocket);
	 
};
