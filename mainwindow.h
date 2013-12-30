#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QToolButton>
#include <QButtonGroup>
#include <QLabel>
#include <QFileDialog>
#include <QSize>
#include <QFileDialog>
#include <QAbstractButton>
#include <QBrush>
#include <QGraphicsDropShadowEffect>

#include "graphscene.h"
#include "graphnode.h"
#include "graphproxy.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();

		bool loadFromFile(const QString &filename);

	protected:
		void changeEvent(QEvent *e);

	private slots:
		void deleteNode();
		void setEndNode();
		void setStartNode();
		void setNormalNode();
		void setStartEndNode();

		void graphButtonGroupClicked(int id);

		void zoomInTriggered();
		void zoomOutTriggered();
		void zoomResetTriggered();

		void antialiasingActionTriggered(bool checked);
		void drawGridActionTriggered(bool checked);
		void exportSceneToImageDialog();

		void newActionTriggered();
		void saveActionTriggered();
		void saveAsActionTriggered();

		void openGraphDialog();

		void updateWindowName();
		void graphSceneChanged();

		bool validateGraph();
		void validateGraphActionTriggered();
		void computeButtonGroupClicked(QAbstractButton *button);
		void backToEditMode();


		void about();

	private:
		Ui::MainWindow *ui;

		GraphScene *m_graphScene;

		int m_zoom;
		bool m_changed;
		bool m_saved;
		bool m_inViewMode;

		GraphProxy *m_graphProxy;

		QString m_currentFilename;
		QSize m_maxSceneSize;

		QColor m_gridColor;
		QColor m_viewModeBcgColor;

		QMenu *m_nodeMenu;
		QLabel *m_zoomLabel;

		QToolButton *m_addNodeButton;
		QToolButton *m_manipulateButton;
		QToolButton *m_addEdgeButton;
		QButtonGroup *m_graphButtonGroup;

		QAction *m_zoomInAction;
		QAction *m_zoomOutAction;
		QAction *m_zoomResetAction;

		QToolBar *m_fileToolBar;
		QToolBar *m_graphToolBar;
		QToolBar *m_viewToolBar;

		QSettings m_settings;

		QBrush m_oldBackgroundBrush;

		QFileDialog *m_exportImageDialog;
		QFileDialog *m_saveDialog;
		QFileDialog *m_openDialog;

		QGraphicsDropShadowEffect *m_dropShadowEffect;

		bool saveToFile(const QString &filename);

		void setSelectedNodesType(GraphNode::NodeType type);
		void updateZoom();

		void setupFileDialog(QFileDialog *dialog);

		bool proceedIfUnsaved();

		void initSettings();

		void setupActions();
		void setupMenus();
		void setupDialogs();

	protected:
		void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
