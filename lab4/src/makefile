all : preProcessor classifier cluster lab4classifier

preProcessor : preProcessor.o
	g++ -o preProcessor preProcessor.o

preProcessor.o : article.h featureVector.h listIO.h preProcessor.cpp
	g++ -c preProcessor.cpp

classifier : classifier.o
	g++ -o classifier classifier.o

classifier.o : knearest.h naiveBayes.h featureVector.h	classifier.cpp
	g++ -c classifier.cpp

cluster : cluster.o
	g++ -o cluster cluster.o

cluster.o : featureVector.h agglomerative.h kmeans.h cluster.h cluster.cpp
	g++ -c cluster.cpp

lab4classifier : lab4classifier.o
	g++ -o lab4classifier lab4classifier.o

lab4classifier.o : listIO.h featureVector.h rule.h lab4classifier.cpp
	g++ -c lab4classifier.cpp

clean :
	rm preProcessor.o classifier.o cluster.o lab4classifier.o

