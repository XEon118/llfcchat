#ifndef USERMGR_H
#define USERMGR_H
#include <QObject>
#include <memory>
#include <singleton.h>
#include "userdata.h"
#include <vector>
#include <QJsonArray>

class UserMgr:public QObject,public Singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~ UserMgr();
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void SetToken(QString token);
    int GetUid();
    QString GetName();
    void AppendApplyList(QJsonArray array); //载入好友申请条目
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    bool AlreadyApply(int uid);     //判断好友申请是否已存在
    void AddApplyList(std::shared_ptr<ApplyInfo> app);  //加入申请信息
    void AppendFriendList(QJsonArray array);        //拉取好友列表
    bool CheckFriendById(int uid);  //判断好友是否已存在
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);  //针对自己同意，服务器回包version
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);//被同意的对方收到通知version
    std::shared_ptr<FriendInfo> GetFriendById(int uid);

    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();  //每页拉取多少个联系人
    bool IsLoadChatFin();   //联系人是否已加载完全
    void UpdateChatLoadedCount();   //更新已加载联系人的数量
    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();   //每页拉取多少个联系人
    void UpdateContactLoadedCount();
    bool IsLoadConFin();
    std::shared_ptr<UserInfo> GetUserInfo();    //获取用户信息
    void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>>); //缓存聊天记录
private:
    UserMgr();
    std::shared_ptr<UserInfo> _user_info;
    QString _token;
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
    int _chat_loaded;
    int _contact_loaded;

};

#endif // USERMGR_H
