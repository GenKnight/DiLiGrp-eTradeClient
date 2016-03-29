#ifndef ETRADECLIENT_UTILITY_OPENSSL_AES_CBC_H_INCLUDED
#define ETRADECLIENT_UTILITY_OPENSSL_AES_CBC_H_INCLUDED

#include <string>

/*Notice: the APIs of "openssl_aes" may throw exceptions when error happens, these exception messages contain
  Chinese characters which are encoding in GBK encoding, so the caller has to call "gbk_2_wstr" to convert the
  exception message to std::wstring in Unicode encoding.*/
namespace AES_CBC
{
	std::string Encrypt(const std::string& plaintext);

	std::string Decrypt(const std::string& ciphertext);

	void EncryptToFile(const std::string& plaintext, const std::string& ciphertext_file_name);

	std::string DecryptFromFile(const std::string& ciphertext_file_name);

	void EncryptFileToFile(const std::string& plaintext_file_name, const std::string& ciphertext_file_name);

	void DecryptFileToFile(const std::string& ciphertext_file_name, const std::string& plaintext_file_name);
} // AES_CBC

#endif // ETRADECLIENT_UTILITY_OPENSSL_AES_CBC_H_INCLUDED
