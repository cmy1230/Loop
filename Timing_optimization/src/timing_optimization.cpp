#include<iostream>
#include<string>
#include"element.h"
#include<fstream>
#include<time.h>//该库仅用于测试

using namespace std;

bool rerun = false;

int get_rand(int min_rand, int max_rand) {
	return (rand() % (max_rand - min_rand + 1)) + min_rand;
}

bool is_repeat(vector<int> list, int num) {
	for (int i = 0; i < list.size(); i++)
		if (list[i] == num)
			return true;
	return false;
}

bool getport_name(string& port_name, string& origin_name, string& buf) {
	int i;
	port_name = origin_name = "";
	for (i = 0; i < buf.length() && buf.at(i) != '.'; i++);
	for (i += +1; i < buf.length() && buf.at(i) != '('; i++)
		origin_name.push_back(buf.at(i));
	for (i += +1; i < buf.length() && buf.at(i) != ')'; i++)
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
					pin1->changetype("I");
					pin1->changeport_name(port_name);
					net.wirte(pin1);
					i--;
					break;
				case'O':
					pin1 = new port;
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
					pin1->changetype("O");
					pin1->changeport_name(port_name);
					pin1->changedelay(1);
					net.wirte(pin1);
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
	int n_net = net.get_num();
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
	int n_net = net.get_num();
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
	int n_net = net.get_num();
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
					port_out->changetype("JJ" + type);
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
					port_in->gettype(type);
					if (type.at(0) == 'O') {
						port_out->gettype(type);
						if (type == "JJI")
							port_in->changedelay(1);
					}
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

bool connect(library& netlist, library& outlist) {
	int n_net = netlist.get_num();
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
				if (name1 != "" && name1 == name2 && type_name2.at(0) == 'I') {
					temp->assign(interface);
				}
			}
			break;
		case'J':
			if (type_name1.at(1) == 'O')
				interface->getahead()[0]->next_assign(interface);
			else if (type_name1.at(1) == 'I') {
				interface->next_assign(interface->getnext()[0]);
			}
		default:;
		}
	}
	for (int i = 0; i < n_net; i++) {
		interface = netlist.getport(i);
		interface->gettype(type_name1);
		if (type_name1 == "JI")
			interface->changetype("I");
		else if (type_name1.at(0) == 'J')
			continue;
		outlist.wirte(interface);
	}
	return true;
}
//构成组合元件的互联关系

int get_port_delay(port* interface) {
	if (interface->get_pose_delay() >= 0)
		return interface->get_pose_delay();
	if (interface->get_next_num() == 0) {
		interface->change_pose_delay(0);
		return 0;
	}
	interface->change_pose_delay(0);
	string name;
	interface->getport_name(name);
	if (match("config", name) >= 0)
		return 0;
	vector<port*> next = interface->getnext();
	int max_delay = 0;
	int temp;
	for (int i = 0; i < interface->get_next_num(); i++) {
		temp = get_port_delay(next[i]) + next[i]->getdelay();
		if (temp > max_delay)
			max_delay = temp;
	}
	interface->change_pose_delay(max_delay);
	return max_delay;
}
//获得端口延迟

int get_port_pre_delay(port* interface) {
	if (interface->get_pre_delay() >= 0)
		return interface->get_pre_delay();
	if (interface->get_ahead_num() == 0) {
		interface->change_pre_delay(0);
		return 0;
	}
	interface->change_pre_delay(0);
	string name;
	interface->getport_name(name);
	if (match("config", name) >= 0)
		return 0;
	vector<port*> ahead = interface->getahead();
	int max_delay = 0;
	int temp;
	for (int i = 0; i < interface->get_ahead_num(); i++) {
		temp = get_port_pre_delay(ahead[i]) + interface->getdelay();
		if (temp > max_delay)
			max_delay = temp;
	}
	interface->change_pre_delay(max_delay);
	return max_delay;
}
//获得端口反向延迟

bool print_way(port* interface) {
	interface->print();
	if (interface->get_next_num() == 0)
		return true;
	int sign = 0;
	int max_delay = 0;
	vector<port*> next = interface->getnext();
	for (int i = 0; i < interface->get_next_num(); i++) {
		if (next[i]->get_pose_delay() > max_delay) {
			max_delay = next[i]->get_pose_delay();
			sign = i;
		}
	}
	return print_way(next[sign]);
}

bool print_ahead_way(port* interface) {
	interface->print();
	if (interface->get_ahead_num() == 0)
		return true;
	int sign = 0;
	int max_delay = 0;
	vector<port*> next = interface->getahead();
	for (int i = 0; i < interface->get_ahead_num(); i++) {
		if (next[i]->get_pre_delay() > max_delay) {
			max_delay = next[i]->get_pre_delay();
			sign = i;
		}
	}
	return print_ahead_way(next[sign]);
}

int get_delay(library& netlist) {
	int num = netlist.get_num();
	port* interface;
	int sign = 0;
	int max_delay = 0;
	string type;
	for (int i = 0; i < num; i++) {
		interface = netlist.getport(i);
		interface->gettype(type);
		if (match(type, "I") < 0)
			continue;
		//if (interface->get_ahead_num() > 0)
			//continue;
		if (get_port_delay(interface) > max_delay) {
			sign = i;
			max_delay = get_port_delay(interface);
		}
	}
	return max_delay;
}
//获得关键路径延时

int get_ahead_delay(library& netlist) {
	int num = netlist.get_num();
	port* interface;
	int sign = 0;
	int max_delay = 0;
	string type;
	for (int i = 0; i < num; i++) {
		interface = netlist.getport(i);
		interface->gettype(type);
		if (match(type, "O") < 0)
			continue;
		if (i > 0) {
			if (get_port_pre_delay(interface) > max_delay) {
				sign = i;
				max_delay = get_port_pre_delay(interface);
			}
		}
	}
	return max_delay;
}
//获得反向关键路径延时

/*bool get_max_delay_port(library& netlist, double upper) {
	int num = netlist.get_num();
	int max_delay = 0;
	int temp_delay = 0;
	int sign = 0;
	string type;
	netlist.clear_delay();
	if (upper >= get_delay(netlist))
		return false;
	else
		get_ahead_delay(netlist);
	port* interface;
	for (int i = 0; i < num; i++) {
		interface = netlist.getport(i);
		interface->gettype(type);
		if (match("J", type) >= 0)
			continue;
		if (interface->get_pre_delay() > upper)
			continue;
		temp_delay = 0;
		for (int j = 0; j < interface->get_next_num(); j++) {
			if (interface->getnext()[j]->get_pose_delay() <= upper)
				continue;
			temp_delay += interface->get_pre_delay();
		}
		if (temp_delay > max_delay) {
			max_delay = temp_delay;
			sign = i;
		}
	}
	cout << max_delay << endl;
	print_ahead_way(netlist.getport(sign));
	return true;
}
*/

bool get_start_port(library netlist, library& outlist, int delay_min) {
	int num = netlist.get_num();
	int result = 0;
	string type;
	netlist.clear_delay();
	get_delay(netlist);
	outlist.clear();
	get_ahead_delay(netlist);
	for (int i = 0; i < num; i++) {
		if (netlist.getport(i)->get_ahead_num() != 0)
			continue;
		if (netlist.getport(i)->get_pose_delay() <= delay_min)
			continue;
		outlist.wirte(netlist.getport(i));
	}
	return true;
}

vector<port*> get_path_port(port* interface) {
	vector<port*> ports, next, temp;
	string type;
	interface->gettype(type);
	next = interface->getnext();
	for (int i = 0; i < interface->get_next_num(); i++) {
		if (next[i]->get_pose_delay() < interface->get_pose_delay() - (type == "I"))
			continue;
		ports.push_back(next[i]);
		temp = get_path_port(next[i]);
		for (int j = 0; j < temp.size(); j++)
			ports.push_back(temp[j]);
	}
	return ports;
}

bool get_path_net(library& netlist,library& outlist) {
	int num = netlist.get_num();
	vector<port*> ports;
	string type;
	for (int i = 0; i < num; i++) {
		ports = get_path_port(netlist.getport(i));
		for (int j = 0; j < ports.size(); j++)
			if (!netlist.repeat(ports[j]))
				netlist.wirte(ports[j]);
	}
	num = netlist.get_num();
	for (int i = 0; i < num; i++) {
		if (netlist.getport(i)->get_ahead_num() == 0)
			continue;
		netlist.getport(i)->gettype(type);
		if (type == "I")
			continue;
		outlist.wirte(netlist.getport(i));
	}
	return true;
}

int test_delay(library netlist, library net, vector<int> reg_num_list) {
	port* interface;
	vector<port*> next;
	pair<port*, vector<port*>> reg;
	vector<pair<port*, vector<port*>>> reg_list;
	for (int i = 0; i < reg_num_list.size(); i++) {
		interface = netlist.getport(reg_num_list[i]);
		next = interface->getnext();
		for (int j = 0; j < next.size(); j++)
			interface->unassign(next[j]);
		reg.first = interface;
		reg.second = next;
		reg_list.push_back(reg);
	}
	net.clear_delay();
	int result = get_delay(net);
	for (int i = 0; i < reg_list.size(); i++) {
		next = reg_list[i].second;
		interface = reg_list[i].first;
		for (int j = 0; j < next.size(); j++)
			interface->assign(next[j]);
	}
	return result;
}

pair<int,vector<int>> rand_port(library& netlist, library& net, int reg_num) {
	vector<int> list;
	int max_num = netlist.get_num() - 1;
	int rand_num;
	for (int i = 0; i < reg_num; i++) {
		rand_num = get_rand(0, max_num);
		while (is_repeat(list, rand_num))
			rand_num = get_rand(0, max_num);
		list.push_back(rand_num);
	}
	pair<int, vector<int>>result;
	result.first = test_delay(netlist, net, list);
	result.second = list;
	return result;
}

int disturbance(int reg_num, vector<int>& reg_list,int max_reg) {
	int rand_num;
	int state = 0;
	for(int i=0;i<reg_list.size();i++)
		if (reg_list[i] == reg_num) {
			state = i;
			break;
		}
	int origin_num = reg_num;
	while (is_repeat(reg_list, reg_num) || reg_num<0 || reg_num>=max_reg) {
		if (reg_num >= max_reg || reg_num < 0)
			reg_num = origin_num;
		rand_num = get_rand(0, origin_delay);
		reg_num += rand_num - origin_delay / 2;
	}
	reg_list[state] = reg_num;
	return reg_num;
}

bool optimize(pair<int, vector<int>> result,library netlist,library net) {
	vector<int> optimized_result;
	port* interface;
	vector<port*> next;
	pair<port*, vector<port*>> reg;
	vector<pair<port*, vector<port*>>> reg_list;
	for (int i = 0; i < result.second.size(); i++) {
		interface = netlist.getport(result.second[i]);
		next = interface->getnext();
		for (int j = 0; j < next.size(); j++)
			interface->unassign(next[j]);
		reg.first = interface;
		reg.second = next;
		reg_list.push_back(reg);
	}
	net.clear_delay();
	cout << get_delay(net) << endl;
	for (int i = 0; i < result.second.size(); i++) {
		for (int j = 0; j < reg_list[i].second.size(); j++)
			reg_list[i].first->assign(reg_list[i].second[j]);
		net.clear_delay();
		if (get_delay(net) > result.first) {
			optimized_result.push_back(result.second[i]);
			for (int j = 0; j < reg_list[i].second.size(); j++)
				reg_list[i].first->unassign(reg_list[i].second[j]);
		}
	}
	net.clear_delay();
	cout << get_delay(net) << endl;
	result.second = optimized_result;
	cout << result.second.size() << endl;
	return true;
}

bool anneal(library netlist) {
	library net_start, net_path;
	pair<int, vector<int>> result, temp, origin_temp;
	vector<port*> next;
	double standard;
	origin_delay = get_delay(netlist);
	cout << origin_delay << endl;
	max_delay = origin_delay / balance;
	if (max_delay * balance < origin_delay)
		max_delay++;
	int reg_num = 0;
	int delay = max_delay;
	while (delay < origin_delay) {
		get_start_port(netlist, net_start, delay);
		reg_num += net_start.get_num();
		delay += max_delay;
	}
	get_start_port(netlist, net_start, max_delay);
	get_path_net(net_start, net_path);
	int max_reg = net_path.get_num();
	temp = result = rand_port(net_path, netlist, reg_num);
	while (temperature >= min_temperature) {
		origin_temp = temp;
		for (int i = 0; i < temp.second.size(); i++)
			disturbance(temp.second[i], temp.second, max_reg);
		temp.first = test_delay(net_path, netlist, temp.second);
		if (temp.first < result.first)
			result = temp;
		else if (temp.first > origin_temp.first) {
			standard = exp(-(temp.first - origin_temp.first) / temperature);
			if (((double)get_rand(0, 1000)) / 1000.0 >= standard)
				temp = origin_temp;
		}
		temperature *= ((double)get_rand(80, 99)) / 100.0;
	}
	cout << result.first <<" "<<result.second.size() << endl;
	optimize(result, net_path, netlist);
	return true;
}

int main() {
	srand(time(0));
	library net;
	library net1;
	if (rerun)
		getmodule("CGRA.v");
	opennetlist(net, "CGRA_element");
	interconnect(net, "CGRA_net");
	connect(net, net1);
	anneal(net1);
	return 0;
}