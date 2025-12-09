#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include "statewidget.h"
#include "userdata.h"
#include <QListWidgetItem>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void addChatUserList(); //测试函数
    void ClearLabelState(StateWidget* lb);  //清空StateWidget状态
protected:
    bool eventFilter(QObject* watched,QEvent* event) override;
    void handleGlobalMousePress(QMouseEvent *event);
private:
    void ShowSearch(bool bsearch = false);
    void AddLBGroup(StateWidget* lb);   //添加label组
    void SetSelectChatItem(int uid = 0);    //设置当前选中的chatitem
    void SetSelectChatPage(int uid = 0);    //设置当前选中的chatpage
    void loadMoreChatUser();    //加载更多联系人（聊天page）
    void loadMoreConUser();     //加载更多的联系人
    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>> msgdata); //根据当前聊天的人决定是否更新聊天框消息
    Ui::ChatDialog *ui;
    ChatUIMode _mode;   //模式，用于控制sidebar
    ChatUIMode _state;  //状态，在不同模式下都可以搜索
    bool _b_loading;    //负责加载
    QList<StateWidget*> _lb_list;
    QMap<int, QListWidgetItem*> _chat_items_added;  //存储添加过的聊天条目
    int _cur_chat_uid;  //当前正在聊天的uid
    QWidget* _last_widget;  //保存前一个界面的信息
private slots:
    void slot_loading_chat_user();  //加载对话框
    void slot_side_chat();      //切换到聊天page
    void slot_side_contact();   //切换到联系人page
    void slot_text_changed(const QString & str);    //搜索框输入变化的槽函数
    void slot_loading_contact_user();
public slots:
    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply);  //收到，处理添加好友请求
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info); //被同意方收到通知
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);          //同意方收到回包
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> si);       //跳转到聊天框
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);//跳转到好友详情页
    void slot_switch_apply_friend_page();                           //跳转到新的朋友页
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info);    //从好友详情页跳转到聊天框
    void slot_item_clicked(QListWidgetItem *item);                  //聊天界面条目被点击
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata);  //将消息缓存
    void slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg);      //收到对端消息的槽函数
};

#endif // CHATDIALOG_H
