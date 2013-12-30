#ifndef GRAPHPROXY_H
#define GRAPHPROXY_H

#include <QList>
#include <QStringList>
#include <QListWidget>
#include <QObject>

#include "graphscene.h"
#include "graphnode.h"
#include "graphedge.h"

#include "algorithmnode.h"
#include "algorithmpath.h"
#include "abstractalgorithm.h"

#include "nodesalgorithm.h"
#include "edgesalgorithm.h"
#include "edgepairalgorithm.h"
#include "simplepathsalgorithm.h"
#include "primepathsalgorithm.h"

class GraphProxy : public QObject
{
	Q_OBJECT

	private:
		GraphScene *m_graphScene;
		QList<Algorithm::Node*> m_nodes;

		QList<Algorithm::Path*> m_reqResults;
		QList<Algorithm::Path*> m_covResults;

		QStringList m_invalidEndNodes;

		QListWidget *m_requirementsList;
		QListWidget *m_coverageList;

		bool m_invalidated;
		bool m_listsLocked;

		Algorithm::Node *findCorrespondingNode(GraphNode *graphNode);
		bool isInvalidatedWarning();

		Algorithm::NodesAlgorithm nodesAlgorithm;
		Algorithm::EdgesAlgorithm edgesAlgorithm;
		Algorithm::EdgePairAlgorithm edgePairAlgorithm;
		Algorithm::SimplePathsAlgorithm simplePathsAlgorithm;
		Algorithm::PrimePathsAlgorithm primePathsAlgorithm;

		void clear();
		void clearNodes();

	public:
		enum AlgorithmType {NodesAlg, EdgesAlg, EdgePairAlg, SimplePathsAlg, PrimePathsAlg};

		GraphProxy(GraphScene *graphScene, QListWidget *requirementsList, QListWidget *coverageList);
		~GraphProxy();

		bool hasEndNode();
		bool hasStartNode();
		QStringList unreachableEndNodeLabels();
		QStringList unreachableStartNodeLabels();

		const QStringList &invalidEndNodes() { return m_invalidEndNodes; }
		void convertNodes();

		const QList<Algorithm::Node*> &nodes() const;

		void clearHighlight();

		void highlightPath(const Algorithm::Path &path);

		void fillListsWithResults();
		void invalidateScene();

		void runAlgorithm(Algorithm::AbstractAlgorithm &alg);
		void runAlgorithm(AlgorithmType algorithmType);

	private slots:
		void coverageListItemActivated(int index);
		void requirementsListItemActivated(int index);

};

#endif // GRAPHPROXY_H
