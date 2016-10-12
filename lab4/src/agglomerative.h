//Derek Boytim
//CSE 674
//Lab 3
//2-13-2012
//agglomerative.h

#ifndef AGGLOMERATIVE_H
#define AGGLOMERATIVE_H

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>

#include "featureVector.h"
#include "cluster.h"

using namespace std;

vector< vector<double> > proxMatrix;

void initClusters (Vector2& vec, vector<Cluster>& clusters) {
    int i;
    for (i=0; i<vec.entries.size(); i++) {
	Cluster c;
	c.entries.push_back (vec.entries[i]);
	vector<double> cent;
	int j;
	for (j=0; j<vec.entries[i].wordCounts.size(); j++) {
	    cent.push_back (vec.entries[i].wordCounts[j]);
	}
	c.centroid =  cent;
	clusters.push_back (c);
    }
}

void createProxMatrix (vector<Cluster>& clusters, int mode) {
    int i;
    double sim;
    for (i=0; i<clusters.size(); i++) {
	int j;
	vector<double> col;
	for (j=0; j<i; j++) {
	    if (i == j) {//dont want to merge if i==j
		sim = 0;
	    }
	    else {
		similarity (sim, clusters[i].centroid, clusters[j].centroid, mode);
	    }
	    //if (sim >= 1)
	    //cout <<sim <<'\n';
	    col.push_back (sim);
	}
	proxMatrix.push_back (col);
    }
}

void findClosestClusters (int& c1, int& c2) {
    cout <<"finding closest...";
    double max=0;
    int i, s1 = (int)proxMatrix.size()-1;
    for (i=s1; i>0; i--) {
	int j, s2 = (int)proxMatrix[i].size()-2;
	for (j=s2; j>=0; j--) {
	    if (proxMatrix[i][j] > max) {
		max = proxMatrix[i][j];
		c1 = i;
		c2 = j;
	    }
	}
    }
}

void mergeClusters (vector<Cluster>& clusters, int c1, int c2) {
    cout <<"merging clusters...";
    cout <<c1 <<':' <<c2 <<"...";
    int i;
    for (i=0; i<clusters[c1].entries.size(); i++) {
	clusters[c2].entries.push_back (clusters[c1].entries[i]);
    }
    clusters[c2].updateCentroid();
    vector<Cluster>::iterator it=clusters.begin();
    it += c1;
    clusters.erase(it);
}

void updateProxMatrix (vector<Cluster>& clusters, int c1, int c2, int mode) {
    //update values for the new cluster.
    cout <<"updating matrix...";
    int i;
    vector<vector<double> >::iterator it = proxMatrix.begin();
    it += c1;
    proxMatrix.erase(it);
    double sim;
    for (i=0; i<(int)proxMatrix[c2].size()-1; i++) {
	similarity (sim, clusters[i].centroid, clusters[c2].centroid, mode);
	proxMatrix[c2][i] = sim;
    }
    for (i=c2+1; i<proxMatrix.size(); i++) {
	similarity (sim, clusters[i].centroid, clusters[c2].centroid, mode);
	proxMatrix[i][c2] = sim;
	if (i >= c1) {
	    vector<double>::iterator it2 = proxMatrix[i].begin();
	    it2 += c1;
	    proxMatrix[i].erase(it2);
	}
    }
    cout <<'\n';
}

void aggCluster (Vector2& vector, vector<Cluster>& clusters, int k, int mode=0) {
    cout <<"clustering...";
    cout <<"creating initial clusters...";
    initClusters (vector, clusters);
    cout <<"creating proximity matrix...";
    createProxMatrix (clusters, mode);
    cout <<"combining clusters...";
    while (clusters.size() > k) {
	int c1, c2;
	findClosestClusters (c1, c2);
	mergeClusters (clusters, c1, c2);
	updateProxMatrix (clusters, c1, c2, mode);
    }
    cout <<"done\n";
}

#endif
