Compile with "lab4classifier.cpp", "featureVector.h", "rule.h" and "listIO.h" in the same directory using the command "g++ -o <outputFile> lab4classifier.cpp".

This program is designed to run without command line input. This program will classify articles in the file "testingData" or another file set by the setting fileToClassify.

Default settings can be changed by declaring a file named "lab4ClassifierSettings.cfg" and listing settings, one per line, in the form: 

	 <setting name>="<value>"

This file must contain a newline charcter after the last setting. Default settings are as follows:

	int classifyBegin = 0
	itn classifyEnd = 6473
	int skipLabeledArticles = 0
	string fileToClassify = testingData

The setting classifyBegin states the index of the article in the input file to begin classifying at.

The setting classifyEnd stated the index after the last article to classify, setting this to 0 will classify all articles from classifyeBegin to the end of the input file.

The setting skipLabeledArticles selects whether or not articles that already have a label should be skiped. 0 = false, 1 = true

A file named rules.lst is expected to be in the same directory as the executable. This file is used to load rules. Also, files named importantWords.lst and labels.lst are also expected to be in the same directory. These files are creted by preProcessor and are used to prune the rules.