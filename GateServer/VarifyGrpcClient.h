#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPol {
public:
	RPConPol(size_t poolsize, std::string host, std::string port);
	~RPConPol();
	void Close();
	std::unique_ptr<VarifyService::Stub> getConnection();	//获取连接
	void returnConnection(std::unique_ptr<VarifyService::Stub> context);	//回收连接

private:
	std::atomic<bool> b_stop_;	//标记是否回收
	size_t poolSize_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr<VarifyService::Stub>> connections_;
	std::condition_variable cond_;
	std::mutex mutex_;
};



class VarifyGrpcClient:public Singleton<VarifyGrpcClient>
{
	friend class Singleton<VarifyGrpcClient>;
public:
	GetVarifyRsp GetVarifyCode(std::string email) {
		ClientContext context;
		GetVarifyRsp reply;
		GetVarifyReq request;
		request.set_email(email);
		auto stub = pool_->getConnection();
		Status status = stub->GetVarifyCode(&context,request,&reply);
		if (status.ok()) {
			pool_->returnConnection(std::move(stub));
			return reply;

		}
		else {
			pool_->returnConnection(std::move(stub));
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}

private:
	VarifyGrpcClient();
	std::unique_ptr<RPConPol> pool_;
	
};

