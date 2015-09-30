#if !defined(CDRLIB_QUEUE_H)
#define CDRLIB_QUEUE_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

struct QUE {
  QUE *next;
  QUE *prior;
  };

#define ABSQ_INIT(qhd) (qhd)->next = qhd; (qhd)->prior = qhd;
#define ABSQ_EMPTY(qhd) ((qhd)->next == qhd)

#define ABSQ_FIRST(qhd, type, fld)                                    \
  (type *)((UBYTE *)(qhd)->next - offsetof(type, fld));

#define ABSQ_LAST(qhd, type, fld)                                     \
  (type *)((UBYTE *)(qhd)->prior - offsetof(type, fld));

#define ABSQ_LOOP(qhd, entry, type, fld)                              \
  {                                                                   \
  QUE *loc_qhdP = (qhd);                                              \
  QUE *loc_queP = loc_qhdP->next;                                     \
                                                                      \
  while (loc_queP != loc_qhdP)                                        \
    {                                                                 \
    type *entry = (type *)((UBYTE *)loc_queP - offsetof(type, fld));  \
    loc_queP = loc_queP->next;

#define ABSQ_POOL } }

#define ABSQ_INSERT_HEAD(qhd, entry, type, fld)                       \
  {                                                                   \
  QUE *blinkP = (qhd);                                                \
  QUE *flinkP = blinkP->next;                                         \
                                                                      \
  QUE *queP = (QUE *)((UBYTE *)(entry) + offsetof(type, fld));        \
                                                                      \
  queP->next = flinkP;                                                \
  queP->prior = blinkP;                                               \
                                                                      \
  blinkP->next = queP;                                                \
  flinkP->prior = queP;                                               \
  }

#define ABSQ_INSERT_TAIL(qhd, entry, type, fld)                       \
  {                                                                   \
  QUE *blinkP = (qhd)->prior;                                         \
  QUE *flinkP = blinkP->next;                                         \
                                                                      \
  QUE *queP = (QUE *)((UBYTE *)(entry) + offsetof(type, fld));        \
                                                                      \
  queP->next = flinkP;                                                \
  queP->prior = blinkP;                                               \
                                                                      \
  blinkP->next = queP;                                                \
  flinkP->prior = queP;                                               \
  }

#define ABSQ_INSERT_NEXT(cur_ent, next_ent, type, fld)                  \
  {                                                                     \
  QUE *queP = (QUE *)((UBYTE *)(cur_ent) + offsetof(type, fld));        \
  QUE *nextqueP = (QUE *)((UBYTE *)(next_ent) + offsetof(type, fld));   \
                                                                        \
  QUE *blinkP = queP;                                                   \
  QUE *flinkP = blinkP->next;                                           \
                                                                        \
  nextqueP->next = flinkP;                                              \
  nextqueP->prior = blinkP;                                             \
                                                                        \
  blinkP->next = nextqueP;                                              \
  flinkP->prior = nextqueP;                                             \
  }

#define ABSQ_INSERT_PRIOR(cur_ent, prior_ent, type, fld)                \
  {                                                                     \
  QUE *queP = (QUE *)((UBYTE *)(cur_ent) + offsetof(type, fld));        \
  QUE *priorqueP = (QUE *)((UBYTE *)(prior_ent) + offsetof(type, fld)); \
                                                                        \
  QUE *blinkP = queP->prior;                                            \
  QUE *flinkP = queP;                                                   \
                                                                        \
  priorqueP->next = flinkP;                                             \
  priorqueP->prior = blinkP;                                            \
                                                                        \
  blinkP->next = priorqueP;                                             \
  flinkP->prior = priorqueP;                                            \
  }

#define ABSQ_REMOVE(entry, type, fld)                                   \
  {                                                                     \
  QUE *queP = (QUE *)((UBYTE *)(entry) + offsetof(type, fld));          \
                                                                        \
  QUE *blinkP = queP->prior;                                            \
  QUE *flinkP = queP->next;                                             \
                                                                        \
  blinkP->next = flinkP;                                                \
  flinkP->prior = blinkP;                                               \
  }

// Prototypes.

int ABSQ_COUNT(QUE *qhdP);

// Restore previous packing alignment.

#pragma pack(pop)

#endif
