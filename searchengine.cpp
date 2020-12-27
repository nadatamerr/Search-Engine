#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <string.h>
#include <cmath>
#include <chrono>
using namespace std;

typedef struct urlsdata {
	int imp{ 0 };
	int click{ 0 };
	double ctr{ 0 };
	int linksto{ 0 };
	double pagerank{ 0 };
	double normPR{ 0 };
	double score{ 0 };
	unordered_set<string> set;
}info;

typedef struct sorted {
	string url;
	double score{ 0 };
} sorted;

unordered_set<string> indexing(vector<vector<string>> qu, unordered_map<string, unordered_set<string>> keywords, long long &ops);
void pageRank(unordered_map<string, info> &urldata, unordered_map<string, unordered_set<string>> graph, long long &ops);
bool sortStruct(sorted lhs, sorted rhs);
void sortDisplay(unordered_map<string, info> &urldata, unordered_set<string> result, long long &ops);

int main() {

	string file1, file2, str1, str2, str = "";
	cout << "Enter <urls filename> <keywords filename>:";
	cin >> file1 >> file2;

	unordered_map<string, info> urldata;
	unordered_map<string, unordered_set<string>> graph;
	unordered_map<string, unordered_set<string>> keywords;

	ifstream f1(file1);
	while (!f1.is_open()) {
		cout << file1 << " not found. Enter valid filename: ";
		cin >> file1;
		f1.open(file1);
	}
	ifstream f2(file2);
	while (!f2.is_open()) {
		cout << file2 << " not found. Enter valid filename: ";
		cin >> file2;
		f2.open(file2);
	}

	while (getline(f2, str)) {
		stringstream ss(str);
		getline(ss, str1, ',');
		while (ss.good()) {
			getline(ss, str2, ',');
			urldata[str1].set.insert(str2);
			keywords[str2].insert(str1);
			str2 = "";
		}
	}
	f2.close();

	str = "";
	str1 = "";
	str2 = "";

	while (getline(f1, str)) {
		stringstream ss(str);
		getline(ss, str1, ',');
		getline(ss, str2, '\n');
		if (str2 != str1 && graph[str2].find(str1) == graph[str2].end()) {
			graph[str2].insert(str1);
			urldata[str1].linksto++;
		}
	}
	f1.close();



	while (true) {

		string mode;
		cout << "\nMode(query, update or exit): ";
		cin >> mode;

		if (mode == "exit") { break; }

		chrono::steady_clock::time_point begin;
		chrono::steady_clock::time_point end;

		if (mode == "query") {
			begin = chrono::steady_clock::now();
			long long ops = 0;

			string kw, t, search;
			cout << "Search for: ";
			getline(cin, search);
			getline(cin, search);
			ops += search.size();

			stringstream ss(search);
			vector<vector<string>> qu;
			int i = 0;
			while (ss.good()) {
				getline(ss, kw, ' ');
				ops += kw.size();
				vector<string> temp;
				if (kw[0] == '"') {
					t = kw.substr(1, kw.size() - 1);
					ops += t.size();
					temp.push_back(t);
					ops++;
					getline(ss, kw, '"');
					ops += kw.size();
					stringstream aa(kw);
					while (aa.good()) {
						string k;
						getline(aa, k, ' ');
						ops += k.size();
						temp.push_back(k);
						ops++;
					}
					qu.push_back(temp);
					ops++;
				}
				else {
					temp.push_back(kw);
					qu.push_back(temp);
					ops += 2;
				}
				i++;
			}

			unordered_set<string> result;

			result = indexing(qu, keywords, ops);
			pageRank(urldata, graph, ops);

			cout << "Results: \n";
			sortDisplay(urldata, result, ops);

			end = chrono::steady_clock::now();

			cout << "\nNumber of operations: " << ops;
			cout << "\nTime elapsed: " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << " microseconds" << endl;
		}
		else if (mode == "update") {
			long long ops = 0;

			begin = chrono::steady_clock::now();
			unordered_map<string, unordered_set<string>>::iterator gr;
			unordered_set<string>::iterator g;
			unordered_set<string>::iterator kr;
			string file3;
			cout << "Enter <update filename>: ";
			cin >> file3;
			ifstream f3(file3);
			while (!f3.is_open()) {
				cout << file3 << " not found. Enter valid filename: ";
				cin >> file3;
				f3.open(file3);
			}
			string up = "", s1 = "", s2 = "", s3 = "", s4 = "", s5 = "", s6 = "";

			while (getline(f3, up)) {
				ops += up.size();
				stringstream ss(up);
				getline(ss, s1, ',');
				ops += s1.size() + 1;
				if (s1 == "A") {
					getline(ss, s2, ',');
					ops += s2.size();
					urldata[s2];
				}
				else {
					ops++;
					if (s1 == "R") {
						getline(ss, s2, ',');
						ops += s2.size();
						for (g = graph[s2].begin(); g != graph[s2].end(); g++) {
							urldata[*g].linksto--; ops++;
						}
						for (gr = graph.begin(); gr != graph.end(); gr++) {
							ops++;
							if ((*gr).second.find(s2) != (*gr).second.end()) {
								(*gr).second.erase(s2);
								ops++;
							}
						}
						for (kr = urldata[s2].set.begin(); kr != urldata[s2].set.end(); kr++) {
							keywords[*kr].erase(s2);
							ops++;
						}
						graph.erase(s2);
						urldata.erase(s2);
						ops += 2;
					}
					else {
						ops++;
						if (s1 == "U") {
							getline(ss, s2, ','); ops += s2.size();
							getline(ss, s3, ','); ops += s3.size();
							urldata[s2].imp = atoi(s3.c_str());
							getline(ss, s4, ','); ops += s4.size();
							urldata[s2].click = atoi(s4.c_str());

							while (ss.good()) {
								getline(ss, s5, ' '); ops += s5.size();
								ops++;
								if (s5 == "A") {
									getline(ss, s6, ' '); ops += s6.size();
									graph[s6].insert(s2); ops++;
									urldata[s2].linksto++; ops++;
								}
								else {
									ops++;
									if (s5 == "R") {
										getline(ss, s6, ' '); ops += s6.size();
										graph[s6].erase(s2); ops++;
										urldata[s2].linksto--; ops++;
									}
								}
							}
						}
					}
				}
			}
			f3.close();
			end = chrono::steady_clock::now();

			cout << "Updates completed.\n";
			cout << "\nNumber of operations: " << ops;
			cout << "\nTime elapsed: " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << " microseconds" << endl;


		}
	}

	return 0;
}

unordered_set<string> indexing(vector<vector<string>> qu, unordered_map<string, unordered_set<string>> keywords, long long &ops) {

	unordered_set<string> result;
	vector<vector<string>>::iterator v;
	unordered_set<string>::iterator k;
	vector<string>::iterator a;

	for (v = qu.begin(); v < qu.end(); v++) {
		ops++;
		if ((*v).size() == 1) {//OR
			ops++;
			if (keywords.find((*v)[0]) != keywords.end()) {
				for (k = keywords[(*v)[0]].begin(); k != keywords[(*v)[0]].end(); k++) {
					if (result.find(*k) == result.end()) result.insert(*k);
					ops += 2;
				}
			}
		}
		else {//AND
			unordered_map<string, int> temp;
			unordered_map<string, int>::iterator t;
			for (a = (*v).begin(); a < (*v).end(); a++) {
				ops++;
				if (keywords.find(*a) != keywords.end()) {
					for (k = keywords[*a].begin(); k != keywords[*a].end(); k++) {
						temp[*k]++;
						ops++;
					}
				}
			}
			for (t = temp.begin(); t != temp.end(); t++) {
				ops++;
				if (t->second == v->size()) {
					if (result.find(t->first) == result.end()) result.insert(t->first);
					ops++;
				}
			}
		}
	}
	return result;

}
void pageRank(unordered_map<string, info> &urldata, unordered_map<string, unordered_set<string>> graph, long long &ops) {

	double val = 1.00 / urldata.size();
	ops++;

	unordered_map<string, unordered_set<string>>::iterator i;
	unordered_set<string>::iterator j;

	for (i = graph.begin(); i != graph.end(); i++) {
		for (j = (*i).second.begin(); j != (*i).second.end(); j++) {
			urldata[i->first].pagerank += (val / urldata[(*j)].linksto);
			ops++;
		}
	}
}

bool sortStruct(sorted lhs, sorted rhs) { return lhs.score > rhs.score; }

void sortDisplay(unordered_map<string, info> &urldata, unordered_set<string> result, long long &ops) {
	unordered_map<string, info>::iterator m;
	double min = 1000000;
	for (m = urldata.begin(); m != urldata.end(); m++) {
		ops += 2;
		if (m->second.pagerank < min && m->second.pagerank != 0) { min = m->second.pagerank + 1; ops++; }
	}
	for (m = urldata.begin(); m != urldata.end(); m++) {
		m->second.normPR = (m->second.pagerank + 1) / min;
		ops += 3;
		if (m->second.click != 0) { m->second.ctr = m->second.click / m->second.imp; ops++; }
		m->second.score = (0.4*m->second.normPR) + (0.6*((m->second.normPR*(1 - ((0.1*m->second.imp) / (1 + 0.1*m->second.imp))))
			+ (m->second.ctr*((0.1*m->second.imp) / (1 + 0.1*m->second.imp)))));
		ops += 15;
	}

	sorted *sorts = new sorted[result.size()];
	unordered_set<string>::iterator r;
	r = result.begin();
	for (int i = 0; i != result.size(); i++) {
		sorts[i].url = (*r);
		sorts[i].score = urldata[*r].score;
		r++;
	}

	sort(sorts, sorts + result.size(), sortStruct);
	ops += result.size()*log(result.size());

	for (int i = 0; i != result.size(); i++) {
		cout << sorts[i].url << endl;
	}
}
