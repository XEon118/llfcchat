#pragma once
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include <grpcpp/grpcpp.h> 
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <queue>
#include "data.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatConPool {
public:
	ChatConPool(size_t poolSize, std::string host, std::string port) :
		poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
		for (size_t i = 0; i < poolSize_; ++i) {
			std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());	//不安全连接的通道
			connections_.push(ChatService::NewStub(channel));		//信使。右值移动语义
		}
	}

	~ChatConPool() {
		std::lock_guard<std::mutex> lock(mutex_);	//对连接加锁
		Close();
		while (!connections_.empty()) {
			connections_.pop();
		}
	}

	std::unique_ptr<ChatService::Stub> getConnection() {
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this] {
			if (b_stop_) {
				return true;		//已停止，持续加锁并继续执行后续
			}
			return !connections_.empty();	//队列为空，解锁；不为空，继续执行后续
			});
		//如果停止则直接返回空指针
		if (b_stop_) {
			return  nullptr;
		}
		auto context = std::move(connections_.front());
		connections_.pop();
		return context;
	}
	//返还连接
	void returnConnection(std::unique_ptr<ChatService::Stub> context) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (b_stop_) {
			return;
		}
		connections_.push(std::move(context));
		cond_.notify_one();
	}

	void Close() {
		b_stop_ = true;
		cond_.notify_all();
	}
private:
	atomic<bool> b_stop_;
	size_t poolSize_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr<ChatService::Stub>> connections_;
	std::mutex mutex_;
	std::condition_variable cond_;
};
//发送
class ChatGrpcClient:public Singleton<ChatGrpcClient>
{
	friend class Singleton<ChatGrpcClient>;		//允许 Singleton模板类突破封装限制，访问其私有构造函数，从而能够成功地创建并管理这个唯一的实例

public:
	~ChatGrpcClient() {

	}

	AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);	//添加好友
	AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);	//通知被同意者，好友认证成功
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);	//通知对方的消息文本
private:
	ChatGrpcClient();
	unordered_map<std::string, std::unique_ptr<ChatConPool>> _pools;
};

