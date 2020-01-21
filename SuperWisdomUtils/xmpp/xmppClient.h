
#pragma once

#pragma execution_character_set("utf-8")

#include "imsdk_global.h"
#include <gloox/client.h>
#include <gloox/messagehandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/presencehandler.h>
#include <gloox/error.h>
#include <gloox/messagesession.h>
#include <gloox/mucroom.h>
#include <gloox/message.h>
#include <gloox/messagefilter.h>
#include <gloox/receipt.h>
#include <gloox/eventhandler.h>
#include "dbutil.h"

#include "../commons/Utils.h"
#include <map>
#include <mutex>
#include <atomic>
#include <iostream>

//xmpp resource
#define XMPPResourse "pc"

#if defined(_WIN32)

#ifdef _DEBUG
#pragma comment(lib,"../debug/gloox.lib")
#else
#pragma comment(lib,"../bin/gloox.lib")
#endif

#endif

using namespace gloox;
using namespace im;
class xmppClient: private gloox::MessageHandler, private gloox::ConnectionListener,
	private gloox::PresenceHandler, private gloox::IqHandler, private gloox::TagHandler,
	private gloox::MUCRoomHandler, private gloox::MUCRoomConfigHandler, private gloox::LogHandler,
	private gloox::EventHandler
{
public:
	xmppClient();

	~xmppClient();

public:
	//登录
	void start();

	//设置用户信息
	void setUser(shared_ptr<User> userPtr);

	//退出登录
	void logoutXmpp();

	//断开连接
	void disconnect();

	//设置回调类
	void setImEventCall(shared_ptr<ImEventcall> imCallback);
	
public:

	// Override ConnectionListener
	virtual void onConnect() override;
	virtual void onDisconnect(gloox::ConnectionError connError) override;
	virtual bool onTLSConnect(const gloox::CertInfo& info) override;
	virtual void onResourceBind(const std::string& resource) override;
	virtual void onResourceBindError(const gloox::Error* error) override;
	virtual void onSessionCreateError(const gloox::Error* error) override;

	// Override MessageSessionHandler
	virtual void handleMessage(const gloox::Message& msg, gloox::MessageSession* session) override;
	virtual bool handleIq(const gloox::IQ& iq) override;
	virtual void handleIqID(const gloox::IQ& iq, int context) override;
	virtual void handleTag(gloox::Tag* tag) override;
	virtual void handleMUCParticipantPresence(gloox::MUCRoom* room, const gloox::MUCRoomParticipant participant, const gloox::Presence& presence) override;
	virtual void handleMUCMessage(gloox::MUCRoom* room, const gloox::Message& msg, bool priv) override;
	virtual bool handleMUCRoomCreation(gloox::MUCRoom* room) override;
	virtual void handleMUCSubject(gloox::MUCRoom* room, const std::string& nick, const std::string& subject) override;
	virtual void handleMUCInviteDecline(gloox::MUCRoom* room, const gloox::JID& invitee, const std::string& reason) override;
	virtual void handleMUCError(gloox::MUCRoom* room, gloox::StanzaError error) override;
	virtual void handleMUCInfo(gloox::MUCRoom* room, int features, const std::string& name, const gloox::DataForm* infoForm) override;
	virtual void handleMUCItems(gloox::MUCRoom* room, const gloox::Disco::ItemList& items) override;

	virtual void handleMUCConfigList(gloox::MUCRoom*, const gloox::MUCListItemList&, gloox::MUCOperation) override {}
	virtual void handleMUCConfigForm(gloox::MUCRoom* room, const gloox::DataForm& form)override;
	virtual void handleMUCConfigResult(gloox::MUCRoom*, bool, gloox::MUCOperation)override {};
	virtual void handleMUCRequest(gloox::MUCRoom*, const gloox::DataForm&)override {};

	// Override LogHandler
	virtual void handleLog(gloox::LogLevel level, gloox::LogArea area, const std::string& message)override;
	virtual void handleEvent(const gloox::Event& event)override;
	virtual void handlePresence(const gloox::Presence& presence) override;


private:
	bool m_bLogin = false;//是否登录
	bool m_bLogout = false;//是否退出登录

	gloox::JID serverJid;	
	shared_ptr<ServerConfig> m_configPtr;
	shared_ptr<User> m_userPtr;
	shared_ptr<gloox::Client> xmppClient_ptr;
	shared_ptr<ImEventcall> m_imCallback;
};

