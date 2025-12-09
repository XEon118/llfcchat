#include "searchlist.h"
#include <QScrollBar>
#include "adduseritem.h"
// #include "invaliditem.h"
#include "findsuccessdlg.h"
#include "tcpmgr.h"
#include "customizeedit.h"
#include "findfaildlg.h"
#include "loadingdlg.h"
#include <QJsonDocument>
#include "usermgr.h"

SearchList::SearchList(QWidget *parent):QListWidget(parent),_find_dlg(nullptr), _search_edit(nullptr), _send_pending(false)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    //添加条目
    addTipItem();
    //连接搜索条目
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::CloseFindDlg()
{
    if(_find_dlg){
        _find_dlg->hide();
        _find_dlg = nullptr;
    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{
    _search_edit = edit;
}

void SearchList::waitPending(bool pending)
{
    if(pending){
        //发送中
        _loadingDialog = new LoadingDlg(this);
        _loadingDialog->setModal(true);
        _loadingDialog->show();
        _send_pending = true;
    }else{
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
        _send_pending = pending;    //false
    }
}

void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);  //不可选中


    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); //获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }
    // 获取到的是不可点击的类型
    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    if(itemType == ListItemType::ADD_USER_TIP_ITEM){
        if(_send_pending){
            //正处于发送状态
            return;
        }
        //保证即使_search_edit为空也不崩溃
        if(!_search_edit){
            return;
        }

        //发送，置一个loading蒙板
        waitPending(true);
        auto search_edit = dynamic_cast<CustomizeEdit*>(_search_edit);
        auto uid_str = search_edit->text(); //昵称或uid
        QJsonObject jsonObj;
        jsonObj["uid"] = uid_str;
        //序列化
        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);   //压缩的方式
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_SEARCH_USER_REQ,
                                                      jsonData);
        return;
    }

    //清除弹出框
    CloseFindDlg();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
    waitPending(false);
    if(si == nullptr){
        //查找失败对话框
        _find_dlg = std::make_shared<FindFailDlg>(this);
    }else{
        //如果是自己，暂且先直接返回，以后看逻辑扩充
        auto self_uid = UserMgr::GetInstance()->GetUid();
        if (si->_uid == self_uid) {
            return;
        }
        //查到了，已经是好友或者未添加好友
        //查找是否已经是好友
        bool bExist = UserMgr::GetInstance()->CheckFriendById(si->_uid);
        if(bExist){
            //此处处理已经添加的好友，实现页面跳转
            //跳转到聊天界面指定的item中
            emit sig_jump_chat_item(si);
            return;
        }
        //不是好友
        _find_dlg = std::make_shared<FindSuccessDlg>(this);
        std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg)->SetSearchInfo(si);
    }

    _find_dlg->show();
}
