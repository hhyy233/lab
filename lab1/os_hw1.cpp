#include <iostream> 
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <map>

using namespace std;

typedef struct TOKEN {
	string str;
	int line;
	int offset;
} Token;

typedef struct PAIR {
	string symbol;
	int value;
} Pair;

typedef struct INSTRUCT {
	char addr;
	int value;
} Instruct;

typedef struct INFO {
	int value;
	int module;
	bool is_use; // if this value has been used
	bool mt_def; // if this value is mutiple time defined
} Info;

queue<Token> Tq;
vector<int> moduleAddr;

vector< vector<Pair> > DefList;
vector< vector<string> > UseList;
vector< vector<Instruct> > ProgList;

const string _EOF_ = "_EOF_";
const int MAX_INS = 512;

//
void printParseError(Token tt, int errcode) {
	static char* errstr[] = {
		"NUM_EXPECTED", // Number expect
		"SYM_EXPECTED", // Symbol Expected
		"ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
		"SYM_TOO_LONG", // Symbol Name is too long
		"TOO_MANY_DEF_IN_MODULE", // > 16
		"TOO_MANY_USE_IN_MODULE", // > 16
		"TOO_MANY_INSTR", // total num_instr exceeds memory size (512)
	};
	printf("Parse Error line %d offset %d: %s\n", tt.line, tt.offset, errstr[errcode]);
	throw -1;
}


void processFile(ifstream& infile) {
	int num_line = 0;
	int len_LastLine;
	string str = "";
	while(getline(infile, str)){
		++num_line;
		int len = str.size();
		for (int ii = 0; ii < len; ii++) {
			if (str[ii] == ' ' || str[ii] == '\t' || str[ii] == '\n') continue;
			else {
				int jj = ii;
				while (str[jj] != ' ' && str[jj] != '\t' && str[ii] != '\n' && jj < len) jj++;
				string ss = str.substr(ii, jj - ii);
				//Error_check
				Token tt;
				tt.str = ss;
				tt.line = num_line;
				tt.offset = ii + 1;
				Tq.push(tt);
				ii = jj; // be careful
			}
		}
		len_LastLine = len;
	}
	Token tt;
	tt.str = _EOF_;
	tt.line = num_line;
	tt.offset = len_LastLine + 1;

	Tq.push(tt);
}

int convValue(string si){
	int len = si.size();
	int sum = 0;
for (int ii = 0; ii < len; ii++) {
	char cc = si[ii];
	sum = sum * 10 + (cc - '0');
}
return sum;
}

bool isSym(string ss) {
	if ((ss == _EOF_) || (!((ss[0] >= 'A'&&ss[0] <= 'Z') || (ss[0] >= 'a'&&ss[0] <= 'z')))) return false;
	for (int ii = 1; ii < ss.size(); ii++) {
		if ((ss[ii] >= 'A'&&ss[ii] <= 'Z') || (ss[ii] >= 'a'&&ss[ii] <= 'z') || (ss[ii] >= '0'&&ss[ii] <= '9')) continue;
		else return false;
	}
	return true;
}

bool isNum(string ss) {
	if (ss == _EOF_) return false;
	for (int ii = 0; ii < ss.size(); ii++) {
		if (ss[ii] >= '0'&& ss[ii] <= '9') continue;
		else return false;
	}
	return true;
}

bool isAddr(string ss) {
	if (ss.size() > 1) return false;
	else if (!(ss[0] == 'A' || ss[0] == 'R' || ss[0] == 'I' || ss[0] == 'E')) return false;

	return true;
}

void parseFile() {

	//test
	if (false) {
	while(!Tq.empty()){
	 	Token tt = Tq.front();
	 	Tq.pop();
	 	printf("%s %d %d\n", tt.str.c_str(), tt.line, tt.offset);
		}
		cin.get();
		return;
	}
	//test

	int num_instr = 0;//modual address

	while (!Tq.empty()) {
		moduleAddr.push_back(num_instr);

		//warning detect
		if (!DefList.empty()) {
			int ii = DefList.size() - 1;
			int jj = DefList[ii].size();
			int mod_addr = moduleAddr[ii + 1];
			int mod_addr_last = moduleAddr[ii];
			int mod_len = mod_addr - mod_addr_last - 1;
			for (int kk = 0; kk < jj; kk++) {
				Pair pp = DefList[ii][kk];
				if (pp.value > mod_addr) {
					printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n", ii + 1, pp.symbol.c_str(), pp.value-mod_addr_last, mod_len);
					DefList[ii][kk].value = mod_addr_last;
				}
			}
		}
		//


		Token cc;
		// definition list
		vector<PAIR> P;
		cc = Tq.front();
		Tq.pop();
		if (cc.str == _EOF_) break;
		else if (!isNum(cc.str)) printParseError(cc, 0);
		int defcount = convValue(cc.str);
		if(defcount > 16) printParseError(cc, 4);

		for (int ii = 0; ii < defcount; ii++) {
			//Error_check
			Token tt = Tq.front();
			Tq.pop();
			if (!isSym(tt.str)) printParseError(tt, 1);
			else if (tt.str.size() > 16) printParseError(tt, 3);
			Token tn = Tq.front();
			Tq.pop();
			if (!isNum(tn.str)) printParseError(tn, 0);

			Pair p;
			p.symbol = tt.str;
			p.value = convValue(tn.str) + moduleAddr.back();
			P.push_back(p);
		}
		// cin.get();
		DefList.push_back(P);

		//use list
		vector<string> UL;
		cc = Tq.front();
		Tq.pop();
		if (!isNum(cc.str))	printParseError(cc, 0);
		int usecount = convValue(cc.str);
		if (usecount > 16) printParseError(cc, 5);

		for (int ii = 0; ii < usecount; ii++) {
			Token tt = Tq.front();
			Tq.pop();
			//Error_check
			if (!isSym(tt.str))	printParseError(tt, 1);
			else if (tt.str.size() > 16) printParseError(tt, 3);

			UL.push_back(tt.str);
		}

		UseList.push_back(UL);

		//program text
		vector<Instruct> IL;
		cc = Tq.front();
		Tq.pop();
		if (!isNum(cc.str))	printParseError(cc, 0);
		int codecount = convValue(cc.str);
		num_instr += codecount;
		if (num_instr > MAX_INS) printParseError(cc, 6);

		for (int ii = 0; ii < codecount; ii++) {
			//Error_check
			Token tt = Tq.front();
			Tq.pop();
			if (!isAddr(tt.str)) printParseError(tt, 2);

			Token tn = Tq.front();
			Tq.pop();
			if (!isNum(tn.str))	printParseError(tn, 0);

			Instruct it;
			it.addr = tt.str[0]; //Error_check
			it.value = convValue(tn.str);

			IL.push_back(it);
		}

		ProgList.push_back(IL);

	}

}

void printReseult() {

	map<string, Info> mapSymbol;
	vector<string> table;

	//symbol table
	for (int ii = 0; ii < DefList.size(); ii++) {
		for (int jj = 0; jj < DefList[ii].size(); ++jj){
			string ss = DefList[ii][jj].symbol;
			int vv = DefList[ii][jj].value;
			//Error_check
			if (mapSymbol.find(ss) == mapSymbol.end()) {
				Info inn = { vv, ii + 1, false, false };
				mapSymbol[ss] = inn;
				table.push_back(ss);
			}
			else {
				mapSymbol[ss].mt_def = true;
				DefList[ii].erase(DefList[ii].begin() + jj);
				jj--;
			}
		}
	}

	cout << "Symbol Table" << endl;
	for (int ii = 0; ii < table.size(); ii++) {
		string ss = table[ii];
		cout << ss << '=' << mapSymbol[ss].value;
		if (mapSymbol[ss].mt_def) cout << " Error: This variable is multiple times defined; first value used";
		cout << endl;
	}
	cout << endl;

	//memory map
	cout << "Memory Map" << endl;
	for (int ii = 0; ii < ProgList.size(); ii++) {

		int len = UseList[ii].size();
		vector<bool> ifisuse;
		for (int jj = 0; jj < len; jj++) {
			ifisuse.push_back(false);
		}


		for (int jj = 0; jj < ProgList[ii].size(); jj++) {
			char aa = ProgList[ii][jj].addr;
			int vv = ProgList[ii][jj].value;
			//Error_check
			printf("%03d: ", moduleAddr[ii] + jj);
			if (aa == 'I') {
				if (vv / 10000) printf("9999 Error: Illegal immediate value; treated as 9999");
				else printf("%04d",vv);
			}
			else if (vv / 10000) {
				printf("9999 Error: Illegal opcode; treated as 9999");
			}
			else if (aa == 'A') {
				if (vv % 1000 > MAX_INS) printf("%d000 Error: Absolute address exceeds machine size; zero used", (vv / 1000));
				else printf("%04d", vv);;
			}
			else if (aa == 'R') {
				int len = moduleAddr[ii + 1] - moduleAddr[ii];
				if (vv % 1000 > len) printf("%d%03d Error: Relative address exceeds module size; zero used", vv / 1000, moduleAddr[ii]);
				else printf("%04d", (vv + moduleAddr[ii]));
			}
			else { // 'E'
				int kk = vv % 1000;
				if (kk > UseList[ii].size()-1) {
					printf("%04d", vv);
					printf(" Error: External address exceeds length of uselist; treated as immediate");
				}
				else {
					ifisuse[kk] = true; //this symbol in uselist has been used
					string ss = UseList[ii][kk];
					if (mapSymbol.find(ss) != mapSymbol.end()) {
						//mapSymbol[ss].is_use = true;
						//cout << ((vv/1000)*1000 + mapSymbol[ss].value);
						printf("%04d", ((vv / 1000) * 1000 + mapSymbol[ss].value));
						mapSymbol[ss].is_use = true;
					}
					else {
						printf("%04d", (vv / 1000) * 1000);
						printf(" Error: %s is not defined; zero used", ss.c_str());
					}
				}
				
			}
			cout << endl;
		}
		// ifisuse
		for (int kk = 0; kk < UseList[ii].size(); kk++) {
			string ss = UseList[ii][kk];
			if (!ifisuse[kk]) {
				printf("Warning: Module %d: %s appeared in the uselist but was not actually used\n", ii + 1, ss.c_str());
			}
		}
		//
	}

	//warning
	for (int ii = 0; ii < DefList.size(); ii++) {
		for (int jj = 0; jj < DefList[ii].size(); jj++) {
			string ss = DefList[ii][jj].symbol;
			Info inn = mapSymbol[ss];
			if (inn.is_use == false)
				printf("\nWarning: Module %d: %s was defined but never used", inn.module, ss.c_str());
		}
	}
	cout << endl;

}

int main(int argc, char* argv[]) {
	ifstream infile;
	infile.open(argv[1]);
	if(!infile) {
		cout<<"file does not exite!!"<<endl;
		return -1;
	}
	
	try{
		processFile(infile);
		parseFile();
	}
	catch(int e){
		return e;
	}

	printReseult();
	infile.close();

	return 0;
}