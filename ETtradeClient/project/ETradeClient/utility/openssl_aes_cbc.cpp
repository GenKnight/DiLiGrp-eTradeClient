
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
			ciphertext.append((char*)iv, kBlockSize);// 保存iv数据

			// 对明文长度加密保存
			const int kPlaintextLength = plaintext.length();
			std::string ciphertext_length = std::to_string(kPlaintextLength);
			memset(c_ciphertext, '\0', kBlockSize);
			memset(c_plaintext, '\0', kBlockSize);
			memcpy(c_plaintext, ciphertext_length.c_str(), ciphertext_length.length());
			m_fn_aes_cbc_encrypt(c_plaintext, c_ciphertext, kBlockSize, &aes_key, iv, AesEncrypt);
			ciphertext.append((char*)c_ciphertext, kBlockSize);
			memcpy(iv, c_ciphertext, kBlockSize);// 同时，密文做iv数据，以备下次加密

			int index = 0;
			for (; index + kBlockSize < kPlaintextLength; index += kBlockSize)
			{// 按块取出明文，做加密处理
				memset(c_ciphertext, '\0', kBlockSize);// 密文空间清零
				memcpy(c_plaintext, plaintext.c_str() + index, kBlockSize);//取出明文
				m_fn_aes_cbc_encrypt(c_plaintext, c_ciphertext, kBlockSize, &aes_key, iv, AesEncrypt); // 加密，iv数据发生变化
				ciphertext.append((char*)c_ciphertext, kBlockSize); //保存密文
				memcpy(iv, c_ciphertext, kBlockSize);// 同时，密文做iv数据，以备下次加密
			}

			// 数据没有按块对齐时，尾部未对齐数据做加密
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
				throw std::exception("数据太少，或损坏!无法解密。");// 数据必须按块对齐且至少有3块数据【iv、长度、内容】

			int index = 0;
			memcpy(iv, ciphertext.c_str() + index, kBlockSize);// 读取iv数据
			index += kBlockSize;

			// 获取明文数据长度
			memset(c_plaintext, '\0', kBlockSize);// 明文空间清零
			memcpy(c_ciphertext, ciphertext.c_str() + index, kBlockSize);//取出密文
			m_fn_aes_cbc_encrypt(c_ciphertext, c_plaintext, kBlockSize, &aes_key, iv, AesDecrypt);// 解密
			const int kPlaintextLength = ::atoi((char*)c_plaintext);
			memcpy(iv, c_ciphertext, kBlockSize); // 同时，密文做iv数据，以备下次加密
			index += kBlockSize;

			for (; index < kCiphertextLength; index += kBlockSize)
			{
				memset(c_plaintext, '\0', kBlockSize);// 明文空间清零
				memcpy(c_ciphertext, ciphertext.c_str() + index, kBlockSize);//取出密文
				m_fn_aes_cbc_encrypt(c_ciphertext, c_plaintext, kBlockSize, &aes_key, iv, AesDecrypt);// 解密
				plaintext.append((char*)c_plaintext, kBlockSize);//保存明文
				memcpy(iv, c_ciphertext, kBlockSize); // 同时，密文做iv数据，以备下次加密
			}

			if ((int)plaintext.length() > kPlaintextLength) // 去掉块补齐数据 
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
		static AesCbcInterface aes_cbc_interface(kUserKey); // 出错会抛出异常
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
			throw std::exception("创建或打开密文文件失败！");

		ciphertext_file << Encrypt(plaintext);
		ciphertext_file.close();
	}

	std::string DecryptFromFile(const std::string& ciphertext_file_name)
	{
		std::ifstream ciphertext_file(ciphertext_file_name, std::ios_base::in | std::ios_base::binary);
		if (!ciphertext_file.is_open())
			throw std::exception("打开密文文件失败，请检查！");

		std::string ciphertext((std::istreambuf_iterator<char>(ciphertext_file)), std::istreambuf_iterator<char>());
		ciphertext_file.close();

		return Decrypt(ciphertext);
	}

	void EncryptFileToFile(const std::string& plaintext_file_name, const std::string& ciphertext_file_name)
	{
		std::ifstream plaintext_file(plaintext_file_name, std::ios_base::in);
		if (!plaintext_file.is_open())
			throw std::exception("打开明文文件失败，请检查！");

		std::string plaintext((std::istreambuf_iterator<char>(plaintext_file)), std::istreambuf_iterator<char>());
		plaintext_file.close();

		EncryptToFile(plaintext, ciphertext_file_name);
	}

	void DecryptFileToFile(const std::string& ciphertext_file_name, const std::string& plaintext_file_name)
	{
		std::ofstream plaintext_file(plaintext_file_name, std::ios_base::out);
		if (!plaintext_file.is_open())
			throw std::exception("创建或打开明文文件失败！");

		plaintext_file << DecryptFromFile(ciphertext_file_name);
		plaintext_file.close();
	}
} // AES_CBC