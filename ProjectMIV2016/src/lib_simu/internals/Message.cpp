
#include "Message.h"
#include <iostream>
#include <string>

void Message::message(std::string text, int level, std::string title) {

	//check display level
	if (level > info_level) return;

	//optionnal output to the console
	if (console_output) {
		std::string prefix = "INFO: ";
		if (title.compare("") != 0) prefix += "(" + title + ") - ";
		std::cout << prefix << text << std::endl;
	}

	//warn all the listeners
	std::list<MessageListener*>::iterator it;
	for (it = listeners.begin() ; it != listeners.end() ; it++) (*it)->message(text, level, title);

}

void Message::error(std::string text, std::string title) {

	//optionnal output to the console
	if (console_output) {
		std::string prefix = "ERROR: ";
		if (title.compare("") != 0) prefix += "(" + title + ") - ";
		std::cout << prefix << text << std::endl;
	}

	//warn all the listeners
	std::list<MessageListener*>::iterator it;
	for (it = listeners.begin() ; it != listeners.end() ; it++) (*it)->error(text, title);

}

void Message::fatal_error(std::string text, std::string title) {

	//optionnal output to the console
	if (console_output) {
		std::string prefix = "FATAL ERROR: ";
		if (title.compare("") != 0) prefix += "(" + title + ") - ";
		std::cout << prefix << text << std::endl;
	}

	//warn all the listeners
	std::list<MessageListener*>::iterator it;
	for (it = listeners.begin() ; it != listeners.end() ; it++) (*it)->fatal_error(text, title);

}


#ifdef _DEBUG

void Message::debug(std::string text, std::string title) {

	//optionnal output to the console
	if (console_output) {
		std::string prefix = "DEBUG: ";
		if (title.compare("") != 0) prefix += "(" + title + ") - ";
		std::cout << prefix << text << std::endl;
	}

	//warn all the listeners
	std::list<MessageListener*>::iterator it;
	for (it = listeners.begin() ; it != listeners.end() ; it++) (*it)->debug(text, title);

}

#endif

//------------------------------------------------------------------------
// addListener
//------------------------------------------------------------------------
void Message::addListener( MessageListener* listener )
{
	listeners.push_back(listener);
}

void Message::removeListener(MessageListener* listener) {
	listeners.remove(listener);
}


//static members
int Message::info_level = 2;
bool Message::console_output = true;
std::list<MessageListener*> Message::listeners;
