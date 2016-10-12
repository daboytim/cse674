//Derek Boytim
//CSE 674
//Lab 2
//1-30-2012
//naiveBayes.h

#ifndef NAIVEBAYES_H
#define NAIVEBAYES_H

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

class LabelWordList {
public:
    map<string, int> weights;
    int labelWeight;
    int numWords;
    LabelWordList () {
	labelWeight = 0;
	numWords = 0;
    }
};

class LabelWordMap {
public:
    map<string, LabelWordList> labelMap;
    int numLabels;
    LabelWordMap () {
	numLabels = 0;
    }
};

/**********************************************/

LabelWordMap pattern;

/**********************************************/

//start and end are scalers acting on |vec1|. These determine how
//much of the vector to use as the sample data (0<=start<end<=1)

void nBCreateLabelWordMap (Vector1& vec1, double start, double end) {
    cout <<"gatering data from sample: " <<(end-start)*100 <<"% of vector1...";
    int i;
    for (i=(int)(vec1.entries.size()*start); i<(int)(vec1.entries.size()*end); i++) {
	V1Entry* sample = &vec1.entries[i];
	int j;
	for (j=0; j<(*sample).labels.size(); j++) {
	    LabelWordList* list = &pattern.labelMap[(*sample).labels[j]];
	    pattern.numLabels++;
	    (*list).labelWeight++;
	    int k;
	    for (k=0; k<(*sample).words.size(); k++) {
		(*list).weights[(*sample).words[k]] += (*sample).wordCounts[k];
		(*list).numWords++;
	    }
	}
    }
    cout <<"done\n";
}

/**********************************************/

//P(L|W1...Wn) = P(L)*PRODUCT i=1->n P(Wi|L)

//start and end are scalers acting on |vec1|. These determine how
//much of the vector to use as the test data (0<=start<end<=1)

void nBClassifyEntries (Vector1& vec1, double start, double end) {
    cout <<"classifying " <<(end-start)*100 <<"% of vector1...";
    int i;
    for (i=(int)(vec1.entries.size()*start); i<(int)(vec1.entries.size()*end); i++) {
	V1Entry* test = &vec1.entries[i];
	if ((*test).labels.size() > 0) { 
	    continue; //already has a label, skip it
	}
	else if ((*test).words.size() == 0) {
	    continue; //no words to help me classify it!
	}
        //classify this article
	double largestProb=0;
	string bestFitLabel;
	map<string, LabelWordList>::iterator it;
	for (it=pattern.labelMap.begin(); it!=pattern.labelMap.end(); it++) {
	    LabelWordList* list = &(*it).second;
	    double prob = (*list).labelWeight/((double)pattern.numLabels);
	    int j;
	    for (j=0; j<(*test).words.size(); j++) {
		//cout <<prob <<'\n';
		if ((*list).weights[(*test).words[j]] == 0)
		    continue; //this word not associated with label
		prob *= (*test).wordCounts[j] *
		        (*list).weights[(*test).words[j]]/((double)(*list).numWords);
	    }
	    if (prob > largestProb) {
		largestProb = prob;
		bestFitLabel = (*it).first;
	    }
	}   
	(*test).labels.insert ((*test).labels.end(), bestFitLabel);
	//cout <<bestFitLabel.c_str() <<'\n';
    }
    cout <<"done\n";
}

/**********************************************/

#endif
