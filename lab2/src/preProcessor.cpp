//Derek Boytim
//CSE 674
//Lab 1
//1-18-2012
//preProcessor.cpp
//updated 1-29-2012 for lab2


#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "article.h"
#include "featureVector.h"
#include "listIO.h"
#include "knearest.h"
#include "naiveBayes.h"

#define blockSize 1024

using namespace std;

/**********************************************/

Vector1 vector1;
Vector2 vector2;
set<string> stopWords;
set<string> ignoreWords;
vector<Article> articles;
map<string, int> wordList;
set<string> topicsList;
string settings[] = {"loadStopWords",     //0
		     "loadIgnoreWords",   //1
		     "makeVector1",       //2
		     "makeVector2",       //3
		     "writeIgnoreWords",  //4
		     "writeWordList",     //5
		     "lowerFreqCutoff",   //6
		     "upperFreqCutoff",   //7
		     "reuterDir",         //8
		     "askForFreqCutoff",  //9
		     "numberOfBins",      //10
		     "writeVector1",      //11
		     "writeVector2",      //12
		     "naiveBayesSampleStart",//13
		     "naiveBayesSampleEnd",  //14
		     "naiveBayesTestStart",  //15
		     "naiveBayesTestEnd",    //16
		     "kNearestSampleStart",  //17
		     "kNearestSampleEnd",    //18
		     "kNearestTestStart",    //19
		     "kNearestTestEnd",      //20
		     "runNaiveBayes",        //21
		     "runKNearest",          //22
		     "kNeighbors",           //23
		     "displayHistogram"};    //24
string openTags[] = {"<REUTERS",   //0
		     "<DATE",      //1
		     "<TOPICS",    //2
		     "<PLACES",    //3
		     "<PEOPLE",    //4
		     "<ORGS",      //5
		     "<EXCHANGES", //6
		     "<COMPANIES", //7
		     "<UNKNOWN",   //8
		     "<TEXT",      //9
		     "<TITLE",     //10
		     "<DATELINE",  //11
		     "<BODY",      //12
		     "<D",         //13
		     "<AUTHOR"};   //14
string closeTags[] = {"</REUTERS>",
		      "</DATE>",
		      "</TOPICS>",
		      "</PLACES>",
		      "</PEOPLE>",
		      "</ORGS>",
		      "</EXCHANGES>",
		      "</COMPANIES>",
		      "</UNKNOWN>",
		      "</TEXT>",
		      "</TITLE>",
		      "</DATELINE>",
		      "</BODY>",
		      "</D>",
		      "</AUTHOR>"};

		     
string newid ("NEWID=\"");
string dir ("/home/0/srini/WWW/674/public/reuters/");
int upperFreqCutoff = 900, lowerFreqCutoff = 110;
int ldStopWords = 1,
    ldIgnoreWords = 0,
    makeVector1 = 1,
    makeVector2 = 0,
    wrVector1 = 1,
    wrVector2 = 1,
    wrIgnoreWords = 1,
    wrWordList = 1,
    askForFreqCutoff = 1,
    numBins = 100,
    runNaiveBayes = 1,
    runKNearest = 0,
    kNeighbors = 3,
    displayHistogram = 0;
double nbss = 0.5,  //        nb/kn              s/t          s/e
    nbse = 1.0,     //naiveBayes/kNearest   sample/test   start/end
    nbts = 0.0,     //...
    nbte = 0.5,     //nbte=naiveBayesTestEnd
    knss = 0.5,
    knse = 1.0,
    knts = 0.0,
    knte = 0.5;
    

/**********************************************/

int loadSettings ();
int cycleArticleDir (const char*);
int parseArticle (const char*);
void processWordList ();
void removeIgnoreWords ();
bool containsAVowel (string);
string removePrefix (string);
string removeSuffix (string);
int test();

/**********************************************/

int main (int argc, char* argv[]) {
    loadSettings();           //read settings.cfg  
    if (ldStopWords == 1)
	loadStopWords(stopWords);      //read stopWords.lst
    if (ldIgnoreWords == 1)
	loadIgnoreWords(ignoreWords);    //read ignoreWords.lst
    
    cycleArticleDir(dir.c_str());

    processWordList();
 
    if (makeVector1 == 1)
	vector1 = createVector1(wordList, articles);
    if (makeVector2 == 1)
	vector2 = createVector2(wordList, articles);

    //lab 2 classification

    // Naive Bayes Classifier
    if (runNaiveBayes == 1) {
	nBCreateLabelWordMap (vector1, nbss, nbse);
	nBClassifyEntries (vector1, nbts, nbte);
    }
    
    // K-Nearest Neighbors Classifier
    if (runKNearest == 1) {
	kNClassifyEntries (vector2, knss, knse, knts, knte, kNeighbors);
    }

    //Lab 2 end
    
    if (makeVector1 == 1 && wrVector1 == 1)
	vector1.write();
    if (makeVector2 == 1 && wrVector2 == 1)
	vector2.write();
    
    if (wrIgnoreWords == 1)
	writeIgnoreWords(ignoreWords);
    if (wrWordList == 1)
	writeWordList(wordList);
    
    return 0;
}

/**********************************************/

void processWordList () {
    cout <<"processing wordList...\n";
    map<string,int>::iterator it=wordList.begin();
    string word, tmp;
    int counts[numBins];
    int i,j, count;
    double max = (*it).second;
    cout <<"wordList.size(): " <<wordList.size() <<'\n';

    //find frequency of most frequent word
    for (it=wordList.begin(); it != wordList.end(); it++) {
	count = (*it).second;
	if (count > max)
	    max = count;
    }
    cout <<"word max freq: " <<max <<'\n';
    
    //display histogram
    if (displayHistogram == 1) {
	//build histogram
	for (i=0; i<numBins; i++) {
	    counts[i] = 0;
	}
	for (it=wordList.begin(); it != wordList.end(); it++) {
	    counts[(int) ( (*it).second / ceil (max/numBins) )]++;
        }
	cout <<"***Histogram of word frequency***\n";
	cout <<numBins <<" bins of width " << floor(max/numBins) <<"\n\n";
	cout <<"#  bound  count\n";
	//display 60 bins at a time
	char c;
	for (i=0; i<numBins; i++) {
	    cout <<i+1 <<" (" <<(int)((i+1)*max/numBins) <<"): ";
	    cout <<counts[i] <<'\n';
	    if ((i+1)%40 == 0) {
		cout <<"Press enter for more bins";
		getchar();
	    }
	}
    }

    //trim wordList
    if (askForFreqCutoff == 1) {
	printf ("%s%d%s", "Lower Frequency Cutoff(", lowerFreqCutoff, "): ");
	scanf ("%d", &lowerFreqCutoff);
	printf ("%s%d%s", "Upper Frequency Cutoff(", upperFreqCutoff, "): ");
	scanf ("%d", &upperFreqCutoff);
    }
    for (it=wordList.begin(); it != wordList.end(); it++) {
	if ((*it).second < lowerFreqCutoff || (*it).second > upperFreqCutoff) {
	    ignoreWords.insert((*it).first);
	}
    }
    cout <<"ignoreWords.size(): " <<ignoreWords.size() <<'\n';
    removeIgnoreWords();
    cout <<"done processig wordList\n";
}

/**********************************************/

void removeIgnoreWords () {
    int i;
    set<string>::iterator it = ignoreWords.begin();
    for ( ; it != ignoreWords.end(); it++) {
        wordList.erase(*it);
    }
    cout <<"wordList.size(): " <<wordList.size() <<'\n';
}

/**********************************************/

int loadSettings () {
    printf ("%s\n", "loading settings...");
    FILE* fdSettings;
    int bytesRead, pos1, pos2;
    char line[40];
    string strLine, setting, value;

    fdSettings = fopen ("settings.cfg", "r");
    if (fdSettings == NULL) {
	printf ("%s\n", "\"settings.cfg\" not found");
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
	    ldStopWords = atoi (value.c_str());
	else if (setting.compare (settings[1]) == 0)
	    ldIgnoreWords = atoi (value.c_str());
	else if (setting.compare (settings[2]) == 0)
	    makeVector1 = atoi (value.c_str());
	else if (setting.compare (settings[3]) == 0)
	    makeVector2 = atoi (value.c_str());
	else if (setting.compare (settings[4]) == 0)
	    wrIgnoreWords = atoi (value.c_str());
	else if (setting.compare (settings[5]) == 0)
	    wrWordList = atoi (value.c_str());
	else if (setting.compare (settings[6]) == 0)
	    lowerFreqCutoff = atoi (value.c_str());
	else if (setting.compare (settings[7]) == 0)
	    upperFreqCutoff = atoi (value.c_str());
	else if (setting.compare (settings[8]) == 0)
	    dir = value;
	else if (setting.compare (settings[9]) == 0)
	    askForFreqCutoff = atoi (value.c_str());
	else if (setting.compare (settings[10]) == 0)
	    numBins = atoi (value.c_str());
	else if (setting.compare (settings[11]) == 0)
	    wrVector1 = atoi (value.c_str());
	else if (setting.compare (settings[12]) == 0)
	    wrVector2 = atoi (value.c_str());	
	else if (setting.compare (settings[13]) == 0)
	    nbss = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[14]) == 0)
	    nbse = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[15]) == 0)
	    nbts = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[16]) == 0)
	    nbte = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[17]) == 0)
	    knss = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[18]) == 0)
	    knse = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[19]) == 0)
	    knts = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[20]) == 0)
	    knte = strtod (value.c_str(), NULL);
	else if (setting.compare (settings[21]) == 0) {
	    runNaiveBayes = atoi (value.c_str());
	    if (runNaiveBayes == 1) {
		makeVector1 = 1;
		makeVector2 = 0;
	    }
	}
	else if (setting.compare (settings[22]) == 0) {
	    runKNearest = atoi (value.c_str());
	    if (runKNearest == 1) {
		makeVector1 = 0;
		makeVector2 = 1;
	    }
	}
	else if (setting.compare (settings[23]) == 0)
	    kNeighbors = atoi (value.c_str());
	else if (setting.compare (settings[24]) == 0)
	    displayHistogram = atoi (value.c_str());
	else 
	    cout <<"unknown setting: ";
	
	printf ("%s=\"%s\"\n", setting.c_str(), value.c_str());
        fscanf (fdSettings, "%s", line);
    }
    printf ("%s\n", "done loading settings");
    return 0;
}

/**********************************************/

int cycleArticleDir (const char* dir) {
    DIR* reuters = opendir(dir);
    dirent* entry = readdir(reuters); //ignore .
    entry = readdir(reuters);         //ignore ..
    entry = readdir(reuters);
    int i=0;
    while(entry != NULL) {
	i++;
	char* fileName = entry->d_name;
	cout <<fileName <<"...";
	int rtnval = parseArticle(fileName);
	if(rtnval == 1) {
	    printf("%s\n", "failed");
	    return 1;
	}
	else {
	    printf("%s\n", "done");
	}
	entry = readdir(reuters);
    }
    return 0;
}

/**********************************************/

void addWord (Article* article, string str, int weight=1) {
    int i;
    //if the word does not contain a vowel, ignore it
    if (!containsAVowel (str)) {
	return;
    }
    
    //remove prefixes and suffixes
    int pos = str.find ("ies");
    if (pos == str.size()-3 && pos > 0) {
	str[pos] = 'y';
	str = str.substr (0, pos+1);
    }
    pos = str.find_last_of ('s');
    if (pos == str.size()-1 && pos > 0) {
	str = str.substr (0, pos);
    }
    pos = str.find ("ing");
    if (pos == str.size()-3 && pos > 0) {
	str = str.substr (0, pos);
	//possible 'drop the e and add ing word'
	string tmp = str+'e';
	if (wordList[tmp] != 0) {
	    str = tmp;
	}
	else {
	    wordList.erase (tmp);
	}
    }
    pos = str.find ("ed");
    if (pos == str.size()-2 && pos > 0) {
	str = str.substr (0, pos);
	//possible end in e word and add d for past tense
	string tmp = str+'e';
	if (wordList[tmp] != 0) {
	    str = tmp;
	}
	else {
	    wordList.erase (tmp);
	}
    }
    pos = str.find ("er");
    if (pos == str.size()-2 && pos > 0) {
	str = str.substr (0, pos);
	//possible end in e word and add r for adjective
	string tmp = str+'e';
	if (wordList[tmp] != 0) {
	    str = tmp;
	}
	else {
	    wordList.erase (tmp);
	}
    }
    pos = str.find ("ly");
    if (pos == str.size()-2 && pos > 0) {
	str = str.substr (0, pos);
    }
    pos = str.find ("re");
    if (pos == 0) {
	str = str.substr (2);
    }
    pos = str.find ("pre");
    if (pos == 0) {
	str = str.substr (3);
    }

    //make a copy to edit
    string strCopy = str;
    bool possibleProperNoun = false;
    //check if only 1st char of word is uppercase -?-> Proper Noun
    //make the copy lowercase
    for(i=0; i<strCopy.size(); i++) {
	if (isupper (strCopy[i]) != 0) {
	    if (i == 0) {
		possibleProperNoun = true;
	    }
	    else {
		possibleProperNoun = false;
	    }
	    strCopy[i] = tolower (strCopy[i]);
	}
    }

    //ignor if in a 'bad word' list 
    set<string>::iterator it;
    it = stopWords.find(strCopy);
    if (it != stopWords.end())
	return;
    it = ignoreWords.find(strCopy);
    if (it != ignoreWords.end())
	return;
    it = ignoreWords.find(str);
    if (it != ignoreWords.end())
	return;

    //add the word to the article
    if (possibleProperNoun) { //choose upper case word
	wordList[str]++;
	(*article).addWord(str, weight);
    }
    else {                    //choose lower case word
	wordList[strCopy]++;
	(*article).addWord(strCopy, weight);
    }
}

/**********************************************/

//in parseArticle() i read a large
//block of the file as a char[], i convert the char[]
//to a string and then extract usefull information from
//it by finding the tags that i care about
//
//**known error: some TOPICS are parsed incorrectly

int parseArticle (const char* fileName) {
    FILE* file;
    int bytesRead, pos1, pos2, size, i, start, end;
    char block[blockSize];
    char segment[200];
    char tmp[20];
    string strBlock, strSegment, strTmp;
    //open the file
    file = fopen ((string(dir)+fileName).c_str(), "rb");
    if (file == NULL) {
	return 1;
    }
    bytesRead = fread (block, sizeof (char), blockSize, file);
    strBlock = string (block);
    
    do {
	//find begining of article <REUTER
	pos1 = strBlock.find (openTags[0]);
	
	//find NEWID and create Article object
	pos1 = strBlock.find (newid, pos1) + newid.size();
	pos2 = strBlock.find ("\"", pos1);
	size = strBlock.copy (tmp, pos2-pos1, pos1);
	tmp[size] = '\0';
	Article article (atoi (tmp));
	
	//find TOPICS and add to lists
	pos1 = strBlock.find (openTags[2], pos1) + openTags[2].size();
	pos2 = strBlock.find (closeTags[2], pos1);
	if(pos2 > pos1) {
	    strSegment = strBlock.substr (pos1, pos2-pos1);
	    i = strSegment.find (openTags[13]) + openTags[13].size();
	    i = strSegment.find (">", i);
	    for ( ; i<strSegment.size(); i++) {
		while (isalpha (strSegment[i]) == 0 && i<strSegment.size()) {i++;}
		start = i;
		while (isalpha (strSegment[i]) != 0 && i<strSegment.size()) {i++;}
		end = i;
		strTmp = strSegment.substr (start, end-start);
		if (strTmp.compare("D") == 0)
		    continue;
		article.addLabel (strTmp);
 		topicsList.insert (strTmp);
		//wordList[strTmp]++;
	    }
	}
	pos2 += closeTags[2].size();
	strBlock.replace (0, pos2, "");

	//find TITLE and add words to lists: weight 5
	pos1 = strBlock.find (openTags[10]) + openTags[10].size();
	pos2 = strBlock.find (closeTags[10], pos1);
	if(pos2 > pos1) {
	    strSegment = strBlock.substr (pos1, pos2-pos1);
	    i = 0;
	    for ( ; i<strSegment.size(); i++) {
		while (isalpha (strSegment[i]) == 0 && i<strSegment.size()) {i++;}
		start = i;
		while (isalpha (strSegment[i]) != 0 && i<strSegment.size()) {i++;}
		end = i;
		strTmp = strSegment.substr (start, end-start);
		addWord (&article, strTmp, 5);
	    }
	}
	pos2 += closeTags[10].size();
	strBlock.replace (0, pos2, "");
	
	//find BODY and add words to lists
	pos1 = strBlock.find (openTags[12]);
	pos2 = strBlock.find (closeTags[12]);
	while (pos2 < 0 || pos1 < 0) {
	    //didnt find whole body look for end of article
	    int pos3 = strBlock.find (closeTags[0]);
	    if (pos3 >= 0) {
		pos2 = pos3;
		pos1 = pos2;
		break;
	    }
	    bytesRead = fread (block, sizeof (char), blockSize, file);
	    strBlock += string (block);

	    pos1 = strBlock.find (openTags[12]);
	    pos2 = strBlock.find (closeTags[12]);
	    //look for end of article again
	    pos3 = strBlock.find (closeTags[0]);
	    if (pos3 >= 0 && pos3 < pos1) {
		pos2 = pos3;
		pos1 = pos2;
		break;
	    }
	}
	//added due to articles with no BODY "eating" the next article
	int pos3 = strBlock.find (closeTags[0]);
	if (pos3 >= 0 && pos3 < pos1) {
	    pos2 = pos3;
	    pos1 = pos2;
	}
	i = pos1+openTags[12].size();
	for ( ; i<pos2; i++) {
	    while (isalpha (strBlock[i]) == 0) {i++;}
	    start = i;
	    while (isalpha (strBlock[i]) != 0) {i++;}
	    end = i;
	    strTmp = strBlock.substr (start, end-start);
	    addWord (&article, strTmp);
	}
	pos2 += closeTags[12].size();
	strBlock.replace (0, pos2, "");
	
	if (strBlock.size() < blockSize) {
	    bytesRead = fread (block, sizeof (char), blockSize, file);
	    strBlock += string (block);
	}
	articles.insert (articles.end(), article);
    } while (bytesRead > 0);

    fclose(file);
    return 0;
}

/**********************************************/

bool containsAVowel (string word) {
    int i;
    char c;
    for (i=0; i<word.size(); i++) {
	c = tolower(word[i]);
	if (c == 'a' || c == 'e' || c == 'i' ||
	    c == 'o' || c == 'u') {
	    return true;
	}
    }
    return false;
}

/**********************************************/

