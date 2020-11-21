#include <iostream>
#include <tuple>
#include "CImg/CImg.h"
#include <fstream>

using namespace cimg_library;
using namespace std;
 
struct Node {
    short x1, y1, x2, y2;
    unsigned char color;
    bool leaf;
    Node* children[4];
    Node(){};
    Node(short x1, short y1, short x2, short y2, unsigned char color): leaf{false}, x1{x1}, x2{x2}, y1{y1}, y2{y2}, color{color} {
        memset(children,0,sizeof(Node*)*4);
    }
    void write(ofstream & file){
        file.write((char*)&x1, sizeof(short));
        file.write((char*)&x2, sizeof(short));
        file.write((char*)&y1, sizeof(short));
        file.write((char*)&y2, sizeof(short));
        file.write((char*)&color, sizeof(unsigned char));
        //file.write((char*)this, sizeof(Node));
    }
    void read(ifstream &file){
        file.read((char*)&x1, sizeof(short));
        file.read((char*)&x2, sizeof(short));
        file.read((char*)&y1, sizeof(short));
        file.read((char*)&y2, sizeof(short));
        file.read((char*)&color, sizeof(unsigned char));
    }
};
 
CImg<unsigned char>  Binarizar(CImg<float> & img, int umbral) {
    CImg<unsigned char> R(img.width(),img.height());
    for(int i=0;i< img.width();i++)
        for(int j=0;j< img.height();j++) {
            int r = img(i,j,0);
            int g = img(i,j,1);
            int b = img(i,j,2);
            if ( (r+g+b)/3  > umbral)
                R(i,j) = 255;
            else
                R(i,j) = 0;
        }
    return R;
}
 

 
class RectangleQuadtree {
private:
    Node *root;
    CImg<unsigned char> NR;
    CImg<unsigned char> R;
public:
 
    RectangleQuadtree(const char* str){
        CImg<float> A(str);
        R = Binarizar(A,85);
        R.display();
        NR = CImg<unsigned char> (R.width(),R.height(),1,1,255);
        execute();
        save(root);
        regenerar();
        NR.display();
    };
    void divide(int x1, int y1, int x2, int y2, Node*& cur) {
        int x3{(x1 + x2) / 2}, y3{(y1 + y2) / 2};
        cur->children[0] = new Node(x3, y1, x2, y3, R(x3,y1));
        cur->children[1] = new Node(x1, y1, x3, y3, R(x1,y1));
        cur->children[2] = new Node(x1, y3, x3, y2, R(x1,y3));
        cur->children[3] = new Node(x3, y3, x2, y2, R(x3,y3));
    }
 
    bool color_unico(int x1, int y1, int x2, int y2) {
       /* if(x2-x1 == 1 && y2-y1 == 1)
            return true;*/
        unsigned char x = R(x1,y1);

        for(int i=x1; i< x2; i++){
            for(int j=y1; j<y2; j++) {
                if(R(i,j) != x){
                    //cout << "R(i,j,0): " << R(i,j) << " x: " << x << endl;
                    return false;
                }
            }
        }
        return true;
    }
 
    void execute(){
        root = new Node(0,0,R.width(),R.height(), R(0,0));
        execute(root);
    }

    void execute(Node*&cur){
        if(color_unico(cur->x1,cur->y1,cur->x2,cur->y2)){
            cur->leaf = true;
            return;
        }else{
            divide(cur->x1,cur->y1,cur->x2,cur->y2,cur);
            for(int i = 0; i < 4; i++)
                execute(cur->children[i]);
            
        }
    }

    void regenerar(){
        ifstream file("newimg.bin",ios::binary);
        Node p;
        while(!file.eof()){
            p.read(file);
            for(int i = p.x1; i < p.x2; i++){
                for(int j = p.y1; j < p.y2; j++){
                    NR(i,j) = p.color;
                }
            }
        }

    }
    void save(Node*&cur){
        ofstream file("newimg.bin", ios::binary);
        SaveDfs(root, file);
        file.close();
    }
    void SaveDfs(Node*&cur, ofstream& file){
        if(cur->leaf){
            if(!cur->color)
                cur->write(file);
        }
        else{
            for(int i = 0; i < 4; i++){
                SaveDfs(cur->children[i],file);
            }
        }
    }

};

int main() {
    RectangleQuadtree A("../carta.jpg");
    
   // CImg<float> A("protesta.jpg");
   // CImg<unsigned char> R = Binarizar(A,40);
    
   // A.display();
   // R.display();
    return 0;
}