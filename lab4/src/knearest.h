//Derek Boytim
//CSE 674
//Lab 2
//1-30-2012
//knearest.h

#ifndef knearest_H
#define knearest_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <map>
#include <set>
#include <vector>

#include "featureVector.h"

using namespace std;

/**********************************************/

void kNClassifyEntries (Vector2& vec2, double sStart, double sEnd, double tStart, double tEnd, int k) {
    cout <<"using " <<(sEnd-sStart)*100 <<"% of vector 2 as training data to\n";
    cout <<"classify " <<(tEnd-tStart)*100 <<"% of vector2...";
    int i;
    for (i=(int)(vec2.entries.size()*tStart); i<(int)(vec2.entries.size()*tEnd); i++) {
	V2Entry* test = &vec2.entries[i];
	if ((*test).labels.size() > 0) {
	    continue; //already has a label, skip it
	}
	map<double,int> distances;
	int j;
	for (j=(int)(vec2.entries.size()*sStart); j<(int)(vec2.entries.size()*sEnd); j++) {
	    V2Entry* sample = &vec2.entries[j];
	    if ((*sample).labels.size() == 0) {
		continue; //has no label, wont be of any help
	    }
	    double dist=0;
	    //compute euclidean distance
	    int m;
	    for (m=0; m<(*sample).wordCounts.size(); m++) {
		dist += pow ((*sample).wordCounts[m]-(*test).wordCounts[m], 2);
	    }
	    dist = sqrt (dist);
	    //save distance and index of this entry
	    distances.insert (pair<double,int>(dist,j));
	}
	//distances sorted by distance with smallest at begin
	map<double,int>::iterator it = distances.begin(); 
	map<string,int> labels;
	int m;
	for (m=0; m<k; m++, it++) {
	    V2Entry* neighbor = &vec2.entries[(*it).second];
	    //add save the labels from this neighbor
	    int n;
	    for (n=0; n<(*neighbor).labels.size(); n++) {
		labels[(*neighbor).labels[n]]++;
	    }
	}
	//find the label with the most hits
	map<string,int>::iterator it2;
	int maxHits=0;
	bool tie=false;
	string label;
	do {
	    for (it2=labels.begin(); it2 != labels.end(); it2++) {
		if ((*it2).second == maxHits) { //tie
		    tie = true;
		}
		else if ((*it2).second > maxHits) {
		    label = (*it2).first;
		    maxHits = (*it2).second;
		    tie = false;
		}
	    }
	    if (tie) {
		it++;
		if (it == distances.end()) {
		    break;
		}
		V2Entry* neighbor = &vec2.entries[(*it).second];
		//add save the labels from this neighbor
		int n;
		for (n=0; n<(*neighbor).labels.size(); n++) {
		    labels[(*neighbor).labels[n]]++;
		}
	    }
	} while (tie);
	
	(*test).labels.insert ((*test).labels.end(), label);
	//cout <<label <<'\n';
    }
    cout <<"done\n";
}

/**********************************************/

#endif
