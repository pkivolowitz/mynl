#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <getopt.h>

using namespace std;

const int MAX_COMMENT_COLUMN = 120;
const int MAX_TAB = 40;

int comment_column = 72;
int tab_size = 4;

istream *fin = &cin;

struct LanguageDependence
{
	string language;
	string start_symbol;
	string end_symbol;
};

const LanguageDependence LD[] = 
{
	{
		"python",
		"#",
		""
	},

	{
		"c",
		"/*",
		"*/"
	},

	{
		"cpp",
		"//",
		""
	},

	{
		"LAST",
		"",
		"",
	}
};

/*	The program is designed to use standard input OR a file
	if the -f option is specified. `fin` is initialized to
	cin. `fin` is overwritten by this function.
*/
bool HandleOpen()
{
	extern char *optarg;
	extern istream *fin;

	bool retval = true;
	static filebuf file;

	file.open(optarg, ios::in);
	if (file.is_open())
	{
		fin = new istream(&file);
	}
	else
	{
		retval = false;
	}
	return retval;
}

void SetCommentSymbol(string &start_symbol, string &end_symbol)
{
	const LanguageDependence *ld = LD;

	while (ld->language != "LAST")
	{
		if (string(optarg) == ld->language)
		{
			start_symbol = ld->start_symbol;
			end_symbol = ld->end_symbol;
			break;
		}
	}
}

int main(int argc, char **argv)
{
	extern bool HandleOptions(int, char **, string &, string &, int &);
	extern int tab_size;
	
	string line;
	string start_symbol = "//";
	string end_symbol;
	int line_number = 1;
	size_t pos;

	if (!HandleOptions(argc, argv, start_symbol, end_symbol, line_number))
	{
		stringstream ss;
		string tab_replacement;
		ss << setw(tab_size) << " ";
		tab_replacement = ss.str();

		while (getline(*fin, line))
		{
			// Ensure ss is empty.
			ss.str(string());
			// Expand any tab characters found to spaces.
			while ((pos = line.find("\t")) != line.npos)
			{
				line.replace(pos, 1, tab_replacement);
			}
			ss << setw(comment_column) << left << line;

			line = ss.str();

			// Ensure lines that are too long have a space before appending line numbers.
			if (line.size() > comment_column && line.back() != ' ')
				line.push_back(' ');

			cout << line << start_symbol << " " << line_number << " " << end_symbol << endl;
			line_number++;
		}
	}
	return 0;
}

void Help()
{
	cout << "mynl - a tool to add TRAILING line numbers suitable for preparing\n";
	cout << "code or other text  for inclusion in documentation or code blocks\n";
	cout << "such as those used in github markdown.\n\n";
	cout << "option    argument    meaning\n";
	cout << "c         int         column at which line numbers will be added\n";
	cout << "l         int         starting line number (defaults to 1)\n";
	cout << "s         string      specifies symbols for comments (see below)\n";
	cout << "t         int         tab size (defaults to 4)\n";
	cout << "h         none        prints this help and exits\n";
	cout << endl;
	cout << "supported languages (strings to follow \"-l\")\n";
	cout << "python, c, cpp\n\n";
	cout << "If a line size is long, a  space  will be added if needed to set\n";
	cout << "the line number.\n\n ";
}

/*	A safer alternative to atoi. If the conversion to int fails,
	bad_value will be returned after swallowing the exception.
	bad_value defaults to -1.
*/
int SaferATOI(char * cstring, int bad_value = -1)
{
	int retval;
	try
	{
		retval = stoi(string(optarg));
	}
	catch(const std::exception& e)
	{
		retval = bad_value;
	}
	return retval;
}

bool HandleOptions(int argc, char **argv, string &start_symbol, string &end_symbol, int &line_number)
{
	extern bool HandleOpen();
	extern void Help();
	extern void SetCommentSymbol(string &, string &);
	extern int tab_size;

	bool force_exit = false;
	int c;

	while ((c = getopt(argc, argv, "hc:f:l:s:t:")) != -1)
	{
		switch (c)
		{
		case 'l':
			line_number = SaferATOI(optarg);
			break;

		case 't':
			tab_size = SaferATOI(optarg);
			if (tab_size < 0 || tab_size >= MAX_TAB)
			{
				cerr << "Bad tab size. Minimum is 0 and maximum is " << MAX_TAB << endl;
				force_exit = true;
			}
			break;

		case 's':
			SetCommentSymbol(start_symbol, end_symbol);
			break;

		case 'f':
			if (!HandleOpen())
			{
				cerr << "File: " << optarg << " could not be opened.\n";
				perror(optarg);
				force_exit = true;
			}
			break;

		case 'c':
			comment_column = SaferATOI(optarg);
			if (comment_column <= 0 || comment_column > MAX_COMMENT_COLUMN)
			{
				cerr << "Invalid comment column: " << comment_column << endl;
				cerr << "Must be in range of 1 to " << MAX_COMMENT_COLUMN << endl;
				force_exit = true;
			}
			break;

		case 'h':
			Help();
			force_exit = true;
			break;
		}
	}
	return force_exit;
}
