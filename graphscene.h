#ifndef GRAPHSCENE_H
#define GRAPHSCENE_H

#include <QGraphicsScene>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QColor>
#include <QSettings>
#include <QGraphicsDropShadowEffect>
#include <QPen>

#include "graphnode.h"
#include "graphscenememento.h"

class GraphScene : public QGraphicsScene
{
	Q_OBJECT

	public:
		enum Mode { InsertNode, InsertEdge, Manipulate, Idle };

		GraphScene(QMenu *nodeMenu, QObject *parent = 0);

		void storeToMemento(GraphSceneMemento &memento);
		void restoreFromMemento(GraphSceneMemento &memento);

		void addNode(GraphNode *node);
		void updateEdgesPositions();

		static QGraphicsDropShadowEffect *createDropShadowEffect();

		void setCurrentID(int id) { m_currentID = id; }

	public slots:
		void setMode(Mode mode);
		void editorLostFocus(GraphNode *node);
		void nodeDragged();

	signals:
		void changed();

	private:
		Mode m_mode;

		int m_currentID;
		
		bool m_leftButtonDown;
		QPointF m_startPoint;
		QGraphicsLineItem *m_line;

		QMenu *m_nodeMenu;

		QSettings m_settings;
		QColor m_insertingLineColor;
		QPen m_insertingLinePen;

		void initSettings();
		void filterNonNodeItems(QList<QGraphicsItem*> &itemList);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
		void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
		void contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent);




};

#endif // GRAPHSCENE_H
