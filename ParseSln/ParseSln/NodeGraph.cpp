#include "NodeGraph.h"

const int height_Window = 938;

vector<QLineF>getPath(QPointF start, QPointF end, vector<QLineF> vecPath)
{
    if (start == end)
    {
        return vecPath;
    }

    // 确保 start 和 end 的 x 坐标不同
    if (start.x() == end.x()) {
        vecPath.push_back(QLineF(start, end));
        start.setY(end.y());
        return getPath(start, end, vecPath);
    }
    else
    {
        vecPath.push_back(QLineF(start.x(), start.y(), end.x(), start.y()));
        start.setX(end.x());
        return getPath(start, end, vecPath);
    }
}

void showProject(string strName, int nLevel)
{
    Project* pProject = getProjectByName(strName);
    pProject->showLevel(nLevel);

    map<string, ProjectGraphNode*>& mapGraphNode = pProject->m_mapGraphNode;

    // 创建场景和视图
    QGraphicsScene* scene = new QGraphicsScene(NULL);
    QGraphicsView* view = new QGraphicsView(scene);
    view->resize(1920, height_Window);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setWindowTitle("Project Graph");

    // 将图添加到场景中
    addGraphToScene(scene, mapGraphNode);


    GraphDialog* dialog = new GraphDialog(view);
    dialog->exec();
    delete dialog;
    delete scene;
    
}

QPointF bezier(const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3, qreal t)
{
    // 计算给定t值时的贝塞尔曲线上点
    return (1 - t) * (1 - t) * (1 - t) * p0 +
        3 * (1 - t) * (1 - t) * t * p1 +
        3 * (1 - t) * t * t * p2 +
        t * t * t * p3;
}

qreal distanceToCircle(const QPointF& point, const QPointF& center, qreal radius)
{
    // 返回点到圆的距离
    return qSqrt(qPow(point.x() - center.x(), 2) + qPow(point.y() - center.y(), 2)) - radius;
}

QList<QPointF> getIntersectPoints(const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3,
    const QPointF& center, qreal radius, qreal epsilon/* = 0.001*/)
{
    QList<QPointF> intersections;
    qreal minDistance = NULL;
    QPointF minPoint;
    for (qreal t = 0; t <= 1; t += 0.0001) {  // 逐步增加t值，步长可以根据精度需求调整
        QPointF pointOnCurve = bezier(p0, p1, p2, p3, t);
        qreal distance = qAbs(distanceToCircle(pointOnCurve, center, radius));
        if (distance < epsilon) {
            // 如果点在圆上，添加到交点列表
            if (minDistance > distance || minDistance == NULL)
            {
                minDistance = distance;
                minPoint = pointOnCurve;
            }
            //intersections.append(pointOnCurve);
        }
    }
    if (minDistance != NULL)
    {
        intersections.append(minPoint);
    }
    return intersections;
}

GraphNodeItem* getNodeItem(QGraphicsScene* scene, QString qsItem)
{
    QList<QGraphicsItem*> itemList = scene->items();
    for (QGraphicsItem* item : itemList) {
        if (GraphNodeItem* graphNodeItem = dynamic_cast<GraphNodeItem*>(item)) {
            QString nodeName = graphNodeItem->getName();
            if (nodeName == qsItem) {
                return graphNodeItem;
            }

        }
    }
    return NULL;
}

void addGraphToScene(QGraphicsScene* scene, map<string, ProjectGraphNode*>& mapGraphNode)
{
    // 计算节点层次
    QMap<QString, int> nodeLayers;
    QSet<QString> visited;
    int currentLayer = 0;
    QSet<QString> nodesToProcess;

    for (auto& iter : mapGraphNode)
    {
        nodesToProcess.insert(QString::fromStdString(iter.first));
    }

    while (!nodesToProcess.isEmpty())
    {
        QSet<QString> nextLayer;
        for (const QString& nodeName : nodesToProcess)
        {
            if (!visited.contains(nodeName))
            {
                nodeLayers[nodeName] = currentLayer;
                visited.insert(nodeName);
                for (const auto& child : mapGraphNode[nodeName.toStdString()]->m_setChild)
                {
                    nextLayer.insert(QString::fromStdString(child->m_strProjectName));
                }
            }
        }
        nodesToProcess = nextLayer;
        ++currentLayer;
    }

    // 计算节点位置
    vector<GraphNodeItem*> vecNodes;

    int nMaxRelateCount = 0;
    set<int>setRelateCount;
    vector<int>quenceRelate;
    map<QString, double>mapRelateRecord;
    for (auto& iter : mapGraphNode)
    {
        QString nodeName = QString::fromStdString(iter.first);
        int layer = nodeLayers[nodeName];
        GraphNodeItem* nodeItem = new GraphNodeItem(nodeName);
        vecNodes.push_back(nodeItem);
        scene->addItem(nodeItem);

        ProjectGraphNode* pNode = iter.second;

        // nRelate 大的在下方
        int nRelate = pNode->depth();
        mapRelateRecord[QString::fromStdString(pNode->m_strProjectName)] = nRelate;
        nMaxRelateCount = max(nMaxRelateCount, nRelate);
        if (setRelateCount.find(nRelate) == setRelateCount.end())
        {
            setRelateCount.insert(nRelate);
            quenceRelate.push_back(nRelate);
        }
    }

    // 关联项目越少 高度越高 y值越小
    std::sort(quenceRelate.begin(), quenceRelate.end()); // 对 vector 进行排序 从小到大

    // 同一行的个数
    map<double, int>mapCountInLevel;
    for (auto& iter : mapRelateRecord)
    {
        for (int nIndex = 0; nIndex < quenceRelate.size(); ++nIndex)
        {
            if (iter.second == quenceRelate[nIndex])
            {
                iter.second = (nIndex + 1.0) / quenceRelate.size();
                ++mapCountInLevel[iter.second];
                break;
            }
        }
    }

    //同一行已经布置的个数
    map<double, int>mapPosInLevel;
    auto getLinePos = [&mapPosInLevel](double dHeightFator)
    {
        auto iter = mapPosInLevel.find(dHeightFator);
        if (iter == mapPosInLevel.end())
        {
            mapPosInLevel[dHeightFator] = 1;
            return 1;
        }
        else
        {
            return ++iter->second;
        }
    };

    const int widthNode = 80;
    map<double, double>mapXInLevel;
    double dTempLeft = 0;

    for (GraphNodeItem* pNode : vecNodes)
    {
        dTempLeft += widthNode;
        double dFactor = mapRelateRecord[pNode->getName()];
        double y = dFactor * height_Window;
        int nCurLevelPos = getLinePos(dFactor);
        double dOffset = (height_Window - pNode->boundingRect().height()) / setRelateCount.size() * (nCurLevelPos - 1) / mapCountInLevel[dFactor];
        y += dOffset;
        double dRealLeft = dTempLeft;
        if (mapXInLevel.find(dFactor) != mapXInLevel.end())
        {
            mapXInLevel[dFactor] += widthNode;
            dRealLeft = mapXInLevel[dFactor];
            dTempLeft -= widthNode;
        }
        else
        {
            mapXInLevel[dFactor] = dTempLeft;
        }
        pNode->setPos(dRealLeft, y);
    }

    std::vector<GraphEdgeItem*> vec_Edges;
    // 添加边
    for (auto& iter : mapGraphNode)
    {
        GraphNodeItem* nodeItem = getNodeItem(scene, QString::fromStdString(iter.second->m_strProjectName));
        for (const auto& child : iter.second->m_setChild)
        {
            GraphNodeItem* childItem = getNodeItem(scene, QString::fromStdString(child->m_strProjectName));
            if (nodeItem && childItem)
            {
                GraphEdgeItem* edgeItem = new GraphEdgeItem(nodeItem, childItem);
                scene->addItem(edgeItem);
                vec_Edges.push_back(edgeItem);
            }
        }
    }


    SceneEventFilter* eventFilter = new SceneEventFilter(scene, vec_Edges, mapGraphNode);
    scene->installEventFilter(eventFilter);
    eventFilter->adjust();
}

////////////////////////GraphNodeItem///////////////////////////////////////////////////
GraphNodeItem::GraphNodeItem(const QString& name, QGraphicsItem* parent /*= nullptr*/)
    : QGraphicsItem(parent), m_strName(name)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    //setCacheMode(DeviceCoordinateCache);
}

QRectF GraphNodeItem::boundingRect() const
{
    return QRectF(-30, -30, 60, 60);
}

// 将文本分割成多行，确保每行宽度不超过指定的最大宽度
QStringList splitTextToFit(const QString& text, const QFontMetrics& fm, int maxWidth) {
    QStringList result;
    QString currentLine;
    foreach(const QString & word, text.split(' ')) {
        if (fm.horizontalAdvance(currentLine + word + ' ') <= maxWidth) {
            currentLine += word + ' ';
        }
        else {
            result << currentLine.trimmed();
            currentLine = word + ' ';
        }
    }
    if (!currentLine.isEmpty()) {
        result << currentLine.trimmed();
    }
    return result;
}

void GraphNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 绘制节点背景
    painter->setBrush(Qt::lightGray);
    painter->drawEllipse(boundingRect());

    // 绘制节点名称
    painter->setPen(Qt::black);
    QFont font = QFont("Arial", 8);
    painter->setFont(font);


    painter->drawText(boundingRect(), Qt::AlignCenter, m_strName);
}

QString GraphNodeItem::getName() const
{
    return m_strName;
}

////////////////////////GraphDialog///////////////////////////////////////////////////
GraphDialog::GraphDialog(QGraphicsView* view, QWidget* parent /*= nullptr*/)
    : QDialog(parent)
    , m_pView(view)
{
    resize(view->size());
    // 创建布局管理器
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_pView);

    // 设置对话框的布局
    setLayout(layout);
}

GraphDialog::~GraphDialog()
{

}

////////////////////////GraphEdgeItem///////////////////////////////////////////////////
GraphEdgeItem::GraphEdgeItem(QGraphicsItem* start, QGraphicsItem* end, QGraphicsItem* parent /*= nullptr*/)
    : QGraphicsItem(parent),
    m_start(start),
    m_end(end)
{
    setZValue(-1.0);  // 确保边在节点下方
}

QRectF GraphEdgeItem::boundingRect() const
{
    return QRectF(m_startPoint, m_endPoint).normalized();
}

void GraphEdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //drawStraightPath(painter);
    //drawCurvedPath(painter);
    //drawCurvedPath2(painter);
    if (isVisible())
    {
        drawCurvedPath3(painter);
    }
}

void GraphEdgeItem::adjust()
{
    if (!m_start || !m_end)
        return;

    m_startPoint = m_start->mapToScene(m_start->boundingRect().center());
    m_endPoint = m_end->mapToScene(m_end->boundingRect().center());
    prepareGeometryChange();
    update();
}

void GraphEdgeItem::drawStraightPath(QPainter* painter)
{
    vector<QLineF> vecLines = getPath(m_startPoint, m_endPoint);
    for (int i = 0; i < vecLines.size(); ++i)
    {
        const QLineF& line = vecLines[i];
        painter->drawLine(line);
        if (i == vecLines.size() - 1)
        {
            qreal arrowLength = 10;  // 箭头长度，可根据实际需求调整
            qreal arrowWidth = 5;    // 箭头宽度，可调整

            QPointF endPoint = m_endPoint;
            //画箭头
            if (line.dx() == 0) {
                // 垂直连线情况（沿y方向），箭头在终点上方或下方
                // 箭头向下
                bool bDown = line.dy() > 0;
                int nDirection = bDown ? -1 : 1;
                endPoint.setY(endPoint.y() + nDirection * m_end->boundingRect().height() / 2);
                QPointF arrowTip = endPoint;
                QPointF arrowLeft = endPoint + nDirection * QPointF(-arrowWidth / 2, arrowLength);
                QPointF arrowRight = endPoint + nDirection * QPointF(arrowWidth / 2, arrowLength);
                QPolygonF arrowHead;
                arrowHead << arrowTip << arrowLeft << arrowRight;
                painter->drawPolygon(arrowHead);
            }
            else if (line.dy() == 0) {
                // 水平连线情况（沿x方向），箭头在终点左侧或右侧
                // 箭头向右
                bool bRight = line.dx() > 0;
                int nDirection = bRight ? -1 : 1;
                endPoint.setX(endPoint.x() + nDirection * m_end->boundingRect().width() / 2);
                QPointF arrowTip = endPoint;
                QPointF arrowTop = endPoint + nDirection * QPointF(arrowLength, -arrowWidth / 2);
                QPointF arrowBottom = endPoint + nDirection * QPointF(arrowLength, arrowWidth / 2);
                QPolygonF arrowHead;
                arrowHead << arrowTip << arrowTop << arrowBottom;
                painter->drawPolygon(arrowHead);
            }
        }
    }
}

void GraphEdgeItem::drawCurvedPath(QPainter* painter)
{
    qreal radius1 = m_start->boundingRect().width() / 2;
    qreal radius2 = m_end->boundingRect().width() / 2;
    // 创建路径对象
    QPainterPath path;
    // 将路径的起点设置为第一个圆边缘上最左侧的点
    path.moveTo(m_startPoint + QPointF(-radius1, 0));
    // 添加贝塞尔曲线段，这里的控制点可以根据需要调整
    QPointF controlPoint1(m_startPoint.x() - radius1 * 2, m_startPoint.y());
    // 终点在圆最上侧的地方
    QPointF controlPoint2(m_endPoint.x(), m_endPoint.y() - radius2 * 2);
    path.cubicTo(controlPoint1, controlPoint2, m_endPoint + QPointF(-radius2, 0));
    // 设置画笔和画刷属性
    QPen pen(Qt::black, 2);
    painter->setPen(pen);
    // 绘制路径
    painter->drawPath(path);
}

void GraphEdgeItem::drawCurvedPath2(QPainter* painter)
{
    qreal radius1 = m_start->boundingRect().width() / 2;
    qreal radius2 = m_end->boundingRect().width() / 2;
    // 分割角度，用于将圆弧近似为多个线段，角度越小越平滑
    qreal angleStep = 5;
    qreal angle = 0;
    QPointF prevPoint;
    QPen pen(Qt::black, 2);
    painter->setPen(pen);

    // 从第一个圆的边缘开始绘制线段
    prevPoint = m_startPoint + QPointF(radius1 * std::cos(angle * M_PI / 180.0), radius1 * std::sin(angle * M_PI / 180.0));
    for (angle = angleStep; angle <= 180; angle += angleStep)
    {
        QPointF currentPoint = m_startPoint + QPointF(radius1 * std::cos(angle * M_PI / 180.0), radius1 * std::sin(angle * M_PI / 180.0));
        painter->drawLine(prevPoint, currentPoint);
        prevPoint = currentPoint;
    }
    // 从第一个圆的边缘连接到第二个圆的边缘
    QPointF connectionPoint1 = m_startPoint + QPointF(radius1 * std::cos(180 * M_PI / 180.0), radius1 * std::sin(180 * M_PI / 180.0));
    QPointF connectionPoint2 = m_endPoint + QPointF(radius2 * std::cos(180 * M_PI / 180.0), radius2 * std::sin(180 * M_PI / 180.0));
    painter->drawLine(connectionPoint1, connectionPoint2);
    // 从第二个圆的边缘继续绘制线段
    prevPoint = connectionPoint2;
    for (angle = 180 - angleStep; angle >= 0; angle -= angleStep)
    {
        QPointF currentPoint = m_endPoint + QPointF(radius2 * std::cos(angle * M_PI / 180.0), radius2 * std::sin(angle * M_PI / 180.0));
        painter->drawLine(prevPoint, currentPoint);
        prevPoint = currentPoint;
    }
}

void GraphEdgeItem::drawCurvedPath3(QPainter* painter)
{
    qreal radius1 = m_start->boundingRect().width() / 2;
    qreal radius2 = m_end->boundingRect().width() / 2;

    int nOffset = 100;
    // 创建路径对象
    QPainterPath path;
    // 将路径的起点设置为第一个圆边缘上最左侧的点
    path.moveTo(m_startPoint);
    // 添加贝塞尔曲线段，这里的控制点可以根据需要调整
    QPointF controlPoint1(min(m_startPoint.x(), m_endPoint.x()) - nOffset, m_startPoint.y());
    QPointF controlPoint2(min(m_startPoint.x(), m_endPoint.x()) - nOffset, m_endPoint.y());
    path.cubicTo(controlPoint1, controlPoint2, m_endPoint);
    // 设置画笔和画刷属性
    QPen pen(Qt::black, 1);
    painter->setPen(pen);
    // 绘制路径
    painter->drawPath(path);

    QList<QPointF>lstIntersection = getIntersectPoints(m_startPoint, controlPoint1, controlPoint2, m_endPoint, m_endPoint, m_end->boundingRect().width() / 2, 1);

    if (lstIntersection.size() == 0)
    {
        cout << "Lack of precision" << endl;
    }
    else
    {
        qreal arrowSize = 10;  // 箭头的大小可以调整
        qreal arrowAngle = 30.0;  // 箭头的角度，单位为度
        qreal angleRad = arrowAngle * M_PI / 180.0;  // 转换为弧度
        QPointF pIntersection = lstIntersection[0];
        QPointF center = m_endPoint;
        // 计算箭头方向
        QPointF direction = pIntersection - center;
        qreal length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (length > 0) {
            direction /= length;  // 归一化方向向量
        }

        // 计算箭头两边的点
        QPointF pointLeft = pIntersection + arrowSize * 
            QPointF(
                direction.x() * std::cos(angleRad) + direction.y() * std::sin(angleRad), 
                -direction.x() * std::sin(angleRad) + direction.y() * std::cos(angleRad));
        QPointF pointRight = pIntersection + arrowSize *
            QPointF(
                direction.x() * std::cos(angleRad) - direction.y() * std::sin(angleRad),
                direction.x() * std::sin(angleRad) + direction.y() * std::cos(angleRad));

        // 创建多边形表示箭头
        QPolygonF arrowHead;
        arrowHead << pIntersection << pointLeft << pointRight;

        // 填充并绘制箭头
        painter->setBrush(Qt::black);  // 可以根据需要改变颜色
        painter->drawPolygon(arrowHead);
    }
}

SearchableListWidget::SearchableListWidget(const QStringList& items, QWidget* parent /*= nullptr*/)
    : QWidget(parent)
    , m_pLineEditSearch(NULL)
    , m_pListWidget(NULL)
{
    createUi();
    setItems(items);
}

void SearchableListWidget::createUi()
{
    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);

    // 创建搜索框
    m_pLineEditSearch = new QLineEdit(this);
    layout->addWidget(m_pLineEditSearch);

    // 创建列表部件
    m_pListWidget = new QListWidget(this);
    layout->addWidget(m_pListWidget);

    // 连接搜索框的文本变化事件
    connectSearchTextChanged();
}

std::string SearchableListWidget::getValue()
{
    if (m_pListWidget)
    {
        return m_pListWidget->currentItem()->text().toStdString();
    }
    else
    {
        return "";
    }
}

void SearchableListWidget::setItems(const QStringList& items)
{
    m_items = items;
    updateList();
}

void SearchableListWidget::connectSearchTextChanged()
{
    // 使用lambda表达式连接搜索框的文本变化事件
    QObject::connect(m_pLineEditSearch, &QLineEdit::textChanged, [this](const QString& text) {
        updateList(text);
        });
}

void SearchableListWidget::updateList(const QString& filter /*= ""*/)
{
    m_pListWidget->clear();

    if (filter.isEmpty()) 
    {
        for (const QString& item : m_items) 
        {
            m_pListWidget->addItem(item);
        }
    }
    else 
    {
        for (const QString& item : m_items) 
        {
            if (item.contains(filter, Qt::CaseInsensitive)) 
            {
                m_pListWidget->addItem(item);
            }
        }
    }
}

CustomListWidget::CustomListWidget(map<string, QStringList>&& mapPossibleRedundant, QWidget* parent/* = nullptr*/)
    : QListWidget(parent)
{
    m_mapPossibleRedundant.swap(mapPossibleRedundant);
    // 设置上下文菜单策略为自定义菜单
    setContextMenuPolicy(Qt::CustomContextMenu);

    // 连接 customContextMenuRequested 信号到槽函数
    connect(this, &CustomListWidget::customContextMenuRequested, [this](const QPoint& pos) {
        showContextMenu(pos);
        });
}

void CustomListWidget::setCurItems(const string& strProjectName)
{
    m_strCurProject = strProjectName;
    clear();
    for (const QString& item : m_mapPossibleRedundant[m_strCurProject])
    {
        addItem(item);
    }

}

void CustomListWidget::showContextMenu(const QPoint& pos)
{
    // 获取全局位置
    QPoint globalPos = mapToGlobal(pos);

    // 创建上下文菜单
    QMenu contextMenu(tr("Context menu"), this);

    // 添加“删除”动作
    QAction* deleteAction = contextMenu.addAction(tr("Delete"));

    // 显示上下文菜单并获取用户选择的动作
    QAction* selectedAction = contextMenu.exec(globalPos);

    // 如果用户选择了“删除”，则删除选定项
    if (selectedAction == deleteAction) {
        deleteSelectedItem();
    }
}

void CustomListWidget::deleteSelectedItem()
{
    QListWidgetItem* pSelectItem = currentItem();
    if (NULL != pSelectItem)
    {
        m_mapPossibleRedundant[m_strCurProject].removeAll(pSelectItem->text());
        delete pSelectItem;
        update();
    }
}

FileSelectDialog::FileSelectDialog(QWidget* parent /*= nullptr*/)
    : QDialog(parent)
    , m_selectedFilePath("")
    , m_pLineEdit(NULL)
    , m_pChkLoadCl(NULL)
{
    // 设置对话框的默认大小
    setMinimumSize(300, 150);
    createUi();
}

QString FileSelectDialog::getSelectedFilePath() const
{
    return m_selectedFilePath;
}

bool FileSelectDialog::loadUnnecessaryProect() const
{
    if (m_pChkLoadCl)
    {
        return m_pChkLoadCl->isChecked();
    }
    return false;
}

void FileSelectDialog::createUi()
{
    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);

    // 选择按钮
    QPushButton* pButtonSelect = new QPushButton("Select", this);
    layout->addWidget(pButtonSelect);

    // 创建标签
    m_pLineEdit = new QLineEdit("", this);
    layout->addWidget(m_pLineEdit);

    m_pChkLoadCl = new QCheckBox("load possible unnecessary project", this);
    layout->addWidget(m_pChkLoadCl);

    // 确认按钮
    QPushButton* pButtonConfrim = new QPushButton("confirm", this);
    pButtonConfrim->setFixedWidth(80);
    layout->addWidget(pButtonConfrim, 0, Qt::AlignHCenter);

    // 连接按钮点击事件
    connectButtonClicked(pButtonSelect, [this]() {
        openSlnFile();
        });

    connectButtonClicked(pButtonConfrim, [this]() {
        accept();
        });
}

void FileSelectDialog::openSlnFile()
{
    // 设置文件对话框的过滤器，只显示 .sln 文件
    QString filter = "Solution Files (*.sln)";

    // 弹出文件选择对话框
    QString filePath = QFileDialog::getOpenFileName(
        this,  // 父窗口
        tr("Open Solution File"),  // 对话框标题
        "",  // 默认目录（空字符串表示当前目录）
        filter  // 文件过滤器
    );

    if (!filePath.isEmpty()) {
        // 用户选择了文件
        m_selectedFilePath = filePath;
        m_pLineEdit->setText(filePath);
    }
    //         else {
    //             // 用户取消了选择
    //             QMessageBox::information(this, tr("No File Selected"), tr("No file was selected."));
    //         }
}

////////////////////////MainWindow///////////////////////////////////////////////////
MainWindow::MainWindow(QStringList listItem, map<string, QStringList>&& mapPossibleRedundant, bool bLoadCl/* = false*/, QWidget* parent /*= nullptr*/)
    : QWidget(parent)
    , m_pRedundant(NULL)
{
    // 设置窗口大小
    setMinimumSize(400, 300);
    createUi(listItem, std::move(mapPossibleRedundant), bLoadCl);
}

void MainWindow::createUi(const QStringList& listItem, map<string, QStringList>&& mapPossibleRedundant, bool bLoadCl)
{
    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* pLabel = new QLabel(this);
    pLabel->setText("input level above this project(default 2)");
    layout->addWidget(pLabel);
    // 创建输入框
    m_pLineEditProjectName = new QLineEdit(this);
    layout->addWidget(m_pLineEditProjectName);

    pLabel = new QLabel("select Project", this);
    layout->addWidget(pLabel);

    // 创建列表部件
    SearchableListWidget* pListWidget = new SearchableListWidget(listItem, this);
    layout->addWidget(pListWidget);

    QPushButton* button = new QPushButton("confirm", this);
    layout->addWidget(button);

    if (bLoadCl)
    {
        pLabel = new QLabel("posible unnecessary Project", this);
        layout->addWidget(pLabel);
        m_pRedundant = new CustomListWidget(std::move(mapPossibleRedundant), this);
        layout->addWidget(m_pRedundant);

        if (pListWidget->getList()->currentItem())
        {
            m_pRedundant->setCurItems(pListWidget->getList()->currentItem()->text().toStdString());
        }
        connect(pListWidget->getList(), &QListWidget::itemSelectionChanged, [this, pListWidget]() {
            QListWidgetItem* pCurItem = pListWidget->getList()->currentItem();
            if (pCurItem)
            {
                m_pRedundant->setCurItems(pCurItem->text().toStdString());
            }
            });
    }


    // 连接按钮点击事件
    connectButtonClicked(button, [this, pListWidget]() {
        //selectDirectory();
        string strName = pListWidget->getValue();
        if (!strName.empty())
        {
            showProject(strName, getIndex());
        }
        });


}

int MainWindow::getIndex()
{
    if (!m_pLineEditProjectName || m_pLineEditProjectName->text().isEmpty())
    {
        return 2;
    }
    return m_pLineEditProjectName->text().toInt();
}

////////////////////////FloatTextWidget///////////////////////////////////////////////////
FloatTextWidget::FloatTextWidget(QWidget* parent /*= nullptr*/)
    : QWidget(parent)
    , m_pLabel(NULL)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose, false);
    //setFocusPolicy(Qt::StrongFocus);
    m_pLabel = new QLabel(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_pLabel);

    // 设置对话框的布局
    setLayout(layout);
}

void FloatTextWidget::setText(QString qsText)
{
    if (m_pLabel)
    {
        m_pLabel->setText(qsText);
        //resize(m_pLabel->size());
    }
}

bool FloatTextWidget::event(QEvent* e)
{
    if (e->type() == QEvent::WindowDeactivate)
    {
        //close();
    }

    return QWidget::event(e);
}

////////////////////////SceneEventFilter///////////////////////////////////////////////////
SceneEventFilter::SceneEventFilter(QGraphicsScene* scene, std::vector<GraphEdgeItem*>& edges, map<string, ProjectGraphNode*>& mapGraphNode)
    : QObject(scene)
    , m_pFloagText(new FloatTextWidget())
    , m_scene(scene)
    , m_edges(edges)
    , m_mapGraphNode(mapGraphNode)
{
}

SceneEventFilter::~SceneEventFilter()
{
    delete m_pFloagText;
}

void SceneEventFilter::adjust()
{
    for (GraphEdgeItem* edge : m_edges)
    {
        edge->adjust();
    }
}

bool SceneEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_scene && event->type() == QEvent::GraphicsSceneMousePress)
    {
        QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            // 右键点击事件

            QGraphicsItem* itemUnderMouse = m_scene->itemAt(mouseEvent->scenePos(), QTransform());

            // 尝试将 itemUnderMouse 转换为 GraphNodeItem
            GraphNodeItem* nodeItem = dynamic_cast<GraphNodeItem*>(itemUnderMouse);
            if (NULL != nodeItem && m_pFloagText)
            {
                m_pFloagText->setText(nodeItem->getName());
                m_pFloagText->show();
                m_pFloagText->move(QCursor::pos());
            }
            return true;  // 事件已处理
        }
        else if (m_pFloagText)
        {
            m_pFloagText->hide();
        }
    }
    if (watched == m_scene && event->type() == QEvent::GraphicsSceneMouseMove) {
        // 检测到图形场景中有鼠标移动事件，遍历所有边及关联节点判断是否有节点被拖动
        QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
        QGraphicsItem* itemUnderMouse = m_scene->itemAt(mouseEvent->scenePos(), QTransform());
        for (GraphEdgeItem* edge : m_edges)
        {
            if (itemUnderMouse == edge->m_start || itemUnderMouse == edge->m_end) {
                // 找到对应的边，根据被拖动的节点更新边的属性
                if (itemUnderMouse == edge->m_start) {
                    edge->m_start->setPos(mouseEvent->scenePos());
                    edge->adjust();
                }
                else if (itemUnderMouse == edge->m_end) {
                    edge->m_end->setPos(mouseEvent->scenePos());
                    edge->adjust();
                }
            }
        }
        m_scene->update();
    }
    else if (watched == m_scene && event->type() == QEvent::GraphicsSceneMouseDoubleClick)
    {
        // 检测到图形场景中有鼠标点击事件
        QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
        QGraphicsItem* itemUnderMouse = m_scene->itemAt(mouseEvent->scenePos(), QTransform());

        // 尝试将 itemUnderMouse 转换为 GraphNodeItem
        GraphNodeItem* nodeItem = dynamic_cast<GraphNodeItem*>(itemUnderMouse);
        for (GraphEdgeItem* edge : m_edges)
        {
            edge->setVisible(NULL == nodeItem);
            edge->update();
        }
        m_scene->update();
        if (NULL != nodeItem)
        {
            ProjectGraphNode* pNode = m_mapGraphNode[nodeItem->getName().toStdString()];
            set<string>setShowNode;
            setShowNode.insert(pNode->m_strProjectName);
            for (GraphEdgeItem* edge : m_edges)
            {
                for (QGraphicsItem* item : { edge->m_start ,edge->m_end })
                {
                    GraphNodeItem* grahItem = dynamic_cast<GraphNodeItem*>(item);
                    if (grahItem)
                    {
                        string strName = grahItem->getName().toStdString();
                        if (setShowNode.find(strName) != setShowNode.end())
                        {
                            edge->setVisible(true);
                            break;
                        }
                    }
                }
            }
        }
    }
    return false;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    FileSelectDialog* fileDialog = new FileSelectDialog();
    if (fileDialog->exec() == QDialog::Accepted)
    {
        bool bLoadCl = fileDialog->loadUnnecessaryProect();
        string strFilePath = fileDialog->getSelectedFilePath().toStdString();
        std::replace(strFilePath.begin(), strFilePath.end(), '\\', '/');
        ProjectAnalyze project(strFilePath, bLoadCl);


        QStringList strLstNode;
        map<string, QStringList>mapPossibleRedundant;
        for (const auto& iter : Project::s_mapProject)
        {
            Project* pNode = iter.second;
            if (pNode && !pNode->m_strName.empty())
            {
                strLstNode.push_back(QString::fromStdString(pNode->m_strName));
                if (bLoadCl)
                {
                    for (const string& strRedundant : pNode->m_setPossibleRedundant)
                    {
                        mapPossibleRedundant[pNode->m_strName].push_back(QString::fromStdString(strRedundant));
                    }
                }
            }
        }
        MainWindow* pWindow = new MainWindow(strLstNode, std::move(mapPossibleRedundant), bLoadCl);
        pWindow->show();
    }
    return app.exec();
}
