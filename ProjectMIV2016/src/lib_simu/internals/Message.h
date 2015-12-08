
#ifndef MESSAGE_H
#define MESSAGE_H

#include <list>
#include <string>

/*! \class Message
 * \brief Listener that can be plugged to the message and called by it for each displayed message
 */
class MessageListener {

public:

	//see Message class for description of the methods
	virtual void message(std::string text, int level, std::string title) {}
	virtual void error(std::string text, std::string title) {}
	virtual void fatal_error(std::string text, std::string title) {}
	virtual void debug(std::string text, std::string title) {}

};

/*! \class Message
 * \brief Class with static member
 */
class Message {

protected:

	static int info_level;			 /*!< level of message to output. each message that has a level <= info_level is displayed. Default value = 2 */
	static bool console_output;		 /*!< whether the messages are displayed into the console before calling the listeners. Useful when no listeners are implemented. Default to true */

	static std::list<MessageListener*> listeners;	/*!< List of listeners of the application messages */

public:

	static void addListener(MessageListener* listener);
	static void removeListener(MessageListener* listener);

	/*!
     *  \brief Output a classical message with optionnal level info
	 * \param text : the text of the message
	 * \param level : the level of importance of the message. If level <= Message::info_level, the message won't be displayed
	 * \param title : optionnal, a title for the message
     */
	static void message(std::string text, int level = 2, std::string title = "");

	/*!
     *  \brief Error message
     */
	static void error(std::string text, std::string title = "");

	/*!
     *  \brief Fatal error message. Should cause the stop of the application or a part of the application.
     */
	static void fatal_error(std::string text, std::string title = "");

	/*!
     *  \brief Debug output that has no effect in release mode
     */
#ifdef _DEBUG
	static void debug(std::string text, std::string title = "");
#else
	inline static void debug(std::string text, std::string title = "") {}
#endif

	/*!
     * set the output level for messages
     */
	inline static void setOutputLevel(int level) { info_level = level; }

	/*!
     * set the console output mode
     */
	inline static void setConsoleOutput(bool enable) { console_output = enable; }

};


#endif