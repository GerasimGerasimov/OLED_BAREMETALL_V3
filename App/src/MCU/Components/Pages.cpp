#include "Pages.h"
void TPage::view() {
}

void TPage::clear() {

}

bool TPage::ProcessMessage(TMessage* m) {//обработчик сообщений
	return false;
}

void TPage::setProps(void* _props) {
	props = _props;
	isOpen = true;
	onOpen();
}

void TPage::onOpen() {
	
}

void TPage::onClose() {
	
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

void TPage::startToClose() {//Если метод переписывается потомком, то он должен вызвать родительский startToClose()
	                        //или в своём startToClose() сделать isOpen = false после завершения процедуры закрытия;
	onClose();
	isOpen = false;
}

bool TPage::isClosed() {
	return (bool)(isOpen == false);
}