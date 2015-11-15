#include "stdafx.h"
#include "NetworkClient.h"
#include "moc_NetworkClient.h"
#include "NetworkMessage.hpp"
#include <QtCore/QDataStream>

NetworkClient::NetworkClient()
{
	m_Socket = new QTcpSocket(this);
	m_Socket->connectToHost(QHostAddress::LocalHost, 2015);
	connect(m_Socket, SIGNAL(connected()), this, SLOT(_onConnected()));
}

void NetworkClient::_createMsg()
{
	std::string in;
	std::getline(std::cin, in);
	QDataStream out(&m_Message, QIODevice::WriteOnly);
	out.writeRawData(network::PacketBegin.data(), network::PacketBegin.size());
	//out << (uint32_t)in.size();
	//out.writeRawData(in.data(), in.size());
	std::array<char, 50000> a;
	a.fill('I');
	out << (uint32_t)a.size();
	out.writeRawData(a.data(), a.size());
}

void NetworkClient::_onConnected()
{
	qDebug() << "connected to address: " << m_Socket->peerAddress() << " port: " << m_Socket->peerPort();
	connect(m_Socket, SIGNAL(bytesWritten(qint64)), this, SLOT(_onBytesWritten(qint64)));
	_createMsg();
	m_Socket->write(m_Message.data(), m_Message.size());
}

void NetworkClient::_onBytesWritten(qint64 _bytes)
{
	if (m_Message.size() == _bytes)
	{
		qDebug() << "Message written.";
		_createMsg();
	}
	else
		m_Message.remove(0, _bytes);
	m_Socket->write(m_Message.data(), m_Message.size());
}
