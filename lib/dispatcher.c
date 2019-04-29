#include "dispatcher.h"

typedef struct s_task {
  struct s_task *next;
  qtDelay   delay;
  qtTaskPtr ptr;
} Task;

typedef struct {
  struct s_task *h;
  struct s_task *t;
} Queue;

static Task task[QT_TASK_COUNT];

static Queue empty, active, delay;

static void head2tail(Queue *q1, Queue *q2);

void qtInit(void){
  Task *s = empty.h = task;
  for(uint8_t i = 0; i < QT_TASK_COUNT - 1; i++){
    s = s->next = s + 1;
  };
  empty.t  = s;
  task[QT_TASK_COUNT - 1].next = NULL;
  active.h = NULL;
  active.t = NULL;
  delay.h  = NULL;
  delay.t  = NULL;
}

void qtTask(qtTaskPtr ptr, qtDelay tick){
  Task *r = empty.h;

  if(r == NULL){ // нет свободных записей. Приплыли.
    _Exit(QT_QUEUE_OVERFLOW_CODE);
  }

  Task *oldTail = delay.t;
  r->ptr = ptr;

  head2tail(&empty, tick ? &delay : &active);

  if(0 == tick){
    return;
  }

  Task *p  = delay.h; // бегунок
  Task *pp = NULL;    // "хвостик" за бегунком
  while(p != r && tick > p->delay){
    tick -= p->delay;
    pp = p;
    p = p->next;
  }
  r->delay = tick;   // установить задержку
  if(NULL == pp){    // head
    delay.h = r;
  }else{
    pp->next = r;
  }
  if(p != r){ // не хвост
    p->delay -= tick;      // поправить задержку следующему
    delay.t = oldTail;     // оторвать от хвоста
    delay.t->next = NULL;
    r->next = p;           // и вставить внутрь
  }
}

void qtDispatch(void){
  if(active.h){
    head2tail(&active, &empty);
    (empty.t->ptr)();
  };
}

void qtDecrementDelay(void){
  if(delay.h && 0 == --delay.h->delay){
    while(delay.h && 0 == delay.h->delay){
      head2tail(&delay, &active);
    }
  }
}

static void head2tail(Queue *q1, Queue *q2){
  if(q2->t == NULL){
    q2->h = q1->h;
  }else{
    q2->t->next = q1->h;
  }
  q2->t = q1->h;
  q1->h = q1->h->next;
  if(q1->h == NULL){
    q1->t = NULL;
  }else{
    q2->t->next = NULL;
  }
}
