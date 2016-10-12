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

class Words {
public:
    map<string, int> weights;
};

class LabelPattern {
public:
    map<string, Words> labelEst;
};

/**********************************************/

LabelPattern pattern;

/**********************************************/

int buildLabelPattern (Vector1& vec1, int percent) {
    cout <<"building label pattern using first " <<percent <<"% of vector1 as sample...\n";
    int i, count=0;
    for (i=0; i<(int) (vec1.entries.size()*(percent/100.0)); i++) {
	V1Entry entry = vec1.entries[i];
	int j;
	for (j=0; j<entry.labels.size(); j++) {
	    Words* list = &pattern.labelEst[entry.labels[j]];
	    int k;
	    for (k=0; k<entry.words.size(); k++) {
		(*list).weights[entry.words[k]] += entry.wordCounts[k];
	    }
	    //pattern.labelEst[entry.labels[j]] = list;
	}
	if (j>0)
	    count++;
    }
    cout <<(count/(vec1.entries.size()*(percent/100.0)))*100 <<"% of sample was helpful...done\n";
}

/**********************************************/

int classifyEntries (Vector1& vec1, int percent) {
    cout <<"classifying last " <<100-percent <<"% of vector1...\n";
    int i;
    for (i=(int) (vec1.entries.size()*(percent/100.0)); i<vec1.entries.size(); i++) {
	V1Entry* entry = &vec1.entries[i];
	if ((*entry).labels.size() == 0) { //try to classify it
	    //cout <<"article " <<entry.articleId <<"...";

	    //get the three most popular words from this entry
	    string word1, //most popular
		word2,    //2nd most popular
		word3;    //...
	    int count1,   //count of word1
		count2,   //count of word2
		count3;   //...
	    if ((*entry).words.size() > 0) {
		word1 = (*entry).words[0];
		word2 = word1;
		word3 = word1;
		count1 = (*entry).wordCounts[0];
		count2 = count1;
		count3 = count1;
		int j;
		for (j=1; j<(*entry).words.size(); j++) {
		    if ((*entry).wordCounts[j] > count1) {
			count3 = count2;
			count2 = count1;
			count1 = (*entry).wordCounts[j];
			word3 = word2;
			word2 = word1;
			word1 = (*entry).words[j];
		    }
		    else if ((*entry).wordCounts[j] > count2) {
			count3 = count2;
			count2 = (*entry).wordCounts[j];
			word3 = word2;
			word2 = (*entry).words[j];
		    }
		    else if ((*entry).wordCounts[j] > count3) {
			count3 = (*entry).wordCounts[j];
			word3 = (*entry).words[j];
		    }
		}
	    }
	    else {//no words in the entry
		//cout <<"could not be classified\n";
		continue;
	    }

	    //find the best fit label for these three words (all three
	    //words may be the same)
	    double leastDist=1000000; //all euclid dists sould be
				      //smaller than this
	    map<string, Words>::iterator it = pattern.labelEst.begin();
	    map<string, Words>::iterator bestFit=it;
	    for ( ; it != pattern.labelEst.end(); it++) {
		Words list = (*it).second;
		//cout <<list.weights.size() <<'\n';
		int listW1 = list.weights[word1],
		    listW2 = list.weights[word2],
		    listW3 = list.weights[word3];

		//cout <<listW1 <<':' <<listW2 <<':' <<listW3 <<'\n';
		
		double dist1 = pow (list.weights[word1]-count1,2),
		       dist2 = pow (list.weights[word2]-count2,2),
		       dist3 = pow (list.weights[word3]-count3,2);
		
		double euclidDist = sqrt (dist1+dist2+dist3);
		if (!(dist1 == pow (count1,2) &&
		      dist2 == pow (count2,2) &&
		      dist3 == pow (count3,2)) ) {
		    if (euclidDist < leastDist) {
			leastDist = euclidDist;
			bestFit = it;
		    }
		}
	    }
	    (*entry).labels.insert ((*entry).labels.end(), (*bestFit).first);
	    //cout <<(*bestFit).first.c_str() <<'\n';
	}
    }
    cout <<"done\n";
}

/**********************************************/

#endif
