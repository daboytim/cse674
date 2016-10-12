//Derek Boytim
//CSE 674
//Lab 3
//2-13-2012
//kmeans.h

#ifndef KMEANS_H
#define KMEANS_H

#include <iostream>
#include <stdio.h>

#include "featureVector.h"
#include "cluster.h"

using namespace std;

void setInitCentroids (Vector2& vector, vector<Cluster>& clusters, int k) {
    cout <<"setting init centroids...";
    int i, count=0;
    int space = vector.entries.size()/k;
    for (i=0; count < k; i++) {
	if (i%space == 0) {
	    Cluster c;
	    c.centroid =  vector.entries[i];
	    clusters.push_back (c);
	    count++;
	}
    }
}

void createInitClusters (Vector2& vector, vector<Cluster>& clusters, int mode) {
    cout <<"creating init clusters...";
    int j, k;
    for (j=0; j<vector.entries.size(); j++) {
	int closest=0;
	double sim, bestSim=0;
	for (k=0; k<clusters.size(); k++) {
	    similarity (sim, vector.entries[j], clusters[k].centroid, mode);
	    if (sim > bestSim) {
		bestSim = sim;
		closest = k;
	    }
	}
	clusters[closest].entries.push_back (vector.entries[j]);
    }
}

bool assignToClosest (vector<Cluster>& clusters, int mode) {
    cout <<"rearanging clusters...";
    bool clusterChanged=false;
    int i, j, k;
    for (i=0; i<clusters.size(); i++) {
	for (j=0; j<clusters[i].entries.size(); j++) {
	    int closest=i;
	    double sim1, sim2;
	    similarity (sim1, clusters[i].entries[j], clusters[i].centroid, mode);
	    for (k=0; k<clusters.size(); k++) {
		if (k == i)
		    continue;
		similarity (sim2, clusters[i].entries[j], clusters[k].centroid, mode);
		if (sim2 > sim1) {
		    sim1 = sim2;
		    closest = k;
		}
	    }
	    if (closest != i) {
		clusters[closest].entries.push_back (clusters[i].entries[j]);
		vector<V2Entry>::iterator it = clusters[i].entries.begin();
		it += j;
		clusters[i].entries.erase (it);
		clusterChanged = true;
	    }
	}
    }
    return clusterChanged;
}

void updateCentroids (vector<Cluster>& clusters) {
    cout <<"updating centroids...";
    int i;
    for (i=0; i < clusters.size(); i++) {
	clusters[i].updateCentroid();
    }
    cout <<'\n';
}

void kmeansCluster (Vector2& vector, vector<Cluster>& clusters, int k, int mode=0) {
    cout <<"clustering...";
    setInitCentroids (vector, clusters, k);
    createInitClusters (vector, clusters, mode);
    updateCentroids (clusters);
    bool clusterChanged=false;
    do {
	clusterChanged = assignToClosest (clusters, mode);
	if (clusterChanged)
	    updateCentroids (clusters);
    } while (clusterChanged);   
    cout <<"done\n";
}

#endif
