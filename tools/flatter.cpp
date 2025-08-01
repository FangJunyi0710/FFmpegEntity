#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QPixmap>
#include <QLineEdit>
#include <QDateTime>
#include <QSpinBox>
#include <QFile>
#include "AVOutput.h"
#include "VideoEncoder.h"

class GraphicsView : public QGraphicsView {
	Q_OBJECT
public:
	GraphicsView(QWidget *parent = nullptr) : QGraphicsView(parent), isDragging(false) {
		setDragMode(QGraphicsView::ScrollHandDrag);
		setRenderHint(QPainter::Antialiasing);
		setRenderHint(QPainter::SmoothPixmapTransform);
		setInteractive(true);
		setResizeAnchor(QGraphicsView::AnchorViewCenter);
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	}
signals:
	void mouseMoved(QPointF position);
	void mouseClicked(QPointF position);
	void rightClicked(QPointF position);
protected:
	bool isDragging;
	void wheelEvent(QWheelEvent *event) override {
		// 缩放因子
		const double scaleFactor = 1.1;
		if (event->angleDelta().y() > 0) {
			scale(scaleFactor, scaleFactor); // 放大
		} else {
			scale(1.0 / scaleFactor, 1.0 / scaleFactor); // 缩小
		}
	}
	void mouseMoveEvent(QMouseEvent *event) override{
		// 标记为拖动状态（鼠标移动时触发）
		isDragging = true;
		// 获取鼠标在场景中的坐标
		emit mouseMoved(mapToScene(event->pos()));
		QGraphicsView::mouseMoveEvent(event);
	}
	void mousePressEvent(QMouseEvent *event) override {
		// 鼠标按下时重置拖动状态（可能开始新的操作）
		isDragging = false;
		QGraphicsView::mousePressEvent(event);
	}
	void mouseReleaseEvent(QMouseEvent *event) override {
		// 如果未触发拖动（即纯点击操作）
		if (!isDragging) {
			// 获取点击位置并发出点击信号
			QPointF scenePos = mapToScene(event->pos());
			if (event->button() == Qt::RightButton) {
				emit rightClicked(scenePos);
			} else {
				emit mouseClicked(scenePos);
			}
		}
		// 重置拖动状态
		isDragging = false;
		QGraphicsView::mouseReleaseEvent(event);
	}
};

class MainWindow : public QWidget {
public:
	MainWindow(QWidget *parent = nullptr) : QWidget(parent){
		setupUI();
		connectSignals();
		setIndex();
	}

protected:
	QLineEdit *pathLabel;
	QLabel *coordLabel;
	GraphicsView *graphicsView;
	QGraphicsScene *scene;
	QVector<QPointF> clickHistory;
	QVector<QPointF> currentClick;
	QVector<QGraphicsRectItem*> rects;
	QSpinBox *widthSpinBox,*heightSpinBox;
	QPixmap pixmap;
	int index=0;
	void setupUI(){
		// 设置窗口标题
		setWindowTitle("Flatter");
		
		// 创建布局
		QVBoxLayout *mainLayout = new QVBoxLayout;
		setLayout(mainLayout);

		QHBoxLayout *topLayout = new QHBoxLayout;
		mainLayout->addLayout(topLayout);

		// 添加图片路径
		pathLabel = new QLineEdit("resource/1.jpg", this);
		topLayout->addWidget(pathLabel);
		
		// 添加宽度SpinBox
		widthSpinBox = new QSpinBox(this);
		widthSpinBox->setRange(1, 9999);
		widthSpinBox->setValue(1);
		topLayout->addWidget(new QLabel("宽度:", this));
		topLayout->addWidget(widthSpinBox);
		
		// 添加高度SpinBox
		heightSpinBox = new QSpinBox(this);
		heightSpinBox->setRange(1, 9999);
		heightSpinBox->setValue(1);
		topLayout->addWidget(new QLabel("高度:", this));
		topLayout->addWidget(heightSpinBox);

		// 添加鼠标坐标显示标签
		coordLabel = new QLabel("鼠标坐标: (0, 0)", this);
		topLayout->addWidget(coordLabel);
		
		// 添加图像交互界面
		graphicsView = new GraphicsView(this);
		scene = new QGraphicsScene(this);
		graphicsView->setScene(scene);
		mainLayout->addWidget(graphicsView);
	}
	void connectSignals(){
		connect(pathLabel,&QLineEdit::returnPressed,this,&MainWindow::loadImage);
		connect(graphicsView,&GraphicsView::mouseMoved,this,&MainWindow::updateCoord);
		connect(graphicsView,&GraphicsView::mouseClicked,this,&MainWindow::handleClick);
		connect(graphicsView,&GraphicsView::rightClicked,this,&MainWindow::handleRightClick);
	}
	void loadImage(){
		QPixmap tmp;
		tmp.load(pathLabel->text());
		if (!tmp.isNull()) {
			pixmap=tmp;

			scene->clear();
			clickHistory.clear();
			currentClick.clear();
			rects.clear();
			scene->addPixmap(pixmap);
			scene->setSceneRect(pixmap.rect());
		}
	}
	void updateCoord(QPointF position){
		coordLabel->setText(QString("鼠标坐标: (%1, %2)").arg(QString::number(position.x()), QString::number(position.y())));
	}
	void handleClick(QPointF position) {
		// 检测是否靠近历史点击点
		const double snapDistance = 10.0;
		for (const QPointF &historyPoint : clickHistory) {
			if (QLineF(position, historyPoint).length() <= snapDistance) {
				position = historyPoint;
				break;
			}
		}
		
		// 记录当前点击点
		currentClick.append(position);
		clickHistory.append(position);
		
		if (currentClick.size() == 4) {
			drawQuadrilateral();
			processImage();
			currentClick.clear();
		}
		
		repaintRects();
	}

	void handleRightClick(QPointF position) {
		const double cancelDistance = 10.0;
		
		// 检查当前点击点
		for (auto it = currentClick.begin(); it != currentClick.end(); ) {
			if (QLineF(position, *it).length() <= cancelDistance) {
				it = currentClick.erase(it);
				break;
			} else {
				++it;
			}
		}
		
		// 检查历史点击点
		for (auto it = clickHistory.begin(); it != clickHistory.end(); ) {
			if (QLineF(position, *it).length() <= cancelDistance) {
				it = clickHistory.erase(it);
				break;
			} else {
				++it;
			}
		}

		repaintRects();
	}
	void repaintRects(){
		// 清除并重新绘制所有点
		for(auto rectItem : rects){
			scene->removeItem(rectItem);
			delete rectItem;
		}
		rects.clear();
		
		// 绘制历史点击点（黄色）
		for (const QPointF &historyPoint : clickHistory) {
			QGraphicsRectItem* rectItem = new QGraphicsRectItem;
			rects.append(rectItem);
			scene->addItem(rectItem);
			rectItem->setBrush(QBrush(Qt::yellow));
			const double size=5;
			rectItem->setRect(historyPoint.x() - size/2, historyPoint.y() - size/2, size, size);
		}
		
		// 绘制当前点击点（红色）
		for (const QPointF &clickPoint : currentClick) {
			QGraphicsRectItem* rectItem = new QGraphicsRectItem;
			rects.append(rectItem);
			scene->addItem(rectItem);
			rectItem->setBrush(QBrush(Qt::red));
			const double size=5;
			rectItem->setRect(clickPoint.x() - size/2, clickPoint.y() - size/2, size, size);
		}
	}
	void drawQuadrilateral() {
		QPolygonF polygon;
		for (const QPointF &point : currentClick) {
			polygon << point;
		}
		scene->addPolygon(polygon, QPen(Qt::red, 2), QBrush(Qt::transparent));
	}
	void processImage(){
		const int size=50;
		int w=widthSpinBox->value()*size,h=heightSpinBox->value()*size;
		QImage result(w, h, QImage::Format_ARGB32);
		QPointF topleft=currentClick[0],bottomleft=currentClick[1],bottomright=currentClick[2],topright=currentClick[3];
		for(int i=0;i<w;++i){
			double bx=myFFmpeg::avg(topleft.x(),topright.x(),i*1.0/w),by=myFFmpeg::avg(topleft.y(),topright.y(),i*1.0/w);
			double ex=myFFmpeg::avg(bottomleft.x(),bottomright.x(),i*1.0/w),ey=myFFmpeg::avg(bottomleft.y(),bottomright.y(),i*1.0/w);
			// qDebug()<<bx<<" -> "<<ex<<" "<<by<<" -> "<<ey;
			
			for(int j=0;j<h;++j){
				double curx=myFFmpeg::avg(bx,ex,j*1.0/h);
				double cury=myFFmpeg::avg(by,ey,j*1.0/h);
				result.setPixel(i,j,pixmap.toImage().pixel(curx,cury));
			}
		}
		result.save(QString("out/flatter-%1.png").arg(index,3,10,QChar('0')));
		++index;
	}
	void setIndex(){
		// 动态设置index，避免覆盖已有文件
		QString basePath = "out/flatter-";
		QString filePath;
		do{
			filePath = QString("%1%2.png").arg(basePath).arg(++index, 3, 10, QChar('0'));
		} while (QFile::exists(filePath));
	}
};

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	MainWindow window;
	window.show();
	
	return app.exec();
}

#include "flatter.moc"