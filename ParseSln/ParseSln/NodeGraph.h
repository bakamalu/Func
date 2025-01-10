#pragma once

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QFont>
#include <QLineF>
#include <QDialog>
#include <QtWidgets>
#include "ParseSln.h"

class GraphNodeItem;

vector<QLineF>getPath(QPointF start, QPointF end, vector<QLineF> vecPath = {});
void showProject(string strName, int nLevel);
QPointF bezier(const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3, qreal t);
qreal distanceToCircle(const QPointF& point, const QPointF& center, qreal radius); 
QList<QPointF> getIntersectPoints(const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3,
    const QPointF& center, qreal radius, qreal epsilon = 0.001);
GraphNodeItem* getNodeItem(QGraphicsScene* scene, QString qsItem);
// 将图添加到场景中
void addGraphToScene(QGraphicsScene* scene, map<string, ProjectGraphNode*>& mapGraphNode);

class GraphNodeItem : public QGraphicsItem
{
public:
    GraphNodeItem(const QString& name, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QString getName() const;

private:
    QString m_strName;
};

class GraphDialog : public QDialog
{
public:
    GraphDialog(QGraphicsView* view, QWidget* parent = nullptr);
    ~GraphDialog();

private:
    QGraphicsView* m_pView;
};

class GraphEdgeItem : public QGraphicsItem
{
public:
    GraphEdgeItem(QGraphicsItem* start, QGraphicsItem* end, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void adjust();

protected:
    // 直线
    void drawStraightPath(QPainter* painter);
    // 曲线
    void drawCurvedPath(QPainter* painter);
    void drawCurvedPath2(QPainter* painter);
    void drawCurvedPath3(QPainter* painter);

public:
    QPointF                 m_startPoint;
    QPointF                 m_endPoint;
    QGraphicsItem           * m_start;
    QGraphicsItem           * m_end;
};

// 定义一个支持搜索的列表部件
class SearchableListWidget : public QWidget {
public:
    SearchableListWidget(const QStringList& items, QWidget* parent = nullptr);
    void createUi();
    string getValue();
    QListWidget* getList() { return m_pListWidget; }

private:
    void setItems(const QStringList& items);
    void connectSearchTextChanged();
    void updateList(const QString& filter = "");

    QLineEdit           * m_pLineEditSearch;  // 搜索框
    QListWidget         * m_pListWidget;  // 列表部件
    QStringList         m_items;  // 存储所有条目的列表
};

class CustomListWidget : public QListWidget {
public:
    explicit CustomListWidget(map<string, QStringList>&& mapPossibleRedundant, QWidget* parent = nullptr);
    void setCurItems(const string& strProjectName);

protected:
    void showContextMenu(const QPoint& pos);

private:
    void deleteSelectedItem();

    string                          m_strCurProject;
    map<string, QStringList>        m_mapPossibleRedundant;
};


class FileSelectDialog : public QDialog 
{
public:
    FileSelectDialog(QWidget* parent = nullptr);

    // 获取所选文件的路径
    QString getSelectedFilePath() const;
    bool loadUnnecessaryProect() const;
    void createUi();

private:
    void openSlnFile();

    void connectButtonClicked(QPushButton* button, std::function<void()> callback) {
        // 使用lambda表达式连接按钮点击事件
        QObject::connect(button, &QPushButton::clicked, [callback]() {
            callback();
            });
    }

    QCheckBox           * m_pChkLoadCl;
    QLineEdit           * m_pLineEdit;  // 用于显示文件路径的标签
    QString             m_selectedFilePath;  // 存储所选文件的路径
};

class MainWindow : public QWidget {
public:
    MainWindow(QStringList listItem, map<string, QStringList>&& mapPossibleRedundant, bool bLoadCl = false, QWidget* parent = nullptr);

    void createUi(const QStringList& listItem, map<string, QStringList>&& mapPossibleRedundant, bool bLoadCl);
    int getIndex();

protected:
    void connectButtonClicked(QPushButton* button, std::function<void()> callback) {
        // 使用lambda表达式连接按钮点击事件
        QObject::connect(button, &QPushButton::clicked, [callback]() {
            callback();
            });
    }

private:
    CustomListWidget                * m_pRedundant;
    QLineEdit                       * m_pLineEditProjectName;  // 用于显示所选目录的输入框
};

class FloatTextWidget : public QWidget
{
public:
    FloatTextWidget(QWidget* parent = nullptr);
    void setText(QString qsText);
protected:
    bool event(QEvent* e);

private:
    QLabel          * m_pLabel;
};

class SceneEventFilter : public QObject
{
public:
    SceneEventFilter(QGraphicsScene* scene, std::vector<GraphEdgeItem*>& edges, map<string, ProjectGraphNode*>& mapGraphNode);
    ~SceneEventFilter();

    void adjust();
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    FloatTextWidget                         * m_pFloagText;
    QGraphicsScene                          * m_scene;
    map<string, ProjectGraphNode*>          m_mapGraphNode;
    std::vector<GraphEdgeItem*>             m_edges;
};