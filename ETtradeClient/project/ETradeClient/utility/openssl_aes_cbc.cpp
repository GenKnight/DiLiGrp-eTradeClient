
#include "stdafx.h"

#include "etradeclient/utility/openssl_aes_cbc.h"
#include "etradeclient/utility/string_converter.h"

#include <fstream>
#include <exception>
#include <array>
#include <random>
#include <climits>
#include <chrono>

#include <windows.h>

namespace
{
	const int kBlockSize = 16;
	const int kKeyLength = 16;// 128Bit
	const std::array<unsigned char, kKeyLength> kUserKey{ { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'i', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'i' } };
	
	class AesCbcInterface
	{
		typedef struct AesKeySt
		{
			unsigned int rd_key[60];
			int rounds;
		}AES_KEY;

		enum CryptType
		{
			AesDecrypt = 0,
			AesEncrypt = 1
		};

		typedef int(__cdecl *AesSetEncryptKey)(const unsigned char *userKey, const int bits, AES_KEY *key);
		typedef int(__cdecl *AesSetDecryptKey)(const unsigned char *userKey, const int bits, AES_KEY *key);
		typedef void(__cdecl *AesCbcEncrypt)(const unsigned char *in, unsigned char *out,
			size_t length, const AES_KEY *key, unsigned char *ivec, const int enc);

	public:
		AesCbcInterface(const std::array<unsigned char, kKeyLength>& user_key) : m_user_key(user_key)
		{
			Init();
		}

		const std::string Encrypt(const std::string& plaintext) const
		{
			unsigned char c_plaintext[kBlockSize];
			unsigned char c_ciphertext[kBlockSize];
			unsigned char iv[kBlockSize];

			std::default_random_engine generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
			std::uniform_int_distribution<int> distribution(0, UCHAR_MAX);// Make generates number in the range of unsigned char value space. 
			for (int index = 0; index < kBlockSize; ++index)
				iv[index] = distribution(generator);
 
			unsigned int kKeyBits = m_user_key.size() * 8;
			AES_KEY aes_key;
			m_fn_aes_set_encrypt_key(m_user_key.data(), kKeyBits, &aes_key);

			std::string ciphertext;
			ciphertext.append((char*)iv, kBlockSize);// ����iv����

			// �����ĳ��ȼ��ܱ���
			const int kPlaintextLength = plaintext.length();
			std::string ciphertext_length = std::to_string(kPlaintextLength);
			memset(c_ciphertext, '\0', kBlockSize);
			memset(c_plaintext, '\0', kBlockSize);
			memcpy(c_plaintext, ciphertext_length.c_str(), ciphertext_length.length());
			m_fn_aes_cbc_encrypt(c_plaintext, c_ciphertext, kBlockSize, &aes_key, iv, AesEncrypt);
			ciphertext.append((char*)c_ciphertext, kBlockSize);
			memcpy(iv, c_ciphertext, kBlockSize);// ͬʱ��������iv���ݣ��Ա��´μ���

			int index = 0;
			for (; index + kBlockSize < kPlaintextLength; index += kBlockSize)
			{// ����ȡ�����ģ������ܴ���
				memset(c_ciphertext, '\0', kBlockSize);// ���Ŀռ�����
				memcpy(c_plaintext, plaintext.c_str() + index, kBlockSize);//ȡ������
				m_fn_aes_cbc_encrypt(c_plaintext, c_ciphertext, kBlockSize, &aes_key, iv, AesEncrypt); // ���ܣ�iv���ݷ����仯
				ciphertext.append((char*)c_ciphertext, kBlockSize); //��������
				memcpy(iv, c_ciphertext, kBlockSize);// ͬʱ��������iv���ݣ��Ա��´μ���
			}

			// ����û�а������ʱ��β��δ��������������
			if (kPlaintextLength - index > 0)
			{
				memset(c_ciphertext, '\0', kBlockSize);
				memset(c_plaintext, '\0', kBlockSize);
				memcpy(c_plaintext, plaintext.c_str() + index, kPlaintextLength - index);
				m_fn_aes_cbc_encrypt(c_plaintext, c_ciphertext, kBlockSize, &aes_key, iv, AesEncrypt);
				ciphertext.append((char*)c_ciphertext, kBlockSize);
			}

			return ciphertext;
		}

		const std::string Decrypt(const std::string& ciphertext) const
		{
			unsigned char c_plaintext[kBlockSize];
			unsigned char c_ciphertext[kBlockSize];
			unsigned char iv[kBlockSize];

			const int kKeyBits = m_user_key.size() * 8;
			AES_KEY aes_key;
			m_fn_aes_set_decrypt_key(m_user_key.data(), kKeyBits, &aes_key);

			std::string plaintext;

			const int kCiphertextLength = ciphertext.length();
			if (0 != kCiphertextLength % kBlockSize || kCiphertextLength < kBlockSize * 3)
				throw std::exception("����̫�٣�����!�޷����ܡ�");// ���ݱ��밴�������������3�����ݡ�iv�����ȡ����ݡ�

			int index = 0;
			memcpy(iv, ciphertext.c_str() + index, kBlockSize);// ��ȡiv����
			index += kBlockSize;

			// ��ȡ�������ݳ���
			memset(c_plaintext, '\0', kBlockSize);// ���Ŀռ�����
			memcpy(c_ciphertext, ciphertext.c_str() + index, kBlockSize);//ȡ������
			m_fn_aes_cbc_encrypt(c_ciphertext, c_plaintext, kBlockSize, &aes_key, iv, AesDecrypt);// ����
			const int kPlaintextLength = ::atoi((char*)c_plaintext);
			memcpy(iv, c_ciphertext, kBlockSize); // ͬʱ��������iv���ݣ��Ա��´μ���
			index += kBlockSize;

			for (; index < kCiphertextLength; index += kBlockSize)
			{
				memset(c_plaintext, '\0', kBlockSize);// ���Ŀռ�����
				memcpy(c_ciphertext, ciphertext.c_str() + index, kBlockSize);//ȡ������
				m_fn_aes_cbc_encrypt(c_ciphertext, c_plaintext, kBlockSize, &aes_key, iv, AesDecrypt);// ����
				plaintext.append((char*)c_plaintext, kBlockSize);//��������
				memcpy(iv, c_ciphertext, kBlockSize); // ͬʱ��������iv���ݣ��Ա��´μ���
			}

			if ((int)plaintext.length() > kPlaintextLength) // ȥ���鲹������ 
				plaintext.erase(kPlaintextLength, plaintext.length() - kPlaintextLength);

			return plaintext;
		}

	private:
		void Init()
		{
			const std::wstring kDLLFile = L"./libeay32.dll";
			m_dll_handle = LoadLibrary(kDLLFile.c_str());

			if (NULL != m_dll_handle)
			{
				m_fn_aes_set_encrypt_key = (AesSetEncryptKey)GetProcAddress(m_dll_handle, "AES_set_encrypt_key");
				m_fn_aes_set_decrypt_key = (AesSetDecryptKey)GetProcAddress(m_dll_handle, "AES_set_decrypt_key");
				m_fn_aes_cbc_encrypt = (AesCbcEncrypt)GetProcAddress(m_dll_handle, "AES_cbc_encrypt");
			}
			else
				throw std::exception(("Fail to load the OpenSSL-AEC-BCB DLL from path : " + wstr_2_str(kDLLFile)).c_str());
		}

	private:
		const std::array<unsigned char, kKeyLength> m_user_key;

		HINSTANCE			m_dll_handle;
		AesSetEncryptKey	m_fn_aes_set_encrypt_key;
		AesSetDecryptKey	m_fn_aes_set_decrypt_key;
		AesCbcEncrypt		m_fn_aes_cbc_encrypt;
	};

	const AesCbcInterface& GetAesCbcInterface()
	{
		static AesCbcInterface aes_cbc_interface(kUserKey); // ������׳��쳣
		return aes_cbc_interface;
	}
}

namespace AES_CBC
{
	std::string Encrypt(const std::string& plaintext)
	{
		return GetAesCbcInterface().Encrypt(plaintext);
	}

	std::string Decrypt(const std::string& ciphertext)
	{
		return GetAesCbcInterface().Decrypt(ciphertext);
	}

	void EncryptToFile(const std::string& plaintext, const std::string& ciphertext_file_name)
	{
		std::ofstream ciphertext_file(ciphertext_file_name, std::ios_base::out | std::ios_base::binary);

		if (!ciphertext_file.is_open())
			throw std::exception("������������ļ�ʧ�ܣ�");

		ciphertext_file << Encrypt(plaintext);
		ciphertext_file.close();
	}

	std::string DecryptFromFile(const std::string& ciphertext_file_name)
	{
		std::ifstream ciphertext_file(ciphertext_file_name, std::ios_base::in | std::ios_base::binary);
		if (!ciphertext_file.is_open())
			throw std::exception("�������ļ�ʧ�ܣ����飡");

		std::string ciphertext((std::istreambuf_iterator<char>(ciphertext_file)), std::istreambuf_iterator<char>());
		ciphertext_file.close();

		return Decrypt(ciphertext);
	}

	void EncryptFileToFile(const std::string& plaintext_file_name, const std::string& ciphertext_file_name)
	{
		std::ifstream plaintext_file(plaintext_file_name, std::ios_base::in);
		if (!plaintext_file.is_open())
			throw std::exception("�������ļ�ʧ�ܣ����飡");

		std::string plaintext((std::istreambuf_iterator<char>(plaintext_file)), std::istreambuf_iterator<char>());
		plaintext_file.close();

		EncryptToFile(plaintext, ciphertext_file_name);
	}

	void DecryptFileToFile(const std::string& ciphertext_file_name, const std::string& plaintext_file_name)
	{
		std::ofstream plaintext_file(plaintext_file_name, std::ios_base::out);
		if (!plaintext_file.is_open())
			throw std::exception("������������ļ�ʧ�ܣ�");

		plaintext_file << DecryptFromFile(ciphertext_file_name);
		plaintext_file.close();
	}
} // AES_CBC