#include <Magick++.h> 
#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std; 
using namespace Magick;

#define CIRCLE_RADIUS 700

map<string,string> gSettings;

const Color getColor(string s) {
        return Color(gSettings[s]);
}
const Color getColor(int i) {
        if(i == -1) return Color(0,0,0,QuantumRange);
        ostringstream oss;
        oss << i;
        return getColor(oss.str());
}
void placeIcons(Image& background, vector<Image>& icons) {
        if(icons.size() == 0)
                throw Exception("Void vector");
        const int center_x = background.columns() / 2, center_y = background.rows() / 2;
        const int num = icons.size();
        for(int i = 0; i < icons.size(); i++) {
                double x = 2.0 * M_PI / num * i;
                background.composite(icons[i],
                                     center_x + cos(x) * CIRCLE_RADIUS - icons[i].columns() / 2,
                                     center_y + sin(x) * CIRCLE_RADIUS - icons[i].rows()    / 2,
                                     OverCompositeOp);
        }
}
void createLines(Image& background, vector<vector<int>> relations) {
        const int center_x = background.columns() / 2, center_y = background.rows() / 2;
        const int num = relations.size();
        for(int i = 0; i < relations.size(); i++) {
                double c = 2.0 * M_PI / num * i;
                double x = center_x + cos(c) * 700,  y = center_y + sin(c) * 700;
                for(int j = 0; j < relations[i].size(); j++) {
                        double c2 = 2.0 * M_PI / num * j;
                        double x2 = center_x + cos(c2) * CIRCLE_RADIUS, y2 = center_y + sin(c2) * CIRCLE_RADIUS;
                        list<Drawable> lineSettings;
                        lineSettings.push_back(DrawableStrokeColor(getColor(relations[i][j])));
                        lineSettings.push_back(DrawableStrokeWidth(5));
                        lineSettings.push_back(DrawableFillColor(Color(0,0,0,QuantumRange)));
                        lineSettings.push_back(DrawableLine(x,y,x2,y2));
                        background.draw(lineSettings);
                }
        }
}
const vector<string> split(const string& s, const char& c) {
	string buff;
	vector<string> v;
	
	for(auto n:s) {
                if(n != c) buff+=n;
                else {
                        v.push_back(buff);
                        buff = "";
                }
        }
	v.push_back(buff);	
	return v;
}
bool readFile(string filename, vector<Image>& images, vector<vector<int>>& relations) {
        ifstream ifs (filename.c_str());
        if(!ifs) return false;

        string str;

        // IMAGES
        getline(ifs, str);
        vector<string> persons = split(str, '|');
        for(auto s : persons) {
                if(s.empty()) continue;
                Image img;
                try {
                        img.read(s + string(".png"));
                } catch(Exception &e) {
                        img.read(gSettings["default"]);
                }
                img.resize(Geometry(200,200));                
               images.push_back(img);
        }

        // RELATIONS
        for(int i = 0; getline(ifs,str); i++) {
                relations.push_back(vector<int>());
                vector<string> parts = split(str,'|');
                for(int j = 1; j <= i; j++) {
                        if(parts[j] == "") relations[i].push_back(-1);
                        else               relations[i].push_back(stoi(parts[j]));
                }
        }
        return true;
}
bool readSetting(string filename) {
        ifstream ifs (filename.c_str());
        if(!ifs) return false;

        string str;
        while(getline(ifs, str)) {
                vector<string> parts = split(str,' ');
                if(!parts.size() != 2 && !str.empty())
                        gSettings[parts[0]] = parts[1];
        }
        return true;
}
int main(int argc,char **argv)  { 
        InitializeMagick(*argv);

        try {
                vector<Image> images;
                vector<vector<int>> relations;

                string file;
                if(argc == 2) file = argv[1];
                else          file = "settings.txt";
                if(!readSetting(file) || !readFile(gSettings["data"], images, relations)) {
                        cout << "No file specified, not doing anything\n";
                        return 0;
                }
                
                Image image(Geometry(2000,2000), getColor("background"));
                createLines(image, relations);
                placeIcons(image, images);
                image.write(gSettings["output"]);
        }
        catch(Exception &e) { 
                cout << "Caught exception: " << e.what() << endl; 
                return 1; 
        } 
        return 0; 
}
