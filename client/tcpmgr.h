#ifndef TCPMGR_H
#define TCPMGR_H
#include <QTcpSocket>
#include "singleton.h"
#include "global.h"
#include <functional>
#include <QObject>
#include "userdata.h"
//#include <QJsonArray>

class TcpMgr:public QObject, public Singleton<TcpMgr>,
        public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    ~TcpMgr();

private:
    friend class Singleton<TcpMgr>;
    TcpMgr();
    void initHandlers();
    void handleMsg(ReqId id, int len, QByteArray data);
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;     //数据的缓冲区
    bool _b_recv_pending;   //非阻塞，防止粘包
    quint16 _message_id;    //两个字节
    quint16 _message_len;   //两个字节
    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;

public slots:
    void slot_tcp_connect(ServerInfo);
    void slot_send_data(ReqId reqId, QByteArray data);
signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QByteArray data);
    void sig_switch_chatdlg();
    void sig_login_failed(int);
    void sig_user_search(std::shared_ptr<SearchInfo>);  //搜索到的用户信息，出错则返回nullptr
    void sig_friend_apply(std::shared_ptr<AddFriendApply>); //对方好友申请
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);    //对方同意申请
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);    //同意好友
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg> msg);   //接收方收到的消息
};

#endif // TCPMGR_H
