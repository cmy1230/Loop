#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<cstdlib>
#include<ctime>
#include<utility>

using namespace std;

#define max 50000 //栈的大小
#define spacer "/"//层级分隔符
#define space '/'
#define balance 4//平衡参数
#define origin_temperature 10000
#define min_temperature 10

double temperature = origin_temperature;
int max_delay = 5;
int origin_delay;

class port {
public:
	port() {
		width = 0;
		port_name = "";
		name = "";
		net = "";
		type = "";
		pose_delay = -1;
		pre_delay = -1;
		delay = 0;
	}

	bool print() {
		cout << "Element:" << name << " Port:" << port_name << " Type:" << type << " Next_num:" << next.size() <<" Ahead_Num:"<<ahead.size() << " Delay:" << delay << " Pose_delay:" << pose_delay << " Pre_delay:" << pre_delay << endl;
		return true;
	}

	bool changeport_name(const string s) {
		port_name = s;
		return true;
	}

	bool changename(const string s) {
		name = s;
		return true;
	}

	bool changenet(const string s) {
		net = s;
		return true;
	}

	bool changetype(const string s) {
		type = s;
		return true;
	}

	bool changewidth(int n) {
		width = n;
		return true;
	}

	bool change_pose_delay(int i) {
		pose_delay = i;
		return true;
	}

	bool change_pre_delay(int i) {
		pre_delay = i;
		return true;
	}

	bool changedelay(int i) {
		delay = i;
		return true;
	}

	bool getname(string& s) {
		s = name;
		return true;
	}

	bool gettype(string& s) {
		s = type;
		return true;
	}

	bool getnet(string& s) {
		s = net;
		return true;
	}

	bool getport_name(string& s) {
		s = port_name;
		return true;
	}

	int getwidth() {
		return width;
	}

	int get_pose_delay() {
		return pose_delay;
	}

	int get_pre_delay() {
		return pre_delay;
	}

	int getdelay() {
		return delay;
	}

	bool assign(port* interface) {
		next.push_back(interface);
		return interface->assigned(this);
	}
	//连接后续端口

	bool unassign(port* interface) {
		vector<port*> temp = next;
		next.clear();
		bool get_port = false;
		for (int i = 0; i < temp.size(); i++)
			if (temp[i] == interface) {
				get_port = true;
				continue;
			}
			else
				next.push_back(temp[i]);
		if (get_port)
			return interface->unassigned(this);
		return false;
	}
	//断开后续端口

	vector<port*> getnext() {
		return next;
	}

	int get_next_num() {
		return next.size();
	}

	vector<port*> getahead() {
		return ahead;
	}

	int get_ahead_num() {
		return ahead.size();
	}

	bool assigned(port* interface) {
		ahead.push_back(interface);
		return true;
	}
	//连接前置端口

	bool unassigned(port* interface) {
		vector<port*> temp = ahead;
		ahead.clear();
		for (int i = 0; i < temp.size(); i++)
			if (temp[i] == interface)
				continue;
			else
				ahead.push_back(temp[i]);
		return false;
	}
	//断开前置端口

	bool next_assign(port* interface) {
		string type_temp;
		interface->gettype(type_temp);
		if (type_temp[0] != 'J')
			return false;
		vector<port*> temp;
		for (int i = 0; i < next.size(); i++)
			if (next[i] = interface)
				for (int j = 0; j < interface->get_next_num(); j++) {
					interface->getnext()[j]->unassigned(interface);
					temp.push_back(interface->getnext()[j]);
				}
			else
				temp.push_back(next[i]);
		next.clear();
		for (int i = 0; i < temp.size(); i++)
			assign(temp[i]);
		return true;
	}

private:
	int width;//位宽
	int pose_delay;
	int pre_delay;
	int delay;
	string net;//所处网络名称
	string type;//端口类型
	vector<port*> next;
	vector<port*> ahead;//前方端口指针
	string port_name;//具体端口名称
	string name;//所属器件名称
};
//端口定义

class library {
public:

	/*~library() {
		if (ports != NULL && num > 0) {
			delete[]ports;
		}
	}*/

	bool clear() {
		ports.clear();
		return true;
	}

	bool repeat(port* interface) {
		for (int i = 0; i < ports.size(); i++)
			if (interface == ports[i])
				return true;
		return false;
	}

	bool print() {
		for (int i = 0; i < ports.size(); i++)
			ports[i]->print();
		return true;
	}

	int get_num() {
		return ports.size();
	}

	port* getport(int n) {
		if (n >= ports.size())
			return NULL;
		return ports[n];
	}

	/*bool deleteport(int n) {
		if (n >= num)
			return false;
		num--;
		ports[n] = ports[num];
		/*port* temp = ports;
		ports = new port[num];
		int j = 0;
		if (temp != NULL)
			for (int i = 0; i < num + 1; i++)
				if (i == n)
					continue;
				else
					ports[j++] = temp[i];
		delete[] temp;
		return true;
	}*/

	bool clear_delay() {
		for (int i = 0; i < ports.size(); i++) {
			ports[i]->change_pose_delay(-1);
			ports[i]->change_pre_delay(-1);
		}
		return true;
	}

	bool wirte(port* interface) {
		ports.push_back(interface);
		return true;
	}
private:
	vector<port*> ports;
};
//元件库定义

class stack {
public:
	stack() {
		msize = max;
		top = -1;
		i = new port * [max];
	}

	void clear() {
		msize = max;
		top = -1;
		return;
	}
	//清空栈

	bool empty() {
		if (top <= -1)
			return true;
		return false;
	}
	//判断栈是否空

	bool full() {
		if (top >= msize - 1)
			return true;
		return false;
	}
	//判断栈是否满

	bool push(port* m) {
		if (top >= msize - 1) {
			printf("Error in push:stack overflow!\n");
			return false;
		}
		i[++top] = m;
		return true;
	}
	//入栈

	port* pop() {
		if (top <= -1) {
			printf("Error in pop:stack is empty!\n");
			return NULL;
		}
		return i[top--];
	}
	//出栈

	port* gettop() {
		if (top <= -1) {
			printf("Error in pop:stack is empty!\n");
			return NULL;
		}
		return i[top];
	}
	//获取栈顶元素

private:
	int msize;
	int top;
	port** i;
};
//栈

int* buildnext(string P) {
	int m = P.length();
	int j = 0;
	int* N = new int[m];
	int t = N[0] = -1;
	while (j < m - 1)
		if (t < 0 || P[j] == P[t]) {
			j++;
			t++;
			N[j] = t;
		}
		else
			t = N[t];
	return N;
}

int match(string P, string T) {//子串P 原串T
	int* next = buildnext(P);
	int n = T.length();
	int m = P.length();
	int i = 0, j = 0;
	while (j < m && i < n)
		if (0 > j || T[i] == P[j]) {
			i++;
			j++;
		}
		else
			j = next[j];
	delete[] next;
	if (j >= m)
		return i - j;
	else
		return -1;
}
//KMP算法

bool stringtoint(string input, int& output) {
	output = 0;
	for (int i = 0; i < input.length(); i++)
		output = output * 10 + input.at(i) - '0';
	return true;
}
//将只含数字的字符串转化成对应的整型