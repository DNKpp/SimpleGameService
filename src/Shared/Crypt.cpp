#include "stdafx.h"
#include "Crypt.hpp"

CryptoPP::RSA::PublicKey Crypt::generateRSA()
{
	CryptoPP::AutoSeededRandomPool rnd;
	m_RSA_Private.GenerateRandomWithKeySize(rnd, 3072);	// ask not why, cryptoPP says its a good number
	return CryptoPP::RSA::PublicKey(m_RSA_Private);
}

void Crypt::setOtherRSAPublic(CryptoPP::RSA::PublicKey _key)
{
	m_OtherRSA_Public = std::move(_key);
}

QByteArray Crypt::encryptRSA(QByteArray _buffer)
{
	int index = 0;
	CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(m_OtherRSA_Public);
	CryptoPP::AutoSeededRandomPool rnd;
	QByteArray result;
	while (index < _buffer.size())
	{
		auto currentIndex = index;
		auto currentSize = std::min((size_t)_buffer.size() - index, encryptor.FixedMaxPlaintextLength());
		auto bytes = QByteArray::fromRawData(_buffer.data() + index, currentSize);
		index += currentSize;

		assert(0 != encryptor.FixedMaxPlaintextLength());
		assert((size_t)bytes.size() <= encryptor.FixedMaxPlaintextLength());

		size_t ecl = encryptor.CiphertextLength(bytes.size());
		assert(0 != ecl);
		result.resize(result.size() + ecl);

		encryptor.Encrypt(rnd, (unsigned char*)bytes.data(), bytes.size(), (unsigned char*)result.data() + currentIndex);
	}
	return result;
}

QByteArray Crypt::decryptRSA(QByteArray _buffer)
{
	int index = 0;
	CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(m_RSA_Private);
	QByteArray message;
	CryptoPP::AutoSeededRandomPool rnd;

	while (index < _buffer.size())
	{
		auto currentIndex = index;
		auto currentSize = std::min((size_t)_buffer.size() - index, decryptor.FixedCiphertextLength());
		auto curBuffer = QByteArray::fromRawData(_buffer.data() + currentIndex, currentSize);
		QByteArray recovered(decryptor.MaxPlaintextLength(curBuffer.size()), 0);
		CryptoPP::DecodingResult result = decryptor.Decrypt(rnd, (unsigned char*)curBuffer.data(), curBuffer.size(), (unsigned char*)recovered.data());

		assert(result.isValidCoding);
		assert(result.messageLength <= decryptor.MaxPlaintextLength(curBuffer.size()));

		// At this point, we can set the size of the recovered
		//  data. Until decryption occurs (successfully), we
		//  only know its maximum size
		recovered.resize(result.messageLength);
		message.append(recovered);
		index += currentSize;
	}
	return message;
}

//void Crypt::setAESKey(CryptoPP::AES::Encryption _key)
//{
//	m_AESKey = std::move(_key);
//}
//
//CryptoPP::AES Crypt::generateAES()
//{
//	CryptoPP::AutoSeededRandomPool rnd;
//	m_AESKey.gen
//}
