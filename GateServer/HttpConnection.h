#pragma once
#include "const.h"
class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;	//打破封装
	HttpConnection(boost::asio::io_context& ioc);
	void Start();//监听读写事件
	tcp::socket& GetSocket();	//返回socket
private:
	void CheckDeadline();	//超时检测
	void WriteResponse();	//应答
	void PreParseGetParam(); //参数解析
	void HandleReq();		//处理请求
	tcp::socket _socket;
	beast::flat_buffer _buffer{ 8192 };	//8K缓冲区
	http::request<http::dynamic_body> _request;	//请求
	http::response<http::dynamic_body> _response;	//回复
	//定时器
	net::steady_timer deadline_{
		_socket.get_executor(),std::chrono::seconds(60)	//绑定。超时
	};
	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;
};

