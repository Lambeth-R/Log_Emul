#include <string>
#include "sysfunk.h"

int main()
{
	std::string test("ReadFile(19|||Engish, moetherfucker, do you speak it?|||39|||39|||NULL)");
	cReadFile RF(test);
	return 0;
}