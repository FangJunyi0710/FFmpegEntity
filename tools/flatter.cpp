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
			emit mouseClicked(scenePos);
		}
		// 重置拖动状态
		isDragging = false;
		QGraphicsView::mouseReleaseEvent(event);
	}
};

class MainWindow : public QWidget {
public:
	MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
		setupUI();
		connectSignals();
	}
	
protected:
	QLineEdit *pathLabel;
	QLabel *coordLabel;
	GraphicsView *graphicsView;
	QGraphicsScene *scene;
	QVector<QPointF> clickPoints;
	QVector<QGraphicsRectItem*> rects;
	QSpinBox *widthSpinBox,*heightSpinBox;
	QPixmap pixmap;
	void setupUI(){
		// 设置窗口标题
		setWindowTitle("Flatter");
		
		// 创建布局
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		setLayout(mainLayout);

		QHBoxLayout *topLayout = new QHBoxLayout(this);
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
	}
	void loadImage(){
		pixmap.load(pathLabel->text());
		if (!pixmap.isNull()) {
			scene->addPixmap(pixmap);
			scene->setSceneRect(pixmap.rect());
		}
	}
	void updateCoord(QPointF position){
		coordLabel->setText(QString("鼠标坐标: (%1, %2)").arg(QString::number(position.x()), QString::number(position.y())));
	}
	void handleClick(QPointF position) {
		clickPoints.append(position);
		if (clickPoints.size() == 4) {
			drawQuadrilateral();
			processImage();
			clickPoints.clear();
		}
		for(auto rectItem : rects){
			scene->removeItem(rectItem);
			delete rectItem;
		}
		rects.clear();
		for (auto clickPoint : clickPoints) {
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
		for (const QPointF &point : clickPoints) {
			polygon << point;
		}
		scene->addPolygon(polygon, QPen(Qt::red, 2), QBrush(Qt::transparent));
	}
	void processImage(){
		int w=widthSpinBox->value(),h=heightSpinBox->value();
		int imgW=w*50,imgH=h*50;
		QPixmap result(imgW, imgH);
	}
};

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	MainWindow window;
	window.show();
	
	return app.exec();
}

#include "flatter.moc"