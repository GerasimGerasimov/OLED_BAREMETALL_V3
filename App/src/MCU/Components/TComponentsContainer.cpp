#include "TComponentsContainer.h"

void TComponentsContainer::Add(TVisualObject* pVisualObject){//добавить объект в список
  List.push_back(pVisualObject);
}

void TComponentsContainer::AddList(std::vector <TVisualObject*> Source) {//добавить список объектов в список
    List.assign(Source.begin(), Source.end());
}

void  TComponentsContainer::Clear(void){//очистит список
  List.clear();
}

TComponentsContainer::TComponentsContainer(std::vector <TVisualObject*> Source) {//конструктор
  AddList(Source);
}

const u16 TComponentsContainer::ItemsCount() const {
    return List.size();
}

TComponentsContainer::~TComponentsContainer() {
};

std::vector<TVisualObject*> TComponentsContainer::getFocusedElements() {
	std::vector<TVisualObject*> res = {};
	return res;
};