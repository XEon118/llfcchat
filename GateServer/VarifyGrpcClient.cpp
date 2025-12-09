#include "VarifyGrpcClient.h"
#include "ConfigMgr.h"

RPConPol::RPConPol(size_t poolsize, std::string host, std::string port):poolSize_(poolsize),host_(host),port_(port),b_stop_(false)
{
	//遍历，每个连接都要信使
	for (size_t i = 0; i < poolSize_; ++i) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host+":"+port,
			grpc::InsecureChannelCredentials());	//内部服务，使用不安全即可
		connections_.push(VarifyService::NewStub(channel));
	}
}

RPConPol::~RPConPol()
{
	std::lock_guard<std::mutex> lock(mutex_);	//加锁
	Close();
	while (!connections_.empty()) {
		connections_.pop();
	}
}

//通知挂起线程，线程池回收
void RPConPol::Close() {
	b_stop_ = true;
	cond_.notify_all();
}

std::unique_ptr<VarifyService::Stub> RPConPol::getConnection()
{
	std::unique_lock<std::mutex> lock(mutex_);	//加锁
	cond_.wait(lock, [this]() {
		if (b_stop_) {
			return true;	//已停止，解锁。立即返回 true，使等待结束，程序会继续执行后续的停止处理逻辑
		}
		return !connections_.empty();	//判断队列是否为空，挂起。有可用连接，返回 true，等待结束，线程可以去获取一个连接。如果连接池为空且未停止，则返回 false，线程继续等待
		});

	//线程唤醒
	if (b_stop_) {
		return nullptr;
	}

	auto context = std::move(connections_.front());	//将队首元素移动出来
	connections_.pop();
	return context;		//返回获取到的连接
}

void RPConPol::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	connections_.push(std::move(context));
	cond_.notify_one();
}

VarifyGrpcClient::VarifyGrpcClient() {
	auto& gCfgMgr = ConfigMgr::Inst();	//获取配置
	std::string host = gCfgMgr["VarifyServer"]["Host"];
	std::string port = gCfgMgr["VarifyServer"]["Port"];
	pool_.reset(new RPConPol(5, host, port));
}
