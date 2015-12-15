
#include "StringExtension.h"
#include <list>
#include <sstream>
#include <iostream>
#include <iomanip>

int StringExtension::stringSplit(std::vector<std::string>& v_string, std::string str, std::string separator, bool remove_void_entries) {

	std::list<std::string> l_string;
	l_string.clear();
	v_string.clear();

	//format the input string
	for (unsigned int i = 0 ; i< str.length() ; i++) if (str[i] == '\t') str.replace(i, 1, " ");;

	//loop on all substrings
	std::string::size_type stTemp = str.find(separator);
	
	while(stTemp != std::string::npos) {
		l_string.push_back(str.substr(0, stTemp));
		str = str.substr(stTemp + 1);
		stTemp = str.find(separator);
	}
	l_string.push_back(str);

	//remove blank spaces from the list if needed...
	if (remove_void_entries) {
		l_string.remove("");
		l_string.remove(" ");
		l_string.remove("\t");
	}

	//copy the list into the vetor
	v_string.resize(l_string.size());

	std::list<std::string>::iterator it;
	int i = 0;
	for (it = l_string.begin() ; it != l_string.end() ; it++) {
		v_string[i++] = (*it);
	}

	//and we're done !
	return v_string.size();

}

float StringExtension::stringToFloat(std::string str) {
	std::stringstream ss;
	float res;
	ss << str;
	ss >> res;
	return res;
}

std::string StringExtension::floatToString(float f, int fixed_width) {
	std::stringstream ss;
	if (fixed_width < 1) {
		ss << f;
	} else {
		ss << std::setiosflags(std::ios_base::fixed) << std::setprecision(3) << std::setw(fixed_width) << f;
	}
	return ss.str();
}

int StringExtension::stringToInt(std::string str) {
	std::stringstream ss;
	int res;
	ss << str;
	ss >> res;
	return res;
}

std::string StringExtension::intToString(int i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}

std::string StringExtension::formatTime(float milliseconds, bool textual) {

	//compute each quantity
	int milli = milliseconds;
	int hours = milli/3600000;
	int rest = milli - hours*3600000;
	int minutes = rest/60000;
	rest = rest - minutes*60000;
	int seconds = rest/1000;
	rest = rest - seconds*1000;
	milli = rest;

	//format string
	std::stringstream ss;
	if (textual) {
		if (hours > 0) ss << hours << "hr ";
		if (minutes > 0) ss << minutes << "mn ";
		if (seconds > 0) ss << seconds << "s ";
		ss << milli << "ms";
	} else {
		ss << std::setw(2) << std::setfill('0') << hours;
		ss << " : ";
		ss << std::setw(2) << std::setfill('0') << minutes;
		ss << " : ";
		ss << std::setw(2) << std::setfill('0') << seconds;
		ss << " : ";
		ss << std::setw(3) << std::setfill('0') << milli;
	}
	return ss.str();
}

bool StringExtension::getFileExtension(std::string& ext, std::string file_path) {
	int ind_pt = file_path.find_last_of(".");
	if (ind_pt == -1) return false;
	ext = file_path.substr(ind_pt + 1);
	return true;
}

std::string StringExtension::getFileNameFromPath(std::string file_path, bool keep_file_extension) {

	//find the last '/' or '\' separator
	int ind_pt1 = file_path.find_last_of("\\");
	int ind_pt2 = file_path.find_last_of("/");
	int ind_pt = ind_pt1;
	if (ind_pt2 > ind_pt1) ind_pt = ind_pt2;

	//keep only the file name
	std::string tmp = file_path.substr(ind_pt + 1, file_path.length());

	//remove optionnaly the extension
	if (keep_file_extension) return tmp;
	ind_pt = tmp.find_last_of(".");
	if (ind_pt == -1) return tmp;
	return tmp.substr(0, ind_pt);

}

std::string StringExtension::getPathFromFileName(std::string file_path) {

	//find the last '/' or '\' separator
	int ind_pt1 = file_path.find_last_of("\\");
	int ind_pt2 = file_path.find_last_of("/");
	int ind_pt = ind_pt1;
	if (ind_pt2 > ind_pt1) ind_pt = ind_pt2;

	if (ind_pt < 1) return "";

	//keep only the path
	std::string tmp = file_path.substr(0, ind_pt + 1);

	return tmp;

}