#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#define PRODUCT_FILE_NAME "./products.txt"//D:/Documents/Visual Studio 2015/Projects/Sortable_Challange/Debug
#define LISTING_FILE_NAME "./listings.txt"
#define RESULT_FILE_NAME "./result.txt"
#define DEBUG 0
using namespace std;
class matching {
	std::multimap<string, std::map<string, string> > product;
	std::multimap<string, std::map<string, string> > listing;
	pair<string, map<string, string>> process(string line) {
		pair<string, std::map<string, string> > attributes_;
		map<string, string> attributes;
		//string word = "";
		//string keyword;
		//bool time = false;
		//int index = 0;
		string data = "";
		data = line.substr(1, line.length() - 1);
		int comma_pos = 0;
		int colon_pos = 0;
		while (data.find(",") != string::npos) {
			//cout << data.find(",") << endl;
			comma_pos = data.find(',');
			colon_pos = data.find(':');
			while (data[comma_pos - 1] != '"' || data[comma_pos + 1] != '"') { //Assuming no spaces are allowed in between
				while (data[comma_pos - 1] != '"')
					comma_pos = data.find(',', comma_pos + 1);
				while (data[comma_pos + 1] != '"')
					comma_pos = data.find(',', comma_pos + 1);
			}
			while (data[colon_pos - 1] != '"' || data[colon_pos + 1] != '"') {
				while (data[colon_pos - 1] != '"')
					colon_pos = data.find(':', colon_pos + 1);
				while (data[colon_pos + 1] != '"')
					colon_pos = data.find(':', colon_pos + 1);
			}
			if (data.substr(data.find(":") + 2, comma_pos - data.find(":") - 3).empty())   //handle empty field
				attributes[data.substr(1, data.find(":") - 2)] = "";
			else
				attributes[data.substr(1, data.find(":") - 2)] = data.substr(data.find(":") + 2, comma_pos - data.find(":") - 3);
			//cout<<data.substr(1,data.find(":")-2) << <<endl;
			data = data.substr(comma_pos + 1, data.length());
		}
		attributes[data.substr(1, data.find(":") - 2)] = data.substr(data.find(":") + 2, data.length() - data.find(":") - 4);
		//cout<<data<<endl;
		//string manufacturer = attributes["manufacturer"];
		attributes_ = (pair<string, map<string, string>>(attributes["manufacturer"], attributes));
		return attributes_;
	}
	multimap<string, map<string, string>> find_duplicates(std::multimap<string, std::map<string, string> > product, std::multimap<string, std::map<string, string> > listing) {
		multimap<string, map<string, string> > result;
		int prev = 0;
		for (auto &name : product) {
			std::multimap<string, std::map<string, string> >::iterator it = listing.find(name.first);
			string value = name.second["product_name"];		//product name with underscores replaced with space and uppercase
			string model = name.second["model"];			//Model name uppercase
			std::transform(value.begin(), value.end(), value.begin(), ::toupper);
			string name_ = value;							//product name with uppercase
			string name__ = value;							//product name with uppercase and '-' replaced with space
			std::replace(value.begin(), value.end(), '_', ' ');
			std::replace(name__.begin(), name__.end(), '-', ' ');
			std::transform(model.begin(), model.end(), model.begin(), ::toupper);
			string name_val = name__;						//product name with uppercase and '-' and '_' replaced with space
			std::replace(name_val.begin(), name_val.end(), '_', ' ');
			while (it != listing.end() && it->first == name.first) {
				map<string, string> map_ = (it->second);
				//map_["title"];
				string val = (it->second)["title"];
				std::transform(val.begin(), val.end(), val.begin(), ::toupper);
				if (val.find(model) != string::npos || val.find(name_val.substr(name.first.length() + 1)) != string::npos		//model check with and without manufacturer 
							|| val.find(name_val) != string::npos || val.find(name__) != string::npos ||								//product names with above mentioned constraints 
							val.find(value.substr(name.first.length() + 1)) != string::npos || val.find(name_) != string::npos || val.find(value) != string::npos)
					result.insert(pair<string, map<string, string>>(name.second["product_name"], it->second));
				//it++;
				//if (val.find("SLT-A33") != string::npos)
				//      for (auto &i : it->second)
				//              cout<<"Here we go "<<i.first<<" :" <<i.second<<endl;
				it++;

			}
			it = listing.find("");                  //for empty fields in input
			while (it != listing.end() && it->first == "") {
				string val = it->second["title"];
				std::transform(val.begin(), val.end(), val.begin(), ::toupper);
				if (val.find(name_) != string::npos || val.find(value) != string::npos)
					result.insert(pair<string, map<string, string>>(name.second["product_name"], it->second));
				it++;
			}
			if (result.size() == prev && DEBUG)
				cout << value << endl;
			prev = result.size();
		}
		return result;
	}
	void read_data(ifstream& file_des, bool is_product = true) {
		string line;
		//file_des.open(LISTING_FILE_NAME);
		if (file_des.is_open())
		{
			std::stringstream buffer;
			buffer << file_des.rdbuf();
			while (getline(buffer, line))
			{
				if (is_product)
					product.insert(process(line));
				else
					listing.insert(process(line));
			}
			if (DEBUG) {		//For debugging
				//	Output second product
				multimap<string, std::map<string, string> >::iterator it = product.begin();
				it++;
				for (auto &i : it->second) {
					cout << it->first << " " << i.first << " " << i.second << " " << product.count(it->first) << endl;
				}
				ofstream result_file_des;
				//	Output all the products that are read into memory
				if (is_product)
					result_file_des.open("./products_read.txt");
				else
					result_file_des.open("./listing_read.txt");
				for (multimap<string, map<string, string> >::iterator each_product = product.begin(); each_product != product.end(); each_product++) {
					string data = "{";
					for (auto &each_listing : each_product->second) {
						data += "\"" + each_listing.first + "\":\"" + each_listing.second + "\", ";
					}
					data = data.substr(0, data.length() - 2);
					result_file_des << "{" << data << "}" << endl;
				}
				result_file_des.close();
			}
			file_des.close();
		}
	}
	void write_results(multimap<string, map<string, string>> result) {
		ofstream result_file_des;
		result_file_des.open(RESULT_FILE_NAME);
		for (multimap<string, map<string, string> >::iterator each_product = result.begin(); each_product != result.end(); each_product++) {
			string data = "[";
			string name = each_product->first;
			while (each_product != result.end() && each_product->first == name) {
				if (data != "[")
					data += ", {";
				else
					data += "{";
				for (auto &each_listing : each_product->second) {
					data += "\"" + each_listing.first + "\":\"" + each_listing.second + "\", ";
				}
				data = data.substr(0, data.length() - 2) + "}";
				each_product++;
			}
			each_product--;
			data += "]";
			result_file_des << "{\"product_name\":\"" << each_product->first << "\",\"listings\":" << data << "}" << endl;
		}
	}
public:
	void find_matches() {
		ifstream product_file_des;
		product_file_des.open(PRODUCT_FILE_NAME);
		ifstream listing_file_des;
		listing_file_des.open(LISTING_FILE_NAME);
		read_data(product_file_des);
		read_data(listing_file_des, false);
		write_results(find_duplicates(product, listing));
		//cout << "Results are captured" << endl;
	}
};
int main() {
	matching m;
	m.find_matches();
	return 0;
}

