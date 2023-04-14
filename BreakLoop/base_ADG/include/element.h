#pragma once
#include<iostream>
#include<string>

using namespace std;

#define gib_num 8
#define track_type 4
#define element_max 50
#define max 50000 //栈的大小
#define spacer "/"//层级分隔符
#define space '/'


const string input_name[] = {"itrackW","itrackN","itrackE","itrackS","opinNE","opinNW","opinSW","opinSE"};
const string output_name[] = { "otrackW","otrackN","otrackE","otrackS","ipinNE","ipinNW","ipinSW","ipinSE" };

int num_track = 0;
int data_width = 0;
int element_id[element_max];
int element_num = 0;

class lut {
public:
	lut() {
		id = -1;
		input_num = output_num = track_type * num_track;
		input = new int[input_num];
		input_id = new int[input_num];
		output = new int[output_num];
		output_id = new int[output_num];
		for (int i = 0; i < track_type; i++)
			for (int j = 0; j < num_track; j++) {
				input[j + i * num_track] = output[j + i * num_track] = i;
				input_id[j + i * num_track] = output_id[j + i * num_track] = j;
			}
	}

	int getid() {
		return id;
	}

	int getinput(int num) {
		if (num >= input_num)
			return 0;
		return input[num];
	}

	int getinput_id(int num) {
		if (num >= input_num)
			return 0;
		return input_id[num];
	}

	int getoutput(int num) {
		if (num >= output_num)
			return 0;
		return output[num];
	}

	int getoutput_id(int num) {
		if (num >= output_num)
			return 0;
		return output_id[num];
	}

	bool changeid(int num) {
		id = num;
		return true;
	}

	bool addinput(int type, int num) {
		int* temp0 = input;
		int* temp1 = input_id;
		input_num++;
		input = new int[input_num];
		input_id = new int[input_num];
		if (temp0 != NULL && temp1 != NULL) {
			for (int i = 0; i < input_num - 1; i++) {
				input[i] = temp0[i];
				input_id[i] = temp1[i];
			}
			delete[] temp0;
			delete[] temp1;
		}
		input[input_num - 1] = type;
		input_id[input_num - 1] = num;
		return true;
	}

	bool addoutput(int type, int num) {
		int* temp0 = output;
		int* temp1 = output_id;
		output_num++;
		output = new int[output_num];
		output_id = new int[output_num];
		if (temp0 != NULL && temp1 != NULL) {
			for (int i = 0; i < output_num - 1; i++) {
				output[i] = temp0[i];
				output_id[i] = temp1[i];
			}
			delete[] temp0;
			delete[] temp1;
		}
		output[output_num - 1] = type;
		output_id[output_num - 1] = num;
		return true;
	}

	bool print() {
		cout << "ID:" << id << endl << "Input:";
		for (int i = 0; i < input_num; i++)
			cout << input[i] << "(" << input_id[i] << ")\t";
		cout << endl << "Output:";
		for (int i = 0; i < output_num; i++)
			cout << output[i] << "(" << output_id[i] << ")\t";
		cout << endl;
		return true;
	}

protected:
	int id;//查找表id
	int input_num;//输入端口数
	int* input;//各输入端口类型
	int* input_id;//各输入端口id
	int output_num;//输出端口数
	int* output;//各输出端口类型
	int* output_id;//各输出端口id
};

class port {
public:
	port() {
		sub_module = "";
		sub_id = -1;
		element = "";
		element_id = -1;
		type = "";
		id = -1;
		next = NULL;
		n_next = 0;
		ahead = NULL;
		n_ahead = 0;
		visited = -1;
	}

	port(string module_name, int module_id, string ele_name, int ele_id, string port_type, int port_id) {
		sub_module = module_name;
		sub_id = module_id;
		element = ele_name;
		element_id = ele_id;
		type = port_type;
		id = port_id;
	}

	bool print() {
		cout << "Module:" << sub_module << sub_id << "\tElement:" << element << element_id << "\tType:" << type << "\tId:" << id << "\tNext:" << n_next << endl;
		return true;
	}

	bool changesub_module(const string& s) {
		sub_module = s;
		return true;
	}

	bool changesub_id(int num) {
		sub_id = num;
		return true;
	}

	bool changeelement(const string& s) {
		element = s;
		return true;
	}

	bool changeelement_id(int num) {
		element_id = num;
		return true;
	}

	bool changetype(const string& s) {
		type = s;
		return true;
	}

	bool changeid(int num) {
		id = num;
		return true;
	}

	bool assigned(port* interface) {
		n_ahead++;
		port** temp = ahead;
		ahead = new port * [n_ahead];
		if (temp != NULL) {
			for (int i = 0; i < n_ahead - 1; i++)
				ahead[i] = temp[i];
			delete[] temp;
		}
		ahead[n_ahead - 1] = interface;
		return true;
	}
	//连接前置端口

	bool unassigned(port* interface) {
		port* temp = ahead[n_ahead - 1];
		for (int i = 0; i < n_ahead; i++)
			if (ahead[i] == interface) {
				ahead[--n_ahead] = NULL;
				ahead[i] = temp;
				return true;
			}
		return false;
	}
	//断开前置端口

	bool assign(port* interface) {
		n_next++;
		port** temp = next;
		next = new port * [n_next];
		if (temp != NULL) {
			for (int i = 0; i < n_next - 1; i++)
				next[i] = temp[i];
			delete[] temp;
		}
		next[n_next - 1] = interface;
		return interface->assigned(this);
	}
	//连接后续端口

	bool unassign(port* interface) {
		port* temp = next[n_next - 1];
		for (int i = 0; i < n_next; i++)
			if (next[i] == interface) {
				next[--n_next] = NULL;
				next[i] = temp;
				return interface->unassigned(this);
			}
		return false;
	}
	//断开后续端口

	bool changevisited(int num) {
		visited = num;
		return true;
	}

	bool getsub_module(string& s) {
		s = sub_module;
		return true;
	}

	int  getsub_id() {
		return true;
	}

	bool getelement(string& s) {
		s = element;
		return true;
	}

	int getelement_id() {
		return element_id;
	}

	bool gettype(string& s) {
		s = type;
		return true;
	}

	int getid() {
		return id;
	}

	port** getnext() {
		return next;
	}

	int getnum() {
		return n_next;
	}

	port** getahead() {
		return ahead;
	}

	int getahead_num() {
		return n_ahead;
	}

	int getvisited() {
		return visited;
	}

protected:
	string sub_module;
	int sub_id;
	string element;
	int element_id;
	string type;
	int id;
	port** next;
	int n_next;
	port** ahead;
	int n_ahead;
	int visited;
};

class library {
public:
	library() {
		num = 0;
		ports = NULL;
	}

	/*~library() {
		if (ports != NULL && num > 0) {
			delete[]ports;
		}
	}*/


	bool clear() {
		num = 0;
		delete[]ports;
		ports = NULL;
		return true;
	}

	bool repeat(string& s) {
		string name = "";
		for (int i = 0; i < num; i++) {
			ports[i]->getelement(name);
			if (name == s)
				return true;
		}
		return false;
	}

	bool print() {
		if (ports == NULL)
			return false;
		for (int i = 0; i < num; i++)
			ports[i]->print();
		return true;
	}

	int getnum() {
		return num;
	}

	port* getport(int n) {
		if (n >= num)
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

	bool wirte(port* interface) {
		num++;
		port** temp = ports;
		ports = new port * [num];
		if (temp != NULL)
			for (int i = 0; i < num - 1; i++)
				ports[i] = temp[i];
		ports[num - 1] = interface;
		if (temp != NULL)
			delete[] temp;
		return true;
	}
private:
	int num;
	port** ports;
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

lut* luts[element_max];
int lut_num = 0;