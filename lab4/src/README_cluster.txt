Compile with "clsuter.cpp", "cluster.h", "featureVector.h", "agglomerative.h" and "kmeans.h" in the same directory using the command "g++ -o <outputFile> cluster.cpp".

The program is designed to run without command line input. This program will cluster the articles in file "vector2" which was created and classified using preProcessor. The output of clustered articles will be in a file named "clusters". The clusters file will contain only the articleID and the class label(s). The keyword dimensions are omitted to make the file easier to read.

This program supports a settings file for running different clustering algorithms and for usig different metrics for determining similarity/difference. Default settings can be changed by declaring a file named "clusterSettings.cfg" and listing settings, one per line, in the form: 

	 <setting name>="<value>"

This file must contain a newline charcter after the last setting. Default settings are as follows:

	int runAgglomerative = 1
	int runKMeans = 0
	int numberOfClusters = 150
	int similarityMode = 0
	int numArticles = 500
	int printClusters = 0
	int writeClusters = 1

Only one of the algorithms, agglomerative or k-means, can be run each time the program is executed. Setting both to a value of 1 will cause only the agglomerative algorithm to run. 

By setting numArticles to 0 the program will read in all articles listed in vector2 otherwise only the first n, where numArticles = n, articles will be read from the file.

Similarity mode can take on one of several values correspoinding to different metrics. These are as follows:
	
	0 -> euclidian distance
	1 -> dice similarity
	2 -> jaccard similarity
	3 -> cosine similarity
	4 -> overlap similarity
