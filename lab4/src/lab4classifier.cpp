//Derek Boytim
//CSE 674
//Lab 4
//3-3-2012
//lab4classifier.cpp

#include <cstdlib>
#include <set>
#include <fstream>
#include <string>
#include <vector>

#include "listIO.h"
#include "featureVector.h"
#include "rule.h"

using namespace std;

string settings[] = {"classifyBegin",       //0
                     "classifyEnd",         //1
                     "skipLabeledArticles", //2
                     "fileToClassify"};     //3

int classifyBegin = 10789,
    classifyEnd = 0;
bool skipLabeledArticles=true;
string classifyFile="testingData", def;

void classifyVector (Vector1&, vector<Rule>&, bool);
int loadSettings ();
void printRules (vector<Rule>&);

int main(int argc, char** argv) {
    loadSettings();
    set<string> wordList, labelList;
    vector<Rule> rules;
    Vector1 vec;
    time_t seconds = time (NULL);
    loadWordList (wordList);
    loadLabelList (labelList);
    loadRules (rules, labelList, wordList, def);
    sortRules (rules);
    readVector1 (vec, classifyFile);
    classifyVector (vec, rules, skipLabeledArticles);
    cout <<"time: " <<time(NULL)-seconds <<" seconds\n";
    vec.write(classifyFile);
    return 0;
}

void classifyVector (Vector1& vec, vector<Rule>& rules, bool skip) {
    cout <<"classifying " <<classifyEnd-classifyBegin <<" at article " <<classifyBegin <<"...";
    int i,j,k, failed=0, ignored=0, success=0;
    if (classifyBegin > vec.entries.size()) {
        return;
    }
    if (classifyEnd > vec.entries.size() || classifyEnd == 0) {
        classifyEnd = vec.entries.size();
    }
    for (i=classifyBegin; i<classifyEnd; i++) {
        V1Entry* entry = &vec.entries[i];
        if ((*entry).labels.size() > 0 && skip) {
            ignored++;
            continue;
        }
        bool ruleFits;
        for (j=0; j<rules.size(); j++) {
            ruleFits = true;
            Rule rule = rules[j];
            set<string>::iterator it = rule.antecedent.begin();
            for ( ; it != rule.antecedent.end(); it++) {
                for (k=0; k<(*entry).words.size(); k++) {
                    if ((*entry).words[k] == (*it)) {
                        break;
                    }
                }
                if (k == (*entry).words.size()) {
                    ruleFits = false;
                    break;
                }
            }
            if (ruleFits) {
                success++;
                (*entry).labels.push_back (rule.consequent);
                break;
            }
        }
        if (!ruleFits) {
            failed++;
            (*entry).labels.push_back (def);
        }
    }
    double size = classifyEnd-classifyBegin;
    cout <<"done\n";
    cout <<"default rule -> " <<def <<'\n';
    cout <<(ignored/size)*100 <<"% of articles were ignored(" <<ignored <<")\n";
    cout <<(failed/(size-ignored))*100 <<"% of articles were given the default rule(" <<failed <<")\n";
    cout <<(success/(size-ignored))*100 <<"% of articles were successfully classified(" <<success <<")\n";
}

int loadSettings () {
    printf ("%s\n", "loading settings...");
    FILE* fdSettings;
    int pos1, pos2;
    char line[40];
    string strLine, setting, value;

    fdSettings = fopen ("lab4ClassifierSettings.cfg", "r");
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
	    classifyBegin = atoi (value.c_str());
	else if (setting.compare (settings[1]) == 0)
	    classifyEnd = atoi (value.c_str());
	else if (setting.compare (settings[2]) == 0) {
	    if (atoi (value.c_str()) == 0) {
                skipLabeledArticles = false;
            }
        }
        else if (setting.compare (settings[3]) == 0)
	    classifyFile = value;
	else
	    cout <<"ERROR, unknown setting: ";
	
	printf ("%s=\"%s\"\n", setting.c_str(), value.c_str());
        fscanf (fdSettings, "%s", line);
    }
    fclose (fdSettings);
    printf ("%s\n", "done loading settings");
    return 0;
}

void printRules (vector<Rule>& rules) {
    int i;
    for (i=0; i<rules.size(); i++) {
        cout <<rules[i].toString().c_str() <<'\n';
    }
}

