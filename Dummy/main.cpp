#include <string>
#include "../wxGUI/cOut.h"
#include "../common/common.h"

int main()
{
	std::string ts = "ReadFile(0000000000000468|||ok [.ShellClassInfo]LocalizedResourceName = @ % SystemRoot % \system32\shell32.dll, -21770 IconResource = % SystemRoot % \system32\imageres.dll, -112 \
		IconFile = % SystemRoot % \system32\shell32.dll \
		IconIndex = -235 \
		|||404|||402|||NULL)";
	msg m1 = {0,false,false,ts};
	std::list<msg> ml;
	ml.push_back(m1);
	cOut t("Sun_Aug_22_00_58_27_2021.dat");
	return 0;
}