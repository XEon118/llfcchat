#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

//CRTP
class HttpMgr:public QObject,public Singleton<HttpMgr>,
                public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    ~HttpMgr();
    void PostHttpReq(QUrl url,QJsonObject json,ReqId req_id,Modules mod);
private:
    friend class Singleton<HttpMgr>;    //singleton中getinstance中的new T需要构造，T是httpmgr，声明友元
    HttpMgr();
    QNetworkAccessManager _manager;

private slots:
    void slot_http_finish(ReqId id,QString res,ErrorCodes err,Modules mod);
signals:
    void sig_http_finish(ReqId id,QString res,ErrorCodes err,Modules mod);//http发送完毕
    void sig_reg_mod_finish(ReqId id,QString res,ErrorCodes err);
    void sig_reset_mod_finish(ReqId id,QString res,ErrorCodes err);
    void sig_login_mod_finish(ReqId id,QString res,ErrorCodes err);
};

#endif // HTTPMGR_H
