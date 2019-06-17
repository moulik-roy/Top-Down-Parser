#include <bits/stdc++.h>
using namespace std;

extern FILE* yyin;
extern int yylex();
extern char* yytext;

set <string> terminals;
vector <vector<string>> productions, LL1_parsing_table;
vector <string> input_tokens;
map <string, set<string>> FIRST, FOLLOW;
map <string, int> terminals_index, non_terminals_index;

void getTerminals(){
	ifstream fin;
	string s;
	fin.open("Terminals.txt", ios::in);
	if(!fin){
		cout<<"Error while opening the Terminals.txt file !! Exiting ...\n";
		exit(0);
	}
	while(fin>>s)
		terminals.insert(s);
	fin.close();
}

void getProductions(){
	ifstream fin;
	string s, token;
	vector <string> tokens;
	fin.open("CFGLeftRecursionRemoved.txt", ios::in);
	if(!fin){
		cout<<"Error while opening the CFGLeftRecursionRemoved.txt !! Exiting ...\n";
		exit(0);
	}
	while(fin.good()){
		getline(fin, s);
		stringstream ss(s);
		while(ss>>token)
			tokens.push_back(token);
		productions.push_back(tokens);
		tokens.clear();
	}
}

set<string> first(string s){
	set <string> first_set;
	if(terminals.find(s)!=terminals.end()){			//if 's' is a terminal then, insert s in the FIRST(s) and return it
		first_set.insert(s);
		return first_set;
	}
	for(int i=0; i<productions.size(); i++){
		if(productions[i][0].compare(s)==0){		//find the production with 's' as the LHS non-terminal
			if(productions[i][2]=="^")				// if X -> ^ then insert ^ in the FIRST(s)
				first_set.insert("^");
			else{
				for(int j=2; j<productions[i].size(); j++){
					bool foundEpsilon=false;
					set <string> temp=first(productions[i][j]);
					for(set<string>::iterator it=temp.begin(); it!=temp.end(); it++){
						first_set.insert(*it);
						if((*it).compare("^")==0)
							foundEpsilon=true;
					}
					if(!foundEpsilon)
						break;
				}
			}
		}
	}
	return first_set;
}

set<string> follow(string s){
	set <string> follow_set, temp;
	if(s.compare(productions[0][0])==0)				//if 's' is the start symbol insert '$' in the FOLLOW(s);
		follow_set.insert("$");
	for(int i=0; i<productions.size(); i++){
		for(int j=2; j<productions[i].size(); j++){
			if(productions[i][j].compare(s)==0){
				if(j<productions[i].size()-1){
					temp=FIRST[productions[i][j+1]];
					bool foundEpsilon=false;
					for(set<string>::iterator it=temp.begin(); it!=temp.end(); it++){
						if((*it).compare("^")==0)
							foundEpsilon=true;
						else
							follow_set.insert(*it);
					}
					if(foundEpsilon && s.compare(productions[i][0])!=0){
						temp=follow(productions[i][0]);
						for(set<string>::iterator it=temp.begin(); it!=temp.end(); it++)
							follow_set.insert(*it);
					}
				}
				else if(j==productions[i].size()-1 && s.compare(productions[i][0])!=0){
					temp=follow(productions[i][0]);
					for(set<string>::iterator it=temp.begin(); it!=temp.end(); it++)
						follow_set.insert(*it);
				}
			}
		}
	}
	return follow_set;
}

void FIRSTandFOLLOW(){
	set <string> temp;
	for(int i=0; i<productions.size(); i++){
		if(FIRST.find(productions[i][0])==FIRST.end())
			FIRST[productions[i][0]]=first(productions[i][0]);
	}
	for(set<string>::iterator it=terminals.begin(); it!=terminals.end(); it++){
		temp.insert(*it);
		FIRST[*it]=temp;
		temp.clear();
	}
	temp.insert("^");
	FIRST["^"]=temp;
	temp.clear();

	ofstream fout;
	fout.open("FIRST.txt", ios::out);
	fout<<"FIRST OF TERMINAL SYMBOLS\n";
	for(set<string>::iterator it=terminals.begin(); it!=terminals.end(); it++)
		fout<<"FIRST("<<(*it)<<") : "<<"[ "<<(*it)<<" ]\n";
	
	fout<<"\nFIRST OF NON-TERMINAL SYMBOLS\n";
	for(map<string, set<string>>::iterator it=FIRST.begin(); it!=FIRST.end(); it++){
		if(terminals.find(it->first)==terminals.end() && it->first.compare("^")!=0){
			fout<<"FIRST("<<(it->first)<<") : [ ";
			temp=it->second;
			for(set<string>::iterator i=temp.begin(); i!=temp.end(); i++)
				fout<<(*i)<<" ";
			fout<<" ]\n";
		}
	}
	fout.close();
	cout<<"FIRST of all terminals and non-terminals created in FIRST.txt\n";

	for(int i=0; i<productions.size(); i++){
		if(FOLLOW.find(productions[i][0])==FOLLOW.end())
			FOLLOW[productions[i][0]]=follow(productions[i][0]);
	}

	fout.open("FOLLOW.txt", ios::out);
	fout<<"FOLLOW OF ALL NON-TERMINAL SYMBOLS\n";
	for(map<string, set<string>>::iterator it=FOLLOW.begin(); it!=FOLLOW.end(); it++){
		fout<<"FOLLOW("<<it->first<<") : [ ";
		temp=it->second;
		for(set<string>::iterator i=temp.begin(); i!=temp.end(); i++)
			fout<<(*i)<<" ";
		fout<<" ]\n";
	}
	fout.close();
	cout<<"FOLLOW of all non-terminals created in FOLLOW.txt\n";
}

void createSymbolsMap(){
	int i, index=1;
	for(set<string>::iterator it=terminals.begin(); it!=terminals.end(); it++)
		terminals_index[(*it)]=index++;
	terminals_index["$"]=index;
	for(i=0, index=1; i<productions.size(); i++){
		if(non_terminals_index.find(productions[i][0])==non_terminals_index.end())
			non_terminals_index[productions[i][0]]=index++;
	}
}

void createLL1ParsingTable(){
	vector <vector<string>> parsing_table(non_terminals_index.size()+1, vector<string>(terminals_index.size()+1, "_"));
	string production_rule;
	bool foundEpsilon;
	set <string> first_set, temp;
	for(map<string, int>::iterator it=terminals_index.begin(); it!=terminals_index.end(); it++)
		parsing_table[0][it->second]=it->first;
	for(map<string, int>::iterator it=non_terminals_index.begin(); it!=non_terminals_index.end(); it++){
		parsing_table[it->second][0]=it->first;
		for(int i=0; i<productions.size(); i++){
			if(productions[i][0].compare(it->first)==0){
				production_rule="";		foundEpsilon=true;		first_set.clear();
				for(int j=2; j<productions[i].size(); j++){
					production_rule+=productions[i][j]+" ";
					if(foundEpsilon){
						foundEpsilon=false;
						temp=FIRST[productions[i][j]];
						for(set<string>::iterator t=temp.begin(); t!=temp.end(); t++){
							if((*t).compare("^")==0)
								foundEpsilon=true;
							else
								first_set.insert(*t);
						}
					}
				}
				for(set<string>::iterator t=first_set.begin(); t!=first_set.end(); t++){
					if(terminals_index.find(*t)!=terminals_index.end())
						parsing_table[it->second][terminals_index[*t]]=production_rule;
				}
				if(foundEpsilon){
					temp=FOLLOW[productions[i][0]];
					for(set<string>::iterator t=temp.begin(); t!=temp.end(); t++){
						if(terminals_index.find(*t)!=terminals_index.end())
							parsing_table[it->second][terminals_index[*t]]=production_rule;
					}
				}
			}
		}
	}

	ofstream fout;
	fout.open("Parsing_Table.txt", ios::out);
	fout<<"LL(I) PARSING TABLE\n\n";
	for(int i=0; i<parsing_table.size(); i++){
		for(int j=0; j<parsing_table[i].size(); j++)
			fout<<parsing_table[i][j]<<"\t|\t";
		fout<<endl;
	}
	fout.close();
	cout<<"LL(I) Parsing Table created in Parsing_Table.txt\n";
	LL1_parsing_table=parsing_table;
}

void extractTokens(){
	int token;
	yyin=fopen("input.txt", "r");
	token=yylex();
	while(token!=0){
		if(token==1)
			input_tokens.push_back(yytext);
		if(token==2)
			input_tokens.push_back("id");
		else if(token==3)
			input_tokens.push_back("integer");
		else if(token==4)
			input_tokens.push_back("fraction");
		token=yylex();
	}
	input_tokens.push_back("$");
}

void parse(){
	stack <string> st, temp;
	string production_rule, token;
	st.push("$");
	st.push(productions[0][0]);

	ofstream fout;
	fout.open("Parsing_Action.txt", ios::out);
	fout<<"PARSING ACTION\n\n";
	fout<<"STACK \t\t\t\t INPUT TOKEN TO BE PARSED\n";
	int index=0;
	while(!st.empty() && index<input_tokens.size()){
		while(!st.empty()){
			temp.push(st.top());
			st.pop();
		}
		while(!temp.empty()){
			fout<<temp.top()<<" ";
			st.push(temp.top());
			temp.pop();
		}
		fout<<"\t\t"<<input_tokens[index]<<"\n";
		if(st.top().compare("$")==0 && input_tokens[index].compare("$")==0){
			fout<<"\nInput file parsed successfully and the input is accepted !\n";
			cout<<"Input Accepted !!\n";
			break;
		}
		if(st.top().compare("^")==0){
			st.pop();
			continue;
		}
		if(non_terminals_index.find(st.top())!=non_terminals_index.end()){
			production_rule=LL1_parsing_table[non_terminals_index[st.top()]][terminals_index[input_tokens[index]]];
			if(production_rule.compare("_")==0){
				fout<<"\nInvalid Input !! Aborting parsing procedure ...\n";
				cout<<"Invalid Input !! Exiting ...\n";
				break;
			}
			st.pop();
			stringstream ss(production_rule);
			while(ss>>token)
				temp.push(token);
			while(!temp.empty()){
				st.push(temp.top());
				temp.pop();
			}
		}
		else{
			if(st.top().compare(input_tokens[index])==0){
				st.pop();
				index++;
			}
			else{
				fout<<"\nInvalid Input !! Aborting parsing procedure ...\n";
				cout<<"Invalid Input !! Exiting ...\n";
				break;
			}
		}
	}
	fout.close();
}

int main(){
	getTerminals();
	getProductions();
	FIRSTandFOLLOW();
	createSymbolsMap();
	createLL1ParsingTable();
	extractTokens();
	parse();
	return 0;
}