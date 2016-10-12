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

#define blockSize 16384  //16KB

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
		     "displayHistogram",  //13
                     "writeLabelList",    //14
                     "transactionBegin",  //15
                     "transactionEnd"};   //16
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
string dir ("/home/0/srini/WWW/674/public/reuters/"); //for unix
//string dir ("E:\\My Documents\\OSU Coursework\\12_Winter\\CSE 674\\reuters\\"); //for windows
int upperFreqCutoff = 900, lowerFreqCutoff = 110;
int ldStopWords = 1,
    ldIgnoreWords = 0,
    makeVector1 = 1,
    makeVector2 = 0,
    wrVector1 = 1,
    wrVector2 = 1,
    wrIgnoreWords = 1,
    wrWordList = 1,
    wrLabelList = 1,
    askForFreqCutoff = 1,
    numBins = 100,
    displayHistogram = 0,
    transBegin = 0,
    transEnd = 0;

/**********************************************/

int loadSettings ();
int cycleArticleDir (const char*);
int parseArticle (const char*);
void processWordList ();
void removeIgnoreWords ();
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
    
    if (makeVector1 == 1 && wrVector1 == 1) {
	vector1.transactional(transBegin, transEnd);
        vector1.write();
    }
    if (makeVector2 == 1 && wrVector2 == 1)
	vector2.write();

    if (wrIgnoreWords == 1)
	writeIgnoreWords(ignoreWords);
    if (wrWordList == 1)
	writeWordList(wordList);
    if (wrLabelList == 1)
        writeLabelList(topicsList);
    
    return 0;
}

/**********************************************/

void processWordList () {
    cout <<"processing wordList...\n";
    map<string,int>::iterator it=wordList.begin();
    int counts[numBins];
    int i;
    double max = (*it).second;
    cout <<"wordList.size(): " <<wordList.size() <<'\n';

    //find frequency of most frequent word
    for (it=wordList.begin(); it != wordList.end(); it++) {
	if ((*it).second > max)
	    max = (*it).second;
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
    int pos1, pos2;
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
	    displayHistogram = atoi (value.c_str());
        else if (setting.compare (settings[14]) == 0)
	    wrLabelList = atoi (value.c_str());
	else if (setting.compare (settings[15]) == 0)
	    transBegin = atoi (value.c_str());
	else if (setting.compare (settings[16]) == 0)
	    transEnd = atoi (value.c_str());
	else
	    cout <<"ERROR, unknown setting: ";
	
	printf ("%s=\"%s\"\n", setting.c_str(), value.c_str());
        fscanf (fdSettings, "%s", line);
    }
    fclose (fdSettings);
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
    for(i=0; i<str.size(); i++) {
	str[i] = tolower (str[i]);
    }

    //ignore if in a 'bad word' list
    set<string>::iterator it;
    it = stopWords.find(str);
    if (it != stopWords.end())
	return;
    it = ignoreWords.find(str);
    if (it != ignoreWords.end())
	return;
    
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
    if (str.size() == 0) {
        return;
    }

    //ignore if in a 'bad word' list
    it = stopWords.find(str);
    if (it != stopWords.end())
	return;
    it = ignoreWords.find(str);
    if (it != ignoreWords.end())
	return;

    //add the word to the article
    wordList[str]++;
    (*article).addWord(str, weight);
}

/**********************************************/

//in parseArticle() i read a large
//block of the file as a char[], i convert the char[]
//to a string and then extract usefull information from
//it by finding the tags that i care about

int parseArticle (const char* fileName) {
    FILE* file;
    int bytesRead, pos1, pos2, size, i, start, end;
    char block[blockSize];
    char tmp[20];
    string strBlock, strSegment, strTmp, strArticle;
    //open the file
    file = fopen ((string(dir)+fileName).c_str(), "rb");
    if (file == NULL) {
	return 1;
    }
    bytesRead = fread (block, sizeof (char), blockSize, file);
    strBlock = string (block);
    do {
	//find the whole article
	pos1 = strBlock.find (openTags[0]) + openTags[0].size();
	pos2 = strBlock.find (closeTags[0], pos1);
	while (pos2 < 0 && feof(file) == 0) {
            bytesRead = fread (block, sizeof (char), blockSize, file);
	    strBlock += string (block, bytesRead);
            pos2 = strBlock.find (closeTags[0], pos1);
	}
	if (feof (file) != 0 && pos2 < 0) {
	    break;
        }
	strArticle = string (strBlock, pos1, pos2-pos1);
	pos2 += closeTags[0].size();
	strBlock.replace (0, pos2, "");
	
	//find NEWID and create Article object
	pos1 = strArticle.find (newid) + newid.size();
	pos2 = strArticle.find ("\"", pos1);
	size = strArticle.copy (tmp, pos2-pos1, pos1);
	tmp[size] = '\0';
	Article article (atoi (tmp));
	
	//find TOPICS and add to lists
	pos1 = strArticle.find (openTags[2]) + openTags[2].size();
	pos2 = strArticle.find (closeTags[2], pos1);
	if(pos2 > pos1) {
	    strSegment = strArticle.substr (pos1, pos2-pos1);
	    i = strSegment.find (openTags[13]) + openTags[13].size();
	    i = strSegment.find (">", i);
	    for ( ; i<strSegment.size(); i++) {
		while (isalpha (strSegment[i]) == 0 && i<strSegment.size()) {i++;}
		start = i;
		while ((isalpha (strSegment[i]) != 0 || strSegment[i] == '-') && i<strSegment.size()) {i++;}
		end = i;
		strTmp = strSegment.substr (start, end-start);
		if (strTmp.compare("D") == 0)
		    continue;
                int j;
                for(j=0; j<strTmp.size(); j++) {
                    strTmp[j] = tolower (strTmp[j]);
                }
		article.addLabel (strTmp);
 		topicsList.insert (strTmp);
		wordList[strTmp]++;
	    }
	    pos2 += closeTags[2].size();
	    strArticle.replace (0, pos2, "");
        }

	//find TEXT
	pos1 = strArticle.find (openTags[9]);
	if (pos1 > 0) {
	    pos1 = strArticle.find ('>', pos1);
	    strArticle.replace (0, pos1, "");
	    pos2 = strArticle.find (closeTags[9]);
	    if (pos2 > 0) {
	        strArticle.replace (pos2, (int)strArticle.size()-pos2, "");
            }
	}

	//find TITLE and add words to lists
	pos1 = strArticle.find (openTags[10]) + openTags[10].size();
	pos2 = strArticle.find (closeTags[10], pos1);
	if(pos2 > pos1) {
	    strSegment = strArticle.substr (pos1, pos2-pos1);
	    i = 0;
	    for ( ; i<strSegment.size(); i++) {
		while (isalpha (strSegment[i]) == 0 && i<strSegment.size()) {i++;}
		start = i;
		while (isalpha (strSegment[i]) != 0 && i<strSegment.size()) {i++;}
		end = i;
		strTmp = strSegment.substr (start, end-start);
		addWord (&article, strTmp);
	    }
	    pos2 += closeTags[10].size();
	    strArticle.replace (0, pos2, "");
	}
	
	//find BODY and add words to lists
	pos1 = strArticle.find (openTags[12]);
	if (pos1 > 0) {
	    pos1 = strArticle.find ('>', pos1);
	    strArticle.replace (0, pos1, "");
            pos2 = strArticle.find (closeTags[12]);
	    if (pos2 > 0) {
	        strArticle.replace (pos2, (int)strArticle.size()-pos2, "");
	    }
	}

	for (i=0 ; i<(int)strArticle.size(); i++) {
	    while (isalpha (strArticle[i]) == 0) {i++;}
	    start = i;
	    while (isalpha (strArticle[i]) != 0) {i++;}
	    end = i;
	    if (i >= strArticle.size()) {break;}
	    strTmp = strArticle.substr (start, end-start);
	    addWord (&article, strTmp);
	}
	strArticle = "";
        articles.insert (articles.end(), article);

	if (strBlock.size() < blockSize && feof(file) == 0) {
	    bytesRead = fread (block, sizeof (char), blockSize, file);
	    strBlock += string (block, bytesRead);
	}
    } while ((int)strBlock.size() > 0);

    fclose(file);
    return 0;
}

/**********************************************/

