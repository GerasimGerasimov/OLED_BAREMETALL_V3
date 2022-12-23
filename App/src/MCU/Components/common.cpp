#include "common.h"
//бaзовый класс визуальных объектов
TVisualObject::TVisualObject()
	: inFocus(false)
	, isSkipped(false)
	, ElementRect({0,0,0,0})
	, DataSrc(nullptr) {
};

TVisualObject::TVisualObject(TVisualObjectInitStructure props)
  : inFocus(props.focused)
  , isSkipped(false)
  , ElementRect(props.Rect)
  , DataSrc(nullptr) {

}

const std::string TVisualObject::ComponentName() {
	return "TVisualObject";
}

TVisualObject::~TVisualObject() {};

TVisualObject* TVisualObject::getFocusedElement() {
	return nullptr;
};

void* TVisualObject::getDataSrc() {
	return DataSrc;
}