#include<iostream>
#include<string>
#include"breakloop.h"
#include<fstream>
#include<time.h>//该库仅用于测试

using namespace std;

bool getport_name(string& port_name,string& origin_name, string& buf) {
	int i;
	port_name = origin_name = "";
	for (i = 0; i < buf.length() && buf.at(i) != '.'; i++);
	for (i += +1; i < buf.length() && buf.at(i) != '('; i++)
		origin_name.push_back(buf.at(i));
	for (i += + 1; i < buf.length() && buf.at(i) != ')'; i++)
		port_name.push_back(buf.at(i));
	return true;
}
//端口名称获取

int getport_width(string& buf) {
	int width = 0;
	int i = 0;
	for (; i < buf.length() && buf.at(i) != '['; i++);
	for (++i; i < buf.length() && buf.at(i) != ':'; i++)
		width = width * 10 + buf.at(i) - 48;
	return width;
}
//端口位宽获取

bool getelement(string file_name) {
	ifstream file, module_file, net_file;
	ofstream out_file;
	file.open(file_name, ios::in);
	out_file.open(file_name + "_element", ios::out);
	if (!file.is_open()) {
		cerr << "Error in getelement:file open failed!" << endl;
		return false;
	}
	string buf, element, module_name, net, element_name, port_name, origin_name;
	int width;
	while (getline(file, buf)) {
		element = element_name = "";
		for (int i = 0; i < buf.length(); i++)
			if (buf.at(i) != ' ') {
				for (; i < buf.length() && buf.at(i) != ' '; i++)
					element.push_back(buf.at(i));//模块名称
				for (i = i + 1; i < buf.length() && buf.at(i) != ' '; i++)
					element_name.push_back(buf.at(i));//模块实例化名称
				if (element == "" || element_name == "")
					break;
				module_file.open("module", ios::in);
				while (getline(module_file, module_name)) {
					if (module_name == element) {
						net_file.open(element, ios::in);
						getline(net_file, net);
						out_file << element_name;
						while (getline(net_file, net) && getline(file, buf) && buf != "  );") {
							for (int j = 0; j < net.length(); j++) {
								if (net.at(j) == 'i') {
									width = getport_width(net);
									getport_name(port_name, origin_name, buf);
									out_file << " I " << width << " " + port_name << " " + origin_name;
									break;
								}
								if (net.at(j) == 'o') {
									width = getport_width(net);
									getport_name(port_name, origin_name, buf);
									out_file << " O " << width << " " + port_name << " " + origin_name;
									break;
								}
							}
						}
						net_file.close();
						out_file << endl;
						break;
					}
					if (module_name == element + "*") {
						net_file.open(element + "_element", ios::in);
						while (getline(net_file, net))
							out_file << element_name << spacer << net << endl;
						net_file.close();
						break;
					}
				}
				module_file.close();

			}
	}
	file.close();
	return true;
}
//元件网表获取

bool getnet(string file_name) {
	ifstream file, module_file, net_file, element_file;
	ofstream out_file;
	file.open(file_name, ios::in);
	if (!file.is_open()) {
		cerr << "Error in getnet:file open failed!" << endl;
		return false;
	}
	out_file.open(file_name + "_net", ios::out);
	string buf, element, module_name, net, element_name, port_name, temp;
	int width;
	int pos, start;
	while (getline(file, buf)) {
		if (match("reg", buf) >= 0 || match("Reg", buf) > 0)
			continue;
		if (match("wire", buf) >= 0 || match("assign", buf) >= 0) {
			pos = match("=", buf);
			if (pos >= 0) {
				port_name = "";
				for (int i = pos - 2; i >= 0 && buf.at(i) != ' '; i--)
					port_name = buf.at(i) + port_name;
				out_file << " " + port_name + " ";
				port_name = "";
				for (int i = pos + 2; i < buf.length(); i++) {
					if (buf.at(i) == ';' || buf.at(i) == '[') {
						out_file << port_name + " " << endl;
						break;
					}
					if (buf.at(i) == ' ') {
						out_file << port_name + " ";
						port_name = "";
						i++;
						for (; i < buf.length() && buf.at(i) != ' '; i++);
						continue;
					}
					port_name.push_back(buf.at(i));
				}
			}
			continue;
		}
		element = element_name = "";
		for (int i = 0; i < buf.length(); i++)
			if (buf.at(i) != ' ') {
				for (; i < buf.length() && buf.at(i) != ' '; i++)
					element.push_back(buf.at(i));//模块名称
				for (i = i + 1; i < buf.length() && buf.at(i) != ' '; i++)
					element_name.push_back(buf.at(i));//模块实例化名称
				if (element == "" || element_name == "")
					break;
				module_file.open("module", ios::in);
				while (getline(module_file, module_name)) {
					if (module_name == element + "*") {
						element_file.open(element, ios::in);
						getline(element_file, temp);
						while (getline(file, buf) && getline(element_file, temp) && buf != "  );") {
							port_name = "";
							for (start = buf.length() - 1; start >= 0 && buf.at(start) != ')'; start--);
							for (start -= 1; start >= 0 && buf.at(start) != '('; start--)
								port_name = buf.at(start) + port_name;
							out_file << spacer + element_name + " " << port_name + " " << port_name + " ";
							port_name = "";
							for (start -= 1; start >= 0 && buf.at(start) != '.'; start--)
								port_name = buf.at(start) + port_name;
							out_file << port_name + " ";
							for (start = 0; start < temp.length(); start++)
								if (temp.at(start) == 'i') {
									width = getport_width(temp);
									out_file << "I " << width << endl;
									break;
								}
								else if (temp.at(start) == 'o') {
									width = getport_width(temp);
									out_file << "O " << width << endl;
									break;
								}
						}//接口特殊书写
						element_file.close();
						if (module_name == element)
							break;
						net_file.open(element + "_net", ios::in);
						port_name = "";
						while (getline(net_file, net))
							out_file << spacer << element_name << net << endl;
						net_file.close();//次级模块互联书写
						break;
					}
				}
				module_file.close();
			}
	}
	out_file.close();
	file.close();
	return true;
}
//互联网表获取

bool getmodule(string file_name) {
	ifstream file;
	ofstream out_file, module_file;
	module_file.open("module", ios::out);
	file.open(file_name, ios::in);
	if (!file.is_open()) {
		cerr << "Error in getmoudle:file open failed!" << endl;
		return false;
	}
	string buf, temp, buffer;
	while (getline(file, buf)) {
		temp = "";
		for (int i = 0; i < buf.length(); i++)
			if (buf.at(i) == ' ' && buf.substr(0, i) == "module") {
				for (int j = i + 1; j < buf.length() && buf.at(j) != '('; j++)
					temp.push_back(buf.at(j));
				getline(file, buf);
				buffer = buf;
				out_file.open(temp, ios::out);
				out_file << temp << endl;
				do {
					out_file << buf << endl;
				} while (buf != "endmodule" && getline(file, buf));
				out_file.close();
				if (match("clock", buffer) >= 0) {
					module_file << temp << "*" << endl;
					getelement(temp);
					getnet(temp);
				}
				else
					module_file << temp << endl;
				break;
			}
	}
	module_file.close();
	file.close();
	return true;
}
//模块网表获取

bool getnet_name(string& element, string& net_name) {
	string temp = "";
	net_name = "";
	for (int i = 0; i < element.length(); i++) {
		if (element.at(i) == space) {
			net_name += temp;
			temp = "";
		}
		else if (element.at(i) == ' ')
			return true;
		else
			temp += element.at(i);
	}
	return true;
}
//从元件名称中提取网络名称

bool opennetlist(library& net, string file_name) {
	ifstream file;
	file.open(file_name, ios::in);
	if (!file.is_open()) {
		cerr << "Error in opennetlist:file open failed!" << endl;
		return false;
	}
	string buf, port_name, element_name, net_name, origin_name;
	int width;
	port* pin1;
	port* pin2;
	while (getline(file, buf)) {
		element_name = "";
		for (int i = 0; i < buf.length(); i++) {
			if (buf.at(i) == ' ') {
				if (element_name == "")
					element_name = buf.substr(0, i);
				i++;
				port_name = "";
				origin_name = "";
				width = 0;
				switch (buf.at(i)) {
				case'I':
					pin1 = new port;
					pin2 = new port;
					i += 2;
					while (i < buf.length() && buf.at(i) != ' ')
						width = width * 10 + buf.at(i++) - 48;
					i++;
					while (i < buf.length() && buf.at(i) != ' ')
						port_name.push_back(buf.at(i++));
					i++;
					while (i < buf.length() && buf.at(i) != ' ')
						origin_name.push_back(buf.at(i++));
					getnet_name(element_name, net_name);
					pin1->changewidth(width);
					pin1->changename(element_name);
					pin1->changenet(net_name);
					pin1->changetype("JI");
					pin1->changeport_name(port_name);
					pin2->changewidth(width);
					pin2->changename(element_name);
					pin2->changenet(element_name);
					pin2->changetype("I");
					pin2->changeport_name(origin_name);
					pin1->assign(pin2);
					net.wirte(pin1);
					net.wirte(pin2);
					i--;
					break;
				case'O':
					pin1 = new port;
					pin2 = new port;
					i += 2;
					while (i < buf.length() && buf.at(i) != ' ')
						width = width * 10 + buf.at(i++) - 48;
					i++;
					while (i < buf.length() && buf.at(i) != ' ')
						port_name.push_back(buf.at(i++));
					i++;
					while (i < buf.length() && buf.at(i) != ' ')
						origin_name.push_back(buf.at(i++));
					getnet_name(element_name, net_name);
					pin1->changewidth(width);
					pin1->changename(element_name);
					pin1->changenet(net_name);
					pin1->changetype("JO");
					pin1->changeport_name(port_name);
					pin2->changewidth(width);
					pin2->changename(element_name);
					pin2->changenet(element_name);
					pin2->changetype("O");
					pin2->changeport_name(origin_name);
					pin2->assign(pin1);
					net.wirte(pin1);
					net.wirte(pin2);
					i--;
					break;
				default:;
				}
			}
		}
	}
	file.close();
	return true;
}
//从元件网表中提取端口

port* getport(library& net, string net_name, string port_name) {
	int n_net = net.getnum();
	string temp, port_name_in;
	port* port_in = NULL;
	for (int i = 0; i < n_net; i++) {
		net.getport(i)->getnet(temp);
		net.getport(i)->getport_name(port_name_in);
		if (temp == net_name && port_name_in == port_name) {
			port_in = net.getport(i);
			break;
		}
	}
	return port_in;
}
//根据端口名和网络名查找对应端口指针

port* getport(library& net, string port_name) {
	int n_net = net.getnum();
	string temp, port_name_in;
	port* port_in = NULL;
	for (int i = 0; i < n_net; i++) {
		net.getport(i)->getnet(temp);
		net.getport(i)->getport_name(port_name_in);
		if (temp == "" && port_name_in == port_name) {
			port_in = net.getport(i);
			break;
		}
	}
	return port_in;
}
//根据端口名查找对应端口指针

bool interconnect(library& net, string file_name) {
	ifstream file;
	file.open(file_name, ios::in);
	if (!file.is_open()) {
		cerr << "Error in interconnect:file open failed!" << endl;
		return false;
	}
	string buf, port_name_in, port_name_out, temp, temp1, net_name, port_name, type;
	int start, width;
	int n_net = net.getnum();
	port* port_in;
	port* port_out;
	while (getline(file, buf)) {
		port_name_in = "";
		port_name_out = "";
		type = "";
		if (buf.at(0) == space) {
			net_name = "";
			for (start = 1; start < buf.length() && buf.at(start) != ' '; start++)
				net_name += buf.at(start);
			for (start += 1; start < buf.length() && buf.at(start) != ' '; start++)
				port_name_in += buf.at(start);
			port_in = getport(net, net_name, port_name_in);
			if (port_in == NULL) {
				for (start += 1; start < buf.length() && buf.at(start) != ' '; start++)
					port_name_out += buf.at(start);
				if (port_name_out != port_name_in)
					continue;
				getnet_name(net_name, temp);
				port_name_out = type = "";
				for (start += 1; start < buf.length() && buf.at(start) != ' '; start++)
					port_name_out += buf.at(start);
				type += buf.at(++start);
				width = 0;
				for (start += 2; start < buf.length() && buf.at(start) != ' '; start++)
					width = width * 10 + buf.at(start) - 48;
				port_in = new port;
				port_in->changewidth(width);
				port_in->changename(temp);
				port_in->changenet(temp);
				port_in->changeport_name(port_name_in);
				port_out = new port;
				port_out->changewidth(width);
				port_out->changenet(net_name);
				port_out->changename(net_name);
				port_out->changeport_name(port_name_out);
				if (type == "I") {
					port_in->changetype("J" + type);
					port_out->changetype("J" + type);
					port_in->assign(port_out);
				}
				if (type == "O") {
					port_in->changetype("J" + type);
					port_out->changetype(type);
					port_out->assign(port_in);
				}
				net.wirte(port_in);
				net.wirte(port_out);
				continue;
			}//接口处理
			for (start += 1; start < buf.length(); start++) {
				if (buf.at(start) == ' ') {
					port_out = getport(net, net_name, port_name_out);
					port_name_out = "";
					if (port_out == NULL)
						continue;
					port_out->assign(port_in);
				}
				else
					port_name_out += buf.at(start);
			}
		}
		else {
			for (start = 1; start < buf.length() && buf.at(start) != ' '; start++)
				port_name_in += buf.at(start);
			port_in = getport(net, port_name_in);
			if (port_in == NULL)
				continue;
			for (start += 1; start < buf.length(); start++) {
				if (buf.at(start) == ' ') {
					port_out = getport(net, port_name_out);
					port_name_out = "";
					if (port_out == NULL)
						continue;
					port_out->assign(port_in);
				}
				else
					port_name_out += buf.at(start);
			}
		}
	}
	file.close();
	return true;
}
//从网络网表中提取对应端口并构成互联关系

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

bool connect(library& netlist, library& outlist) {
	int n_net = netlist.getnum();
	string name1, name2, type_name1, type_name2;
	port* interface = NULL;
	port* temp = NULL;
	for (int i = 0; i < n_net; i++) {
		interface = netlist.getport(i);
		interface->gettype(type_name1);
		switch (type_name1.at(0)) {
		case'I':break;
		case'O':
			interface->getname(name1);
			for (int j = 0; j < n_net; j++) {
				if (j == i)
					continue;
				temp = netlist.getport(j);
				temp->gettype(type_name2);
				temp->getname(name2);
				if (name1!=""&&name1 == name2 && type_name2.at(0) == 'I') {
					temp->assign(interface);
				}
			}
			break;
		default:;
		}
	}
	for (int i = 0; i < n_net; i++) {
		interface = netlist.getport(i);
		prune(interface);
		interface->gettype(type_name1);
		if(interface->getnum()>0 &&type_name1.at(0)=='O')
			outlist.wirte(interface);
	}
	return true;
}
//构成组合元件的互联关系

bool wirte_constraints(ofstream* file, string element_name1, string element_name2,string port_name1, string port_name2, int width) {
	bool way = false;
	if (way) {
		if (width == 0) {
			*file << "set_disable_timing" << " [get_cells " << element_name1 << "] -from "; 
			*file << port_name1 << " -to " << port_name2;
			return true;
			//*file << " [get_cells " << element_name << "]";
		}
		for (int i = 0; i <= width; i++) {
			*file << "set_disable_timing [get_cells " << element_name1 << "] -from ";
			*file << port_name1 + "[" << i << "] -to " << port_name2 << "[" << i << "]" << endl;
			//*file << "[" << i << "] [get_cells " << element_name << "]" << endl;
		}
	}
	else {
		if (width == 0) {
			*file << "set_disable_timing -through [get_pins " << element_name1 << spacer + port_name1 + "]";
			*file << " -through [get_pins " << element_name2 << spacer + port_name2 + "]" << endl;
			return true;
		}
		for (int i = 0; i <= width; i++) {
			*file << "set_false_path -through [get_pins " << element_name1 << spacer + port_name1 + "[" << i << "]]";
			*file << " -through [get_pins " << element_name2 << spacer + port_name2 + "[" << i << "]]" << endl;
		}
	}
	*file << endl;
	return true;
}
//书写约束文件

bool getchisel_element(string element_name, string& type, int& number) {
	string temp = "";
	char value;
	int i = 0;
	for (; i < element_name.length(); i++) {
		value = element_name.at(i);
		if (value == '_') {
			type = temp;
			temp = "";
			break;
		}
		temp += value;
	}
	i++;
	for (; i < element_name.length(); i++) {
		value = element_name.at(i);
		if (value == '_') {
			stringtoint(temp, number);
			break;
		}
		temp += value;
	}
	return true;
}
//从端口名中获得chisel语句中器件相关信息

bool getchisel_port(string port_name, string& type, int& number) {
	string temp = "";
	char value;
	int i;
	for (i = port_name.length() - 1; i >= 0; i--) {
		value = port_name.at(i);
		if (value == '_') {
			stringtoint(temp, number);
			temp = "";
			break;
		}
		temp = value + temp;
	}
	i--;
	for (; i >= 0; i--) {
		value = port_name.at(i);
		if (value == '_') {
			type = temp;
			break;
		}
		temp = value + temp;
	}

	return true;
}
//从端口名中获得chisel语句中端口的相关信息

bool wirte_chisel(ofstream* file,  string port_name1, string port_name2, int number) {
	string chisel_type;
	int chisel_number;
	getchisel_element(port_name2, chisel_type, chisel_number);
	*file << chisel_type + "(" << chisel_number << ").io.";
	getchisel_port(port_name2, chisel_type, chisel_number);
	*file << chisel_type + "(" << chisel_number;
	getchisel_element(port_name1, chisel_type, chisel_number);
	*file << ") := RegNext(" << chisel_type + "(" << chisel_number << ").io.";
	getchisel_port(port_name1, chisel_type, chisel_number);
	*file << chisel_type + "(" << chisel_number << "))" << endl;
	// ") := breakloop_reg_" << number << endl;
	return true;
}
//书写chisel文件

bool ergodic(stack& T, string& port_name,string& type, string& element_name,string& net_name) {
	if (T.empty())
		return false;
	port* temp = T.pop();
	port** next = temp->getnext();
	int n = temp->getnum();
	if (temp == NULL || next == NULL)
		return false;
	temp->getname(element_name);
	temp->getnet(net_name);
	temp->getport_name(port_name);
	temp->gettype(type);
	for (int i = 0; i < n; i++)
		T.push(next[i]);
	return true;
}
//单次深度遍历操作

bool optimize(port** ports, int num,int out_num) {
	ofstream file;
	file.open("breakloop.xdc", ios::out);
	stack T;
	port* port1;
	port* port2;
	int reg_num = 0;
	string port_name1, element_name2, port_name2, net_name2, type2;
	string temp = "";
	for (int i = 0; i < num; i += 2) {
		T.clear();
		port1 = ports[i];
		port2 = ports[i + 1];
		port1->getport_name(port_name1);
		T.push(port2);
		port1->unassign(port2);
		port1->assign(port2);
		while (!T.empty()) {
			if (port1 == T.gettop()) {
				port1->getport_name(port_name1);
				port2->getport_name(port_name2);
				cout << "wirte constraints...\t" << reg_num << endl;
				wirte_chisel(&file, port_name1, port_name2, reg_num++);
				port1->unassign(port2);
				if (port1->getnum() == 0)
					prune(port1);
				break;
			}
			while (!T.empty() && T.gettop()->getvisited() == i)
				T.pop();
			if (!T.empty() && T.gettop()->getvisited() < i)
				T.gettop()->changevisited(i);
			//通过对已遍历的端口进行标记，防止过深的路径影响运行速度
			if (!T.empty())
				ergodic(T, port_name2, type2, element_name2, net_name2);
		}
	}
	file.close();
	return true;
}
//对得到的寄存器位置进行优化

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
	int out_num =  outlist.getnum();
	//int width;
	stack T1, T2;
	string element_name1, element_name2, port_name1, port_name2, net_name1, net_name2, type1, type2;
	string previous_port, previous_element, previous_net, previous_type;
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
				ergodic(T1, previous_port, previous_type, previous_element, previous_net);
				if (T1.empty())
					break;
				ergodic(T1, port_name1, type1, element_name1, net_name1);
				ergodic(T2, port_name2, type2, element_name2, net_name2);
				if (T1.empty() || T2.empty())
					break;
				if (port_name1 == port_name2 && element_name1 == element_name2 && net_name1 == net_name2) {
					if (type1 != "JI" || previous_type != "JO") {
						ergodic(T1, port_name1, type1, element_name1, net_name1);
						continue;
					}
					if (match("gib", port_name1) < 0 || match("gib", previous_port) < 0) {
						ergodic(T1, port_name1, type1, element_name1, net_name1);
						continue;
					}
					/*
					if (type1 == "JO" && previous_type == "OM") {
						for (int j = 0; j < 6; j++)
							ergodic(T1, port_name1, type1, element_name1, net_name1);
					}
					if (element_name1 == "" || previous_element == "") {
						ergodic(T1, port_name1, type1, element_name1, net_name1);
						continue;
					}
					if ((type1 != "IM" ) || (previous_type != "OM" )) {
						ergodic(T1, port_name1, type1, element_name1, net_name1);
						continue;
					}*/
					port1->unassign(port2);
					/*cout << "wirte constraints..." << endl;
					//width = port1->getwidth();
					//wirte_constraints(&file, previous_net, net_name1, previous_port, port_name1, width);*/
					ports[num++] = port1;
					ports[num++] = port2;
					getloop = true;
					break;
				}
				if (port1 == port2)
					ergodic(T1, port_name1, type1, element_name1, net_name1);
			}
			T1.clear();
			T2.clear();
		}
	}
	return optimize(ports, num, out_num);
	//file.close();
	//return true;
}
//打破环路

int main() {
	ofstream file,file1;
	file.open("result.txt", ios::out);
	file1.open("result1.txt", ios::app);
	int i = 9;
	int j = 9;
	clock_t start, end;
	library net;
	library net1;
	start = clock();
	getmodule("CGRA.v");
	opennetlist(net, "CGRA_element");
	interconnect(net, "CGRA_net");
	connect(net, net1);
	end = clock();
	breakloop(net1);
	file << end - start << endl;
	file1 << i * j << endl;
	file.close();
	file1.close();
	file.open("end.txt", ios::app);
	file.close();
	return 0;
}