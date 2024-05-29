#include "d:/Cloud project/Additional/my_project_header.h"

std::string configLocation(const char *funcName, const char *fileName) 
{
	return "In file: " + std::string(fileName) + " in function: " + std::string(funcName);
}


void Local::removeStartEndSpaces(std::string &str)
{
	int idx = 0;
	for (idx; idx < str.size(); idx++)
	{
		if (str[idx] == ' ')
			continue;
		else
			break;
	}
	str = str.substr(idx, str.size() - idx);
    for (idx = str.size() - 1; idx >= 0; idx--)
    {
        if (str[idx] == ' ')
            continue;
        else
            break;
    }
    str = str.substr(0, idx + 1);
}

std::string Local::toStringFillZeroes(int number, int totalStringSize)
{
    size_t numberOfDigits = 0;
    // get num of digits of the size to fill up zeroes.
    numberOfDigits = (number > 0) ? (size_t)log10(number) : 0;
    numberOfDigits++;
    // If the number of digits is larger than totalStringSize throw an error.
    if (numberOfDigits > totalStringSize && totalStringSize == 9)
        throw CommunicationError("Length of message it too long", MY_LOCATION);
    // convert number to a string
    std::string strNum = std::to_string(number);
    // Fill up with zeroes
    strNum.insert(0, totalStringSize - numberOfDigits, '0');
    return strNum;
}


