#include "ProgressWait.h"
#include <QPainter>
ProgressWait::ProgressWait(QWidget *parent)
	: QObject(parent)
{
}

ProgressWait::~ProgressWait()
{
}





void ProgressWait::paintEvent(QPaintEvent *)
{
	int width = this->width();
	int height = this->height();
	int side = qMin(width, height);

	//绘制准备工作,启用反锯齿,平移坐标轴中心,等比例缩放
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.translate(width / 2, height / 2);
	painter.scale(side / 200.0, side / 200.0);

	if (barStyle == BarStyle_Arc) {
		drawArc(&painter);
	}
	else if (barStyle == BarStyle_RoundCircle) {
		drawRoundCircle(&painter);
	}
	else if (barStyle == BarStyle_Pie) {
		drawPie(&painter);
	}
	else if (barStyle == BarStyle_Line) {
		drawLine(&painter);
	}
	else if (barStyle == BarStyle_Ring) {
		drawRing(&painter);
	}
	else if (barStyle == BarStyle_SingleCircle) {
		drawSingleCircle(&painter);
	}
	else if (barStyle == BarStyle_DoubleCircle) {
		drawDoubleCircle(&painter);
	}

	drawValue(&painter);
}

void ProgressWait::drawArc(QPainter *painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);

	//计算中心点坐标
	int centerX = 0;
	int centerY = 0;
	int radius = 99;
	int radiusBig = radius / 2;
	int radiusSmall = radius / 6;
	double currentangle = currentValue * (360 / (maxValue + 1));

	if (clockWise) {
		currentangle = -currentangle;
	}

	//绘制八卦大圆1
	painter->setBrush(foreground);
	QPainterPath pathBig1(QPointF(centerX + radius * qCos(degreesToRadians(currentangle)),
		centerY - radius * qSin(degreesToRadians(currentangle))));
	pathBig1.arcTo(centerX - radius, centerY - radius, radius * 2, radius * 2, currentangle, 180);
	pathBig1.arcTo(centerX + radiusBig * qCos(degreesToRadians(currentangle + 180)) - radiusBig,
		centerY - radiusBig * qSin(degreesToRadians(currentangle + 180)) - radiusBig,
		radiusBig * 2, radiusBig * 2, currentangle + 180, 180);
	pathBig1.arcTo(centerX + radiusBig * qCos(degreesToRadians(currentangle)) - radiusBig,
		centerY - radiusBig * qSin(degreesToRadians(currentangle)) - radiusBig,
		radiusBig * 2, radiusBig * 2, currentangle + 180, -180
	);
	painter->drawPath(pathBig1);

	//绘制八卦大圆2
	painter->setBrush(background);
	QPainterPath pathBig2(QPointF(centerX + radius * qCos(degreesToRadians(currentangle)),
		centerY - radius * qSin(degreesToRadians(currentangle))));
	pathBig2.arcTo(centerX - radius, centerY - radius, radius * 2, radius * 2, currentangle, -180);
	pathBig2.arcTo(centerX + radiusBig * qCos(degreesToRadians(currentangle + 180)) - radiusBig,
		centerY - radiusBig * qSin(degreesToRadians(currentangle + 180)) - radiusBig,
		radiusBig * 2, radiusBig * 2, currentangle + 180, 180);
	pathBig2.arcTo(centerX + radiusBig * qCos(degreesToRadians(currentangle)) - radiusBig,
		centerY - radiusBig * qSin(degreesToRadians(currentangle)) - radiusBig,
		radiusBig * 2, radiusBig * 2, currentangle + 180, -180
	);
	painter->drawPath(pathBig2);

	//绘制八卦小圆1
	painter->setBrush(foreground);
	QPainterPath pathSmall1;
	pathSmall1.addEllipse(centerX + radiusBig * qCos(degreesToRadians(currentangle)) - radiusSmall,
		centerY - radiusBig * qSin(degreesToRadians(currentangle)) - radiusSmall,
		radiusSmall * 2, radiusSmall * 2);
	painter->drawPath(pathSmall1);

	//绘制八卦小圆2
	painter->setBrush(background);
	QPainterPath pathSmall2;
	pathSmall2.addEllipse(centerX + radiusBig * qCos(degreesToRadians(180 + currentangle)) - radiusSmall,
		centerY - radiusBig * qSin(degreesToRadians(180 + currentangle)) - radiusSmall,
		radiusSmall * 2, radiusSmall * 2);
	painter->drawPath(pathSmall2);

	painter->restore();
}

void ProgressWait::drawRoundCircle(QPainter *painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);

	int radius = 99;
	int minRadius = radius / 6;
	double angleStep = 360.0 / maxValue;
	double alpha = (double)1 / maxValue;

	if (!clockWise) {
		angleStep = -angleStep;
	}

	//计算中心点坐标
	int centerX = 0;
	int centerY = 0;
	double centerRadius = radius / 1.2;

	for (int i = 0; i < maxValue; i++) {
		double angle = (currentValue + i) * angleStep;
		double initX = centerRadius * qCos(degreesToRadians(angle)) + centerX;
		double initY = centerRadius * qSin(degreesToRadians(angle)) + centerY;

		int value = i * alpha * 255;
		value = value < 30 ? 30 : value;

		foreground.setAlpha(value);
		painter->setBrush(foreground);
		painter->drawEllipse(initX - minRadius, initY - minRadius, minRadius * 2, minRadius * 2);
	}

	painter->restore();
}