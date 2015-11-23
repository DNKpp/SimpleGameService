#pragma once

class Crypt
{
private:
	CryptoPP::RSA::PrivateKey m_RSA_Private;
	CryptoPP::RSA::PublicKey m_OtherRSA_Public;
	//CryptoPP::AES::Encryption m_AESKey;

public:
	CryptoPP::RSA::PublicKey generateRSA();
	void setOtherRSAPublic(CryptoPP::RSA::PublicKey _key);
	QByteArray encryptRSA(QByteArray _buffer);
	QByteArray decryptRSA(QByteArray _buffer);

	//void setAESKey(CryptoPP::AES _key);
	//CryptoPP::AES generateAES();
};
