#include "graphproxy.h"

#include <QGraphicsItem>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QLabel>
#include <QListWidgetItem>

using namespace Algorithm;

GraphProxy::GraphProxy(GraphScene *graphScene, QListWidget *requirementsList, QListWidget *coverageList)
	: m_graphScene(graphScene), m_requirementsList(requirementsList), m_coverageList(coverageList)
{
	connect(m_requirementsList, SIGNAL(currentRowChanged(int)), this, SLOT(requirementsListItemActivated(int)));
	connect(m_coverageList, SIGNAL(currentRowChanged(int)), this, SLOT(coverageListItemActivated(int)));

	m_invalidated = true;
}

Node *GraphProxy::findCorrespondingNode(GraphNode *graphNode)
{
	foreach(Node *node, m_nodes)
		if(node->graphNode() == graphNode)
			return node;

	return 0;
}

bool GraphProxy::hasStartNode()
{
	foreach(Node *node, m_nodes)
	{
		if(node->type() == GraphNode::StartNode ||
		   node->type() == GraphNode::StartEndNode)
			return true;
	}

	return false;
}

bool GraphProxy::hasEndNode()
{
	foreach(Node *node, m_nodes)
	{
		if(node->type() == GraphNode::EndNode ||
		   node->type() == GraphNode::StartEndNode)
			return true;
	}

	return false;
}

QStringList GraphProxy::unreachableEndNodeLabels()
{
	QStringList list;

	foreach(Node *node, m_nodes)
	{
		if(node->type() == GraphNode::EndNode ||
		   node->type() == GraphNode::StartEndNode)
			continue;

		if(!Algorithm::Path::isEndReachable(node))
			list << node->label();
	}

	return list;
}

QStringList GraphProxy::unreachableStartNodeLabels()
{
	QStringList list;

	foreach(Node *node, m_nodes)
	{
		if(node->type() == GraphNode::StartNode ||
		   node->type() == GraphNode::StartEndNode)
			continue;

		if(node->distanceToNearestStartNode() == -1)
			list << node->label();
	}

	return list;
}

void GraphProxy::invalidateScene()
{
	m_invalidated = true;
}

void GraphProxy::clear()
{
	clearNodes();
	m_invalidEndNodes.clear();
}

void GraphProxy::convertNodes()
{
	clear();

	foreach(QGraphicsItem *item, m_graphScene->items())
		if(item->type() == GraphNode::Type)
		{
			GraphNode *graphNode = qgraphicsitem_cast<GraphNode*>(item);
			Node *node = new Node(graphNode);

			m_nodes.append(node);
		}

	foreach(QGraphicsItem *item, m_graphScene->items())
		if(item->type() == GraphEdge::Type)
		{
			GraphEdge *graphEdge = qgraphicsitem_cast<GraphEdge*>(item);

			Node *fromNode = findCorrespondingNode(graphEdge->fromNode());
			Node *toNode = findCorrespondingNode(graphEdge->toNode());

			if(fromNode->type() == GraphNode::EndNode)
				m_invalidEndNodes << fromNode->label();

			fromNode->addLink(toNode, graphEdge);
			toNode->addBackLink(fromNode);
		}
}

const QList<Algorithm::Node*> &GraphProxy::nodes() const
{
	return m_nodes;
}

void GraphProxy::clearHighlight()
{
	foreach(QGraphicsItem *item, m_graphScene->items())
	{
		if(item->type() == GraphNode::Type)
			qgraphicsitem_cast<GraphNode*>(item)->clearHighlight();
		else if(item->type() == GraphEdge::Type)
			qgraphicsitem_cast<GraphEdge*>(item)->clearHighlight();
	}
}

void GraphProxy::highlightPath(const Path &path)
{
	clearHighlight();

	foreach(Node *node, path.nodes())
	{
		node->graphNode()->highlight();
	}

	for(int i = 0; i < (path.nodes().size() - 1); ++i)
	{
		GraphEdge *edge = path.nodes().at(i)->getGraphEdgeToNode(path.nodes().at(i + 1));

#ifdef DEBUG
		if(edge == 0)
			qDebug() << "highlightPath: invalidPath: edge(" +
					path.nodes().at(i)->label() + "->" +
					path.nodes().at(i + 1)->label() + ")";
#endif
		if(edge != 0)
			edge->highlight();

	}
}

bool GraphProxy::isInvalidatedWarning()
{
	if(m_invalidated)
	{
		QMessageBox::warning(0, tr("Warning"), tr("Cannot highlight results - you're in editing mode."));
		return true;
	}

	return false;
}

void GraphProxy::coverageListItemActivated(int index)
{
	if(m_listsLocked)
		return;

	if(isInvalidatedWarning())
		return;

	if(index >= m_covResults.size())
	{
#ifdef DEBUG
		qWarning() << "coverageListItemActivated: no such item";
#endif
		return;
	}

	highlightPath(*m_covResults.at(index));
}

void GraphProxy::requirementsListItemActivated(int index)
{
	if(m_listsLocked)
		return;

	if(isInvalidatedWarning())
		return;

	if(index >= m_reqResults.size())
	{
#ifdef DEBUG
		qWarning() << "requirementsListItemActivated: no such item";
#endif
		return;
	}

	highlightPath(*m_reqResults.at(index));
}

void GraphProxy::fillListsWithResults()
{
	m_listsLocked = true;
	m_coverageList->clear();

	foreach(Path *path, m_covResults)
	{
		//m_coverageList->addItem(path->toText());

		QListWidgetItem *item = new QListWidgetItem();
		QLabel *label = new QLabel(path->toRichText());
		label->setTextFormat(Qt::RichText);
		label->adjustSize();
		item->setSizeHint(label->sizeHint());

		m_coverageList->addItem(item);
		m_coverageList->setItemWidget(item, label);
	}


	m_requirementsList->clear();

	foreach(Path *path, m_reqResults)
	{
		//m_requirementsList->addItem(path->toText());

		QListWidgetItem *item = new QListWidgetItem();
		QLabel *label = new QLabel(path->toRichText());
		label->setTextFormat(Qt::RichText);
		label->adjustSize();
		item->setSizeHint(label->sizeHint());
		m_requirementsList->addItem(item);
		m_requirementsList->setItemWidget(item, label);
	}

	m_listsLocked = false;

}

void GraphProxy::runAlgorithm(AbstractAlgorithm &alg)
{
	alg.compute(m_nodes);

	m_covResults = alg.coverageResults();
	m_reqResults = alg.requirementsResults();

	fillListsWithResults();
	m_invalidated = false;
}

void GraphProxy::runAlgorithm(AlgorithmType algorithmType)
{
	switch(algorithmType)
	{
		case NodesAlg:
			runAlgorithm(nodesAlgorithm);
		break;

		case EdgesAlg:
			runAlgorithm(edgesAlgorithm);
		break;

		case EdgePairAlg:
			runAlgorithm(edgePairAlgorithm);
		break;

		case SimplePathsAlg:
			runAlgorithm(simplePathsAlgorithm);
		break;

		case PrimePathsAlg:
			runAlgorithm(primePathsAlgorithm);
		break;
	}
}

GraphProxy::~GraphProxy()
{
	m_listsLocked = true;
	m_coverageList->clear();
	m_requirementsList->clear();

	clearNodes();
}


void GraphProxy::clearNodes()
{
	foreach(Node *node, m_nodes)
		delete node;

	m_nodes.clear();
}
