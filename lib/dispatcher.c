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
  Task *r = empty.h; // Rec - эта запись окажется в хвосте delay/active

  if(r == NULL){ // нет свободных записей. Приплыли.
    _Exit(QT_QUEUE_OVERFLOW_CODE);
  }

  Task *oldTail = delay.t;
  r->ptr = ptr;

  head2tail(&empty, tick ? &delay : &active);

  if(0 == tick){
    return;
  }

  Task *p  = delay.h; // бегунок указывает на голову delay
  Task *pp = NULL;    // "хвостик" за бегунком
  while(p != r && tick > p->delay){ // p != r - бегунок НЕ достиг хвоста (наша новая запись сейчас в хвосте)
    tick -= p->delay;
    pp = p;
    p = p->next;
  } // нашли место для нашей записи (r) - между pp и p
  r->delay = tick;   // установить задержку
  // устанавливаем связь PREV -> REC
  if(NULL == pp){    // head r - новая голова очереди
    delay.h = r;
  }else{
    pp->next = r;
  }
  // устанавливаем связь REC -> NEXT
  if(p != r){ // новое место не в хвосте, где она сейчас, значит переносим
    p->delay -= tick;      // поправить задержку следующему
    delay.t = oldTail;     // восстанавливаем указатель на старый хвост (он и истается хвостом)
    delay.t->next = NULL;  // поправляем ему указатель, что он не ссылался на r
    r->next = p;           // и установить связь r -> next
  }
}

/**
 Выполняет одну задачу из очереди "активные задачи"
 */
void qtDispatch(void){
  if(active.h){
    head2tail(&active, &empty);
    empty.t->ptr();
  }
}

/**
 Выполняет одну задачу из очереди "активные задачи" и возращает подтверждениие выполнения
 @return bool
 */
bool qtDispatch2(void){
  if(active.h){
    head2tail(&active, &empty);
    (empty.t->ptr)();
    return true;
  }
  return false;
}

void qtDecrementDelay(void){
  if(delay.h && 0 == --delay.h->delay){
    while(delay.h && 0 == delay.h->delay){
      head2tail(&delay, &active);
    }
  }
}

/**
 Декремент бОльше, чем один тик.
 Передает на выполнение задачи, которым пришел срок.
 Возращает к-во тиков, которые остались.
 Одно выполнение функции активирует лишь одну пачку заданий - на ближайшее время.
 Основная программма может выполнять функцию в цикле, пока она не вернет 0 -
  это активирует все задачи, для которых пришло время.
 */
qtDelay qtDecrementDelay2(qtDelay tick){
  if(delay.h){
    qtDelay tmp = delay.h->delay;
    if(tick < tmp){ //мало тиков, время еще не пришло
      delay.h->delay -= tick;
    }else{
      head2tail(&delay, &active); // активируем первую задачу и все с этой пачки
      while(delay.h && 0 == delay.h->delay){
        head2tail(&delay, &active);
      }
      return tick - tmp; // "оставшиеся" тики
    }
  }
  return 0;
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
