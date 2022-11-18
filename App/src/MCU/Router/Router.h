#ifndef T_ROUTER_H
#define T_ROUTER_H

#include <map>
#include "Pages.h"
#include "stm32f4xx.h"
#include "msg.h"
#include "signal.h"

typedef struct {
    std::string value;
    std::string tag;
} TPageValueEditEntryData;

typedef struct {
    bool isDone;
    std::string NextPage;
    void* props;
} TRouterTask ;


class TRouter {
public:
    static void ProcessMessage(TMessage* m);
    static void Init(void);
    TRouter();
    ~TRouter();
    static void setBackPage(std::string url);
    static TPage* goBack();
    static TPage* Page;
    static TPage* BackPage;
    static bool changeToNextPage();
    static bool chekNextPage();
    static void setTask(TRouterTask task);
    static TPageValueEditEntryData PageValueEditEntryData;
private:
    static TPage* setActivePage(std::string PageName, void* props);
    static void setInitPage();
    static TRouterTask RouterTask;
    static std::map<std::string, TPage*> Pages;
};

#endif