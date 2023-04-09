#pragma once
#include<iostream>
#include<string>

using namespace std;

#define gib_num 8
#define track_type 4
#define element_max 50

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

lut* luts[element_max];
int lut_num = 0;