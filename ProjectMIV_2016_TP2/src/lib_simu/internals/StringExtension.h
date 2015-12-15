
#ifndef STRING_EXTENSION_H
#define STRING_EXTENSION_H

#include <string>
#include <vector>

/*! \class StringExtension
 * \brief Uselful function for string an path manipulations
 */
class StringExtension {

public:

	/*!
     * \brief Split a string into an array of strings using a separator
	 * \param v_string : output array of strings
	 * \param str : the input string to be split
	 * \param separator : the separator used to split the string
	 * \param remove_void_entries : remove element of v_string that contain only blank spaces and/or tabulations
	 * \return the number of elements in v_string
     */
	static int stringSplit(std::vector<std::string>& v_string, std::string str, std::string separator, bool remove_void_entries = true);

	/*!
     * \brief Convert a string into a float
	 * \param str : the string to be converted
	 * \return the float value for the string
     */
	static float stringToFloat(std::string str);
	static std::string floatToString(float f, int fixed_width = -1);

	/*!
     * \brief Convert a string into a int
	 * \param str : the string to be converted
	 * \return the int value for the string
     */
	static int stringToInt(std::string str);
	static std::string intToString(int i);

	/*!
     * \brief Convert a float millisecnod value into a formatted string for time display
	 * \param milliseconds : the milliseconds float valu to be formatted
	 * \param textual : if true, formatted string will have the form ## hr ## min ## s ### ms, with 0 value not displayed. Otherwise it will have the form ## : ## : ## : ###, with zero values displayed
	 * \return the formatted string valuie representing time
     */
	static std::string formatTime(float milliseconds, bool textual = true);

	/*!
     * \brief Get the extension of the file, useful for file type checking.
	 * \param ext : output found extension
	 * \param file_path : the name or path of the file
	 * \return true if an extension has been found, false otherwise
     */
	static bool getFileExtension(std::string& ext, std::string file_path);

	/*!
     * \brief Extract the name of the file name from a full path
	 * \param file_path : the full path that include the file name
	 * \param keep_file_extension : whether or not to remove the extension of the file from the returned string
	 * \return the file name, without folders names.
     */
	static std::string getFileNameFromPath(std::string file_path, bool keep_file_extension = false);

	/*!
     * \brief Extract the path of a full file name 
	 * \param file_path : the full path that include the file name
	 * \return the path, without the file name
     */
	static std::string getPathFromFileName(std::string file_path);

};


#endif
