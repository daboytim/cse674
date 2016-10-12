//Derek Boytim
//CSE 674
//Lab 1
//1-18-2012
//preProcessor.cpp

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "article.h"
#include "featureVector.h"

#define blockSize 1024

using namespace std;

set<string> stopWords;
set<string> ignoreWords;
vector<Article> articles;
map<string, int> wordList;
set<string> topicsList;
string settings[] = {"loadStopWords",
		     "loadIgnoreWords",
		     "makeVector1",
		     "makeVector2",
		     "writeIgnoreWords",
		     "writeWordList",
		     "lowerFreqCutoff",
		     "upperFreqCutoff",
		     "reuterDir",
		     "askForFreqCuttoff",
		     "numberOfBins"};
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
		     "<D>",        //13
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
#define prefixSize 8
string prefixes[] = {"re",
		     "post",
		     "pre",
		     "be",
		     "co",
		     "de",
		     "en",
		     "ex"};
#define suffixSize 10
string suffixes[] = {"s",
		     "d",
		     "er",
		     "ed",
		     "st",
		     "est",
		     "ing",
		     "ly",
		     "cy",
		     "an"};
		     
string newid ("NEWID=\"");
string dir ("/home/0/srini/WWW/674/public/reuters/");
int upperFreqCutoff = 900, lowerFreqCutoff = 100;
int ldStopWords = 1,
    ldIgnoreWords = 0,
    makeVector1 = 1,
    makeVector2 = 0,
    wrIgnoreWords = 1,
    wrWordList = 1,
    askForFreqCutoff = 1,
    numBins = 100;

int loadSettings ();
int loadStopWords ();
int loadIgnoreWords ();
int writeIgnoreWords ();
int writeWordList ();
int cycleArticleDir (const char*);
int parseArticle (const char*);
void processWordList ();
void removeIgnoreWords ();
void createVector1 ();
void createVector2 ();
bool containsAVowel (string);
string removePrefixOrSuffix (string);

/**********************************************/

int main (int argc, char* argv[]) {
    loadSettings();           //read settings.cfg  
    if (ldStopWords == 1)
	loadStopWords();      //read stopWords.lst
    if (ldIgnoreWords == 1)
	loadIgnoreWords();    //read ignoreWords.lst
    
    cycleArticleDir(dir.c_str());

    processWordList();
    
//print wordList    
//     map<string, int>::iterator it = wordList.begin();
//     while(it != wordList.end()) {
// 	printf("%s\t\t%d\n", (*it).first.c_str(), (*it).second);
// 	it++;
//     }
    
    if (makeVector1 == 1)
	createVector1(wordList, articles);
    if (makeVector2 == 1)
	createVector2(wordList, articles);

    if (wrIgnoreWords == 1)
	writeIgnoreWords();
    if (wrWordList == 1)
	writeWordList();
    
    return 0;
}

/**********************************************/

void processWordList () {
    map<string,int>::iterator it=wordList.begin();
    string word, tmp;
    int counts[numBins];
    int i,j, count, max = (*it).second;
    cout <<"wordList.size(): " <<wordList.size() <<endl;
    for (i=0; i<numBins; i++) {
	counts[i] = 0;
    }
    for ( ; it != wordList.end(); it++) {
	count = (*it).second;
	//counts[count/131]++;
	if (count > max)
	    max = count;
//      tmp = removePrefixOrSuffix (word);
// 	if (word != tmp) {
// 	    if (wordList[tmp] == 0) {//tmp was not there before now
// 		wordList.erase (tmp);
// 	    }
// 	    else {
// 		wordList[tmp] += wordList[word];
// 		wordList.erase (word);
//              //i--;
// 	    }
// 	}
    }

    for (it=wordList.begin(); it != wordList.end(); it++) {
	counts[(*it).second/(max/numBins)]++;
    }
    cout <<"word max freq: " <<max <<endl;

    int maxFreq = counts[0];
    for (i=0; i<numBins; i++) {
	if (counts[i] > maxFreq)
	    maxFreq = counts[i];
    }
    cout <<"bin max freq: " <<maxFreq <<endl;
    
// //print histogram    
//     for (i=0; i<numBins; i++) {
// 	cout <<i+1 <<": ";
// 	cout <<counts[i] <<'\n';
//     }

    if (askForFreqCutoff == 1) {
	printf ("%s", "Lower Frequency Cutoff(110): ");
	scanf ("%d", &lowerFreqCutoff);
	printf ("%s", "Upper Frequency Cutoff(900): ");
	scanf ("%d", &upperFreqCutoff);
    }
    int tmp1=0, tmp2=0;
    for (it=wordList.begin(); it != wordList.end(); it++) {
	tmp1++;
	if ((*it).second < lowerFreqCutoff || (*it).second > upperFreqCutoff) {
	    ignoreWords.insert((*it).first);
	    tmp2++;
	}
    }
    cout <<tmp1 <<" : " <<tmp2 <<'\n';
    cout <<"ignoreWords.size(): " <<ignoreWords.size() <<'\n';
    removeIgnoreWords();
}

/**********************************************/

int writeIgnoreWords() {
    cout <<"writing ignoreWords...";
    FILE* iwf;
    iwf = fopen("ignoreWords.lst", "w");
    if (iwf == NULL) {
	cout <<"error\n";
	return 1;
    }
    int i;
    set<string>::iterator it = ignoreWords.begin();
    for (i=0; i<ignoreWords.size(); i++) {
        fprintf(iwf, "%s\n", (*it).c_str());
	it++;
    }
    fclose(iwf);
    cout <<"done\n";
}

/**********************************************/

int writeWordList() {
    cout <<"writing wordList...";
    FILE* iwf;
    iwf = fopen("importantWords.lst", "w");
    if (iwf == NULL) {
	cout <<"error\n";
	return 1;
    }
    int i;
    map<string,int>::iterator it = wordList.begin();
    for (i=0; i<wordList.size(); i++) {
        fprintf(iwf, "%s\n", (*it).first.c_str());
	it++;
    }
    fclose(iwf);
    cout <<"done\n";
}

/**********************************************/

void removeIgnoreWords () {
    int i;
    set<string>::iterator it = ignoreWords.begin();
    for ( ; it != ignoreWords.end(); it++) {
        wordList.erase(*it);
    }
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
	
	printf ("%s=\"%s\"\n", setting.c_str(), value.c_str());
        fscanf (fdSettings, "%s", line);
    }
    printf ("%s\n", "done");
    return 0;
}

/**********************************************/

int loadStopWords () {
    FILE* fdStopWords;
    char word[20];
    printf ("%s", "loading stop words...");
    fdStopWords = fopen ("stopWords.lst", "r");
    if (fdStopWords == NULL) {
	printf ("%s\n", "error");
	return 1;
    }
    
    fscanf(fdStopWords, "%s", word);
    while (feof(fdStopWords) == 0) {
	stopWords.insert ( string (word) );
        fscanf (fdStopWords, "%s", word);
    }
    printf ("%d...", stopWords.size());
    printf ("%s\n", "done");
    fclose (fdStopWords);
    
    return 0;
}

/**********************************************/

int loadIgnoreWords () {
    FILE* fdIgnoreWords;
    char word[20];
    printf ("%s", "loading ignore words...");
    fdIgnoreWords = fopen ("ignoreWords.lst", "r");
    if (fdIgnoreWords == NULL) {
	printf ("%s\n", "error");
	return 1;
    }
    
    fscanf(fdIgnoreWords, "%s", word);
    while (feof(fdIgnoreWords) == 0) {
	ignoreWords.insert ( string (word) );
        fscanf (fdIgnoreWords, "%s", word);
    }
    printf ("%d...", ignoreWords.size());
    printf ("%s\n", "done");
    fclose (fdIgnoreWords);
    
    return 0;
}

/**********************************************/

int cycleArticleDir (const char* dir) {
    DIR* reuters = opendir(dir);
    dirent* entry = readdir(reuters); //ignore .
    entry = readdir(reuters); //ignore ..
    entry = readdir(reuters);
    while(entry != NULL) {
	char* fileName = entry->d_name;
	printf("%s...", fileName);
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
    
    /*******This section is not needed due to the way parseArticle()
	    finds a word: str passed by parseArticle() will only
	    contain alpha characters
	    
    //break up hyphonated sets of words, consider them
    //independent of eachother and call addWord on each
    int pos = str.find('-');
    while(pos > 0) {
	string tmp = str.substr (0,pos);
	str.replace(0, pos+1, "");
	addWord(article, tmp, weight);
	addWord(article, str, weight);
	pos = str.find('-');
    }
    //do the same with underscored words
    pos = str.find('_');
    while(pos > 0) {
	string tmp = str.substr (0,pos);
	str.replace(0, pos+1, "");
	addWord(article, tmp, weight);
	addWord(article, str, weight);
	pos = str.find('-');
    }

    //remove non alpha characters from front of word
    while (isalpha (str[0]) == 0 && str.size() > 0) {
	str.replace(0, 1, "");
    }
    //remove non alpha characters from end of word
    while (isalpha (str[str.size()-1]) == 0 && str.size() > 0) {
	str.replace(str.size()-1, 1, "");
    }
    //if is a contraction (') remove the '
    //if non alpha character remains, ignore the word
    int i;
    for (i==0; i<str.size(); i++) {
	if (str[i] == '\'') {
	    str.replace(i, 1, "");
	    i--;
	}
	if (isalpha (str[i]) != 0) {
	    return;
	}
    }
    ********/
    
    int i;
    //if the word dones not contain a vowel, ignore it
    if (!containsAVowel (str)) {
	return;
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
    if (it == stopWords.end())
	return;
    it = ignoreWords.find(strCopy);
    if (it == stopWords.end())
	return;
    it = ignoreWords.find(str);
    if (it == stopWords.end())
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

//in the version below(now in "preProcOldCode.cpp"),
//parseArticle() attempts to
//read whitespace seperated chunks of characters and
//step its way through each article while looking
//for tags (eg. <TOPICS>).
//
//in the new version, parseArticle(), i read a large
//block of the file as a char[], i convert the char[]
//to a string and then extract usefull information from
//it by finding the tags that i care about
//
//**known error: some TOPICS are parsed incorrectly

int parseArticle (const char* fileName) {
    FILE* file;
    int bytesRead, pos1, pos2, size;
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
	pos2 = strBlock.find (openTags[2], pos1);
	if(pos2 > pos1) {
	    strSegment = strBlock.substr (pos1, pos2-pos1);
	    pos1 = strSegment.find (openTags[13]);
	    while (pos1 >= 0) {
		pos1 = strSegment.find (">", pos1);
		if (pos1 < 0) {
		    cout <<"error reading TOPICS in article: " <<tmp <<'\n';
		    break;
		}
		pos1++;
		//pos1 += openTags[13].size();
		pos2 = strSegment.find (closeTags[13], pos1);
		strTmp = strSegment.substr (pos1, pos2-pos1);
		pos2 += closeTags[13].size();
		strSegment.replace (0, pos2, "");
		article.addLabel (strTmp);
		topicsList.insert (strTmp);
		wordList[strTmp]++;
		
		pos1 = strSegment.find (openTags[13], pos2);
		//cout <<pos1 <<'\n';
	    }
	}
	pos2 += closeTags[13].size() + closeTags[2].size();
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
	}
	int i = pos1+openTags[12].size(), start, end;
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

string removePrefixOrSuffix (string word) {
    int i,pos;
    for (i=0; i<prefixSize; i++) {
	pos = word.find (prefixes[i]);
	if (pos == 0) {
	    word =  word.substr (prefixes[i].size());
	}
    }
    for (i=0; i<suffixSize; i++) {
	pos = word.find (suffixes[i]);
	if (pos == word.size()-suffixes[i].size()) {
	    word =  word.substr (0,word.size()-suffixes[i].size());
	}
    }
    return word;
}

/**********************************************/

