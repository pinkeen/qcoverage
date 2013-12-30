#ifndef ALGORITHMPATH_H
#define ALGORITHMPATH_H

#include <QList>
#include <QColor>
#include <QSettings>
#include <QString>

#include "algorithmnode.h"

namespace Algorithm
{
	class Path
	{
		private:
			QList<Node*> m_nodes;

			QString m_startNodeColor;
			QString m_endNodeColor;
			QString m_startEndNodeColor;
			QSettings m_settings;

			void initSettings();

		public:
			Path();
			Path(const QList<Node*> &nodes);
			Path(Node* node);
			Path(const Path* path);

			void appendNode(Node *node);
			void prependNode(Node *node);

			void appendPath(Path *path);
			void prependPath(Path *path);

			int nodeCount() const;
			int edgeCount() const;

			bool containsPath(const Path *path) const;
			bool containsEdge(Node *fromNode, Node *toNode);
			bool containsNode(Node *node);
			bool countNode(Node *node);

			bool isTestPath();
			bool isBorderPath();

			void intersectionLeftPos(const Path *path, int &start, int &end, int &len) const;
			void intersectionRightPos(const Path *path, int &start, int &end, int &len) const;

			Path *mergedWith(const Path *path) const;
			Node *firstNode() { return m_nodes.first(); }
			Node *lastNode() { return m_nodes.last(); }

			int intersects(const Path *path) const;
			int intersectsRight(const Path *path) const;
			int intersectsLeft(const Path *path) const;

			static bool isEndReachable(Node *fromNode, Path *touredPath = 0);

			const QList<Node*> nodes() const { return m_nodes; }

			QString toText() const;
			QString toRichText() const;
	};
}

#endif // ALGORITHMPATH_H
