#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHBoxLayout>
#include <QSettings>
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QDialog>
#include <QStringList>
#include <QFileInfo>
#include <QMessageBox>
#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QDateTime>
#include <QFile>
#include <QDataStream>
#include <QGraphicsDropShadowEffect>

#include "graphnode.h"
#include "graphscenememento.h"

#define QCV_MAGIC 0x3fac9e3d

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow),
	m_zoom(100), m_changed(false), m_saved(false), m_inViewMode(false),
	m_currentFilename(tr("Untitled.qcv"))
{
	ui->setupUi(this);

	move(QApplication::desktop()->availableGeometry(this).center() - rect().center());

	setupActions();
	setupMenus();
	setupDialogs();

	initSettings();

	m_graphScene = new GraphScene(m_nodeMenu, this);
	m_graphScene->setSceneRect(QRectF(QPoint(), QSizeF(m_maxSceneSize)));

	m_graphProxy = new GraphProxy(m_graphScene, ui->requirementsList, ui->coverageList);

	ui->graphicsView->setScene(m_graphScene);
	ui->graphicsView->setInteractive(true);
	ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	ui->graphicsView->setBackgroundBrush(QBrush(Qt::white));

	antialiasingActionTriggered(true);
	graphButtonGroupClicked(GraphScene::Manipulate);
	updateZoom();
	updateWindowName();

	connect(m_graphScene, SIGNAL(changed()), this, SLOT(graphSceneChanged()));
}

void MainWindow::graphSceneChanged()
{
	if(m_changed)
		return;

	m_changed = true;
	updateWindowName();
}

void MainWindow::updateWindowName()
{
	QFileInfo fileInfo(m_currentFilename);
	QString filename(fileInfo.fileName());

	QString title(filename);

	if(m_changed)
		title += "*";

	title += " - " + QCoreApplication::applicationName();

	setWindowTitle(title);
}

void MainWindow::initSettings()
{
	m_viewModeBcgColor = m_settings.value("viewModeBcgColor").toString();
	m_gridColor = m_settings.value("gridColor").toString();
	m_maxSceneSize = m_settings.value("defaultSceneSize").toSize();
}

void MainWindow::setupFileDialog(QFileDialog *dialog)
{
	dialog->setOption(QFileDialog::DontUseNativeDialog);
	dialog->setFileMode(QFileDialog::AnyFile);
	dialog->setViewMode(QFileDialog::List);
	dialog->setFilter(QDir::AllDirs | QDir::Files | QDir::AllEntries | QDir::NoDotAndDotDot);
	dialog->setModal(true);
	dialog->setConfirmOverwrite(true);
	dialog->setNameFilterDetailsVisible(true);
}

void MainWindow::setupDialogs()
{
	m_exportImageDialog = new QFileDialog(this, tr("Export to image"));

	setupFileDialog(m_exportImageDialog);

	m_exportImageDialog->setDefaultSuffix("png");
	m_exportImageDialog->setAcceptMode(QFileDialog::AcceptSave);
	m_exportImageDialog->setNameFilter(tr("Images (*.png *.jpg *.jpeg)"));


	m_saveDialog = new QFileDialog(this, tr("Save graph"));

	setupFileDialog(m_saveDialog);

	m_saveDialog->setDefaultSuffix("qcv");
	m_saveDialog->setNameFilter(tr("Graphs (*.qcv)"));

	m_saveDialog->setAcceptMode(QFileDialog::AcceptSave);


	m_openDialog = new QFileDialog(this, tr("Open graph"));

	setupFileDialog(m_openDialog);

	m_openDialog->setNameFilter(tr("Graphs (*.qcv)"));
	m_openDialog->setAcceptMode(QFileDialog::AcceptOpen);
	m_openDialog->setFileMode(QFileDialog::ExistingFile);
}

void MainWindow::antialiasingActionTriggered(bool checked)
{
	ui->graphicsView->setRenderHint(QPainter::Antialiasing, checked);
	ui->graphicsView->setRenderHint(QPainter::TextAntialiasing, checked);
	ui->graphicsView->resetCachedContent();
	ui->graphicsView->scene()->update();
	ui->graphicsView->repaint();
}

void MainWindow::drawGridActionTriggered(bool checked)
{
	if(m_inViewMode)
	{
		ui->drawGridAction->setChecked(!checked);
		return;
	}

	QBrush brush(ui->graphicsView->backgroundBrush());

	if(checked)
	{
		brush.setStyle(Qt::CrossPattern);
		brush.setColor(m_gridColor);
	}
	else
	{
		brush.setStyle(Qt::NoBrush);
	}

	ui->graphicsView->setBackgroundBrush(brush);
	ui->graphicsView->repaint();
}

void MainWindow::graphButtonGroupClicked(int id)
{
	m_graphScene->setMode(GraphScene::Mode(id));
	ui->graphicsView->setDragMode(QGraphicsView::NoDrag);

	switch(GraphScene::Mode(id))
	{
		case GraphScene::InsertNode:
			ui->graphicsView->setCursor(QCursor(Qt::CrossCursor));
		break;

		case GraphScene::InsertEdge:
			ui->graphicsView->setCursor(QCursor(Qt::PointingHandCursor));
		break;

		case GraphScene::Manipulate:
			ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
		break;

		default:
			ui->graphicsView->setCursor(QCursor(Qt::ArrowCursor));
	}
}

void MainWindow::saveAsActionTriggered()
{
	m_saveDialog->exec();

	if(m_saveDialog->result() != QDialog::Accepted)
		return;

	QString filename = m_saveDialog->selectedFiles().first();

	m_currentFilename = filename;

	saveToFile(filename);
}

void MainWindow::saveActionTriggered()
{
	if(!m_saved)
	{
		saveAsActionTriggered();
		return;
	}

	saveToFile(m_currentFilename);
}

bool MainWindow::proceedIfUnsaved()
{
	if(!m_changed)
		return true;

	int result = QMessageBox::warning(this, tr("Warning"),
					  tr("You have unsaved work. Do you want to proceed anyway?"),
					  QMessageBox::Yes | QMessageBox::Save | QMessageBox::Abort);

	if(result == QMessageBox::Save)
	{
		saveActionTriggered();

		if(!m_changed)
			return true;

		return false;
	}

	if(result == QMessageBox::Abort)
		return false;

	return true;
}

void MainWindow::openGraphDialog()
{
	if(!proceedIfUnsaved())
		return;

	m_openDialog->exec();

	if(m_openDialog->result() != QDialog::Accepted)
		return;

	if(m_inViewMode)
		backToEditMode();

	delete m_graphProxy;
	m_graphProxy = new GraphProxy(m_graphScene, ui->requirementsList, ui->coverageList);

	QString filename = m_openDialog->selectedFiles().first();
	m_currentFilename = filename;

	loadFromFile(filename);
}

void MainWindow::exportSceneToImageDialog()
{
	if(m_graphScene->items().isEmpty())
	{
		QMessageBox::warning(this, tr("Aborting..."),
				     tr("The graph is empty!"),
				     QMessageBox::Abort);
		return;
	}

	m_exportImageDialog->exec();

	if(m_exportImageDialog->result() != QDialog::Accepted)
		return;

	m_graphScene->updateEdgesPositions();

	QStringList filenames = m_exportImageDialog->selectedFiles();
	QString filename = filenames.first();
	QString ext = QFileInfo(filename).suffix().toLower();

	bool transparent = false;

	if(ext == "png")
	{
		QMessageBox msgBox(QMessageBox::Question, tr("Transparency"),
				   tr("Do you want transparent background?"),
				   QMessageBox::Yes | QMessageBox::No, this);

		msgBox.exec();

		if(msgBox.result() == QMessageBox::Yes)
			transparent = true;
	}

	QRectF sourceRect(m_graphScene->itemsBoundingRect());

	/* Add some padding. */
	qreal padding = (m_maxSceneSize.width() + m_maxSceneSize.height()) / 20.0;
	sourceRect.adjust(-padding, -padding, padding, padding);

	QRectF targetRect(QPointF(), sourceRect.size());

	QImage image(targetRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
	image.fill(qRgba(0, 0, 0, 0));

	QPainter painter(&image);

	painter.initFrom(ui->graphicsView);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

	if(!transparent)
		painter.eraseRect(image.rect());

	painter.drawText(QRectF(1, 1, targetRect.size().width(), padding), m_currentFilename);

	painter.setBackgroundMode(Qt::TransparentMode);

	m_graphScene->clearSelection();
	m_graphScene->render(&painter, targetRect, sourceRect.toRect());

	if(ext == "jpg" || ext == "jpeg")
		image.save(filename, "JPG", 100);
	else
		image.save(filename);

	painter.end();
}


void MainWindow::deleteNode()
{
	int deletedCount = 0;

	foreach(QGraphicsItem *item, m_graphScene->selectedItems())
	{
		if(item->type() == GraphNode::Type)
		{
			qgraphicsitem_cast<GraphNode*>(item)->removeAllEdges();
			m_graphScene->removeItem(item);
			delete item;

			deletedCount++;
		}
		else if(item->type() == GraphEdge::Type)
		{
			GraphEdge *edge = qgraphicsitem_cast<GraphEdge*>(item);
			edge->fromNode()->removeEdge(edge);
			edge->toNode()->removeEdge(edge);
			m_graphScene->removeItem(item);
			delete item;

			deletedCount++;
		}
	}

	if(deletedCount != 0)
		graphSceneChanged();

}


void MainWindow::setSelectedNodesType(GraphNode::NodeType type)
{
	int changedCount = 0;

	foreach(QGraphicsItem *item, m_graphScene->selectedItems())
	{
		if(item->type() == GraphNode::Type)
		{
			qgraphicsitem_cast<GraphNode*>(item)->setNodeType(type);
			changedCount++;
		}
	}

	if(changedCount != 0)
		graphSceneChanged();
}

void MainWindow::setStartNode()
{
	setSelectedNodesType(GraphNode::StartNode);
}

void MainWindow::setEndNode()
{
	setSelectedNodesType(GraphNode::EndNode);
}

void MainWindow::setNormalNode()
{
	setSelectedNodesType(GraphNode::NormalNode);
}

void MainWindow::setStartEndNode()
{
	setSelectedNodesType(GraphNode::StartEndNode);
}


void MainWindow::zoomInTriggered()
{
	m_zoom += 10;
	updateZoom();
}

void MainWindow::zoomOutTriggered()
{
	m_zoom -= 10;
	updateZoom();
}

void MainWindow::zoomResetTriggered()
{
	m_zoom = 100;
	updateZoom();
}

void MainWindow::updateZoom()
{
	if(m_zoom < 10)
		m_zoom = 10;

	float newScale = (float)m_zoom / 100.0f;

	m_zoomLabel->setText(QString::number(m_zoom) + "%");

	QMatrix oldMatrix = ui->graphicsView->matrix();
	ui->graphicsView->resetMatrix();
	ui->graphicsView->translate(oldMatrix.dx(), oldMatrix.dy());
	ui->graphicsView->scale(newScale, newScale);
}


MainWindow::~MainWindow()
{
	delete m_graphProxy;
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);

	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
		break;

		default: ;
	}
}

void MainWindow::validateGraphActionTriggered()
{
	if(validateGraph())
		QMessageBox::information(this, tr("Graph validation"), tr("Graph is ok."));
}

bool MainWindow::validateGraph()
{
	m_graphProxy->convertNodes();
	QStringList invalidEndNodes = m_graphProxy->invalidEndNodes();
	QStringList unreachableEndNodes = m_graphProxy->unreachableEndNodeLabels();
	QStringList unreachableStartNodes = m_graphProxy->unreachableStartNodeLabels();

	bool hasEnd = m_graphProxy->hasEndNode();
	bool hasStart = m_graphProxy->hasStartNode();
	bool invalidEnd = invalidEndNodes.size() != 0;
	bool hasNodesNotReachingEnd = unreachableEndNodes.size() != 0;
	bool hasNodesNotReachingStart = unreachableStartNodes.size() != 0;

	QString text("<b>Invalid graph!</b><br/>");

	if(!hasEnd)
		text += tr("Graph must at least have one end node!<br/>");

	if(!hasStart)
		text += tr("Graph must at least have one end or start/end node!<br/>");

	if(invalidEnd)
	{
		text += tr("End nodes can't have outgoing edges: ");
		invalidEndNodes.sort();
		text += ("<em>(" + invalidEndNodes.join(", ") + ")</em><br/>");
	}

	if(hasNodesNotReachingEnd)
	{
		text += tr("All nodes must reach at least one end node. This don't: ");
		unreachableEndNodes.sort();
		text += ("<em>(" + unreachableEndNodes.join(", ") + ")</em><br/>");
	}

	if(hasNodesNotReachingStart)
	{
		text += tr("All nodes must be reachable from a start/startend node. This don't: ");
		unreachableStartNodes.sort();
		text += ("<em>(" + unreachableStartNodes.join(", ") + ")</em><br/>");
	}

	bool ok = hasEnd && hasStart && !invalidEnd && !hasNodesNotReachingEnd && !hasNodesNotReachingStart;


	if(!ok)
		QMessageBox::critical(this, tr("Graph validation"), text);

	return ok;
}

void MainWindow::computeButtonGroupClicked(QAbstractButton *button)
{
	if(!validateGraph())
		return;

	if(!m_inViewMode)
		m_oldBackgroundBrush = ui->graphicsView->backgroundBrush();

	m_inViewMode = true;

	ui->graphicsView->setBackgroundBrush(QBrush(m_viewModeBcgColor));
	ui->graphicsView->repaint();
	ui->graphicsView->setInteractive(false);
	m_graphScene->clearSelection();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	if(button == ui->nodesButton)
	{
		m_graphProxy->runAlgorithm(GraphProxy::NodesAlg);
	}
	else if(button == ui->edgesButton)
	{
		m_graphProxy->runAlgorithm(GraphProxy::EdgesAlg);
	}
	else if(button == ui->edgePairButton)
	{
		m_graphProxy->runAlgorithm(GraphProxy::EdgePairAlg);
	}
	else if(button == ui->simplePathsButton)
	{
		m_graphProxy->runAlgorithm(GraphProxy::SimplePathsAlg);
	}
	else if(button == ui->primePathsButton)
	{
		m_graphProxy->runAlgorithm(GraphProxy::PrimePathsAlg);
	}

	QApplication::restoreOverrideCursor();

        ui->requirementsCountLabel->setText(tr("Count: <b>%1</b>").arg(ui->requirementsList->count()));
        ui->coverageCountLabel->setText(tr("Count: <b>%1</b>").arg(ui->coverageList->count()));

	ui->backToEditButton->setEnabled(true);
}

void MainWindow::backToEditMode()
{
	ui->graphicsView->setInteractive(true);
	ui->backToEditButton->setEnabled(false);

	m_graphProxy->invalidateScene();
	m_graphProxy->clearHighlight();

	ui->graphicsView->setBackgroundBrush(m_oldBackgroundBrush);
	ui->graphicsView->repaint();

	m_inViewMode = false;
}

void MainWindow::setupActions()
{
	m_addNodeButton = new QToolButton;
	m_addNodeButton->setIcon(QIcon(":/images/add"));
	m_addNodeButton->setCheckable(true);
	m_addNodeButton->setText(tr("New node"));
	m_addNodeButton->setStatusTip(tr("Adds a new node"));
	m_addNodeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	m_addEdgeButton = new QToolButton;
	m_addEdgeButton->setIcon(QIcon(":/images/edge"));
	m_addEdgeButton->setCheckable(true);
	m_addEdgeButton->setText(tr("New edge"));
	m_addEdgeButton->setStatusTip(tr("Lets you add new edge"));
	m_addEdgeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	m_manipulateButton = new QToolButton;
	m_manipulateButton->setIcon(QIcon(":/images/manipulate"));
	m_manipulateButton->setCheckable(true);
	m_manipulateButton->setChecked(true);
	m_manipulateButton->setText(tr("Manipulate"));
	m_manipulateButton->setStatusTip(tr("Manipulate nodes and edges"));
	m_manipulateButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	m_graphButtonGroup = new QButtonGroup(this);
	m_graphButtonGroup->addButton(m_manipulateButton, int(GraphScene::Manipulate));
	m_graphButtonGroup->addButton(m_addEdgeButton, int(GraphScene::InsertEdge));
	m_graphButtonGroup->addButton(m_addNodeButton, int(GraphScene::InsertNode));

	connect(m_graphButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(graphButtonGroupClicked(int)));

	m_zoomInAction = new QAction(QIcon(":/images/zoomin"), tr("Zoom In"), this);
	m_zoomOutAction = new QAction(QIcon(":/images/zoomout"), tr("Zoom Out"), this);
	m_zoomResetAction = new QAction(QIcon(":/images/reset"), tr("Zoom Reset"), this);

	connect(m_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomInTriggered()));
	connect(m_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOutTriggered()));
	connect(m_zoomResetAction, SIGNAL(triggered()), this, SLOT(zoomResetTriggered()));

	m_fileToolBar = addToolBar(tr("File"));
	m_fileToolBar->addActions(ui->fileMenu->actions());
	m_fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	m_graphToolBar = addToolBar(tr("Graph"));
	m_graphToolBar->addWidget(m_manipulateButton);
	m_graphToolBar->addWidget(m_addNodeButton);
	m_graphToolBar->addWidget(m_addEdgeButton);

	connect(ui->exportGraphImageAction, SIGNAL(triggered()), this, SLOT(exportSceneToImageDialog()));
	connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(ui->antialiasingAction, SIGNAL(triggered(bool)), this, SLOT(antialiasingActionTriggered(bool)));
	connect(ui->drawGridAction, SIGNAL(triggered(bool)), this, SLOT(drawGridActionTriggered(bool)));

	connect(ui->saveAction, SIGNAL(triggered()), this, SLOT(saveActionTriggered()));
	connect(ui->saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsActionTriggered()));
	connect(ui->openAction, SIGNAL(triggered()), this, SLOT(openGraphDialog()));

	connect(ui->deleteAction, SIGNAL(triggered()), this, SLOT(deleteNode()));
	connect(ui->startNodeAction, SIGNAL(triggered()), this, SLOT(setStartNode()));
	connect(ui->endNodeAction, SIGNAL(triggered()), this, SLOT(setEndNode()));
	connect(ui->normalNodeAction, SIGNAL(triggered()), this, SLOT(setNormalNode()));
	connect(ui->startEndNodeAction, SIGNAL(triggered()), this, SLOT(setStartEndNode()));
	connect(ui->validateGraphAction, SIGNAL(triggered()), this, SLOT(validateGraphActionTriggered()));
	connect(ui->computeButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(computeButtonGroupClicked(QAbstractButton*)));
	connect(ui->backToEditButton, SIGNAL(clicked()), this, SLOT(backToEditMode()));
	connect(ui->newAction, SIGNAL(triggered()), this, SLOT(newActionTriggered()));
}

void MainWindow::setupMenus()
{
	m_nodeMenu = ui->nodeMenu;

	ui->viewMenu->addAction(m_zoomInAction);
	ui->viewMenu->addAction(m_zoomOutAction);
	ui->viewMenu->addSeparator();
	ui->viewMenu->addAction(m_zoomResetAction);

	QToolButton *zoomInToolButton = new QToolButton();
	QToolButton *zoomOutToolButton = new QToolButton();
	QToolButton *zoomResetToolButton = new QToolButton();

	m_zoomLabel = new QLabel();

	zoomInToolButton->setDefaultAction(m_zoomInAction);
	zoomOutToolButton->setDefaultAction(m_zoomOutAction);
	zoomResetToolButton->setDefaultAction(m_zoomResetAction);

	ui->statusBar->addPermanentWidget(zoomInToolButton);
	ui->statusBar->addPermanentWidget(zoomOutToolButton);
	ui->statusBar->addPermanentWidget(zoomResetToolButton);
	ui->statusBar->addPermanentWidget(m_zoomLabel);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if(!proceedIfUnsaved())
	{
		event->ignore();
		return;
	}

	QMainWindow::closeEvent(event);
}

void MainWindow::about()
{
	QString text;

	text += "<h1>" + QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion() + "</h1>";
	text += "2010 &copy; Filip Sobalski <a href=\"mailto:pinkeen@gmail.com\">&lt;pinkeen@gmail.com&gt;</a><br/>";
	text += tr("Interface icons &copy; <a href=\"http://www.visualpharm.com/\">VisualPharm (Ivan Boyko)</a><br/><br/>");
	text += tr("<i>This application is free software licensed under GPLv3 or any later version at your option.</i><br/>");

	QMessageBox::about(this, tr("About") + " " + QCoreApplication::applicationName(), text);
}

bool MainWindow::saveToFile(const QString &filename)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	GraphSceneMemento memento;
	m_graphScene->storeToMemento(memento);

	QFile file(filename);

	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, tr("Error"),
				      tr("Could not open file <i>") + filename +
				      tr("</i> with error: <b>") +
				      file.errorString() + "</b>");

		QApplication::restoreOverrideCursor();
		return false;
	}

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_0);

	out << (quint32)QCV_MAGIC;

	memento.write(out);

	file.flush();
	file.close();

	ui->statusBar->showMessage(tr("Succesfully saved file ") + filename, 3);

	m_saved = true;
	m_changed = false;
	updateWindowName();

	QApplication::restoreOverrideCursor();

	return true;
}

bool MainWindow::loadFromFile(const QString &filename)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QFile file(filename);

	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, tr("Error"),
				      tr("Could not open file <i>") + filename +
				      tr("</i> with error: <b>") +
				      file.errorString() + "</b>");

		QApplication::restoreOverrideCursor();

		return false;
	}


	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_0);

	quint32 magic;

	in >> magic;

	if(magic != (quint32)QCV_MAGIC)
	{
		QMessageBox::critical(this, tr("Error"),
				      tr("File %1 is either corrupted or is not %2 graph!").
				      arg(filename, QCoreApplication::applicationName()));
		file.close();

		QApplication::restoreOverrideCursor();

		return false;
	}

	GraphSceneMemento memento;
	memento.read(in);

	file.close();

	m_graphScene->restoreFromMemento(memento);

	ui->graphicsView->repaint();
	ui->statusBar->showMessage(tr("Succesfully loaded file ") + filename, 3);

	m_saved = true;
	m_changed = false;
	updateWindowName();

	QApplication::restoreOverrideCursor();

	return true;
}

void MainWindow::newActionTriggered()
{
	if(!proceedIfUnsaved())
		return;

	if(m_inViewMode)
		backToEditMode();

	m_saved = false;
	m_changed = false;
	m_currentFilename = tr("Untitled.qcv");
	updateWindowName();

	delete m_graphProxy;
	m_graphProxy = new GraphProxy(m_graphScene, ui->requirementsList, ui->coverageList);

	m_graphScene->clear();
	m_graphScene->setCurrentID(0);
	ui->graphicsView->repaint();
}
