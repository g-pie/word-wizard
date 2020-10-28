// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9
// PROJECT 1 - Letterman Reboot (Path Finding)
// RIP CHADWICK

#include "Dictionary.h"
int main(int argc, char* argv[]){
	ios_base::sync_with_stdio(false);
	Letterman L;
	L.getOptions(argc, argv);
	L.readDictionary();
	L.search();
	return 0;
}