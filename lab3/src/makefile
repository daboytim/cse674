all : preProcessor cluster

preProcessor : preProcessor.o
	g++ -o preProcessor preProcessor.o

preProcessor.o : article.h featureVector.h listIO.h knearest.h \
		naiveBayes.h preProcessor.cpp
	g++ -c preProcessor.cpp

cluster : cluster.o
	g++ -o cluster cluster.o

cluster.o : featureVector.h agglomerative.h kmeans.h cluster.h cluster.cpp
	g++ -c cluster.cpp

clean :
	rm preProcessor.o cluster.o


