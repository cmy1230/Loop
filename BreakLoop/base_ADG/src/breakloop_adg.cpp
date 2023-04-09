#include<iostream>
#include<string>
#include<fstream>
#include"breakloop.h"
#include"element.h"

using namespace std;

int getnumber(const string& buf) {
	int start;
	int num = 0;
	for (start = buf.length() - 2; start >= 0 && buf.at(start) != ' '; start--);
	for (++start; start <= buf.length() - 2; start++)
		num = num * 10 + buf.at(start) - '0';
	return num;
}
//获得语句中所需数字

string gettype_name(const string& buf) {
	int start;
	string type = "";
	for (start = buf.length() - 3; start >= 0 && buf.at(start) != '\"'; start--);
	for (++start; start <= buf.length() - 3; start++)
		type += buf.at(start);
	return type;
}
//获得语句中所需类型名称

bool getdispose(ifstream* file) {
	string buf;
	bool gettrack, getwidth;
	gettrack = getwidth = false;
	while (getline(*file, buf)) {
		if (match("num_track", buf) >= 0) {
			num_track = getnumber(buf);
			gettrack = true;
		}
		if (match("data_width", buf) >= 0) {
			data_width = getnumber(buf);
			getwidth = true;
		}
		if (gettrack && getwidth)
			return true;
	}
	return false;
}
//获得track数目、位宽等基础配置信息

bool getsub_lut(const string& name) {
	ifstream file;
	file.open(name, ios::in);
	if (!file.is_open()) {
		cerr << "Error in getsub_lut:file open failed!" << endl;
		return false;
	}
	string buf;
	int num = 0;
	lut* sub_lut = new lut;
	while (getline(file, buf))
		if (match("num_iopin_list", buf) >= 0)
			break;
	for (int i = 0; i < gib_num - track_type; i++) {
		getline(file, buf);
		num = getnumber(buf);
		for (int j = 0; j < num; j++)
			sub_lut->addinput(track_type + i, j);
	}
	for (int i = 0; i < gib_num - track_type; i++) {
		getline(file, buf);
		if (i == gib_num - track_type - 1)
			buf += " ";
		num = getnumber(buf);
		for (int j = 0; j < num; j++)
			sub_lut->addoutput(track_type + i, j);
	}
	sub_lut->changeid(element_id[lut_num]);
	luts[lut_num++] = sub_lut;
	return true;
}
//获得子模块的端口查找表

bool getsub_adg(const string& name) {
	ifstream file;
	ofstream outfile,module_file;
	file.open(name, ios::in);
	if (!file.is_open()) {
		cerr << "Error in getsub_adg:file open failed!" << endl;
		return false;
	}
	getdispose(&file);
	string buf, module_name;
	bool getid = true;
	int num = 0;
	int id = -1;
	while (getline(file, buf))
		if (match("top_module", buf) >= 0)
			break;
	module_file.open("module", ios::out);
	while (getline(file, buf)) {
		if (getid) {
			getid = false;
			if (match("}", buf) >= 0 || match("{", buf) > 0) {
				if (outfile.is_open()) {
					element_id[element_num++] = id;
					outfile.close();
					if (match("GIB", module_name) >= 0)
						getsub_lut(module_name);
				}
				getline(file, buf);
				if (match("cfg_blk_offset", buf) >= 0)
					break;
				module_name = gettype_name(buf) + "_" + to_string(num++);
				module_file << module_name << endl;
				outfile.open(module_name, ios::out);
			}
		}
		if (match("\"id\"", buf) >= 0) {
			getid = true;
			id = getnumber(buf + " ");
		}
		outfile << buf << endl;
	}
	module_file.close();
	return true;
}
//获得每个子模块的ADG文件

int main() {
	getsub_adg("cgra_adg.json");
	return 0;
}