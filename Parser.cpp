

#include <Parser.h>
#include <iostream>

const std::string COMMENT_BLOCK_START_A = "{";
const std::string COMMENT_BLOCK_START_B = "[";
const std::string COMMENT_BLOCK_END_A   = "}";
const std::string COMMENT_BLOCK_END_B   = "]";
const std::string COMMENT_LINE_A        = "#";
const std::string COMMENT_LINE_B        = ";";
const std::string TOKEN_ASSIGNMENT_A    = "=";
const std::string TOKEN_ASSIGNMENT_B    = ":";
const std::string TOKEN_DELIMITER_A     = " ";
const std::string TOKEN_DELIMITER_B     = "\n";
const std::string TOKEN_DELIMITER_C     = "\t";
const std::string TOKEN_DELIMITER_D     = ",";
const std::string TOKEN_STRING_A        = "\"";
const std::string TOKEN_STRING_B        = "\'";

INIFile::INIFile()
:isGood(false)
,readingString(false)
{
}

INIFile::INIFile(const std::string& filename)
:isGood(false)
,readingString(false)
{
    isGood = Load(filename);
}
        
bool INIFile::Load(const std::string& filename, bool verbose)
{
    std::fstream ini_file;
    ini_file.open( filename.c_str(), std::ios::in | std::ios::binary );
    if( !ini_file.is_open() )
    {
        std::cout<<"ERROR Opening "<<filename <<"  file not created.\n";
        return false;
    }

    bool inBlockCommentState = false;
    bool inLineCommentState = false;
    std::string token_buffer;
    std::string token;
    std::string key;
    std::vector< std::string > values;


    char c = 0;

    c = 0;
    while( ini_file.good() )
    {
        //Are we entering a comments section?
        PeekChar( ini_file, c );
        
        //Are we reading a string value in?
        if( readingString )
        {
            token=""; token.push_back( c );
            //did we complete the string?
            if( token == TOKEN_STRING_A || token == TOKEN_STRING_B )
            {
                ReadChar( ini_file, c ); //munch the end token
                readingString = false;
                continue;
            }

            ReadChar( ini_file, c );
            token=""; token.push_back( c );
            token_buffer.push_back(c);  
            continue;
        }

        if( !inBlockCommentState && !inLineCommentState )
        {
            token=""; token.push_back( c );
            if( token == COMMENT_BLOCK_START_A || token == COMMENT_BLOCK_START_B )
            {
                inBlockCommentState = true;
                token_buffer = "";
            }
            else if( token == COMMENT_LINE_A || token == COMMENT_LINE_B )
            {
                inLineCommentState = true;
                token_buffer = "";
            }
        }

        if( inBlockCommentState )
        {
            ReadChar( ini_file, c );
            token=""; token.push_back( c );
            if( token == COMMENT_BLOCK_END_A || token == COMMENT_BLOCK_END_B )
            {
                inBlockCommentState = false;
            }
            continue;
        }

        if( inLineCommentState )
        {
            ReadChar( ini_file, c );
            token=""; token.push_back( c );
            if( token == "\n" )
            {
                inLineCommentState = false;
            }
            continue;
        }


        //Get our data
        token=""; 
        bool good = ReadChar( ini_file, c );	
		if(good)
			token.push_back( c );
		


        //Is it time to write a key or value?
        if( token == TOKEN_ASSIGNMENT_A || token == TOKEN_ASSIGNMENT_B )
        {
            if( !values.empty() )
                AddValue(key, values);

            if( token_buffer.empty() )
                continue;

            values.clear();
            values.resize(0);
            key = token_buffer;
            //std::cout<<"key: "<<token_buffer<<"\n";
            token_buffer = "";
        }
        else if( token == TOKEN_STRING_A || token == TOKEN_STRING_B )
        {
            //may only reach here if we're not reading a string and we hit the string token
            readingString = true;
            continue;
        }
        else if( token == TOKEN_DELIMITER_A || token == TOKEN_DELIMITER_B || token == TOKEN_DELIMITER_C || token == TOKEN_DELIMITER_D )
        {
        	char next = 0;
            if( PeekChar( ini_file, next ) )
            {
                token=""; token.push_back( next );
                if( token == TOKEN_DELIMITER_A || token == TOKEN_DELIMITER_B || token == TOKEN_DELIMITER_C || token == TOKEN_DELIMITER_D )
                {
                    continue;
                }
                else if( token == TOKEN_ASSIGNMENT_A || token == TOKEN_ASSIGNMENT_B )
                {
                    if( !values.empty() )
                        AddValue(key, values);

                    if( token_buffer.empty() )
                        continue;

                    values.clear();
                    values.resize(0);
                    key = token_buffer;
                    //std::cout<<"key: "<<token_buffer<<"\n";
                    token_buffer = "";
                    continue;   
                }
            }

            if( token_buffer.empty() )
                continue;

            values.push_back( token_buffer );
            //std::cout<<"value of size "<<token_buffer.size() <<" added: "<<token_buffer<<"\n";
            token_buffer = "";
        }
        else
        {
			if(good)
            	token_buffer.push_back(c);  
        }
    }   
    
    //don't forget to add final value
    if( !ini_file.good() && !token_buffer.empty() )
    {
        values.push_back( token_buffer );
        //std::cout<<"final value of size "<<token_buffer.size() <<" added: "<<token_buffer<<"\n";
        token_buffer = "";
        AddValue(key, values);
		values.clear();
    }

	if( !ini_file.good() && !values.empty() )
	{
		//std::cout<<"final key/value add performed!\n";
        AddValue(key, values);
		values.clear();
	}

	if(verbose)
	{
	    std::map< std::string, std::vector< std::string > >::iterator iter = dictionary.begin();
		unsigned int index = 0;
	    for(; iter != dictionary.end(); ++iter)
	    {
	        std::cout<<"item "<<iter->first<<": ";
	        for(size_t i = 0; i < iter->second.size(); ++i)
	        {
	            std::cout<<"i["<<index<<"]: "<<iter->second[i]<<" ";
	        }
			++index;
	        std::cout<<"\n";
	    }
	}

	isGood = true;
    return true;
}

bool INIFile::PeekChar(std::fstream& ini_file, char& c)
{
    if( ini_file.good() )
    {
        ini_file.get( c );
        ini_file.unget();
        return true;
    }
    return false;
}

bool INIFile::ReadChar(std::fstream& ini_file, char& c)
{
    if( ini_file.good() )
    {
        ini_file.get( c );
    	if( ini_file.good() )
        	return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<char>(const std::string& key, char& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        //value = Serialize( dictionary[ key ] );
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<char>(const std::string& key, char* value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        for(size_t i = 0; i < dictionary[ key ].size(); ++i)
        {
            std::stringstream data;
            data << dictionary[ key ][i];
            data >> value[i];
        }
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<int>(const std::string& key, int& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        //value = Serialize( dictionary[ key ] );
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<int>(const std::string& key, int* value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        for(size_t i = 0; i < dictionary[ key ].size(); ++i)
        {
            std::stringstream data;
            data << dictionary[ key ][i];
            data >> value[i];
        }
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<unsigned int>(const std::string& key, unsigned int& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<long>(const std::string& key, long& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<short>(const std::string& key, short& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<float>(const std::string& key, float& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<float>(const std::string& key, float* value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        for(size_t i = 0; i < dictionary[ key ].size(); ++i)
        {
            std::stringstream data;
            data << dictionary[ key ][i];
            data >> value[i];
        }
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<bool>(const std::string& key, bool& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

template <>
bool INIFile::GetValue<std::string>(const std::string& key, std::string& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

void INIFile::AddValue(const std::string& key, const std::string& value)
{
    if( dictionary.find( key ) == dictionary.end() )
    {
        std::vector<std::string> v; v.push_back( value );
        dictionary[ key ] = v;
    }
}

void INIFile::AddValue(const std::string& key, std::vector< std::string > value)
{
    if( dictionary.find( key ) == dictionary.end() )
    {
        dictionary[ key ] = value;
    }
}

