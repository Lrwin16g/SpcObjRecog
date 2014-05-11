#include <QApplication>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "spcobjrecog.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
	std::cerr << "Usage: " << argv[0] << " <filelist> " << std::endl;
	return -1;
    }
    
    QApplication app(argc, argv);
    
    std::ifstream ifs(argv[1]);
    std::vector<std::string> filepath;
    std::string line;
    while (ifs >> line) {
	filepath.push_back(line);
    }
    
    ifs.close();
    
    SpcObjRecog spcwin;
    spcwin.show();
    
    spcwin.loadDataBase(filepath);
    
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    
    return app.exec();
}
