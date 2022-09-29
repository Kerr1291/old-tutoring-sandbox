/*
//
// Parses files and creates a map of key-value pairs
// Data may be retrieved by calling GetValue( key, data )
// If the data is a user define class then the method Serialize( std::vector< std::string > )
// must be written to handle the conversion from string data.
// GetValue returns true if the key was found, false otherwise.
//
// USAGE EXAMPLES:
// 
//
//    INIFile test("assets/config.ini");
//
//
//    int           result1; 
//    unsigned int  result2;
//    std::string   result3;
//    float         result4[3];
//
//
//    test.GetValue("SomeIntData", result1);
//    test.GetValue("SomeUIntData", result2);
//    test.GetValue("SomeStringData", result3);
//    test.GetValue("ThisIsAnArrayOfFloats", result4);
//
//
*/

#ifndef PARSER_H
#define PARSER_H
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

class INIFile
{
    public:

        INIFile();
        INIFile(const std::string& filename);

		inline bool HasValue(const std::string& key);

        template <typename T>
        bool GetValue(const std::string& key, T& value);

        template <typename T>
        bool GetValue(const std::string& key, T* value);

        template <typename T>
        bool GetValue(const std::string& key, std::vector<T>& value);

        bool Load(const std::string& filename, bool verbose = false);
        bool IsGood() { return isGood; }

    private:
        void AddValue(const std::string& key, const std::string& value);
        void AddValue(const std::string& key, std::vector< std::string > value);
        bool PeekChar(std::fstream& ini_file, char& c);
        bool ReadChar(std::fstream& ini_file, char& c);

        bool isGood;
        bool readingString;
        std::map< std::string, std::vector< std::string > > dictionary;
};
		
inline bool INIFile::HasValue(const std::string& key)
{
    if( dictionary.find( key ) != dictionary.end() )
        return true;
    return false;
}

/////////////////////////////////////////////////////////
// Specializations for POD or other simple data types
template <>
bool INIFile::GetValue<char>(const std::string& key, char& value);
template <>
bool INIFile::GetValue<int>(const std::string& key, int& value);
template <>
bool INIFile::GetValue<unsigned int>(const std::string& key, unsigned int& value);
template <>
bool INIFile::GetValue<long>(const std::string& key, long& value);
template <>
bool INIFile::GetValue<short>(const std::string& key, short& value);
template <>
bool INIFile::GetValue<float>(const std::string& key, float& value);
template <>
bool INIFile::GetValue<bool>(const std::string& key, bool& value);
template <>
bool INIFile::GetValue<std::string>(const std::string& key, std::string& value);

template <>
bool INIFile::GetValue<float>(const std::string& key, float* value);
template <>
bool INIFile::GetValue<int>(const std::string& key, int* value);
template <>
bool INIFile::GetValue<char>(const std::string& key, char* value);
//
/////////////////////////////////////////////////////////

template <typename T>
bool INIFile::GetValue(const std::string& key, std::vector<T>& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        for(size_t i = 0; i < dictionary[ key ].size(); ++i)
        {
            value.push_back( dictionary[ key ][i] );
        }
        return true;
    }
    return false;
}


template <typename T>
bool INIFile::GetValue(const std::string& key, T& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        value.Serialize( dictionary[ key ] );
        return true;
    }
    return false;
}

template <typename T>
bool INIFile::GetValue(const std::string& key, T* value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        value->Serialize( dictionary[ key ] );
        return true;
    }
    return false;
}



#endif
