#pragma once
#include<iostream>
#include<string>

using namespace std;

#define max 50000 //ջ�Ĵ�С
#define spacer "/"//�㼶�ָ���
#define space '/'

class port {
public:

	port() {
		width = 0;
		port_name = "";
		name = "";
		net = "";
		type = "";
		visited = -1;
		next = NULL;
		n_next = 0;
		ahead = NULL;
		n_ahead = 0;
	}

	bool print() {
		cout << "Element:" << name << " Net:" << net << " Port:" << port_name << " Type:" << type << " Next_num:" << n_next << endl;
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

	bool changevisited(int i) {
		visited = i;
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

	int getvisited() {
		return visited;
	}

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
	//���Ӻ����˿�

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
	//�Ͽ������˿�

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
	//����ǰ�ö˿�

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
	//�Ͽ�ǰ�ö˿�

private:
	int n_next;//�����Ӻ����˿ڸ���
	int n_ahead;//������ǰ�ö˿���Ŀ
	int width;//λ��
	int visited;
	string net;//������������
	string type;//�˿�����
	port** next;//�����˿�ָ��
	port** ahead;//ǰ���˿�ָ��
	string port_name;//����˿�����
	string name;//������������
};
//�˿ڶ���

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
			ports[i]->getname(name);
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
//Ԫ���ⶨ��

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
	//���ջ

	bool empty() {
		if (top <= -1)
			return true;
		return false;
	}
	//�ж�ջ�Ƿ��

	bool full() {
		if (top >= msize - 1)
			return true;
		return false;
	}
	//�ж�ջ�Ƿ���

	bool push(port* m) {
		if (top >= msize - 1) {
			printf("Error in push:stack overflow!\n");
			return false;
		}
		i[++top] = m;
		return true;
	}
	//��ջ

	port* pop() {
		if (top <= -1) {
			printf("Error in pop:stack is empty!\n");
			return NULL;
		}
		return i[top--];
	}
	//��ջ

	port* gettop() {
		if (top <= -1) {
			printf("Error in pop:stack is empty!\n");
			return NULL;
		}
		return i[top];
	}
	//��ȡջ��Ԫ��

private:
	int msize;
	int top;
	port** i;
};
//ջ

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

int match(string P, string T) {//�Ӵ�P ԭ��T
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
//KMP�㷨

bool stringtoint(string input, int& output) {
	output = 0;
	for (int i = 0; i < input.length(); i++)
		output = output * 10 + input.at(i) - '0';
	return true;
}
//��ֻ�����ֵ��ַ���ת���ɶ�Ӧ������