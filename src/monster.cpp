#include "monster.h"
#include "main.h"
using namespace std;	
Monster::Monster(color_t color) {

    for(int i=0;i<num_monsters;i++)
        this->prism.push_back(Prism(rand()%400 - 200 ,rand()%200 -200, color));

}

void Monster::draw(glm::mat4 VP) {

	vector <Prism> :: iterator it;
    for(it = this->prism.begin(); it != this->prism.end(); it++)
    	it->draw(VP);

}

void Monster::tick(float x, float z){
	
	vector <Prism> :: iterator it;
    for(it = this->prism.begin(); it != this->prism.end(); it++)
    	it->tick(x,z);
}

void Monster::kill(vector <Prism> :: iterator ind){
	this->prism.erase(ind);
}
