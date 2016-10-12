//Derek Boytim
//CSE 674
//Lab 3
//2-13-2012
//cluster.h

#ifndef CLUSTER_H
#define CLUSTER_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>

#include "featureVector.h"

using namespace std;

class Cluster {
public:
    vector<double> centroid;
    vector<V2Entry> entries;
    void print ();
    string toString ();
    void updateCentroid ();
    void entropy (double&);
};

void Cluster::print () {
    int i;
    for (i=0; i<entries.size(); i++) {
	cout <<entries[i].toString().c_str() <<'\n';
    }
}

string Cluster::toString () {
    string s;
    int i;
    for (i=0; i<entries.size(); i++) {
	s += entries[i].toString() + '\n';
    }
    return s;
}

void Cluster::updateCentroid () {
    if (entries.size() == 0)
	return;
    int i,j;
    for (i=0; i<centroid.size(); i++) {
	double sum=0;
	for (j=0; j<entries.size(); j++) {
	    sum += entries[j].wordCounts[i];
	}
	centroid[i] = sum/entries.size();
    }
}

void similarity (double& sim, vector<double>& c1, vector<double>& c2, int mode) {
    int i,j;
    if (mode == 0) { //euclidian
	double dist=0;
	for (j=0; j<c1.size(); j++) {
	    dist += pow (c1[j]-c2[j], 2);
	}
	dist = sqrt (dist);
	if (dist < 1) {
	//    dist = 100;
	//    cout <<dist <<'\n';
	}
	//convert dist to sim
        sim = 1/(dist + 1);
	//dist=0 -> sim=1...dist=large -> sim=small
    }
    else{ //cosine
	double sumTop=0, sum1=0, sum2=0;
	for (j=0; j<c1.size(); j++) {
	    sumTop += c1[j]*c2[j];
	    sum1 += pow (c1[j], 2);
	    sum2 += pow (c2[j], 2);
	}
	if (sum1 == 0 || sum2 == 0) {
	    sim = 0;
	}
	else if (mode == 1) {
	    sim = 2*sumTop/(sum1+sum2);   //dice
	}
	else if (mode == 2) {
	    sim = sumTop/(sum1+sum2-sumTop); //jaccard
	}
	else if (mode == 3) {
	    sim = sumTop/sqrt(sum1*sum2); //cosine
	}
	else if (mode == 4) {
	    if (sum1 > sum2)             //overlap
		sim = sumTop/sum2;
	    else
		sim = sumTop/sum1;	
	}
    }
}

void similarity (double& sim, V2Entry& c1, vector<double>& c2, int mode) {
	vector<double> cent;
	int i;
	for (i=0; i<c1.wordCounts.size(); i++) {
		cent.push_back (c1.wordCounts[i]);
	}
	similarity (sim, cent, c2, mode);
}

void similarity (double& sim, V2Entry& c1, V2Entry& c2, int mode) {
	vector<double> cent;
	int i;
	for (i=0; i<c2.wordCounts.size(); i++) {
		cent.push_back (c2.wordCounts[i]);
	}
	similarity (sim, c1, cent, mode);
}

void Cluster::entropy (double& ent) {
    ent = 0;
    map<string, int> topics;
    double total=0.0;
    int i, j;
    for (i=0; i<entries.size(); i++) {
	for (j=0; j<entries[i].labels.size(); j++) {
	    topics[entries[i].labels[j]]++;
	    total++;
	}
    }
    map<string, int>::iterator it = topics.begin();
    for ( ; it != topics.end(); it++) {
	ent += ( (*it).second/total ) * log10 ( total / ((*it).second) );
    }
}

#endif
