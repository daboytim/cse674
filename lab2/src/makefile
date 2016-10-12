preProcessor : preProcessor.o
	g++ -o preProcessor preProcessor.o

preProcessor.o : article.h featureVector.h listIO.h knearest.h naiveBayes.h preProcessor.cpp
	g++ -c preProcessor.cpp

