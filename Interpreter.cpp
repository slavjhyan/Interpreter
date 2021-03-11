#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<cctype>

void Alloc(const std::string&, int, int&, std::ifstream&, std::vector<std::string>&, std::vector<int>&, int&);
void Output(const std::string&, int, int&, std::vector<std::string>&, std::vector<int>&);
void VariableAssignment(const std::string&, int, std::vector<int>&, std::vector<std::string>&, std::string, int);
void Check(std::string&, int&, int, std::vector<std::string>&, std::vector<int>&, std::ifstream&, int&);
void Anew(std::string&, int&, int, std::vector<std::string>&, std::vector<int>&, std::ifstream&, int&);

int lineCount = 0;
bool checkVal = false;

int main()
{
	std::ifstream inFile("myCode.cmm");
	std::string input;
	std::ifstream keywords("tokens.txt");
	std::vector<std::string> variableNames;
	std::vector<int> variableValues;
	std::string line;
	int variableCount = 0;
	while (inFile >> input)
	{
		if (input != ";")
		{
			line = line + input;
			line.push_back(' ');
		}
		else
		{
			int i = 0;
			line.push_back(';');
			bool lineOver = true;
			while (line[i] != ';' && lineOver)
			{
				int k = i;
				while (line[k] != ' ' && line[k] != '(')
				{
					++k;
				}
				std::string kword = line.substr(i, k - i);
				i = k + 1;
				if (kword == "alloc")
				{
					Alloc(line, lineCount, i, keywords, variableNames, variableValues, variableCount);
					break;
				}
				if (kword == "output")
				{
					Output(line, lineCount, i, variableNames, variableValues);
					break;
				}
				if (kword == "check")
				{
					Check(line, lineCount, i, variableNames, variableValues, keywords, variableCount);
					break;
				}
				if (kword == "anew")
				{
					Anew(line, lineCount, i, variableNames, variableValues, keywords, variableCount);
					break;
				}
				for (int i = 0; i < variableCount; ++i)
				{
					if (kword == variableNames[i])
					{
						VariableAssignment(line, i, variableValues, variableNames, kword, lineCount);
						lineOver = false;
						break;
					}
				}
			}
			line.clear();
		}
	}
}

void Alloc(const std::string& line, int lineCount, int& i, std::ifstream& keywords, std::vector<std::string>& variableNames, std::vector<int>& variableValues, int& variableCount)
{
	++lineCount;
	std::string varName;
	int j = i;
	while (((line[j] != ' ') && (line[j] != '<' && line[j + 1] != '-')) && line[j] != ';')
	{
		++j;
	}
	varName = line.substr(i, j - i);
	i = j + 1;
	std::string invalidVarName;
	while (keywords >> invalidVarName)
	{
		if (varName == invalidVarName)
		{
			std::cout << std::endl;
			std::cerr << "Line " << lineCount << ": Cannot use " << varName << " as a variable name.\nEnding process..." << std::endl;
			exit(1);
		}
	}
	for (int i = 0; i < variableNames.size(); ++i)
	{
		if (variableNames[i] == varName)
		{
			std::cout << std::endl;
			std::cerr << "Line " << lineCount << ": Redefinition of variable '" << varName << "'.\nEnding process..." << std::endl;
			exit(1);
		}
	}
	variableNames.push_back(varName);
	++variableCount;
	variableValues.push_back(INT_MIN);
	while (line[j] != '<')
	{
		if (line[j] == ';')
			return;
		++j;
	}
	if (line[++j] == '-')
	{
		while ((line[++j] - '0') < 0 || (line[++j] - '0') > 9)
		{
			if (line[j] == ';')
			{
				std::cout << std::endl;
				std::cerr << "Line " << lineCount << ": Didn't give argument to assignment operator" << "'.\nEnding process..." << std::endl;
				exit(1);
			}
		}
		--j;
		int jx = j;
		std::string n;
		int nx = 0;
		while (((line[j] - '0') >= 0) && ((line[j] - '0') <= 9))
		{
			++nx;
			++j;
		}
		n = line.substr(jx, jx + nx);
		std::stringstream num(n);
		int value;
		num >> value;
		variableValues[variableCount - 1] = value;
		return;
	}
	std::cout << std::endl;
	std::cerr << "Line " << lineCount << ": Missing '-' after '<' " << "'.\nEnding process..." << std::endl;
	exit(1);
}

void Output(const std::string& line, int i, int& lineCount, std::vector<std::string>& variableNames, std::vector<int>& variableValues)
{
	++lineCount;
	++i;
	while (line[i] != '(')
	{
		if (line[i] == ';')
		{
			std::cout << std::endl;
			std::cerr << "Line " << lineCount << ": missing '(' after 'output'.\nEnding process..." << std::endl;
			exit(1);
		}
		++i;
	}
	std::string output;
	int j = ++i;
	if (line[j] == '"')
	{
		++j;
		while (line[j] != '"')
		{
			std::cout << line[j];;
			++j;
			if (line[j] == ';')
			{
				std::cout << std::endl;
				std::cerr << "Line " << lineCount << ": missing '\34' in output.\nEnding process..." << std::endl;
				exit(1);
			}
		}
		std::cout << std::endl;
	}
	else
	{
		++j;
		while (line[j] != ')')
		{
			++j;
			if (line[j] == ';')
			{
				std::cout << std::endl;
				std::cerr << "Line " << lineCount << ": missing ')' in output.\nEnding process..." << std::endl;
				exit(1);
			}
		}
		std::string posVarName;
		posVarName = line.substr(i, (j - i));
		if (isdigit(posVarName[0]))
		{
			int outputVal;
			std::stringstream val(posVarName);
			val >> outputVal;
			std::cout << outputVal << std::endl;
			return;
		}
		for (int i = 0; i < variableNames.size(); ++i)
		{
			if (variableNames[i] == posVarName)
			{
				if (variableValues[i] != INT_MIN)
				{
					std::cout << variableValues[i] << std::endl;
					return;
				}
				else
				{
					std::cout << std::endl;
					std::cerr << "Line " << lineCount << ": " << posVarName << " variable was not initialized.\nEnding process..." << std::endl;
					exit(1);
				}
			}
		}
		std::cout << std::endl;
		std::cerr << "Line " << lineCount << ": " << posVarName << " variable not found.\nEnding process..." << std::endl;
		exit(1);
	}
}

void VariableAssignment(const std::string& line, int lhsIndex, std::vector<int>& variableValues, std::vector<std::string>& variableNames, std::string lhsName, int lineCount)
{
	++lineCount;
	int j = 0;
	while (line[j] != '<' && line[j + 1] != '-')
	{
		++j;
	}
	j += 2;
	while (line[++j] == ' ') {}
	std::vector<std::string> rhs;
	while (line[j] != ';')
	{
		int tempLength = 0;
		while (line[j] != ' ')
		{
			++tempLength;
			++j;
		}
		rhs.push_back(line.substr(j - tempLength, tempLength));
		++j;
	}
	if (rhs.size() != 1)
	{
		int val, val1, val2;
		if (isdigit(rhs[0][0]))
		{
			std::stringstream num1(rhs[0]);
			num1 >> val1;
		}
		else
		{
			bool isInvalid = true;
			for (int i = 0; i < variableNames.size(); ++i)
			{
				if (rhs[0] == variableNames[i])
				{
					val1 = variableValues[i];
					isInvalid = false;
					break;
				}
			}
			if (isInvalid)
			{
				std::cerr << "Line " << lineCount << ": " << val1 << "variable not initialized.\nEnding process..." << std::endl;
				exit(1);
			}
		}
		if (isdigit(rhs[2][0]))
		{
			std::stringstream num2(rhs[2]);
			val2;
			num2 >> val2;
		}
		else
		{
			bool isInvalid2 = false;
			for (int i = 0; i < variableNames.size(); ++i)
			{
				if (rhs[2] == variableNames[i])
				{
					val2 = variableValues[i];
					isInvalid2 = false;
					break;
				}
			}
			if (isInvalid2)
			{
				std::cerr << "Line " << lineCount << ": " << val2 << "variable not initialized.\nEnding process..." << std::endl;
				exit(1);
			}
		}
		char ch = rhs[1][0];
		switch (ch)
		{
		case '+':
			val = val1 + val2; break;
		case '-':
			val = val1 - val2; break;
		case '*':
			val = val1 * val2; break;
		case '/':
			if (val2 != 0)
			{
				val = val1 / val2;
			}
			else
			{
				std::cerr << "Line " << lineCount << ": can't divide by zero.\nEnding process..." << std::endl;
				exit(1);
			}
			break;
		default:
			std::cerr << "Line " << lineCount << ": invalid operator.\nEnding process..." << std::endl;
			exit(1);
		}
		variableValues[lhsIndex] = val;
	}
	else
	{
		int rhsIndex;
		for (int i = 0; i < variableNames.size(); ++i)
		{
			if (rhs[0] == variableNames[i])
			{
				rhsIndex = i;
				goto x;
			}
		}
		std::cout << std::endl;
		std::cerr << "Line " << lineCount << ": " << rhs[0] << " variable not found.\nEnding process..." << std::endl;
		exit(1);

	x:
		for (int i = 0; i < variableNames.size(); ++i)
		{
			if (lhsName == variableNames[i])
			{
				variableValues[lhsIndex] = variableValues[rhsIndex];
				return;
			}
		}
	}
}

void Check(std::string& line, int& lineCount, int i, std::vector<std::string>& variableNames, std::vector<int>& variableValues, std::ifstream& keywords, int& variableCount)
{
	++lineCount;
	while (line[i++] != '(')
	{
		if (line[i] == ';')
		{
			std::cout << std::endl;
			std::cerr << "Line " << lineCount << ": missing '(' after 'check'.\nEnding process..." << std::endl;
			exit(1);
		}
	}
	int j = i - 1;
	std::vector<std::string> condition;
	while (line[j] != ')')
	{
		++j;
		int tokenLength = 0;
		while (line[j] != ' ' && line[j] != ')')
		{
			++tokenLength;
			++j;
		}
		condition.push_back(line.substr(j - tokenLength, tokenLength));
	}
	bool isInvalid = true;
	int val1, val2;
	if (isdigit(condition[0][0]))
	{
		std::stringstream num1(condition[0]);
		num1 >> val1;
	}
	else
	{
		for (int i = 0; i < variableNames.size(); ++i)
		{
			if (condition[0] == variableNames[i])
			{
				val1 = variableValues[i];
				isInvalid = false;
				break;
			}
		}
		if (isInvalid)
		{
			std::cerr << "Line " << lineCount << ": " << condition[0] << "variable not initialized.\nEnding process..." << std::endl;
			exit(1);
		}
	}
	if (isdigit(condition[2][0]))
	{
		std::stringstream num2(condition[2]);
		num2 >> val2;
	}
	else
	{
		bool isInvalid2 = false;
		for (int i = 0; i < variableNames.size(); ++i)
		{
			if (condition[2] == variableNames[i])
			{
				val2 = variableValues[i];
				isInvalid2 = false;
				break;
			}
		}
		if (isInvalid2)
		{
			std::cerr << "Line " << lineCount << ": " << condition[2] << "variable not initialized.\nEnding process..." << std::endl;
			exit(1);
		}
	}
	char ch = condition[1][0];
	switch (ch)
	{
	case '>':
		if (val1 > val2) checkVal = true;
		break;

	case '<':
		if (val1 < val2) checkVal = true;
		break;

	case '=':
		if (val1 == val2) checkVal = true;
		break;

	default:
		std::cerr << "Line " << lineCount << ": invalid relational operator.\nEnding process..." << std::endl;
		exit(1);
	}

	if (checkVal)
	{
		while (line[++j] != '{') {}
		++j;
		int ix = j;
		while (line[j++] != '}' && line[j] != ';')
		{
			std::string keyword;
			int k = 0;
			while (line[j] != ' ' && line[j] != '(')
			{
				++k;
				++j;
			}
			keyword = line.substr(j - 1 - k, k);
			bool lineOver = true;
			switch (keyword[0])
			{
			case 'a':
				Alloc(line, lineCount, ix, keywords, variableNames, variableValues, variableCount);
				break;

			case 'o':
				Output(line, ix, lineCount, variableNames, variableValues);
				break;
			default:
				for (int i = 0; i < variableCount; ++i)
				{
					if (keyword == variableNames[i])
					{
						VariableAssignment(line, ix, variableValues, variableNames, keyword, lineCount);
						lineOver = false;
						break;
					}
				}
				break;
			}
			++j;
			return;
		}
	}
}

void Anew(std::string& line, int& linecount, int i, std::vector<std::string>& variableNames, std::vector<int>& variableValues, std::ifstream& keywords, int& variableCount)
{
	++lineCount;
	int j = i - 1;
	std::string condition;
	++j;
	int conditionLength = 0;
	while (line[j] != ')')
	{
		++conditionLength;
		++j;
	}
	condition = line.substr(j - conditionLength, conditionLength);
	int val = 0;
	if (condition[0] - '0' >= 0 && condition[0] - '0' <= 9)
	{
		std::stringstream num(condition);
		num >> val;
		if (val - (int)val != 0 || val <= 0)
		{
			std::cerr << "Line " << linecount << ": anew condition must be a natural number.\nEnding process..." << std::endl;
			exit(1);
		}
	}
	else
	{
		bool isInvalid = true;
		for (int i = 0; i < variableNames.size(); ++i)
		{
			if (condition == variableNames[i])
			{
				isInvalid = false;
				val = variableValues[i];
			}
		}
		if (isInvalid)
		{
			std::cerr << "Line " << linecount << ": " << condition << " variable not found.\nEnding process..." << std::endl;
			exit(1);
		}
	}
	while (line[j++] != '{') {}
	int jx = j;
	for (int i = 0; i < val; ++i)
	{
		j = jx;
		while (line[j] != '}')
		{
			std::string command;
			int k = 0;
			int ix = j;
			while (line[j] != ' ' && line[j] != '(')
			{
				++k;
				++j;
			}
			command = line.substr(j - k, k);
			bool invalidVar = true;
			switch (command[0])
			{
			case 'o':
				Output(line, ix, lineCount, variableNames, variableValues);
				break;
			default:
				for (int i = 0; i < variableCount; ++i)
				{
					if (command == variableNames[i])
					{
						VariableAssignment(line, i, variableValues, variableNames, command, lineCount);
						invalidVar = false;
						break;
					}
				}
				break;
			}
			bool cycleEnd = false;
			while (line[++j] != ',')
			{
				if (line[j] == '}')
				{
					cycleEnd = true;
					break;
				}
			}
			if (cycleEnd)
			{
				break;
			}
			while (isalpha(line[j++])) {}
			++j;
		}
	}
}
