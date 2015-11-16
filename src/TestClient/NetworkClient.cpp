#include "stdafx.h"
#include "NetworkClient.h"
#include "moc_NetworkClient.h"
#include "NetworkMessage.hpp"
#include <QtCore/QDataStream>
#include "Message.pb.h"
#include "NetworkConnection.hpp"

namespace
{
	QByteArray _createMessage()
	{
		std::cin.ignore();
		using namespace network::protobuf;
		GameAchievement msg;
		auto ident = std::make_unique<GameIdent>();
		ident->set_gameid(1);
		std::array<uint8_t, 16> hash = { 0xc4, 0xca, 0x42, 0x38, 0xa0, 0xb9, 0x23, 0x82, 0x0d, 0xcc, 0x50, 0x9a, 0x6f, 0x75, 0x84, 0x9b };
		ident->set_hash(hash.data(), hash.size());
		msg.set_allocated_gameid(ident.release());
		auto achive = msg.add_achievements();
		achive->set_id(1);
		QByteArray buffer(msg.ByteSize(), 0);
		msg.SerializeToArray(buffer.data(), buffer.size());
		return buffer;
	}
} // anonymous namespace

NetworkClient::NetworkClient()
{
	m_Socket = new QTcpSocket(this);
	m_Socket->connectToHost(QHostAddress::LocalHost, 2015);
	connect(m_Socket, SIGNAL(connected()), this, SLOT(_onConnected()));

	//connect(con, SIGNAL(messageComplete(const network::IMessage&)), &m_MsgListener, SLOT(onMessageReceived(const network::IMessage&)));
}

void NetworkClient::_onConnected()
{
	qDebug() << "connected to address: " << m_Socket->peerAddress() << " port: " << m_Socket->peerPort();
	m_Connection = new network::Connection(*m_Socket, this);
	m_Connection->send(_createMessage());
}
