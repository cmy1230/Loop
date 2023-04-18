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
//�Ժ��ö˿���Ϊ0�Ķ˿ڽ��м�֦

int getport_num(const string& buf, int& pointer) {
	int num = 0;
	for (; pointer < buf.length() && buf.at(pointer) != ','; pointer++)
		if (buf.at(pointer) >= '0' && buf.at(pointer) <= '9')
			num = num * 10 + buf.at(pointer) - '0';
	return num;
}
//������е�ָ��λ�ÿ�ʼ��ȡ��������

string getport_name(const string& buf, int& pointer) {
	string name = "";
	for (; pointer < buf.length() && buf.at(pointer) != '\"'; pointer++);
	for (++pointer; pointer < buf.length() && buf.at(pointer) != '\"'; pointer++)
		name += buf.at(pointer);
	return name;
}
//������е�ָ��λ�ÿ�ʼ��ȡ�ַ���

bool is_equivalence(port* port1, port* port2) {
	if (port1->getsub_id() != port2->getsub_id())
		return false;
	if (port1->getelement_id() != port2->getelement_id())
		return false;
	return true;
}
//�ж��Ƿ�Ϊͬһ�����Ķ˿�

bool connect(library* list) {
	int num = list->getnum();
	string type1, type2;
	port* interface;
	port* temp;
	for (int i = 0; i < num; i++) {
		interface = list->getport(i);
		if (interface->getelement_id() == 0)
			continue;
		interface->gettype(type1);
		if (type1 == "I")
			continue;
		for (int j = 0; j < num; j++) {
			if (j == i)
				continue;
			temp = list->getport(j);
			temp->gettype(type2);
			if (type2 != "I")
				continue;
			if (is_equivalence(interface, temp))
				temp->assign(interface);
		}
	}
	return true;
}
//�Դμ�ģ���еĶ˿ڽ��л���

bool getsub_library(const string& name, int num, int id, library* list) {
	ifstream file;
	int sub_id = 0;
	string buf;
	int pointer;
	int ahead_id, ahead_port, next_id, next_port;
	int sub_num;
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
	if (name == "GIB")
		sub_num = gib++;
	else if (name == "GPE")
		sub_num = gpe++;
	else if (name == "OB")
		sub_num = ob++;
	else if (name == "IB")
		sub_num = ib++;
	if (!file.is_open()) {
		cerr << "Error in getsub_library:file" + name + "_" + to_string(sub_id) + " open failed!" << endl;
		return false;
	}
	while (getline(file, buf))
		if (match("\"connections\"", buf) >= 0)
			break;
	while (getline(file, buf)) {
		if (match("}", buf) >= 0)
			break;
		if (match("\"RF\"", buf) > 0 || match("\"Const\"", buf) > 0)
			continue;//����ʱ������
		for (pointer = 0; pointer < buf.length() && buf.at(pointer) != '['; pointer++);
		ahead_id = getport_num(buf, pointer);
		ahead_name = getport_name(buf, pointer);
		pointer += 2;
		ahead_port = getport_num(buf, pointer);
		next_id = getport_num(buf, ++pointer);
		next_name = getport_name(buf, pointer);
		pointer += 2;
		next_port = getport_num(buf, pointer);
		//��û�������е������Ϣ
		if (ahead_name == "This") {
			if (next_name == "This") {
				ahead_type = "I";
				next_type = "O";
			}
			else
				ahead_type = next_type = "I";
		}
		else if (next_name == "This")
			ahead_type = next_type = "O";
		else {
			ahead_type = "O";
			next_type = "I";
		}
		/*���ݶ˿�����ģ�����Ƽ�˳���ж϶˿����ͣ��о�����
		* 1����ǰ����ģ����������Ӵμ�ģ������
		* 2���μ�ģ��������ӵ�ǰ����ģ�����
		* 3���μ�ģ��������Ӵμ�ģ������
		* 4�����޴μ�ģ�飬�򶥲�ģ�����������
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
			ahead = new port(name, id, ahead_name, ahead_id, ahead_type, ahead_port, sub_num, sub_id);
			list->wirte(ahead);
		}
		if (next == NULL) {
			next = new port(name, id, next_name, next_id, next_type, next_port, sub_num, sub_id);
			list->wirte(next);
		}
		ahead->assign(next);
	}
	connect(list);
	file.close();
	return true;
}
//���ݴμ�ģ���adg����������Ӧ�˿ڲ��γɻ�����ϵ

int getnumber(const string& buf) {
	int start;
	int num = 0;
	for (start = buf.length() - 2; start >= 0 && buf.at(start) != ' '; start--);
	for (++start; start <= buf.length() - 2; start++)
		num = num * 10 + buf.at(start) - '0';
	return num;
}
//����������������

string gettype_name(const string& buf) {
	int start;
	string type = "";
	for (start = buf.length() - 3; start >= 0 && buf.at(start) != '\"'; start--);
	for (++start; start <= buf.length() - 3; start++)
		type += buf.at(start);
	return type;
}
//��������������������

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
//���track��Ŀ��λ��Ȼ���������Ϣ

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
//�����ģ��Ķ˿ڲ��ұ�

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
	outfile.open("Top", ios::out);
	while (getline(file, buf)) {
		outfile << buf << endl;
	}
	module_file.close();
	return true;
}
//���ÿ����ģ�鼰����ģ���ADG�ļ�

bool getconnection(ifstream& file,library* netlist){
	int pointer;
	int ahead_id, ahead_port, next_id, next_port;
	port* port1;
	port* port2;
	string ahead_name, next_name, buf;
	while (getline(file, buf))
		if (match("\"connections\"", buf) >= 0)
			break;
	while (getline(file, buf)) {
		if (match("},", buf) >= 0)
			break;
		if (match("This", buf) >= 0)
			continue;
		for (pointer = 0; pointer < buf.length() && buf.at(pointer) != '['; pointer++);
		ahead_id = getport_num(buf, pointer);
		ahead_name = getport_name(buf, pointer);
		pointer += 2;
		ahead_port = getport_num(buf, pointer);
		next_id = getport_num(buf, ++pointer);
		next_name = getport_name(buf, pointer);
		pointer += 2;
		next_port = getport_num(buf, pointer);
		port1 = netlist->getport(ahead_id, ahead_port, "O");
		port2 = netlist->getport(next_id, next_port, "I");
		if (port1 == NULL || port2 == NULL)
			continue;
		port1->assign(port2);
	}
	return true;
}
//������ģ��֮��Ļ���

bool getsub_module(library* netlist) {
	library list;
	ifstream file;
	file.open("Top", ios::in);
	if (!file.is_open()) {
		cerr << "Error in getsub_module:file open failed!" << endl;
		return false;
	}
	int module_id, id;
	string element, buf;
	while (getline(file, buf)) {
		if (match("} ],", buf) >= 0)
			break;
		while (getline(file, buf)) {
			if (match("\"id\"", buf) >= 0) {
				id = getnumber(buf + " ");
				break;
			}
			if (match("\"module_id\"", buf) >= 0) {
				module_id = getnumber(buf);
				continue;
			}
			if (match("\"type\"", buf) >= 0)
				element = gettype_name(buf);
		}
		if (element == "This")
			continue;
		list.clear();
		getsub_library(element, module_id, id, &list);
		netlist->merge(&list);
	}
	getconnection(file, netlist);
	file.close();
	return true;
}
//ʵ����ÿ���������ģ��

bool wirte_chisel(ofstream* file, port* port1, port* port2, int number) {
	*file << position << "(" << port2->getorder() << ").io.";
	*file << input_name[luts[port2->getmodule_id()]->getinput(port2->getid())] + "(";
	*file << luts[port2->getmodule_id()]->getinput_id(port2->getid());
	*file << ") := RegNext(" << position << "(" << port1->getorder() << ").io.";
	*file << output_name[luts[port1->getmodule_id()]->getinput(port1->getid())] + "(";
	*file << luts[port1->getmodule_id()]->getinput_id(port1->getid()) << "))" << endl;
	return true;
}
//��дchisel�ļ�

bool ergodic(stack& T, int& port_id, string& type, int& element_id,int& sub_id,string& sub_name) {
	if (T.empty())
		return false;
	port* temp = T.pop();
	port** next = temp->getnext();
	int n = temp->getnum();
	if (temp == NULL || next == NULL)
		return false;
	element_id = temp->getelement_id();
	port_id = temp->getid();
	sub_id = temp->getsub_id();
	temp->getsub_module(sub_name);
	temp->gettype(type);
	for (int i = 0; i < n; i++)
		T.push(next[i]);
	return true;
}
//������ȱ�������

bool optimize(port** ports, int num, int out_num) {
	ofstream file;
	file.open("breakloop.xdc", ios::out);
	stack T;
	port* port1;
	port* port2;
	int reg_num = 0;
	int port_id1, port_id2, sub_id1, sub_id2;
	int element_id2;
	string type1, type2, sub2;
	string temp = "";
	for (int i = 0; i < num; i += 2) {
		T.clear();
		port1 = ports[i];
		port2 = ports[i + 1];
		port_id1 = port1->getid();
		sub_id1 = port1->getsub_id();
		T.push(port2);
		port1->unassign(port2);
		port1->assign(port2);
		while (!T.empty()) {
			if (port1 == T.gettop()) {
				port_id2 = port2->getid();
				sub_id2 = port2->getsub_id();
				cout << "wirte constraints...\t" << reg_num << endl;
				wirte_chisel(&file, port1, port2, reg_num++);
				port1->unassign(port2);
				if (port1->getnum() == 0)
					prune(port1);
				break;
			}
			while (!T.empty() && T.gettop()->getvisited() == i)
				T.pop();
			if (!T.empty() && T.gettop()->getvisited() < i)
				T.gettop()->changevisited(i);
			//ͨ�����ѱ����Ķ˿ڽ��б�ǣ���ֹ�����·��Ӱ�������ٶ�
			if (!T.empty())
				ergodic(T, port_id2, type2, element_id2, sub_id2, sub2);
		}
	}
	file.close();
	return true;
}
//�Եõ��ļĴ���λ�ý����Ż�

bool breakloop(library& outlist) {
	/*ofstream file;
	file.open("breakloop.xdc", ios::out);
	//file << "set_hierarchy_separator " << spacer << endl;*/
	port* interface = NULL;
	port* port1;
	port* port2;
	port* ports[max];
	bool getloop = true;
	int num = 0;
	int out_num = outlist.getnum();
	//int width;
	stack T1, T2;
	int element_id1, element_id2, sub_id1, sub_id2, id1, id2;
	int previous_sub, previous_element, previous_id;
	string type1, type2, previous_type, sub_name1, sub_name2, previous_name;
	string temp;
	while (getloop) {
		getloop = false;
		for (int i = 0; i < out_num; i++) {
			interface = outlist.getport(i);
			T1.push(interface);
			T2.push(interface);
			while (!T1.empty() && !T2.empty()) {
				port1 = T1.gettop();
				port2 = T2.gettop();
				ergodic(T1, previous_id, previous_type, previous_element, previous_sub, previous_name);
				if (T1.empty())
					break;
				ergodic(T1, id1, type1, element_id1, sub_id1, sub_name1);
				ergodic(T2, id2, type2, element_id2, sub_id2, sub_name2);
				if (T1.empty() || T2.empty())
					break;
				if (id1 == id2 && element_id1 == element_id2 && sub_id1 == sub_id2 && type1 == type2) {
					if (previous_element != 0 || element_id1 != 0) {
						ergodic(T1, id1, type1, element_id1, sub_id1, sub_name1);
						continue;
					}
					if (previous_name != "GIB" || sub_name2 != "GIB") {
						ergodic(T1, id1, type1, element_id1, sub_id1, sub_name1);
						continue;
					}
					port1->unassign(port2);
					ports[num++] = port1;
					ports[num++] = port2;
					getloop = true;
					break;
				}
				if (port1 == port2)
					ergodic(T1, id1, type1, element_id1, sub_id1, sub_name1);
			}
			T1.clear();
			T2.clear();
		}
	}
	return optimize(ports, num, out_num);
}
//���ƻ�·

int main() {
	getsub_adg("cgra_adg.json");
	library netlist;
	getsub_module(&netlist);
	breakloop(netlist);
	return 0;
}