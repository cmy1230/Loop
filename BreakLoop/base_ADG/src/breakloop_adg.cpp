#include<iostream>
#include<string>
#include<fstream>
#include"element.h"

using namespace std;

bool prune(port* interface) {
	if (interface->getnum() > 0 || interface->getahead_num() == 0)
		return false;
	port** ports = interface->getahead();
	for (int i = 0; i < interface->getahead_num(); i++) {
		ports[i]->unassign(interface);
		prune(ports[i]);
	}
	return true;
}
//对后置端口数为0的端口进行剪枝

int getport_num(const string& buf, int& pointer) {
	int num = 0;
	for (; pointer < buf.length() && buf.at(pointer) != ','; pointer++)
		if (buf.at(pointer) >= '0' && buf.at(pointer) <= '9')
			num = num * 10 + buf.at(pointer) - '0';
	return num;
}
//从语句中的指定位置开始提取所需整数

string getport_name(const string& buf, int& pointer) {
	string name = "";
	for (; pointer < buf.length() && buf.at(pointer) != '\"'; pointer++);
	for (++pointer; pointer < buf.length() && buf.at(pointer) != '\"'; pointer++)
		name += buf.at(pointer);
	return name;
}
//从语句中的指定位置开始提取字符串

bool is_equivalence(port* port1, port* port2) {
	if (port1->getsub_id() != port2->getsub_id())
		return false;
	if (port1->getelement_id() != port2->getelement_id())
		return false;
	return true;
}
//判断是否为同一器件的端口

bool connect(library* list) {
	int num = list->getnum();
	string type1, type2, element;
	port* interface;
	port* temp;
	for (int i = 0; i < num; i++) {
		interface = list->getport(i);
		interface->getelement(element);
		if (element == "This")
			continue;
		interface->gettype(type1);
		if (type1 == "I")
			continue;
		for (int j = 0; j < num; j++) {
			if (j == i)
				continue;
			temp = list->getport(j);
			temp->gettype(type2);
			if (type2 == "O")
				continue;
			if (is_equivalence(interface, temp))
				temp->assign(interface);
		}
	}
	return true;
}
//对次级模块中的端口进行互联

bool getsub_library(const string& name, int num, int id, library* list) {
	ifstream file;
	int sub_id = 0;
	string buf;
	int pointer;
	int ahead_id, ahead_port, next_id, next_port;
	string ahead_name, ahead_type, next_name, next_type, type;
	port* ahead;
	port* next;
	port* temp;
	for (; sub_id < element_num; sub_id++)
		if (element_id[sub_id] == num)
			break;
	if (sub_id == element_num)
		return false;
	file.open(name + "_" + to_string(sub_id), ios::in);
	if (!file.is_open()) {
		cerr << "Error in getsub_library:file open failed!" << endl;
		return false;
	}
	while (getline(file, buf))
		if (match("\"connections\"", buf) >= 0)
			break;
	while (getline(file, buf)) {
		if (match("},", buf) >= 0)
			break;
		if (match("\"RF\"", buf) > 0 || match("\"Const\"", buf) > 0)
			continue;//过滤时序器件
		for (pointer = 0; pointer < buf.length() && buf.at(pointer) != '['; pointer++);
		ahead_id = getport_num(buf, pointer);
		ahead_name = getport_name(buf, pointer);
		pointer += 2;
		ahead_port = getport_num(buf, pointer);
		next_id = getport_num(buf, ++pointer);
		next_name = getport_name(buf, pointer);
		pointer += 2;
		next_port = getport_num(buf, pointer);
		//获得互联语句中的相关信息
		if (ahead_name == "This")
			ahead_type = next_type = "I";
		else if (next_name == "This")
			ahead_type = next_type = "O";
		else {
			ahead_type = "O";
			next_type = "I";
		}
		/*根据端口所属模块名称及顺序判断端口类型，判据如下
		* 1、当前顶层模块的输入连接次级模块输入
		* 2、次级模块输出连接当前顶层模块输出
		* 3、次级模块输出连接次级模块输入
		*/
		ahead = NULL;
		next = NULL;
		for (int i = 0; i < list->getnum(); i++) {
			temp = list->getport(i);
			temp->gettype(type);
			if (temp->getelement_id() == ahead_id && temp->getid() == ahead_port && type == ahead_type)
				ahead = temp;
			if (temp->getelement_id() == next_id && temp->getid() == next_port && type == next_type)
				next = temp;
		}
		if (ahead == NULL) {
			ahead = new port(name, id, ahead_name, ahead_id, ahead_type, ahead_port);
			list->wirte(ahead);
		}
		if (next == NULL) {
			next = new port(name, id, next_name, next_id, next_type, next_port);
			list->wirte(next);
		}
		ahead->assign(next);
	}
	connect(list);
	file.close();
	return true;
}
//根据次级模块的adg表述构建对应端口并形成互联关系

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