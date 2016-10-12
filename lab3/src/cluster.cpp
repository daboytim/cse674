//Derek Boytim
//CSE 674
//Lab 3
//2-13-2012
//cluster.cpp

#include <iostream>
#include <stdio.h>
#include <string>

#include "featureVector.h"
#include "agglomerative.h"
#include "kmeans.h"

using namespace std;

/**********************************************/

string settings[] = {"runAgglomerative", //0
		     "runKMeans",        //1
		     "numberOfClusters", //2
		     "similarityMode",   //3
		     "numArticles",      //4
		     "printClusters",    //5
		     "writeClusters"};   //6

int runAgglom = 1,
    runKMeans = 0,
    numClusters = 150,
    simMode = 0,
    numArticles = 500,
    printClusters = 0,
    wrClusters = 1;

/**********************************************/

int loadSettings ();
void calculateEntropySkew (vector<Cluster>&, double&, double&, int);
void writeClusters (vector<Cluster>&);

/**********************************************/

int main(int argc, char* argv[]) {
    int rtn;
    Vector2 vec;
    vector<Cluster> clusters;

    loadSettings();
    rtn = readVector2 (vec, "vector2", numArticles);
    if (rtn != 0) {
	cout <<"failed reading vector2\n";
	return 1;
    }
    time_t seconds;
    seconds = time (NULL);
    if (runAgglom == 1)
	aggCluster (vec, clusters, numClusters, simMode);
    else if (runKMeans == 1)
	kmeansCluster (vec, clusters, numClusters, simMode);
    seconds = time (NULL) - seconds;
    cout <<"Time: " <<seconds <<" sec\n";

    double entropy, skew;
    calculateEntropySkew (clusters, entropy, skew, vec.entries.size());
    cout <<"Entropy: " <<entropy <<'\n';
    cout <<"Skew: " <<skew <<'\n';
    
    if (printClusters) {
	int i;
	for (i=0; i<clusters.size(); i++) {
	    cout <<"cluster " <<i <<"\n";
	    clusters[i].print();
	    if ((i+1)%2 == 0) {
		getchar();
	    }
	}
    }

    if (wrClusters)
	writeClusters(clusters);

    return 0;
}

/**********************************************/

void calculateEntropySkew (vector<Cluster>& clusters, double& entropy, double& skew, int total) {
    int i;
    double avgSize=clusters.size()/total;
    entropy = 0.0;
    skew = 0.0;
    for (i=0; i<clusters.size(); i++) {
	double ent, size = (double)clusters[i].entries.size();
	double weight = size/total;
	
	clusters[i].entropy (ent);
	entropy += weight*ent;
	skew += pow (size - avgSize, 2);
    }
    skew /= (int)clusters.size()-1;
    skew = sqrt(skew);
}

/**********************************************/

int loadSettings () {
    printf ("%s\n", "loading settings...");
    FILE* fdSettings;
    int pos1, pos2;
    char line[40];
    string strLine, setting, value;

    fdSettings = fopen ("clusterSettings.cfg", "r");
    if (fdSettings == NULL) {
	printf ("%s\n", "\"clusterSettings.cfg\" not found");
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
	    runAgglom = atoi (value.c_str());
	else if (setting.compare (settings[1]) == 0)
	    runKMeans = atoi (value.c_str());
	else if (setting.compare (settings[2]) == 0)
	    numClusters = atoi (value.c_str());
	else if (setting.compare (settings[3]) == 0)
	    simMode = atoi (value.c_str());
	else if (setting.compare (settings[4]) == 0)
	    numArticles = atoi (value.c_str());
	else if (setting.compare (settings[5]) == 0)
	    printClusters = atoi (value.c_str());
	else if (setting.compare (settings[6]) == 0)
	    wrClusters = atoi (value.c_str());
	else 
	    cout <<"unknown setting: ";
	
	printf ("%s=\"%s\"\n", setting.c_str(), value.c_str());
        fscanf (fdSettings, "%s", line);
    }
    fclose (fdSettings);
    printf ("%s\n", "done loading settings");
    return 0;
}

/**********************************************/

void writeClusters (vector<Cluster>& clusters) {
    cout <<"writing clusters...";
    FILE* fdClusters;

    fdClusters = fopen ("clusters", "w");

    int i;
    for (i=0; i<clusters.size(); i++) {
	fprintf (fdClusters, "%s %d\n", "cluster", i);
	fprintf (fdClusters, "%s\n", clusters[i].toString().c_str());
    }

    fclose(fdClusters);
    cout <<"done\n";
}

/**********************************************/
