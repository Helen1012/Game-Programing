#pragma once

#include "stdafx.h"
using namespace std;

#define STUDENT_NUM 10

// �л� ����ü
struct student {
	string name;
	int score = 0;
};

// �л����� ������ ���� ������� ����
bool cmp(const student& p1, const student& p2) {
	if (p1.score > p2.score) return true;
	else return false;
}