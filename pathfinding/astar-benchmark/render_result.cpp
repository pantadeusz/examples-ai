#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <img_img8.hpp>

using namespace std;

int main(int argc, char **argv) {
	vector<string> args(argv, argv+argc);
	if (args.size() <= 1) {
		cout << args[0] << " image.png points.txt" << endl;
		return -1;
	}
	tp::img::Img8 img(args[1]);
	//img.load(args[1]);
	ifstream points_file(args[2]);
	
	while(!points_file.eof()) {
		int x, y;
		points_file >> x >> y;
		img(x,y) = 128;
	}
	points_file.close();
	img.save(args[2]+".png");
	return 0;
}
