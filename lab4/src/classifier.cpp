/* 
 * File:   classifier.cpp
 * Author: Derek
 *
 * Created on March 2, 2012, 2:24 AM
 */

#include <iostream>
#include <cstdlib>

#include "knearest.h"
#include "naiveBayes.h"
#include "featureVector.h"

using namespace std;

string settings[] = {"naiveBayesSampleStart",//0
		     "naiveBayesSampleEnd",  //1
		     "naiveBayesTestStart",  //2
		     "naiveBayesTestEnd",    //3
		     "kNearestSampleStart",  //4
		     "kNearestSampleEnd",    //5
		     "kNearestTestStart",    //6
		     "kNearestTestEnd",      //7
		     "runNaiveBayes",        //8
		     "runKNearest",          //9
		     "kNeighbors"};          //10
double nbss = 0.5,  //        nb/kn              s/t          s/e
    nbse = 1.0,     //naiveBayes/kNearest   sample/test   start/end
    nbts = 0.0,     //...
    nbte = 0.5,     //nbte=naiveBayesTestEnd
    knss = 0.5,
    knse = 1.0,
    knts = 0.0,
    knte = 0.5;
int runNaiveBayes = 1,
    runKNearest = 0,
    kNeighbors = 3;

int loadSettings ();

/*
 * 
 */
int main(int argc, char** argv) {
    loadSettings();
    
    // Naive Bayes Classifier
    if (runNaiveBayes == 1) {
        Vector1 vector1;
        readVector1 (vector1, "vector1");
	nBCreateLabelWordMap (vector1, nbss, nbse);
	nBClassifyEntries (vector1, nbts, nbte);
        vector1.write();
    }

    // K-Nearest Neighbors Classifier
    if (runKNearest == 1) {
        Vector2 vector2;
        readVector2 (vector2, "vector2");
	kNClassifyEntries (vector2, knss, knse, knts, knte, kNeighbors);
        vector2.write();
    }

    return 0;
}

int loadSettings () {
    printf ("%s\n", "loading settings...");
    FILE* fdSettings;
    int pos1, pos2;
    char line[40];
    string strLine, setting, value;

    fdSettings = fopen ("classifierSettings.cfg", "r");
    if (fdSettings == NULL) {
	printf ("%s\n", "\"classifierSettings.cfg\" not found");
	return 1;
    }

    fscanf(fdSettings, "%s", line);
    while (feof(fdSettings) == 0) {
	strLine = string (line);
	pos1 = strLine.find ('"');
	pos2 = strLine.find ('"', pos1+1);
	setting = strLine.substr (0, pos1-1);
	value = strLine.substr (pos1+1, pos2-pos1-1);
	if (setting.compare (settings[0]) == 0)
	    nbss = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[1]) == 0)
	    nbse = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[2]) == 0)
	    nbts = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[3]) == 0)
	    nbte = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[4]) == 0)
	    knss = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[5]) == 0)
	    knse = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[6]) == 0)
	    knts = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[7]) == 0)
	    knte = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[8]) == 0) {
	    runNaiveBayes = atoi (value.c_str());
	}
	else if (setting.compare (settings[9]) == 0) {
	    runKNearest = atoi (value.c_str());
	}
	else if (setting.compare (settings[10]) == 0)
	    kNeighbors = atoi (value.c_str());
	else
	    cout <<"unknown setting: ";

	printf ("%s=\"%s\"\n", setting.c_str(), value.c_str());
        fscanf (fdSettings, "%s", line);
    }
    fclose (fdSettings);
    printf ("%s\n", "done loading settings");
    return 0;
}
