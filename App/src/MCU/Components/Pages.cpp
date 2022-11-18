#include "Pages.h"
void TPage::view() {
}

void TPage::clear() {

}

bool TPage::ProcessMessage(TMessage* m) {//обработчик сообщений
	return true;
}

void TPage::setProps(void* _props) {
	props = _props;
	onOpen();
}

void TPage::onOpen() {

}

TPage::TPage(std::string name)
	:TComponentsContainer()
	,isOpen(false)
	,Name(name)
	,BackPage("")
	,props(NULL){
}

TPage::TPage(std::string name, bool isOpenState, std::vector <TVisualObject*> Source)
	: TComponentsContainer(Source)
	, isOpen(isOpenState)
	, Name(name)
	, BackPage("")
	, props(NULL){
}

TPage::~TPage() {};//деструктор

void TPage::startToClose() {

}

bool TPage::isClosed() {
	return true;
}